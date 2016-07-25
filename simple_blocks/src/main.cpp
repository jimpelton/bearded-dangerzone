#include <GL/glew.h>
#include <GLFW/glfw3.h>

// local includes
#include "cmdline.h"
#include "create_vao.h"
#include "axis_enum.h"
#include "timing.h"
#include "blockrenderer.h"
#include "constants.h"

// BD lib
#include <bd/geo/axis.h>
#include <bd/geo/BBox.h>
#include <bd/geo/quad.h>
#include <bd/graphics/shader.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/graphics/view.h>
#include <bd/log/logger.h>
#include <bd/log/gl_log.h>
#include <bd/volume/block.h>
#include <bd/volume/blockcollection.h>
#include <bd/util/util.h>
#include <bd/util/ordinal.h>
#include <bd/io/indexfile.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>
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
#include "colormap.h"


// Constant definitions

const int ELEMENTS_PER_QUAD{ 5 }; //< 5 elements = 4 verts + 1 restart symbol

const glm::vec3 X_AXIS{ 1.0f, 0.0f, 0.0f };
const glm::vec3 Y_AXIS{ 0.0f, 1.0f, 0.0f };
const glm::vec3 Z_AXIS{ 0.0f, 0.0f, 1.0f };

const int VERTEX_COORD_ATTR = 0;
const int VERTEX_COLOR_ATTR = 1;

const int BLOCK_TEXTURE_UNIT = 0;
const int TRANSF_TEXTURE_UNIT = 1;

const char *VOLUME_SAMPLER_UNIFORM_STR = "volume_sampler";
const char *TRANSF_SAMPLER_UNIFORM_STR = "tf_sampler";

const char *VOLUME_MVP_MATRIX_UNIFORM_STR = "mvp";
const char *VOLUME_TRANSF_UNIFORM_STR = "tfScalingVal";

const char *WIREFRAME_MVP_MATRIX_UNIFORM_STR = "mvp";



///////////////////////////////////////////////////////////////////////////////
// Geometry  /  VAOs
///////////////////////////////////////////////////////////////////////////////


bd::CoordinateAxis g_axis; ///< The coordinate axis lines.
//bd::Box g_box;

BlockRenderer *g_volRend{ nullptr };

size_t g_elementBufferSize{ 0 };
bd::VertexArrayObject *g_axisVao{ nullptr };
bd::ShaderProgram *g_wireframeShader{ nullptr };

float g_scaleValue{ 1.0f };

///////////////////////////////////////////////////////////////////////////////
// Viewing and Controls Data
///////////////////////////////////////////////////////////////////////////////
bd::View g_camera;
int g_screenWidth{ 1000 };
int g_screenHeight{ 1000 };
float g_fov_deg{ 50.0f };   ///< Field of view in degrees.

glm::vec2 g_cursorPos;
float g_mouseSpeed{ 1.0f };
bool g_toggleBlockBoxes{ false };
bool g_toggleWireFrame{ false };

//bd::IndexFile *g_indexFile;
//std::vector<bd::Block*> g_blocks;

glm::vec3 g_backgroundColors[2] {
    {0.15, 0.15, 0.15},
    {1.0, 1.0, 1.0 }
};
int g_currentBackgroundColor{ 0 };

//TODO: bool g_toggleVolumeBox{ false };


///////////////////////////////////////////////////////////////////////////////
//  Miscellaneous  radness
///////////////////////////////////////////////////////////////////////////////



void glfw_cursorpos_callback(GLFWwindow *window, double x, double y);

void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode,
                            int action, int mods);

void glfw_error_callback(int error, const char *description);

void glfw_window_size_callback(GLFWwindow *window, int width, int height);

void glfw_scrollwheel_callback(GLFWwindow *window, double xoff, double yoff);

void setRotation(glm::vec2 const &dr);

void loop(GLFWwindow *window);

void cleanup();

unsigned int loadTransfer_1dtformat(std::string const &filename,
                                    bd::Texture &transferTex,
                                    bd::ShaderProgram &volumeShader);


/************************************************************************/
/* Timer Stuff                                                          */
/************************************************************************/



///////////////////////////////////////////////////////////////////////////////
/// \brief Init opengl queries for GPU frame times.
///////////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* G L F W     C A L L B A C K S                                        */
/************************************************************************/
void glfw_error_callback(int error, const char *description) {
  bd::Err() << "GLFW ERROR: code " << error << " msg: " << description;
}

