#include "block.h"
#include "cmdline.h"

#include <bd/log/gl_log.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/graphics/BBox.h>
#include <bd/util/util.h>
#include <bd/graphics/axis.h>
#include <bd/graphics/quad.h>
#include <bd/file/datareader.h>
#include <bd/file/datatypes.h>
#include <bd/graphics/shader.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <vector>
#include <array>
#include <memory>

#include <sstream>
#include <fstream>

#include <cstdarg>
#include <ctime>
#include <iostream>

const glm::vec3 X_AXIS{1.0f, 0.0f, 0.0f};
const glm::vec3 Y_AXIS{0.0f, 1.0f, 0.0f};
const glm::vec3 Z_AXIS{0.0f, 0.0f, 1.0f};

enum class SliceSet
{
    XZ, YZ, XY, NoneOfEm, AllOfEm
};
SliceSet g_selectedSliceSet{SliceSet::XZ};
bool g_toggleBlockBoxes{ false };
//TODO: bool g_toggleVolumeBox{ false };

bd::ShaderProgram g_simpleShader;
bd::ShaderProgram g_volumeShader;


std::vector<bd::VertexArrayObject *> g_vaoIds;
std::vector<Block> g_blocks;
std::vector<Block*> g_nonEmptyBlocks;

glm::quat g_rotation;
glm::mat4 g_viewMatrix;
glm::mat4 g_projectionMatrix;
glm::mat4 g_vpMatrix;
glm::vec3 g_camPosition{0.0f, 0.0f, -2.0f};
glm::vec3 g_camFocus{0.0f, 0.0f, 0.0f};
glm::vec3 g_camUp{0.0f, 1.0f, 0.0f};
glm::vec2 g_cursorPos;

float g_mouseSpeed{1.0f};
int g_screenWidth{1000};
int g_screenHeight{1000};
float g_fov_deg{50.0f};
bool g_viewDirty{true};
bool g_modelDirty{true};

bd::Axis g_axis;
bd::Box g_box;


//GLuint loadShader(GLenum type, std::string filepath);

//GLuint compileShader(GLenum type, const char *shader);

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
    if (action != GLFW_PRESS) {
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
    float fov = static_cast<float>(g_fov_deg +(yoff*1.75f));

    std::cout << "fov: " << fov << std::endl;

    if (fov<1 || fov>120) return;

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
    g_modelDirty = true;

}




///////////////////////////////////////////////////////////////////////////////
void loop(GLFWwindow *window)
{
    glm::mat4 mvp{1.0f};
    bd::VertexArrayObject *vao = nullptr;


    do {
        if (g_viewDirty) {
            updateViewMatrix();
        }

        if (g_modelDirty) {
            mvp = g_vpMatrix * glm::toMat4(g_rotation);
            g_modelDirty = false;
        }

        gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        g_simpleShader.bind();
        // Coordinate Axis
        vao = g_vaoIds[0];
        vao->bind();
        g_simpleShader.setUniform("mvp", mvp);
        g_axis.draw();
        vao->unbind();

        /// wireframe boxs ///
        vao = g_vaoIds[2];
        vao->bind();
        for (auto *b : g_nonEmptyBlocks) {

            glm::mat4 mmvp = mvp * b->transform().matrix();
            g_simpleShader.setUniform("mvp", mmvp);

            gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0));
            gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT,
                (GLvoid *) (4 * sizeof(GLushort))));
            gl_check(glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT,
                (GLvoid *) (8 * sizeof(GLushort))));

        } // for
        vao->unbind();

        g_volumeShader.bind();
        /// Quad geometry ///
        vao = g_vaoIds[1];
        vao->bind();
        for (auto *b : g_nonEmptyBlocks) {
            b->texture().bind();
            glm::mat4 mmvp = mvp * b->transform().matrix();
            //g_simpleShader.setUniform("mvp", mmvp);
            g_volumeShader.setUniform("mvp", mmvp);

            switch (g_selectedSliceSet) {
            case SliceSet::XY:
                gl_check( glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0));
                break;
            case SliceSet::YZ:
                gl_check( glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT,
                    (GLvoid *) (4 * sizeof(unsigned short))));
                break;
            case SliceSet::XZ:
                gl_check( glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT,
                        (GLvoid *) (8 * sizeof(unsigned short))));
                break;
            case SliceSet::AllOfEm:
                gl_check( glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0));
                gl_check( glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT,
                    (GLvoid *) (4 * sizeof(unsigned short))));
                gl_check( glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT,
                        (GLvoid *) (8 * sizeof(unsigned short))));
                break;
            case SliceSet::NoneOfEm:
            default:
                break;
            } // switch

        } // for
        vao->unbind();


        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);
}


