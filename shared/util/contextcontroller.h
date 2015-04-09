#ifndef contextcontroller_h__
#define contextcontroller_h__

#include <log/gl_log.h>
#include <util/context.h>

namespace bd
{
;

class ContextController
{
public:
    ContextController() 
    {
    }

    virtual ~ContextController() 
    { 
    }

    virtual void renderLoop(Context &) = 0;
    virtual void keyboard_callback(int key, int scancode, int action, int mods) = 0;
    virtual void window_size_callback(int width, int height) = 0;
    virtual void cursorpos_callback(double x, double y) = 0;
    virtual void scrollwheel_callback(double xoff, double yoff) = 0;

};

}  // namespace bd 

#endif
