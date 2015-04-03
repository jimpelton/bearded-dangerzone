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
#include "blockscollection.h"

#include <geometry/BBox.h>
#include <geometry/quad.h>
#include <geometry/axis.h>
#include <file/datatypes.h>
#include <file/datareader.h>
#include <log/gl_log.h>
#include <util/util.h>
#include <util/shader.h>
#include <util/glfwcontext.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <array>
#include <memory>
#include <iostream>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////

const char *sub_quads_vertex_shader = "simplevr/shaders/sub-quads-vs.glsl";
const char *volume_fragment_shader = "simplevr/shaders/volume-frag.glsl";
const char *simple_vertex_shader = "simplevr/shaders/simple-vs.glsl";
const char *simple_color_fragment_shader = "simplevr/shaders/simple-color-frag.glsl";

CommandLineOptions g_opts;
bd::GlfwContext *g_context{ nullptr };
BlocksCollection *g_bcol{ nullptr };
bd::Axis g_axis;

///////////////////////////////////////////////////////////////////////////////


/// vertex shader ///
GLuint g_uniform_vp; // proj * view matrix
//GLuint  g_uniform_v;
//GLuint  g_uniform_p;
GLuint g_uniform_m; // model matrix
GLuint g_uniform_r; // rotation mat
GLuint g_uniform_vdir; // view dir vec
GLuint g_uniform_ds; // slice distance
GLuint g_volume_shader_Id;

/// fragment shader ///
GLuint g_uniform_color;
GLuint g_uniform_volume;
GLuint g_uniform_tf;

GLuint g_simple_shader_Id;
GLuint g_uniform_simple_mvp;

GLuint g_q_vaoId; // quad vertex array obj id
GLuint g_q_vboId; // quad geo vertex buffer id
GLuint g_q_iboId; // quad geo index buffer id
//GLuint g_box_vaoId; //
//GLuint g_box_vboId; //
//GLuint g_box_iboId; //
GLuint g_axis_vaoId;
GLuint g_axis_vboId[2];
GLuint g_axis_iboId;

glm::quat g_rotation;
glm::mat4 g_viewMatrix;
glm::mat4 g_projectionMatrix;
glm::mat4 g_vpMatrix;
glm::vec3 g_camPosition { 0.0f, 0.0f, -3.0f };
glm::vec3 g_camFocus { 0.0f, 0.0f, 0.0f };
glm::vec3 g_camUp { 0.0f, 1.0f, 0.0f };
glm::vec2 g_cursorPos;

//unsigned int NUMSLICES = 1u;
const glm::vec3 g_x_vdir { 1.0f, 0.0f, 0.0f };
const glm::vec3 g_y_vdir { 0.0f, 1.0f, 0.0f };
const glm::vec3 g_z_vdir { 0.0f, 0.0f, 1.0f };

float g_mouseSpeed = 1.0f;
unsigned int g_screenWidth { 1280 };
unsigned int g_screenHeight { 720 };
float g_fov { 50.0f };
bool g_viewDirty { false };

//bd::BBox g_bbox;


/////////////////////////////////////////////////////////////////////////////////////

void cleanup();
void usage();
void usage(const char *);

GLuint loadShader(GLenum type, std::string filepath);
GLuint compileShader(GLenum type, const char *shader);

void cursorpos_callback(double x, double y);
void keyboard_callback(int key, int scancode, int action, int mods);
void error_callback(int error, const char *description);
void window_size_callback(int width, int height);

void updateMvpMatrix();
void setRotation(const glm::vec2 &dr);
void drawBoundingBox(bd::Box *b, unsigned int vaoIdx);
void loop(bd::GlfwContext *);

void initBoxVbos(unsigned int vaoId);
void initQuadVbos(unsigned int vaoId);
void drawSlicesInstanced(unsigned int vaoId);

/////////////////////////////////////////////////////////////////////////////////////

/************************************************************************/
/*             C A L L B A C K S                                        */
/************************************************************************/

void keyboard_callback(int key, int scancode, int action, int mods) {}

void window_size_callback(int width, int height) {
    g_screenWidth = width;
    g_screenHeight = height;
    glViewport(0, 0, width, height);
}

