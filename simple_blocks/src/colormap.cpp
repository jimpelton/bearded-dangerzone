//
// Created by Jim Pelton on 5/22/16.
//


#include "colormap.h"

#include <bd/log/logger.h>

#include <fstream>

using bd::Err;
using bd::Info;

namespace subvol
{

// These colormaps are lifted out of Toirt volume renderer.
// scalar, r, g, b

const std::vector<glm::vec4> ColorMap::FULL_RAINBOW {
    glm::vec4{ 0.00f, 0.93f, 0.51f, 0.93f },
    glm::vec4{ 0.16f, 0.40f, 0.00f, 0.40f },
    glm::vec4{ 0.33f, 0.00f, 0.00f, 1.00f },
    glm::vec4{ 0.50f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 0.66f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 0.83f, 1.00f, 0.50f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.00f }
};

const std::vector<glm::vec4> ColorMap::INVERSE_FULL_RAINBOW {
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.00f },
    glm::vec4{ 0.16f, 1.00f, 0.50f, 0.00f },
    glm::vec4{ 0.33f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 0.50f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 0.66f, 0.00f, 0.00f, 1.00f },
    glm::vec4{ 0.83f, 0.40f, 0.00f, 0.40f },
    glm::vec4{ 1.00f, 0.93f, 0.51f, 0.93f }
};

const std::vector<glm::vec4> ColorMap::RAINBOW {
    glm::vec4{ 0.00f, 0.00f, 0.00f, 1.00f },
    glm::vec4{ 0.25f, 0.00f, 1.00f, 1.00f },
    glm::vec4{ 0.50f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 0.75f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.00f }
};


const std::vector<glm::vec4> ColorMap::INVERSE_RAINBOW {
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.00f },
    glm::vec4{ 0.25f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 0.50f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 0.75f, 0.00f, 1.00f, 1.00f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMap::COLD_TO_HOT {
    glm::vec4{ 0.00f, 0.00f, 0.00f, 1.00f },
    glm::vec4{ 0.50f, 0.75f, 0.00f, 0.75f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.00f }
};


const std::vector<glm::vec4> ColorMap::HOT_TO_COLD {
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.00f },
    glm::vec4{ 0.50f, 0.75f, 0.00f, 0.75f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 1.00f }
};

const  std::vector<glm::vec4> ColorMap::BLACK_TO_WHITE {
    glm::vec4{ 0.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 1.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMap::WHITE_TO_BLACK {
    glm::vec4{ 0.00f, 1.00f, 1.00f, 1.00f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f }
};

const std::vector<glm::vec4> ColorMap::HSB_HUES {
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.00f },
    glm::vec4{ 0.16f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 0.33f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 0.50f, 0.00f, 1.00f, 1.00f },
    glm::vec4{ 0.66f, 0.00f, 0.00f, 1.00f },
    glm::vec4{ 0.83f, 1.00f, 0.00f, 1.00f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.00f }
};

const std::vector<glm::vec4> ColorMap::INVERSE_HSB_HUES {
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.00f },
    glm::vec4{ 0.16f, 1.00f, 0.00f, 1.00f },
    glm::vec4{ 0.33f, 0.00f, 0.00f, 1.00f },
    glm::vec4{ 0.50f, 0.00f, 1.00f, 1.00f },
    glm::vec4{ 0.66f, 0.00f, 1.00f, 0.00f },
    glm::vec4{ 0.83f, 1.00f, 1.00f, 0.00f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.00f }
};

