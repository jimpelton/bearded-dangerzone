//
// Created by jim on 10/22/15.
//

#ifndef volumerenderer_h__
#define volumerenderer_h__

#include "sliceset.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <bd/graphics/view.h>
#include <bd/graphics/shader.h>
#include <bd/volume/blockcollection.h>
#include <bd/graphics/vertexarrayobject.h>

#include <glm/fwd.hpp>

#include <memory>


class VolumeRenderer {

public:
  VolumeRenderer();


  //////////////////////////////////////////////////////////////////////////////
  VolumeRenderer(std::shared_ptr<bd::ShaderProgram> volumeShader,
            std::shared_ptr<bd::ShaderProgram> wireframeShader,
            std::shared_ptr<bd::BlockCollection> blockCollection,
            std::shared_ptr<Texture> tfuncTexture,
            std::shared_ptr<bd::VertexArrayObject> blocksVAO);

  //////////////////////////////////////////////////////////////////////////////
  virtual ~VolumeRenderer();


  //////////////////////////////////////////////////////////////////////////////
  //virtual void renderSingleFrame();

  void drawNonEmptyBlocks();

  void drawNonEmptyBoundingBoxes();

  //////////////////////////////////////////////////////////////////////////////
  bool init();


  //////////////////////////////////////////////////////////////////////////////
  void setTfuncScaleValue(float val);


  //////////////////////////////////////////////////////////////////////////////
  void setViewMatrix(const glm::mat4 &);


  //////////////////////////////////////////////////////////////////////////////
  void setNumSlices(int n);

private:

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Draw wireframe bounding boxes around the blocks.
  //////////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////////////
  /// \brief Disable GL_DEPTH_TEST and draw transparent slices
  //////////////////////////////////////////////////////////////////////////////
  void drawSlices(int baseVertex);


  //////////////////////////////////////////////////////////////////////////////
  /// \brief Loop through the blocks and draw each one
  //////////////////////////////////////////////////////////////////////////////
  void drawNonEmptyBlocks_Forward();


  //////////////////////////////////////////////////////////////////////////////
  /// \brief Determine the viewing direction and draw the blocks in proper order.
  //////////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////////////
  /// \brief Compute the base vertex offset for the slices vertex buffer based
  ///        off the largest component of \c viewdir.
  /////////////////////////////////////////////////////////////////////////////
  int computeBaseVertexFromViewDir(const glm::vec3 &viewdir);

private:

  int m_numSlicesPerBlock;            ///< Number of slices per block
  float m_tfuncScaleValue;            ///< Transfer function scaling value

  glm::mat4 m_viewMatrix;             ///< View matrix for the camera

  SliceSet m_selectedSliceSet; // { SliceSet::XY };

  std::shared_ptr<bd::ShaderProgram> m_volumeShader;
  std::shared_ptr<bd::ShaderProgram> m_wireframeShader;
  std::shared_ptr<bd::BlockCollection> m_blockCollection;
  std::shared_ptr<Texture> m_tfuncTexture;                   ///< Transfer function texture
  std::shared_ptr<bd::VertexArrayObject> m_quadsVao;

};


#endif //volumerenderer_h__
