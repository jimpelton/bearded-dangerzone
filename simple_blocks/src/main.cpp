//#include <GL/glew.h>
//#include <GLFW/glfw3.h>

// local includes
#include "cmdline.h"
#include "create_vao.h"
#include "axis_enum.h"
#include "timing.h"
#include "blockrenderer.h"
#include "constants.h"
#include "colormap.h"
#include "renderhelp.h"
#include "controls.h"
#include "controlpanel.h"
#include "blockcollection.h"

// BD lib
#include <bd/graphics/shader.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/log/logger.h>
#include <bd/io/indexfile.h>


#include <QApplication>

// STL and STD lib
#include <string>
#include <fstream>
#include <iostream>
#include <ostream>
#include <future>


#include <memory>


// profiling
#include "nvpm.h"


//std::shared_ptr<bd::CoordinateAxis> g_axis{ nullptr }; ///< The coordinate axis lines.
std::shared_ptr<subvol::BlockRenderer> g_renderer{ nullptr };
std::shared_ptr<bd::ShaderProgram> g_wireframeShader{ nullptr };
std::shared_ptr<bd::ShaderProgram> g_volumeShader{ nullptr };
std::shared_ptr<bd::ShaderProgram> g_volumeShaderLighting{ nullptr };
std::shared_ptr<bd::VertexArrayObject> g_axisVao{ nullptr };
std::shared_ptr<bd::VertexArrayObject> g_boxVao{ nullptr };
std::shared_ptr<bd::VertexArrayObject> g_quadVao{ nullptr };
std::shared_ptr<subvol::BlockCollection> g_blockCollection{ nullptr };
std::shared_ptr<bd::IndexFile> g_indexFile{ nullptr };
//std::shared_ptr<subvol::Controls> g_controls{ nullptr };
bool g_renderInitComplete{ false };

double g_rovMin, g_rovMax;

void cleanup();
void printBlocks(subvol::BlockCollection *bcol);
void printNvPmApiCounters(const char *perfOutPath);


using subvol::ColorMapManager;
using subvol::CommandLineOptions;

