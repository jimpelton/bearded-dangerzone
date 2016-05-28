//
// Created by Jim Pelton on 5/22/16.
//

#ifndef SUBVOL_COLORMAPS_H
#define SUBVOL_COLORMAPS_H

#include <bd/graphics/texture.h>

#include <glm/glm.hpp>

#include <vector>
#include <map>


namespace subvol
{

class ColorMap
{


public:
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
  /* ALL OF EM */
  static const std::map<const char*, const std::vector<glm::vec4>* > maps;


public:
  static void generateTransferFunctionTextures();

  /// \brief Get the texture of colormap with name.
  /// \throws std::out_of_range
  static bd::Texture* getMapTexture(const std::string &name);

private:
  static void makeTexture(std::vector<glm::vec4> *texels, const std::vector<glm::vec4> &map);
  static glm::vec4 lerp(glm::vec4, glm::vec4, float);


public:

private:
  static std::map<std::string, bd::Texture*> textures;
}; // class ColorMap

} // namespace subvol

#endif // ! SUBVOL_COLORMAPS_H
