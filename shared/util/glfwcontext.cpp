#include "glfwcontext.h"

#include "log/gl_log.h"

namespace bd {



///////////////////////////////////////////////////////////////////////////////
GlfwContext::GlfwContext(ContextController *concon) 
    : Context(concon)
{
}


///////////////////////////////////////////////////////////////////////////////
GlfwContext::~GlfwContext()
{
    glfwTerminate();
}


///////////////////////////////////////////////////////////////////////////////
void GlfwContext::init(int width, int height)
{

    m_window = nullptr;
    if (!glfwInit()) {
        gl_log_err("could not start GLFW3");
        //return nullptr;
        return;
    }

    glfwSetErrorCallback(GlfwContext::glfw_error_callback);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    // number of samples to use for multi sampling
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, "Minimal", NULL, NULL);
    if (!m_window) {
        gl_log_err("ERROR: could not open window with GLFW3");
        glfwTerminate();
        //return nullptr;
        return;
    }

    glfwSetCursorPosCallback(m_window, glfw_cursorpos_callback);
    glfwSetWindowSizeCallback(m_window, glfw_window_size_callback);
    glfwSetScrollCallback(m_window, glfw_scrollwheel_callback);
    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwMakeContextCurrent(m_window);

    glewExperimental = GL_TRUE;
    GLenum error = glewInit();
    if (error) {
        gl_log_err("could not init glew %s", glewGetErrorString(error));
        //return nullptr;
        return;
    }
    
    glDebugMessageCallback((GLDEBUGPROC)bd::gl_debug_message_callback, NULL);
    glEnable(GL_DEBUG_OUTPUT);

    gl_check(glEnable(GL_DEPTH_TEST));
    gl_check(glDepthFunc(GL_LESS));
    gl_check(glClearColor(0.1f, 0.1f, 0.1f, 0.0f));

    isInit(true);

    //return m_window;
}


///////////////////////////////////////////////////////////////////////////////
void GlfwContext::swapBuffers()
{
    glfwSwapBuffers(m_window);
}


///////////////////////////////////////////////////////////////////////////////
void GlfwContext::pollEvents()
{
    glfwPollEvents();
}


///////////////////////////////////////////////////////////////////////////////
GLFWwindow* GlfwContext::window() const 
{
    return m_window;
}


////////////////////////////////////////////////////////////////////////////////////
// static glfw callbacks
////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
void GlfwContext::glfw_error_callback(int error, const char *description)
{
    gl_log("GLFW ERROR: code %i msg: %s", error, description);
}


///////////////////////////////////////////////////////////////////////////////
void GlfwContext::glfw_cursorpos_callback(GLFWwindow *win, double x, double y)
{
    /*if (m_cursor_pos_cbfunc != nullptr)
    (*m_cursor_pos_cbfunc)(x, y);*/
    concon().cursorpos_callback(x, y);
}


///////////////////////////////////////////////////////////////////////////////
void GlfwContext::glfw_window_size_callback(GLFWwindow *win, int w, int h)
{
    /*if (m_window_size_cbfunc != nullptr)
    (*m_window_size_cbfunc)(w, h);*/
    concon().window_size_callback(w, h);
}


///////////////////////////////////////////////////////////////////////////////
void GlfwContext::glfw_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    /*if (m_keyboard_cbfunc != nullptr)
    (*m_keyboard_cbfunc)(key, scancode, action, mods);*/
    concon().keyboard_callback(key, scancode, action, mods);
}


///////////////////////////////////////////////////////////////////////////////
void GlfwContext::glfw_scrollwheel_callback(GLFWwindow *window, double xoff, double yoff)
{
    /*if (m_scroll_wheel_cbfunc != nullptr)
    (*m_scroll_wheel_cbfunc)(xoff, yoff);*/
    concon().scrollwheel_callback(xoff, yoff);
}

} // namespace bd

