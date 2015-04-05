

#include "geometry.h"

#include <graphics/axis.h>
#include <graphics/quad.h>

#include <GL/glew.h>

void initQuadVbos(unsigned int vaoId, unsigned int *q_vboId, unsigned int *q_iboId)
{
    using bd::Quad;
    glBindVertexArray(vaoId);

    glGenBuffers(1, q_vboId);
    glBindBuffer(GL_ARRAY_BUFFER, *q_vboId);
    glBufferData(GL_ARRAY_BUFFER,
        Quad::verts.size() * sizeof(decltype(Quad::verts[0])),
        Quad::verts.data(),
        GL_STATIC_DRAW);
    const unsigned vertex_coord_attr = 0;
    glEnableVertexAttribArray(vertex_coord_attr);
    glVertexAttribPointer(vertex_coord_attr, // attribute
        Quad::vert_element_size, // number of elements per vertex, here (x,y,z,w)
        GL_FLOAT, // the type of each element
        GL_FALSE, // take our values as-is
        0, // no extra data between each position
        0 // offset of first element
        );

    glGenBuffers(1, q_iboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *q_iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        Quad::elements.size() * sizeof(decltype(Quad::elements[0])),
        Quad::elements.data(),
        GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void initAxisVbos(unsigned int vaoId, unsigned int axis_vboId[2]) 
{
    using bd::Axis;
    glBindVertexArray(vaoId);

    glGenBuffers(2, axis_vboId);
    glBindBuffer(GL_ARRAY_BUFFER, axis_vboId[0]);
    glBufferData(GL_ARRAY_BUFFER,
        Axis::verts.size() * sizeof(decltype(Axis::verts[0])),
        Axis::verts.data(),
        GL_STATIC_DRAW);
    const unsigned vertex_coord_attr = 0;
    glEnableVertexAttribArray(vertex_coord_attr);
    glVertexAttribPointer(vertex_coord_attr, // attribute
        Axis::vert_element_size, // number of elements per vertex, here (x,y,z,w)
        GL_FLOAT, // the type of each element
        GL_FALSE, // take our values as-is
        0, // no extra data between each position
        0); // offset of first element


    glBindBuffer(GL_ARRAY_BUFFER, axis_vboId[1]);
    glBufferData(GL_ARRAY_BUFFER,
        Axis::colors.size() * sizeof(decltype(Axis::colors[0])),
        Axis::colors.data(),
        GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
}

