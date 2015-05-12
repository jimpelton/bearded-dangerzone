#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "block.h"
#include "cmdline.h"
#include "create_vao.h"

#include <bd/log/gl_log.h>

#include <bd/graphics/shader.h>
#include <bd/graphics/axis.h>
#include <bd/graphics/BBox.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/graphics/quad.h>

#include <bd/util/util.h>


//#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>

#include <string>
#include <vector>
#include <array>
#include <memory>

#include <fstream>
#include <iostream>

#ifdef BDPROF
#define PATH_TO_NVPMAPI_CORE L"D:\\libs\\perfkit\\4.4.0-windows-desktop\\bin\\x64\\NvPmApi.Core.dll"

#ifndef NVPM_INIGUID
#define NVPM_INITGUID 1
#endif
#include "NvPmApi.h"
#include "NvPmApi.Manager.h"

//Simple singleton implementation for grabbing the NvPmApi
static NvPmApiManager S_NVPMManager;
NVPMContext g_nvpmContext{ 0 };


///////////////////////////////////////////////////////////////////////////
extern NvPmApiManager *GetNvPmApiManager()
{
    return &S_NVPMManager;
}


//////////////////////////////////////////////////////////////////////////
const NvPmApi *GetNvPmApi()
{
    return S_NVPMManager.Api();
}



//////////////////////////////////////////////////////////////////////////
bool initNvPm()
{
    NVPMRESULT nvResult;

    if ((nvResult = GetNvPmApiManager()->Construct(PATH_TO_NVPMAPI_CORE)) != S_OK) {
        gl_log_err("NvPmApi Manager could not construct. result: %d", nvResult);
        return false; // This is an error condition
    }

    if ((nvResult = GetNvPmApi()->Init()) != NVPM_OK) {
        gl_log_err("NvPmApi could not initialize. result: %d", nvResult);
        return false; // This is an error condition
    }
    
    nvResult = 
        GetNvPmApi()->CreateContextFromOGLContext(uint64_t(::wglGetCurrentContext()), 
            &g_nvpmContext);
    
    if (nvResult != NVPM_OK)
    {
        gl_log_err("NvPmApi could not create context from ogl context! result: %d", nvResult);
        return false; // This is an error condition
    }

    gl_log("NvPmApi initialized.");

    return true;
}

bool nvpmShutdown()
{
    if (!S_NVPMManager.Api()) {
        return false;
    }

    S_NVPMManager.Api()->Shutdown();
    
    return true;
}

struct NvPmGlobals
{
    NVPMUINT framestart;
} g_globals;

struct Mode
{
    NVPMContext perfCtx;
    NVPMUINT  nTotalPasses;
    NVPMUINT nPass;
    NVPMUINT nFrame;

    int objectId;
    bool isCollecting;

    const char** counterNames;
    int counterCount;
};

void experimentFrameBegin(Mode *m)
{
    if (m->nFrame == g_globals.framestart){
        glFinish();
        GetNvPmApi()->BeginExperiment(g_nvpmContext, &m->nTotalPasses);
        m->isCollecting = true;
        m->nPass = 0;
    }

    if (m->isCollecting) {
        GetNvPmApi()->BeginPass(m->perfCtx, m->nPass);
    }
}

void experimentFrameEnd(Mode *mode)
{
    if (mode->isCollecting) {
        GetNvPmApi()->EndPass(mode->perfCtx, mode->nPass);
        ++mode->nPass;
        mode->objectId = 0;
    }
    if (mode->isCollecting && mode->nPass == mode->nTotalPasses) {
        mode->isCollecting = false;
        mode->nPass = 0;
        mode->nTotalPasses = 0;
        GetNvPmApi()->EndExperiment(mode->perfCtx);

        //TODO: printcounters(...);
    }
    ++mode->nFrame;
}