namespace subvol
{

/////////////////////////////////////////////////////////////////////////////////
void
cleanup()
{
  glfwTerminate();
}


/////////////////////////////////////////////////////////////////////////////////
void
printBlocks(subvol::BlockCollection *bcol)
{
  std::ofstream block_file("blocks.txt", std::ofstream::trunc);

  if (block_file.is_open()) {
    bd::Info() << "Writing blocks to blocks.txt in the current working directory.";
    for (auto &b : bcol->blocks()) {
      block_file << b << "\n";
    }
    block_file.flush();
    block_file.close();
  } else {
    bd::Err() << "Could not print blocks because blocks.txt couldn't be created "
        "in the current working directory.";
  }
}


/////////////////////////////////////////////////////////////////////////////////
//std::shared_ptr<bd::IndexFile>
//openIndexFile(subvol::CommandLineOptions const &clo)
//{
//  // Read the index file to get values from it that we need to populate
//  // the CommandLineOptions struct.
//  std::shared_ptr<bd::IndexFile> indexFile{
//      bd::IndexFile::fromBinaryIndexFile(clo.indexFilePath) };
//  if (indexFile == nullptr) {
//    bd::Err() << "Could open the index file.";
//  }
//
//  return indexFile;
//}


/////////////////////////////////////////////////////////////////////////////////
void
updateCommandLineOptionsFromIndexFile(subvol::CommandLineOptions &clo,
                                      std::shared_ptr<bd::IndexFile> const &indexFile)
{

  // Since the IndexFileHeader contains most of the options needed to
  // render the volume, we copy those over into the CommandLineOptions struct.
  // Without an index file these options are provided via argv anyway.

  // Clamp clo.blockThreshold_Min/Max to the min/max values of
  // the blocks in indexfile.
  double min{ std::numeric_limits<double>::max() };
  double max{ std::numeric_limits<double>::lowest() };


  auto minmaxE =
      std::minmax_element(indexFile->getFileBlocks().begin(),
                          indexFile->getFileBlocks().end(),
                          [](bd::FileBlock const & lhs, bd::FileBlock const & rhs)
                              -> bool {
                            return lhs.rov < rhs.rov;
                          } );

  g_rovMin = min = (*minmaxE.first).rov;
  g_rovMax = max = (*minmaxE.second).rov;

  // Clamp the given block thresholds to the actual.
  if (clo.blockThreshold_Min < min) {
    clo.blockThreshold_Min = min;
  }
  if (clo.blockThreshold_Max > max) {
    clo.blockThreshold_Max = max;
  }

  clo.vol_w = indexFile->getVolume().voxelDims().x;
  clo.vol_h = indexFile->getVolume().voxelDims().y;
  clo.vol_d = indexFile->getVolume().voxelDims().z;
  clo.numblk_x = indexFile->getVolume().block_count().x;
  clo.numblk_y = indexFile->getVolume().block_count().y;
  clo.numblk_z = indexFile->getVolume().block_count().z;
  clo.dataType = bd::to_string(bd::IndexFileHeader::getType(indexFile->getHeader()));

}


/////////////////////////////////////////////////////////////////////////////////
void
setRendererInitialTransferFunction(bool loaded, std::string const &name,
                                   subvol::BlockRenderer &renderer)
{

  if (loaded) {

    // The color map was loaded so, give it to the renderer.
    renderer.setColorMapTexture(
        ColorMapManager::getMapByName(name).getTexture());

  } else {

    renderer.setColorMapTexture(
        ColorMapManager::getMapByName(
            ColorMapManager::getCurrentMapName()).getTexture());

  }

}


/////////////////////////////////////////////////////////////////////////////////
bool
initializeTransferFunctions(subvol::CommandLineOptions const &clo)
{

  ColorMapManager::generateDefaultTransferFunctionTextures();

  // if user transfer function was loaded.
  bool loaded{ false };
  try {

    // if at least one of color and opacity files are given... load those tfuncs
    if ( !clo.colorTFuncPath.empty() || !clo.opacityTFuncPath.empty()) {
      loaded = ColorMapManager::loadColorMap("USER", clo.colorTFuncPath,
                                             clo.opacityTFuncPath);
    } else if (!clo.tfunc1dtPath.empty()) {
      loaded = ColorMapManager::load1DT("USER", clo.tfunc1dtPath);
    }

  } catch (std::ios_base::failure &e) {
    bd::Warn() << "Error reading user defined transfer function file(s). "
        "The function won't be available.";
  }


  return loaded;
}


/////////////////////////////////////////////////////////////////////////////////
void
initializeVertexBuffers(subvol::CommandLineOptions const &clo)
{

  // 2d slices
  g_quadVao = std::make_shared<bd::VertexArrayObject>();
  g_quadVao->create();
  //TODO: generate quads shaped to the actual volume dimensions.
  bd::Dbg() << "Generating proxy geometry VAO";
  
  subvol::genQuadVao(*g_quadVao,
                     { -0.5f, -0.5f, -0.5f },
                     { 0.5f, 0.5f, 0.5f },
                     { clo.num_slices, clo.num_slices, clo.num_slices });


  // coordinate axis
  bd::Dbg() << "Generating coordinate axis VAO";
  g_axisVao = std::make_shared<bd::VertexArrayObject>();
  g_axisVao->create();
  subvol::genAxisVao(*g_axisVao);


  // bounding boxes
  bd::Dbg() << "Generating bounding box VAO";
  g_boxVao = std::make_shared<bd::VertexArrayObject>();
  g_boxVao->create();
  subvol::genBoxVao(*g_boxVao);
}


int
initializeShaders(subvol::CommandLineOptions &clo)
{
  int rval = 0b111;


  // Wireframe Shader
  g_wireframeShader = std::make_shared<bd::ShaderProgram>();
  GLuint programId{
      g_wireframeShader->linkProgram("shaders/vert_vertexcolor_passthrough.glsl",
                                     "shaders/frag_vertcolor.glsl") };
  if (programId == 0) {
    bd::Err() << "Error building passthrough shader, program id was 0.";
    rval &= 0b110;
  }
  g_wireframeShader->unbind();


  // Volume shader
  g_volumeShader = std::make_shared<bd::ShaderProgram>();
  programId =
      g_volumeShader->linkProgram("shaders/vert_vertexcolor_passthrough.glsl",
                                  "shaders/frag_volumesampler_noshading.glsl");
  if (programId == 0) {
    bd::Err() << "Error building volume shader, program id was 0.";
    rval &= 0b101;
  }
  g_volumeShader->unbind();


  // Volume shader with Lighting
  g_volumeShaderLighting = std::make_shared<bd::ShaderProgram>();
  programId =
      g_volumeShaderLighting->linkProgram("shaders/vert_vertexcolor_passthrough.glsl",
                                          "shaders/frag_shading_otfgrads.glsl");
  if (programId == 0) {
    bd::Err() << "Error building volume lighting shader, program id was 0.";
    rval &= 0b011;
  }
  g_volumeShaderLighting->unbind();


  return rval;
}



/////////////////////////////////////////////////////////////////////////////////
GLFWwindow *
init_subvol(subvol::CommandLineOptions &clo)
{

  // Open the index file if possible, then setup the BlockCollection
  // and give away ownership of the index file to the BlockCollection.
  std::shared_ptr<bd::IndexFile> indexFile{ std::make_shared<bd::IndexFile>() };
  if (! clo.indexFilePath.empty()) {
    bool ok = false;
    indexFile = std::move(
        bd::IndexFile::fromBinaryIndexFile(clo.indexFilePath, ok));
    if (! ok) {
      return nullptr;
    }
    updateCommandLineOptionsFromIndexFile(clo, indexFile);
    g_indexFile = indexFile;
  } //else {
//    bd::Err() << "No IndexFile provided.";
//    return nullptr;
//  }

  subvol::printThem(clo);


  // Initialize OpenGL and GLFW and generate our transfer function textures.
  GLFWwindow *window{ nullptr };
  window = subvol::renderhelp::initGLContext(clo.windowWidth, clo.windowHeight);
  if (window == nullptr) {
    bd::Err() << "Could not initialize GLFW, exiting.";
    return nullptr;
  }
  bd::Info() << "Open GL initialized.";
  
  int64_t totalMemory{ 0 };
  subvol::renderhelp::queryGPUMemory(&totalMemory);
  bd::Info() << "GPU memory: " << (totalMemory * 1e-6) << "MB";

  if (clo.gpuMemoryBytes > totalMemory) {
    bd::Warn() << "Requested m_gpu memory, " << clo.gpuMemoryBytes
               << " greater than actual GPU memory, using " << totalMemory << " bytes.";
    clo.gpuMemoryBytes = static_cast<size_t>(totalMemory);
  }

  subvol::renderhelp::setInitialGLState();
  subvol::initializeVertexBuffers(clo);
  subvol::initializeShaders(clo);
  bool loaded = subvol::initializeTransferFunctions(clo);

  BlockCollection *bc{ nullptr };
  if (! renderhelp::initializeBlockCollection(&bc, indexFile.get(), clo)) {
    bd::Err() << "Error initializing block collection.";
    return nullptr;
  }
  g_blockCollection = std::shared_ptr<BlockCollection>(bc);
  
  g_renderer =
      std::make_shared<subvol::BlockRenderer>(int(clo.num_slices),
                                              g_volumeShader,
                                              g_volumeShaderLighting,
                                              g_wireframeShader,
                                              bc,
                                              g_quadVao,
                                              g_boxVao,
                                              g_axisVao);

  setRendererInitialTransferFunction(loaded, "USER", *g_renderer);

  g_renderer->resize(clo.windowWidth, clo.windowHeight);
  g_renderer->getCamera().setEye({ 0, 0, 4 });
  g_renderer->getCamera().setLookAt({ 0, 0, 0 });
  g_renderer->getCamera().setUp({ 0, 1, 0 });
  g_renderer->setViewMatrix(g_renderer->getCamera().createViewMatrix());
  g_renderer->setROVRange(clo.blockThreshold_Min, clo.blockThreshold_Max);
  g_renderer->setDrawNonEmptySlices(true);
  g_renderer->setDrawNonEmptyBoundingBoxes(false);


//  setCameraPosPreset(clo.cameraPos);

  //// NV Perf Thing ////
  perf_initNvPm();
  perf_initMode(clo.perfMode);

  subvol::renderhelp::initializeControls(window, g_renderer);
  g_renderInitComplete = true;

  return window;

} // init_subvol

} // namespace subvol


