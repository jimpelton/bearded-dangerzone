#include "texture.h"

#include <GL/glew.h>

#include <bd/log/gl_log.h>

#include <array>

namespace {

static const std::array<GLenum, 4> texfmt = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
static const std::array<GLenum, 3> textype = { GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D };

template <typename T>
unsigned int ordinal(T t)
{
    return static_cast<unsigned int>(t);
}

}

Texture::Texture()
    : m_id{ 0 }
    , m_samplerUniform{ 0 }
    , m_unit{ 0 }
    , m_type{ Target::Tex3D  }
//    , m_sampler{ Sampler::Volume }
{
}

Texture::~Texture()
{
}

//void Texture::bind()
//{
//    glActiveTexture(GL_TEXTURE0+m_unit);
//    glBindTexture(textype[static_cast<unsigned int>(m_type)], m_id);
//    glUniform1i(m_samplerUniform,  m_unit);
//}

void Texture::bind(unsigned int unit)
{
    glActiveTexture(GL_TEXTURE0+unit);
    glBindTexture(textype[static_cast<unsigned int>(m_type)], m_id);
    glUniform1i(m_samplerUniform, unit);
}

unsigned int Texture::genGLTex1d(float *img, Format ity, Format ety, size_t w)
{
    GLuint texId;
    gl_check(glGenTextures(1, &texId));
    gl_check(glBindTexture(GL_TEXTURE_1D, texId));

    using uint = unsigned int;
    gl_check(glTexImage1D(
        GL_TEXTURE_1D,
        0,
        texfmt.at(static_cast<uint>(ity)),
        w,
        0,
        texfmt.at(static_cast<uint>(ety)),
        GL_FLOAT,
        (void*)img
    ));

    gl_check(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    gl_check(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    gl_check(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));

    m_type = Target::Tex1D;
//    m_sampler = Sampler::Transfer;
    m_id = texId;

    return texId;
}

unsigned int Texture::genGLTex2d(float* img, Format ity, Format ety,
    size_t w, size_t h)
{

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

    m_type = Target::Tex2D;
//    m_sampler = Sampler::Poop;
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
//    m_sampler = Sampler::Volume;
    m_id = texId;

    return texId;
}

