#include <bd/log/gl_log.h>
#include <bd/graphics/vertexarrayobject.h>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <array>

#include <sstream>
#include <fstream>

#include <cstdarg>
#include <ctime>

const char *vertex_shader =
    "#version 400\n"
        "in vec3 vp;"
        "in vec3 in_col;"
        "uniform mat4 mvp;"
        "out vec3 vcol;"
        "void main () {"
        "  gl_Position = mvp * vec4(vp, 1.0);"
        "  vcol = in_col;"
        "}";

const char *fragment_shader =
    "#version 400\n"
        "in vec3 vcol;"
        "out vec4 frag_colour;"
        "void main () {"
        "  frag_colour = vec4(vcol, 1.0);"
        "}";

//const unsigned int NUMBOXES{ 1 };

const glm::vec3 X_AXIS{ 1.0f, 0.0f, 0.0f };
const glm::vec3 Y_AXIS{ 0.0f, 1.0f, 0.0f };
const glm::vec3 Z_AXIS{ 0.0f, 0.0f, 1.0f };

GLint g_uniform_mvp;
GLuint g_shaderProgramId;
std::vector<bd::VertexArrayObject *> g_vaoIds;

glm::quat g_rotation;
glm::mat4 g_modelMatrix{ 1.0 };
glm::mat4 g_viewMatrix;
glm::mat4 g_projectionMatrix;
glm::mat4 g_vpMatrix;
glm::vec3 g_camPosition{ 0.0f, 0.0f, -10.0f };
glm::vec3 g_camFocus{ 0.0f, 0.0f, 0.0f };
glm::vec3 g_camUp{ 0.0f, 1.0f, 0.0f };
glm::vec2 g_cursorPos;

float g_mouseSpeed{ 1.0f };
float g_screenWidth{ 1280.0f };
float g_screenHeight{ 720.0f };
float g_fov{ 50.0f };
bool g_viewDirty{ true };

void cleanup();

GLuint loadShader(GLenum type, std::string filepath);

GLuint compileShader(GLenum type, const char *shader);

void glfw_cursorpos_callback(GLFWwindow *window, double x, double y);

void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode, int action,
    int mods);

void glfw_error_callback(int error, const char *description);

void glfw_window_size_callback(GLFWwindow *window, int width, int height);

void updateMvpMatrix();

void setRotation(const glm::vec2 &dr);

//void drawBoundingBox();
void loop(GLFWwindow *window);


/************************************************************************/
/* G L F W     C A L L B A C K S                                        */
/************************************************************************/
void glfw_error_callback(int error, const char *description)
{
    gl_log_err("GLFW ERROR: code %i msg: %s", error, description);
}

void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode, int action,
    int mods)
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

/************************************************************************/
/* S H A D E R   C O M P I L I N G                                      */
/************************************************************************/
GLuint loadShader(GLenum type, std::string filepath)
{
    GLuint shaderId = 0;
    std::ifstream file(filepath.c_str());
    if (!file.is_open()) {
        gl_log("Couldn't open %s", filepath.c_str());
        return 0;
    }

    std::stringstream shaderCode;
    shaderCode << file.rdbuf();

    std::string code = shaderCode.str();
    const char *ptrCode = code.c_str();
    file.close();

    gl_log("Compiling shader: %s", filepath.c_str());
    shaderId = compileShader(type, ptrCode);

    return shaderId;
}

GLuint compileShader(GLenum type, const char *shader)
{
    // Create shader and compile
    GLuint shaderId = gl_check(glCreateShader(type));
    gl_log("Created shader, type: 0x%x04, id: %d", type, shaderId);
    gl_check(glShaderSource(shaderId, 1, &shader, NULL));

    gl_check(glCompileShader(shaderId));

    // Check for errors.
    GLint Result = GL_FALSE;
    GLint infoLogLength;

    gl_check(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result));
    gl_check(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength));

    if (infoLogLength > 1) {
        std::vector<char> msg(infoLogLength + 1);
        glGetShaderInfoLog(shaderId, infoLogLength, NULL, &msg[0]);
        gl_log("%s", &msg[0]);
    }

    return shaderId;
}