#define call_draw(_func)                                           \
    do {                                                           \
        unsigned int _nPasses = 6;                                  \
        GetNvPmApi()->BeginExperiment(g_nvpmContext, &(_nPasses));  \
        for (unsigned _i=0; _i < (_nPasses); _i++) {                \
            GetNvPmApi()->BeginPass(g_nvpmContext, _i);            \
            GetNvPmApi()->BeginObject(g_nvpmContext, 0);           \
            _func();                                               \
            glFlush();                                             \
            GetNvPmApi()->EndObject(g_nvpmContext, 0);             \
            GetNvPmApi()->EndPass(g_nvpmContext, _i);              \
        }                                                          \
        GetNvPmApi()->EndExperiment(g_nvpmContext);                \
    } while (0)

#define init_nvperf_or_exit()                               \
    do {                                                    \
        if (! initNVPerfThing()) {                          \
            gl_log_err("Could not init nvperf api!");       \
            cleanup();                                      \
            exit(1);                                        \
        }                                                   \
    } while(0)


#else

#define call_draw(_func) (_func);
#define init_nvperf()

#endif

const glm::vec3 X_AXIS{ 1.0f, 0.0f, 0.0f };
const glm::vec3 Y_AXIS{ 0.0f, 1.0f, 0.0f };
const glm::vec3 Z_AXIS{ 0.0f, 0.0f, 1.0f };

enum class SliceSet : unsigned int
{
    XZ, YZ, XY, NoneOfEm, AllOfEm
};

enum class ObjType : unsigned int
{
    Axis, Quads, Boxes
};


///////////////////////////////////////////////////////////////////////////////
// Geometry and VAOs
///////////////////////////////////////////////////////////////////////////////
SliceSet g_selectedSliceSet{ SliceSet::XY };
bd::Axis g_axis;
bd::Box g_box;
std::vector<bd::VertexArrayObject *> g_vaoIds;
std::vector<Block> g_blocks;
std::vector<Block*> g_nonEmptyBlocks;
size_t g_elementBufferSize{ 0 };
const int g_elementsPerQuad{ 5 };

///////////////////////////////////////////////////////////////////////////////
// Shaders and Textures
///////////////////////////////////////////////////////////////////////////////
bd::ShaderProgram g_simpleShader;
bd::ShaderProgram g_volumeShader;
Texture g_tfuncTex;
float g_scaleValue{ 1.0f };


///////////////////////////////////////////////////////////////////////////////
// Viewing and Controls Data
///////////////////////////////////////////////////////////////////////////////
glm::quat g_rotation;
glm::mat4 g_viewMatrix;
glm::mat4 g_projectionMatrix;
glm::mat4 g_vpMatrix;
glm::vec3 g_camPosition{ 0.0f, 0.0f, 2.0f }; // looking down +Z axis.
glm::vec3 g_camFocus{ 0.0f, 0.0f, 0.0f };
glm::vec3 g_camUp{ 0.0f, 1.0f, 0.0f };
glm::vec2 g_cursorPos;

float g_mouseSpeed{ 1.0f };
int g_screenWidth{ 1000 };
int g_screenHeight{ 1000 };
float g_fov_deg{ 50.0f };
bool g_viewDirty{ true };
bool g_modelDirty{ true };
bool g_toggleBlockBoxes{ false };
int g_numSlices{ 1 };

//TODO: bool g_toggleVolumeBox{ false };


void glfw_cursorpos_callback(GLFWwindow *window, double x, double y);

void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode, int action,
    int mods);

void glfw_error_callback(int error, const char *description);

void glfw_window_size_callback(GLFWwindow *window, int width, int height);

void glfw_scrollwheel_callback(GLFWwindow *window, double xoff, double yoff);

void updateViewMatrix();

void setRotation(const glm::vec2 &dr);

void loop(GLFWwindow *window);

void cleanup();


/************************************************************************/
/* G L F W     C A L L B A C K S                                        */
/************************************************************************/
void glfw_error_callback(int error, const char *description)
{
    gl_log_err("GLFW ERROR: code %i msg: %s", error, description);
}


