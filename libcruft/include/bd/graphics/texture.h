#ifndef bd_texture_h
#define bd_texture_h

#include <bd/graphics/shader.h>
#include <bd/io/datatypes.h>

#include <glm/glm.hpp>

#include <string>
#include <cstdlib>
#include <vector>

namespace bd
{
///////////////////////////////////////////////////////////////////////////////
/// \brief  Wraps up GL texture creation and binds it when asked.
/// \note If Texture is wrapping a 3D texture, then the texture unit
///   \c GL_TEXTURE0+static_cast<unsigned>(Sampler::Volume) is activated.
///
/// \note If a 1D texture is wrapped then the texture unit bound is
///   \c GL_TEXTURE0+static_cast<unsigned>(Sampler::Transfer) is activated.
///////////////////////////////////////////////////////////////////////////////
class Texture
{
public:

  enum class Target
      : unsigned int
  {
    Tex1D,
    Tex2D,
    Tex3D
  };

  enum class Format
      : unsigned int
  {
    RED,
    R8,
    R8UI,
    R16,
    R32F,
    RG,
    RGB,
    RGBA
  };


  /// \brief Generate \c n textures, allocate OpenGL storage on GPU and place them into the
  /// given vector.
  ///
  /// \note The vector will be cleared of data.
  /// \param v storage for the textures
  /// \param n number of textures to generate
  static void
  GenTextures3d(int num,
                DataType t,
                Format internal,
                Format external,
                int w, int h, int d,
                std::vector<Texture *> *v);


  Texture(Target textureType);


  virtual ~Texture();


  /// \brief Binds this texture to the current active texture.
  void
  bind() const;


  /// \brief Activates texture unit \c unit and binds this texture.
  void
  bind(unsigned int unit) const;


  unsigned int
  genGLTex1d(float const *img, Format internal, Format external, size_t w);


  unsigned int
  genGLTex2d(float *img, Format internal, Format external, size_t w, size_t h);


  unsigned int
  genGLTex3d(Format internal, Format external, size_t w, size_t h, size_t d,
             DataType ty, void *pixelData);


  void
  subImage1D(Format external,
             int xoff, int w,
             DataType type, void const *pixelData) const;


  void
  subImage2D(Format external,
             int xoff, int yoff, int w, int h,
             DataType type, void const *pixelData) const;


  void
  subImage3D(int xoff, int yoff, int zoff, int w, int h, int d,
             void const *pixelData) const;


  void
  subImage3D(void const *pixelData) const;


  unsigned int
  id() const
  {
    return m_id;
  }


  Target
  target() const
  {
    return m_target;
  }

  DataType
  dataType() const
  {
    return m_dType;
  }


  std::string
  to_string() const;


private:
  unsigned int m_id; ///< OpenGL handle for the texture.
  Target m_target;     ///< the gl target (ex, GL_TEXTURE_2D) to bind to.
  Format m_external;
  Format m_internal;
  DataType m_dType;
  glm::u32vec3 m_dims;

};


std::ostream &
operator<<(std::ostream &os, const bd::Texture &t);
} // namespace bd


#endif // ! texture_h__


