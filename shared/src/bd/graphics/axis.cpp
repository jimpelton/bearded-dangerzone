
#include <bd/log/gl_log.h>
#include <bd/graphics/axis.h>
#include <GL/glew.h>

namespace bd {

const std::array<glm::vec4, 6> Axis::verts
{
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),  // x
    glm::vec4(0.5f, 0.0f, 0.0f, 1.0f),

    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),  // y
    glm::vec4(0.0f, 0.5f, 0.0f, 1.0f),
    
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),  //z
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
    0, 1,  // x
    0, 2,  // y
    0, 3   // z
};

Axis::Axis()
    : Transformable()
{
}

void Axis::draw()
{
    gl_check(glDrawArrays(GL_LINES, 0, verts.size()));
}

} /* namespace  bd */