GLuint linkProgram(const std::vector<GLuint> &shaderIds)
{
    GLuint programId = gl_check(glCreateProgram());
    gl_log("Created program id: %d", programId);

    for (auto &sh : shaderIds) {
        gl_check(glAttachShader(programId, sh));
    }

    gl_log("Linking program");
    gl_check(glLinkProgram(programId));

    // Check the program
    GLint result = GL_FALSE;
    GLint infoLogLength = 0;

    gl_check(glGetProgramiv(programId, GL_LINK_STATUS, &result));
    gl_check(glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength));

    if (infoLogLength > 1) {
        std::vector<char> programErrorMessage(infoLogLength + 1);
        gl_check(glGetProgramInfoLog(programId, infoLogLength, NULL,
            &programErrorMessage[0]));
        gl_log("%s", &programErrorMessage[0]);
    }

    return programId;
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
    g_projectionMatrix = glm::perspective(g_fov, g_screenWidth / g_screenHeight,
        0.1f, 100.0f);
    g_vpMatrix = g_projectionMatrix * g_viewMatrix;
    g_viewDirty = false;
}

//void drawBoundingBox(bearded::dangerzone::geometry::BBox *b, unsigned int vaoIdx)
//{
//    glm::mat4 mvp = g_vpMatrix * b->transform() * glm::toMat4(g_rotation);
//
//    glUseProgram(g_shaderProgramId);
//    glUniformMatrix4fv(g_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
//    glBindVertexArray(g_vaoIds[vaoIdx]);
//    glDrawElementsInstanced(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0, 2);
//    glDrawElementsInstanced(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)), 2);
//    glDrawElementsInstanced(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)), 2);
//    glBindVertexArray(0);
//}

void loop(GLFWwindow *window)
{
    glUseProgram(g_shaderProgramId);

    do {
        gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        if (g_viewDirty) {
            updateMvpMatrix();
        }

        gl_check(glUniformMatrix4fv(g_uniform_mvp, 1, GL_FALSE,
            glm::value_ptr(g_vpMatrix * glm::toMat4(g_rotation))));

        // Coordinate Axis
        bd::VertexArrayObject *vao = g_vaoIds[0];
        vao->bind();
            gl_check(glDrawArrays(GL_LINES, 0, 6));
        vao->unbind();

        // Quad geometry
        vao = g_vaoIds[1];
        vao->bind();
            gl_check(glDrawElements(GL_TRIANGLE_STRIP, vao->numElements(), GL_UNSIGNED_SHORT, 0));
        vao->unbind();

        vao = g_vaoIds[2];
        vao->bind();
            gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0));
            gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort))));
            gl_check(glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort))));
        vao->unbind();

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);
}

GLFWwindow *init()
{
    GLFWwindow *window = nullptr;
    if (!glfwInit()) {
        gl_log("could not start GLFW3");
        return nullptr;
    }

    glfwSetErrorCallback(glfw_error_callback);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    // number of samples to use for multi sampling
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 720, "Minimal Instance Rendering!!!", NULL,
        NULL);
    if (!window) {
        gl_log("ERROR: could not open window with GLFW3");
        glfwTerminate();
        return nullptr;
    }

    glfwSetCursorPosCallback(window, glfw_cursorpos_callback);
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum error = glewInit();
    if (error) {
        gl_log("could not init glew %s", glewGetErrorString(error));
        return nullptr;
    }

    bd::subscribe_debug_callbacks();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

    return window;
}

void genQuadVao(bd::VertexArrayObject &vao)
{
    const std::array<glm::vec4, 4> verts
        {
            glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f), // 0 ll
            glm::vec4(0.5f, -0.5f, 0.0f, 1.0f), // 1 lr
            glm::vec4(0.5f, 0.5f, 0.0f, 1.0f), // 2 ur
            glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f) // 3 ul
        };

    const std::array<unsigned short, 4> elements
        {
            0, 1, 3, 2      // triangle strip maybe?
            //0, 1, 2, 3    // line strip maybe?
        };

    const std::array<glm::vec3, 4> colors
        {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        };

    vao.addVbo(( float * ) (verts.data()), verts.size() * 4, 4, 0);
    vao.addVbo(( float * ) (colors.data()), colors.size() * 3, 3, 1);
    vao.setIndexBuffer(( unsigned short * ) (elements.data()), elements.size());

}

