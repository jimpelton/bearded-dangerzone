//************************************************************//
// Jim Pelton
// Graduate Student
// Dept. of Computer Science
// Boise State University
// Spring 2015
//
// Simplevr -- Simple slice-based 3D texture volume renderer
//************************************************************//


#include "cmdline.h"
#include "block.h"

#include <geometry/BBox.h>
#include <geometry/quad.h>
#include <geometry/axis.h>
#include <file/datatypes.h>
#include <file/datareader.h>
#include <log/gl_log.h>
#include <util/util.h>
#include <util/shader.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vector>
#include <array>

#include <iostream>
#include <sstream>

namespace bd = bearded::dangerzone;


///////////////////////////////////////////////////////////////////////////////

const char* vertex_shader = "simplevr/shaders/sub-quads-vs.glsl";
const char* fragment_shader = "simplevr/shaders/simple-color-frag.glsl";

CommandLineOptions g_opts;

///////////////////////////////////////////////////////////////////////////////

//GLuint  g_uniform_vp;     // proj * view matrix
GLuint  g_uniform_color;
GLuint  g_uniform_v;
GLuint  g_uniform_p;
GLuint  g_uniform_m;            // model matrix
GLuint  g_uniform_rot;          // rotation mat
GLuint  g_uniform_vdir;         // view dir vec
GLuint  g_uniform_st;           // start
GLuint  g_uniform_ds;           // slice distance
GLuint  g_shaderProgramId;

GLuint g_q_vaoId;     // quad vertex array obj id
GLuint g_q_vboId;     // quad geo vertex buffer id
GLuint g_q_iboId;     // quad geo index buffer id
GLuint g_box_vaoId;   //
GLuint g_box_vboId;   //
GLuint g_box_iboId;   //
GLuint g_axis_vboId;
GLuint g_axis_iboId;


glm::quat g_rotation;
glm::mat4 g_viewMatrix;
glm::mat4 g_projectionMatrix;
// glm::mat4 g_vpMatrix;
glm::vec3 g_camPosition(0.0f, 0.0f, -3.0f);
glm::vec3 g_camFocus(0.0f, 0.0f, 0.0f);
glm::vec3 g_camUp(0.0f, 1.0f, 0.0f);
glm::vec2 g_cursorPos;

const unsigned int NUMBLOCKS = 5u;    // blocks per volume dimension
const unsigned int NUMSLICES = 8u;
const glm::vec3 g_x_vdir(1.0f, 0.0f, 0.0f);
const glm::vec3 g_y_vdir(0.0f, 1.0f, 0.0f);
const glm::vec3 g_z_vdir(0.0f, 0.0f, 1.0f);

float g_mouseSpeed = 1.0f;
float g_screenWidth = 1280.0f;
float g_screenHeight = 720.0f;
float g_fov = 50.0f;
bool  g_viewDirty = false;

bd::geometry::BBox g_bbox;
//std::vector<bd::geometry::Quad> theQuads;
std::vector<Block> *g_blocks = nullptr;

/////////////////////////////////////////////////////////////////////////////////////

void cleanup();
void usage();
void usage(const char *);

GLuint loadShader(GLenum type, std::string filepath);
GLuint compileShader(GLenum type, const char *shader);

void glfw_cursorpos_callback(GLFWwindow *window, double x, double y);
void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void glfw_error_callback(int error, const char* description);
void glfw_window_size_callback(GLFWwindow *window, int width, int height);

void updateMvpMatrix();
void setRotation(const glm::vec2 &dr);
void drawBoundingBox(bd::geometry::BBox *b, unsigned int vaoIdx);
void loop(GLFWwindow *window);

void initBoxVbos(unsigned int vaoId);
void initQuadVbos(unsigned int vaoId);
void drawSlicesInstanced(unsigned int vaoId);



/////////////////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* G L F W     C A L L B A C K S                                        */
/************************************************************************/
void glfw_error_callback(int error, const char* description)
{
    gl_log("GLFW ERROR: code %i msg: %s", error, description);
}

void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{

}

void glfw_window_size_callback(GLFWwindow *window, int width, int height)
{
    g_screenWidth = width;
    g_screenHeight = height;
    glViewport(0, 0, width, height);
}

void glfw_cursorpos_callback(GLFWwindow *window, double x, double y)
{
    glm::vec2 cpos(floor(x), floor(y));
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        glm::vec2 delta(cpos - g_cursorPos);
        setRotation(delta);
    }
    g_cursorPos = cpos;
}

