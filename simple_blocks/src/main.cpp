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
#include <bd/log/gl_log.h>
#include <bd/volume/block.h>
#include <bd/volume/blockcollection.h>
#include <bd/util/util.h>
#include <bd/util/ordinal.h>

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

/// \brief Enumerates the types of objects in the scene.
enum class ObjType : unsigned int {
  Axis, /*Quads,*/ Boxes
};

bd::CoordinateAxis g_axis; ///< The coordinate axis lines.
bd::Box g_box;

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

void setRotation(const glm::vec2 &dr);

void loop(GLFWwindow *window);

void cleanup();

unsigned int loadTransfer_1dtformat(const std::string &filename,
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
  gl_log_err("GLFW ERROR: code %i msg: %s", error, description);
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
      break;
    }
  }

  // while holding key down.
  if (action != GLFW_RELEASE) {
    switch (key) {
    case GLFW_KEY_PERIOD:
      if (mods & GLFW_MOD_SHIFT)
        g_scaleValue += 0.1f;
      else if (mods & GLFW_MOD_CONTROL)
        g_scaleValue += 0.001f;
      else
        g_scaleValue += 0.01f;

      g_volRend->setTfuncScaleValue(g_scaleValue);

      std::cout << "Transfer function scaler: " << g_scaleValue << std::endl;
      break;
    case GLFW_KEY_COMMA:
      if (mods & GLFW_MOD_SHIFT)
        g_scaleValue -= 0.1f;
      else if (mods & GLFW_MOD_CONTROL)
        g_scaleValue -= 0.001f;
      else
        g_scaleValue -= 0.01f;

      g_volRend->setTfuncScaleValue(g_scaleValue);

      std::cout << "Transfer function scaler: " << g_scaleValue << std::endl;
      break;
    }
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
  glm::vec2 cpos(floor(x), floor(y));
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS) {
    glm::vec2 delta(cpos - g_cursorPos);
    setRotation(delta);
  }

  g_cursorPos = cpos;
}


///////////////////////////////////////////////////////////////////////////////
void glfw_scrollwheel_callback(GLFWwindow *window, double xoff, double yoff) {
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
  gl_log("About to enter render loop.");

  do {
    startCpuTime();
    g_camera.updateViewMatrix();

    startGpuTimerQuery();

    draw();
    glfwSwapBuffers(window);

    endGpuTimerQuery();

    glfwPollEvents();

    endCpuTime();

  } while (glfwGetKey(window, GLFW_KEY_ESCAPE)!=GLFW_PRESS &&
      glfwWindowShouldClose(window)==0);

  gl_log("Render loop exited.");
}


///////////////////////////////////////////////////////////////////////////////
//  G E O M E T R Y   C R E A T I O N
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
void genAxisVao(bd::VertexArrayObject &vao) {
  gl_log("Generating axis vertex buffers.");

  using Axis = bd::CoordinateAxis;

  // vertex positions into attribute 0
  vao.addVbo((float *) (Axis::verts.data()),
             Axis::verts.size()*Axis::vert_element_size,
             Axis::vert_element_size,
             VERTEX_COORD_ATTR); // attr 0

  // vertex colors into attribute 1
  vao.addVbo((float *) (Axis::colors.data()),
             Axis::colors.size()*3,
             3,   // 3 floats per color
             VERTEX_COLOR_ATTR);  // attr 1
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Generate the vertex buffers for bounding box around the blocks
///////////////////////////////////////////////////////////////////////////////
void genBoxVao(bd::VertexArrayObject &vao) {
  gl_log("Generating bounding box vertex buffers.");

  // positions as vertex attribute 0
  vao.addVbo((float *) (bd::Box::vertices.data()),
             bd::Box::vertices.size()*bd::Box::vert_element_size,
             bd::Box::vert_element_size,
             VERTEX_COORD_ATTR);

  // colors as vertex attribute 1
  vao.addVbo((float *) bd::Box::colors.data(),
             bd::Box::colors.size()*3,
             3,
             VERTEX_COLOR_ATTR);

  vao.setIndexBuffer((unsigned short *) bd::Box::elements.data(),
                     bd::Box::elements.size());

}


///////////////////////////////////////////////////////////////////////////////
//   I N I T I A L I Z A T I O N
///////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
void initGraphicsState() {
  gl_log("Initializing gl state.");
  gl_check(glClearColor(0.15f, 0.15f, 0.15f, 0.0f));

//  gl_check(glEnable(GL_CULL_FACE));
//  gl_check(glCullFace(GL_FRONT));
  gl_check(glDisable(GL_CULL_FACE));

  gl_check(glEnable(GL_DEPTH_TEST));
  gl_check(glDepthFunc(GL_LESS));

  gl_check(glEnable(GL_BLEND));
  gl_check(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  gl_check(glEnable(GL_PRIMITIVE_RESTART));
  gl_check(glPrimitiveRestartIndex(0xFFFF));
}


/////////////////////////////////////////////////////////////////////////////////
GLFWwindow *init() {
  gl_log("Initializing GLFW.");
  if (!glfwInit()) {
    gl_log("could not start GLFW3");
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
    gl_log("ERROR: could not open window with GLFW3");
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
    gl_log("Could not init glew %s", glewGetErrorString(error));
    return nullptr;
  }

  glfwSwapInterval(0);

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
    gl_log("Writing blocks to blocks.txt in the current working directory.");
    for (auto &b : bcol->blocks()) {
      block_file << b << "\n";
    }
    block_file.flush();
    block_file.close();
  } else {
    gl_log_err("Could not print blocks because blocks.txt coulnt'd be created "
                   "in the current working directory.");
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
    r = glm::rotate(r, -1 * glm::half_pi<float>(), Y_AXIS);
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
      gl_log_err(
          "Could not open %s for performance counter output. Using stdout instead.",
          perfOutPath);
      perf_printCounters(std::cout);
      printTimes(std::cout);
    }
  }
}