/////////////////////////////////////////////////////////////////////////////////
void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode, int action,
    int mods)
{
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_0:
            g_selectedSliceSet = SliceSet::NoneOfEm;
            break;
        case GLFW_KEY_1:
            g_selectedSliceSet = SliceSet::XY;
            break;
        case GLFW_KEY_2:
            g_selectedSliceSet = SliceSet::XZ;
            break;
        case GLFW_KEY_3:
            g_selectedSliceSet = SliceSet::YZ;
            break;
        case GLFW_KEY_4:
            g_selectedSliceSet = SliceSet::AllOfEm;
            break;
        case GLFW_KEY_B:
            g_toggleBlockBoxes = !g_toggleBlockBoxes;
            break;
        }
    }

    if (action != GLFW_RELEASE){
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


/////////////////////////////////////////////////////////////////////////////////
void glfw_window_size_callback(GLFWwindow *window, int width, int height)
{
    g_screenWidth = width;
    g_screenHeight = height;
    glViewport(0, 0, width, height);
    g_viewDirty = true;
}


/////////////////////////////////////////////////////////////////////////////////
void glfw_cursorpos_callback(GLFWwindow *window, double x, double y)
{
    glm::vec2 cpos(floor(x), floor(y));
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        glm::vec2 delta(cpos - g_cursorPos);
        setRotation(delta);
    }

    g_cursorPos = cpos;
}


///////////////////////////////////////////////////////////////////////////////
void glfw_scrollwheel_callback(GLFWwindow *window, double xoff, double yoff)
{
    float fov = static_cast<float>(g_fov_deg + (yoff*1.75f));

    std::cout << "fov: " << fov << std::endl;

    if (fov < 1 || fov>120) return;

    g_fov_deg = fov;

    g_viewDirty = true;
}

/************************************************************************/
/*     D R A W I N'                                                     */
/************************************************************************/


/////////////////////////////////////////////////////////////////////////////////
void setRotation(const glm::vec2 &dr)
{
    glm::quat rotX = glm::angleAxis<float>(
        glm::radians(-dr.y) * g_mouseSpeed,
        glm::vec3(1, 0, 0)
        );

    glm::quat rotY = glm::angleAxis<float>(
        glm::radians(dr.x) * g_mouseSpeed,
        glm::vec3(0, 1, 0)
        );

    g_rotation = (rotX * rotY) * g_rotation;

    g_modelDirty = true;
}


///////////////////////////////////////////////////////////////////////////////
void updateViewMatrix()
{
    g_viewMatrix = glm::lookAt(g_camPosition, g_camFocus, g_camUp);
    g_projectionMatrix = glm::perspective(glm::radians(g_fov_deg),
        g_screenWidth / static_cast<float>(g_screenHeight), 0.1f, 100.0f);
    g_vpMatrix = g_projectionMatrix * g_viewMatrix;

    g_viewDirty = false;
    // update the mvp in render loop
    g_modelDirty = true;

}


///////////////////////////////////////////////////////////////////////////////
void drawNonEmptyBoundingBoxes(const glm::mat4 &mvp)
{
    for (auto *b : g_nonEmptyBlocks) {

        glm::mat4 mmvp = mvp * b->transform().matrix();
        g_simpleShader.setUniform("mvp", mmvp);

        gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0));
        gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT,
            (GLvoid *)(4 * sizeof(GLushort))));
        gl_check(glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT,
            (GLvoid *)(8 * sizeof(GLushort))));

    } // for
}


///////////////////////////////////////////////////////////////////////////////
void drawSlices_XY()
{
    static const size_t xy_byteOffset{ 0 };
    gl_check(glDrawElements(GL_TRIANGLE_STRIP, g_elementsPerQuad * g_numSlices,
        GL_UNSIGNED_SHORT, (GLvoid *)xy_byteOffset));
}


///////////////////////////////////////////////////////////////////////////////
void drawSlices_XZ()
{
    static size_t xz_byteOffset{ g_elementsPerQuad * g_numSlices * sizeof(uint16_t) };
    gl_check(glDrawElements(GL_TRIANGLE_STRIP, g_elementsPerQuad * g_numSlices,
        GL_UNSIGNED_SHORT, (GLvoid *)xz_byteOffset));
}


