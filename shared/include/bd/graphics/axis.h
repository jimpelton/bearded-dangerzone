#ifndef axis_h__
#define axis_h__


#include <bd/scene/transformable.h>
#include <bd/graphics/drawable.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace bd {

class Axis : public Transformable, public IDrawable
{
public:
    static const std::array<glm::vec4, 6> verts;
    static const std::array<glm::vec3, 6> colors;
    static const std::array<unsigned short, 6> elements;
    static const unsigned int vert_element_size = 4;

    Axis();
//    Axis(const glm::vec3 &center, const glm::vec3 &dims);
    ~Axis() { }

    virtual void draw() override;

};
} // namespace bd

#endif // !axis_h__
