#ifndef contextcontroller_h__
#define contextcontroller_h__

#include <bd/util/context.h>
#include <bd/scene/view.h>

namespace bd {

//////////////////////////////////////////////////////////////////////////
/// \brief Contains logic for rendering scene and callbacks for input
/// devices.
///
/// The  context will call initialize() just before the renderLoop()
/// method is called.
//////////////////////////////////////////////////////////////////////////
class RenderLoop
{
public:
    RenderLoop() { }
    virtual ~RenderLoop() { }

    virtual void initialize(Context &) = 0;
    virtual void renderLoop() = 0;

    virtual void keyboard_callback(int key, int scancode, int action, int mods) = 0;
    virtual void window_size_callback(int width, int height) = 0;
    virtual void cursorpos_callback(double x, double y) = 0;
    virtual void scrollwheel_callback(double xoff, double yoff) = 0;

protected:
    View& view() { return m_view; }

private:
    View m_view; ///< Contains proj and view mats.

};

}  // namespace bd 

#endif
