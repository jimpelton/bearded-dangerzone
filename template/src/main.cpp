
#include "geometry/BBox.h"
#include "file/tiny_obj_loader/tiny_obj_loader.h"

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

#include <cstdio>
#include <cstdarg>
#include <ctime>

const char* vertex_shader =
"#version 400\n"
"in vec3 vp;"
"uniform mat4 mvp;"
"void main () {"
"  gl_Position = mvp * vec4(vp, 1.0);"
"}";

const char* fragment_shader =
"#version 400\n"
"out vec4 frag_colour;"
"void main () {"
"  frag_colour = vec4 (1.0, 1.0, 1.0, 1.0);"
"}";

const char *g_logfile = "gl.log";
FILE *g_file = nullptr;

const unsigned int NUMBOXES = 1;
std::vector<bearded::dangerzone::BBox> g_bbox;


GLuint  g_uniform_mvp;
GLuint  g_shaderProgramId;
std::vector<GLuint> g_vaoIds;

glm::quat g_cameraRotation;
glm::mat4 g_modelMatrix;
glm::mat4 g_viewMatrix;
glm::mat4 g_projectionMatrix;
glm::mat4 g_vpMatrix;
glm::vec3 g_camPosition(0.0f, 0.0f, -10.0f);
glm::vec3 g_camFocus(0.0f, 0.0f, 0.0f);
glm::vec3 g_camUp(0.0f, 1.0f, 0.0f);
glm::vec2 g_cursorPos;

float g_mouseSpeed = 1.0f;
float g_screenWidth = 1280.0f;
float g_screenHeight = 720.0f;
float g_fov = 50.0f;
bool g_viewDirty = false;


void log(const char* message, ...);
void closeLog();
void cleanup();

GLuint loadShader(GLenum type, std::string filepath);
GLuint compileShader(GLenum type, const char *shader);

void glfw_cursorpos_callback(GLFWwindow *window, double x, double y);
void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void glfw_error_callback(int error, const char* description);
void glfw_window_size_callback(GLFWwindow *window, int width, int height);

void updateMvpMatrix();
void setRotation(const glm::vec2 &dr);
void drawBoundingBox(bearded::dangerzone::BBox *b, unsigned int vaoIdx);
void loop(GLFWwindow *window);

void log(const char* message, ...)
{
    va_list argptr;
    if (!g_file) {
        g_file = fopen(g_logfile, "w");
        if (!g_file) {
            fprintf(stderr, "ERROR: could not open %s file for appending\n", g_logfile);
            return;
        }
        time_t now = time(NULL); char* date = ctime(&now);
        fprintf(g_file, "------------------------\nOpening %s. local time %s\n", g_logfile, date);
    }
    va_start(argptr, message); vfprintf(g_file, message, argptr); va_end(argptr);
    fprintf(g_file, "\n"); fflush(g_file);
}

void closeLog()
{
    if (!g_file) {
        fprintf(stderr, "Error: not closing %s because it was not opened.", g_logfile);
        return;
    }
    time_t now = time(NULL); char* date = ctime(&now);
    fprintf(g_file, "------------------------\nClosing %s. local time %s\n", g_logfile, date);
    fclose(g_file);
}

/************************************************************************/
/* OPENGL CALLBACKS                                                     */
/************************************************************************/
void gl_debug_message_callback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    void *userParam)
{
   /* fprintf(stderr, "OGL_DEBUG: source: 0x%04X, type 0x%04X, id %u, severity 0x%0X, '%s'\n",
        source, type, id, severity, message);*/
    log("OGL_DEBUG: source: 0x%04X, type 0x%04X, id %u, severity 0x%0X, '%s'\n",
        source, type, id, severity, message);
}


