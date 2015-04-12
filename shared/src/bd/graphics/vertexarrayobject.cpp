#include <bd/graphics/vertexarrayobject.h>
#include <bd/log/gl_log.h>

#include <utility>

namespace bd
{


///////////////////////////////////////////////////////////////////////////////
VertexArrayObject::VertexArrayObject()
    : m_bufIds{ } 
    , m_idxBufId{ 0 } 
    , m_id{ 0 }
    , m_name{  }
{
}


///////////////////////////////////////////////////////////////////////////////
VertexArrayObject::~VertexArrayObject()
{
    // TODO: gl delete stuffs.
    gl_log_err("You're leaking memory (but where am I?!?)!");
}


///////////////////////////////////////////////////////////////////////////////
unsigned int VertexArrayObject::create() 
{
    if (m_id == 0)
        gl_check(glGenVertexArrays(1, &m_id));

    return m_id;
}


///////////////////////////////////////////////////////////////////////////////
unsigned int VertexArrayObject::addVbo(const std::vector<float> &verts, 
    unsigned elements_per_vertex, unsigned attr_idx) 
{
    unsigned int vboId{ 0 };

    vboId = gen_vbo(verts.data(), verts.size(), elements_per_vertex, attr_idx);
    
    return vboId;
}


///////////////////////////////////////////////////////////////////////////////
unsigned int VertexArrayObject::addVbo(const std::vector<glm::vec3> &verts, 
    unsigned int attr_idx)
{
    const int elements_per_vertex{ 3 };
    unsigned int vboId{ 0 };

    vboId = gen_vbo(reinterpret_cast<const float*>(verts.data()),
        verts.size()*elements_per_vertex, elements_per_vertex, attr_idx);

    return vboId;
}


///////////////////////////////////////////////////////////////////////////////
unsigned int VertexArrayObject::addVbo(const std::vector<glm::vec4> &verts, 
    unsigned int attr_idx)
{
    const int elements_per_vertex{ 4 };
    unsigned int vboId{ 0 };
    
    vboId = gen_vbo(reinterpret_cast<const float*>(verts.data()), 
        verts.size()*elements_per_vertex, elements_per_vertex, attr_idx);
 
    return vboId;
}


///////////////////////////////////////////////////////////////////////////////
unsigned int VertexArrayObject::setIndexBuffer(const std::vector<unsigned short> &indices)
{
    unsigned int iboId{ 0 };

    if (m_idxBufId != 0) 
        return m_idxBufId;

    iboId = gen_ibo(indices.data(), indices.size());

    return iboId;
}


///////////////////////////////////////////////////////////////////////////////
void VertexArrayObject::bind() 
{
    gl_check(glBindVertexArray(m_id));
}


///////////////////////////////////////////////////////////////////////////////
void VertexArrayObject::unbind() 
{
    gl_check(glBindVertexArray(0));
}


///////////////////////////////////////////////////////////////////////////////
// Private Members
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
unsigned int VertexArrayObject::gen_vbo(const float *verts, size_t length,
    unsigned elements_per_vertex, unsigned attr_idx) 
{
    const unsigned int stride_between_verts{ 0 };
    const void * offset{ nullptr };

    unsigned int vbo{ 0 };
    
    create();
    bind();

    gl_check(glGenBuffers(1, &vbo));
    gl_check(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    gl_check(glBufferData(GL_ARRAY_BUFFER, 
        length * sizeof(float),
        verts, 
        GL_STATIC_DRAW));

    gl_check(glEnableVertexAttribArray(attr_idx));
    gl_check(glVertexAttribPointer(attr_idx, 
        elements_per_vertex, 
        GL_FLOAT, 
        GL_FALSE, 
        stride_between_verts, 
        offset));

    unbind();

    if (vbo == 0) {
        gl_log_err("Unable to add vertex buffer to vertex buffer object."
            "(The returned id for the vertex buffer was 0)");
    } else {
        m_bufIds.push_back(vbo);
    }

    return vbo;
}


///////////////////////////////////////////////////////////////////////////////
unsigned int VertexArrayObject::gen_ibo(const unsigned short *indices, size_t length)
{
    unsigned int ibo{ 0 };

    create();
    bind();

    gl_check(glGenBuffers(1, &ibo));
    gl_check(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    gl_check(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        length * sizeof(unsigned short),
        indices,
        GL_STATIC_DRAW));

    unbind();

    if (ibo == 0) {
        gl_log_err("Unable to set index buffer. (The returned id was 0)");
    } else {
        m_idxBufId = ibo;
    }

    return ibo;
}



} // namespace bd
