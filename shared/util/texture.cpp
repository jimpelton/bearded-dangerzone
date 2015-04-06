

#include "texture.h"
#include <log/gl_log.h>
#include <array>


namespace bd {
; // <-- stop visual studio indenting

Texture::Texture() 
    : m_id{ 0 }
    , m_type{  }
{
}

Texture::~Texture()
{
}

unsigned int Texture::genGLTex1d()
{
    m_type = Type::Tex1D;
    return 0;
}

unsigned int Texture::genGLTex3d(float* img, Format ity,
    Format ety, size_t w, size_t h, size_t d)
{
    static const std::array<GLenum, 1> tf = { GL_R };

    GLuint texId = 0;
    gl_check(glGenTextures(1, &texId));
    gl_check(glBindTexture(GL_TEXTURE_3D, texId));

    gl_check(glTexImage3D(
        GL_TEXTURE_3D,
        0,
        tf.at(static_cast<int>(ity)),
        w, h, d,
        0,
        tf.at(static_cast<int>(ety)), 
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