///////////////////////////////////////////////////////////////////////////////
void drawSlices_YZ()
{
    static size_t yz_byteOffset{ 2 * g_elementsPerQuad * g_numSlices * sizeof(uint16_t) };
    gl_check(glDrawElements(GL_TRIANGLE_STRIP, g_elementsPerQuad * g_numSlices,
        GL_UNSIGNED_SHORT, (GLvoid *)yz_byteOffset));
}


///////////////////////////////////////////////////////////////////////////////
void drawNonEmptyBlocks_Forward(const glm::mat4 &mvp)
{
    static const int elementsPerQuad = 5;
//    std::cout << "forward" << std::endl;
    
    for (auto *b : g_nonEmptyBlocks) {
        b->texture().bind();
        glm::mat4 wmvp = mvp * b->transform().matrix();
        g_volumeShader.setUniform("mvp", wmvp);
        g_volumeShader.setUniform("tfScalingVal", g_scaleValue);

        switch (g_selectedSliceSet) {
        
        case SliceSet::XY:
//			nvprofile_begin_obj();
            call_draw(drawSlices_XY);
//			nvprofile_end_obj();
            break;
        case SliceSet::XZ:
//			nvpushA("XZ");
            drawSlices_XZ();
//			nvpopA();
            break;
        case SliceSet::YZ:
//			nvpushA("YZ");
            drawSlices_YZ();
//			nvpopA();
            break;
        case SliceSet::AllOfEm:
            drawSlices_XY();
            drawSlices_XZ();
            drawSlices_YZ();
            break;
        case SliceSet::NoneOfEm:
        default:
            break;
        } // switch

    } // for

}


///////////////////////////////////////////////////////////////////////////////
void drawNonEmptyBlocks_Reverse(const glm::mat4 &mvp)
{
    for (size_t i = g_nonEmptyBlocks.size() - 1; i >= 0; --i) {
        Block *b = g_nonEmptyBlocks[i];
        b->texture().bind();
        glm::mat4 wmvp = mvp * b->transform().matrix();
        g_volumeShader.setUniform("mvp", wmvp);
        g_volumeShader.setUniform("tfScalingVal", g_scaleValue);

        switch (g_selectedSliceSet) {
        
        case SliceSet::XY:
//			nvprofile_begin_obj();
            drawSlices_XY();
//			nvprofile_end_obj();
            break;
        case SliceSet::XZ:
//			nvpushA("XZ");
            drawSlices_XZ();
//			nvpopA();
            break;
        case SliceSet::YZ:
//			nvpushA("YZ");
            drawSlices_YZ();
//			nvpopA();
            break;
        case SliceSet::AllOfEm:
            drawSlices_XY();
            drawSlices_XZ();
            drawSlices_YZ();
            break;
        case SliceSet::NoneOfEm:
        default:
            break;
        } // switch
    } // for
    
}


///////////////////////////////////////////////////////////////////////////////
void drawNonEmptyBlocks(const glm::mat4 &mvp)
{
    g_volumeShader.bind();
//    glm::vec4 viewdir = glm::normalize(g_viewMatrix[2]);
//    glm::vec4 absViewdir = glm::abs(viewdir);

//    if (absViewdir.x > absViewdir.y && absViewdir.x > absViewdir.z) {
//        if (viewdir.x > 0) {
//            drawNonEmptyBlocks_Forward(mvp);
//        }
//        else {
//            drawNonEmptyBlocks_Reverse(mvp);
//        }
//    }
//    else if (absViewdir.y > absViewdir.x && absViewdir.y > absViewdir.z) {
//        if (viewdir.y > 0) {
//            drawNonEmptyBlocks_Forward(mvp);
//        }
//        else {
//            drawNonEmptyBlocks_Reverse(mvp);
//        }
//    }
//    else if (absViewdir.z > absViewdir.x && absViewdir.z > absViewdir.y) {
//        if (viewdir.z > 0) {
//            drawNonEmptyBlocks_Forward(mvp);
//        }
//        else {
//            drawNonEmptyBlocks_Reverse(mvp);
//        }
//    }

    //TODO: determine +/- dir of viewing vector.
    if (true){
        drawNonEmptyBlocks_Forward(mvp);
    } else {
        drawNonEmptyBlocks_Reverse(mvp);
    }

}


