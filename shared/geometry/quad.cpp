
#include "quad.h"

#include <glm/glm.hpp>

#include <array>

namespace bearded {
namespace dangerzone {
namespace geometry {

const std::array<glm::vec4, 4> Quad::verts
{
    glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f),
    glm::vec4(0.5f, -0.5f, 0.0f, 1.0f),
    glm::vec4(0.5f, 0.5f, 0.0f, 1.0f),
    glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f)
};

const std::array<unsigned short, 4> Quad::elements
{
    0, 1, 2, 3
};

}}}