void cursorpos_callback(double x, double y) {
    glm::vec2 cpos { floor(x), floor(y) };
    if (glfwGetMouseButton(g_context->window(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        glm::vec2 delta { cpos - g_cursorPos };
        setRotation(delta);
    }
    g_cursorPos = cpos;
}

void scrollwheel_callback(double xoff, double yoff) {
    g_camPosition += yoff;
    g_viewDirty = true;
}

/************************************************************************/
/*     D R A W I N'                                                     */
/************************************************************************/

void setRotation(const glm::vec2 &dr) {
    glm::quat rotX { glm::angleAxis<float>(
                         glm::radians(dr.y) * g_mouseSpeed,
                         glm::vec3(1, 0, 0)
                     ) };

    glm::quat rotY { glm::angleAxis<float>(
                         glm::radians(-dr.x) * g_mouseSpeed,
                         glm::vec3(0, 1, 0)
                     ) };

    g_rotation = (rotX * rotY) * g_rotation;

    g_viewDirty = true;
}

void updateMvpMatrix() {
    g_viewMatrix = glm::lookAt(g_camPosition, g_camFocus, g_camUp);

    g_projectionMatrix = glm::perspective(g_fov,
                                          static_cast<float>(g_screenWidth) / g_screenHeight,
                                          0.1f, 100.0f);

    g_vpMatrix = g_projectionMatrix * g_viewMatrix;
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

void drawAxisElements(unsigned int vaoId)
{
    glBindVertexArray(vaoId);
    glUseProgram(g_simple_shader_Id);
    
    glUniformMatrix4fv(g_uniform_simple_mvp, 1, GL_FALSE, 
        glm::value_ptr(g_vpMatrix * glm::toMat4(g_rotation)));

    glDrawArrays(GL_LINES, 0, bd::Axis::verts.size());

    glBindVertexArray(0);
}

void drawSlicesInstanced(unsigned int vaoId) {
    glBindVertexArray(vaoId);

    glUseProgram(g_volume_shader_Id);
    if (g_viewDirty) {
        updateMvpMatrix();
        glUniformMatrix4fv(g_uniform_vp, 1, GL_FALSE, glm::value_ptr(g_vpMatrix)); // vp
    }

    glUniformMatrix4fv(g_uniform_r, 1, GL_FALSE, glm::value_ptr(glm::toMat4(g_rotation))); // r

    for (const Block &b : g_bcol->blocks()) {
        if (b.empty()) continue;

        glUniformMatrix4fv(g_uniform_m, 1, GL_FALSE,
                           glm::value_ptr(b.cQuad().translate() * b.cQuad().scale()));

        glUniform3fv(g_uniform_color, 1, glm::value_ptr(b.cQuad().cColor()));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, b.texid());
        glUniform1i(g_uniform_volume, 0);

        glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0, g_opts.num_slices);
    }

    glBindVertexArray(0);
}

void loop(bd::GlfwContext *context) {
    GLFWwindow *window = context->window();
    g_viewDirty = true;

    float ds { 1.0f / g_bcol->volume().numBlocks().x / g_opts.num_slices };

    glUseProgram(g_volume_shader_Id);
    glUniform3fv(g_uniform_vdir, 1, glm::value_ptr(g_z_vdir)); // vdir
    glUniform1f(g_uniform_ds, ds); // ds

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawSlicesInstanced(g_q_vaoId);
        drawAxisElements(g_axis_vaoId);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);
}

/************************************************************************/
/*     I N I T I A L I Z A T I O N                                      */
/************************************************************************/

//void initBoxVbos(unsigned int vaoId) {
//    using bd::geometry::BBox;
//    glBindVertexArray(vaoId);
//
//    glGenBuffers(1, &g_box_vboId);
//    glBindBuffer(GL_ARRAY_BUFFER, g_box_vboId);
//    glBufferData(GL_ARRAY_BUFFER,
//                 BBox::vertices.size() * sizeof(decltype(BBox::vertices[0])),
//                 BBox::vertices.data(),
//                 GL_STATIC_DRAW);
//    const unsigned vertex_coord_attr = 0;
//    glEnableVertexAttribArray(vertex_coord_attr);
//    glVertexAttribPointer(vertex_coord_attr, // attribute
//                          BBox::vert_element_size, // number of elements per vertex, here (x,y,z,w)
//                          GL_FLOAT, // the type of each element
//                          GL_FALSE, // take our values as-is
//                          0, // no extra data between each position
//                          0 // offset of first element
//                         );
//
//    glGenBuffers(1, &g_box_iboId);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_box_iboId);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//                 BBox::elements.size() * sizeof(decltype(BBox::elements[0])),
//                 BBox::elements.data(),
//                 GL_STATIC_DRAW);
//}

