#include <bd/graphics/axis.h>

namespace bd {

const std::array<glm::vec4, 6> Axis::verts
{
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 
    glm::vec4(0.5f, 0.0f, 0.0f, 1.0f),

    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.5f, 0.0f, 1.0f),
    
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 0.5f, 1.0f)
};

const std::array<glm::vec3, 6> Axis::colors
{
    glm::vec3(1.0f, 0.0f, 0.0f),  // x: red
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),  // y: green
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),  // z: blue
    glm::vec3(0.0f, 0.0f, 1.0f)
};

const std::array<unsigned short, 6> Axis::elements
{
    0, 1,
    0, 2,
    0, 3
};

} /* namespace  bd */
