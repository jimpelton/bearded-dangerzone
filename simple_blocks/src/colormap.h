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

class ColorMap
{

public:
  static void generateDefaultTransferFunctionTextures();

  /// \brief Get the texture of colormap with name.
  /// \throws std::out_of_range if name is not a default colormap
  static const bd::Texture* getDefaultMapTexture(const std::string& name);

  static const bd::Texture* load_1dt(const std::string& filename);

private:
  static void interpolateTexels(std::vector< glm::vec4 > * texels,
                                const std::vector< glm::vec4 >& map);

  static glm::vec4 lerp(glm::vec4, glm::vec4, float);


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
  static const std::unordered_map<std::string, const std::vector<glm::vec4>* > s_mapPtrs;

private:
  static std::unordered_map<std::string, const bd::Texture*> s_textures;




}; // class ColorMap

} // namespace subvol

#endif // ! SUBVOL_COLORMAPS_H
