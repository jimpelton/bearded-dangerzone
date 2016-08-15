//
// Created by jim on 8/12/16.
//

#ifndef SUBVOL_CONTROLS_H
#define SUBVOL_CONTROLS_H

#include "blockrenderer.h"


#include <string>
#include <vector>
#include <memory>


namespace subvol
{

struct Cursor
{
  glm::vec2 pos{ 0, 0 };
  float mouseSpeed{ 1.0f };
};

class Controls
{
public:
  Controls();
  ~Controls();


  void cursorpos_callback(double x, double y);

  void keyboard_callback(int key, int scancode, int action, int mods);

  void window_size_callback(int width, int height);

  void scrollwheel_callback(double xoff, double yoff);

  Cursor & getCursor();

private:
  Cursor m_cursor;
  std::vector<std::string> m_tfuncNames;
  std::shared_ptr<BlockRenderer> m_renderer;
};

} // namespace subvol


#endif //SUBVOL_CONTROLS_H
