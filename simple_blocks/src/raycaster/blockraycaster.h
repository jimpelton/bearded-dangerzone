//
// Created by jim on 2/16/19.
//

#ifndef SUBVOL_BLOCKRAYCASTER_H
#define SUBVOL_BLOCKRAYCASTER_H

#include "io/blockcollection.h"

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

  ///////////////////////////////////////////////////////////////////////////////
  BlockingRaycaster(std::unique_ptr<subvol::BlockCollection> bc);


  ///////////////////////////////////////////////////////////////////////////////
  virtual
  ~BlockingRaycaster() noexcept override;


  virtual bool
  initialize() override;

  ///////////////////////////////////////////////////////////////////////////////
  virtual void
  draw() override;


  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Draw all the non-empty bounding boxes.
  void
  drawNonEmptyBoundingBoxes();


  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Draw the axis if enabled.
  void
  drawAxis();


  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Draw all the non-empty blocks
  void
  drawNonEmptyBlocks();


  ///////////////////////////////////////////////////////////////////////////////
  void
  setUniforms();


private:
  ///////////////////////////////////////////////////////////////////////////////
  void
  sortBlocks();

  ///////////////////////////////////////////////////////////////////////////////
  void
  initShaders();

private:
  std::unique_ptr<bd::ShaderProgram> m_alphaBlending;
  std::unique_ptr<subvol::BlockCollection> m_blockCollection;
  bd::Mesh m_cube;
  unsigned int m_volumeSampler;
};

}
}; // namespace subvol -- renderer

#endif //SUBVOL_BLOCKRAYCASTER_H