void setCameraPosPreset(unsigned int);

////////////////////////////////////////////////////////////////////////////////
void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode,
  int action, int mods) {

  // on key press
  if (action == GLFW_PRESS) {

    switch (key) {

    case GLFW_KEY_0:
      setCameraPosPreset(0);
      break;

    case GLFW_KEY_1:
      setCameraPosPreset(1);
      break;

    case GLFW_KEY_2:
      setCameraPosPreset(2);
      break;

    case GLFW_KEY_3:
      setCameraPosPreset(3);
      break;

    case GLFW_KEY_W:
      g_toggleWireFrame = !g_toggleWireFrame;
      break;

    case GLFW_KEY_B:
      g_toggleBlockBoxes = !g_toggleBlockBoxes;
      std::cout << "Toggle bounding boxes.\n";
      break;

    case GLFW_KEY_Q:
      g_currentBackgroundColor ^= 1;
      std::cout << "Background color: "
                << (g_currentBackgroundColor == 0 ? "Dark" : "Light")
                << '\n';
      g_volRend->setBackgroundColor(g_backgroundColors[g_currentBackgroundColor]);
      break;

    default:
      break;
    } // switch
  }

  // while holding key down.
  if (action != GLFW_RELEASE) {

    switch (key) {

    // Positive transfer function scaling
    case GLFW_KEY_PERIOD:
      if (mods & GLFW_MOD_SHIFT)
        g_scaleValue += 0.1f;
      else if (mods & GLFW_MOD_CONTROL)
        g_scaleValue += 0.001f;
      else if (mods & GLFW_MOD_ALT)
        g_scaleValue += 0.0001f;
      else
        g_scaleValue += 0.01f;

      g_volRend->setTfuncScaleValue(g_scaleValue);
      std::cout << "Transfer function scaler: " << g_scaleValue << std::endl;
      break;

    // Negative transfer function scaling
    case GLFW_KEY_COMMA:
      if (mods & GLFW_MOD_SHIFT)
        g_scaleValue -= 0.1f;
      else if (mods & GLFW_MOD_CONTROL)
        g_scaleValue -= 0.001f;
      else if (mods & GLFW_MOD_ALT)
        g_scaleValue -= 0.0001f;
      else
        g_scaleValue -= 0.01f;

      g_volRend->setTfuncScaleValue(g_scaleValue);
      std::cout << "Transfer function scaler: " << g_scaleValue << std::endl;
      break;

    default:
      break;
    } // switch
  }
}


////////////////////////////////////////////////////////////////////////////////
void glfw_window_size_callback(GLFWwindow *window, int width, int height) {
  g_screenWidth = width;
  g_screenHeight = height;
  g_camera.setGLViewport(0, 0, width, height);
}


////////////////////////////////////////////////////////////////////////////////
void glfw_cursorpos_callback(GLFWwindow *window, double x, double y) {
  glm::vec2 cpos(std::floor(x), std::floor(y));
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS) {
    glm::vec2 delta(cpos - g_cursorPos);
    setRotation(delta);
  }

  g_cursorPos = cpos;
}


///////////////////////////////////////////////////////////////////////////////
void glfw_scrollwheel_callback(GLFWwindow *window, double xoff, double yoff) {
  // 1.75 scales the vertical motion of the scroll wheel so changing the
  // field of view isn't so slow.
  float fov = static_cast<float>(g_fov_deg + (yoff*1.75f));

  if (fov < 1 || fov > 120) return;

  std::cout << "fov: " << fov << std::endl;

  g_fov_deg = fov;
  g_camera.setPerspectiveProjectionMatrix(glm::radians(fov),
      g_screenWidth / float(g_screenHeight),
      0.1f, 10000.0f);
}


/************************************************************************/
/*     D R A W I N'    S T U F F                                        */
/************************************************************************/


////////////////////////////////////////////////////////////////////////////////
void setRotation(const glm::vec2 &dr) {
  glm::quat rotX{ glm::angleAxis<float>(glm::radians(-dr.y)*g_mouseSpeed,
                                        glm::vec3(1, 0, 0)) };

  glm::quat rotY{ glm::angleAxis<float>(glm::radians(dr.x)*g_mouseSpeed,
                                        glm::vec3(0, 1, 0)) };

  g_camera.rotateBy(rotX*rotY);
}






