//************************************************************//
// Jim Pelton
// Graduate Student
// Dept. of Computer Science
// Boise State University
// Spring 2015
//
// Simplevr -- Simple slice-based 3D texture volume renderer
//************************************************************//


#include "geometry/BBox.h"
#include "file/datareader.h"
#include "log/gl_log.h"

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

class Quad {
public:
	static const std::array<glm::vec4, 4> verts;
    static const std::array<unsigned short, 4> elements;

public:
	Quad()
        : Quad(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f))
    { }

    Quad(const glm::vec3 &center, const glm::vec2 &dims)
        : Quad(center, dims, glm::vec3(1.0f, 1.0f, 1.0f))
    { }

    Quad(const glm::vec3 &center, const glm::vec2 &dims, const glm::vec3 &color) 
        : m_model(glm::translate(glm::mat4(1.0f), center) * glm::scale(glm::mat4(1.0f), glm::vec3(dims, 1.0f)))
        //: m_model(glm::translate(glm::mat4(1.0f), center))
        , m_color(color)
    { }

	~Quad() {}

	const glm::mat4& model() {
        return m_model;
    }

    const glm::vec3& color() {
        return m_color;
    }

    //const glm::mat4& scale() {
    //    return m_scale;
    //}

private:
	glm::mat4 m_model;
    //glm::mat4 m_scale;
    glm::vec3 m_color;
};

const std::array<glm::vec4, 4> Quad::verts {
	glm::vec4(-0.5f, -0.5f, 0.0, 1.0),
	glm::vec4( 0.5f, -0.5f, 0.0, 1.0),
	glm::vec4( 0.5f,  0.5f, 0.0, 1.0),
	glm::vec4(-0.5f,  0.5f, 0.0, 1.0)
};

const std::array<unsigned short, 4> Quad::elements {
    0, 1, 2, 3
};

///////////////////////////////////////////////////////////////////////////////

const char* vertex_shader =
"#version 400\n"
"in vec3 vert;"
"uniform mat4 v;"      // view matrix
"uniform mat4 p;"      // proj matrix
"uniform mat4 m;"      // model matrix
"uniform mat4 rot;"    // rotation matrix
"uniform vec3 vdir;"   // view dir
"uniform float st;"    // start
"uniform float ds;"    // slice distance
"uniform int numblocks;" // number of blocks that will be drawn.
""
"void main () {"
"  vec4 offset = vec4(vdir * ( st + ds * gl_InstanceID ), 1.0f);"
"  gl_Position = (p * v * rot * (m * vec4(vert, 1.0f))) + offset ;"

"}";

const char* fragment_shader =
"#version 400\n"
"uniform vec3 in_col;"
"out vec4 col;"
"void main () {"
"  col = vec4(in_col, 1.0f);"
"}";

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


glm::quat g_cameraRotation;
glm::mat4 g_viewMatrix;
glm::mat4 g_projectionMatrix;
// glm::mat4 g_vpMatrix;
glm::vec3 g_camPosition(0.0f, 0.0f, -30.0f);
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
std::vector<Quad> theQuads;

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
    GLuint shaderId = glCreateShader(type);
    gl_log("Created shader, type: 0x%x04, id: %d", type, shaderId);
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
        gl_log("%s", &msg[0]);
    }

    return shaderId;
}

