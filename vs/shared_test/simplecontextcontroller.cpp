
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

#include <cmath>
#include <ctime>

namespace 
{
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

    const std::vector<glm::vec3> qcolors{
        { 0.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 1.0 }
    };

    bd::Shader vert{ bd::ShaderType::Vertex };
    bd::Shader frag{ bd::ShaderType::Fragment };
    bd::ShaderProgram prog;
    bd::VertexArrayObject quad_vbo;
    bd::Quad quad_geo;
    glm::vec2 m_cursorPos;

} // namespace 

SimpleContextController::SimpleContextController() 
    : ContextController()
{
}

SimpleContextController::~SimpleContextController() 
{
}

double SimpleContextController::getTime()
{
    static float totalTime = 0.0f;
    static std::clock_t previousTime = std::clock();
    std::clock_t currentTime = std::clock();
    float deltaTime = (currentTime - previousTime) / (float)CLOCKS_PER_SEC;
    previousTime = currentTime;
    totalTime += deltaTime;
    
    return totalTime;
}

void SimpleContextController::renderLoop(bd::Context &context)
{
    bd::GlfwContext *c = dynamic_cast<bd::GlfwContext*>(&context);  // oops.
    if (c == nullptr) {
        gl_log_err("dynamic_cast failed in renderLoop().");
        return;
    }

    GLFWwindow *window = c->window();
    
    using namespace bd;
   
    vert.loadFromString(g_vertStr);
    frag.loadFromString(g_fragStr);
    if (prog.linkProgram(&vert, &frag) == 0) {
        gl_log_err("could not link shader program in renderLoop!");
        return;
    }

    std::vector<glm::vec4> qverts(bd::Quad::verts.begin(), bd::Quad::verts.end());
    std::vector<unsigned short> elems(bd::Quad::elements.begin(), bd::Quad::elements.end());

    quad_vbo.addVbo(qverts, 0);
    quad_vbo.setIndexBuffer(elems);
    quad_vbo.addVbo(qcolors, 1);

    quad_vbo.bind();
    prog.bind();

    view().setProjectionMatrix(50.0f, 1280.f / 720.f, 0.1f, 100.f);
    view().setPosition(glm::vec3(0, 0, 10));
    
    /// render loop ///
    do 
    {
        double totalTime = getTime();
        glm::vec3 quad_scale{ ::cos(totalTime), ::sin(totalTime), 0.0f };
        quad_geo.transform().scale(quad_scale);
        view().updateViewMatrix();
        quad_geo.update(nullptr);
        glm::mat4 mvp
        { 
            view().getProjectionMatrix() * 
            view().getViewMatrix() * 
            quad_geo.transform().matrix() 
        };
        prog.setUniform("mvp", mvp);
        gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        gl_check(glDrawElements(GL_TRIANGLE_STRIP, Quad::verts.size(), GL_UNSIGNED_SHORT, 0));
        context.swapBuffers();
        context.pollEvents();

    } while ( glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
              glfwWindowShouldClose(window) == 0  );
}

void SimpleContextController::cursorpos_callback(double x, double y)
{
    glm::vec2 cpos{ floor(x), floor(y) };
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        ///////  CAMERA ROTATION LEFT BUTTON  ///////  
        glm::vec2 delta { cpos - m_cursorPos };
        
        glm::quat rotX = glm::angleAxis<float>(
            glm::radians(delta.y) * 1.0,
            glm::vec3(1, 0, 0)
            );

        glm::quat rotY = glm::angleAxis<float>(
            glm::radians(delta.x) * 1.0,
            glm::vec3(0, 1, 0)
            );

        view().rotate(rotX * rotY);

    } else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        ///////  OBJECT ROTATION RIGHT BUTTON  ///////  
        glm::vec2 delta{ cpos - m_cursorPos };

        glm::quat rotX = glm::angleAxis<float>(
            glm::radians(delta.y) * 1.0,
            glm::vec3(1, 0, 0)
            );

        glm::quat rotY = glm::angleAxis<float>(
            glm::radians(delta.x) * 1.0,
            glm::vec3(0, 1, 0)
            );

        quad_geo.transform().rotate(rotX * rotY);

    } else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        ///////  OBJECT MOVEMENT MIDDLE BUTTON  ///////  
        glm::vec2 delta{ cpos - m_cursorPos};
        glm::vec3 dt{ -delta.x, delta.y, 0.0f }; 
        dt *= 0.001f;

        quad_geo.transform().translate(dt);
    }

    m_cursorPos = cpos;
}

void SimpleContextController::keyboard_callback(int key, int scancode, int action, int mods)
{
    
}

void SimpleContextController::window_size_callback(int width, int height)
{
    view().setViewport( 0, 0, width, height );
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
