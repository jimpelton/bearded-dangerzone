//
// Created by jim on 10/22/15.
//

#ifndef blockrenderer_h__
#define blockrenderer_h__

#include "sliceset.h"
#include "renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <bd/graphics/shader.h>
#include <bd/graphics/texture.h>
#include <bd/volume/blockcollection.h>
#include <bd/graphics/vertexarrayobject.h>

#include <glm/fwd.hpp>

#include <memory>

namespace subvol
{

class BlockRenderer : public Renderer
{

public:
  BlockRenderer();


  //////////////////////////////////////////////////////////////////////////////
  BlockRenderer(int numSlices,
                std::shared_ptr<bd::ShaderProgram> volumeShader,
                std::shared_ptr<bd::ShaderProgram> wireframeShader,
                std::vector<bd::Block*>* blocks,
                std::shared_ptr<bd::VertexArrayObject> blocksVAO,
                std::shared_ptr<bd::VertexArrayObject> bboxVAO,
                std::shared_ptr<bd::VertexArrayObject> axisVao);


  virtual ~BlockRenderer();


  //////////////////////////////////////////////////////////////////////////////
  /// \brief Draw each non-empty block.
  void drawNonEmptyBlocks();


  //////////////////////////////////////////////////////////////////////////////
  /// \brief Draw wireframe bounding boxes around the blocks.
  /// \param[in] vp View projection matrix.
  void drawNonEmptyBoundingBoxes();


  //////////////////////////////////////////////////////////////////////////////
  bool init();


  /// \brief Set the transfer function texture.
  void setTFuncTexture(bd::Texture const& tfunc);


  //////////////////////////////////////////////////////////////////////////////
  void setTfuncScaleValue(float val);


  //////////////////////////////////////////////////////////////////////////////
  void setBackgroundColor(glm::vec3 const &c);


private:

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Disable GL_DEPTH_TEST and draw transparent slices
  void drawSlices(int baseVertex);


  //////////////////////////////////////////////////////////////////////////////
  /// \brief Loop through the blocks and draw each one
  void drawNonEmptyBlocks_Forward();

//  void setInitialGLState();

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Determine the viewing direction and draw the blocks in proper
  ///        order.


  //////////////////////////////////////////////////////////////////////////////
  /// \brief Compute the base vertex offset for the slices vertex buffer based
  ///        off the largest component of \c viewdir.
  int computeBaseVertexFromViewDir();


private:

  int m_numSlicesPerBlock;            ///< Number of slices per block
  float m_tfuncScaleValue;            ///< Transfer function scaling value
  bool m_drawNonEmptyBoundingBoxes;   ///< True to draw bounding boxes.
  glm::vec3 m_backgroundColor;        ///< Current background color.

  SliceSet m_selectedSliceSet;
  std::shared_ptr<bd::ShaderProgram> m_volumeShader;
  std::shared_ptr<bd::ShaderProgram> m_wireframeShader;
  std::vector<bd::Block*> *m_blocks;
  bd::Texture const* m_colorMapTexture; ///< Transfer function texture
  std::shared_ptr<bd::VertexArrayObject> m_quadsVao;    ///< Quad geometry verts
  std::shared_ptr<bd::VertexArrayObject> m_boxesVao;    ///< bounding box wireframe verts
  std::shared_ptr<bd::VertexArrayObject> m_axisVao;
};

} // namepsace subvol

#endif // blockrenderer_h__

