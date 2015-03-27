
#include "axis.h"

namespace bearded { namespace dangerzone { namespace geometry {

const std::array<glm::vec4, 4> Axis::verts
{
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
};

const std::array<unsigned short, 6> Axis::elements
{
    0, 1,
    0, 2,
    0, 3
};


} } } /* namespace */