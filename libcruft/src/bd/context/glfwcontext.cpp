#include <GL/glew.h>

#include <bd/log/logger.h>
#include <bd/log/gl_log.h>
#include <bd/context/glfwcontext.h>

//#include <stdio.h>


namespace bd
{
///////////////////////////////////////////////////////////////////////////////
GlfwContext::GlfwContext(RenderLoop* concon)
  : Context(concon)
{
}


///////////////////////////////////////////////////////////////////////////////
GlfwContext::~GlfwContext()
{
  glfwTerminate();
}


///////////////////////////////////////////////////////////////////////////////
bool
GlfwContext::init(int width, int height)
{
  m_window = nullptr;
  if (!glfwInit()) {
    Err() << "could not start GLFW3";
    //return nullptr;
    return false;
  }

  glfwSetErrorCallback(GlfwContext::glfw_error_callback);

  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  // number of samples to use for multi sampling
  //    glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_window = glfwCreateWindow(width, height, "Minimal", NULL, NULL);
  if (!m_window) {
    Err() << "ERROR: could not open window with GLFW3";
    glfwTerminate();
    return false;
  }

  glfwSetCursorPosCallback(m_window, glfw_cursorpos_callback);
  glfwSetWindowSizeCallback(m_window, glfw_window_size_callback);
  glfwSetScrollCallback(m_window, glfw_scrollwheel_callback);
  glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwMakeContextCurrent(m_window);

  glewExperimental = GL_TRUE;
  GLenum error = glewInit();
  if (error) {
    Err() << "could not init glew " << glewGetErrorString(error);
    return false;
  }

  subscribe_debug_callbacks();

  Info() << "GLFWContext initialized...";

  return true;
}


///////////////////////////////////////////////////////////////////////////////
void
GlfwContext::swapBuffers()
{
  glfwSwapBuffers(m_window);
}


///////////////////////////////////////////////////////////////////////////////
void
GlfwContext::pollEvents()
{
  glfwPollEvents();
}

bool
GlfwContext::windowShouldClose() const
{
  return glfwWindowShouldClose(m_window) == GL_TRUE;
}


///////////////////////////////////////////////////////////////////////////////
GLFWwindow*
GlfwContext::window() const
{
  return m_window;
}


////////////////////////////////////////////////////////////////////////////////////
// static glfw callbacks
////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
void
GlfwContext::glfw_error_callback(int error, const char* description)
{
  Err() << "GLFW ERROR: code " << error << " msg: " << description;
}


///////////////////////////////////////////////////////////////////////////////
void
GlfwContext::glfw_cursorpos_callback(GLFWwindow* win, double x, double y)
{
  renderLoop().cursorpos_callback(x, y);
}


///////////////////////////////////////////////////////////////////////////////
void
GlfwContext::glfw_window_size_callback(GLFWwindow* win, int w, int h)
{
  renderLoop().window_size_callback(w, h);
}


///////////////////////////////////////////////////////////////////////////////
void GlfwContext::glfw_keyboard_callback(GLFWwindow* window, int key, int scancode,
                                         int action, int mods)
{
  renderLoop().keyboard_callback(key, scancode, action, mods);
}


///////////////////////////////////////////////////////////////////////////////
void GlfwContext::glfw_scrollwheel_callback(GLFWwindow* window, double xoff,
                                            double yoff)
{
  renderLoop().scrollwheel_callback(xoff, yoff);
}
} // namespace bd


