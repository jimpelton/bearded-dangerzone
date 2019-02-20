//
// Created by Jim Pelton on 8/1/16.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderhelp.h"
#include "io/blockloader.h"
#include "io/blockcollection.h"
#include "controls.h"
#include "create_vao.h"
#include "timing.h"
#include "cmdline.h"
#include "colormap.h"
#include "constants.h"
#include "slicingblockrenderer.h"
#include "raycaster/blockraycaster.h"

#include <bd/log/logger.h>
#include <bd/log/gl_log.h>
#include <bd/graphics/renderer.h>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <bd/io/indexfile/v2/jsonindexfile.h>

namespace subvol
{
namespace renderhelp
{
//std::shared_ptr<subvol::BlockRenderer> g_renderer{ nullptr };
std::shared_ptr<bd::ShaderProgram> g_wireframeShader{ nullptr };
std::shared_ptr<bd::ShaderProgram> g_volumeShader{ nullptr };
std::shared_ptr<bd::ShaderProgram> g_volumeShaderLighting{ nullptr };
std::shared_ptr<bd::VertexArrayObject> g_axisVao{ nullptr };
std::shared_ptr<bd::VertexArrayObject> g_boxVao{ nullptr };
std::shared_ptr<bd::VertexArrayObject> g_quadVao{ nullptr };
//std::shared_ptr<subvol::BlockCollection> g_blockCollection{ nullptr };
//std::shared_ptr<bd::IndexFile> g_indexFile{ nullptr };
double g_rovMin = 0;
double g_rovMax = 0;

//BLThreadData g_blThreadData{};
namespace
{
/////////////////////////////////////////////////////////////////////////////////
void
s_error_callback(int error, const char *description)
{
  bd::Err() << "GLFW ERROR: code " << error << " msg: " << description;
}


/////////////////////////////////////////////////////////////////////////////////
void
initializeMemoryBuffers(std::vector<char *> *buffers, size_t num, size_t sz)
{
  buffers->resize(num, nullptr);
  char *mem{ new char[num * sz] };

  for (size_t i{ 0 }; i < num; ++i) {
    char *idx = mem + i * sz;
    ( *buffers )[i] = idx;
  }
}
} // namespace

///////////////////////////////////////////////////////////////////////////////
GLFWwindow *
initGLContext(int screenWidth, int screenHeight)
{
  bd::Info() << "Initializing GLFW.";
  if (!glfwInit()) {
    bd::Err() << "could not start GLFW3";
    return nullptr;
  }

  glfwSetErrorCallback(s_error_callback);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  // number of samples to use for multi sampling
  glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window{
      glfwCreateWindow(screenWidth, screenHeight, "Blocks",
                       nullptr, nullptr)
  };

  if (!window) {
    bd::Err() << "ERROR: could not open window with GLFW3";
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  GLenum error{ glewInit() };
  if (error) {
    bd::Err() << "Could not initGLContext glew " << glewGetErrorString(error);
    return nullptr;
  }

  // Generate OpenGL queries for frame times.
  subvol::timing::genQueries();

  glfwSwapInterval(1); // 0 = no vertical sync.

#ifndef __APPLE__
  bd::subscribe_debug_callbacks();
#endif

  bd::checkForAndLogGlError(__FILE__, __FUNCTION__, __LINE__);

  return window;
} // initGLContext()

BlockLoader *
initializeBlockLoader(bd::indexfile::v2::JsonIndexFile const &indexFile,
                      subvol::CommandLineOptions const &clo)
{
  glm::u64vec3 dims = indexFile.getVolume().block_dims();
  bd::DataType type = indexFile.getDatType();

  // Number of bytes on the GPU for each block (sizeof(float)).
  uint64_t blockBytes = dims.x * dims.y * dims.z * sizeof(float);

  BLThreadData *tdata{ new BLThreadData() };
  size_t numBlocks{ indexFile.getFileBlocks().size() };

  // Provided block dimensions were such that we got 0 for the block bytes,
  // so lets not allow rendering of any blocks at all.
  if (blockBytes == 0) {
    tdata->maxCpuBlocks = 0;
    tdata->maxGpuBlocks = 0;
    bd::Warn() << "Blocks have a dimension that is 0."; //, so I can't go on.";
  } else {
    bd::Info() << "Block texture size (bytes): " << blockBytes;

    // Find max cpu blocks (assert no larger than actual number of blocks).
    tdata->maxCpuBlocks = clo.mainMemoryBytes / blockBytes;
    tdata->maxCpuBlocks = tdata->maxCpuBlocks > numBlocks
                          ? numBlocks
                          : tdata->maxCpuBlocks;

    // Find max gpu blocks (assert no larger than actual number of blocks).
    tdata->maxGpuBlocks = clo.gpuMemoryBytes / blockBytes;
    tdata->maxGpuBlocks = tdata->maxGpuBlocks > numBlocks
                          ? numBlocks
                          : tdata->maxGpuBlocks;
  } // else

  tdata->type = type;
  tdata->slabDims[0] = indexFile.getVolume().voxelDims().x;
  tdata->slabDims[1] = indexFile.getVolume().voxelDims().y;
  tdata->filename = clo.rawFilePath;

  tdata->texs = new std::vector<bd::Texture *>();
  tdata->buffers = new std::vector<char *>();

  // ugh, such cringe! more global data = more ugh!
  //  g_blThreadData = *tdata;

  bd::Info() << "Max cpu blocks: " << tdata->maxCpuBlocks;
  bd::Info() << "Max GPU blocks: " << tdata->maxGpuBlocks;

  bd::Texture::GenTextures3d(tdata->maxGpuBlocks,
                             bd::DataType::Float,
                             bd::Texture::Format::R32F,
                             bd::Texture::Format::RED,
                             dims.x, dims.y, dims.z,
                             tdata->texs);

  bd::Info() << "Generated " << tdata->texs->size() << " textures.";

  initializeMemoryBuffers(tdata->buffers, tdata->maxCpuBlocks, blockBytes);
  bd::Info() << "Generated " << tdata->buffers->size() << " main memory buffers.";

  BlockLoader *loader{ new BlockLoader(tdata, indexFile.getVolume()) };
  return loader;
}


/////////////////////////////////////////////////////////////////////////////////
BlockCollection *
initializeBlockCollection(BlockLoader *loader,
                          bd::indexfile::v2::JsonIndexFile const &indexFile,
                          subvol::CommandLineOptions const &clo)
{
  BlockCollection *bc{ new BlockCollection(loader, indexFile) };
  bc->setRangeMin(0);
  bc->setRangeMax(0);
  bc->changeClassificationType(ClassificationType::Rov);
  //  g_blockCollection = std::shared_ptr<BlockCollection>(bc);

  bd::Info() << bc->getBlocks().size() << " blocks in index file.";

  // filter blocks in the index file that are within ROV thresholds
  //bc_local->filterBlocksByROVRange(clo.blockThreshold_Min, clo.blockThreshold_Max);
  return bc;
}


///////////////////////////////////////////////////////////////////////////////
void
setInitialGLState()
{
  bd::Info() << "Initializing gl state.";
  gl_check(glClearColor(0.15f, 0.15f, 0.15f, 0.0f));

  gl_check(glEnable(GL_CULL_FACE));
  gl_check(glCullFace(GL_BACK));
//  gl_check(glDisable(GL_CULL_FACE));

  gl_check(glEnable(GL_DEPTH_TEST));
  gl_check(glDepthFunc(GL_LESS));

  gl_check(glEnable(GL_BLEND));
  gl_check(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  gl_check(glEnable(GL_PRIMITIVE_RESTART));
  gl_check(glPrimitiveRestartIndex(0xFFFF));
}


///////////////////////////////////////////////////////////////////////////////
void
initializeControls(GLFWwindow *window, std::shared_ptr<BlockRenderer> renderer)
{
  Controls::initialize(std::move(renderer));

  glfwSetCursorPosCallback(window, &Controls::s_cursorpos_callback);
  glfwSetWindowSizeCallback(window, &Controls::s_window_size_callback);
  glfwSetKeyCallback(window, &Controls::s_keyboard_callback);
  glfwSetScrollCallback(window, &Controls::s_scrollwheel_callback);
  //  glfwSetMouseButtonCallback(window, &Controls::s_mousebutton_callback);
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
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
    if (!clo.colorTFuncPath.empty() || !clo.opacityTFuncPath.empty()) {
      loaded = ColorMapManager::loadColorMap("USER", clo.colorTFuncPath,
                                             clo.opacityTFuncPath);
    } else if (!clo.tfunc1dtPath.empty()) {
      loaded = ColorMapManager::load1DT("USER", clo.tfunc1dtPath);
    }

  }
  catch (std::ios_base::failure &) {
    bd::Warn() << "Error reading user defined transfer function file(s). "
                  "The function won't be available.";
  }

  return loaded;
}


///////////////////////////////////////////////////////////////////////////////
bool
initializeShaders(subvol::CommandLineOptions const &clo)
{
  GLuint programId{ 0 };

  // Wireframe Shader
  renderhelp::g_wireframeShader = std::make_shared<bd::ShaderProgram>();
  programId = renderhelp::g_wireframeShader->linkProgram(
      "shaders/vert_vertexcolor_passthrough.glsl",
      "shaders/frag_vertcolor.glsl");
  if (programId == 0) {
    bd::Err() << "Error building passthrough shader, program id was 0.";
    return false;
  }
  renderhelp::g_wireframeShader->unbind();


  // Volume shader
  renderhelp::g_volumeShader = std::make_shared<bd::ShaderProgram>();
  programId = renderhelp::g_volumeShader->linkProgram(
      "shaders/vert_vertexcolor_passthrough.glsl",
      "shaders/frag_volumesampler_noshading.glsl");
  if (programId == 0) {
    bd::Err() << "Error building volume shader, program id was 0.";
    return false;
  }
  renderhelp::g_volumeShader->unbind();


  // Volume shader with Lighting
  renderhelp::g_volumeShaderLighting = std::make_shared<bd::ShaderProgram>();
  programId = renderhelp::g_volumeShaderLighting->linkProgram(
      "shaders/vert_vertexcolor_passthrough.glsl",
      "shaders/frag_shading_otfgrads.glsl");
  if (programId == 0) {
    bd::Err() << "Error building volume lighting shader, program id was 0.";
    return false;
  }
  renderhelp::g_volumeShaderLighting->unbind();

  return true;
}


/////////////////////////////////////////////////////////////////////////////////
void
initializeVertexBuffers(subvol::CommandLineOptions const &clo,
                        bd::Volume const &v,
                        glm::u64vec3 *numSlices)
{
  // 2d slices
  renderhelp::g_quadVao = std::make_shared<bd::VertexArrayObject>();
  renderhelp::g_quadVao->create();
  *numSlices = subvol::genQuadVao(*g_quadVao, v,
      glm::vec3{clo.smod_x, clo.smod_y, clo.smod_z});

  // coordinate axis
  renderhelp::g_axisVao = std::make_shared<bd::VertexArrayObject>();
  renderhelp::g_axisVao->create();
  subvol::genAxisVao(*g_axisVao);


  // bounding boxes
  renderhelp::g_boxVao = std::make_shared<bd::VertexArrayObject>();
  renderhelp::g_boxVao->create();
  subvol::genBoxVao(*g_boxVao);
}


/////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<BlockRenderer>
initializeRenderer(std::shared_ptr<BlockCollection> bc,
                   bd::Volume const &v,
                   subvol::CommandLineOptions const &clo)
{
  renderhelp::setInitialGLState();
  glm::u64vec3 numSlices;
  renderhelp::initializeVertexBuffers(clo, v, &numSlices);
  bd::Info() << "Generated: " << numSlices[0] << "x" << numSlices[1] << "x" << numSlices[2] << " slices.";
  if (!renderhelp::initializeShaders(clo)) {
    return nullptr;
  }

  bool loaded = initializeTransferFunctions(clo);

//  BlockRenderer *br = new SlicingBlockRenderer(
//      numSlices,
//      renderhelp::g_volumeShader,
//      renderhelp::g_volumeShaderLighting,
//      renderhelp::g_wireframeShader,
//      bc,
//      renderhelp::g_quadVao,
//      renderhelp::g_boxVao,
//      renderhelp::g_axisVao);

  BlockRenderer *br = new subvol::render::BlockingRaycaster(bc, v);

  setRendererInitialTransferFunction(loaded, "USER", *br);

  br->resize(clo.windowWidth, clo.windowHeight);
  br->getCamera().setEye({ 0, 0, 4 });
  br->getCamera().setLookAt({ 0, 0, 0 });
  br->getCamera().setUp({ 0, 1, 0 });
  br->setViewMatrix(br->getCamera().createViewMatrix());
  br->setDrawNonEmptyBlocks(true);
  br->setDrawNonEmptyBoundingBoxes(false);

  return std::shared_ptr<BlockRenderer>(br);
}


///////////////////////////////////////////////////////////////////////////////
void
queryGPUMemory(int64_t *total, int64_t *avail)
{
  GLint64 total_mem_kb = 0;
  //0x9047
  glGetInteger64v(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &total_mem_kb);

  *total = total_mem_kb * 1024;

  if (avail) {
    GLint cur_avail_mem_kb = 0;
    // 0x9049
    glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &cur_avail_mem_kb);
    *avail = cur_avail_mem_kb * 1024;
  }
}


///////////////////////////////////////////////////////////////////////////////
// Keep this commented code for later -- JP 2018/02/08
//void
//setCameraPosPreset(unsigned int cameraPos)
//{
//
//  glm::quat r;
//  switch (cameraPos) {
//    case 3:
//      break;
//    case 2:
//      //put camera at { 2.0f, 0.0f, 0.0f  } (view along positive X axis)
//      r = glm::rotate(r, -1.0f * glm::half_pi<float>(), Y_AXIS);
//      //g_camera.rotateTo(Y_AXIS);
//      break;
//    case 1:
//      //put camera at { 0.0f, 2.0f, 0.0f } (view along positive Y axis)
//      r = glm::rotate(r, glm::half_pi<float>(), X_AXIS);
//      //g_camera.rotateTo(X_AXIS);
//      break;
//    case 0:
//    default:
//      //put camera at oblique positive quadrant.
//      // no rotation needed, this is default cam location.
//      r = glm::rotate(r, glm::pi<float>(), Y_AXIS) *
//        glm::rotate(r, glm::pi<float>() / 4.0f, X_AXIS);
//
//      //g_camera.rotateTo(Z_AXIS);
//      break;
//  }
//}

//void setDefaultView(glm::vec3 const &eye)
//{
//  camera->setEye(eye);
//  camera->setLookAt({ 0, 0, 0 });
//  camera->setUp({ 0, 1, 0 });
//  renderer->setViewMatrix(camera->createViewMatrix());
//}
} // namespace renderhelp
} // namespace subvol