///////////////////////////////////////////////////////////////////////////////
void loop(GLFWwindow *window)
{
    gl_log("Entered render loop.");

    glm::mat4 mvp{ 1.0f };
    bd::VertexArrayObject *vao = nullptr;
    
    g_volumeShader.bind();
    g_tfuncTex.bind(); 

    do {
        if (g_viewDirty) {
            updateViewMatrix();
        }

        if (g_modelDirty) {
            mvp = g_vpMatrix * glm::toMat4(g_rotation);
            g_modelDirty = false;
        }

        gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


        ////////  Axis    /////////////////////////////////////////
        vao = g_vaoIds[static_cast<unsigned int>(ObjType::Axis)];
        vao->bind();
        g_simpleShader.bind();
        g_simpleShader.setUniform("mvp", mvp);
        g_axis.draw();
        vao->unbind();

        if (g_toggleBlockBoxes) {

        ////////  BBoxes  /////////////////////////////////////////
            vao = g_vaoIds[static_cast<unsigned int>(ObjType::Boxes)];
            vao->bind();
            drawNonEmptyBoundingBoxes(mvp);
            vao->unbind();
        }

        //////// Quad Geo /////////////////////////////////////////
        vao = g_vaoIds[static_cast<unsigned int>(ObjType::Quads)];
        vao->bind();
        drawNonEmptyBlocks(mvp);
        vao->unbind();

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);

    gl_log("Render loop exited.");
}


///////////////////////////////////////////////////////////////////////////////
void genQuadVao(bd::VertexArrayObject &vao, unsigned int numSlices)
{
    std::vector<glm::vec4> temp;
    std::vector<glm::vec4> vbuf;
    std::vector<glm::vec4> texbuf;
    std::vector<uint16_t> elebuf;

    /// For each axis, populate vbuf with verts for numSlices quads, adjust  ///
    /// z coordinate based on slice index.                                   ///

    create_verts_xy(numSlices, temp);
    std::copy(temp.begin(), temp.end(), std::back_inserter(vbuf));

    create_verts_xz(numSlices, temp);
    std::copy(temp.begin(), temp.end(), std::back_inserter(vbuf));

    create_verts_yz(numSlices, temp);
    std::copy(temp.begin(), temp.end(), std::back_inserter(vbuf));

    create_texbuf_xy(numSlices, temp);
    std::copy(temp.begin(), temp.end(), std::back_inserter(texbuf));

    create_texbuf_xz(numSlices, temp);
    std::copy(temp.begin(), temp.end(), std::back_inserter(texbuf));

    create_texbuf_yz(numSlices, temp);
    std::copy(temp.begin(), temp.end(), std::back_inserter(texbuf));

    create_elementIndices(numSlices*3, elebuf);
    g_elementBufferSize = elebuf.size();

    /// Add buffers to VAO ///
    // vertex positions into attribute 0
    vao.addVbo(reinterpret_cast<float *>(vbuf.data()), 
        vbuf.size() * bd::Quad::vert_element_size, bd::Quad::vert_element_size, 0);

    const size_t texbuf_element_size = 4;
    // vertex texcoords into attribute 1
    vao.addVbo(reinterpret_cast<float *>(texbuf.data()), 
        texbuf.size() * texbuf_element_size, texbuf_element_size, 1);
    
    // element index buffer
    vao.setIndexBuffer(elebuf.data(), elebuf.size());
}


///////////////////////////////////////////////////////////////////////////////
void genAxisVao(bd::VertexArrayObject &vao)
{
    // vertex positions into attribute 0
    vao.addVbo((float *)(bd::Axis::verts.data()),
        bd::Axis::verts.size() * bd::Axis::vert_element_size,
        bd::Axis::vert_element_size, 0);

    // vertex colors into attribute 1
    vao.addVbo((float *)(bd::Axis::colors.data()),
        bd::Axis::colors.size() * 3,
        3, 1);
}


