
#include "simplerenderloop.h"


#include <bd/log/gl_log.h>
#include <bd/graphics/shader.h>
#include <bd/util/context.h>
#include <bd/util/glfwcontext.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/graphics/quad.h>
#include <bd/scene/renderstate.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <ctime>

//namespace
//{

bd::Shader vert{ bd::ShaderType::Vertex };
bd::Shader frag{ bd::ShaderType::Fragment };
bd::ShaderProgram prog;
bd::VertexArrayObject vao{ bd::VertexArrayObject::Method::ELEMENTS };
bd::RenderState *state { nullptr };

bd::Transformable root;

std::vector<glm::vec4> vertices;
std::vector<unsigned short> indices;
std::vector<glm::vec4> qverts(bd::Quad::verts.begin(), bd::Quad::verts.end());
std::vector<glm::vec3> colors(bd::Quad::colors.begin(), bd::Quad::colors.end());
std::vector<unsigned short> elems(bd::Quad::elements.begin(), bd::Quad::elements.end());

glm::vec2 m_cursorPos;

GLFWwindow *m_window { nullptr };
bd::GlfwContext *m_ctx { nullptr };

//} // namespace

SimpleRenderLoop::SimpleRenderLoop()
    : RenderLoop()
{
}

SimpleRenderLoop::~SimpleRenderLoop()
{
}

double SimpleRenderLoop::getTime()
{
    static float totalTime = 0.0f;
    static std::clock_t previousTime = std::clock();
    std::clock_t currentTime = std::clock();
    float deltaTime = (currentTime - previousTime) / (float)CLOCKS_PER_SEC;
    previousTime = currentTime;
    totalTime += deltaTime;
    
    return totalTime;
}


void SimpleRenderLoop::initialize(bd::Context &context)
{
    bd::GlfwContext *c = dynamic_cast<bd::GlfwContext*>(&context);  // oops.
    if (c == nullptr) {
        gl_log_err("The dynamic_cast failed in initialize().");
        return;
    }

    m_ctx = c;
    m_window = c->window();

    vert.loadFromString(m_vertStr);
    frag.loadFromString(m_fragStr);

    if (prog.linkProgram(&vert, &frag) == 0) {
        gl_log_err("could not link shader program in renderLoop!");
        return;
    }

    if (vao.create() != 0) {
        vao.addVbo(qverts, 0);
        vao.addVbo(colors, 1);
        vao.setIndexBuffer(elems);
    }

    state = new bd::RenderState(&vao, &prog);

}

void SimpleRenderLoop::renderLoop()
{
    using namespace bd;

//    view().setProjectionMatrix(50.0f, 1280.f / 720.f, 0.1f, 10000.f);
//    view().setPosition(glm::vec3(5, 5, -5));
    glm::mat4 proj { glm::perspective(60.0f, 1280.0f / 720.0f, 0.1f, 100.0f)};
    glm::mat4 view { glm::lookAt(glm::vec3(0,0,-100), glm::vec3(0,0,0), glm::vec3(0,1,0)) };

    /// render loop ///
    do 
    {
        double totalTime = getTime();

//        glm::vec3 quad_scale{ ::cos(totalTime) + 1, ::sin(totalTime) + 1, 0.0f };
//        m_root->transform().scale(quad_scale);
//        view().updateViewMatrix();
//        m_root->update();

        glm::mat4 mvp
        { 
//            view().getProjectionMatrix() *
//            view().getViewMatrix() *
//            m_root->transform().matrix()
            proj * view
        };


        state->bind();
        prog.setUniform("mvp", mvp);
        state->draw();
        state->unbind();

        m_ctx->swapBuffers();
        m_ctx->pollEvents();

    } while ( glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
              glfwWindowShouldClose(m_window) == 0  );
}

void SimpleRenderLoop::cursorpos_callback(double x, double y)
{
    glm::vec2 cpos{ floor(x), floor(y) };
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        ///////  CAMERA ROTATION LEFT BUTTON  ///////  
        glm::vec2 delta { cpos - m_cursorPos };
        
        glm::quat rotX = glm::angleAxis<float>(
            glm::radians(delta.y) * 1.0f,
            glm::vec3(1, 0, 0)
            );

        glm::quat rotY = glm::angleAxis<float>(
            glm::radians(delta.x) * 1.0f,
            glm::vec3(0, 1, 0)
            );

//        view().rotate(rotX * rotY);

    } else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        ///////  OBJECT ROTATION RIGHT BUTTON  ///////  
        glm::vec2 delta{ cpos - m_cursorPos };

        glm::quat rotX = glm::angleAxis<float>(
            glm::radians(delta.y) * 1.0f,
            glm::vec3(1, 0, 0)
            );

        glm::quat rotY = glm::angleAxis<float>(
            glm::radians(delta.x) * 1.0f,
            glm::vec3(0, 1, 0)
            );

//        m_root->transform().rotate(rotX * rotY);

    } else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        ///////  OBJECT MOVEMENT MIDDLE BUTTON  ///////  
        glm::vec2 delta{ cpos - m_cursorPos};
        glm::vec3 dt{ -delta.x, delta.y, 0.0f }; 
        dt *= 0.001f;

//        m_root->transform().translate(dt);
    }

    m_cursorPos = cpos;
}

void SimpleRenderLoop::keyboard_callback(int key, int scancode, int action, int mods)
{
    
}

void SimpleRenderLoop::window_size_callback(int width, int height)
{
    view().setViewport( 0, 0, width, height );
}

void SimpleRenderLoop::scrollwheel_callback(double xoff, double yoff)
{
    
}

void SimpleRenderLoop::error_callback(int error, const char* description)
{
    std::cerr << "Error: [" << error << "] Desc: " << description << std::endl;
}

void SimpleRenderLoop::setRoot(bd::Transformable *t)
{
    m_root = t;
}
