#include "glfwcontext.h"

#include "log/gl_log.h"

namespace bd {

ContextController *GlfwContext::m_concon = nullptr;

////////////////////////////////////////////////////////////////////////////////////
// static glfw callbacks
////////////////////////////////////////////////////////////////////////////////////

void GlfwContext::glfw_error_callback(int error, const char *description)
{
    gl_log("GLFW ERROR: code %i msg: %s", error, description);
}

void GlfwContext::glfw_cursorpos_callback(GLFWwindow *win, double x, double y)
{
    /*if (m_cursor_pos_cbfunc != nullptr)
    (*m_cursor_pos_cbfunc)(x, y);*/
    m_concon->cursorpos_callback(x, y);
}

void GlfwContext::glfw_window_size_callback(GLFWwindow *win, int w, int h)
{
    /*if (m_window_size_cbfunc != nullptr)
    (*m_window_size_cbfunc)(w, h);*/
    m_concon->window_size_callback(w, h);
}

void GlfwContext::glfw_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    /*if (m_keyboard_cbfunc != nullptr)
    (*m_keyboard_cbfunc)(key, scancode, action, mods);*/
    m_concon->keyboard_callback(key, scancode, action, mods);
}

void GlfwContext::glfw_scrollwheel_callback(GLFWwindow *window, double xoff, double yoff)
{
    /*if (m_scroll_wheel_cbfunc != nullptr)
    (*m_scroll_wheel_cbfunc)(xoff, yoff);*/
    m_concon->scrollwheel_callback(xoff, yoff);
}


GlfwContext::GlfwContext()
{
}

GlfwContext::~GlfwContext()
{
    glfwTerminate();
}

GLFWwindow* GlfwContext::init(ContextController *con, int width, int height)
{
    m_concon = con;

    m_window = nullptr;
    if (!glfwInit()) {
        gl_log_err("could not start GLFW3");
        return nullptr;
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
        return nullptr;
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
        return nullptr;
    }

    glDebugMessageCallback((GLDEBUGPROC)bd::gl_debug_message_callback, NULL);
    glEnable(GL_DEBUG_OUTPUT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

    return m_window;
}

GLFWwindow* GlfwContext::window() const 
{
    return m_window;
}

void GlfwContext::swapBuffers() 
{
    glfwSwapBuffers(m_window);
}

void GlfwContext::pollEvents() 
{
    glfwPollEvents();
}
} // namespace bd

