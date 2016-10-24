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

// BD lib
#include <bd/graphics/shader.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/log/logger.h>
#include <bd/volume/blockcollection.h>
#include <bd/io/indexfile.h>


#include <QApplication>

// STL and STD lib
#include <string>
#include <fstream>
#include <iostream>
#include <ostream>

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
bd::BlockCollection *g_blockCollection{ nullptr };
//std::shared_ptr<subvol::Controls> g_controls{ nullptr };
bool renderInitComplete{ false };

double g_rovMin, g_rovMax;

void cleanup();
void printBlocks(bd::BlockCollection *bcol);
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
printBlocks(bd::BlockCollection *bcol)
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
std::shared_ptr<bd::IndexFile>
openIndexFile(subvol::CommandLineOptions const &clo)
{
  // Read the index file to get values from it that we need to populate
  // the CommandLineOptions struct.
  std::shared_ptr<bd::IndexFile> indexFile{
      bd::IndexFile::fromBinaryIndexFile(clo.indexFilePath) };
  if (indexFile == nullptr) {
    bd::Err() << "Could open the index file.";
  }

  return indexFile;
}


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
  double min{ std::numeric_limits<double>::lowest() };
  double max{ std::numeric_limits<double>::lowest() };


  auto minmaxE = std::minmax_element(indexFile->getBlocks().begin(),
                        indexFile->getBlocks().end(),
                        [](bd::FileBlock const & lhs, bd::FileBlock const & rhs) -> bool {
                          return lhs.rov < rhs.rov;
                        } );

  g_rovMin = min = (*minmaxE.first).rov;
  g_rovMax = max = (*minmaxE.second).rov;

  if (clo.blockThreshold_Min < min) {
    clo.blockThreshold_Min = min;
  }
  if (clo.blockThreshold_Max > max) {
    clo.blockThreshold_Max = max;
  }

  clo.vol_w = indexFile->getHeader().volume_extent[0];
  clo.vol_h = indexFile->getHeader().volume_extent[1];
  clo.vol_d = indexFile->getHeader().volume_extent[2];
  clo.numblk_x = indexFile->getHeader().numblocks[0];
  clo.numblk_y = indexFile->getHeader().numblocks[1];
  clo.numblk_z = indexFile->getHeader().numblocks[2];
  clo.dataType = bd::to_string(bd::IndexFileHeader::getType(indexFile->getHeader()));

}


/////////////////////////////////////////////////////////////////////////////////
void
checkColorMapLoaded(bool loaded, std::string const &name,
                    subvol::BlockRenderer &renderer)
{

  if (loaded) {


    // The 1dt color map was loaded so, give it to the renderer.
    renderer.setColorMapTexture(
        ColorMapManager::getMapByName(name).getTexture());


  } else {

    bd::Err() << "Transfer function was malformed. The function won't be available.";
    renderer.setColorMapTexture(
        ColorMapManager::getMapByName(
            ColorMapManager::getCurrentMapName()).getTexture());


  }

}


/////////////////////////////////////////////////////////////////////////////////
bool
initializeTransferFunctions(subvol::CommandLineOptions const &clo,
                            subvol::BlockRenderer &renderer)
{

  ColorMapManager::generateDefaultTransferFunctionTextures();

  bool loaded{ false };
  try {

    // if both color and opacity files are given... load those tfuncs
    if (!( clo.colorTFuncPath.empty() || clo.opacityTFuncPath.empty())) {

      loaded = ColorMapManager::loadColorMap("USER",
                                             clo.colorTFuncPath,
                                             clo.opacityTFuncPath);

    } else if (!clo.tfuncPath.empty()) {

      loaded = ColorMapManager::load1DT("USER", clo.tfuncPath);

    }
  }
  catch (std::ifstream::failure e) {
    bd::Err() << "Error reading user defined transfer function file(s). "
        "The function won't be available.";
  }
  checkColorMapLoaded(loaded, "USER", renderer);
  return loaded;
}


