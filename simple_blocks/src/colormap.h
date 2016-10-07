//
// Created by Jim Pelton on 5/22/16.
//

#ifndef SUBVOL_COLORMAPS_H
#define SUBVOL_COLORMAPS_H

#include <bd/graphics/texture.h>
#include <bd/volume/transferfunction.h>
#include <bd/util/color.h>

#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>


namespace subvol
{

/**********************************************************************************
 *  ColorMap
***********************************************************************************/

class ColorOpacityKnot
{
public:
  double s;
  glm::vec4 rgba;
}; // class ColorOpacityKnot


class ColorMap
{
public:
  ColorMap();

//  ColorMap(std::string const &name);

  ColorMap(std::string const &name, std::vector<glm::vec4> const &knots);


  ~ColorMap();


  bool
  loadFromKnots(std::string const &name,
                std::vector<glm::vec4> const &knots);


  bool
  load(std::string const &funcName,
       std::string const &colorTF,
       std::string const &opacityTF);


  bd::ColorTransferFunction const &
  getCtf() const;


  void
  setCtf(bd::ColorTransferFunction const &ctf);


  bd::OpacityTransferFunction const &
  getOtf() const;


  void
  setOtf(bd::OpacityTransferFunction const &otf);


  bd::Texture const &
  getTexture() const;


  void
  setTexture(bd::Texture const &texture);


  std::string const &
  getName() const;


  void
  setName(std::string const &name);

  std::string
  to_string() const;

private:


  /// \brief Create tfunc texture and put it in the colormap map.
  /// \return true on success, false otherwise.
  bool
  generateColorMapTexture(std::string const &name,
                          std::vector<glm::vec4> const &func);


  bd::ColorTransferFunction m_ctf;
  bd::OpacityTransferFunction m_otf;
  bd::Texture m_texture;
  std::string m_name;

}; // class ColorMap

std::ostream &
operator<<(std::ostream &os, subvol::ColorMap const &c);


/**********************************************************************************
 *  ColorMapManager
***********************************************************************************/



class ColorMapManager
{
public:

  /// \brief Generate the set of predefined transfer functions.
  static void
  generateDefaultTransferFunctionTextures();


  /// \brief Get the texture of colormap with name.
  /// \throws std::out_of_range if name is not a default colormap
  static
  ColorMap const &
  getMapByName(std::string const &name);


  /// \brief Get the next color map Texture in the circular queue.
  static
  ColorMap const &
  getNextMap();


  /// \brief Get the previous color map Texture in the circular queue.
  static ColorMap const &
  getPrevMap();


  /// \brief Get the strings of the names of each color map in the
  /// list of maps.
  static std::vector<std::string>
  getMapNameStrings();


  static std::string const &
  getCurrentMapName();


  /// \brief Load a .1dt format transfer function
  /// \note This is the type of 1D transfer function exported by ImageVis3D
  /// \return false if the transfer function is malformed or has more than 8192 knots.
  /// \throws std::ifstream::failure if there is an error opening/reading/closing the file.
  static
  bool
  load1DT(std::string const &funcName,
          std::string const &filename); //TODO: error handling in load_1dt

  /// \brief Load a .1dt format transfer function
  /// \note This is the type of 1D transfer function exported by ImageVis3D
  /// \return false if the transfer function is malformed or has more than 8192 knots.
  /// \throws std::ifstream::failure if there is an error opening/reading/closing the file.
//  static bool
//  loadOpacity1D(std::string const &funcName, std::string const &filename); //TODO: error handling in load_1dt


  static
  bool
  loadColorMap(std::string const &funcName,
               std::string const &colorTF,
               std::string const &opacityTF);


private:
  /// \brief Fill texels with interpolated values between the knots in map.
//  static void
//  interpolateTexels(std::vector<glm::vec4> * texels,
//                    std::vector<glm::vec4> const &map);




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
  static std::unordered_map<std::string, ColorMap> s_maps;
  /// \brief Lists the names of the color maps
  /// (pointers are to std::string in s_textures map)
  static std::vector<std::string const *> s_colorMapNames;
  static int s_currentMapNameIdx;

}; // class ColorMapManager

} // namespace subvol

#endif // ! SUBVOL_COLORMAPS_H
