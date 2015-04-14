


#ifndef vertexarrayobject_h__
#define vertexarrayobject_h__

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <array>
#include <utility>

namespace bd {

class VertexArrayObject {
public:
    enum class BufType : int
    {
        Vertex=0, Index=1
    };


    ///////////////////////////////////////////////////////////////////////////////
    // Ctor/Dtor
    ///////////////////////////////////////////////////////////////////////////////
    VertexArrayObject();

    ~VertexArrayObject();


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
    /// \brief Add a vbo with the vertices in \c verts
    ///
    /// \note Subsequent calls to addIndexedVbo() return the previous gl id.
    /// 
    /// \returns The gl id of the vertex buffer object created.
    ///////////////////////////////////////////////////////////////////////////////
    unsigned int addVbo(const std::vector<float> &verts, unsigned int elements_per_vertex,
        unsigned int attr_idx);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Add a vbo with the vertices in \c verts
    ///
    /// \note Subsequent calls to addIndexedVbo() return the previous gl id.
    /// 
    /// \returns The gl id of the vertex buffer object created.
    ///////////////////////////////////////////////////////////////////////////////
    unsigned int addVbo(const std::vector<glm::vec3> &verts, unsigned int attr_idx);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Add a vbo with the vertices in \c verts
    ///
    /// \note Subsequent calls to addIndexedVbo() return the previous gl id.
    /// 
    /// \returns The gl id of the vertex buffer object created.
    ///////////////////////////////////////////////////////////////////////////////
    unsigned int addVbo(const std::vector<glm::vec4> &verts, unsigned int attr_idx);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Add an index buffer to be bound with this VAO.
    ///
    /// \note Subsequent calls to setIndexBuffer() have no effect and return the 
    ///  id of the index buffer.
    /// 
    /// \returns The gl id of the index buffer created.
    ///////////////////////////////////////////////////////////////////////////////
    unsigned int setIndexBuffer(const std::vector<unsigned short> &indices);

    void bind();

    void unbind();

private:
    ///////////////////////////////////////////////////////////////////////////////
    // Private Members
    ///////////////////////////////////////////////////////////////////////////////
   
    unsigned int gen_vbo(const float *verts, size_t length, unsigned int elements_per_vertex, 
        unsigned int attr_idx);

    unsigned int gen_ibo(const unsigned short *indices, size_t length);


    ///////////////////////////////////////////////////////////////////////////////
    // Private Data
    ///////////////////////////////////////////////////////////////////////////////
    
    std::vector<unsigned int> m_bufIds; //< gl ids of vert buffers
    unsigned int m_idxBufId; //< gl id of the index buffer
    unsigned int m_id; //< gl id of the VAO rep'd by this instance.

    std::string m_name;
};

} // namespace bd
#endif // vertexarrayobject_h__