/////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char *argv[]) {
  CommandLineOptions clo;
  if (parseThem(argc, argv, clo)==0) {
    std::cout << "No arguments provided.\nPlease use -h for usage info."
        << std::endl;
    return 1;
  }

  printThem(clo);
  bd::gl_log_restart();


  //// GLFW init ////
  GLFWwindow *window;
  if ((window = init()) == nullptr) {
    gl_log("Could not initialize GLFW, exiting.");
    return 1;
  }

  initGraphicsState();

  //// Shaders Init ////


  //// Geometry Init ////
  // 2d slices
  bd::VertexArrayObject *quadVao{ new bd::VertexArrayObject() };
  quadVao->create();
  //TODO: generate quads for actual volume extent.
  genQuadVao(*quadVao, { -0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f },
             { clo.num_slices, clo.num_slices, clo.num_slices });

  // coordinate axis
  bd::VertexArrayObject *axisVao{ new bd::VertexArrayObject() };
  axisVao->create();
  genAxisVao(*axisVao);

  g_axisVao = axisVao;

  // bounding boxes
  bd::VertexArrayObject *boxVao{ new bd::VertexArrayObject() };
  boxVao->create();
  genBoxVao(*boxVao);




  //// Blocks and Data Init ////
  bd::BlockCollection *blockCollection{ new bd::BlockCollection() };
  blockCollection->initBlocks(
      glm::u64vec3(clo.numblk_x, clo.numblk_y, clo.numblk_z),
      glm::u64vec3(clo.vol_w, clo.vol_h, clo.vol_d));

  std::unique_ptr<float[]> data{
      std::move(bd::readVolumeData(clo.type, clo.filePath, clo.vol_w, clo.vol_h, clo.vol_d))
  };

  if (data == nullptr) {
    gl_log_err("data file was not opened. exiting...");
    cleanup();
    return 1;
  }

  blockCollection->filterBlocks(data.get(), clo.tmin, clo.tmax);
//  data.release();

  if (clo.printBlocks) { printBlocks(blockCollection); }

  //// Render Init ////
  setCameraPosPreset(clo.cameraPos);


  //// Wireframe Shader ////
  bd::ShaderProgram *wireframeShader{ new bd::ShaderProgram() };

  GLuint wireframeProgramId{
      wireframeShader->linkProgram(
          "shaders/vert_vertexcolor_passthrough.glsl",
          "shaders/frag_vertcolor.glsl")
  };

  if (wireframeProgramId==0) {
    gl_log_err("Error building passthrough shader, program id was 0.");
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
    gl_log_err("Error building volume sampling shader, program id was 0.");
    return 1;
  }
 
  //// Transfer function texture ////
  bd::Texture *tfuncTex{ new bd::Texture(bd::Texture::Target::Tex1D) };

  unsigned int tfuncTextureId{
      loadTransfer_1dtformat(clo.tfuncPath, *tfuncTex, *volumeShader)
  };

  if (tfuncTextureId==0) {
    gl_log_err("Exiting because tfunc texture was not bound.");
    exit(1);
  }



  BlockRenderer volRend{ int(clo.num_slices), volumeShader, wireframeShader,
                         blockCollection, tfuncTex, quadVao, boxVao };

  volRend.setTfuncScaleValue(g_scaleValue);
  volRend.init();
  g_volRend = &volRend;


  //// NV Perf Thing ////
  perf_initNvPm();
  perf_initMode(clo.perfMode);

  loop(window);

  printNvPmApiCounters(clo.perfOutPath.c_str());
  cleanup();
  bd::gl_log_close();

  return 0;
}
