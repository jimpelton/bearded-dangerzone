//
// Created by jim on 2/17/19.
//

#ifndef SUBVOL_BLOCKRENDERER_H
#define SUBVOL_BLOCKRENDERER_H

#include <bd/graphics/texture.h>
#include <bd/graphics/renderer.h>

#include <glm/glm.hpp>

namespace subvol
{

class BlockRenderer : public bd::Renderer
{
public:

  /// \brief Set the transfer function texture.
  virtual void
  setColorMapTexture(bd::Texture const &tfunc)
  {
    _colorMap = &tfunc;
  }


  virtual void
  setColorMapScaleValue(float val)
  {
    _tfuncScaleValue = val;
  };


  virtual float
  getColorMapScaleValue() const
  {
    return _tfuncScaleValue;
  };


  virtual void
  setShaderNShiney(float n)
  {
    _shaderNShiney = n;
  };


  virtual void
  setShaderLightPos(glm::vec3 const &L)
  {
    _shaderLightPos = L;
  };


//  glm::vec3 const &
//  getShaderLightPos() const;


  virtual void
  setShaderMaterial(glm::vec3 const &M)
  {
    _shaderMat = M;
  };


//  glm::vec3 const &
//  getShaderMaterial() const;


  virtual void
  setBackgroundColor(glm::vec4 const &c);


  virtual glm::vec4 const &
  getBackgroundColor() const
  {
    return _backgroundColor;
  };


  virtual void
  setShouldUseLighting(bool b)
  {
    _shouldUseLighting = true;
  };


  virtual bool
  getShouldUseLighting() const
  {
    return _shouldUseLighting;
  };


  virtual void
  setDrawNonEmptyBoundingBoxes(bool b)
  {
    _drawNonEmptyBoundingBoxes = b;
  };

  virtual bool
  getDrawNonEmptyBoundingBoxes(bool b)
  {
    return _drawNonEmptyBoundingBoxes;
  };

  virtual void
  setDrawNonEmptyBlocks(bool b)
  {
    _drawNonEmptyBlocks = b;
  };

  virtual bool
  getDrawNonEmptyBlocks(bool b)
  {
    return _drawNonEmptyBlocks;
  };

protected:
  float _tfuncScaleValue;
  /// True to draw bounding boxes.
  bool _drawNonEmptyBoundingBoxes;
  /// True to draw the non-empty blocks
  bool _drawNonEmptyBlocks;
  /// Show bounding boxes if rov is changing.
  bool _rangeChanging;
  /// True to use Phong lighting shader.
  bool _shouldUseLighting;
  /// Current background color.
  glm::vec4 _backgroundColor;
  glm::vec3 _shaderLightPos;
  glm::vec3 _shaderMat;
  float _shaderNShiney;
  bd::Texture const *_colorMap;


};
} // namespace subvol
#endif //SUBVOL_BLOCKRENDERER_H
