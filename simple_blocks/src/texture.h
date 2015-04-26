#ifndef texture_h__
#define texture_h__

#include <cstdlib>
#include <bd/graphics/shader.h>


class Texture
{
public:

    enum class Target : int
    {
        Tex1D, Tex2D, Tex3D
    };

    enum class Format : int
    {
        RED, RG, RGB, RGBA
    };

    enum class Sampler : int
    {
        Volume, Transfer
    };

    ///////////////////////////////////////////////////////////////////////////////
    // Constructors/Destructor
    ///////////////////////////////////////////////////////////////////////////////
    Texture();
    virtual ~Texture();


    ///////////////////////////////////////////////////////////////////////////////
    // Interface 
    ///////////////////////////////////////////////////////////////////////////////

    void bind();

    unsigned int genGLTex1d(float *img, Format ity, Format ety, size_t w);
    
    unsigned int genGLTex2d(float* img, Format ity,
    Format ety, size_t w, size_t h);

    unsigned int genGLTex3d(float *img, Format internal, Format external, 
        size_t w, size_t h, size_t d);

    unsigned int id() const { return m_id; }

    unsigned int samplerLocation() const { return m_samplerUniform; }
    void samplerLocation(unsigned int loc) { m_samplerUniform = loc; }

    unsigned int textureUnit() const { return m_unit; }
    void textureUnit(unsigned int unit) { m_unit = unit; }

    Target target() const { return m_type; }


private:
    ///////////////////////////////////////////////////////////////////////////////
    // Data members
    ///////////////////////////////////////////////////////////////////////////////
    unsigned int m_id;   ///< OpenGL id of the texture data.
    unsigned int m_samplerUniform;  ///< OpenGL id of the texture sampler to use.
    unsigned int m_unit; ///< Texture sampling unit.
    Target m_type;         ///< the gl target to bind to.
};



#endif
