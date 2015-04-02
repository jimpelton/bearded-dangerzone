#ifndef vertexbufferobject_h__
#define vertexbufferobject_h__

#include <GL/glew.h>

#include <string>

class VertexBufferObject 
{
    static int unnamed_counter;
public:
    VertexBufferObject() 
        : m_id{ 0 }
        , m_name{ std::string("noname_") + std::to_string(unnamed_counter++) }
    {
        
    }

    ~VertexBufferObject() {

    }

    void bufferData(float *data) {
        
    }

private:
    unsigned int m_id;
    std::string m_name;
};

int VertexBufferObject::unnamed_counter = 0;

#endif // vertexbufferobject_h__
