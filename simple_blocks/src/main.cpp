#include <GL/glew.h>
#include <GLFW/glfw3.h>

// local includes
//#include "block.h"
//#include "blockcollection.h"
#include "cmdline.h"
#include "create_vao.h"

// BD lib
#include <bd/geo/axis.h>
#include <bd/geo/BBox.h>
#include <bd/geo/quad.h>
#include <bd/graphics/shader.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/graphics/view.h>
#include <bd/log/gl_log.h>
#include <bd/util/util.h>
#include <bd/volume/block.h>
#include <bd/volume/blockcollection.h>



// GLM
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>

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


///////////////////////////////////////////////////////////////////////////////
//  Const  Data
///////////////////////////////////////////////////////////////////////////////
const glm::vec3 X_AXIS{ 1.0f, 0.0f, 0.0f };
const glm::vec3 Y_AXIS{ 0.0f, 1.0f, 0.0f };
const glm::vec3 Z_AXIS{ 0.0f, 0.0f, 1.0f };

const int g_elementsPerQuad{ 5 };


///////////////////////////////////////////////////////////////////////////////
// Geometry  /  VAOs
///////////////////////////////////////////////////////////////////////////////
enum class ObjType : unsigned int
{
    Axis, Quads, Boxes
};


enum class SliceSet : unsigned int
{
    XZ, YZ, XY, NoneOfEm, AllOfEm
};

std::ostream& operator<<(std::ostream &ostr, SliceSet s)
{
    switch (s)
    {
    case SliceSet::XZ: ostr << "XZ"; return ostr;
    case SliceSet::YZ: ostr << "YZ"; return ostr;
    case SliceSet::XY: ostr << "XY"; return ostr;
    default: ostr << "unknown"; return ostr;
    }
}


bd::Axis g_axis;
bd::Box g_box;

std::vector<bd::VertexArrayObject *> g_vaoIds;
size_t g_elementBufferSize{ 0 };

int g_numSlices{ 1 };



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
bd::View g_camera;
int g_screenWidth{ 1000 };
int g_screenHeight{ 1000 };
float g_fov_deg{ 50.0f };

glm::vec2 g_cursorPos;
float g_mouseSpeed{ 1.0f };

bool g_toggleBlockBoxes{ false };
bool g_toggleWireFrame{ false };

SliceSet g_selectedSliceSet{ SliceSet::XY };

//TODO: bool g_toggleVolumeBox{ false };


///////////////////////////////////////////////////////////////////////////////
//  Miscellaneous  radness
///////////////////////////////////////////////////////////////////////////////
unsigned long long g_totalGPUTime_nonEmptyBlocks{ 0 };
unsigned long long g_totalFramesRendered{ 0 };
double g_totalElapsedCPUFrameTime{ 0 };

bd::BlockCollection g_blocks;


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

#define QUERY_BUFFERS 2
#define QUERY_COUNT 1
GLuint queryID[QUERY_BUFFERS][QUERY_COUNT];
unsigned int queryBackBuffer = 0;
unsigned int queryFrontBuffer = 1;

/// \brief Init opengl queries for GPU frame times.
void genQueries()
{
    gl_check(glGenQueries(QUERY_COUNT, queryID[queryBackBuffer]));
    gl_check(glGenQueries(QUERY_COUNT, queryID[queryFrontBuffer]));
    // dummy query to prevent OpenGL errors from popping out
    //gl_check(glQueryCounter(queryID[queryFrontBuffer][0], GL_TIMESTAMP));
}

void swapQueryBuffers()
{
    if (queryBackBuffer) {
        queryBackBuffer = 0;
        queryFrontBuffer = 1;
    } else {
        queryBackBuffer = 1;
        queryFrontBuffer = 0;
    }
}

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
            g_toggleWireFrame = !g_toggleWireFrame; break;
        case GLFW_KEY_B:
            g_toggleBlockBoxes = !g_toggleBlockBoxes; break;
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
    g_camera.setViewport(0, 0, width, height);
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
    g_camera.setProjectionMatrix(glm::radians(fov), g_screenWidth / float(g_screenHeight),
        0.1f, 10000.0f);
}


