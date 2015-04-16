
#include "volrendloop.h"
#include "block.h"

#include <GLFW/glfw3.h>

#include <bd/util/glfwcontext.h>
#include <bd/log/gl_log.h>
#include <bd/graphics/shader.h>

#include <vector>
#include <iostream>


namespace
{
    bd::GlfwContext *m_ctx{ nullptr };
    GLFWwindow *m_window{ nullptr };

    const std::string g_vertStr =
            "#version 400\n"
                    "in vec3 vert;"
                    "in vec3 col;"
                    "uniform mat4 mvp;"
                    "out vec3 color;"
                    "void main() { "
                    "    gl_Position = mvp * vec4(vert, 1.0f);"
                    "    color = col;"
                    "}";

    const std::string g_fragStr =
            "#version 400\n"
                    "in vec3 color;"
                    "out vec4 out_col;"
                    "void main() {"
                    "    out_col = vec4(color, 1.0f);"
//                    "    out_col = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
                    "}";
};


//////////////////////////////////////////////////////////////////////////
VolRendLoop::VolRendLoop()
    : m_mouseSpeed{1.0f}
    , m_cursorPos{ }

    , m_vol{ nullptr }

    , m_volshader_vertex{ bd::ShaderType::Vertex }
    , m_volshader_fragment{ bd::ShaderType::Fragment }
    , m_volshader_program{ }

    , m_slices_vao{ }
{
}


//////////////////////////////////////////////////////////////////////////
VolRendLoop::~VolRendLoop() {}

void VolRendLoop::initialize(bd::Context &c)
{
    bd::GlfwContext *glfwContext = dynamic_cast<bd::GlfwContext*>(&c);
    assert(glfwContext != nullptr);

    m_ctx = glfwContext;
    m_window = glfwContext->window();
}

//////////////////////////////////////////////////////////////////////////
void VolRendLoop::renderLoop()
{
    assert(m_vol != nullptr);

    view().setProjectionMatrix(70.0f, 1280.f / 720.f, 0.1f, 100.f);
    view().setPosition(glm::vec3(0, 0, 50));

    do {

        m_vol->update();
        view().updateViewMatrix();

        m_volshader_program.bind();
        m_slices_vao.bind();

        gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        drawBlocks();

        m_slices_vao.unbind();
        m_volshader_program.unbind();

        m_ctx->swapBuffers();
        m_ctx->pollEvents();

    } while ( glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
              glfwWindowShouldClose(m_window) == 0  );

}

///////////////////////////////////////////////////////////////////////////////////
// Callbacks
///////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
void VolRendLoop::cursorpos_callback(double x, double y)
{
    glm::vec2 cpos{ std::floor(x), std::floor(y) };

    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        ///////  OBJECT ROTATION LEFT BUTTON  ///////
        glm::vec2 delta{ cpos - m_cursorPos };

        glm::quat rotX
        {
            glm::angleAxis<float>(glm::radians(delta.y) * m_mouseSpeed,
                glm::vec3(1, 0, 0))
        };

        glm::quat rotY
        {
            glm::angleAxis<float>(glm::radians(delta.x) * m_mouseSpeed,
                glm::vec3(0, 1, 0))
        };

        m_vol->transform().rotate(rotX * rotY);
//        m_vol->transform().scale(glm::vec3(1.5f,1.5f,1.5f));

    } else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        ///////  CAMERA ROTATION RIGHT BUTTON  ///////
        glm::vec2 delta{ cpos - m_cursorPos };

        glm::quat rotX
        {
            glm::angleAxis<float>(glm::radians(delta.y) * m_mouseSpeed,
                glm::vec3(1, 0, 0))
        };

        glm::quat rotY
        {
            glm::angleAxis<float>(glm::radians(delta.x) * m_mouseSpeed,
                glm::vec3(0, 1, 0))
        };

        view().rotate(rotX * rotY);
    }

    m_cursorPos = cpos;
}


//////////////////////////////////////////////////////////////////////////
void VolRendLoop::keyboard_callback(int key, int scancode, int action, int mods)
{    
}


//////////////////////////////////////////////////////////////////////////
void VolRendLoop::window_size_callback(int width, int height)
{
    view().setViewport(0, 0, width, height);
}


//////////////////////////////////////////////////////////////////////////
void VolRendLoop::scrollwheel_callback(double xoff, double yoff)
{
    
}


void VolRendLoop::addVbaContext(const std::vector<glm::vec4> &verts,
    const std::vector<unsigned short> &indices)
{
    const std::vector<glm::vec3> qcolors{
            { 0.0, 0.0, 0.0 },
            { 1.0, 0.0, 0.0 },
            { 0.0, 1.0, 0.0 },
            { 0.0, 0.0, 1.0 }
    };

    m_slices_vao.addVbo(verts, 0);
    m_slices_vao.addVbo(qcolors, 1);
    m_slices_vao.setIndexBuffer(indices);
}


void VolRendLoop::makeVolumeRenderingShaders(const std::string &vert_path, 
    const std::string &frag_path)
{
//    m_volshader_vertex.loadFromFile(vert_path);
//    m_volshader_fragment.loadFromFile(frag_path);

    m_volshader_vertex.loadFromString(g_vertStr);
    m_volshader_fragment.loadFromString(g_fragStr);

    unsigned int shaderId
    {
        m_volshader_program.linkProgram(&m_volshader_vertex,
            &m_volshader_fragment)
    };

    if (shaderId == 0) {
        gl_log_err("Could not create volume rendering shader program, "
                           "id returned was 0.");
    } else {
        gl_log("Created volume rendering shader program, id=%d", shaderId);
    }

}

void VolRendLoop::drawBlocks()
{
    using bd::Quad;
    //const std::vector<Block> &col = m_vol.collection().blocks();

    for (auto *t : m_vol->children()) {
        glm::mat4 mvp
        {
            view().getProjectionMatrix() *
            view().getViewMatrix() *
            t->transform().matrix()
        };

        m_volshader_program.setUniform("mvp", mvp);

        gl_check(glDrawElements(GL_TRIANGLE_STRIP, Quad::verts.size(), GL_UNSIGNED_SHORT, 0));
    }
}


void VolRendLoop::root(bd::Transformable *v)
{ m_vol = v;}