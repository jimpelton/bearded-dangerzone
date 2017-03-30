//
// Created by jim on 10/22/15.
//

#ifndef blockrenderer_h__
#define blockrenderer_h__

#include "blockcollection.h"
#include "renderer.h"
#include "sliceset.h"
#include "classificationtype.h"


#include <bd/graphics/shader.h>
#include <bd/graphics/texture.h>
#include <bd/graphics/vertexarrayobject.h>

#include <glm/fwd.hpp>

#include <memory>

namespace subvol
{

class BlockRenderer
    : public subvol::Renderer
{

public:
  BlockRenderer();


  BlockRenderer(int numSlices,
                std::shared_ptr<bd::ShaderProgram> volumeShader,
                std::shared_ptr<bd::ShaderProgram> volumeShaderLighting,
                std::shared_ptr<bd::ShaderProgram> wireframeShader,
                subvol::BlockCollection *blockCollection,
                std::shared_ptr<bd::VertexArrayObject> blocksVAO,
                std::shared_ptr<bd::VertexArrayObject> bboxVAO,
                std::shared_ptr<bd::VertexArrayObject> axisVao);


  virtual ~BlockRenderer();


private:
  bool
  init();


public:
  ClassificationType
  getClassificationType();

  void
  setClassificationType(ClassificationType type);

  /// \brief Set the transfer function texture.
  void
  setColorMapTexture(bd::Texture const &tfunc);


  void
  setColorMapScaleValue(float val);


  float
  getColorMapScaleValue();


  void
  setShaderNShiney(float n);


  void
  setShaderLightPos(glm::vec3 const &L);


  void
  setShaderMaterial(glm::vec3 const &M);


  void
  setBackgroundColor(glm::vec3 const &c);


  void
  setShouldUseLighting(bool b);


  bool
  getShouldUseLighting() const;


  void
  setDrawNonEmptyBoundingBoxes(bool b);


  void
  setROVChanging(bool b);


  //void
  //setIsRotating(bool b);


  void
  setDrawNonEmptySlices(bool b);


  void
  setROVRange(double min, double max);


  double
  getROVMin();


  double
  getROVMax();


  unsigned long long int
  getNumBlocks() const;


  unsigned long long int
  getNumBlocksShown() const;


  /// \brief Draw each non-empty block.
  void
  draw();


  /// \brief Draw wireframe bounding boxes around the blocks.
  /// \param[in] vp View projection matrix.
  void
  drawNonEmptyBoundingBoxes();


  /// \brief Hint that the block caches should be updated.
  void
  updateCache();


private:

  /// \brief Disable GL_DEPTH_TEST and draw transparent slices
  void
  drawSlices(int baseVertex);


  /// \brief Draw the coordinate axis.
  void
  drawAxis();


  /// \brief Loop through the blocks and draw each one
  void
  drawNonEmptyBlocks_Forward();


  /// \brief Compute the base vertex offset for the slices vertex buffer based
  ///        off the largest component of \c viewdir.
  int
  computeBaseVertexFromViewDir(glm::vec3 const &viewdir);


  /// \brief Sort visible blocks by distance from the camera (for painter's algorithm)
  /// Distance is from camera to center of each block. Order is decending.
  void
  sortBlocks();

  void
  filterBlocksByROV();

  /// By which metric should the blocks be deemed visible or not.
  ClassificationType m_classificationType;
  /// Number of slices per block
  int m_numSlicesPerBlock;
  /// Transfer function scaling value
  float m_tfuncScaleValue;
  double m_rov_min;
  double m_rov_max;
  uint64_t m_timeOfLastJob;
  /// True to draw bounding boxes.
  bool m_drawNonEmptyBoundingBoxes;
  bool m_drawNonEmptySlices;
  bool m_ROVRangeChanged;
  /// Show bounding boxes if rov is changing.
  bool m_ROVChanging;
  bool m_cacheNeedsUpdating;
  /// True to use Phong lighting shader.
  bool m_shouldUseLighting;
  /// Current background color.
  glm::vec3 m_backgroundColor;
  /// Transfer function texture
  bd::Texture const *m_colorMapTexture;
  /// Current shader being used (lighting, flat, wire, etc).
  bd::ShaderProgram *m_currentShader;

  SliceSet m_selectedSliceSet;

  std::shared_ptr<bd::ShaderProgram> m_volumeShader;
  std::shared_ptr<bd::ShaderProgram> m_volumeShaderLighting;
  std::shared_ptr<bd::ShaderProgram> m_wireframeShader;
  std::shared_ptr<bd::VertexArrayObject> m_quadsVao;    ///< Quad geometry verts
  std::shared_ptr<bd::VertexArrayObject> m_boxesVao;    ///< bounding box wireframe verts
  std::shared_ptr<bd::VertexArrayObject> m_axisVao;
  subvol::BlockCollection *m_collection;

  std::vector<bd::Block *> *m_nonEmptyBlocks;  ///< Non-empty blocks to draw.
  std::vector<bd::Block *> *m_blocks;       ///< All the blocks!

  //std::vector<bd::Block *> m_renderableBlocks;  ///< Blocks that are on the m_gpu.

};

} // namepsace subvol

#endif // blockrenderer_h__

