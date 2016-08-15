//
// Created by Jim Pelton on 5/22/16.
//

#ifndef SUBVOL_COLORMAPS_H
#define SUBVOL_COLORMAPS_H

#include <bd/graphics/texture.h>

#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>


namespace subvol
{

class ColorMapManager
{

public:

  /// \brief Generate the set of predefined transfer functions.
  static void
  generateDefaultTransferFunctionTextures();

  /// \brief Get the texture of colormap with name.
  /// \throws std::out_of_range if name is not a default colormap
  static bd::Texture const &
  getMapTextureByName(std::string const &name) ;

  /// \brief Get the strings of the names of each color map in the
  /// list of maps.
  static std::vector<std::string>
  getMapNameStrings();

  /// \brief Load a .1dt format transfer function
  /// This is the type of 1D transfer function exported by ImageVis3D
  static void
  load_1dt(std::string const &funcName, std::string const &filename); //TODO: error handling in load_1dt



private:
  /// \brief Fill texels with interpolated values between the knots in map.
  static void
  interpolateTexels(std::vector<glm::vec4> * texels,
                    std::vector<glm::vec4> const &map);

  static void
  do_generateTransferFunctionTexture(std::string const &name,
                                     std::vector<glm::vec4> const &func);

  static glm::vec4
  lerp(glm::vec4, glm::vec4, float);


  static const std::vector<glm::vec4> FULL_RAINBOW;
  static const std::vector<glm::vec4> INVERSE_FULL_RAINBOW;
  /* RAINBOW */
  static const std::vector<glm::vec4> RAINBOW;
  static const std::vector<glm::vec4> INVERSE_RAINBOW;
  /* COLD_TO_HOT */
  static const std::vector<glm::vec4> COLD_TO_HOT;
  static const std::vector<glm::vec4> HOT_TO_COLD;
  /* BLACK_TO_WHITE */
  static const std::vector<glm::vec4> BLACK_TO_WHITE;
  static const std::vector<glm::vec4> WHITE_TO_BLACK;
  /* HSB_HUES */
  static const std::vector<glm::vec4> HSB_HUES;
  static const std::vector<glm::vec4> INVERSE_HSB_HUES;
  /* DAVINCI */
  static const std::vector<glm::vec4> DAVINCI;
  static const std::vector<glm::vec4> INVERSE_DAVINCI;
  /* SEISMIC */
  static const std::vector<glm::vec4> SEISMIC;
  static const std::vector<glm::vec4> INVERSE_SEISMIC;

  /// \brief Holds the textures generated.
  static std::unordered_map<std::string, bd::Texture const *> s_textures;

}; // class ColorMapManager

} // namespace subvol

#endif // ! SUBVOL_COLORMAPS_H
