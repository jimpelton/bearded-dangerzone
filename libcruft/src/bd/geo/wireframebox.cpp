#include <GL/glew.h>

#include <bd/geo/wireframebox.h>
#include <bd/log/gl_log.h>

#include <array>
#include <bd/graphics/vertexarrayobject.h>

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
  m_vao.create();

  m_vao.addVbo((float *) vertices.data(),
      vertices.size() * vert_element_size,
      vert_element_size,
      POSITION,
      VertexArrayObject::Usage::Static_Draw);

  m_vao.addVbo((float *) colors.data(),
      colors.size() * 3,
      3,
      COLOR,
      VertexArrayObject::Usage::Static_Draw);

  m_vao.setIndexBuffer(( unsigned short *) elements.data(),
      elements.size(),
      VertexArrayObject::Usage::Static_Draw);

}


///////////////////////////////////////////////////////////////////////////////
WireframeBox::~WireframeBox()
{
}


///////////////////////////////////////////////////////////////////////////////
void
WireframeBox::draw()
{
  m_vao.bind();
  // draw front face
  gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0));
  // draw back face
  gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort))));
  // connect front and back faces
  gl_check(glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort))));
  m_vao.unbind();
}
} // namespace bd