///////////////////////////////////////////////////////////////////////////////
void genBoxVao(bd::VertexArrayObject &vao)
{
    // vertex positions into attribute 0
    vao.addVbo((float *)(bd::Box::vertices.data()),
        bd::Box::vertices.size() * bd::Box::vert_element_size,
        bd::Box::vert_element_size, 0);

    // vertex colors into attribute 1
    vao.addVbo((float *)(bd::Box::colors.data()),
        bd::Box::colors.size() * 3,
        3, 1);

    vao.setIndexBuffer((unsigned short *)(bd::Box::elements.data()),
        bd::Box::elements.size());

}


/////////////////////////////////////////////////////////////////////////////////
void initGraphicsState()
{
    gl_check(glClearColor(0.2f, 0.2f, 0.2f, 0.0f));

//    gl_check(glEnable(GL_CULL_FACE));
//    gl_check(glCullFace(GL_BACK));

    gl_check(glDepthFunc(GL_LESS));
    gl_check(glEnable(GL_DEPTH_TEST));

    gl_check(glEnable(GL_BLEND));
    gl_check(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    gl_check(glEnable(GL_PRIMITIVE_RESTART));
    gl_check(glPrimitiveRestartIndex(0xFFFF));
}


/////////////////////////////////////////////////////////////////////////////////
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
    //glfwWindowHint(GLFW_SAMPLES, 4);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(g_screenWidth, g_screenHeight, "Blocks", nullptr, nullptr);
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
    bd::subscribe_debug_callbacks();


    return window;
}