/************************************************************************/
/*     D R A W I N'    S T U F F                                        */
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

    g_camera.rotate(rotX * rotY);
}


///////////////////////////////////////////////////////////////////////////////
void updateViewMatrix()
{
    g_camera.updateViewMatrix();
}


/////////////////////////////////////////////////////////////////////////////////
//void sortBlocksFarthestToNearest()
//{
//    glm::vec3 camPos = g_camera.getPosition();
//    std::sort(g_blocks.nonEmptyBlocks().begin(), g_blocks.nonEmptyBlocks().end(),
//        [&camPos](const Block *a, const Block *b)
//    {
//        float a_dist = glm::distance(camPos, a->transform().origin());
//        float b_dist = glm::distance(camPos, b->transform().origin());
//        return a_dist < b_dist;
//    });
//}


///////////////////////////////////////////////////////////////////////////////
void drawNonEmptyBoundingBoxes(const glm::mat4 &mvp)
{
    for (auto *b : g_blocks.nonEmptyBlocks()) {
        glm::mat4 mmvp = mvp * b->transform().matrix();
        g_simpleShader.setUniform("mvp", mmvp);
        gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid *)0));
        gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT,
            (GLvoid *)(4 * sizeof(GLushort))));
        gl_check(glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT,
            (GLvoid *)(8 * sizeof(GLushort))));
    } // for
}


