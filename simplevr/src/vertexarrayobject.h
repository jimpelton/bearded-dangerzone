


#ifndef vertexarrayobject_h__
#define vertexarrayobject_h__

#include "vertexbufferobject.h"

#include <log/gl_log.h>
#include <GL/glew.h>

#include <string>
#include <vector>

class VertexArrayObject {
public:
    
    VertexArrayObject& create() {
        glGenVertexArrays(1, &m_id);
    }



    void createVbo(std::vector<float> data) 
    {
        bind();
        GLuint vbo = 0;
        gl_check(glGenBuffers(1, &vbo));
        gl_check(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        gl_check(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(decltype(data[0])),
            data.data(), GL_STATIC_DRAW));


    }
    
    void bind() {
        glBindVertexArray(m_id);
    }
    void unbind() {
        glBindVertexArray(0);
    }

private:
    std::vector<VertexBufferObject> m_vbos;
    unsigned int m_id;
    const char *m_name;
};


#endif // vertexarrayobject_h__