/////////////////////////////////////////////////////////////////////////////////
void cleanup()
{
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
void printBlocks()
{
    std::ofstream block_file("blocks.txt", std::ofstream::trunc);
    if (block_file.is_open()) {
        for (auto &b : g_blocks) {
            block_file << b << "\n";
        }
        block_file.flush();
        block_file.close();
    }
}


/////////////////////////////////////////////////////////////////////////////////
unsigned int loadTransfter_1dtformat(const std::string &filename, Texture &transferTex)
{
    gl_log("Reading transfer function file in .1dt format");

    std::ifstream file(filename.c_str(), std::ifstream::in);
    if (!file.is_open()) {
        gl_log_err("Caint open tfunc file: %s", filename.c_str());
        return 0;
    }

    size_t lineNum{ 0 };
    size_t numKnots{ 0 };

    file >> numKnots; // number of entries/lines in the 1dt file.
    lineNum++;
    if (numKnots > 8192) {
        gl_log_err("The 1dt transfer function has %d knots but max allowed is 8192)."
            "Skipping loading the transfer function file.", numKnots);
        return 0;
    }

    glm::vec4 *rgba{ new glm::vec4[numKnots] };
    // read rest of file consisting of rgba colors    
    float r, g, b, a;
    while (lineNum < numKnots && file >> r >> g >> b >> a) { 
        rgba[lineNum] = { r, g, b, a };
        lineNum++;
    }

    file.close();

    unsigned int texId{ 
        transferTex.genGLTex1d(reinterpret_cast<float*>(rgba), Texture::Format::RGBA,
            Texture::Format::RGBA, numKnots)
    };

    if (texId == 0) {
        gl_log_err("Could not make transfer function texture, returned id was 0.");
        return texId;
    }

    transferTex.textureUnit(1);

    unsigned int smp{ g_volumeShader.getUniformLocation("tf_sampler") };
    transferTex.samplerLocation(smp);

    delete [] rgba;

    return texId;
}


///////////////////////////////////////////////////////////////////////////////
void setupCameraPos(unsigned cameraPos)
{
    switch (cameraPos) {
    case 2:
        //cam position = { 2.0f, 0.0f, 0.0f  };
        g_rotation = glm::rotate(g_rotation, -1 * glm::half_pi<float>(), Y_AXIS);
        g_selectedSliceSet = SliceSet::YZ;
        break;
    case 1:
        //cam position = { 0.0f, 2.0f, 0.0f };
        g_rotation = glm::rotate(g_rotation, glm::half_pi<float>(), X_AXIS);
        g_selectedSliceSet = SliceSet::XZ;
        break;
    case 0:
    default:
        //cam position = { 0.0f, 0.0f, 2.0f };
        // no rotation needed, this is default cam location.
        g_selectedSliceSet = SliceSet::XY;
        break;
    }

    g_viewDirty = true;
}

/////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char *argv [])
{
    CommandLineOptions clo;
    if (parseThem(argc, argv, clo) == 0) {
        std::cout << "No arguments provided.\nPlease use -h for usage info." << std::endl;
        return 1;
    }

    printThem(clo);
    g_numSlices = clo.num_slices;
    bd::gl_log_restart();

    //// GLFW init ////
    GLFWwindow *window;
    if ((window = init()) == nullptr) {
        gl_log("Could not initialize GLFW, exiting.");
        return 1;
    }

    //// NV Perf Thing ////

    //// Shaders Init ////
    GLuint programId
    { 
        g_simpleShader.linkProgram
        (
            "shaders/vert_vertexcolor_passthrough.glsl",
            "shaders/frag_vertcolor.glsl"
        )
    };
    if (programId == 0) {
        gl_log_err("Error building passthrough shader, program id was 0.");
        return 1;
    }

    GLuint volumeProgramId
    { 
        g_volumeShader.linkProgram
        (
            "shaders/vert_vertexcolor_passthrough.glsl",
            "shaders/frag_volumesampler_noshading.glsl"
        )
    };

    if (volumeProgramId == 0) {
        gl_log_err("Error building volume sampling shader, program id was 0.");
        return 1;
    }

    //// Geometry Init ////
    bd::VertexArrayObject quadVbo(bd::VertexArrayObject::Method::ELEMENTS);
    quadVbo.create();

    bd::VertexArrayObject axisVbo(bd::VertexArrayObject::Method::ARRAYS);
    axisVbo.create();

    bd::VertexArrayObject boxVbo(bd::VertexArrayObject::Method::ELEMENTS);
    boxVbo.create();

    genQuadVao(quadVbo, clo.num_slices);
    genAxisVao(axisVbo);
    genBoxVao(boxVbo);

    g_vaoIds.resize(3);
    g_vaoIds[static_cast<unsigned int>(ObjType::Axis)] = &axisVbo;
    g_vaoIds[static_cast<unsigned int>(ObjType::Quads)] = &quadVbo;
    g_vaoIds[static_cast<unsigned int>(ObjType::Boxes)] = &boxVbo;


    //// Blocks and Data Init ////
    Block::initBlocks
    (
        glm::u64vec3( clo.numblk_x, clo.numblk_y, clo.numblk_z),
        glm::u64vec3( clo.w, clo.h, clo.d ), 
        g_blocks
    );

    std::unique_ptr<float []> data
    {
        std::move( bd::readVolumeData( clo.type, clo.filePath, clo.w, clo.h, clo.d ) )
    };
    
    if (data == nullptr) {
        gl_log_err("data file was not opened. exiting...");
        cleanup();
        return 1;
    }

    Block::filterBlocks
    ( 
        data.get(),                                               // data set
        g_blocks,                                                 // da blocks
        g_nonEmptyBlocks,                                         // da non empty blocks
//        glm::u64vec3( clo.numblk_x, clo.numblk_y, clo.numblk_z ), // number of blocks
//        glm::u64vec3( clo.w, clo.h, clo.d ),                      
        g_volumeShader.getUniformLocation("volume_sampler"),
        clo.tmin, 
        clo.tmax 
    );

    if (clo.printBlocks) { printBlocks(); }

    //// Transfer function texture ////
    unsigned int tfuncTextureId
    { 
        loadTransfter_1dtformat(clo.tfuncPath, g_tfuncTex) 
    };
    if (tfuncTextureId == 0) {
        gl_log_err("Exiting because tfunc texture was not bound.");
        exit(1);
    }

    //// Renderage ////
    setupCameraPos(clo.cameraPos);
    initGraphicsState();
    loop(window);
    cleanup();
    bd::gl_log_close();

    return 0;
}
