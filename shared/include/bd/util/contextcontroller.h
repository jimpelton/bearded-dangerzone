#ifndef contextcontroller_h__
#define contextcontroller_h__

#include <bd/util/context.h>
#include <bd/graphics/view.h>

namespace bd
{
;

class ContextController
{
public:
    //ContextController() 
    //{
    //}

    //virtual ~ContextController() 
    //{ 
    //}

    virtual void renderLoop(Context &) = 0;
    virtual void keyboard_callback(int key, int scancode, int action, int mods) = 0;
    virtual void window_size_callback(int width, int height) = 0;
    virtual void cursorpos_callback(double x, double y) = 0;
    virtual void scrollwheel_callback(double xoff, double yoff) = 0;

    View& view() { return m_view; }

private:
    View m_view;
    

};

}  // namespace bd 

#endif
