#include <bd/graphics/texture.h>
#include <bd/log/gl_log.h>
#include <bd/log/logger.h>

#include <glm/vec4.hpp>

#include <string>
#include <fstream>

/////////////////////////////////////////////////////////////////////////////////
unsigned int loadTransfer_1dtformat(const std::string &filename, bd::Texture &transferTex,
                                    bd::ShaderProgram &volumeShader) {
  bd::Dbg() << "Reading 1dt formatted transfer function file and generating texture.";

  std::ifstream file(filename.c_str(), std::ifstream::in);
  if (!file.is_open()) {
    bd::Err() << "Can't open tfunc file: " << filename;
    return 0;
  }

  size_t lineNum{ 0 };
  size_t numKnots{ 0 };

  file >> numKnots; // number of entries/lines in the 1dt file.
  lineNum++;
  if (numKnots > 8192) {
    bd::Err() << "The 1dt transfer function has " << numKnots << " knots but max allowed is 8192)."
                   "Skipping loading the transfer function file.";
    return 0;
  }

  glm::vec4 *rgba{ new glm::vec4[numKnots] };
  // read rest of file consisting of rgba colors
  float r, g, b, a;
  while (lineNum < numKnots && file >> r >> g >> b >> a) {
    rgba[lineNum] = {r, g, b, a};
    lineNum++;
  }

  file.close();

  unsigned int texId{
      transferTex.genGLTex1d(reinterpret_cast<float *>(rgba),
                             bd::Texture::Format::RGBA,
                             bd::Texture::Format::RGBA, numKnots) };

  if (texId == 0) {
    bd::Err() << "Could not make transfer function texture, returned id was 0.";
    return texId;
  }

//  transferTex.textureUnit(1);

//  unsigned int samp{volumeShader.getUniformLocation("tf_sampler")};
//  transferTex.samplerLocation(samp);

  delete[] rgba;

  return texId;
}