void glfw_scrollwhell_callback(GLFWwindow *window, double xoff, double yoff)
{
    g_camPosition += yoff;
    g_viewDirty = true;
}

/************************************************************************/
/*     D R A W I N'                                                     */
/************************************************************************/

void setRotation(const glm::vec2 &dr)
{
    glm::quat rotX = glm::angleAxis<float>(
        glm::radians(dr.y) * g_mouseSpeed,
        glm::vec3(1, 0, 0)
        );

    glm::quat rotY = glm::angleAxis<float>(
        glm::radians(-dr.x) * g_mouseSpeed,
        glm::vec3(0, 1, 0)
        );

    g_rotation = (rotX * rotY) * g_rotation;

    g_viewDirty = true;
}

void updateMvpMatrix()
{
    g_viewMatrix = glm::lookAt(g_camPosition, g_camFocus, g_camUp);
    g_projectionMatrix = glm::perspective(g_fov, g_screenWidth/g_screenHeight, 0.1f, 100.0f);
    g_viewDirty = false;
}

// void drawBoundingBox(bd::geometry::BBox *b, unsigned int vaoIdx)
// {
//     glm::mat4 mvp = g_vpMatrix * b->modelTransform() * glm::toMat4(g_rotation);
// 
//     glUseProgram(g_shaderProgramId);
//     glUniformMatrix4fv(g_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
//     glBindVertexArray(g_vaoIds[vaoIdx]);
//     glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
//     glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
//     glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
//     glBindVertexArray(0);
// }

//bd::geometry::Axis g_axis;
//void drawAxisElements(unsigned int vaoId)
//{
//    glBindVertexArray(vaoId);
//
//    glUseProgram(g_shaderProgramId);
//
//    glBindVertexArray(0);
//}

void drawSlicesInstanced(unsigned int vaoId) 
{
    glBindVertexArray(vaoId);

    glUseProgram(g_shaderProgramId);
  
    for (auto &b : *g_blocks) {
        if (b.isEmpty()) continue;
        glUniformMatrix4fv(g_uniform_m, 1, GL_FALSE, glm::value_ptr(b.quad().model()));
        glUniform3fv(g_uniform_color, 1, glm::value_ptr(b.quad().color()));
        //glDrawElementsInstanced(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0, NUMSLICES);
        glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0, NUMSLICES);
    }

//     for (auto &q : theQuads) {
//         glUniformMatrix4fv(g_uniform_m, 1, GL_FALSE, glm::value_ptr(q.model()));
//         glUniform3fv(g_uniform_color, 1, glm::value_ptr(q.color()));
//         glDrawElementsInstanced(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0, NUMSLICES);
//         //glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0, NUMSLICES);
//     }

    glBindVertexArray(0);
}

void loop(GLFWwindow *window)
{
    g_viewDirty = true;
    float db = 1.0f / NUMBLOCKS;
    float ds = db / NUMSLICES;
    float start = -0.5f + ds; //-1.0f * (ds * (NUMSLICES/2));

    glUseProgram(g_shaderProgramId);
    glUniform3fv(g_uniform_vdir, 1, glm::value_ptr(g_z_vdir));         // vdir
    glUniform1f(g_uniform_st, start);                                  // st
    glUniform1f(g_uniform_ds, ds);                                     // ds

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (g_viewDirty) {
            updateMvpMatrix();
            glUniformMatrix4fv(g_uniform_p, 1, GL_FALSE, glm::value_ptr(g_projectionMatrix));   // p
            glUniformMatrix4fv(g_uniform_v, 1, GL_FALSE, glm::value_ptr(g_viewMatrix));         // v
        }
        
        glUniformMatrix4fv(g_uniform_rot, 1, GL_FALSE, glm::value_ptr(glm::toMat4(g_rotation)));   // rot
        
        drawSlicesInstanced(g_q_vaoId);

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);
}

/************************************************************************/
/*     I N I T I A L I Z A T I O N                                      */
/************************************************************************/

