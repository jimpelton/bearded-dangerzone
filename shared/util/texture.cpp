

#include "texture.h"
#include <log/gl_log.h>


namespace bd {

unsigned int util::genGLTex3d(float* img, GLenum internal_type, GLenum external_type, 
    size_t w, size_t h, size_t d)
{
    //gl_log("Making 3D texture: %ullx%ullx%ull.", w, h, d);
    GLuint texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_3D, texId);

    glTexImage3D(
        GL_TEXTURE_3D,
        0,
        internal_type,
        w, h, d,
        0,
        external_type, 
        GL_FLOAT, 
        img);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);

    glBindTexture(GL_TEXTURE_3D, 0);

    //gl_log("Created 3D texture. Id: %d", texId);
    

    return texId;
}


}