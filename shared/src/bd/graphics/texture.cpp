
#include <GL/glew.h>

#include <bd/util/texture.h>
#include <bd/log/gl_log.h>

#include <array>

namespace {

static const std::array<GLenum, 4> texfmt = { GL_R, GL_RG, GL_RGB, GL_RGBA };

}

namespace bd {


Texture::Texture() 
    : m_id{ 0 }
    , m_type{  }
{
}

Texture::~Texture()
{
}

unsigned int Texture::genGLTex1d(float *img, Format ity, Format ety, size_t w)
{
    m_type = Type::Tex1D;
    return 0;
}

unsigned int Texture::genGLTex2d(float* img, Format ity,
    Format ety, size_t w, size_t h)
{
    m_type = Type::Tex2D;

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

    m_type = Type::Tex3D;

    return texId;
}

} // namespace bd
