//
// Created by Jim Pelton on 8/1/16.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderhelp.h"
#include "blockloader.h"
#include "blockcollection.h"
#include "controls.h"
#include "create_vao.h"
#include "timing.h"


#include <bd/log/logger.h>
#include <bd/log/gl_log.h>
#include <bd/graphics/renderer.h>
#include "cmdline.h"

//#include <glm/glm.hpp>
//#include <glm/matrix.hpp>

namespace subvol
{
namespace renderhelp
{

std::shared_ptr<subvol::BlockRenderer> g_renderer{ nullptr };
std::shared_ptr<bd::ShaderProgram> g_wireframeShader{ nullptr };
std::shared_ptr<bd::ShaderProgram> g_volumeShader{ nullptr };
std::shared_ptr<bd::ShaderProgram> g_volumeShaderLighting{ nullptr };
std::shared_ptr<bd::VertexArrayObject> g_axisVao{ nullptr };
std::shared_ptr<bd::VertexArrayObject> g_boxVao{ nullptr };
std::shared_ptr<bd::VertexArrayObject> g_quadVao{ nullptr };
std::shared_ptr<subvol::BlockCollection> g_blockCollection{ nullptr };
std::shared_ptr<bd::IndexFile> g_indexFile{ nullptr };
double g_rovMin = 0;
double g_rovMax = 0;

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
initializeBlockCollection(bd::IndexFile const *indexFile,
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

  BlockCollection *bc_local{ new BlockCollection(loader, *indexFile) };
  bc_local->setRangeMin(0);
  bc_local->setRangeMax(0);
  bc_local->changeClassificationType(ClassificationType::Rov);
  g_blockCollection = std::shared_ptr<BlockCollection>(bc_local);

  bd::Info() << bc_local->getBlocks().size() << " blocks in index file.";

  // filter blocks in the index file that are within ROV thresholds
  //bc_local->filterBlocksByROVRange(clo.blockThreshold_Min, clo.blockThreshold_Max);
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
int
initializeShaders(subvol::CommandLineOptions &clo)
{
  int rval = 0b111;


  // Wireframe Shader
  renderhelp::g_wireframeShader = std::make_shared<bd::ShaderProgram>();
  GLuint programId{
      renderhelp::g_wireframeShader->linkProgram("shaders/vert_vertexcolor_passthrough.glsl",
                                                 "shaders/frag_vertcolor.glsl") };
  if (programId == 0) {
    bd::Err() << "Error building passthrough shader, program id was 0.";
    rval &= 0b110;
  }
  renderhelp::g_wireframeShader->unbind();


  // Volume shader
  renderhelp::g_volumeShader = std::make_shared<bd::ShaderProgram>();
  programId =
      renderhelp::g_volumeShader->linkProgram("shaders/vert_vertexcolor_passthrough.glsl",
                                              "shaders/frag_volumesampler_noshading.glsl");
  if (programId == 0) {
    bd::Err() << "Error building volume shader, program id was 0.";
    rval &= 0b101;
  }
  renderhelp::g_volumeShader->unbind();


  // Volume shader with Lighting
  renderhelp::g_volumeShaderLighting = std::make_shared<bd::ShaderProgram>();
  programId =
      renderhelp::g_volumeShaderLighting->linkProgram("shaders/vert_vertexcolor_passthrough.glsl",
                                                      "shaders/frag_shading_otfgrads.glsl");
  if (programId == 0) {
    bd::Err() << "Error building volume lighting shader, program id was 0.";
    rval &= 0b011;
  }
  renderhelp::g_volumeShaderLighting->unbind();


  return rval;
}


/////////////////////////////////////////////////////////////////////////////////
void
initializeVertexBuffers(subvol::CommandLineOptions const &clo)
{

  // 2d slices
  renderhelp::g_quadVao = std::make_shared<bd::VertexArrayObject>();
  renderhelp::g_quadVao->create();
  //TODO: generate quads shaped to the actual volume dimensions.
  bd::Dbg() << "Generating proxy geometry VAO";

  subvol::genQuadVao(*g_quadVao,
                     { -0.5f, -0.5f, -0.5f },
                     { 0.5f, 0.5f, 0.5f },
                     { clo.num_slices, clo.num_slices, clo.num_slices });


  // coordinate axis
  bd::Dbg() << "Generating coordinate axis VAO";
  renderhelp::g_axisVao = std::make_shared<bd::VertexArrayObject>();
  renderhelp::g_axisVao->create();
  subvol::genAxisVao(*g_axisVao);


  // bounding boxes
  bd::Dbg() << "Generating bounding box VAO";
  renderhelp::g_boxVao = std::make_shared<bd::VertexArrayObject>();
  renderhelp::g_boxVao->create();
  subvol::genBoxVao(*g_boxVao);
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
