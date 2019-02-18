//
// Created by jim on 2/17/19.
//

#ifndef SUBVOL_BLOCKRENDERER_H
#define SUBVOL_BLOCKRENDERER_H

#include <bd/graphics/texture.h>
#include <bd/graphics/renderer.h>

#import <glm/glm.hpp>

namespace subvol
{

class BlockRenderer : public bd::Renderer
{
public:

  /// \brief Set the transfer function texture.
  virtual void
  setColorMapTexture(bd::Texture const &tfunc);


  virtual void
  setColorMapScaleValue(float val);


  virtual float
  getColorMapScaleValue() const;


  virtual void
  setShaderNShiney(float n);


  virtual void
  setShaderLightPos(glm::vec3 const &L);


//  glm::vec3 const &
//  getShaderLightPos() const;


  virtual void
  setShaderMaterial(glm::vec3 const &M);


//  glm::vec3 const &
//  getShaderMaterial() const;


  virtual void
  setBackgroundColor(glm::vec3 const &c);


  virtual glm::vec3 const &
  getBackgroundColor() const = 0;


  virtual void
  setShouldUseLighting(bool b) = 0;


  virtual bool
  getShouldUseLighting() const = 0;


  virtual void
  setDrawNonEmptyBoundingBoxes(bool b) = 0;


protected:
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


};
} // namespace subvol
#endif //SUBVOL_BLOCKRENDERER_H
