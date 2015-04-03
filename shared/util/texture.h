

#ifndef texture_h__
#define texture_h__

#include <GL/glew.h>

namespace bd {

    unsigned int genGLTex3d(float *img, GLenum internal_type, GLenum external_type, 
        size_t w, size_t h, size_t d);

}  // namespace bd

#endif