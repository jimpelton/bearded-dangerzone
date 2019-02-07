#ifndef renderloop_h__
#define renderloop_h__

#include <bd/context/context.h>
#include <bd/graphics/view.h>

namespace bd
{
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
  RenderLoop()
  {
  }

  virtual
  ~RenderLoop()
  {
  }

  virtual void initialize(Context&) = 0;

  virtual void render() = 0;

  virtual void
  keyboard_callback(int key, int scancode, int action, int mods)
  {
  }

  virtual void
  window_size_callback(int width, int height)
  {
  }

  virtual void
  cursorpos_callback(double x, double y)
  {
  }

  virtual void
  scrollwheel_callback(double xoff, double yoff)
  {
  }

protected:
  View&
  view()
  {
    return m_view;
  }

private:
  View m_view; ///< Contains proj and view mats.
};
} // namespace bd 

#endif // !renderloop_h__