///////////////////////////////////////////////////////////////////////////////
void draw() {
  glm::mat4 viewMat = g_camera.getViewMatrix();
  gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  ////////  Axis    /////////////////////////////////////////
  g_axisVao->bind();
  g_wireframeShader->bind();
  g_wireframeShader->setUniform("mvp", viewMat);
  g_axis.draw();
  g_wireframeShader->unbind();
  g_axisVao->unbind();

  g_volRend->setViewMatrix(viewMat);
  
  ////////  BBoxes  /////////////////////////////////////////
  if (g_toggleBlockBoxes) {
    g_volRend->drawNonEmptyBoundingBoxes();
  }

  //////// Quad Geo (drawNonEmptyBlocks)  /////////////////////
  g_volRend->drawNonEmptyBlocks();

}


///////////////////////////////////////////////////////////////////////////////
void loop(GLFWwindow *window) {
  assert(window!=nullptr && "window was passed as nullptr in loop()");
  bd::Info() << "About to enter render loop.";

  do {
    startCpuTime();
    g_camera.updateViewMatrix();

//    startGpuTimerQuery();

    draw();
    glfwSwapBuffers(window);

//    endGpuTimerQuery();

    glfwPollEvents();

    endCpuTime();

  } while (glfwGetKey(window, GLFW_KEY_ESCAPE)!=GLFW_PRESS &&
      glfwWindowShouldClose(window)==0);

  bd::Info() << "Render loop exited.";
}


///////////////////////////////////////////////////////////////////////////////
//   I N I T I A L I Z A T I O N
///////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
void setInitialGLState() {
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


/////////////////////////////////////////////////////////////////////////////////
GLFWwindow *initGLContext() {
  bd::Info() << "Initializing GLFW.";
  if (!glfwInit()) {
    bd::Err() << "could not start GLFW3";
    return nullptr;
  }

  glfwSetErrorCallback(glfw_error_callback);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  // number of samples to use for multi sampling
  //glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window{
      glfwCreateWindow(g_screenWidth, g_screenHeight, "Blocks",
                       nullptr, nullptr)
  };

  if (!window) {
    bd::Err () << "ERROR: could not open window with GLFW3";
    glfwTerminate();
    return nullptr;
  }

  glfwSetCursorPosCallback(window, glfw_cursorpos_callback);
  glfwSetWindowSizeCallback(window, glfw_window_size_callback);
  glfwSetKeyCallback(window, glfw_keyboard_callback);
  glfwSetScrollCallback(window, glfw_scrollwheel_callback);

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
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
  genQueries();
  
  bd::checkForAndLogGlError(__FILE__, __FUNCTION__, __LINE__);
  
  return window;
}


/////////////////////////////////////////////////////////////////////////////////
void cleanup() {
  //    std::vector<GLuint> bufIds;
  //    for (unsigned i=0; i<NUMBOXES; ++i) {
  //        bufIds.push_back(g_bbox[i].iboId());
  //        bufIds.push_back(g_bbox[i].vboId());
  //    }
  //    glDeleteBuffers(NUMBOXES, &bufIds[0]);
  //    glDeleteProgram(g_shaderProgramId);
  glfwTerminate();
}


