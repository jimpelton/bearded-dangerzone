
#include "simplecontextcontroller.h"


#include <log/gl_log.h>
#include <util/shader.h>
#include <util/context.h>
#include <util/glfwcontext.h>
#include <graphics/vertexarrayobject.h>
#include <graphics/quad.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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
//"    out_col = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
"}";

SimpleContextController::SimpleContextController() 
    : ContextController()
{
}

SimpleContextController::~SimpleContextController() 
{
}

void SimpleContextController::renderLoop(bd::Context &context)
{
    bd::GlfwContext *c = reinterpret_cast<bd::GlfwContext*>(&context);  // crap.
    GLFWwindow *window = c->window();
    
    using namespace bd;
    
    Shader vert{ ShaderType::Vertex };
    vert.loadFromString(g_vertStr);
    Shader frag{ ShaderType::Fragment };
    frag.loadFromString(g_fragStr);
    ShaderProgram prog{ &vert, &frag };
    prog.linkProgram();

    VertexArrayObject quad;
    std::vector<glm::vec4> qverts(Quad::verts.begin(), Quad::verts.end());
    quad.addVbo(qverts, 0);
    std::vector<unsigned short> elems(Quad::elements.begin(), Quad::elements.end());
    quad.setIndexBuffer(elems);
    
    const std::vector<glm::vec3> qcolors{
        { 0.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 1.0 }
    };
    quad.addVbo(qcolors, 1);

    quad.bind();
    prog.bind();

    m_view.setProjectionMatrix(50.0f, 1280.f / 720.f, 0.1f, 100.f);
    m_view.setPosition(glm::vec3(0, 0, 10));



    do 
    {
        gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        m_view.updateViewMatrix();
        glm::mat4 mvp{
            m_view.getProjectionMatrix() * m_view.getViewMatrix()
        };
        prog.setUniform("mvp", mvp);

        gl_check(glDrawElements(GL_TRIANGLE_STRIP, Quad::verts.size(), GL_UNSIGNED_SHORT, 0));

        context.swapBuffers();
        context.pollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);
    
    
}

void SimpleContextController::cursorpos_callback(double x, double y)
{
    glm::vec2 cpos{ floor(x), floor(y) };
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        glm::vec2 delta { cpos - m_cursorPos };
        glm::quat rotX = glm::angleAxis<float>(
            glm::radians(delta.y) * 1.0,
            glm::vec3(1, 0, 0)
            );

        glm::quat rotY = glm::angleAxis<float>(
            glm::radians(delta.x) * 1.0,
            glm::vec3(0, 1, 0)
            );
        m_view.setRotation(rotX * rotY);
    }
    m_cursorPos = cpos;
}

void SimpleContextController::keyboard_callback(int key, int scancode, int action, int mods)
{
    
}

void SimpleContextController::window_size_callback(int width, int height)
{
    m_view.setViewport( 0, 0, width, height );
}

void SimpleContextController::scrollwheel_callback(double xoff, double yoff)
{
    
}

void SimpleContextController::error_callback(int error, const char* description)
{
    std::cerr << "Error: [" << error << "] Desc: " << description << std::endl;
}

void SimpleContextController::setWindow(GLFWwindow * w)
{
    m_window = w;
}
