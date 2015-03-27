


#ifndef vertexarrayobject_h__
#define vertexarrayobject_h__

#include "vertexbufferobject.h"

#include <GL/glew.h>

#include <string>

class VertexArrayObject {
public:
    
    VertexArrayObject& create() {
        glGenVertexArrays(1, &m_id);
    }

    VertexArrayObject& attachVbo(VertexBufferObject &vbo) {
        bind();
        
        unbind();
    }
    
    void bind() {
        glBindVertexArray(m_id);
    }
    void unbind() {
        glBindVertexArray(0);
    }

private:
    unsigned int m_id;
    const char *m_name;
};


#endif // vertexarrayobject_h__