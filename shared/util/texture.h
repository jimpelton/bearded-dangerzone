

#ifndef texture_h__
#define texture_h__

#include <file/DataTypes.h>

namespace bd {
; // <-- stop vs from indenting (so redonc!)

class Texture
{
public:

    enum class Type
    {
        Tex1D, Tex3D
    };

    enum class Format
    {
        OneChannel
    };


    ///////////////////////////////////////////////////////////////////////////////
    // Constructors/Destructor
    ///////////////////////////////////////////////////////////////////////////////
    Texture();
    virtual ~Texture();


    ///////////////////////////////////////////////////////////////////////////////
    // Interface 
    ///////////////////////////////////////////////////////////////////////////////
    unsigned int genGLTex1d();
    
    unsigned int genGLTex3d(float *img, Format internal, Format external, 
        size_t w, size_t h, size_t d);

    unsigned int id() const { return m_id; }
    
    Type type() const { return m_type; }


private:
    ///////////////////////////////////////////////////////////////////////////////
    // Data members
    ///////////////////////////////////////////////////////////////////////////////
    unsigned int m_id;
    Type m_type;
};


}  // namespace bd

#endif