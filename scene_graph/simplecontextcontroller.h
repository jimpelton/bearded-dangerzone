
#ifndef simplerenderloop_h__
#define simplerenderloop_h__

#include <bd/util/contextcontroller.h>
#include <bd/scene/view.h>
#include <bd/scene/transformable.h>

#include <iostream>



class SimpleContextController : public bd::RenderLoop
{
public:

    SimpleContextController();

    virtual ~SimpleContextController();


    virtual void renderLoop() override;

    virtual void cursorpos_callback(double x, double y) override;

    virtual void keyboard_callback(int key, int scancode, int action, int mods) override;

    virtual void window_size_callback(int width, int height) override;

    virtual void scrollwheel_callback(double xoff, double yoff) override;

    virtual void error_callback(int error, const char *description);

    virtual void initialize(bd::Context &context) override;

    void setRoot(bd::Transformable *);

private:
    double getTime();

    bd::Transformable *m_root;
};

#endif
