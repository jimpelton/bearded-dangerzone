#ifndef glfwcontext_h__
#define glfwcontext_h__

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace bd {

class ContextController
{
public:
    ContextController() { }
    virtual ~ContextController() { }

    virtual void keyboard_callback(int key, int scancode, int action, int mods) = 0;
    virtual void window_size_callback(int width, int height) = 0;
    virtual void cursorpos_callback(double x, double y) = 0;
    virtual void scrollwheel_callback(double xoff, double yoff) = 0;
};

typedef void (*CursorPosCallback)(double x, double y);
typedef void (*WindowSizeCallback)(int x, int y);
typedef void (*ScrollWheelCallback)(double xoff, double yoff);
typedef void (*KeyboardCallback)(int key, int scancode, int action, int mods);

class GlfwContext {
private:
    static ContextController *m_concon;
    
    static void glfw_error_callback(int error, const char *description);

    static void glfw_cursorpos_callback(GLFWwindow *win, double x, double y);

    static void glfw_window_size_callback(GLFWwindow *win, int w, int h);

    static void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode,
                                       int action, int mods);

    static void glfw_scrollwheel_callback(GLFWwindow *window, double xoff, double yoff);

public:
    explicit GlfwContext();
    ~GlfwContext();
    
    GLFWwindow* init(ContextController *, int scr_w, int scr_h);

public:
    GLFWwindow* window() const;

    void swapBuffers();

    void pollEvents();

private:
    GLFWwindow *m_window;
    
};
} // namespace bd

#endif // glfwcontext_h__

