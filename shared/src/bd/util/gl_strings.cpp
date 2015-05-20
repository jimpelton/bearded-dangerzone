
#include <bd/util/gl_strings.h>

namespace bd {

const char* gl_to_string(GLenum e)
{
    const char* ret = nullptr;
    switch (e) 
    {
    case GL_TEXTURE_1D:                    ret = "GL_TEXTURE_1D";                    break;
    case GL_TEXTURE_2D:                    ret = "GL_TEXTURE_2D";                    break;
    case GL_TEXTURE_3D:                    ret = "GL_TEXTURE_3D";                    break;
    case GL_TEXTURE_WIDTH:                 ret = "GL_TEXTURE_WIDTH";                 break;
    case GL_TEXTURE_HEIGHT:                ret = "GL_TEXTURE_HEIGHT";                break;
    case GL_TEXTURE_RED_SIZE:              ret = "GL_TEXTURE_RED_SIZE";              break;
    case GL_TEXTURE_BLUE_SIZE:             ret = "GL_TEXTURE_BLUE_SIZE";             break;
    case GL_TEXTURE_GREEN_SIZE:            ret = "GL_TEXTURE_GREEN_SIZE";            break;
    case GL_TEXTURE_ALPHA_SIZE:            ret = "GL_TEXTURE_ALPHA_SIZE";            break;
    case GL_TEXTURE_COMPRESSED:            ret = "GL_TEXTURE_COMPRESSED";            break;
    case GL_TEXTURE_COMPRESSED_IMAGE_SIZE: ret = "GL_TEXTURE_COMPRESSED_IMAGE_SIZE"; break;
    default:                               ret = "unknown";                          break;
    }

    return ret;
}

} // namespace bd


