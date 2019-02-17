//
// Created by jim on 2/16/19.
//

#ifndef SUBVOL_BLOCKRAYCASTER_H
#define SUBVOL_BLOCKRAYCASTER_H

#include <bd/graphics/renderer.h>
#include <bd/graphics/shader.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/geo/mesh.h>

#include <memory>

namespace subvol
{
namespace render
{

class BlockingRaycaster : public bd::Renderer
{
public:

  BlockingRaycaster();


  virtual ~BlockingRaycaster() noexcept override;

  void
  initShaders();

  void
  setUniforms();

  void
  draw();

private:
  std::unique_ptr<bd::ShaderProgram> m_alphaBlending;
  bd::Mesh m_cube;

};

}
}; // namespace subvol -- renderer

#endif //SUBVOL_BLOCKRAYCASTER_H