/////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
  
  TCLAP::CmdLine cmd("Simple Blocks blocking volume render experiment.", ' ');
  subvol::CommandLineOptions clo;
  if (subvol::parseThem(argc, argv, cmd, clo) == 0) {
    std::cout << "No arguments provided.\nPlease use -h for usage info."
              << std::endl;
//    return 1;
  }


//  if (clo.indexFilePath.empty()) {
//    bd::Err() << "Provide an index file path.";
//    return 1;
//  }


  GLFWwindow * window{ subvol::init_subvol(clo) };
  if (window == nullptr) {
    bd::Err() << "Could not initialize GLFW (window could not be created). Exiting...";
    return 1;
  }

  std::condition_variable_any wait;
  std::mutex m;


  // Start the qt event stuff on a separate thread.
  std::future<int> returned =
      std::async(std::launch::async,
                 [&]() {
                   QApplication a(argc, argv);
                   m.lock();
                   subvol::ControlPanel panel(g_renderer.get(),
                                              g_blockCollection.get(),
                                              g_indexFile);

                   panel.setGlobalRovMinMax(g_rovMin, g_rovMax);
                   panel.setMinMax(0,0);
                   panel.show();
                   m.unlock();
                   wait.notify_all();

                   return a.exec();
                 });
  wait.wait(m);
  subvol::renderhelp::loop(window, g_renderer.get());
  std::cout << "Waiting for GUI to close..." << std::endl;
  returned.wait();
  std::cout << "subvol exiting: " << returned.get() << std::endl;

//  printNvPmApiCounters(clo.perfOutPath.c_str());
  subvol::cleanup();

  return 0;
}
