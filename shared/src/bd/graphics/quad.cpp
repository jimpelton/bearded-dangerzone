#include <bd/graphics/quad.h>

#include <glm/glm.hpp>

#include <array>
#include <bd/log/gl_log.h>
#include <GL/glew.h>

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
    0, 1, 3, 2      // triangle strip maybe?
    //0, 1, 2, 3    // line strip maybe?
};


const std::array<glm::vec3, 4> Quad::colors
{
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f )
};

Quad::Quad() : Transformable()
{
}

Quad::~Quad()
{
}

void Quad::draw()
{
    gl_check(glDrawElements(GL_TRIANGLE_STRIP, elements.size(), GL_UNSIGNED_SHORT, 0));
}

} // namespace bd

