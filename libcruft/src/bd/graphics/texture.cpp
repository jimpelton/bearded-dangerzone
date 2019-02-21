#include <bd/graphics/texture.h>
#include <bd/util/gl_strings.h>
#include <bd/util/ordinal.h>
#include <bd/log/gl_log.h>

#include <GL/glew.h>

#include <array>
#include <sstream>

namespace bd
{

namespace
{
static const std::array<GLenum, 8> gl_format{
    GL_RED, GL_R8, GL_R8UI, GL_R16, GL_R32F, GL_RG, GL_RGB, GL_RGBA
};

static const std::array<GLenum, 3> gl_target{
    GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D
};


GLenum
pixelType(DataType dt)
{
  GLenum rval;
  switch (dt) {
    case DataType::Integer:
      rval = GL_INT;
      break;
    case DataType::UnsignedInteger:
      rval = GL_UNSIGNED_INT;
      break;
    case DataType::Character:
      rval = GL_BYTE;
      break;
    case DataType::UnsignedCharacter:
      rval = GL_UNSIGNED_BYTE;
      break;
    case DataType::Short:
      rval = GL_SHORT;
      break;
    case DataType::UnsignedShort:
      rval = GL_UNSIGNED_SHORT;
      break;
    case DataType::Float:
    case DataType::Unknown:
    default:
      rval = GL_FLOAT;
      break;
  }

  return rval;
}


} // namespace


void
Texture::GenTextures3d(int num,
                       DataType t,
                       Format internal,
                       Format external,
                       int w, int h, int d,
                       std::vector<Texture *> *v)
{
  v->clear();
  v->resize(num);

  GLuint * texs{ new GLuint[num] };
  gl_check(glGenTextures(num, texs));

  // allocate on the gpu and set Texture's GL id.
  for(int i{ 0 }; i < num; ++i) {
    GLuint id{ texs[i] };

    gl_check(glBindTexture(GL_TEXTURE_3D, id));
    gl_check(glTexStorage3D(GL_TEXTURE_3D, 1,
                                gl_format[bd::ordinal(internal)],
                                w, h, d));

//    gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
//    gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
//    gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
//    gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
//    gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    Texture *tex{ new Texture{ Target::Tex3D }};
    tex->m_id = id;
    tex->m_dims = {w, h, d};
    tex->m_dType = t;
    tex->m_internal = internal;
    tex->m_external = external;
    (*v)[i] = tex;
  }

  delete[] texs;
}


////////////////////////////////////////////////////////////////////////////////
Texture::Texture(Target textureType)
    : m_id{ 0 }
    , m_target{ textureType }
{
}


////////////////////////////////////////////////////////////////////////////////
Texture::~Texture()
{
}


////////////////////////////////////////////////////////////////////////////////
void
Texture::bind() const
{
  glBindTexture(gl_target[bd::ordinal<Target>(m_target)], m_id);
}


////////////////////////////////////////////////////////////////////////////////
void
Texture::bind(unsigned int unit) const
{
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(gl_target[ordinal<Target>(m_target)], m_id);
  //  glUniform1i(m_samplerUniform, unit);
}


////////////////////////////////////////////////////////////////////////////////
unsigned int
Texture::genGLTex1d(float const *img, Format ity, Format ety, size_t w)
{
  GLuint texId{ 0 };
  gl_check(glGenTextures(1, &texId));
  gl_check(glBindTexture(GL_TEXTURE_1D, texId));

  gl_check(glTexImage1D(
      GL_TEXTURE_1D,
      0,
      gl_format[ordinal<Format>(ity)],
      w,
      0,
      gl_format[ordinal<Format>(ety)],
      GL_FLOAT,
      static_cast<void const *>(img)
  ));

  gl_check(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  gl_check(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  gl_check(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP));

  m_target = Target::Tex1D;
  m_id = texId;
  m_internal = ity;
  m_external = ety;

  return texId;
}


////////////////////////////////////////////////////////////////////////////////
unsigned int
Texture::genGLTex2d(float *img, Format ity, Format ety, size_t w, size_t h)
{
  GLuint texId{ 0 };
  gl_check(glGenTextures(1, &texId));
  gl_check(glBindTexture(GL_TEXTURE_2D, texId));

  gl_check(glTexImage2D(
      GL_TEXTURE_2D,
      0,
      gl_format[ordinal<Format>(ity)],
      w, h,
      0,
      gl_format[ordinal<Format>(ety)],
      GL_FLOAT,
      img));

  gl_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  gl_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  gl_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
  gl_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));

  gl_check(glBindTexture(GL_TEXTURE_2D, 0));

  m_target = Target::Tex2D;
  m_id = texId;

  m_internal = ity;
  m_external = ety;

  return texId;
}


////////////////////////////////////////////////////////////////////////////////
unsigned int
Texture::genGLTex3d(Format ity, Format ety, size_t w, size_t h, size_t d,
                    DataType ty, void *pixelData)
{
  const GLint border{ 0 };
  const GLint mipMapLevel{ 0 };

  GLuint texId{ 0 };
  gl_check(glGenTextures(1, &texId));
  gl_check(glBindTexture(GL_TEXTURE_3D, texId));

  gl_check(glTexImage3D(
      GL_TEXTURE_3D,
      mipMapLevel,                         // mip-map detail level
      gl_format[ordinal<Format>(ity)],     // internal format (how texture is to be stored by GL)
      w, h, d,                             // dimensions
      border,                              // border
      gl_format[ordinal<Format>(ety)],     // pixel data format (R, RG, etc.)
      pixelType(ty),                       // pixel data type
      pixelData));                         // ptr to data

  gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
  gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

  gl_check(glBindTexture(GL_TEXTURE_3D, 0));

  m_target = Target::Tex3D;
  m_id = texId;
  m_internal = ity;
  m_external = ety;

  return texId;
}

