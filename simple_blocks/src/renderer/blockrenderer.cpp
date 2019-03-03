//
// Created by jim on 2/24/19.
//


#include "blockrenderer.h"
#include <bd/log/gl_log.h>
#include <GL/glew.h>

namespace subvol {

void
BlockRenderer::setBackgroundColor(glm::vec4 const &c)
{
  _backgroundColor = c;
  gl_check(glClearColor(c.r, c.g, c.b, c.a));
};


}