GLuint linkProgram(const std::vector<GLuint> &shaderIds)
{
    GLuint programId = glCreateProgram();
    gl_log("Created program id: %d", programId);

    for (auto &sh : shaderIds) {
        glAttachShader(programId, sh);
    }

    gl_log("Linking program");
    glLinkProgram(programId);

    // Check the program
    int InfoLogLength = 0;
    GLint result = GL_FALSE;

    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if (InfoLogLength > 1) {
        std::vector<char> programErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(programId, InfoLogLength, NULL, &programErrorMessage[0]);
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

    g_cameraRotation = (rotX * rotY) * g_cameraRotation;

    g_viewDirty = true;
}

void updateMvpMatrix()
{
    g_viewMatrix = glm::lookAt(g_camPosition, g_camFocus, g_camUp);
    g_projectionMatrix = glm::perspective(g_fov, g_screenWidth/g_screenHeight, 0.1f, 100.0f);
    // g_vpMatrix = g_projectionMatrix * g_viewMatrix;
    g_viewDirty = false;
}

//void drawBoundingBox(bearded::dangerzone::geometry::BBox *b, unsigned int vaoIdx)
//{
//    glm::mat4 mvp = g_vpMatrix * b->modelTransform() * glm::toMat4(g_cameraRotation);
//
//    glUseProgram(g_shaderProgramId);
//    glUniformMatrix4fv(g_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
//    glBindVertexArray(g_vaoIds[vaoIdx]);
//    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
//    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
//    glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
//    glBindVertexArray(0);
//}

void drawSlicesInstanced(unsigned int vaoId) 
{
    glBindVertexArray(vaoId);

    for (auto &q : theQuads) {
        glUniformMatrix4fv(g_uniform_m, 1, GL_FALSE, glm::value_ptr(q.model()));
        //glUniformMatrix4fv(g_uniform_scale, 1, GL_FALSE, glm::value_ptr(q.scale()));
        glUniform3fv(g_uniform_color, 1, glm::value_ptr(q.color()));
        glDrawElementsInstanced(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0, NUMSLICES);
    }

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
        
    
        glUniformMatrix4fv(g_uniform_rot, 1, GL_FALSE, glm::value_ptr(glm::toMat4(g_cameraRotation)));   // rot
        
        drawSlicesInstanced(g_q_vaoId);

        //drawBoundingBox(&g_bbox[0], 0);
        //drawBoundingBox(g_bbox[1], 1);

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

    window = glfwCreateWindow(1280, 720, "Minimal", NULL, NULL);
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

    glDebugMessageCallback((GLDEBUGPROC)gl_debug_message_callback, NULL);
    glEnable(GL_DEBUG_OUTPUT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

    return window;
}

void initBoxVbos(unsigned int vaoId) {
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
            4,                  // number of elements per vertex, here (x,y,z,w)
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

void initQuadVbos(unsigned int vaoId) {
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
            4,                  // number of elements per vertex, here (x,y,z,w)
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

bool readVolumeData(const std::string &rawFile, size_t w, size_t h, size_t d, float **rawdata) {
    if (rawFile.length() == 0)
        return false;
    if (w == 0 || h == 0 || d == 0)
        return false;

    bearded::dangerzone::file::DataReader<float, float> reader;
    reader.loadRaw3d(rawFile, w, h, d);
    *rawdata = reader.takeOwnership();

    return true;
}

void hsvToRgb(float h, float s, float v, glm::vec3 &rgb) {
    while (h < 0.0f) { h += 360.0f; }
    while (h > 360.0f) { h -= 350.0f; }

    if (v <= 0.0f) {
        rgb.r = rgb.g = rgb.b = 0.0f;
    } else if (s <= 0.0f) {
        rgb.r = rgb.g = rgb.b = v;
    } else {
        float chroma = v * s;
        float H = h / 60.0f;
        int thang = int(H);
        float wat = H - thang;
        float pv = v * (1.0f - s);
        float qv = v * (1.0f - s * wat);
        float tv = v * (1.0f - s * (1.0f - wat));
        switch (thang) {
        case 0:
            rgb.r = v; 
            rgb.g = tv; 
            rgb.b = pv;
            break;
        case 1:
            rgb.r = qv;
            rgb.g = v;
            rgb.b = tv;
            break;
        case 2:
            rgb.r = pv;
            rgb.g = v;
            rgb.b = tv;
            break;
        case 3:
            rgb.r = pv;
            rgb.g = qv;
            rgb.b = v;
            break;
        case 4:
            rgb.r = tv;
            rgb.g = pv;
            rgb.b = v;
            break;
        case 5:
            rgb.r = v;
            rgb.g = pv;
            rgb.b = qv;
            break;
        case 6:
            rgb.r = v;
            rgb.g = tv;
            rgb.b = pv;
            break;
        case -1:
            rgb.r = v; 
            rgb.g = pv;
            rgb.b = qv;
            break;
        default:
            rgb.r = rgb.g = rgb.b = v;
            break;
        }
    }
}

void makeQuadsAndColors() {
    const float db = 1.0f / NUMBLOCKS;
    const glm::vec2 dims(db-0.01f, db-0.01f);
    glm::vec3 color;
    int hue = 0;
    const int huemax = 360;
    const int dh = huemax / (NUMBLOCKS*NUMBLOCKS*NUMBLOCKS);

    for (int z = 0; z < NUMBLOCKS; ++z) {
        for (int y = 0; y < NUMBLOCKS; ++y) {
            for (int x = 0; x < NUMBLOCKS; ++x)
            {
                glm::vec3 c(db*x, db*y, db*z);
                c += -0.5f;
                hsvToRgb(hue, 1.0f, 1.0f, color);
                hue += dh;
                theQuads.push_back({ c, dims, color });
            }
        }
    }
}


void cleanup()
{
    if (g_q_vboId > 0) {
        glDeleteBuffers(1, &g_q_vboId);
    }
//    std::vector<GLuint> bufIds;
//    for (unsigned i=0; i<NUMBOXES; ++i) {
//        bufIds.push_back(g_bbox[i].iboId());
//        bufIds.push_back(g_bbox[i].vboId());
//    }
//
//    glDeleteBuffers(NUMBOXES, &bufIds[0]);
//    glDeleteProgram(g_shaderProgramId);
}

void usage(const char *msg = nullptr) {
    std::cout << "simplevr <raw-file>\n";
    if (msg) {
        std::cout << msg << "\n";
    }
}

int main(int argc, char* argv[])
{
    gl_log_restart();
    gl_debug_log_restart();

    GLFWwindow * window;
    if ((window = init()) == nullptr) {
        gl_log("Could not initialize GLFW, exiting.");
        return 1;
    }
    std::string rawFile(argc > 1 ? argv[1] : std::string());

    size_t height = argc > 2 ? atol(argv[2]) : 0ul;
    size_t width  = argc > 3 ? atol(argv[3]) : 0ul;
    size_t depth  = argc > 4 ? atol(argv[4]) : 0ul;

    float *data = nullptr;

    /*if(! readVolumeData(rawFile, width, height, depth, &data)) {
        usage();
        return 1;
      }*/

    glGenVertexArrays(1, &g_q_vaoId);
    initQuadVbos(g_q_vaoId);
    makeQuadsAndColors();

    GLuint vsId = compileShader(GL_VERTEX_SHADER, vertex_shader);
    if (vsId == 0) {
        gl_log_err("Vertex shader failed to compile. Exiting...");
        cleanup();
        exit(1);
    }

    GLuint fsId = compileShader(GL_FRAGMENT_SHADER, fragment_shader);
    if (fsId == 0) {
        gl_log_err("Fragment shader failed to compile. Exiting...");
        cleanup();
        exit(1);
    }

    g_shaderProgramId = linkProgram({ vsId, fsId });
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

