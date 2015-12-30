#include <GL/glew.h>
#include <GLFW/glfw3.h>

// local includes
#include "cmdline.h"
#include "create_vao.h"
#include "axis_enum.h"
#include "timing.h"

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
#include "volumerenderer.h"


///////////////////////////////////////////////////////////////////////////////
//  Const  Data
///////////////////////////////////////////////////////////////////////////////
const glm::vec3 X_AXIS{ 1.0f, 0.0f, 0.0f };
const glm::vec3 Y_AXIS{ 0.0f, 1.0f, 0.0f };
const glm::vec3 Z_AXIS{ 0.0f, 0.0f, 1.0f };


///////////////////////////////////////////////////////////////////////////////
// Geometry  /  VAOs
///////////////////////////////////////////////////////////////////////////////

/// \brief Enumerates the types of objects in the scene.
enum class ObjType : unsigned int {
    Axis, /*Quads,*/ Boxes
};



bd::CoordinateAxis g_axis; ///< The coordinate axis lines.
bd::Box g_box;

std::vector<bd::VertexArrayObject *> g_vaoArray;
size_t g_elementBufferSize{ 0 };



///////////////////////////////////////////////////////////////////////////////
// Shaders and Textures
///////////////////////////////////////////////////////////////////////////////
bd::ShaderProgram g_simpleShader;   ///< Shader for the wireframe stuff.
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

//SliceSet g_selectedSliceSet{ SliceSet::XY };

//TODO: bool g_toggleVolumeBox{ false };


///////////////////////////////////////////////////////////////////////////////
//  Miscellaneous  radness
///////////////////////////////////////////////////////////////////////////////

std::unique_ptr<VolumeRenderer> g_volRend{ nullptr };

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
                                    Texture &transferTex,
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


