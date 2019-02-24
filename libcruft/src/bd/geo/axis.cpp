
#include <GL/glew.h>

#include <bd/geo/axis.h>
#include <bd/log/gl_log.h>

namespace bd
{
const std::array<glm::vec4, 6> CoordinateAxis::verts
{
  glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // x
  glm::vec4(0.5f, 0.0f, 0.0f, 1.0f),

  glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // y
  glm::vec4(0.0f, 0.5f, 0.0f, 1.0f),

  glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), //z
  glm::vec4(0.0f, 0.0f, 0.5f, 1.0f)
};

const std::array<glm::vec3, 6> CoordinateAxis::colors
{
  glm::vec3(1.0f, 0.0f, 0.0f), // x: red
  glm::vec3(1.0f, 0.0f, 0.0f),

  glm::vec3(0.0f, 1.0f, 0.0f), // y: green
  glm::vec3(0.0f, 1.0f, 0.0f),

  glm::vec3(0.0f, 0.0f, 1.0f), // z: blue
  glm::vec3(0.0f, 0.0f, 1.0f)
};

const std::array<unsigned short, 6> CoordinateAxis::elements
{
  0, 1, // x
  0, 2, // y
  0, 3 // z
};

CoordinateAxis::CoordinateAxis()
{
  m_vao.create();

  m_vao.addVbo((float *) ( verts.data()),
             verts.size()*vert_element_size,
             vert_element_size,
             POSITION,
             bd::VertexArrayObject::Usage::Static_Draw); // attr 0

  // vertex colors into attribute 1
  m_vao.addVbo((float *) ( colors.data()),
             colors.size() * 3,
             3, // 3 floats per color
             COLOR,
             bd::VertexArrayObject::Usage::Static_Draw); // attr 1
}



void
CoordinateAxis::draw()
{
  m_vao.bind();
  gl_check(glDrawArrays(GL_LINES,
                        0,
                        static_cast<GLsizei>(bd::CoordinateAxis::verts.size())));
  m_vao.unbind();
}


} /* namespace  bd */


