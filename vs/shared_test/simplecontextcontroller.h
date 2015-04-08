
#ifndef simplecontextcontroller_h__
#define simplecontextcontroller_h__

#include <util/contextcontroller.h>
#include <util/glfwcontext.h>
#include <iostream>



class SimpleContextController : public bd::ContextController
{
public:

    SimpleContextController();

    virtual ~SimpleContextController();

    bool init(int scr_w, int scr_h) override;

    void renderLoop() override;

    virtual void cursorpos_callback(double x, double y) override;

    virtual void keyboard_callback(int key, int scancode, int action, int mods) override;

    virtual void window_size_callback(int width, int height) override;

    virtual void scrollwheel_callback(double xoff, double yoff) override;

    virtual void error_callback(int error, const char *description);


private:
    bd::GlfwContext *context;
    



};

#endif
