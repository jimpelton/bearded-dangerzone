//
// Created by Jim Pelton on 8/1/16.
//

#ifndef SUBVOL_RENDERHELP_H
#define SUBVOL_RENDERHELP_H


#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <memory>

namespace subvol
{

class Renderer;
class Camera;
class Controls;
class BlockRenderer;

glm::vec3 const g_backgroundColors[2]{
  { 0.15, 0.15, 0.15 },
  { 1.0,  1.0,  1.0 }
};

namespace renderhelp
{

GLFWwindow *
initGLContext(int screenWidth, int screenHeight);

void
setInitialGLState();

void
initializeControls(GLFWwindow *, std::shared_ptr<BlockRenderer>);

void
loop(GLFWwindow *, BlockRenderer *);
} // namespace renderhelp
} // namespace subvol


#endif //SUBVOL_GLRENDERHELP_H
