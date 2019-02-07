
#include <bd/geo/wireframebox.h>
#include <bd/log/gl_log.h>

#include <array>

namespace bd
{
const std::array<glm::vec4, 8> WireframeBox::vertices{
  glm::vec4(-0.5, -0.5, -0.5, 1.0),
  glm::vec4(0.5, -0.5, -0.5, 1.0),
  glm::vec4(0.5, 0.5, -0.5, 1.0),
  glm::vec4(-0.5, 0.5, -0.5, 1.0),
  glm::vec4(-0.5, -0.5, 0.5, 1.0),
  glm::vec4(0.5, -0.5, 0.5, 1.0),
  glm::vec4(0.5, 0.5, 0.5, 1.0),
  glm::vec4(-0.5, 0.5, 0.5, 1.0)
};

const std::array<unsigned short, 16> WireframeBox::elements{
  0, 1, 2, 3,
  4, 5, 6, 7,
  0, 4, 1, 5, 2, 6, 3, 7
};

const std::array<glm::vec3, 8> WireframeBox::colors{
  glm::vec3(0.5, 0.5, 0.5),
  glm::vec3(0.5, 0.5, 0.5),
  glm::vec3(0.5, 0.5, 0.5),
  glm::vec3(0.5, 0.5, 0.5),
  glm::vec3(0.5, 0.5, 0.5),
  glm::vec3(0.5, 0.5, 0.5),
  glm::vec3(0.5, 0.5, 0.5),
  glm::vec3(0.5, 0.5, 0.5)
};


//////////////////////////////////////////////////////////////////////////
WireframeBox::WireframeBox()
{
}


///////////////////////////////////////////////////////////////////////////////
WireframeBox::~WireframeBox()
{
}


///////////////////////////////////////////////////////////////////////////////
//void
//WireframeBox::draw()
//{
//  // draw front face
//  gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0));
//  // draw back face
//  gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort))));
//  // connect front and back faces
//  gl_check(glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort))));
//}
} // namespace bd


