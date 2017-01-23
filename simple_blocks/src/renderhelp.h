//
// Created by Jim Pelton on 8/1/16.
//

#ifndef SUBVOL_RENDERHELP_H
#define SUBVOL_RENDERHELP_H


#include <GLFW/glfw3.h>

#include "blockcollection.h"
#include "cmdline.h"

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


bool
initializeBlockCollection(BlockCollection **bc,
  bd::IndexFile const *indexFile, subvol::CommandLineOptions const &clo);


void
setInitialGLState();

void
initializeControls(GLFWwindow *, std::shared_ptr<BlockRenderer>);

//TODO: move init functions from main to right here!

void
loop(GLFWwindow *, BlockRenderer *);

/// \brief Get the total and avail memory. If no pointer to avail is provided
/// then just total is returned.
void
queryGPUMemory(int *total, int *avail = nullptr);

//void
//setCameraPosPreset(unsigned int cameraPos);

} // namespace renderhelp
} // namespace subvol


#endif //SUBVOL_GLRENDERHELP_H