GLFWwindow* init()
{
    GLFWwindow *window = nullptr;
    if (!glfwInit()) {
        gl_log_err("could not start GLFW3");
        return nullptr;
    }

    glfwSetErrorCallback(glfw_error_callback);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    // number of samples to use for multi sampling
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 720, "Minimal", NULL, NULL);
    if (!window) {
        gl_log_err("ERROR: could not open window with GLFW3");
        glfwTerminate();
        return nullptr;
    }

    glfwSetCursorPosCallback(window, glfw_cursorpos_callback);
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);
    glfwSetScrollCallback(window, glfw_scrollwhell_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum error = glewInit();
    if (error) {
        gl_log_err("could not init glew %s", glewGetErrorString(error));
        return nullptr;
    }

    glDebugMessageCallback((GLDEBUGPROC)gl_debug_message_callback, NULL);
    glEnable(GL_DEBUG_OUTPUT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

    return window;
}

void initBoxVbos(unsigned int vaoId) 
{
    glBindVertexArray(vaoId);

    glGenBuffers(1, &g_box_vboId);
    glBindBuffer(GL_ARRAY_BUFFER, g_box_vboId);
    glBufferData(
            GL_ARRAY_BUFFER,
            bd::geometry::BBox::vertices.size() * sizeof(decltype(bd::geometry::BBox::vertices[0])),
            bd::geometry::BBox::vertices.data(),
            GL_STATIC_DRAW);
    const unsigned vertex_coord_attr = 0;
    glEnableVertexAttribArray(vertex_coord_attr);
    glVertexAttribPointer(
            vertex_coord_attr,  // attribute
            bd::geometry::BBox::vert_element_size,                  // number of elements per vertex, here (x,y,z,w)
            GL_FLOAT,           // the type of each element
            GL_FALSE,           // take our values as-is
            0,                  // no extra data between each position
            0                   // offset of first element
    );

    glGenBuffers(1, &g_box_iboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_box_iboId);
    glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            bd::geometry::BBox::elements.size() * sizeof(decltype(bd::geometry::BBox::elements[0])),
            bd::geometry::BBox::elements.data(),
            GL_STATIC_DRAW);
}

void initQuadVbos(unsigned int vaoId) 
{
    using bd::geometry::Quad;
    glBindVertexArray(vaoId);

    glGenBuffers(1, &g_q_vboId);
    glBindBuffer(GL_ARRAY_BUFFER, g_q_vboId);
    glBufferData(
            GL_ARRAY_BUFFER,
            Quad::verts.size() * sizeof(decltype(Quad::verts[0])),
            Quad::verts.data(),
            GL_STATIC_DRAW);
    const unsigned vertex_coord_attr = 0;
    glEnableVertexAttribArray(vertex_coord_attr);
    glVertexAttribPointer(
            vertex_coord_attr,  // attribute
            bd::geometry::Quad::vert_element_size,                  // number of elements per vertex, here (x,y,z,w)
            GL_FLOAT,           // the type of each element
            GL_FALSE,           // take our values as-is
            0,                  // no extra data between each position
            0                   // offset of first element
    );

    glGenBuffers(1, &g_q_iboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_q_iboId);
    glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            Quad::elements.size() * sizeof(decltype(Quad::elements[0])),
            Quad::elements.data(),
            GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void initAxisVbos(unsigned int vaoId)
{
    using bd::geometry::Axis;
    glBindVertexArray(vaoId);

    glGenBuffers(1, &g_axis_vboId);
    glBindBuffer(GL_ARRAY_BUFFER, g_axis_vboId);
    glBufferData(
        GL_ARRAY_BUFFER,
        Axis::verts.size() * sizeof(decltype(Axis::verts[0])),
        Axis::verts.data(),
        GL_STATIC_DRAW);
    const unsigned vertex_coord_attr = 0;
    glEnableVertexAttribArray(vertex_coord_attr);
    glVertexAttribPointer(
        vertex_coord_attr,  // attribute
        bd::geometry::Axis::vert_element_size,  // number of elements per vertex, here (x,y,z,w)
        GL_FLOAT,           // the type of each element
        GL_FALSE,           // take our values as-is
        0,                  // no extra data between each position
        0                   // offset of first element
        );

    glGenBuffers(1, &g_axis_iboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_axis_iboId);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        Axis::elements.size() * sizeof(decltype(Axis::elements[0])),
        Axis::elements.data(),
        GL_STATIC_DRAW);

    glBindVertexArray(0);
}

float* readData(const std::string &dtype, const std::string &fname, 
    size_t x, size_t y, size_t z)
{
    using namespace bd::file;
    DataType t = bd::file::DataTypesMap.at(dtype);
    float *rval = nullptr;
    switch (t) {
    case DataType::Float:
    {
        DataReader<float, float> reader;
        reader.loadRaw3d(g_opts.filePath, g_opts.w, g_opts.h, g_opts.d);
        rval = reader.takeOwnership();
        break;
    }
    default:
        break;
    }

    return rval;
}

