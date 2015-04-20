//
// Created by jim on 4/19/15.
//

#ifndef geometry_h__
#define geometry_h__

namespace bd {

class Geometry
{
public:
    Geometry() { }
    virtual ~Geometry() { }

    virtual const float *vertexData() = 0;
    virtual const unsigned short *elementData() = 0;
};

} // namespace bd




#endif // !geometry_h
