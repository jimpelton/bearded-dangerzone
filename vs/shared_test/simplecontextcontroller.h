
#ifndef simplecontextcontroller_h__
#define simplecontextcontroller_h__

#include <util/contextcontroller.h>
#include <graphics/view.h>

#include <GLFW/glfw3.h>

#include <iostream>



class SimpleContextController : public bd::ContextController
{
public:

    SimpleContextController();

    virtual ~SimpleContextController();


    virtual void renderLoop(bd::Context &) override;

    virtual void cursorpos_callback(double x, double y) override;

    virtual void keyboard_callback(int key, int scancode, int action, int mods) override;

    virtual void window_size_callback(int width, int height) override;

    virtual void scrollwheel_callback(double xoff, double yoff) override;

    virtual void error_callback(int error, const char *description);

    void setWindow(GLFWwindow *);

private:
    bd::View m_view;
    GLFWwindow *m_window;

    glm::vec2 m_cursorPos;

};

#endif
