#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

// BD lib
#include <bd/geo/axis.h>
#include <bd/graphics/shader.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/log/logger.h>
#include <bd/log/gl_log.h>
//#include <bd/volume/block.h>
#include <bd/volume/blockcollection.h>
#include <bd/util/util.h>
#include <bd/util/ordinal.h>
#include <bd/io/indexfile.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>

// STL and STD lib
#include <string>
#include <vector>
#include <array>

#include <fstream>
#include <iostream>
#include <ostream>

#include <memory>
#include <chrono>

#include <cstring>

// profiling
#include "nvpm.h"


std::shared_ptr<bd::CoordinateAxis> g_axis{ }; ///< The coordinate axis lines.
std::shared_ptr<subvol::BlockRenderer> g_renderer{};
std::shared_ptr<bd::ShaderProgram> g_wireframeShader{};
std::shared_ptr<bd::ShaderProgram> g_volumeShader{};
std::shared_ptr<bd::VertexArrayObject> g_axisVao{};
std::shared_ptr<bd::VertexArrayObject> g_boxVao{};
std::shared_ptr<bd::VertexArrayObject> g_quadVao{};
std::shared_ptr<subvol::Controls> g_controls{};


void cleanup();
void printBlocks(bd::BlockCollection *bcol);
void setCameraPosPreset(unsigned cameraPos);
void printNvPmApiCounters(const char *perfOutPath);



/////////////////////////////////////////////////////////////////////////////////
void cleanup()
{
  glfwTerminate();
}


/////////////////////////////////////////////////////////////////////////////////
void printBlocks(bd::BlockCollection *bcol)
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



/// \brief Set camera orientation to along X, Y, or Z axis
void setCameraPosPreset(unsigned cameraPos)
{
  glm::quat r;
  switch (cameraPos) {
    case 3:
      break;
    case 2:
      //put camera at { 2.0f, 0.0f, 0.0f  } (view along positive X axis)
      r = glm::rotate(r, -1.0f * glm::half_pi<float>(), Y_AXIS);
      //g_camera.rotateTo(Y_AXIS);
      break;
    case 1:
      //put camera at { 0.0f, 2.0f, 0.0f } (view along positive Y axis)
      r = glm::rotate(r, glm::half_pi<float>(), X_AXIS);
      //g_camera.rotateTo(X_AXIS);
      break;
    case 0:
    default:
      //put camera at oblique positive quadrant.
      // no rotation needed, this is default cam location.
      r = glm::rotate(r, glm::pi<float>(), Y_AXIS) *
        glm::rotate(r, glm::pi<float>() / 4.0f, X_AXIS);

      //g_camera.rotateTo(Z_AXIS);
      break;
  }
}

/// \brief print counters from NvPmApi to file \c perfOutPath, or to \c stdout
///  if no path is provided.
void printNvPmApiCounters(const char *perfOutPath = "")
{
  if (std::strlen(perfOutPath) == 0) {
    perf_printCounters(std::cout);
    subvol::timing::printTimes(std::cout);
  }
  else {
    std::ofstream outStream(perfOutPath);
    if (outStream.is_open()) {
      perf_printCounters(outStream);
      subvol::timing::printTimes(outStream);
    }
    else {
      bd::Err() <<
                "Could not open " << perfOutPath << " for performance counter output. "
                  "Using stdout instead.";
      perf_printCounters(std::cout);
      subvol::timing::printTimes(std::cout);
    }
  }
}


