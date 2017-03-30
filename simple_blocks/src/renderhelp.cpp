//
// Created by Jim Pelton on 8/1/16.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderhelp.h"
#include "blockloader.h"
#include "blockcollection.h"
#include "renderer.h"
#include "controls.h"
#include "timing.h"


#include <bd/log/logger.h>
#include <bd/log/gl_log.h>
#include "cmdline.h"

//#include <glm/glm.hpp>
//#include <glm/matrix.hpp>

namespace subvol
{
namespace renderhelp
{

namespace
{

void
s_error_callback(int error, const char *description)
{
  bd::Err() << "GLFW ERROR: code " << error << " msg: " << description;
}

void
initializeMemoryBuffers(std::vector<char *> *buffers, size_t num, size_t sz)
{
  buffers->resize(num, nullptr);
  char *mem{ new char[num*sz] };
  
  for (size_t i{ 0 }; i<num; ++i)
  {
    char *idx = mem + i * sz;
    (*buffers)[i] = idx;
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
//glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
#endif

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window{ glfwCreateWindow(screenWidth, screenHeight, "Blocks",
                                       nullptr, nullptr) };

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

  glfwSwapInterval(1); // 0 = no vertical sync.

#ifndef __APPLE__
  bd::subscribe_debug_callbacks();
#endif

// Generate OpenGL queries for frame times.
  subvol::timing::genQueries();

  bd::checkForAndLogGlError(__FILE__, __FUNCTION__, __LINE__);

  return window;
} // initGLContext()


bool
initializeBlockCollection(BlockCollection **bc,
                          bd::IndexFile const *indexFile,
                          subvol::CommandLineOptions const &clo)
{
  glm::u64vec3 dims = indexFile->getVolume().block_dims();
  bd::DataType type = bd::IndexFileHeader::getType(indexFile->getHeader());

  // Number of bytes on the GPU for each block (sizeof(float)).
  uint64_t blockBytes = dims.x * dims.y * dims.z * sizeof(float);

  BLThreadData *tdata{ new BLThreadData() };
  size_t numBlocks{ indexFile->getFileBlocks().size() };

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
    tdata->maxCpuBlocks = tdata->maxCpuBlocks > numBlocks ?
                          numBlocks : tdata->maxCpuBlocks;

    // Find max gpu blocks (assert no larger than actual number of blocks).
    tdata->maxGpuBlocks = clo.gpuMemoryBytes / blockBytes;
    tdata->maxGpuBlocks = tdata->maxGpuBlocks > numBlocks ?
                          numBlocks : tdata->maxGpuBlocks;
  } // else

  tdata->type = type;
  tdata->slabDims[0] = indexFile->getVolume().voxelDims().x;
  tdata->slabDims[1] = indexFile->getVolume().voxelDims().y;
  tdata->filename = clo.rawFilePath;

  tdata->texs = new std::vector<bd::Texture*>();
  tdata->buffers = new std::vector<char*>();

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

  BlockLoader *loader{ new BlockLoader(tdata, indexFile->getVolume()) };

  BlockCollection *bc_local{ new BlockCollection(loader) };
  bc_local->initBlocksFromIndexFile(*indexFile, tdata->texs, tdata->buffers);

  bd::Info() << bc_local->blocks().size() << " blocks in index file.";

  // filter blocks in the index file that are within ROV thresholds
  bc_local->filterBlocksByROVRange(clo.blockThreshold_Min, clo.blockThreshold_Max);
  *bc = bc_local;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
void
setInitialGLState()
{
  bd::Info() << "Initializing gl state.";
  gl_check(glClearColor(0.15f, 0.15f, 0.15f, 0.0f));

  gl_check(glEnable(GL_CULL_FACE));
  gl_check(glCullFace(GL_FRONT));
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


///////////////////////////////////////////////////////////////////////////////
void
loop(GLFWwindow *window, BlockRenderer *renderer)
{
  assert(window != nullptr && "window was passed as nullptr in loop()");
  bd::Info() << "About to enter render loop.";

  do {
    subvol::timing::startCpuTime();
//    subvol::timing::startGpuTimerQuery();
    renderer->draw();
    glfwSwapBuffers(window);
//    subvol::timing::endGpuTimerQuery();
    glfwPollEvents();
    subvol::timing::endCpuTime();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0);

  bd::Info() << "Render loop exited.";
}

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
