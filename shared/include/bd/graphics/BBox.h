#ifndef bbox_h__
#define bbox_h__

#include <bd/scene/transformable.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

namespace bd {


class Box : public Transformable {
public:
    static const std::array<float, 32> vertices;
    static const std::array<unsigned short, 16> elements;
    static const unsigned int vert_element_size = 4;


    Box();


    //////////////////////////////////////////////////////////////////////////////
    ///  \brief create a bbox with the given mins/maxes.
    ///  \param minmax minX, maxX, minY, maxY, minZ, maxZ
    //////////////////////////////////////////////////////////////////////////////
    Box(const std::array<float, 6> &minmax);


    virtual  ~Box();

};

} // namspace bd

#endif