void genAxisVao(bd::VertexArrayObject &vao)
{

    const std::array<glm::vec4, 6> verts
    {
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0.5f, 0.0f, 0.0f, 1.0f),

        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0.0f, 0.5f, 0.0f, 1.0f),

        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0.0f, 0.0f, 0.5f, 1.0f)
    };

    const std::array<glm::vec3, 6> colors
    {
        glm::vec3(1.0f, 0.0f, 0.0f),  // x: red
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),  // y: green
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),  // z: blue
        glm::vec3(0.0f, 0.0f, 1.0f)
    };

    vao.addVbo(( float * ) (verts.data()), verts.size() * 4, 4, 0);
    vao.addVbo(( float * ) (colors.data()), colors.size() * 3, 3, 1);

}

void genBoxVao(bd::VertexArrayObject &vao)
{
    const std::array<glm::vec4, 8> verts {
        glm::vec4(-0.5, -0.5, -0.5, 1.0),
        glm::vec4(0.5, -0.5, -0.5, 1.0),
        glm::vec4(0.5, 0.5, -0.5, 1.0),
        glm::vec4(-0.5, 0.5, -0.5, 1.0),
        glm::vec4(-0.5, -0.5, 0.5, 1.0),
        glm::vec4(0.5, -0.5, 0.5, 1.0),
        glm::vec4(0.5, 0.5, 0.5, 1.0),
        glm::vec4(-0.5, 0.5, 0.5, 1.0)
    };
    const std::array<glm::vec3, 8> colors{
        glm::vec3(0.5, 0.5, 0.5),
        glm::vec3(0.5, 0.5, 0.5),
        glm::vec3(0.5, 0.5, 0.5),
        glm::vec3(0.5, 0.5, 0.5),
        glm::vec3(0.5, 0.5, 0.5),
        glm::vec3(0.5, 0.5, 0.5),
        glm::vec3(0.5, 0.5, 0.5),
        glm::vec3(0.5, 0.5, 0.5)
    };

    const std::array<unsigned short, 16> elements {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    vao.addVbo((float *) (verts.data()), verts.size()*4, 4, 0);
    vao.addVbo((float *) (colors.data()), colors.size()*3, 3, 1);
    vao.setIndexBuffer((unsigned short *)(elements.data()), elements.size());

}


void cleanup()
{
    std::vector<GLuint> bufIds;
//    for (unsigned i=0; i<NUMBOXES; ++i) {
//        bufIds.push_back(g_bbox[i].iboId());
//        bufIds.push_back(g_bbox[i].vboId());
//    }
//    glDeleteBuffers(NUMBOXES, &bufIds[0]);
    glDeleteProgram(g_shaderProgramId);
}

int main(int argc, char *argv[])
{
    bd::gl_log_restart();

    GLFWwindow *window;
    if ((window = init()) == nullptr) {
        gl_log("Could not initialize GLFW, exiting.");
        return 1;
    }

    GLuint vsId = compileShader(GL_VERTEX_SHADER, vertex_shader);
    GLuint fsId = compileShader(GL_FRAGMENT_SHADER, fragment_shader);
    g_shaderProgramId = linkProgram({ vsId, fsId });
    g_uniform_mvp = glGetUniformLocation(g_shaderProgramId, "mvp");

    bd::VertexArrayObject quad(bd::VertexArrayObject::Method::ELEMENTS);
    quad.create();

    bd::VertexArrayObject axis(bd::VertexArrayObject::Method::ARRAYS);
    axis.create();

    bd::VertexArrayObject box(bd::VertexArrayObject::Method::ELEMENTS);
    box.create();

    genQuadVao(quad);
    genAxisVao(axis);
    genBoxVao(box);

    g_vaoIds.push_back(&axis);
    g_vaoIds.push_back(&quad);
    g_vaoIds.push_back(&box);

    loop(window);
    cleanup();

    bd::gl_log_close();

    glfwTerminate();

    return 0;
}
