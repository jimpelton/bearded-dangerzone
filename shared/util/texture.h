

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

public:
    Texture();
    virtual ~Texture();

public:
    unsigned int genGLTex1d();
    unsigned int genGLTex3d(float *img, Format internal, 
        Format external, size_t w, size_t h, size_t d);

    unsigned int location() const { return m_location; }
    Type type() const { return m_type; }


private:
    unsigned int m_location;
    Type m_type;

};


}  // namespace bd

#endif