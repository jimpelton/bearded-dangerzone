//
// Created by jim on 10/22/15.
//

#ifndef blockrenderer_h__
#define blockrenderer_h__

#include "io/blockcollection.h"
#include "sliceset.h"
#include "classificationtype.h"
#include "messages/recipient.h"
#include "blockrenderer.h"
#include "nvtools.h"

#include <bd/graphics/renderer.h>
#include <bd/graphics/shader.h>
#include <bd/graphics/texture.h>
#include <bd/volume/volume.h>
#include <bd/graphics/vertexarrayobject.h>

#include <memory>

namespace subvol
{
namespace renderer
{

class SlicingBlockRenderer
    : public BlockRenderer, public Recipient
{

public:
  SlicingBlockRenderer();


  SlicingBlockRenderer(glm::vec3 smod,
      std::shared_ptr<subvol::BlockCollection> blockCollection,
      bd::Volume const &v);


  virtual ~SlicingBlockRenderer();


  bool
  initialize() override;


  void
  draw() override;


public:

  /// \brief Set the transfer function texture.
  void
  setColorMapTexture(bd::Texture const &tfunc) override;


  void
  setColorMapScaleValue(float val) override;


  float
  getColorMapScaleValue() const override;


  void
  setShaderNShiney(float n) override;


  void
  setShaderLightPos(glm::vec3 const &L) override;


//  glm::vec3 const &
//  getShaderLightPos() const;


  void
  setShaderMaterial(glm::vec3 const &M) override;


//  glm::vec3 const &
//  getShaderMaterial() const;


  void
  setShouldUseLighting(bool b) override;


  bool
  getShouldUseLighting() const override;


  void
  setDrawNonEmptyBoundingBoxes(bool b) override;


  void
  setDrawNonEmptyBlocks(bool b);


  /// \brief Draw wireframe bounding boxes around the blocks.
  /// \param[in] vp View projection matrix.
  void
  drawNonEmptyBoundingBoxes();


private:

  void
  initVao(float smod_x, float smod_y, float smod_z, bd::Volume const &v);


  void
  initShaders();


  /// \brief Disable GL_DEPTH_TEST and draw transparent slices
  void
  drawSlices(int baseVertex, int elementOffset, unsigned int numSlices) const;


  /// \brief Draw the coordinate axis.
  void
  drawAxis() const;


  /// \brief Loop through the blocks and draw each one
  void
  drawNonEmptyBlocks_Forward();


  /// \brief Compute the base vertex offset for the slices vertex buffer based
  ///        off the largest component of \c viewdir.
  std::pair<int, int>
  computeBaseVertexFromViewDir(glm::vec3 const &viewdir);


  /// \brief Sort visible blocks by distance from the camera (for painter's algorithm)
  /// Distance is from camera to center of each block. Order is decending.
  void
  sortBlocks();


  float m_tfuncScaleValue;
  /// True to draw bounding boxes.
  bool m_drawNonEmptyBoundingBoxes;
  bool m_drawNonEmptySlices;
  /// Show bounding boxes if rov is changing.
  bool m_rangeChanging;
  /// True to use Phong lighting shader.
  bool m_shouldUseLighting;

  /// Number of slices per block
  glm::u64vec3 m_numSlicesPerBlock;
  glm::vec3 m_smod;
  /// Transfer function texture
  bd::Texture const *m_colorMapTexture;
  bd::Volume const &m_volume;

  SliceSet m_selectedSliceSet;

  unsigned int m_sampler_state;
  /// Current shader being used (lighting, flat, wire, etc).
  bd::ShaderProgram *m_currentShader;
  std::unique_ptr<bd::ShaderProgram> m_volumeShader;
  std::unique_ptr<bd::ShaderProgram> m_volumeShaderLighting;
  std::unique_ptr<bd::ShaderProgram> m_wireframeShader;
  std::unique_ptr<bd::VertexArrayObject> m_quadsVao;    ///< Quad geometry verts
  std::unique_ptr<bd::VertexArrayObject> m_boxesVao;    ///< bounding box wireframe verts
  std::unique_ptr<bd::VertexArrayObject> m_axisVao;
  std::shared_ptr<BlockCollection> m_collection;

  std::vector<bd::Block *> *m_nonEmptyBlocks;  ///< Non-empty blocks to draw.
  std::vector<bd::Block *> *m_blocks;       ///< All the blocks!

public:

  void
  handle_ROVChangingMessage(ROVChangingMessage &) override;


};

} // namespace renderer
} // namepsace subvol

#endif // blockrenderer_h__

