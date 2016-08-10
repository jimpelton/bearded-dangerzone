//
// Created by Jim Pelton on 8/1/16.
//

#include "renderhelp.h"
#include "renderer.h"

#include <GL/glew.h>

#include <bd/log/logger.h>
#include <bd/log/gl_log.h>

//#include <glm/glm.hpp>
//#include <glm/matrix.hpp>

namespace subvol
{


void setInitialGLState()
{
  bd::Info() << "Initializing gl state.";
  gl_check(glClearColor(0.15f, 0.15f, 0.15f, 0.0f));

//  gl_check(glEnable(GL_CULL_FACE));
//  gl_check(glCullFace(GL_FRONT));
//  gl_check(glDisable(GL_CULL_FACE));

  gl_check(glEnable(GL_DEPTH_TEST));
  gl_check(glDepthFunc(GL_LESS));

  gl_check(glEnable(GL_BLEND));
  gl_check(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  gl_check(glEnable(GL_PRIMITIVE_RESTART));
  gl_check(glPrimitiveRestartIndex(0xFFFF));

}


//void setDefaultView(glm::vec3 const &eye)
//{
//  camera->setEye(eye);
//  camera->setLookAt({ 0, 0, 0 });
//  camera->setUp({ 0, 1, 0 });
//  renderer->setViewMatrix(camera->createViewMatrix());
//}

} // namespace subvol
