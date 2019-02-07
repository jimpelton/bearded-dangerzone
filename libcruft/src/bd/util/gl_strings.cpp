#include <bd/util/gl_strings.h>

namespace bd
{
const char*
gl_to_string(GLenum e)
{
  const char* ret = nullptr;
  switch (e) {
  case GL_NONE: ret = "GL_NONE";
    break;
  case GL_SIGNED_NORMALIZED: ret = "GL_SIGNED_NORMALIZED";
    break;
  case GL_UNSIGNED_NORMALIZED: ret = "GL_UNSIGNED_NORMALIZED";
    break;
  case GL_FLOAT: ret = "GL_FLOAT";
    break;
  case GL_INT: ret = "GL_INT";
    break;
  case GL_UNSIGNED_INT: ret = "GL_UNSIGNED_INT";
    break;
  case GL_TEXTURE_1D: ret = "GL_TEXTURE_1D";
    break;
  case GL_TEXTURE_2D: ret = "GL_TEXTURE_2D";
    break;
  case GL_TEXTURE_3D: ret = "GL_TEXTURE_3D";
    break;
  case GL_TEXTURE_WIDTH: ret = "GL_TEXTURE_WIDTH";
    break;
  case GL_TEXTURE_HEIGHT: ret = "GL_TEXTURE_HEIGHT";
    break;
  case GL_TEXTURE_DEPTH: ret = "GL_TEXTURE_DEPTH";
    break;
  case GL_TEXTURE_INTERNAL_FORMAT: ret = "GL_TEXTURE_INTERNAL_FORMAT";
    break;
  case GL_TEXTURE_RED_TYPE: ret = "GL_TEXTURE_RED_TYPE";
    break;
  case GL_TEXTURE_BLUE_TYPE: ret = "GL_TEXTURE_BLUE_TYPE";
    break;
  case GL_TEXTURE_GREEN_TYPE: ret = "GL_TEXTURE_GREEN_TYPE";
    break;
  case GL_TEXTURE_ALPHA_TYPE: ret = "GL_TEXTURE_ALPHA_TYPE";
    break;
  case GL_TEXTURE_RED_SIZE: ret = "GL_TEXTURE_RED_SIZE";
    break;
  case GL_TEXTURE_BLUE_SIZE: ret = "GL_TEXTURE_BLUE_SIZE";
    break;
  case GL_TEXTURE_GREEN_SIZE: ret = "GL_TEXTURE_GREEN_SIZE";
    break;
  case GL_TEXTURE_ALPHA_SIZE: ret = "GL_TEXTURE_ALPHA_SIZE";
    break;
  case GL_TEXTURE_COMPRESSED: ret = "GL_TEXTURE_COMPRESSED";
    break;
  case GL_TEXTURE_COMPRESSED_IMAGE_SIZE: ret = "GL_TEXTURE_COMPRESSED_IMAGE_SIZE";
    break;
  case GL_TEXTURE_BUFFER_SIZE: ret = "GL_TEXTURE_BUFFER_SIZE";
    break;
  case GL_VERTEX_SHADER: ret = "GL_VERTEX_SHADER";
    break;
  case GL_FRAGMENT_SHADER: ret = "GL_FRAGMENT_SHADER";
    break;
  default: ret = "unknown";
    break;
  }

  return ret;
}
} // namespace bd