void initQuadVbos(unsigned int vaoId) {
    using bd::Quad;
    glBindVertexArray(vaoId);

    glGenBuffers(1, &g_q_vboId);
    glBindBuffer(GL_ARRAY_BUFFER, g_q_vboId);
    glBufferData(GL_ARRAY_BUFFER,
                 Quad::verts.size() * sizeof(decltype(Quad::verts[0])),
                 Quad::verts.data(),
                 GL_STATIC_DRAW);
    const unsigned vertex_coord_attr = 0;
    glEnableVertexAttribArray(vertex_coord_attr);
    glVertexAttribPointer(vertex_coord_attr, // attribute
                          Quad::vert_element_size, // number of elements per vertex, here (x,y,z,w)
                          GL_FLOAT, // the type of each element
                          GL_FALSE, // take our values as-is
                          0, // no extra data between each position
                          0 // offset of first element
                         );



    glGenBuffers(1, &g_q_iboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_q_iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 Quad::elements.size() * sizeof(decltype(Quad::elements[0])),
                 Quad::elements.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void initAxisVbos(unsigned int vaoId) {
    using bd::Axis;
    glBindVertexArray(vaoId);

    glGenBuffers(2, g_axis_vboId);
    glBindBuffer(GL_ARRAY_BUFFER, g_axis_vboId[0]);
    glBufferData(GL_ARRAY_BUFFER,
                 Axis::verts.size() * sizeof(decltype(Axis::verts[0])),
                 Axis::verts.data(),
                 GL_STATIC_DRAW);
    const unsigned vertex_coord_attr = 0;
    glEnableVertexAttribArray(vertex_coord_attr);
    glVertexAttribPointer(vertex_coord_attr, // attribute
                          Axis::vert_element_size, // number of elements per vertex, here (x,y,z,w)
                          GL_FLOAT, // the type of each element
                          GL_FALSE, // take our values as-is
                          0, // no extra data between each position
                          0); // offset of first element
    

    glBindBuffer(GL_ARRAY_BUFFER, g_axis_vboId[1]);
    glBufferData(GL_ARRAY_BUFFER, 
        Axis::colors.size() * sizeof(decltype(Axis::colors[0])),
        Axis::colors.data(), 
        GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);


    //glGenBuffers(1, &g_axis_iboId);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_axis_iboId);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    //             Axis::elements.size() * sizeof(decltype(Axis::elements[0])),
    //             Axis::elements.data(),
    //             GL_STATIC_DRAW);

    glBindVertexArray(0);
}

float *readData(const std::string &dtype, const std::string &fname,
                size_t x, size_t y, size_t z) {
    using bd::DataType;
    DataType t = bd::DataTypesMap.at(dtype);
    float *rval = nullptr;
    switch (t) {
    case DataType::Float: 
    {
        bd::DataReader<float, float> reader;
        reader.loadRaw3d(g_opts.filePath, g_opts.w, g_opts.h, g_opts.d);
        rval = reader.takeOwnership();
        break;
    }
    case DataType::UnsignedShort:
    {
        bd::DataReader<unsigned short, float> reader;
        reader.loadRaw3d(g_opts.filePath, g_opts.w, g_opts.h, g_opts.d);
        rval = reader.takeOwnership();
        break;
    }
    default:
        break;
    }

    return rval;
}

void cleanup() {
    //if (g_q_vboId > 0) {
    //    glDeleteBuffers(1, &g_q_vboId);
    //}

    //TODO: cleanup buffers
    bd::gl_log_close();
}

int main(int argc, char *argv[]) {
    bd::gl_log_restart();
    bd::gl_debug_log_restart();

    if (parseThem(argc, argv, g_opts) == 0) {
        gl_log_err("Check command line arguments... Exiting.");
        cleanup();
        exit(1);
    }

    bd::GlfwContext context;
    g_context = &context;

    if (context.init(1280, 720) == nullptr) {
        gl_log("Could not initialize GLFW, exiting.");
        exit(1);
    }

    context.setCursorPosCallback(cursorpos_callback);
    context.setKeyboardCallback(keyboard_callback);
    context.setScrollWheelCallback(scrollwheel_callback);
    context.setWindowSizeCallback(window_size_callback);

    std::unique_ptr<float []> data {
        readData(g_opts.type, g_opts.filePath, g_opts.w, g_opts.h, g_opts.d)
    };

    if (data == nullptr) {
        gl_log_err("Nope! Did not read that data right! Exiting...");
        exit(1);
    }

    size_t voxels { g_opts.w *g_opts.h * g_opts.d };
    int voxelsPerBlock { g_opts.block_side *g_opts.block_side * g_opts.block_side };
    size_t numblocks { voxels / voxelsPerBlock };

    Volume v {
        { 1.0, 1.0, 1.0 },
        { g_opts.w, g_opts.h, g_opts.d },
        { g_opts.w / g_opts.block_side, g_opts.h / g_opts.block_side, g_opts.d / g_opts.block_side }
    };

    BlocksCollection bcol { data, &v };
    bcol.initBlocks();
    bcol.avgblocks();
    bcol.createNonEmptyTextures();
    bcol.printblocks();

    g_bcol = &bcol;

    glGenVertexArrays(1, &g_axis_vaoId);
    initAxisVbos(g_axis_vaoId);
    
    glGenVertexArrays(1, &g_q_vaoId);
    initQuadVbos(g_q_vaoId);

    GLuint sq_vsId = bd::loadShader(GL_VERTEX_SHADER, sub_quads_vertex_shader);
    if (sq_vsId == 0) {
        gl_log_err("Sub-quads vertex shader failed to compile. Exiting...");
        cleanup();
        exit(1);
    }

    GLuint vol_fsId = bd::loadShader(GL_FRAGMENT_SHADER, volume_fragment_shader);
    if (vol_fsId == 0) {
        gl_log_err("Volume fragment shader failed to compile. Exiting...");
        cleanup();
        exit(1);
    }

    g_volume_shader_Id = bd::linkProgram({ sq_vsId, vol_fsId });
    if (g_volume_shader_Id == 0) {
        gl_log_err("Sub-quads+volume shader program failed to link. Exiting...");
        cleanup();
        exit(1);
    }

    GLuint simple_vsId = bd::loadShader(GL_VERTEX_SHADER, simple_vertex_shader);
    if (simple_vsId == 0) {
        gl_log_err("Simple vertex shader failed to compile. Exiting...");
        cleanup();
        exit(1);
    }

    GLuint color_fsId = bd::loadShader(GL_FRAGMENT_SHADER, simple_color_fragment_shader);
    if (color_fsId == 0) {
        gl_log_err("Simple-color fragment shader failed to compile. Exiting...");
        cleanup();
        exit(1);
    }

    g_simple_shader_Id = bd::linkProgram({ simple_vsId, color_fsId });
    if (g_simple_shader_Id == 0) {
        gl_log_err("Simple-vertex+simple-color shader program failed to link. Exiting...");
        cleanup();
        exit(1);
    }

    /// vertex shader ///
    g_uniform_simple_mvp = glGetUniformLocation(g_simple_shader_Id, "mvp");

    g_uniform_vp = glGetUniformLocation(g_volume_shader_Id, "vp");
    g_uniform_m = glGetUniformLocation(g_volume_shader_Id, "m");
    g_uniform_vdir = glGetUniformLocation(g_volume_shader_Id, "vdir");
    g_uniform_r = glGetUniformLocation(g_volume_shader_Id, "r");
    g_uniform_ds = glGetUniformLocation(g_volume_shader_Id, "ds");

    /// fragment shader ///
    g_uniform_color = glGetUniformLocation(g_volume_shader_Id, "color");
    g_uniform_volume = glGetUniformLocation(g_volume_shader_Id, "volume");
    g_uniform_tf = glGetUniformLocation(g_volume_shader_Id, "tf");


    loop(&context);
    cleanup();

    glfwTerminate();

    return 0;
}
