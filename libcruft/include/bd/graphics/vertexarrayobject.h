#ifndef vertexarrayobject_h__
#define vertexarrayobject_h__

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <array>
#include <utility>

namespace bd
{
class VertexArrayObject
{
public:

  /// \brief Buffer usage specifiers.
  enum class Usage: int
  {
    Stream_Draw,
    Static_Draw,
    Dynamic_Draw
  };

  ///////////////////////////////////////////////////////////////////////////////
  // Ctor/Dtor
  ///////////////////////////////////////////////////////////////////////////////
  VertexArrayObject();

  virtual ~VertexArrayObject();


  ///////////////////////////////////////////////////////////////////////////////
  // Methods
  ///////////////////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////////////////
  /// \brief If this VAO hasn't been created by opengl yet, create it.
  ///
  /// \note Subsequent calls to create() return the previous gl id.
  ///
  /// \returns The gl id of the vao created
  ///////////////////////////////////////////////////////////////////////////////
  unsigned int create();

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Add a vbo with \c length vertices supplied in \c verts
  ///
  /// \note Subsequent calls to addIndexedVbo() return the previous gl id.
  ///
  /// \returns The gl id of the vertex buffer object created.
  ///////////////////////////////////////////////////////////////////////////////
  unsigned int addVbo(float const *verts,
                      size_t length,
                      unsigned int elements_per_vertex,
                      unsigned int attr_idx,
                      Usage usage);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Add a vbo with the vertices in \c verts
  ///
  /// \note Subsequent calls to addIndexedVbo() return the previous gl id.
  ///
  /// \returns The gl id of the vertex buffer object created.
  ///////////////////////////////////////////////////////////////////////////////
  unsigned int addVbo(const std::vector<float> &verts,
                      unsigned int elements_per_vertex,
                      unsigned int attr_idx, Usage usage);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Add a vbo with the vertices in \c verts
  ///
  /// \note Subsequent calls to addIndexedVbo() return the previous gl id.
  ///
  /// \returns The gl id of the vertex buffer object created.
  ///////////////////////////////////////////////////////////////////////////////
  unsigned int addVbo(const std::vector<glm::vec3> &verts,
                      unsigned int attr_idx, Usage usage);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Add a vbo with the vertices in \c verts
  ///
  /// \note Subsequent calls to addIndexedVbo() return the previous gl id.
  ///
  /// \returns The gl id of the vertex buffer object created.
  ///////////////////////////////////////////////////////////////////////////////
  unsigned int addVbo(std::vector<glm::vec4> const &verts,
                      unsigned int attr_idx, Usage usage);

  /// \brief Add an index buffer to be bound with this VAO with \c length indices.
  ///
  /// \note Subsequent calls to setIndexBuffer() have no effect and return the
  ///  id of the index buffer.
  ///
  /// \returns The gl id of the index buffer created.
  unsigned int setIndexBuffer(unsigned short const *indices, unsigned int length, Usage u);

  /// \brief Add an index buffer to be bound with this VAO.
  ///
  /// \note Subsequent calls to setIndexBuffer() have no effect and return the
  ///  id of the index buffer.
  ///
  /// \returns The gl id of the index buffer created.
  unsigned int setIndexBuffer(std::vector<unsigned short> const &indices, Usage u);

  //////////////////////////////////////////////////////////////////////////
  /// \brief Number of elements in index array.
  //////////////////////////////////////////////////////////////////////////
  unsigned int numElements() const;

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Bind this VertexArrayObject to the context.
  ///////////////////////////////////////////////////////////////////////////////
  void bind() const;

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Unbind this VAO from the context.
  ///////////////////////////////////////////////////////////////////////////////
  void unbind() const;

private:
  ///////////////////////////////////////////////////////////////////////////////
  // Private Members
  ///////////////////////////////////////////////////////////////////////////////

  // vertex buffer object helper
  unsigned int gen_vbo(float const *verts,
                       size_t length,
                       unsigned int elements_per_vertex,
                       unsigned int attr_idx, Usage usage);

  // index buffer object helper
  unsigned int gen_ibo(unsigned short const *indices, size_t length, Usage usage);


  ///////////////////////////////////////////////////////////////////////////////
  // Private Data
  ///////////////////////////////////////////////////////////////////////////////

  std::vector<unsigned int> m_bufIds; //< gl ids of vert buffers
  unsigned int m_idxBufId; //< gl id of the index buffer
  unsigned int m_id; //< gl id of the VAO rep'd by this instance.
  unsigned int m_numEle; //< Number of elements in index array (= #of verts)
};
} // namespace bd

#endif // !vertexarrayobject_h__


