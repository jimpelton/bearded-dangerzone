#ifndef bbox_h__
#define bbox_h__

#include <bd/scene/transformable.h>
#include <bd/graphics/geometry.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

namespace bd {


class Box : public Transformable{
public:
    static const std::array<float, 32> vertices;
    static const std::array<unsigned short, 16> elements;
    static const unsigned int vert_element_size = 4;

    Box();
    ~Box();

    virtual void draw();
};

} // namspace bd

#endif