/////////////////////////////////////////////////////////////////////////////////
void printBlocks(bd::BlockCollection *bcol) {
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
void printTimes(std::ostream &str) {
  double gputime_ms = getTotalGPUTime_NonEmptyBlocks()*1.0e-6;
  double cputime_ms = getTotalElapsedCPUFrameTime()*1.0e-3;

  str <<
      "frames_rendered: " << getTotalFramesRendered() << " frames\n"
      "gpu_ft_total_nonempty: " << gputime_ms << "ms\n"
      "gpu_ft_avg_nonempty: " << (gputime_ms/getTotalFramesRendered()) << "ms\n"
      "cpu_ft_total: " << cputime_ms << "ms\n"
      "cpu_ft_avg: " << (cputime_ms/getTotalFramesRendered()) << "ms"
      << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Set camera orientation to along X, Y, or Z axis
///////////////////////////////////////////////////////////////////////////////
void setCameraPosPreset(unsigned cameraPos) {
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

  g_camera.rotateBy(r);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief print counters from NvPmApi to file \c perfOutPath, or to \c stdout
///  if no path is provided.
///////////////////////////////////////////////////////////////////////////////
void printNvPmApiCounters(const char *perfOutPath = "") {
  if (std::strlen(perfOutPath)==0) {
    perf_printCounters(std::cout);
    printTimes(std::cout);
  }
  else {
    std::ofstream outStream(perfOutPath);
    if (outStream.is_open()) {
      perf_printCounters(outStream);
      printTimes(outStream);
    }
    else {
      bd::Err() <<
          "Could not open " << perfOutPath << " for performance counter output. "
            "Using stdout instead.";
      perf_printCounters(std::cout);
      printTimes(std::cout);
    }
  }
}


/////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char *argv[]) {
  subvol::CommandLineOptions clo;
  if (subvol::parseThem(argc, argv, clo)==0) {
    std::cout << "No arguments provided.\nPlease use -h for usage info."
        << std::endl;
    return 1;
  }

  // Read the index file to get values from it that we need to populate
  // the CommandLineOptions struct.
  std::unique_ptr<bd::IndexFile> indexFile{
    bd::IndexFile::fromBinaryIndexFile(clo.indexFilePath) };

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

  // Setup the block collection and give up ownership of the index file.
  bd::BlockCollection *blockCollection{ new bd::BlockCollection() };
  blockCollection->initBlocksFromIndexFile(std::move(indexFile));


  // This lambda is used by the BlockCollection to filter the blocks by
  // the block average voxel value.
  auto isEmpty = [&](bd::Block const *b) -> bool {
    return b->avg() < clo.tmin || b->avg() > clo.tmax;
  };
  blockCollection->filterBlocks(isEmpty);


  // Initialize OpenGL and GLFW and generate our transfer function textures.
  GLFWwindow *window{ initGLContext() };
  if (window == nullptr) {
    bd::Err() << "Could not initialize GLFW, exiting.";
    return 1;
  }
  setInitialGLState();
  subvol::ColorMap::generateDefaultTransferFunctionTextures();


  // Now that OpenGL is initialized, generate the textures for each block
  // that is marked non-empty.
  blockCollection->initBlockTextures(clo.rawFilePath);
  bd::Dbg() << blockCollection->blocks()[0]->texture();

  // 2d slices
  bd::VertexArrayObject *quadVao{ new bd::VertexArrayObject() };
  quadVao->create();
  //TODO: generate quads for actual volume extent.
  bd::Dbg() << "Generating proxy geometry VAO";
  subvol::genQuadVao(*quadVao,
                     { -0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f },
                     { clo.num_slices, clo.num_slices, clo.num_slices });

  // coordinate axis
  bd::Dbg() << "Generating coordinate axis VAO";
  bd::VertexArrayObject *axisVao{ new bd::VertexArrayObject() };
  axisVao->create();
  subvol::genAxisVao(*axisVao);
  g_axisVao = axisVao;

  // bounding boxes
  bd::Dbg() << "Generating bounding box VAO";
  bd::VertexArrayObject *boxVao{ new bd::VertexArrayObject() };
  boxVao->create();
  subvol::genBoxVao(*boxVao);


  //// Wireframe Shader ////
  bd::ShaderProgram *wireframeShader{ new bd::ShaderProgram() };
  GLuint wireframeProgramId{
      wireframeShader->linkProgram(
          "shaders/vert_vertexcolor_passthrough.glsl",
          "shaders/frag_vertcolor.glsl")
  };
  if (wireframeProgramId==0) {
    bd::Err() << "Error building passthrough shader, program id was 0.";
    return 1;
  }
  g_wireframeShader = wireframeShader;

  //// Volume shader ////
  bd::ShaderProgram *volumeShader{ new bd::ShaderProgram() };
  GLuint volumeProgramId{
      volumeShader->linkProgram(
          "shaders/vert_vertexcolor_passthrough.glsl",
          "shaders/frag_volumesampler_noshading.glsl")
  };
  if (volumeProgramId==0) {
    bd::Err() << "Error building volume sampling shader, program id was 0.";
    return 1;
  }


  bd::Texture const *colormap{ subvol::ColorMap::getDefaultMapTexture("BLACK_TO_WHITE") };
  BlockRenderer volRend{ int(clo.num_slices),
                         volumeShader,
                         wireframeShader,
                         &blockCollection->nonEmptyBlocks(),
                         colormap,
                         quadVao,
                         boxVao };

  volRend.setTfuncScaleValue(g_scaleValue);
  volRend.init();
  g_volRend = &volRend;

  setCameraPosPreset(clo.cameraPos);

  //// NV Perf Thing ////
  perf_initNvPm();
  perf_initMode(clo.perfMode);

  loop(window);

  printNvPmApiCounters(clo.perfOutPath.c_str());
  cleanup();
  delete colormap;

  return 0;
}
