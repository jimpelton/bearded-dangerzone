//
// Created by Jim Pelton on 5/22/16.
//

#ifndef SUBVOL_COLORMAPS_H
#define SUBVOL_COLORMAPS_H

#include <glm/glm.hpp>

#include <array>

namespace subvol
{


//struct RGBAColor
//{
//  RGBAColor(float r, float g, float b, float a)
//      : r{ r }
//      , g{ g }
//      , b{ b }
//      , a{ a }
//  { }
//
//
//  float r,g,b,a;
//
//};

class ColorMap
{


public:

  static const std::array<glm::vec4, 7> FULL_RAINBOW;

  static const std::array<glm::vec4, 7> INVERSE_FULL_RAINBOW;

/* RAINBOW */
  static const std::array<glm::vec4, 5> RAINBOW;
  static const std::array<glm::vec4, 5> INVERSE_RAINBOW;

/* COLD_TO_HOT */
  static const std::array<glm::vec4, 3> COLD_TO_HOT;
  static const std::array<glm::vec4, 3> HOT_TO_COLD;

/* BLACK_TO_WHITE */
  static const  std::array<glm::vec4, 2> BLACK_TO_WHITE;

  static const std::array<glm::vec4, 2> WHITE_TO_BLACK;

/* HSB_HUES */
  static const std::array<glm::vec4, 7> HSB_HUES;

  static const std::array<glm::vec4, 7> INVERSE_HSB_HUES;
/* DAVINCI */
  static const std::array<glm::vec4, 11> DAVINCI;

  static const std::array<glm::vec4, 11> INVERSE_DAVINCI;
/* SEISMIC */
  static const std::array<glm::vec4, 3> SEISMIC;

  static const std::array<glm::vec4, 3> INVERSE_SEISMIC;

  static const std::map<std::string, glm::vec4*> maps;
}; // class ColorMap

} // namespace subvol

#endif // ! SUBVOL_COLORMAPS_H
