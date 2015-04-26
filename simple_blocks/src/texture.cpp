#include "texture.h"

#include <GL/glew.h>

#include <bd/log/gl_log.h>

#include <array>

namespace {

static const std::array<GLenum, 4> texfmt = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
static const std::array<GLenum, 3> textype = { GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D };
}

Texture::Texture()
    : m_id{ 0 }
    , m_samplerUniform{ 0 }
    , m_unit{ 0 }
    , m_type{  }
{
}

Texture::~Texture()
{
}

void Texture::bind()
{
    glActiveTexture(GL_TEXTURE0+m_unit);
//    glBindTexture(textype[static_cast<unsigned int>(m_type)], m_id);
    glBindTexture(GL_TEXTURE_3D, m_id);
    glUniform1i(m_samplerUniform,  static_cast<int>(Sampler::Volume));
}

unsigned int Texture::genGLTex1d(float *img, Format ity, Format ety, size_t w)
{
    m_type = Target::Tex1D;
    return 0;
}

unsigned int Texture::genGLTex2d(float* img, Format ity, Format ety,
    size_t w, size_t h)
{
    m_type = Target::Tex2D;

    GLuint texId = 0;
    gl_check(glGenTextures(1, &texId));
    gl_check(glBindTexture(GL_TEXTURE_2D, texId));

    using uint = unsigned int;
    gl_check(glTexImage2D(
        GL_TEXTURE_2D,
        0,
        texfmt.at(static_cast<uint>(ity)),
        w, h,
        0,
        texfmt.at(static_cast<uint>(ety)),
        GL_FLOAT,
        img));

    gl_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    gl_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    gl_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
    gl_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));

    gl_check(glBindTexture(GL_TEXTURE_2D, 0));

    m_id = texId;

    return texId;
}

unsigned int Texture::genGLTex3d(float* img, Format ity,
    Format ety, size_t w, size_t h, size_t d)
{
    GLuint texId = 0;
    gl_check(glGenTextures(1, &texId));
    gl_check(glBindTexture(GL_TEXTURE_3D, texId));

    using uint = unsigned int;
    gl_check(glTexImage3D(
        GL_TEXTURE_3D,
        0,
        texfmt.at(static_cast<uint>(ity)),
        w, h, d,
        0,
        texfmt.at(static_cast<uint>(ety)),
        GL_FLOAT,
        img));

    gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP));
    gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP));
    gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP));

    gl_check(glBindTexture(GL_TEXTURE_3D, 0));

    m_type = Target::Tex3D;
    m_id = texId;

    return texId;
}