/************************************************************************/
/* G L F W     C A L L B A C K S                                        */
/************************************************************************/
void glfw_error_callback(int error, const char* description)
{
    log("GLFW ERROR: code %i msg: %s", error, description);
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

/************************************************************************/
/* S H A D E R   C O M P I L I N G                                      */
/************************************************************************/
GLuint loadShader(GLenum type, std::string filepath)
{
    GLuint shaderId = 0;
    std::ifstream file(filepath.c_str());
    if (!file.is_open()) {
        log("Couldn't open %s", filepath.c_str());
        return 0;
    }

    std::stringstream shaderCode;
    shaderCode << file.rdbuf();

    std::string code = shaderCode.str();
    const char *ptrCode = code.c_str();
    file.close();

    log("Compiling shader: %s", filepath.c_str());
    shaderId = compileShader(type, ptrCode);
    return shaderId;
}

GLuint compileShader(GLenum type, const char *shader)
{
    // Create shader and compile
    GLuint shaderId = glCreateShader(type);
    log("Created shader, type: 0x%x04, id: %d", type, shaderId);
    glShaderSource(shaderId, 1, &shader, NULL);

    glCompileShader(shaderId);

    // Check for errors.
    GLint Result = GL_FALSE;
    int InfoLogLength;

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if (InfoLogLength > 1){
        std::vector<char> msg(InfoLogLength + 1);
        glGetShaderInfoLog(shaderId, InfoLogLength, NULL, &msg[0]);
        log("%s", &msg[0]);
    }

    return shaderId;
}

GLuint linkProgram(const std::vector<GLuint> &shaderIds)
{
    GLuint programId = glCreateProgram();
    log("Created program id: %d", programId);

    for (auto &sh : shaderIds) {
        glAttachShader(programId, sh);
    }

    log("Linking program");
    glLinkProgram(programId);

    // Check the program
    int InfoLogLength = 0;
    GLint result = GL_FALSE;

    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if (InfoLogLength > 1) {
        std::vector<char> programErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(programId, InfoLogLength, NULL, &programErrorMessage[0]);
        log("%s", &programErrorMessage[0]);
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

    g_cameraRotation = (rotX * rotY) * g_cameraRotation;

    g_viewDirty = true;
}

void updateMvpMatrix()
{
    g_viewMatrix = glm::lookAt(g_camPosition, g_camFocus, g_camUp);
    g_projectionMatrix = glm::perspective(g_fov, g_screenWidth/g_screenHeight, 0.1f, 100.0f);
    g_vpMatrix = g_projectionMatrix * g_viewMatrix;
    g_viewDirty = false;
}

void drawBoundingBox(bearded::dangerzone::BBox *b, unsigned int vaoIdx)
{
    glm::mat4 mvp = g_vpMatrix * b->modelTransform() * glm::toMat4(g_cameraRotation);

    glUseProgram(g_shaderProgramId);
    glUniformMatrix4fv(g_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
    glBindVertexArray(g_vaoIds[vaoIdx]);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
    glBindVertexArray(0);
}

void loop(GLFWwindow *window)
{
    g_viewDirty = true;

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (g_viewDirty) {
            updateMvpMatrix();
        }

        drawBoundingBox(&g_bbox[0], 0);
        //drawBoundingBox(g_bbox[1], 1);

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);
}

GLFWwindow* init()
{
    GLFWwindow *window = nullptr;
    if (!glfwInit()) {
        log("could not start GLFW3");
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
        log("ERROR: could not open window with GLFW3");
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
        log("could not init glew %s", glewGetErrorString(error));
        return nullptr;
    }

    glDebugMessageCallback((GLDEBUGPROC)gl_debug_message_callback, NULL);
    glEnable(GL_DEBUG_OUTPUT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

    return window;
}

void cleanup()
{
    std::vector<GLuint> bufIds;
    for (int i=0; i<NUMBOXES; ++i) {
        bufIds.push_back(g_bbox[i].iboId());
        bufIds.push_back(g_bbox[i].vboId());
    }
    glDeleteBuffers(NUMBOXES, &bufIds[0]);
    glDeleteProgram(g_shaderProgramId);
}

int main(int argc, char* argv[])
{
    GLFWwindow * window;
    if ((window = init()) == nullptr) {
        log("Could not initialize GLFW, exiting.");
        return 1;
    }

//    std::vector<tinyobj::shape_t> shapes;
//    std::vector<tinyobj::material_t> materials;
//    tinyobj::LoadObj(shapes, materials, "torus/torus.obj", "torus/torus.mtl");

    GLuint vsId = compileShader(GL_VERTEX_SHADER, vertex_shader);
    GLuint fsId = compileShader(GL_FRAGMENT_SHADER, fragment_shader);
    g_shaderProgramId = linkProgram({ vsId, fsId });
    glGetUniformLocation(g_shaderProgramId, "mvp");

    const GLfloat minmax[6] =
        { -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f };

    g_vaoIds.reserve(NUMBOXES);
    glGenVertexArrays(NUMBOXES, &g_vaoIds[0]);
    for(int i=0; i<NUMBOXES; ++i)
    {
        glBindVertexArray(g_vaoIds[i]);
        bearded::dangerzone::BBox box(minmax);
        box.init();
        g_bbox.push_back(box);
    }

    glBindVertexArray(0);

    loop(window);
    cleanup();
    closeLog();
    glfwTerminate();

    return 0;
}

