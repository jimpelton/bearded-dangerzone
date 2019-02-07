#include <GL/glew.h>

#include <bd/graphics/vertexarrayobject.h>
#include <bd/log/gl_log.h>
#include <bd/log/logger.h>
#include <bd/util/ordinal.h>

namespace bd
{


namespace
{
  static const std::array<GLenum, 3> gl_usage{
    GL_STREAM_DRAW,
    GL_STATIC_DRAW,
    GL_DYNAMIC_DRAW
  };
}

///////////////////////////////////////////////////////////////////////////////
VertexArrayObject::VertexArrayObject()
  : m_bufIds{},
    m_idxBufId{0},
    m_id{0},
    m_numEle{0}
{
}

///////////////////////////////////////////////////////////////////////////////
VertexArrayObject::~VertexArrayObject()
{
  Dbg() << "Deleting " << m_bufIds.size() << " vertex buffers.";

  for (unsigned int u : m_bufIds) {
    glDeleteBuffers(1, &u);
  }
}

///////////////////////////////////////////////////////////////////////////////
unsigned int
VertexArrayObject::create()
{
  if (m_id == 0) {
    gl_check(glGenVertexArrays(1, &m_id));
    Dbg() << "Created vertex array object id=" << m_id;
  }

  return m_id;
}

///////////////////////////////////////////////////////////////////////////////
unsigned int
VertexArrayObject::addVbo(const float *verts, 
                          size_t length,
                          unsigned elements_per_vertex, 
                          unsigned attr_idx, 
                          Usage usage)
{
  unsigned int vboId{0};

  vboId = gen_vbo(verts, length, elements_per_vertex, attr_idx, usage);

  return vboId;
}

///////////////////////////////////////////////////////////////////////////////
unsigned int
VertexArrayObject::addVbo(const std::vector<float> &verts,
                          unsigned int elements_per_vertex, 
                          unsigned int attr_idx, 
                          Usage usage)
{
  unsigned int vboId{0};

  vboId = gen_vbo(verts.data(), verts.size(), elements_per_vertex, attr_idx, usage);

  return vboId;
}

///////////////////////////////////////////////////////////////////////////////
unsigned int
VertexArrayObject::addVbo(const std::vector<glm::vec3> &verts, 
                          unsigned int attr_idx,
                          Usage usage)
{
  const int elements_per_vertex{3};
  unsigned int vboId{0};

  vboId = gen_vbo(reinterpret_cast<const float *>(verts.data()),
                  verts.size() * elements_per_vertex, elements_per_vertex,
                  attr_idx, usage);

  return vboId;
}

///////////////////////////////////////////////////////////////////////////////
unsigned int
VertexArrayObject::addVbo(const std::vector<glm::vec4> &verts,
                          unsigned int attr_idx, 
                          Usage usage)
{
  const int elements_per_vertex{4};
  unsigned int vboId{0};

  vboId = gen_vbo(reinterpret_cast<const float *>(verts.data()),
                  verts.size() * elements_per_vertex, elements_per_vertex,
                  attr_idx, usage);

  return vboId;
}

///////////////////////////////////////////////////////////////////////////////
unsigned int
VertexArrayObject::setIndexBuffer(std::vector<unsigned short> const &indices,
                                  Usage u)
{
  unsigned int iboId{0};

  if (m_idxBufId != 0) {
    return m_idxBufId;
  }

  iboId = gen_ibo(indices.data(), indices.size(), u);

  return iboId;
}

///////////////////////////////////////////////////////////////////////////////
unsigned int
VertexArrayObject::setIndexBuffer(unsigned short const *indices,
                                  unsigned int length,
                                  Usage u)
{
  unsigned int iboId{0};

  if (m_idxBufId != 0) {
    return m_idxBufId;
  }

  iboId = gen_ibo(indices, length, u);

  return iboId;
}

///////////////////////////////////////////////////////////////////////////////
void
VertexArrayObject::bind() const
{
  gl_check(glBindVertexArray(m_id));
}

///////////////////////////////////////////////////////////////////////////////
void
VertexArrayObject::unbind() const
{
  gl_check(glBindVertexArray(0));
}

///////////////////////////////////////////////////////////////////////////////
unsigned int
VertexArrayObject::numElements() const
{
  return m_numEle;
}


///////////////////////////////////////////////////////////////////////////////
//VertexArrayObject::Method VertexArrayObject::method() const {
//  return m_method;
//}

///////////////////////////////////////////////////////////////////////////////
// Private Members
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
unsigned int
VertexArrayObject::gen_vbo(const float *verts,
                           size_t length,
                           unsigned elements_per_vertex,
                           unsigned attr_idx,
                           Usage usage_type)
{
  const unsigned int stride_between_verts{0};
  const void *offset{nullptr};

  unsigned int vbo{0};

  //  checkEqualVertexCount(length, elements_per_vertex);

  create();
  bind();

  gl_check(glGenBuffers(1, &vbo));
  gl_check(glBindBuffer(GL_ARRAY_BUFFER, vbo));
  gl_check(glBufferData(GL_ARRAY_BUFFER,
                        length * sizeof(float),
                        verts,
                        gl_usage[ordinal<Usage>(usage_type)]) );

  gl_check(glEnableVertexAttribArray(attr_idx));
  gl_check(glVertexAttribPointer(attr_idx,
                                 elements_per_vertex,
                                 GL_FLOAT,
                                 GL_FALSE,
                                 stride_between_verts,
                                 offset));

  //unbind();

  if (vbo == 0) {
    Err() << "Unable to add vertex buffer to vertex buffer object."
      "(The returned id for the generated vertex buffer was 0)";
  }
  else {
    m_bufIds.push_back(vbo);
    Dbg() << "Created VBO, id=" << vbo << ", verticies=" << length / elements_per_vertex;
  }

  return vbo;
}

///////////////////////////////////////////////////////////////////////////////
unsigned int
VertexArrayObject::gen_ibo(unsigned short const *indices,
                           size_t length,
                           Usage usage)
{
  unsigned int ibo{0};

  create();
  bind();

  gl_check(glGenBuffers(1, &ibo));
  gl_check(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
  gl_check(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        length * sizeof(unsigned short),
                        indices,
                        gl_usage[ordinal<Usage>(usage)] ));

  unbind();

  if (ibo == 0) {
    Err() << "Unable to set index buffer. (The returned id for the generated "
      "element buffer was 0)";
  }
  else {
    m_idxBufId = ibo;
    m_numEle = length;
    Dbg() << "Created IBO, id=" << ibo << " elements=" << length;
  }

  return ibo;
}
} // namespace bd