const std::vector<glm::vec4> ColorMap::DAVINCI {
    glm::vec4{ 0.00f, 0.00f, 0.00f, 0.00f },
    glm::vec4{ 0.10f, 0.18f, 0.08f, 0.00f },
    glm::vec4{ 0.20f, 0.27f, 0.18f, 0.08f },
    glm::vec4{ 0.30f, 0.37f, 0.27f, 0.18f },
    glm::vec4{ 0.40f, 0.47f, 0.37f, 0.27f },
    glm::vec4{ 0.50f, 0.57f, 0.47f, 0.37f },
    glm::vec4{ 0.60f, 0.67f, 0.57f, 0.47f },
    glm::vec4{ 0.70f, 0.76f, 0.67f, 0.57f },
    glm::vec4{ 0.80f, 0.86f, 0.76f, 0.67f },
    glm::vec4{ 0.90f, 1.00f, 0.86f, 0.76f },
    glm::vec4{ 1.00f, 1.00f, 1.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMap::INVERSE_DAVINCI {
    glm::vec4{ 0.00f, 1.00f, 1.00f, 1.00f },
    glm::vec4{ 0.10f, 1.00f, 0.86f, 0.76f },
    glm::vec4{ 0.20f, 0.86f, 0.76f, 0.67f },
    glm::vec4{ 0.30f, 0.76f, 0.67f, 0.57f },
    glm::vec4{ 0.40f, 0.67f, 0.57f, 0.47f },
    glm::vec4{ 0.50f, 0.57f, 0.47f, 0.37f },
    glm::vec4{ 0.60f, 0.47f, 0.37f, 0.27f },
    glm::vec4{ 0.70f, 0.37f, 0.27f, 0.18f },
    glm::vec4{ 0.80f, 0.27f, 0.18f, 0.08f },
    glm::vec4{ 0.90f, 0.18f, 0.08f, 0.00f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 0.00f }
};

const std::vector<glm::vec4> ColorMap::SEISMIC {
    glm::vec4{ 0.00f, 1.00f, 0.00f, 0.00f },
    glm::vec4{ 0.50f, 1.00f, 1.00f, 1.00f },
    glm::vec4{ 1.00f, 0.00f, 0.00f, 1.00f }
};

const std::vector<glm::vec4> ColorMap::INVERSE_SEISMIC{
    glm::vec4{ 0.00f, 0.00f, 0.00f, 1.00f },
    glm::vec4{ 0.50f, 1.00f, 1.00f, 1.00f },
    glm::vec4{ 1.00f, 1.00f, 0.00f, 0.00f }
};

const std::map<std::string, const std::vector<glm::vec4> *> ColorMap::s_mapPtrs{
      { "FULL_RAINBOW",         &FULL_RAINBOW },
      { "INVERSE_FULL_RAINBOW", &INVERSE_FULL_RAINBOW },
      { "RAINBOW",              &RAINBOW },
      { "INVERSE_RAINBOW",      &INVERSE_RAINBOW },
      { "COLD_TO_HOT",          &COLD_TO_HOT },
      { "HOT_TO_COLD",          &HOT_TO_COLD },
      { "BLACK_TO_WHITE",       &BLACK_TO_WHITE },
      { "WHITE_TO_BLACK",       &WHITE_TO_BLACK },
      { "HSB_HUES",             &HSB_HUES },
      { "INVERSE_HSB_HUES",     &INVERSE_HSB_HUES },
      { "DAVINCI",              &DAVINCI },
      { "INVERSE_DAVINCI",      &INVERSE_DAVINCI },
      { "SEISMIC",              &SEISMIC },
      { "INVERSE_SEISMIC",      &INVERSE_SEISMIC }
};

std::map<std::string, const bd::Texture*> ColorMap::s_textures;


const bd::Texture*
ColorMap::load_1dt(const std::string& filename)
{
  bd::Dbg() << "Reading 1dt formatted transfer function file and generating texture.";

  std::ifstream file(filename.c_str(), std::ifstream::in);
  if (!file.is_open()) {
    bd::Err() << "Can't open tfunc file: " << filename;
    return nullptr;
  }

  size_t lineNum{ 0 };
  size_t numKnots{ 0 };

  file >> numKnots; // number of entries/lines in the 1dt file.
  lineNum++;
  if (numKnots > 8192) {
    bd::Err() << "The 1dt transfer function has " << numKnots <<
        " knots but max allowed is 8192)."
            "Skipping loading the transfer function file.";
    return nullptr;
  }

  glm::vec4 * rgba{ new glm::vec4[numKnots] };
// read rest of file consisting of rgba colors
  float r, g, b, a;
  while (lineNum < numKnots && file >> r >> g >> b >> a) {
    rgba[lineNum] = { r, g, b, a };
    lineNum++;
  }

  file.close();
  bd::Texture *transferTex{ new bd::Texture(bd::Texture::Target::Tex1D) };
  unsigned int texId{ transferTex->genGLTex1d(reinterpret_cast<float *>(rgba),
                                              bd::Texture::Format::RGBA,
                                              bd::Texture::Format::RGBA, numKnots)
  };

  if (texId == 0) {
    bd::Err() << "Could not make transfer function texture, returned id was 0.";
    return nullptr;
  }

  delete[] rgba;
}
//  transferTex.textureUnit(1);

//  unsigned int samp{volumeShader.getUniformLocation("tf_sampler")};
//  transferTex.samplerLocation(samp);



/* static */
void
ColorMap::generateDefaultTransferFunctionTextures()
{
  std::vector<glm::vec4> texels;
  texels.resize(4096);
  size_t texelElements{ texels.size() * 4 };

  for(auto it : s_mapPtrs) {
    interpolateTexels(&texels, *(it.second));

    float *textureData{ reinterpret_cast<float*>(texels.data()) };
    bd::Texture *t{ new bd::Texture(bd::Texture::Target::Tex1D) };

    unsigned int name = t->genGLTex1d(textureData,
                                     bd::Texture::Format::RGBA,
                                     bd::Texture::Format::RGBA,
                                     texelElements);

    if (name == 0) {
      Err() << "The texture for colormap " << it.first << " could not be created.";
    } else {
      s_textures[it.first] = t;
      bd::Dbg() << "Added " << it.first << " colormap texture.";
    }

  } // for

  Info() << "Generated " << s_textures.size() << " built-in colormaps.";

}

/* static */
const bd::Texture*
ColorMap::getDefaultMapTexture(const std::string& name)
{
  const bd::Texture *rval{ nullptr };
  try{
    rval = s_textures.at(name);
  } catch (std::out_of_range e) {
    Err() << name << " is not a colormap.";
    throw e;
  }
  return rval;
}

/* static */
void
ColorMap::interpolateTexels(std::vector< glm::vec4 > * texels,
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
      (*texels)[idx++] = glm::vec4{ v[1], v[2], v[3], v[0] }; //use the scalar as the alpha value
      a += stepDelta;
    }

    leftKnot = rightKnot;
    rightKnot = *++knots;
  }
}

glm::vec4
ColorMap::lerp(glm::vec4 x, glm::vec4 y, float a)
{
  return x * (1.0f - a) + y * a;
}

} // namespace subvol