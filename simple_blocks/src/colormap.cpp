//
// Created by Jim Pelton on 5/22/16.
//


#include "colormap.h"

#include <bd/log/logger.h>

#include <fstream>
#include <algorithm>
#include <set>

using bd::Err;
using bd::Info;

namespace subvol
{

/**********************************************************************************
 *  ColorMap Implementation
***********************************************************************************/



ColorMap::ColorMap()
    : m_ctf{ }
    , m_otf{ }
    , m_texture{ bd::Texture::Target::Tex1D }
    , m_name{ "default-name" }
{
}


//ColorMap::ColorMap(std::string const &name)
//    : m_ctf{ }
//    , m_otf{ }
//    , m_texture{ bd::Texture::Target::Tex1D }
//    , m_name{ name }
//{
//
//}

ColorMap::ColorMap(std::string const &name, std::vector<glm::vec4> const &knots)
    : m_ctf{ }
    , m_otf{ }
    , m_texture{ bd::Texture::Target::Tex1D }
    , m_name{ name }
{
  loadFromKnots(name, knots);
}


ColorMap::~ColorMap()
{
}


bool
ColorMap::loadFromKnots(std::string const &name, std::vector<glm::vec4> const &knots)
{
  return generateColorMapTexture(name, knots);
}


bool
ColorMap::load(std::string const &funcName,
               std::string const &colorTF,
               std::string const &opacityTF)
{
  try {
    bd::ColorTransferFunction ctf;
    ctf.load(colorTF);

    bd::OpacityTransferFunction otf;
    otf.load(opacityTF);

    std::set<double> scalars;

    for (auto &k : otf.getKnotsVector()) {
      scalars.insert(k.s);
    }

    for (auto &k : ctf.getKnotsVector()) {
      scalars.insert(k.s);
    }

    std::vector<glm::vec4> knots;
    for (auto &k : scalars) {
      double opacity{ otf.interpolate(k) };
      bd::Color color{ ctf.interpolate(k) };
      knots.push_back({ color.r, color.g, color.b, opacity });
    }

    m_ctf = ctf;
    m_otf = otf;

    generateColorMapTexture(funcName, knots);

  }
  catch (std::exception e) {
    bd::Err() << "Exception encountered when loading user defined colormap: "
        "Name: " << funcName << ". Message was: " << e.what();
    return false;
  }

  return true;
}


bd::ColorTransferFunction const &
ColorMap::getCtf() const
{
  return m_ctf;
}


void
ColorMap::setCtf(bd::ColorTransferFunction const &ctf)
{
  m_ctf = ctf;
}


bd::OpacityTransferFunction const &
ColorMap::getOtf() const
{
  return m_otf;
}


void
ColorMap::setOtf(bd::OpacityTransferFunction const &otf)
{
  m_otf = otf;
}


bd::Texture const &
ColorMap::getTexture() const
{
  return m_texture;
}


void
ColorMap::setTexture(bd::Texture const &texture)
{
  m_texture = texture;
}


std::string const &
ColorMap::getName() const
{
  return m_name;
}


void
ColorMap::setName(std::string const &name)
{
  m_name = name;
}


bool
ColorMap::generateColorMapTexture(std::string const &name,
                                  std::vector<glm::vec4> const &func)
{
  bool success{ true };

  float const *textureData{ reinterpret_cast<float const *>(func.data()) };
  size_t numElements{ func.size() * 4 };         // 4 elements in glm::vec4


  unsigned int texId{ 0 };
  texId = m_texture.genGLTex1d(textureData,
                               bd::Texture::Format::RGBA,
                               bd::Texture::Format::RGBA,
                               numElements);

  if (texId == 0) {
    Err() << "The texture for colormap " << name << " could not be created.";
    success = false;
  }
//  } else {
////    s_textures[name] = t;
////    Info() << "Added " << name << " colormap texture.";
//  }

  return success;
}

std::string
ColorMap::to_string() const
{
  std::stringstream ss;
  ss << "{Name: " << m_name
     << ", CTF: " << m_ctf << ", OTF: " << m_otf;

  ss << '}';

  return ss.str();

}



/**********************************************************************************
 *  ColorMapManager Implementation
***********************************************************************************/



// These colormaps are lifted out of Toirt volume renderer.
// r, g, b, scalar

const std::vector<glm::vec4> ColorMapManager::FULL_RAINBOW{
    glm::vec4{ 0.51f, 0.93f, 0.93f, 0.00f },
    glm::vec4{ 0.00f, 0.40f, 0.40f, 0.16f },
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.33f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.50f },
    glm::vec4{ 1.00f, 0.00f, 1.00f, 0.66f },
    glm::vec4{ 0.50f, 0.00f, 1.00f, 0.83f },
    glm::vec4{ 0.00f, 0.00f, 1.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::INVERSE_FULL_RAINBOW{
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 1.00f, 0.50f, 0.00f, 0.16f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.33f },
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.50f },
    glm::vec4{ 0.00f, 0.00f, 1.00f, 0.66f },
    glm::vec4{ 0.40f, 0.00f, 0.40f, 0.83f },
    glm::vec4{ 0.93f, 0.51f, 0.93f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::RAINBOW{
    glm::vec4{ 0.00f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 0.00f, 1.00f, 1.00f, 0.25f },
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.50f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.75f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::INVERSE_RAINBOW{
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.25f },
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.50f },
    glm::vec4{ 0.00f, 1.00f, 1.00f, 0.75f },
    glm::vec4{ 0.00f, 0.00f, 1.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::COLD_TO_HOT{
    glm::vec4{ 0.00f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 0.75f, 0.00f, 0.75f, 0.50f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::HOT_TO_COLD{
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 0.75f, 0.00f, 0.75f, 0.50f },
    glm::vec4{ 0.00f, 0.00f, 1.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::BLACK_TO_WHITE{
    glm::vec4{ 0.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 1.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::WHITE_TO_BLACK{
    glm::vec4{ 1.00f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 0.00f, 0.00f, 0.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::HSB_HUES{
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.16f },
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.33f },
    glm::vec4{ 0.00f, 1.00f, 1.00f, 0.50f },
    glm::vec4{ 0.00f, 0.00f, 1.00f, 0.66f },
    glm::vec4{ 1.00f, 0.00f, 1.00f, 0.83f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::INVERSE_HSB_HUES{
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 1.00f, 0.00f, 1.00f, 0.16f },
    glm::vec4{ 0.00f, 0.00f, 1.00f, 0.33f },
    glm::vec4{ 0.00f, 1.00f, 1.00f, 0.50f },
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.66f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.83f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::DAVINCI{
    glm::vec4{ 0.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 0.18f, 0.08f, 0.00f, 0.10f },
    glm::vec4{ 0.27f, 0.18f, 0.08f, 0.20f },
    glm::vec4{ 0.37f, 0.27f, 0.18f, 0.30f },
    glm::vec4{ 0.47f, 0.37f, 0.27f, 0.40f },
    glm::vec4{ 0.57f, 0.47f, 0.37f, 0.50f },
    glm::vec4{ 0.67f, 0.57f, 0.47f, 0.60f },
    glm::vec4{ 0.76f, 0.67f, 0.57f, 0.70f },
    glm::vec4{ 0.86f, 0.76f, 0.67f, 0.80f },
    glm::vec4{ 1.00f, 0.86f, 0.76f, 0.90f },
    glm::vec4{ 1.00f, 1.00f, 1.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::INVERSE_DAVINCI{
    glm::vec4{ 1.00f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 1.00f, 0.86f, 0.76f, 0.10f },
    glm::vec4{ 0.86f, 0.76f, 0.67f, 0.20f },
    glm::vec4{ 0.76f, 0.67f, 0.57f, 0.30f },
    glm::vec4{ 0.67f, 0.57f, 0.47f, 0.40f },
    glm::vec4{ 0.57f, 0.47f, 0.37f, 0.50f },
    glm::vec4{ 0.47f, 0.37f, 0.27f, 0.60f },
    glm::vec4{ 0.37f, 0.27f, 0.18f, 0.70f },
    glm::vec4{ 0.27f, 0.18f, 0.08f, 0.80f },
    glm::vec4{ 0.18f, 0.08f, 0.00f, 0.90f },
    glm::vec4{ 0.00f, 0.00f, 0.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::SEISMIC{
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 1.00f, 0.50f },
    glm::vec4{ 0.00f, 0.00f, 1.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::INVERSE_SEISMIC{
    glm::vec4{ 0.00f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 1.00f, 0.50f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 1.00f }
};

std::unordered_map<std::string, ColorMap> ColorMapManager::s_maps;

std::vector<std::string const *> ColorMapManager::s_colorMapNames;

int ColorMapManager::s_currentMapNameIdx{ 0 };


/* static */
void
ColorMapManager::generateDefaultTransferFunctionTextures()
{

  s_maps["FULL_RAINBOW"] = { "FULL_RAINBOW", FULL_RAINBOW };
  s_maps["INVERSE_FULL_RAINBOW"] = ColorMap("INVERSE_FULL_RAINBOW", INVERSE_FULL_RAINBOW);
  s_maps["RAINBOW"] = ColorMap("RAINBOW", RAINBOW);
  s_maps["INVERSE_RAINBOW"] = ColorMap("INVERSE_RAINBOW", INVERSE_RAINBOW);

  s_maps["COLD_TO_HOT"] = ColorMap("COLD_TO_HOT", COLD_TO_HOT);
  s_maps["HOT_TO_COLD"] = ColorMap("HOT_TO_COLD", HOT_TO_COLD);

  s_maps["BLACK_TO_WHITE"] = ColorMap("BLACK_TO_WHITE", BLACK_TO_WHITE);
  s_maps["WHITE_TO_BLACK"] = ColorMap("WHITE_TO_BLACK", WHITE_TO_BLACK);

  s_maps["HSB_HUES"] = ColorMap("HSB_HUES", HSB_HUES);
  s_maps["INVERSE_HSB_HUES"] = ColorMap("INVERSE_HSB_HUES", INVERSE_HSB_HUES);

  s_maps["DAVINVI"] = ColorMap("DAVINCI", DAVINCI);
  s_maps["INVERSE_DAVINCI"] = ColorMap("INVERSE_DAVINCI", INVERSE_DAVINCI);

  s_maps["SEISMIC"] = ColorMap("SEISMIC", SEISMIC);
  s_maps["INVERSE_SEISMIC"] = ColorMap("INVERSE_SEISMIC", INVERSE_SEISMIC);

  for (auto &pair : s_maps) {
    s_colorMapNames.push_back(&pair.first);
  }

  Info() << "Generated " << s_maps.size() << " built-in colormaps.";

}


/* static */
const ColorMap &
ColorMapManager::getMapByName(std::string const &name)
{
  try {

    s_currentMapNameIdx =
        std::find_if(s_colorMapNames.begin(),
                     s_colorMapNames.end(),
                     [&name](std::string const *s) {
                       return name == *s;
                     }) - s_colorMapNames.begin();

    return s_maps.at(name);

  }
  catch (std::out_of_range e) {
    Err() << name << " is not a colormap.";
    throw e;
  }
}


/* static */
ColorMap const &
ColorMapManager::getNextMap()
{
  s_currentMapNameIdx += 1;
  if (s_currentMapNameIdx >= s_colorMapNames.size()) {
    s_currentMapNameIdx = 0;
  }
  std::string const *name{ s_colorMapNames[s_currentMapNameIdx] };
  return s_maps.find(*name)->second;
}


/* static */
ColorMap const &
ColorMapManager::getPrevMap()
{
  s_currentMapNameIdx -= 1;
  if (s_currentMapNameIdx < 0) {
    s_currentMapNameIdx = int(s_colorMapNames.size()) - 1;
  }
  std::string const *name{ s_colorMapNames[s_currentMapNameIdx] };
  return s_maps.find(*name)->second;
}


/* static */
std::vector<std::string>
ColorMapManager::getMapNameStrings()
{
  std::vector<std::string> result;
  for (auto pair : s_maps) {
    result.push_back(pair.first);
  }
  return result;
}


/* static */
std::string const &
ColorMapManager::getCurrentMapName()
{
  return *s_colorMapNames[s_currentMapNameIdx];
}


/* static */
bool
ColorMapManager::loadColorMap(std::string const &funcName,
                              std::string const &colorFilePath,
                              std::string const &opacityFilePath)
{
  bd::Info()
      << "Creating colormap from transfer functions: " << funcName << ','
      << " Color tf: " << colorFilePath << ','
      << " Opacity tf: " << opacityFilePath;

  ColorMap c;
  bool success = c.load(funcName, colorFilePath, opacityFilePath);
  if (!success) {
    return false;
  }


  return false;
}


/* static */
bool
ColorMapManager::load1DT(std::string const &funcName,
                         std::string const &filename)
{
  bd::Dbg() << "Reading 1dt formatted transfer function file and generating texture.";

  size_t lineNum{ 0 };
  size_t numKnots{ 0 };
  std::vector<glm::vec4> rgba;

  std::ifstream file;
  //file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

//  try {
  file.open(filename.c_str(), std::ifstream::in);
  if (!file.is_open()) {
    bd::Err() << "Can't open tfunc file: " << filename;
    return false;
  }
  file >> numKnots; // number of entries/lines in the 1dt file.
  lineNum++;

  if (numKnots > 8192) {
    bd::Err() << "The 1dt transfer function has " << numKnots
              << " knots but max allowed is 8192)."
                  "Skipping loading the transfer function file.";
    return false;
  }

  // read rest of file consisting of rgba colors
  float r, g, b, a;
  while (lineNum < numKnots && file >> r >> g >> b >> a) {
    rgba.push_back(glm::vec4{ r, g, b, a });
    lineNum++;
  }
  file.close();

  if (lineNum < numKnots) {
    bd::Err() << "Malformed 1dt transfer function (too few nots?).";
    return false;
  }


  ColorMap c{ funcName, rgba };
  s_maps[funcName] = c;

  std::string const *name{ &s_maps.find(funcName)->first };
  s_colorMapNames.push_back(name);

  return true;
}

///* static */
//void
//ColorMapManager::interpolateTexels(std::vector< glm::vec4 > * texels,
//                            const std::vector< glm::vec4 >& map)
//{
//
////  const unsigned int RED = 0; const unsigned int GREEN = 1;
////  const unsigned int BLUE = 2; const unsigned int ALPHA = 3;
//
//  size_t steps{ texels->size() }; // total steps
//  float stepDelta{ map.size()/float(steps) };
//
//  auto knots = std::begin(map);
//  auto itEnd = std::end(map);
//
//  glm::vec4 leftKnot{ *knots };
//  glm::vec4 rightKnot{ *++knots };
//
//  size_t stepsBetween{ steps/map.size() };
//  size_t idx{ 0 };
//  float a{ 0.0f };
//
//  while(knots != itEnd) {
//    for (size_t i{ 0 }; i < stepsBetween; ++i) {
//      glm::vec4 v{ lerp(leftKnot, rightKnot, a) };
//      (*texels)[idx++] = glm::vec4{ v[0], v[1], v[2], v[3] }; //use the scalar as the alpha value
//      a += stepDelta;
//    }
//
//    leftKnot = rightKnot;
//    rightKnot = *++knots;
//  }
//}






} // namespace subvol
