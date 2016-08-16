//
// Created by Jim Pelton on 5/22/16.
//


#include "colormap.h"

#include <bd/log/logger.h>

#include <fstream>
#include <algorithm>

using bd::Err;
using bd::Info;

namespace subvol
{

// These colormaps are lifted out of Toirt volume renderer.
// r, g, b, scalar

const std::vector<glm::vec4> ColorMapManager::FULL_RAINBOW {
    glm::vec4{ 0.51f, 0.93f , 0.00f, 0.93f},
    glm::vec4{ 0.00f, 0.40f , 0.16f, 0.40f},
    glm::vec4{ 0.00f, 1.00f , 0.33f, 0.00f},
    glm::vec4{ 1.00f, 0.00f , 0.50f, 0.00f},
    glm::vec4{ 1.00f, 0.00f , 0.66f, 1.00f},
    glm::vec4{ 0.50f, 0.00f , 0.83f, 1.00f},
    glm::vec4{ 0.00f, 0.00f , 1.00f, 1.00f}
};

const std::vector<glm::vec4> ColorMapManager::INVERSE_FULL_RAINBOW {
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f},
    glm::vec4{ 1.00f, 0.50f, 0.00f, 0.16f},
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.33f},
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.50f},
    glm::vec4{ 0.00f, 0.00f, 1.00f, 0.66f},
    glm::vec4{ 0.40f, 0.00f, 0.40f, 0.83f},
    glm::vec4{ 0.93f, 0.51f, 0.93f, 1.00f}
};

const std::vector<glm::vec4> ColorMapManager::RAINBOW {
    glm::vec4{  0.00f, 0.00f, 1.00f, 0.00f},
    glm::vec4{  0.00f, 1.00f, 1.00f, 0.25f},
    glm::vec4{  0.00f, 1.00f, 0.00f, 0.50f},
    glm::vec4{  1.00f, 1.00f, 0.00f, 0.75f},
    glm::vec4{  1.00f, 0.00f, 0.00f, 1.00f}
};


const std::vector<glm::vec4> ColorMapManager::INVERSE_RAINBOW {
    glm::vec4{  1.00f, 0.00f, 0.00f, 0.00f},
    glm::vec4{  1.00f, 1.00f, 0.00f, 0.25f},
    glm::vec4{  0.00f, 1.00f, 0.00f, 0.50f},
    glm::vec4{  0.00f, 1.00f, 1.00f, 0.75f},
    glm::vec4{  0.00f, 0.00f, 1.00f, 1.00f}
};

const std::vector<glm::vec4> ColorMapManager::COLD_TO_HOT {
    glm::vec4{  0.00f, 0.00f, 1.00f ,0.00f },
    glm::vec4{  0.75f, 0.00f, 0.75f ,0.50f },
    glm::vec4{  1.00f, 0.00f, 0.00f ,1.00f }
};


const std::vector<glm::vec4> ColorMapManager::HOT_TO_COLD {
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f},
    glm::vec4{ 0.75f, 0.00f, 0.75f, 0.50f},
    glm::vec4{ 0.00f, 0.00f, 1.00f, 1.00f}
};

