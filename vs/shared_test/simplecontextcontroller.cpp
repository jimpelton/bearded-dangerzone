
#include "simplecontextcontroller.h"

#include <log/gl_log.h>
#include <util/shader.h>
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
//"in vec3 col;"
"uniform mat4 mvp;"
//"out vec3 color;"
"void main() { "
"    gl_Position = mvp * vec4(vert, 1.0f);"
//"    color = col;"
"}";

const std::string g_fragStr =
"#version 400\n"
//"in vec3 color;"
"out vec4 out_col;"
"void main() {"
//"    out_col = vec4(color, 1.0f);"
"    out_col = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
"}";

SimpleContextController::SimpleContextController() 
    : ContextController()
    , context{ nullptr }
{
    bd::gl_log_restart();
    bd::gl_debug_log_restart();
}

SimpleContextController::~SimpleContextController() {
    delete context;
}

bool SimpleContextController::init(int scr_w, int scr_h) {
    context = new bd::GlfwContext(this);
    return  context->init(scr_w, scr_h) != nullptr;
}

void SimpleContextController::renderLoop()
{
    GLFWwindow *window = context->window();
    
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

    quad.bind();
    prog.bind();

    glm::mat4 mvp = glm::perspective(50.0f, 1280.f / 720.f, 0.1f, 100.0f) *
        glm::lookAt(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    prog.setUniform("mvp", mvp);

    do 
    {
        gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


        gl_check(glDrawElements(GL_TRIANGLES, Quad::verts.size(), GL_UNSIGNED_SHORT, 0));

        context->swapBuffers();
        context->pollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);
    
    
}

void SimpleContextController::cursorpos_callback(double x, double y)
{
    
}

void SimpleContextController::keyboard_callback(int key, int scancode, int action, int mods)
{
    
}

void SimpleContextController::window_size_callback(int width, int height)
{
    
}

void SimpleContextController::scrollwheel_callback(double xoff, double yoff)
{
    
}

void SimpleContextController::error_callback(int error, const char* description) 
{
    std::cerr << "Error: [" << error << "] Desc: " << description << std::endl;
}

