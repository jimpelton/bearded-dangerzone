
#ifndef glfwcontext_h__
#define glfwcontext_h__

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace bearded { namespace dangerzone { namespace util { 
; // <-- stop VS from indenting

typedef void(*CursorPosCallback)(double x, double y);
typedef void(*WindowSizeCallback)(int x, int y);
typedef void(*ScrollWheelCallback)(double xoff, double yoff);
typedef void(*KeyboardCallback)(int key, int scancode, int action, int mods);

class GlfwContext {
    static void glfw_error_callback(int error, const char* description);

    static void glfw_cursorpos_callback(GLFWwindow *win, double x, double y);

    static void glfw_window_size_callback(GLFWwindow *win, int w, int h);

    static void glfw_keyboard_callback(GLFWwindow *window, int key, int scancode,
        int action, int mods);

    static void glfw_scrollwheel_callback(GLFWwindow *window, double xoff, double yoff);

public:
    GlfwContext();
    ~GlfwContext();


public:   
    void setCursorPosCallback(CursorPosCallback cbfunc) { m_cursor_pos_cbfunc = cbfunc; }
    void setWindowSizeCallback(WindowSizeCallback cbfunc) { m_window_size_cbfunc = cbfunc; }
    void setScrollWheelCallback(ScrollWheelCallback cbfunc) { m_scroll_wheel_cbfunc = cbfunc; }
    void setKeyboardCallback(KeyboardCallback cbfunc) { m_keyboard_cbfunc = cbfunc; }
    
    GLFWwindow* init(int w, int h);
  
    GLFWwindow *window() const { return m_window; }

private:
    GLFWwindow *m_window;

    static CursorPosCallback m_cursor_pos_cbfunc;
    static WindowSizeCallback m_window_size_cbfunc;
    static ScrollWheelCallback m_scroll_wheel_cbfunc;
    static KeyboardCallback m_keyboard_cbfunc;
};


}}} // namespace


#endif // glfwcontext_h__