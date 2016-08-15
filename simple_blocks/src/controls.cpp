//
// Created by jim on 8/12/16.
//

#include "controls.h"

namespace subvol
{
Controls::Controls()
  : m_cursor{ }
  , m_tfuncNames{ }
  , m_renderer{ nullptr }
{

}

Controls::~Controls() {}

void
Controls::cursorpos_callback(double x, double y)
{

}

void
Controls::keyboard_callback(int key, int scancode, int action, int mods)
{

}

void
Controls::window_size_callback(int width, int height)
{

}

void
Controls::scrollwheel_callback(double xoff, double yoff)
{

}

Cursor &
Controls::getCursor()
{
  return m_cursor;
}


} // namespace subvol