const  std::vector<glm::vec4> ColorMapManager::BLACK_TO_WHITE {
    glm::vec4{ 0.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 1.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::WHITE_TO_BLACK {
    glm::vec4{ 1.00f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 0.00f, 0.00f, 0.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::HSB_HUES {
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.16f },
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.33f },
    glm::vec4{ 0.00f, 1.00f, 1.00f, 0.50f },
    glm::vec4{ 0.00f, 0.00f, 1.00f, 0.66f },
    glm::vec4{ 1.00f, 0.00f, 1.00f, 0.83f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::INVERSE_HSB_HUES {
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 1.00f, 0.00f, 1.00f, 0.16f },
    glm::vec4{ 0.00f, 0.00f, 1.00f, 0.33f },
    glm::vec4{ 0.00f, 1.00f, 1.00f, 0.50f },
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.66f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.83f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::DAVINCI {
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

const std::vector<glm::vec4> ColorMapManager::INVERSE_DAVINCI {
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

const std::vector<glm::vec4> ColorMapManager::SEISMIC {
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 1.00f, 0.50f },
    glm::vec4{ 0.00f, 0.00f, 1.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMapManager::INVERSE_SEISMIC {
    glm::vec4{ 0.00f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 1.00f, 0.50f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 1.00f }
};


std::unordered_map<std::string, bd::Texture const *> ColorMapManager::s_textures;
std::vector<std::string const *> ColorMapManager::s_colorMapNames;
int ColorMapManager::s_currentMapName{ 0 };



/* static */
const bd::Texture &
ColorMapManager::getMapTextureByName(std::string const &name)
{
  const bd::Texture *rval{ nullptr };
  try{
    rval = s_textures.at(name);

    s_currentMapName = std::find_if(s_colorMapNames.begin(), s_colorMapNames.end(),
                                    [&name] (std::string const *s)
                                    {
                                      return name == *s;
                                    } ) - s_colorMapNames.begin();

  } catch (std::out_of_range e) {
    Err() << name << " is not a colormap.";
    throw e;
  }
  return *rval;
}

bd::Texture const &
ColorMapManager::getNextMapTexture()
{
  s_currentMapName += 1;
  if (s_currentMapName >= s_colorMapNames.size()) {
    s_currentMapName = 0;
  }
  std::string const * name{ s_colorMapNames[s_currentMapName] };
  return *(s_textures.find(*name)->second);
}


///////////////////////////////////////////////////////////////////////////////
std::vector<std::string>
ColorMapManager::getMapNameStrings()
{
  std::vector<std::string> result;
  for(auto pair : s_textures) {
    result.push_back(pair.first);
  }
  return result;
}

/* static */
void
ColorMapManager::generateDefaultTransferFunctionTextures()
{
  do_generateTransferFunctionTexture("FULL_RAINBOW", FULL_RAINBOW);
  do_generateTransferFunctionTexture("INVERSE_FULL_RAINBOW", INVERSE_FULL_RAINBOW);

  do_generateTransferFunctionTexture("RAINBOW", RAINBOW);
  do_generateTransferFunctionTexture("INVERSE_RAINBOW", INVERSE_RAINBOW);

  do_generateTransferFunctionTexture("COLD_TO_HOT", COLD_TO_HOT);
  do_generateTransferFunctionTexture("HOT_TO_COLD", HOT_TO_COLD);

  do_generateTransferFunctionTexture("BLACK_TO_WHITE", BLACK_TO_WHITE);
  do_generateTransferFunctionTexture("WHITE_TO_BLACK", WHITE_TO_BLACK);

  do_generateTransferFunctionTexture("HSB_HUES", HSB_HUES);
  do_generateTransferFunctionTexture("INVERSE_HSB_HUES", INVERSE_HSB_HUES);

  do_generateTransferFunctionTexture("DAVINCI", DAVINCI);
  do_generateTransferFunctionTexture("INVERSE_DAVINCI", INVERSE_DAVINCI);

  do_generateTransferFunctionTexture("SEISMIC", SEISMIC);
  do_generateTransferFunctionTexture("INVERSE_SEISMIC", INVERSE_SEISMIC);

  for(auto &pair : s_textures) {
    s_colorMapNames.push_back(&pair.first);
  }


  Info() << "Generated " << s_textures.size() << " built-in colormaps.";

}


void
ColorMapManager::load_1dt(std::string const &funcName, std::string const &filename)
{
  bd::Dbg() << "Reading 1dt formatted transfer function file and generating texture.";

  std::ifstream file(filename.c_str(), std::ifstream::in);
  if (!file.is_open()) {
    bd::Err() << "Can't open tfunc file: " << filename;
  }


  size_t lineNum{ 0 }, numKnots{ 0 };
  file >> numKnots; // number of entries/lines in the 1dt file.
  lineNum++;
  if (numKnots > 8192) {
    bd::Err() << "The 1dt transfer function has " << numKnots
              << " knots but max allowed is 8192)."
                "Skipping loading the transfer function file.";
  }

  // read rest of file consisting of rgba colors
  std::vector<glm::vec4> rgba;
  float r, g, b, a;
  while (lineNum < numKnots && file >> r >> g >> b >> a) {
    rgba.push_back(glm::vec4{ r, g, b, a });
    lineNum++;
  }
  file.close();

  do_generateTransferFunctionTexture(funcName, rgba);
  std::string const * name{ &s_textures.find(funcName)->first };
  s_colorMapNames.push_back(name);

}

/* static */
void
ColorMapManager::interpolateTexels(std::vector< glm::vec4 > * texels,
                            const std::vector< glm::vec4 >& map)
{

//  const unsigned int RED = 0; const unsigned int GREEN = 1;
//  const unsigned int BLUE = 2; const unsigned int ALPHA = 3;

  size_t steps{ texels->size() }; // total steps
  float stepDelta{ map.size()/float(steps) };

  auto knots = std::begin(map);
  auto itEnd = std::end(map);

  glm::vec4 leftKnot{ *knots };
  glm::vec4 rightKnot{ *++knots };

  size_t stepsBetween{ steps/map.size() };
  size_t idx{ 0 };
  float a{ 0.0f };

  while(knots != itEnd) {
    for (size_t i{ 0 }; i < stepsBetween; ++i) {
      glm::vec4 v{ lerp(leftKnot, rightKnot, a) };
      (*texels)[idx++] = glm::vec4{ v[0], v[1], v[2], v[3] }; //use the scalar as the alpha value
      a += stepDelta;
    }

    leftKnot = rightKnot;
    rightKnot = *++knots;
  }
}

///////////////////////////////////////////////////////////////////////////////
void
ColorMapManager::do_generateTransferFunctionTexture(
  std::string const &name,
  std::vector<glm::vec4> const &func)
{

  float const *textureData{ reinterpret_cast<float const*>(func.data()) };
  size_t numElements{ func.size() * 4};         // 4 elements in glm::vec4

  bd::Texture *t{ new bd::Texture{ bd::Texture::Target::Tex1D } };
  unsigned int texId { t->genGLTex1d(textureData,
                                     bd::Texture::Format::RGBA,
                                     bd::Texture::Format::RGBA,
                                     numElements) };

  if (texId == 0) {
    Err() << "The texture for colormap " << name << " could not be created.";
    delete t;
    t = nullptr;
  } else {
    s_textures[name] = t;
    bd::Info() << "Added " << name << " colormap texture.";
  }

}

/* static */
glm::vec4
ColorMapManager::lerp(glm::vec4 x, glm::vec4 y, float a)
{
  return x * (1.0f - a) + y * a;
}

} // namespace subvol