//  void makeQuadsAndColors() {
//      //const float bounds_min = -0.5f;
//      //const float bounds_max =  0.5f;
//      const float side = 1.0f / NUMBLOCKS;
//      const glm::vec2 dims(side, side);
//      const int huemax = 360;
//      const int dh = huemax / (NUMBLOCKS*NUMBLOCKS*NUMBLOCKS);
//      
//      glm::vec3 color;
//      int hue = 0;
//      
//      for (int z = 0; z < NUMBLOCKS; ++z) 
//      for (int y = 0; y < NUMBLOCKS; ++y) 
//      for (int x = 0; x < NUMBLOCKS; ++x) {
//          glm::vec3 c(side*x, side*y, side*z);
//          c += -0.5f;
//          bd::util::hsvToRgb(hue, 1.0f, 1.0f, color);
//          hue += dh;
//          theQuads.push_back({ c, dims, color });
//       }
//  }

void cleanup()
{
    if (g_q_vboId > 0) {
        glDeleteBuffers(1, &g_q_vboId);
    }

    gl_log_close();
//    std::vector<GLuint> bufIds;
//    for (unsigned i=0; i<NUMBOXES; ++i) {
//        bufIds.push_back(g_bbox[i].iboId());
//        bufIds.push_back(g_bbox[i].vboId());
//    }
//
//    glDeleteBuffers(NUMBOXES, &bufIds[0]);
//    glDeleteProgram(g_shaderProgramId);
}

int main(int argc, char* argv[])
{
    gl_log_restart();
    gl_debug_log_restart();

    if (parseThem(argc, argv, g_opts) == 0) {
        gl_log_err("Check command line arguments... Exiting.");
        cleanup();
        exit(1);
    }

    GLFWwindow * window;
    if ((window = init()) == nullptr) {
        gl_log("Could not initialize GLFW, exiting.");
        exit(1);
    }

    float *data = readData(g_opts.type, g_opts.filePath, g_opts.w, g_opts.h, g_opts.d);
    if( data == nullptr ) {
        gl_log_err("Nope! Did not read that data right! Exiting...");
        exit(1);
    }

    int voxels = g_opts.w * g_opts.h * g_opts.d;
    int voxelsPerBlock = g_opts.block_side * g_opts.block_side * g_opts.block_side;
    int numblocks = voxels / voxelsPerBlock;
    std::vector<Block> blocks(numblocks);

    Block::initBlocks(blocks, 
        g_opts.w, g_opts.h, g_opts.d,
        g_opts.block_side, g_opts.block_side, g_opts.block_side);
    Block::avgblocks(blocks, voxelsPerBlock);
    Block::printblocks(blocks);
    g_blocks = &blocks;

    glGenVertexArrays(1, &g_q_vaoId);
    initQuadVbos(g_q_vaoId);

    //makeQuadsAndColors();
    
    GLuint vsId = bd::util::loadShader(GL_VERTEX_SHADER, vertex_shader);
    if (vsId == 0) {
        gl_log_err("Vertex shader failed to compile. Exiting...");
        cleanup();
        exit(1);
    }

    GLuint fsId = bd::util::loadShader(GL_FRAGMENT_SHADER, fragment_shader);
    if (fsId == 0) {
        gl_log_err("Fragment shader failed to compile. Exiting...");
        cleanup();
        exit(1);
    }

    g_shaderProgramId = bd::util::linkProgram({ vsId, fsId });
    if (g_shaderProgramId == 0) {
        gl_log_err("Shader program failed to link. Exiting...");
        cleanup();
        exit(1);
    }

    //g_uniform_vp   = glGetUniformLocation(g_shaderProgramId, "vp");
    g_uniform_color = glGetUniformLocation(g_shaderProgramId, "in_col");
    g_uniform_p = glGetUniformLocation(g_shaderProgramId, "p");
    g_uniform_v = glGetUniformLocation(g_shaderProgramId, "v");
    g_uniform_m = glGetUniformLocation(g_shaderProgramId, "m");
    //g_uniform_scale = glGetUniformLocation(g_shaderProgramId, "scale");
    g_uniform_vdir = glGetUniformLocation(g_shaderProgramId, "vdir");
    g_uniform_rot = glGetUniformLocation(g_shaderProgramId, "rot");
    g_uniform_st = glGetUniformLocation(g_shaderProgramId, "st");
    g_uniform_ds = glGetUniformLocation(g_shaderProgramId, "ds");
    
    loop(window);
    cleanup();

    if (data != nullptr) {
        delete [] data;
    }

    gl_log_close();
    //closeLog();
    glfwTerminate();

    return 0;
}