/////////////////////////////////////////////////////////////////////////////////
int
run_subvol(subvol::CommandLineOptions &clo)
{
  bd::BlockCollection *blockCollection{ new bd::BlockCollection() };
//  g_blockCollection = blockCollection;

  // Open the index file if possible, then setup the BlockCollection
  // and give away ownership of the index file to the BlockCollection.
  {
    std::shared_ptr<bd::IndexFile> indexFile{ openIndexFile(clo) };
    if (indexFile == nullptr) {
      bd::Err() << "Couldn't open provided index file path: " << clo.indexFilePath;
      return 1;
    }
    updateCommandLineOptionsFromIndexFile(clo, indexFile);

    blockCollection->initBlocksFromIndexFile(std::move(indexFile));
    // filter blocks in the index file that are within ROV thresholds
    blockCollection->filterBlocks(
        [&clo](bd::Block const &b) {
          return b.fileBlock().rov >= clo.blockThreshold_Min &&
              b.fileBlock().rov <= clo.blockThreshold_Max;
        });
  }


  if (blockCollection->nonEmptyBlocks().size() == 0) {
    bd::Info() << "All blocks where filtered out... exiting";
    return 1;
  }
  bd::Info() << blockCollection->nonEmptyBlocks().size()
             << " non-empty blocks in index file.";

  subvol::printThem(clo);


  // Initialize OpenGL and GLFW and generate our transfer function textures.
  GLFWwindow *window{ nullptr };
  window = subvol::renderhelp::initGLContext(clo.windowWidth, clo.windowHeight);
  if (window == nullptr) {
    bd::Err() << "Could not initialize GLFW, exiting.";
    return 1;
  }
  subvol::renderhelp::setInitialGLState();



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


  // Wireframe Shader
  g_wireframeShader = std::make_shared<bd::ShaderProgram>();
  GLuint programId{
      g_wireframeShader->linkProgram("shaders/vert_vertexcolor_passthrough.glsl",
                                     "shaders/frag_vertcolor.glsl") };
  if (programId == 0) {
    bd::Err() << "Error building passthrough shader, program id was 0.";
    return 1;
  }


  // Volume shader
  g_volumeShader = std::make_shared<bd::ShaderProgram>();
  programId =
      g_volumeShader->linkProgram("shaders/vert_vertexcolor_passthrough.glsl",
                                  "shaders/frag_volumesampler_noshading.glsl");
  if (programId == 0) {
    bd::Err() << "Error building volume shader, program id was 0.";
    return 1;
  }
  g_volumeShader->unbind();


  // Volume shader with Lighting
  g_volumeShaderLighting = std::make_shared<bd::ShaderProgram>();
  programId =
      g_volumeShaderLighting->linkProgram("shaders/vert_vertexcolor_passthrough.glsl",
                                          "shaders/frag_shading_otfgrads.glsl");
  if (programId == 0) {
    bd::Err() << "Error building volume lighting shader, program id was 0.";
    return 1;
  }
  g_volumeShaderLighting->unbind();

  g_renderer =
      std::make_shared<subvol::BlockRenderer>(int(clo.num_slices),
                                              g_volumeShader,
                                              g_volumeShaderLighting,
                                              g_wireframeShader,
                                              &blockCollection->nonEmptyBlocks(),
                                              g_quadVao, g_boxVao, g_axisVao);
  g_renderer->resize(clo.windowWidth, clo.windowHeight);
  g_renderer->getCamera().setEye({ 0, 0, 2 });
  g_renderer->getCamera().setLookAt({ 0, 0, 0 });
  g_renderer->getCamera().setUp({ 0, 1, 0 });
  g_renderer->setViewMatrix(g_renderer->getCamera().createViewMatrix());
  g_renderer->setROVRange(clo.blockThreshold_Min, clo.blockThreshold_Max);

  blockCollection->initBlockTextures(clo.rawFilePath);
  initializeTransferFunctions(clo, *g_renderer);
  g_renderer->setColorMapTexture(
      ColorMapManager::getMapByName(
          ColorMapManager::getCurrentMapName()).getTexture());

//  setCameraPosPreset(clo.cameraPos);

  //// NV Perf Thing ////
  perf_initNvPm();
  perf_initMode(clo.perfMode);

  subvol::renderhelp::initializeControls(window, g_renderer);
  renderInitComplete = true;
  subvol::renderhelp::loop(window, g_renderer.get());

  return 0;

} // run_subvol

} // namespace subvol


/////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
  TCLAP::CmdLine cmd("Simple Blocks blocking volume render experiment.", ' ');
  subvol::CommandLineOptions clo;
  if (subvol::parseThem(argc, argv, cmd, clo) == 0) {
    std::cout << "No arguments provided.\nPlease use -h for usage info."
              << std::endl;
    return 1;
  }


  if (clo.indexFilePath.empty()) {
    bd::Err() << "Provide an index file path.";
    return 1;
  }


  // start renderer on separate thread.
  std::future<int>
      returned = std::async(std::launch::async,
                            [&clo]() {
                              return subvol::run_subvol(clo);
                            });

  //TODO: use semaphore
  while(renderInitComplete == false);

  QApplication a(argc, argv);
  subvol::ControlPanel panel(g_renderer.get());

  panel.setGlobalRange(g_rovMin, g_rovMax);
  panel.setMinMax(clo.blockThreshold_Min, clo.blockThreshold_Max);
  panel.show();

  a.exec();

  std::cout << "Waiting..." << std::endl;
  returned.wait();
  std::cout << "subvol exiting: " << returned.get() << std::endl;

//  printNvPmApiCounters(clo.perfOutPath.c_str());
  subvol::cleanup();

  return 0;
}
