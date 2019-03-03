#include <bd/geo/mesh.h>
#include <GL/glew.h>

namespace bd
{

Mesh::Mesh() {}

Mesh::Mesh(const std::vector<glm::vec3> &vertices, const std::vector<unsigned short> &indices)
    : m_verts_count{ vertices.size() }
    , m_indices_count{ indices.size() }
    , m_vao{ }
{
  m_vao.create();
  m_vao.addVbo(vertices, POSITION, bd::VertexArrayObject::Usage::Static_Draw);
  m_vao.setIndexBuffer(indices, bd::VertexArrayObject::Usage::Static_Draw);
}

Mesh::~Mesh()
{
}

void
Mesh::init()
{
}




void
Mesh::draw()
{
  m_vao.bind();
  glDrawElements(GL_TRIANGLES, m_indices_count, GL_UNSIGNED_SHORT, nullptr);
  m_vao.unbind();
}




} /* namepsace bd */