///////////////////////////////////////////////////////////////////////////////
void drawSlices(size_t baseVertex)
{
    perf_workBegin();
    gl_check(glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, g_elementsPerQuad * g_numSlices, 
        GL_UNSIGNED_SHORT, 0, baseVertex));
    perf_workEnd();
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Loop through the blocks and draw each one
///////////////////////////////////////////////////////////////////////////////
void drawNonEmptyBlocks_Forward(const glm::mat4 &vp, bool drawReversed)
{
    size_t baseVertex{ 0 };
    perf_frameBegin();
    for (auto *b : g_blocks.nonEmptyBlocks()) {
        b->texture().bind(0);
        glm::mat4 wmvp = vp * b->transform().matrix();
        g_volumeShader.setUniform("mvp", wmvp);
        g_volumeShader.setUniform("tfScalingVal", g_scaleValue);

        switch (g_selectedSliceSet) {
//        case SliceSet::XY:
//            baseVertex = 0;                                           break;
        case SliceSet::XZ:
            baseVertex = bd::Quad::vert_element_size * g_numSlices;     break;
        case SliceSet::YZ:
            baseVertex = 2 * bd::Quad::vert_element_size * g_numSlices; break;
        default: break;
        } 
    }

    drawSlices(baseVertex);

    perf_frameEnd();
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Determine the viewing direction and draw the blocks in proper order.
///////////////////////////////////////////////////////////////////////////////
void drawNonEmptyBlocks(const glm::mat4 &vp)
{
    glm::vec4 viewdir{ glm::normalize(g_camera.getViewMatrix()[2]) };
    glm::vec4 absViewdir{ glm::abs(viewdir) };
    SliceSet previousSet{ g_selectedSliceSet };

    // Select current slice set based on longeset component of viewing vector.
    g_selectedSliceSet = SliceSet::YZ;
    float longest{ absViewdir.x };
    if (absViewdir.y > longest)
    {
        g_selectedSliceSet = SliceSet::XZ;
        longest = viewdir.y;
    }
    if (absViewdir.z > longest)
    {
        g_selectedSliceSet = SliceSet::XY;
        longest = absViewdir.z;
    }



//    if (absViewdir.x > absViewdir.y && absViewdir.x > absViewdir.z) {
//        g_selectedSliceSet = SliceSet::YZ;
//        longest = viewdir.x;
//    }
//    else if (absViewdir.y > absViewdir.x && absViewdir.y > absViewdir.z) {
//        g_selectedSliceSet = SliceSet::XZ;
//        longest = viewdir.y;
//    }
//    else if (absViewdir.z > absViewdir.x && absViewdir.z > absViewdir.y) {
//        g_selectedSliceSet = SliceSet::XY;
//        longest = viewdir.z;
//    }

//    sortBlocksFarthestToNearest();

    if (previousSet != g_selectedSliceSet) {
        std::cout << "Switched slice set: " << 
            (longest < 0 ? '-' : '+') <<  g_selectedSliceSet << '\n';
    }

    if (g_toggleWireFrame) {
        gl_check(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE))
    }

    g_volumeShader.bind();
    drawNonEmptyBlocks_Forward(vp, longest < 0);

    if (g_toggleWireFrame){
        gl_check(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
    }

}


///////////////////////////////////////////////////////////////////////////////
void draw(const glm::mat4 &vp)
{
    bd::VertexArrayObject *vao = nullptr;
    ////////  Axis    /////////////////////////////////////////
    vao = g_vaoIds[static_cast<unsigned int>(ObjType::Axis)];
    vao->bind();
    g_simpleShader.bind();
    g_simpleShader.setUniform("mvp", vp);
    g_axis.draw();
    vao->unbind();

    ////////  BBoxes  /////////////////////////////////////////
    if (g_toggleBlockBoxes) {
        vao = g_vaoIds[static_cast<unsigned int>(ObjType::Boxes)];
        vao->bind();
        drawNonEmptyBoundingBoxes(vp);
        vao->unbind();
    }

    //////// Quad Geo (drawNonEmptyBlocks)  /////////////////////
    vao = g_vaoIds[static_cast<unsigned int>(ObjType::Quads)];
    vao->bind();
    drawNonEmptyBlocks(vp);
    vao->unbind();
}


///////////////////////////////////////////////////////////////////////////////
std::chrono::system_clock::time_point startTime()
{
    using namespace std::chrono;
    return high_resolution_clock::now();
}


///////////////////////////////////////////////////////////////////////////////
void endTime(std::chrono::system_clock::time_point before)
{
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    g_totalElapsedCPUFrameTime += 
        duration_cast<microseconds>(now - before).count();
}


///////////////////////////////////////////////////////////////////////////////
void loop(GLFWwindow *window)
{
    gl_log("Entered render loop.");
    GLuint64 frame_gpuTime_nonEmptyBlocks{0};

    g_volumeShader.bind();
    g_tfuncTex.bind(1); 

    do {
        auto frame_startTime = startTime();

        g_camera.updateViewMatrix();

        gl_check(glBeginQuery(GL_TIME_ELAPSED, queryID[queryBackBuffer][0]));
        gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        draw(g_camera.getViewMatrix());
        glfwSwapBuffers(window);

        gl_check(glEndQuery(GL_TIME_ELAPSED));
        gl_check(glGetQueryObjectui64v(queryID[queryFrontBuffer][0], GL_QUERY_RESULT, 
            &frame_gpuTime_nonEmptyBlocks));
        swapQueryBuffers();

        g_totalGPUTime_nonEmptyBlocks += frame_gpuTime_nonEmptyBlocks;

        glfwPollEvents();
        endTime(frame_startTime);

        g_totalFramesRendered++;

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);


    gl_log("Render loop exited.");
}


///////////////////////////////////////////////////////////////////////////////
//  G E O M E T R Y   C R E A T I O N
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// \brief Create slices inside the cononical block for each axis.
///////////////////////////////////////////////////////////////////////////////
void genQuadVao(bd::VertexArrayObject &vao, unsigned int numSlices)
{
    gl_log("Generating quad slice vertex buffers for %d slices.", numSlices);
    std::vector<glm::vec4> temp;
    std::vector<glm::vec4> vbuf;
    std::vector<glm::vec4> texbuf;
    std::vector<uint16_t> elebuf;

    /// For each axis, populate vbuf with verts for numSlices quads, adjust  ///
    /// z coordinate based on slice index.                                   ///

    vert::create_verts_xy(numSlices, temp);
    std::copy(temp.begin(), temp.end(), std::back_inserter(vbuf));

    vert::create_verts_xz(numSlices, temp);
    std::copy(temp.begin(), temp.end(), std::back_inserter(vbuf));

    vert::create_verts_yz(numSlices, temp);
    std::copy(temp.begin(), temp.end(), std::back_inserter(vbuf));

    vert::create_texbuf_xy(numSlices, temp);
    std::copy(temp.begin(), temp.end(), std::back_inserter(texbuf));

    vert::create_texbuf_xz(numSlices, temp);
    std::copy(temp.begin(), temp.end(), std::back_inserter(texbuf));

    vert::create_texbuf_yz(numSlices, temp);
    std::copy(temp.begin(), temp.end(), std::back_inserter(texbuf));

    vert::create_elementIndices(numSlices, elebuf);
    g_elementBufferSize = elebuf.size();

    /// Add buffers to VAO ///
    // vertex positions into attribute 0
    vao.addVbo(reinterpret_cast<float *>(vbuf.data()), 
        vbuf.size() * bd::Quad::vert_element_size, bd::Quad::vert_element_size, 0);

    // vertex texcoords into attribute 1
    const size_t texcoord_element_size = 4;
    vao.addVbo(reinterpret_cast<float *>(texbuf.data()), 
        texbuf.size() * texcoord_element_size, texcoord_element_size, 1);
    
    // element index buffer
    vao.setIndexBuffer(elebuf.data(), elebuf.size());
}


///////////////////////////////////////////////////////////////////////////////
void genAxisVao(bd::VertexArrayObject &vao)
{
    gl_log("Generating axis vertex buffers.");
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
    gl_log("Generating bounding box vertex buffers.");
    // vertex positions into attribute 0
    vao.addVbo((float *)(bd::Box::vertices.data()),
        bd::Box::vertices.size() * bd::Box::vert_element_size,
        bd::Box::vert_element_size, 0);

    // vertex colors into attribute 1
    vao.addVbo((float *) bd::Box::colors.data(),
        bd::Box::colors.size() * 3,
        3, 1);

    vao.setIndexBuffer((unsigned short *) bd::Box::elements.data(),
        bd::Box::elements.size());

}


///////////////////////////////////////////////////////////////////////////////
//   I N I T I A L I Z A T I O N
///////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////
void initGraphicsState()
{
    gl_log("Initializing gl state.");
    gl_check(glClearColor(1.0f, 1.0f, 1.0f, 0.0f));

//    gl_check(glEnable(GL_CULL_FACE));
//    gl_check(glCullFace(GL_BACK));

//    gl_check(glEnable(GL_DEPTH_TEST));
//    gl_check(glDepthFunc(GL_LESS));

    gl_check(glEnable(GL_BLEND));
    gl_check(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    gl_check(glEnable(GL_PRIMITIVE_RESTART));
    gl_check(glPrimitiveRestartIndex(0xFFFF));
}


/////////////////////////////////////////////////////////////////////////////////
GLFWwindow* init()
{
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(g_screenWidth, g_screenHeight, "Blocks", 
        nullptr, nullptr);
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

    genQueries();

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
        gl_log("Writing blocks to blocks.txt in the current working directory.");
//        for (const bd::Block &b : g_blocks.blocks()) {
        for (auto &b : g_blocks.blocks()) {
            block_file << b << "\n";
        }
        block_file.flush();
        block_file.close();
    } 
    else {
        gl_log_err("Could not print blocks because blocks.txt coulnt'd be created in the current working directory.");
    }
}


/////////////////////////////////////////////////////////////////////////////////
void printTimes(std::ostream &str)
{
    float gputime_ms = g_totalGPUTime_nonEmptyBlocks * 1.0e-6f;
    float cputime_ms = g_totalElapsedCPUFrameTime * 1.0e-3f;

    str << 
        "frames_rendered: "       << g_totalFramesRendered << " frames\n"
        "gpu_ft_total_nonempty: " << gputime_ms << "ms\n"
        "gpu_ft_avg_nonempty: "   << (gputime_ms / float(g_totalFramesRendered)) << "ms\n"
        "cpu_ft_total: "          << cputime_ms << "ms\n"
        "cpu_ft_avg: "            << (cputime_ms / float(g_totalFramesRendered)) << "ms"
    << std::endl;
}


/////////////////////////////////////////////////////////////////////////////////
unsigned int loadTransfter_1dtformat(const std::string &filename, Texture &transferTex)
{
    gl_log("Reading 1dt formatted transfer function file and generating texture.");

    std::ifstream file(filename.c_str(), std::ifstream::in);
    if (!file.is_open()) {
        gl_log_err("Can't open tfunc file: %s", filename.c_str());
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

    unsigned int samp{ g_volumeShader.getUniformLocation("tf_sampler") };
    transferTex.samplerLocation(samp);

    delete [] rgba;

    return texId;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Set an initial camera location/orientation (-c command line option)
///////////////////////////////////////////////////////////////////////////////
void setupCameraPos(unsigned cameraPos)
{
    glm::quat r;
    switch (cameraPos) {
    case 2:
        //cam position = { 2.0f, 0.0f, 0.0f  }
        r = glm::rotate(r, -1 * glm::half_pi<float>(), Y_AXIS);
        g_camera.rotate(r);
        g_selectedSliceSet = SliceSet::YZ;
        break;
    case 1:
        //cam position = { 0.0f, 2.0f, 0.0f }
        r = glm::rotate(r , glm::half_pi<float>(), X_AXIS);
        g_camera.rotate(r);
        g_selectedSliceSet = SliceSet::XZ;
        break;
    case 0:
    default:
        //cam position = { 0.0f, 0.0f, 2.0f }
        // no rotation needed, this is default cam location.
        g_selectedSliceSet = SliceSet::XY;
        break;
    }

    //g_viewDirty = true;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief print counters from NvPmApi to file \c perfOutPath, or to \c stdout
///  if no path is provided.
///////////////////////////////////////////////////////////////////////////////
void printNvPmApiCounters(const char *perfOutPath = "")
{
    if (std::strlen(perfOutPath) == 0) {
        perf_printCounters(std::cout);
        printTimes(std::cout);
    }
    else{
        std::ofstream outStream(perfOutPath);
        if (outStream.is_open()) {
            perf_printCounters(outStream);
            printTimes(outStream);
        } 
        else {
            gl_log_err("Could not open %s for performance counter output. Using stdout instead.", 
                perfOutPath);
            perf_printCounters(std::cout);
            printTimes(std::cout);
        }
    }
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
    g_vaoIds[static_cast<unsigned int>(ObjType::Axis)]  = &axisVbo;
    g_vaoIds[static_cast<unsigned int>(ObjType::Quads)] = &quadVbo;
    g_vaoIds[static_cast<unsigned int>(ObjType::Boxes)] = &boxVbo;


    //// Blocks and Data Init ////
    g_blocks.initBlocks
    (
        glm::u64vec3( clo.numblk_x, clo.numblk_y, clo.numblk_z),
        glm::u64vec3( clo.w, clo.h, clo.d )

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

    g_blocks.filterBlocks
    ( 
        data.get(),                                               // data set
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

    //// Render Init and Loop ////
    setupCameraPos(clo.cameraPos);
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