/////////////////////////////////////////////////////////////////////////////////
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

    window = glfwCreateWindow(g_screenWidth, g_screenHeight, "Blocks", NULL, NULL);
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

    bd::subscribe_debug_callbacks();

    gl_check(glEnable(GL_DEPTH_TEST));
    gl_check(glDepthFunc(GL_LESS));
    gl_check(glClearColor(0.1f, 0.1f, 0.1f, 0.0f));

    return window;
}


///////////////////////////////////////////////////////////////////////////////
void genQuadVao(bd::VertexArrayObject &vao)
{
    // copy 3 sets of quad verts, each aligned with different plane
    std::array<glm::vec4, 12> vbuf;
    // x-y quad
    auto vbufIter =
        std::copy(bd::Quad::verts_xy.begin(), bd::Quad::verts_xy.end(), vbuf.begin());

    // x-z quad
    vbufIter =
        std::copy(bd::Quad::verts_xz.begin(), bd::Quad::verts_xz.end(), vbufIter);

    // y-z quad
    std::copy(bd::Quad::verts_yz.begin(), bd::Quad::verts_yz.end(), vbufIter);

    // copy the bd::Quad vertex colors 3 times
    std::array<glm::vec3, 12> colorBuf;

    auto colorIter =
        std::copy(bd::Quad::colors.begin(), bd::Quad::colors.end(), colorBuf.begin());

    colorIter =
        std::copy(bd::Quad::colors.begin(), bd::Quad::colors.end(), colorIter);

    std::copy(bd::Quad::colors.begin(), bd::Quad::colors.end(), colorIter);

    std::array<glm::vec3, 12> texcoords{
        glm::vec3(0.0f, 0.0f, 0.0f), // 0 ll
        glm::vec3(1.0, 0.0f, 0.0f), // 1 lr
        glm::vec3(1.0f, 1.0f, 0.0f), // 2 ur
        glm::vec3(0.0f,  1.0f, 0.0f),  // 3 ul


        glm::vec3(0.0f, 0.0f, 0.0f), // 0 ll
        glm::vec3(1.0f, 0.0f, 0.0f), // 1 lr
        glm::vec3(1.0f, 0.0f, 1.0f), // 2 ur
        glm::vec3(0.0f, 0.0f, 1.0f),  // 3 ul


        glm::vec3(0.0f, 0.0f, 0.0f), // 0 ll
        glm::vec3(0.0f, 1.0f, 0.0f), // 1 lr
        glm::vec3(0.0f, 1.0f, 1.0f), // 2 ur
        glm::vec3(0.0f, 0.0f, 1.0f)  // 3 ul
    };


    // Element index buffer
    std::array<unsigned short, 12> ebuf{
        0, 1, 3, 2,     // x-y
        4, 5, 7, 6,     // x-z
        8, 9, 11, 10    // y-z
    };

    // vertex positions into attribute 0
    vao.addVbo((float *) vbuf.data(), vbuf.size() * bd::Quad::vert_element_size,
        bd::Quad::vert_element_size, 0);

    // vertex colors into attribute 1
//    vao.addVbo((float *) (colorBuf.data()), colorBuf.size() * 3, 3, 1);
    vao.addVbo((float *) (texcoords.data()), texcoords.size() * 3, 3, 1);

    vao.setIndexBuffer((unsigned short *) (ebuf.data()), ebuf.size());
}


