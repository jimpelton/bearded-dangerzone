#ifndef bbox_h__
#define bbox_h__

#include <bd/scene/transformable.h>
#include <bd/graphics/drawable.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

namespace bd {

class Box : public Transformable, public IDrawable {
public:
    static const std::array<glm::vec4, 8> vertices;
    static const std::array<glm::vec3, 8> colors;
    static const std::array<unsigned short, 16> elements;

    static const unsigned int vert_element_size = 4;

    Box();
    ~Box();

    virtual void draw() override;
};

} // namspace bd

#endif
