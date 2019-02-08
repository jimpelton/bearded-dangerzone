//
// Created by jim on 10/22/15.
//

#ifndef blockrenderer_h__
#define blockrenderer_h__

#include "blockcollection.h"
#include "sliceset.h"
#include "classificationtype.h"
#include "messages/recipient.h"

#include "nvtools.h"

#include <bd/graphics/renderer.h>
#include <bd/graphics/shader.h>
#include <bd/graphics/texture.h>
#include <bd/graphics/vertexarrayobject.h>

#include <memory>

namespace subvol
{

class BlockRenderer
    : public bd::Renderer, public Recipient
{

public:
  BlockRenderer();


  BlockRenderer(glm::u64vec3 numSlices,
                std::shared_ptr<bd::ShaderProgram> volumeShader,
                std::shared_ptr<bd::ShaderProgram> volumeShaderLighting,
                std::shared_ptr<bd::ShaderProgram> wireframeShader,
                std::shared_ptr<subvol::BlockCollection> blockCollection,
                std::shared_ptr<bd::VertexArrayObject> blocksVAO,
                std::shared_ptr<bd::VertexArrayObject> bboxVAO,
                std::shared_ptr<bd::VertexArrayObject> axisVao);


  virtual ~BlockRenderer();


private:
  bool
  init();


public:

  /// \brief Set the transfer function texture.
  void
  setColorMapTexture(bd::Texture const &tfunc);


  void
  setColorMapScaleValue(float val);


  float
  getColorMapScaleValue() const;


  void
  setShaderNShiney(float n);


  void
  setShaderLightPos(glm::vec3 const &L);


//  glm::vec3 const &
//  getShaderLightPos() const;


  void
  setShaderMaterial(glm::vec3 const &M);


//  glm::vec3 const &
//  getShaderMaterial() const;


  void
  setBackgroundColor(glm::vec3 const &c);


  glm::vec3 const &
  getBackgroundColor() const;


  void
  setShouldUseLighting(bool b);


  bool
  getShouldUseLighting() const;


  void
  setDrawNonEmptyBoundingBoxes(bool b);


  void
  setDrawNonEmptySlices(bool b);


  /// \brief Draw each non-empty block.
  void
  draw();


  /// \brief Draw wireframe bounding boxes around the blocks.
  /// \param[in] vp View projection matrix.
  void
  drawNonEmptyBoundingBoxes();


private:

  /// \brief Disable GL_DEPTH_TEST and draw transparent slices
  void
  drawSlices(int baseVertex, int elementOffset, int numSlices) const;


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


  /// Number of slices per block
  glm::u64vec3 m_numSlicesPerBlock;
  /// Transfer function scaling value
  float m_tfuncScaleValue;
  /// True to draw bounding boxes.
  bool m_drawNonEmptyBoundingBoxes;
  bool m_drawNonEmptySlices;
  /// Show bounding boxes if rov is changing.
  bool m_rangeChanging;
  /// True to use Phong lighting shader.
  bool m_shouldUseLighting;
  /// Current background color.
  glm::vec3 m_backgroundColor;
  /// Transfer function texture
  bd::Texture const *m_colorMapTexture;
  /// Current shader being used (lighting, flat, wire, etc).
  bd::ShaderProgram *m_currentShader;

  SliceSet m_selectedSliceSet;
  unsigned int m_sampler_state;
  std::shared_ptr<bd::ShaderProgram> m_volumeShader;
  std::shared_ptr<bd::ShaderProgram> m_volumeShaderLighting;
  std::shared_ptr<bd::ShaderProgram> m_wireframeShader;
  std::shared_ptr<bd::VertexArrayObject> m_quadsVao;    ///< Quad geometry verts
  std::shared_ptr<bd::VertexArrayObject> m_boxesVao;    ///< bounding box wireframe verts
  std::shared_ptr<bd::VertexArrayObject> m_axisVao;
  std::shared_ptr<BlockCollection> m_collection;

  std::vector<bd::Block *> *m_nonEmptyBlocks;  ///< Non-empty blocks to draw.
  std::vector<bd::Block *> *m_blocks;       ///< All the blocks!

public:

  void
  handle_ROVChangingMessage(ROVChangingMessage &) override;


};

} // namepsace subvol

#endif // blockrenderer_h__