///////////////////////////////////////////////////////////////////////////////
void genAxisVao(bd::VertexArrayObject &vao)
{
    // vertex positions into attribute 0
    vao.addVbo((float *) (bd::Axis::verts.data()),
        bd::Axis::verts.size() * bd::Axis::vert_element_size,
        bd::Axis::vert_element_size, 0);

    // vertex colors into attribute 1
    vao.addVbo((float *) (bd::Axis::colors.data()),
        bd::Axis::colors.size() * 3,
        3, 1);
}


///////////////////////////////////////////////////////////////////////////////
void genBoxVao(bd::VertexArrayObject &vao)
{
    // vertex positions into attribute 0
    vao.addVbo((float *) (bd::Box::vertices.data()),
        bd::Box::vertices.size() * bd::Box::vert_element_size,
        bd::Box::vert_element_size, 0);

    // vertex colors into attribute 1
    vao.addVbo((float *) (bd::Box::colors.data()),
        bd::Box::colors.size() * 3,
        3, 1);

    vao.setIndexBuffer((unsigned short *) (bd::Box::elements.data()),
        bd::Box::elements.size());

}

///////////////////////////////////////////////////////////////////////////////
// bs: number of blocks
// vol: volume voxel dimensions
void initBlocks(glm::u64vec3 nb, glm::u64vec3 vd)
{
    // block world dims
    glm::vec3 blk_dims{1.0f / glm::vec3(nb)};

    gl_log("Starting block init: Number of blocks: %dx%dx%d, "
        "Volume dimensions: %dx%dx%d Block dimensions: %.2f,%.2f,%.2f",
        nb.x, nb.y, nb.z,
        vd.x, vd.y, vd.z,
        blk_dims.x, blk_dims.y, blk_dims.z);

    // Loop through block coordinates and populate block fields.
    for (auto bz = 0ul; bz < nb.z; ++bz)
    for (auto by = 0ul; by < nb.y; ++by)
    for (auto bx = 0ul; bx < nb.x; ++bx) {

        // i,j,k block identifier
        glm::u64vec3 blkId{bx, by, bz};
        // lower left corner in world coordinates
        glm::vec3 worldLoc{(blk_dims * glm::vec3(blkId)) - 0.5f}; // - 0.5f;
        // origin in world coordiates
        glm::vec3 blk_origin{(worldLoc + (worldLoc + blk_dims)) * 0.5f};

        Block blk{glm::u64vec3(bx, by, bz), blk_dims, blk_origin};
        g_blocks.push_back(blk);
    }

    gl_log("Finished block init: total blocks is %d.", g_blocks.size());
}


/////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<float[]> readData(const std::string &dtype, const std::string &fpath,
    size_t volx, size_t voly, size_t volz)
{
    bd::DataType t = bd::DataTypesMap.at(dtype);
    float *rawdata = nullptr;
    switch (t) {
    case bd::DataType::Float:
    {
        bd::DataReader<float, float> reader;
        reader.loadRaw3d(fpath, volx, voly, volz);
        rawdata = reader.takeOwnership();
        break;
    }
    case bd::DataType::UnsignedCharacter:
    {
        bd::DataReader<unsigned char, float> reader;
        reader.loadRaw3d(fpath, volx, voly, volz);
        rawdata = reader.takeOwnership();
        break;
    }
    case bd::DataType::UnsignedShort:
    {
        bd::DataReader<unsigned short, float> reader;
        reader.loadRaw3d(fpath, volx, voly, volz);
        rawdata = reader.takeOwnership();
        break;
    }
    default:
        break;
    }

    return std::unique_ptr<float[]>(rawdata);
}


