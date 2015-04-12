#include <bd/graphics/quad.h>

#include <glm/glm.hpp>

#include <array>

namespace bd {

const std::array<glm::vec4, 4> Quad::verts
{
    glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f), // 0 ll
    glm::vec4(0.5f, -0.5f, 0.0f, 1.0f), // 1 lr
    glm::vec4(0.5f, 0.5f, 0.0f, 1.0f), // 2 ur
    glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f) // 3 ul
};

const std::array<unsigned short, 4> Quad::elements
{
    0, 1, 3, 2
    //0, 1, 2, 3
};

Quad::Quad() : Transformable()
{
}



} // namespace bd

