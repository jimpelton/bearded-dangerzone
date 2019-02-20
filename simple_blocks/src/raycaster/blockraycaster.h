//
// Created by jim on 2/16/19.
//

#ifndef SUBVOL_BLOCKRAYCASTER_H
#define SUBVOL_BLOCKRAYCASTER_H

#include "io/blockcollection.h"
#include "blockrenderer.h"
#include <bd/graphics/renderer.h>
#include <bd/graphics/shader.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/geo/mesh.h>
#include <bd/volume/volume.h>

#include <memory>

namespace subvol
{
namespace render
{

class BlockingRaycaster : public subvol::BlockRenderer
{
public:

  ///////////////////////////////////////////////////////////////////////////////
  BlockingRaycaster(std::shared_ptr<subvol::BlockCollection> bc, bd::Volume const &v);


  ///////////////////////////////////////////////////////////////////////////////
  ~BlockingRaycaster() noexcept override;


  bool
  initialize() override;

  ///////////////////////////////////////////////////////////////////////////////
  void
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
  std::shared_ptr<bd::ShaderProgram> m_wireframeShader;
  std::shared_ptr<subvol::BlockCollection> m_blockCollection;

  bd::Mesh m_cube;
  bd::Volume m_volume;
  unsigned int m_volumeSampler;
};

}
}; // namespace subvol -- renderer

#endif //SUBVOL_BLOCKRAYCASTER_H