////////////////////////////////////////////////////////////////////////////////
void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode, int action,
                            int mods) {

  // on key press
  if (action == GLFW_PRESS) {
    switch (key) {
//        case GLFW_KEY_0:
//            g_selectedSliceSet = SliceSet::NoneOfEm;
//            break;
//        case GLFW_KEY_1:
//            g_selectedSliceSet = SliceSet::XY;
//            break;
//        case GLFW_KEY_2:
//            g_selectedSliceSet = SliceSet::XZ;
//            break;
//        case GLFW_KEY_3:
//            g_selectedSliceSet = SliceSet::YZ;
//            break;
//        case GLFW_KEY_4:
//            g_selectedSliceSet = SliceSet::AllOfEm;
//            break;
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

        //        cout << "Transfer function scaler: " << g_scaleValue << endl;
        break;
      case GLFW_KEY_COMMA:
        if (mods & GLFW_MOD_SHIFT)
          g_scaleValue -= 0.1f;
        else if (mods & GLFW_MOD_CONTROL)
          g_scaleValue -= 0.001f;
        else
          g_scaleValue -= 0.01f;

        //        cout << "Transfer function scaler: " << g_scaleValue << endl;
        break;
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
void glfw_window_size_callback(GLFWwindow *window, int width, int height) {
  g_screenWidth = width;
  g_screenHeight = height;
  g_camera.setViewport(0, 0, width, height);
}


////////////////////////////////////////////////////////////////////////////////
void glfw_cursorpos_callback(GLFWwindow *window, double x, double y) {
  glm::vec2 cpos(floor(x), floor(y));
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    glm::vec2 delta(cpos - g_cursorPos);
    setRotation(delta);
  }

  g_cursorPos = cpos;
}


///////////////////////////////////////////////////////////////////////////////
void glfw_scrollwheel_callback(GLFWwindow *window, double xoff, double yoff) {
  float fov = static_cast<float>(g_fov_deg + (yoff * 1.75f));

  if (fov < 1 || fov > 120) return;

  std::cout << "fov: " << fov << std::endl;

  g_fov_deg = fov;
  g_camera.setProjectionMatrix(glm::radians(fov),
                               g_screenWidth / float(g_screenHeight),
                               0.1f, 10000.0f);
}


/************************************************************************/
/*     D R A W I N'    S T U F F                                        */
/************************************************************************/


////////////////////////////////////////////////////////////////////////////////
void setRotation(const glm::vec2 &dr) {
  glm::quat rotX = glm::angleAxis<float>(
      glm::radians(-dr.y) * g_mouseSpeed,
      glm::vec3(1, 0, 0)
  );

  glm::quat rotY = glm::angleAxis<float>(
      glm::radians(dr.x) * g_mouseSpeed,
      glm::vec3(0, 1, 0)
  );

  g_camera.rotate(rotX * rotY);
}




/////////////////////////////////////////////////////////////////////////////////
//void drawNonEmptyBoundingBoxes(const glm::mat4 &mvp) {
//  for (auto *b : g_blockCollection.nonEmptyBlocks()) {
//    glm::mat4 mmvp = mvp * b->transform().matrix();
//    g_simpleShader.setUniform("mvp", mmvp);
//
//    gl_check(glDrawElements(GL_LINE_LOOP,
//                            4,
//                            GL_UNSIGNED_SHORT,
//                            (GLvoid *) 0));
//
//    gl_check(glDrawElements(GL_LINE_LOOP,
//                            4,
//                            GL_UNSIGNED_SHORT,
//                            (GLvoid *) (4 * sizeof(GLushort))));
//
//    gl_check(glDrawElements(GL_LINES,
//                            8,
//                            GL_UNSIGNED_SHORT,
//                            (GLvoid *) (8 * sizeof(GLushort))));
//  }
//}


///////////////////////////////////////////////////////////////////////////////
/// \brief Disable GL_DEPTH_TEST and draw transparent slices
///////////////////////////////////////////////////////////////////////////////
//void drawSlices(GLint baseVertex) {
//  gl_check(glDisable(GL_DEPTH_TEST));
//
//  perf_workBegin();
//  gl_check(glDrawElementsBaseVertex(GL_TRIANGLE_STRIP,
//                                    g_elementsPerQuad * g_numSlices,
//                                    GL_UNSIGNED_SHORT,
//                                    0,
//                                    baseVertex));
//  perf_workEnd();
//
//  gl_check(glEnable(GL_DEPTH_TEST));
//}


///////////////////////////////////////////////////////////////////////////////
/// \brief Loop through the blocks and draw each one
///////////////////////////////////////////////////////////////////////////////
//void drawNonEmptyBlocks_Forward(const std::vector<bd::Block*> &blocks,
//                                const glm::mat4 &vp) {
//  //glm::vec4 viewdir{ glm::normalize(g_camera.getViewMatrix()[2]) };
////  GLint baseVertex{ computeBaseVertexFromSliceSet(g_selectedSliceSet) };
//  GLint baseVertex{ 0 };
//
//  perf_frameBegin();
//  for (auto *b : blocks) {
//    b->texture().bind(0);
//    glm::mat4 wmvp = vp * b->transform().matrix();
//    g_volumeShader.setUniform("mvp", wmvp);
//    g_volumeShader.setUniform("tfScalingVal", g_scaleValue);
//    drawSlices(baseVertex);
//  }
//  perf_frameEnd();
//}

///////////////////////////////////////////////////////////////////////////////
/// \brief Determine the viewing direction and draw the blocks in proper order.
///////////////////////////////////////////////////////////////////////////////
//void drawNonEmptyBlocks(const glm::mat4 &vp) {
//
//  if (g_toggleWireFrame) {
//    gl_check(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
//  }
//
//  //TODO: sort quads farthest to nearest.
//  g_volumeShader.bind();
//  drawNonEmptyBlocks_Forward(g_blockCollection.nonEmptyBlocks(), vp);
//
//  if (g_toggleWireFrame) {
//    gl_check(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
//  }
//
//}


///////////////////////////////////////////////////////////////////////////////
void draw() {
  bd::VertexArrayObject *vao{ nullptr };

  glm::mat4 viewMat = g_camera.getViewMatrix();
  gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  ////////  Axis    /////////////////////////////////////////
  vao = g_vaoArray[bd::ordinal(ObjType::Axis)];
  vao->bind();
  g_simpleShader.bind();
  g_simpleShader.setUniform("mvp", viewMat);
  g_axis.draw();
  vao->unbind();

  ////////  BBoxes  /////////////////////////////////////////
  if (g_toggleBlockBoxes) {
//    vao = g_vaoArray[bd::ordinal(ObjType::Boxes)];
//    vao->bind();
    g_volRend->drawNonEmptyBoundingBoxes();
//    vao->unbind();
  }

  //////// Quad Geo (drawNonEmptyBlocks)  /////////////////////
//  vao = g_vaoArray[bd::ordinal(ObjType::Quads)];
//  vao->bind();
  g_volRend->drawNonEmptyBlocks();
//  vao->unbind();
}


///////////////////////////////////////////////////////////////////////////////
void loop(GLFWwindow *window) {
  assert(window != nullptr && "window was passed as nullptr in loop()");
  gl_log("About to enter render loop.");

  // initial bindage of tfunc texture to volume shader.
//  g_volumeShader.bind();
//  g_tfuncTex.bind(1);

  do {
    startCpuTime();
    g_camera.updateViewMatrix();

    startGpuTimerQuery();

    draw();
    glfwSwapBuffers(window);

    endGpuTimerQuery();

    glfwPollEvents();

    endCpuTime();

  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

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
             Axis::verts.size() * Axis::vert_element_size,
             Axis::vert_element_size,
             0); // attr 0

  // vertex colors into attribute 1
  vao.addVbo((float *) (Axis::colors.data()),
             Axis::colors.size() * 3,
             3,   // 3 floats per color
             1);  // attr 1
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Generate the vertex buffers for bounding box around the blocks
///////////////////////////////////////////////////////////////////////////////
void genBoxVao(bd::VertexArrayObject &vao) {
  gl_log("Generating bounding box vertex buffers.");

  // positions as vertex attribute 0
  vao.addVbo((float *) (bd::Box::vertices.data()),
             bd::Box::vertices.size() * bd::Box::vert_element_size,
             bd::Box::vert_element_size,
             0);

  // colors as vertex attribute 1
  vao.addVbo((float *) bd::Box::colors.data(),
             bd::Box::colors.size() * 3,
             3,
             1);

  vao.setIndexBuffer((unsigned short *) bd::Box::elements.data(),
                     bd::Box::elements.size());

}


///////////////////////////////////////////////////////////////////////////////
//   I N I T I A L I Z A T I O N
///////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
void initGraphicsState() {
  gl_log("Initializing gl state.");
  gl_check(glClearColor(1.0f, 1.0f, 1.0f, 0.0f));

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

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window =
      glfwCreateWindow(g_screenWidth,
                       g_screenHeight,
                       "Blocks",
                       nullptr,
                       nullptr);

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
  GLenum error = gl_check(glewInit());
  if (error) {
    gl_log("could not init glew %s", glewGetErrorString(error));
    return nullptr;
  }

  glfwSwapInterval(0);
  //bd::subscribe_debug_callbacks();

  genQueries();

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
  double gputime_ms = getTotalGPUTime_NonEmptyBlocks() * 1.0e-6;
  double cputime_ms = getTotalElapsedCPUFrameTime() * 1.0e-3;

  str <<
  "frames_rendered: " << getTotalFramesRendered() << " frames\n"
      "gpu_ft_total_nonempty: " << gputime_ms << "ms\n"
      "gpu_ft_avg_nonempty: " << (gputime_ms / getTotalFramesRendered()) << "ms\n"
      "cpu_ft_total: " << cputime_ms << "ms\n"
      "cpu_ft_avg: " << (cputime_ms / getTotalFramesRendered()) << "ms"
  << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Set an initial camera location/orientation (-c command line option)
///////////////////////////////////////////////////////////////////////////////
void setupCameraPos(unsigned cameraPos) {
  glm::quat r;
  switch (cameraPos) {
    case 2:
      //cam position = { 2.0f, 0.0f, 0.0f  }
      r = glm::rotate(r, -1 * glm::half_pi<float>(), Y_AXIS);
      g_camera.rotate(r);
//      g_selectedSliceSet = SliceSet::YZ;
      break;
    case 1:
      //cam position = { 0.0f, 2.0f, 0.0f }
      r = glm::rotate(r, glm::half_pi<float>(), X_AXIS);
      g_camera.rotate(r);
//      g_selectedSliceSet = SliceSet::XZ;
      break;
    case 0:
    default:
      //cam position = { 0.0f, 0.0f, 2.0f }
      // no rotation needed, this is default cam location.
//      g_selectedSliceSet = SliceSet::XY;
      break;
  }

  //g_viewDirty = true;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief print counters from NvPmApi to file \c perfOutPath, or to \c stdout
///  if no path is provided.
///////////////////////////////////////////////////////////////////////////////
void printNvPmApiCounters(const char *perfOutPath = "") {
  if (std::strlen(perfOutPath) == 0) {
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
  if (parseThem(argc, argv, clo) == 0) {
    std::cout << "No arguments provided.\nPlease use -h for usage info." << std::endl;
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


  //// Shaders Init ////
  GLuint programId{
      g_simpleShader.linkProgram(
          "shaders/vert_vertexcolor_passthrough.glsl",
          "shaders/frag_vertcolor.glsl") };

  if (programId == 0) {
    gl_log_err("Error building passthrough shader, program id was 0.");
    return 1;
  }

  bd::ShaderProgram volumeShader;
  GLuint volumeProgramId{
          volumeShader.linkProgram(
                  "shaders/vert_vertexcolor_passthrough.glsl",
                  "shaders/frag_volumesampler_noshading.glsl")
  };
  if (volumeProgramId == 0) {
    gl_log_err("Error building volume sampling shader, program id was 0.");
    return 1;
  }

  //// Transfer function texture ////
  Texture tfuncTex;
  unsigned int tfuncTextureId{
      loadTransfer_1dtformat(clo.tfuncPath, tfuncTex, volumeShader)
  };
  if (tfuncTextureId == 0) {
    gl_log_err("Exiting because tfunc texture was not bound.");
    exit(1);
  }

  // Initially bind the transfer function texture to the volume shader, currently
  // the tfunc texture doesn't change between frames, but the volume data
  // texture does change per block.
//  volumeShader.bind();
//  tfuncTex.bind(1);

  //// Geometry Init ////
  // 2d slices
  bd::VertexArrayObject quadVao;
  quadVao.create();
  genQuadVao(quadVao, {-0.5f,-0.5f,-0.5f}, {0.5f, 0.5f, 0.5f},
             {clo.num_slices, clo.num_slices, clo.num_slices});

  // coordinate axis
  bd::VertexArrayObject axisVao;
  axisVao.create();
  genAxisVao(axisVao);

  // bounding boxes
  bd::VertexArrayObject boxVao;
  boxVao.create();
  genBoxVao(boxVao);


  g_vaoArray.resize(2);
  g_vaoArray[bd::ordinal(ObjType::Axis)] = &axisVao;
//  g_vaoArray[bd::ordinal(ObjType::Quads)] = &quadVao;
  g_vaoArray[bd::ordinal(ObjType::Boxes)] = &boxVao;


  //// Blocks and Data Init ////
  bd::BlockCollection blockCollection;
  blockCollection.initBlocks(glm::u64vec3(clo.numblk_x, clo.numblk_y, clo.numblk_z),
                      glm::u64vec3(clo.w, clo.h, clo.d));

  std::unique_ptr<float[]> data{
      std::move(bd::readVolumeData(clo.type, clo.filePath, clo.w, clo.h, clo.d))
  };
  if (data == nullptr) {
    gl_log_err("data file was not opened. exiting...");
    cleanup();
    return 1;
  }

  blockCollection.filterBlocks(data.get(),
                        volumeShader.getUniformLocation("volume_sampler"),
                        clo.tmin, clo.tmax);

  if (clo.printBlocks) { printBlocks(&blockCollection); }

  //// Render Init ////
  setupCameraPos(clo.cameraPos);
  VolumeRenderer volRend{ std::make_shared(volumeShader),
                          std::make_shared(g_simpleShader), blockCollection,
    tfuncTex, ) };
  g_volRend = &volRend;



  initGraphicsState();

  //// NV Perf Thing ////
  perf_initNvPm();
  perf_initMode(clo.perfMode);

  loop(window);

  printNvPmApiCounters(clo.perfOutPath.c_str());
  cleanup();
  bd::gl_log_close();

  return 0;
}
