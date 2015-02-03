

#include "BBox.h"

#include <array>

namespace tst {


    const float BBox::vertices[] = {
        -0.5, -0.5, -0.5, 1.0,
        0.5, -0.5, -0.5, 1.0,
        0.5, 0.5, -0.5, 1.0,
        -0.5, 0.5, -0.5, 1.0,
        -0.5, -0.5, 0.5, 1.0,
        0.5, -0.5, 0.5, 1.0,
        0.5, 0.5, 0.5, 1.0,
        -0.5, 0.5, 0.5, 1.0,
    };

    const unsigned short BBox::elements[] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    BBox::BBox()
        : min_x(0.0f)
        , max_x(0.0f)
        , min_y(0.0f)
        , max_y(0.0f)
        , min_z(0.0f)
        , max_z(0.0f)
        , m_vaoId(0)
        , m_vboId(0)
        , m_iboId(0)
    { }

    BBox::BBox(const float *minmax)
        : min_x(minmax[0])
        , max_x(minmax[1])
        , min_y(minmax[2])
        , max_y(minmax[3])
        , min_z(minmax[4])
        , max_z(minmax[5])
        , m_vaoId(0)
        , m_vboId(0)
        , m_iboId(0)
    { }

    BBox::~BBox()
    { }

    void BBox::init()
    {
        glGenVertexArrays(1, &m_vaoId);
        glBindVertexArray(m_vaoId);

        glGenBuffers(1, &m_vboId);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        const unsigned vertex_coord_attr = 0;
        glEnableVertexAttribArray(vertex_coord_attr);
        glVertexAttribPointer(
            vertex_coord_attr,  // attribute
            4,                  // number of elements per vertex, here (x,y,z,w)
            GL_FLOAT,           // the type of each element
            GL_FALSE,           // take our values as-is
            0,                  // no extra data between each position
            0                   // offset of first element
            );
        
        //glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &m_iboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
        glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
        glm::mat4 transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);
        m_transform = transform;
    }

}

//void makeBuffers(GLuint *vboId, GLuint *iboId)
//{
//
//
//    // calculate bounding size
//    GLfloat min_x, max_x,
//        min_y, max_y,
//        min_z, max_z;
//
//    min_x = max_x = mesh->vertices[0].x;
//    min_y = max_y = mesh->vertices[0].y;
//    min_z = max_z = mesh->vertices[0].z;
//    for (int i = 0; i < mesh->vertices.size(); i++) {
//        if (mesh->vertices[i].x < min_x) min_x = mesh->vertices[i].x;
//        if (mesh->vertices[i].x > max_x) max_x = mesh->vertices[i].x;
//        if (mesh->vertices[i].y < min_y) min_y = mesh->vertices[i].y;
//        if (mesh->vertices[i].y > max_y) max_y = mesh->vertices[i].y;
//        if (mesh->vertices[i].z < min_z) min_z = mesh->vertices[i].z;
//        if (mesh->vertices[i].z > max_z) max_z = mesh->vertices[i].z;
//    }
//
//
//}