/////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char *argv[])
{
  subvol::CommandLineOptions clo;
  if (subvol::parseThem(argc, argv, clo) == 0) {
    std::cout << "No arguments provided.\nPlease use -h for usage info."
              << std::endl;
    return 1;
  }

  // Read the index file to get values from it that we need to populate
  // the CommandLineOptions struct.
  std::shared_ptr<bd::IndexFile> indexFile{
    bd::IndexFile::fromBinaryIndexFile(clo.indexFilePath) };
  if (indexFile == nullptr) {
    bd::Err() << "Could open the index file.";
    return 1;
  }

  // Since the IndexFileHeader contains most of the options needed to
  // render the volume, we copy those over into the CommandLineOptions struct.
  // Without an index file these options are provided via argv anyway.
  clo.vol_w = indexFile->getHeader().volume_extent[0];
  clo.vol_h = indexFile->getHeader().volume_extent[1];
  clo.vol_d = indexFile->getHeader().volume_extent[2];
  clo.numblk_x = indexFile->getHeader().numblocks[0];
  clo.numblk_y = indexFile->getHeader().numblocks[1];
  clo.numblk_z = indexFile->getHeader().numblocks[2];
  clo.dataType = bd::to_string(bd::IndexFileHeader::getType(indexFile->getHeader()));
  subvol::printThem(clo);

  // Initialize OpenGL and GLFW and generate our transfer function textures.
  GLFWwindow *window{ subvol::renderhelp::initGLContext(clo.windowWidth,
                                                        clo.windowHeight) };
  if (window == nullptr) {
    bd::Err() << "Could not initialize GLFW, exiting.";
    return 1;
  }
  subvol::renderhelp::setInitialGLState();
  subvol::ColorMapManager::generateDefaultTransferFunctionTextures();


  // Setup the block collection and give up ownership of the index file.
  bd::BlockCollection *blockCollection{ new bd::BlockCollection() };
  blockCollection->initBlocksFromIndexFile(std::move(indexFile));

  // This lambda is used by the BlockCollection to filter the blocks by
  // the block average voxel value.
  auto isEmpty = [&](bd::Block const *b) -> bool {
    return b->avg() < clo.tmin || b->avg() > clo.tmax;
  };
  blockCollection->filterBlocks(isEmpty);
  blockCollection->initBlockTextures(clo.rawFilePath);

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
  GLuint wireframeProgramId{
    g_wireframeShader->linkProgram("shaders/vert_vertexcolor_passthrough.glsl",
                                   "shaders/frag_vertcolor.glsl") };
  if (wireframeProgramId == 0) {
    bd::Err() << "Error building passthrough shader, program id was 0.";
    return 1;
  }


  // Volume shader
  g_volumeShader = std::make_shared<bd::ShaderProgram>();
  GLuint volumeProgramId{
    g_volumeShader->linkProgram("shaders/vert_vertexcolor_passthrough.glsl",
                                "shaders/frag_volumesampler_noshading.glsl") };
  if (volumeProgramId == 0) {
    bd::Err() << "Error building volume sampling shader, program id was 0.";
    return 1;
  }

  //TODO: move renderer init into initGLContext().
  g_renderer =
    std::make_shared<subvol::BlockRenderer>(int(clo.num_slices),
                                            g_volumeShader, g_wireframeShader,
                                            &blockCollection->nonEmptyBlocks(),
                                            g_quadVao, g_boxVao, g_axisVao);

  g_renderer->resize(clo.windowWidth, clo.windowHeight);
  g_renderer->getCamera().setEye({ 0, 0, 2 });
  g_renderer->getCamera().setLookAt({ 0, 0, 0 });
  g_renderer->getCamera().setUp({ 0, 1, 0 });
  g_renderer->setViewMatrix(g_renderer->getCamera().createViewMatrix());

  if (! clo.tfuncPath.empty()) {
    subvol::ColorMapManager::load_1dt("USER", clo.tfuncPath);
  }
  g_renderer->setTFuncTexture(subvol::ColorMapManager::getMapTextureByName("USER"));
//  m_renderer->setTFuncTexture(*subvol::ColorMapManager::getMapTextureByName("FULL_RAINBOW"));
//  g_renderer->setTfuncScaleValue(g_scaleValue);
  g_renderer->init();

//  setCameraPosPreset(clo.cameraPos);

  //// NV Perf Thing ////
  perf_initNvPm();
  perf_initMode(clo.perfMode);
  subvol::renderhelp::initializeControls(window, g_renderer);
  subvol::renderhelp::loop(window, g_renderer.get());

  printNvPmApiCounters(clo.perfOutPath.c_str());
  cleanup();

  return 0;
}