////////////////////////////////////////////////////////////////////////////////
void
Texture::subImage1D(Texture::Format external,
                    int xoff,
                    int w,
                    DataType type,
                    void const *pixelData) const
{
  assert(false && "not implemented");
}


////////////////////////////////////////////////////////////////////////////////
void
Texture::subImage2D(Texture::Format external,
                    int xoff,
                    int yoff,
                    int w,
                    int h,
                    DataType type,
                    void const *pixelData) const
{
  assert(false && "not implemented");
}


////////////////////////////////////////////////////////////////////////////////
void
Texture::subImage3D(int xoff, int yoff, int zoff,
                    int w, int h, int d,
                    void const *pixelData) const
{

//  const GLint border{ 0 };
//  const GLint mipMapLevel{ 0 };

  gl_check(glBindTexture(GL_TEXTURE_3D, m_id));

  gl_check(glTexSubImage3D(GL_TEXTURE_3D, 0,
                      xoff, yoff, zoff, w, h, d,
                      gl_format[ordinal(m_external)],
                      pixelType(m_dType),
                      pixelData));

//  gl_check(glTexImage3D(
//      GL_TEXTURE_3D,
//      mipMapLevel,                         // mip-map detail level
//      gl_format[ordinal<Format>(m_internal)],  // internal format (how texture is to be stored by GL)
//      w, h, d,                             // dimensions
//      border,                              // border
//      gl_format[ordinal<Format>(m_external)],     // pixel data format (R, RG, etc.)
//      pixelType(m_dType),                       // pixel data type
//      pixelData));                         // ptr to data

  gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
  gl_check(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

  gl_check(glBindTexture(GL_TEXTURE_3D, 0));



}


///////////////////////////////////////////////////////////////////////////////
void
Texture::subImage3D(void const *pixelData) const
{
  subImage3D(0, 0, 0, m_dims.x, m_dims.y, m_dims.z, pixelData);
}


////////////////////////////////////////////////////////////////////////////////
std::string
Texture::to_string() const
{
  std::stringstream ss;
  GLenum tt;
  tt = gl_target[bd::ordinal<Target>(m_target)];

  ss << "{ Id: " << m_id << ", Type: " << bd::gl_to_string(tt);
  if (m_id != 0) {
    ss << "\nGL values:";
    GLint val{ 0 };
    gl_check(glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_WIDTH, &val));
    ss << "\n\t" << bd::gl_to_string(GL_TEXTURE_WIDTH) << ": " << val;
    gl_check(glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_HEIGHT, &val));
    ss << "\n\t" << bd::gl_to_string(GL_TEXTURE_HEIGHT) << ": " << val;
    gl_check(glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_DEPTH, &val));
    ss << "\n\t" << bd::gl_to_string(GL_TEXTURE_DEPTH) << ": " << val;
    gl_check(glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_INTERNAL_FORMAT, &val));
    ss << "\n\t" << bd::gl_to_string(GL_TEXTURE_INTERNAL_FORMAT) << ": "
       << bd::gl_to_string(val);
    gl_check(glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_RED_TYPE, &val));
    ss << "\n\t" << bd::gl_to_string(GL_TEXTURE_RED_TYPE) << ": " << val;
    gl_check(glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_BLUE_TYPE, &val));
    ss << "\n\t" << bd::gl_to_string(GL_TEXTURE_BLUE_TYPE) << ": " << val;
    gl_check(glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_GREEN_TYPE, &val));
    ss << "\n\t" << bd::gl_to_string(GL_TEXTURE_GREEN_TYPE) << ": " << val;
    gl_check(glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_ALPHA_TYPE, &val));
    ss << "\n\t" << bd::gl_to_string(GL_TEXTURE_ALPHA_TYPE) << ": " << val;
    gl_check(glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_RED_SIZE, &val));
    ss << "\n\t" << bd::gl_to_string(GL_TEXTURE_RED_SIZE) << ": " << val;
    gl_check(glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_BLUE_SIZE, &val));
    ss << "\n\t" << bd::gl_to_string(GL_TEXTURE_BLUE_SIZE) << ": " << val;
    gl_check(glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_GREEN_SIZE, &val));
    ss << "\n\t" << bd::gl_to_string(GL_TEXTURE_GREEN_SIZE) << ": " << val;
    gl_check(glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_ALPHA_SIZE, &val));
    ss << "\n\t" << bd::gl_to_string(GL_TEXTURE_ALPHA_SIZE) << ": " << val;
    gl_check(glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_COMPRESSED, &val));
    ss << "\n\t" << bd::gl_to_string(GL_TEXTURE_COMPRESSED) << ": " << val;
    gl_check(glGetTextureLevelParameteriv(m_id,
                                          0,
                                          GL_TEXTURE_COMPRESSED_IMAGE_SIZE,
                                          &val));
    ss << "\n\t" << bd::gl_to_string(GL_TEXTURE_COMPRESSED_IMAGE_SIZE) << ": " << val;

  }
  ss << " }";
  return ss.str();
}



std::ostream &
operator<<(std::ostream &os, const bd::Texture &t)
{
  return os << t.to_string();
}


} // namespace bd


