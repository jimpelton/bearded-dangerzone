//
// Created by jim on 2/16/19.
//

#include "blockraycaster.h"

#include <bd/graphics/shader.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/geo/mesh.h>
#include <memory>

namespace subvol
{
namespace render
{

BlockingRaycaster::BlockingRaycaster()
  : m_cube {
    {
       -1.0f, -1.0f, 1.0f,
       1.0f, -1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       -1.0f, 1.0f, 1.0f,
       -1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, 1.0f, -1.0f,
       -1.0f, 1.0f, -1.0f,
    }, 3,
    {
       // front
       0, 1, 2,
       0, 2, 3,
       // right
       1, 5, 6,
       1, 6, 2,
       // back
       5, 4, 7,
       5, 7, 6,
       // left
       4, 0, 3,
       4, 3, 7,
       // top
       2, 6, 7,
       2, 7, 3,
       // bottom
       4, 5, 1,
       4, 1, 0,
    } }
{
  initShaders();
}


BlockingRaycaster::~BlockingRaycaster() noexcept
{

}

void
BlockingRaycaster::initShaders()
{
  m_alphaBlending = std::unique_ptr<bd::ShaderProgram>();
  auto pid = m_alphaBlending->linkProgram("shaders/alpha_blending.vert", "shaders/alpha_blending.frag");
  if (pid == 0) {
    throw std::runtime_error("could not initialize shaders");
  }
}

void
BlockingRaycaster::setUniforms()
{

}


void
BlockingRaycaster::draw()
{

}



}} // namespace subvol namespace renderer

