#ifndef glfwcontext_h__
#define glfwcontext_h__


#include <bd/util/context.h>
#include <bd/util/contextcontroller.h>

#include <GLFW/glfw3.h>

namespace bd {



//typedef void (*CursorPosCallback)(double x, double y);
//typedef void (*WindowSizeCallback)(int x, int y);
//typedef void (*ScrollWheelCallback)(double xoff, double yoff);
//typedef void (*KeyboardCallback)(int key, int scancode, int action, int mods);

class GlfwContext : public Context {
public:

    explicit GlfwContext(ContextController *);
    ~GlfwContext();
    
    bool init(int scr_w, int scr_h) override;

    void swapBuffers() override;

    void pollEvents() override;

    GLFWwindow* window() const;

private:


///////////////////////////////////////////////////////////////////////////////
// GLFW static callbacks
///////////////////////////////////////////////////////////////////////////////
    static void glfw_error_callback(int error, const char *description);

    static void glfw_cursorpos_callback(GLFWwindow *win, double x, double y);

    static void glfw_window_size_callback(GLFWwindow *win, int w, int h);

    static void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode,
                                       int action, int mods);

    static void glfw_scrollwheel_callback(GLFWwindow *window, double xoff, double yoff);

    ///////////////////////////////////////////////////////////////////////////////
    // Data members
    ///////////////////////////////////////////////////////////////////////////////
    GLFWwindow *m_window;
    
};
} // namespace bd

#endif // glfwcontext_h__

