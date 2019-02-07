#include <bd/geo/quad.h>
#include <bd/log/gl_log.h>

#include <array>
//#include <GL/glew.h>

namespace bd
{
const std::array<glm::vec4, 4> Quad::verts_xy
{
  glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f), // 0 ll
  glm::vec4(1.0f, -1.0f, 0.0f, 1.0f), // 1 lr
  glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f), // 2 ul
  glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) // 3 ur
};

const std::array<glm::vec4, 4> Quad::verts_yz
{
  glm::vec4(0.0f, -1.0f, -1.0f, 1.0f), // 0 ll
  glm::vec4(0.0f, 1.0f, -1.0f, 1.0f), // 1 lr
  glm::vec4(0.0f, -1.0f, 1.0f, 1.0f), // 2 ul
  glm::vec4(0.0f, 1.0f, 1.0f, 1.0f) // 3 ur
};

const std::array<glm::vec4, 4> Quad::verts_xz
{
  glm::vec4(-1.0f, 0.0f, -1.0f, 1.0f), // 0 ll
  glm::vec4(1.0f, 0.0f, -1.0f, 1.0f), // 1 lr
  glm::vec4(-1.0f, 0.0f, 1.0f, 1.0f), // 2 ul
  glm::vec4(1.0f, 0.0f, 1.0f, 1.0f) // 3 ur
};

const std::array<glm::vec3, 4> Quad::texcoords_xy{
  // x-y quad
  glm::vec3(0.0f, 0.0f, 0.0f), // 0 ll
  glm::vec3(1.0f, 0.0f, 0.0f), // 1 lr
  glm::vec3(0.0f, 1.0f, 0.0f), // 2 ul
  glm::vec3(1.0f, 1.0f, 0.0f) // 3 ur
};

const std::array<glm::vec3, 4> Quad::texcoords_xz{
  // x-z quad
  glm::vec3(0.0f, 0.0f, 0.0f), // 0 ll
  glm::vec3(1.0f, 0.0f, 0.0f), // 1 lr
  glm::vec3(0.0f, 0.0f, 1.0f), // 2 ul
  glm::vec3(1.0f, 0.0f, 1.0f) // 3 ur
};

const std::array<glm::vec3, 4> Quad::texcoords_yz{
  // y-z quad
  glm::vec3(0.0f, 0.0f, 0.0f), // 0 ll
  glm::vec3(0.0f, 1.0f, 0.0f), // 1 lr
  glm::vec3(0.0f, 0.0f, 1.0f), // 2 ul
  glm::vec3(0.0f, 1.0f, 1.0f) // 3 ur
};

const std::array<unsigned short, 4> Quad::elements
{
  0, 1, 2, 3 // triangle strip
};

const std::array<glm::vec3, 4> Quad::colors
{
  glm::vec3(0.0f, 0.0f, 0.0f),
  glm::vec3(1.0f, 0.0f, 0.0f),
  glm::vec3(0.0f, 0.0f, 1.0f),
  glm::vec3(0.0f, 1.0f, 0.0f)
};

Quad::Quad()
{
}

Quad::~Quad()
{
}

//void
//Quad::draw()
//{
//  gl_check(glDrawElements(GL_TRIANGLE_STRIP,
//    static_cast<GLsizei>(elements.size()),
//    GL_UNSIGNED_SHORT,
//    nullptr));
//}
} // namespace bd