/////////////////////////////////////////////////////////////////////////////////
void filterBlocks(float *data, std::vector<Block> &blocks, glm::u64vec3 numBlks,
      glm::u64vec3 volsz, float tmin=0.1f, float tmax=0.9f)
{
    size_t emptyCount { 0 };
    glm::u64vec3 bsz{ volsz / numBlks };
    size_t blkPoints = bd::vecCompMult(bsz);

    std::vector<float> image;
    image.reserve(blkPoints);

    for (auto &b : blocks) {
        glm::u64vec3 bst{ b.ijk() * bsz }; // block start = block index * block size
        float avg{ 0.0f };

        size_t imageIdx = 0;
        for (auto k = bst.z; k < bst.z + bsz.z; ++k)
        for (auto j = bst.y; j < bst.y + bsz.y; ++j)
        for (auto i = bst.x; i < bst.x + bsz.x; ++i) {
            size_t dataIdx{ bd::to1D(i, j, k, volsz.x, volsz.y) };
            float val = data[dataIdx];
            image[imageIdx++] = val;
            avg += val;
        } // for for for

        avg /= blkPoints;
        b.avg(avg);

        if (avg < tmin || avg > tmax) {
            b.empty(true);
            emptyCount += 1;
        } else {
            unsigned int smp =  g_volumeShader.getParamLocation("volume_sampler");
            b.texture().samplerLocation(smp);
            b.texture().textureUnit(0);
            b.texture().genGLTex3d(image.data(),
                Texture::Format::RED, Texture::Format::RED,
                bsz.x, bsz.y, bsz.z);

            g_nonEmptyBlocks.push_back(&b);
        }
    } // for auto

    // TODO: create list of pointers to non-empty blocks.

    gl_log("%d/%d blocks removed.", emptyCount, bd::vecCompMult(numBlks));

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
int main(int argc, const char *argv[])
{
   CommandLineOptions clo;
   if (parseThem(argc, argv, clo) == 0) {
       return 0;
   }

    bd::gl_log_restart();

    GLFWwindow *window;
    if ((window = init()) == nullptr) {
        gl_log("Could not initialize GLFW, exiting.");
        return 1;
    }

    GLuint programId = g_simpleShader.linkProgram(
        "shaders/vert_vertexcolor_passthrough.glsl",
        "shaders/frag_vertcolor.glsl"
    );

    if (programId == 0) {
        gl_log_err("Error building passthrough shader, program id was 0.");
        return 1;
    }

    GLuint volumeProgramId = g_volumeShader.linkProgram(
        "shaders/vert_vertexcolor_passthrough.glsl",
        "shaders/frag_volumesampler_noshading.glsl"
    );

    if (volumeProgramId == 0) {
        gl_log_err("Error building volume sampling shader, program id was 0.");
        return 1;
    }

    bd::VertexArrayObject quadVbo(bd::VertexArrayObject::Method::ELEMENTS);
    quadVbo.create();

    bd::VertexArrayObject axisVbo(bd::VertexArrayObject::Method::ARRAYS);
    axisVbo.create();

    bd::VertexArrayObject boxVbo(bd::VertexArrayObject::Method::ELEMENTS);
    boxVbo.create();

    genQuadVao(quadVbo);
    genAxisVao(axisVbo);
    genBoxVao(boxVbo);

    g_vaoIds.push_back(&axisVbo);
    g_vaoIds.push_back(&quadVbo);
    g_vaoIds.push_back(&boxVbo);

    initBlocks( glm::u64vec3{clo.numblk_x, clo.numblk_y, clo.numblk_z},
        glm::u64vec3{clo.w, clo.h, clo.d}
    );

    std::unique_ptr<float[]> data {
        std::move( readData(clo.type, clo.filePath, clo.w, clo.h, clo.d) )
    };

    filterBlocks(data.get(), g_blocks,
        {clo.numblk_x, clo.numblk_y, clo.numblk_z},
        {clo.w, clo.h, clo.d},
        clo.tmin, clo.tmax
    );

    if (clo.printBlocks) {
        printBlocks();
    }

    loop(window);
    cleanup();

    bd::gl_log_close();

    glfwTerminate();

    return 0;
}
