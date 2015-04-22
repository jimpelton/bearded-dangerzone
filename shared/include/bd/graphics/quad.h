#ifndef quad_h__
#define quad_h__

#include <bd/scene/transformable.h>
#include <bd/graphics/drawable.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace bd {

class Quad : public Transformable, public IDrawable {
public:
    static const std::array<glm::vec4, 4> verts;
    static const std::array<unsigned short, 4> elements;
    static const std::array<glm::vec3, 4> colors;
    static const unsigned int vert_element_size = 4;

public:
    Quad();
    virtual ~Quad();

    virtual void draw();
};

} /* namespace bd */

#endif
