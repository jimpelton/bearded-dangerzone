

#include "geometry.h"

#include <bd/log/gl_log.h>
#include <bd/graphics/axis.h>
#include <bd/graphics/quad.h>

void makeBlockSlices(int num_slices, std::vector<glm::vec4> &vertices,
                std::vector<unsigned short> &indices)
{
    using bd::Quad;
    unsigned int n = num_slices;
    size_t vertexCount, indexCount;

//    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> colors;
//    std::vector<unsigned short> indices;

    if (n == 0 || n == 1) {
        n = 1;
    } else if (n%2 != 0) {
        n += 1;
    }

    vertexCount = n * Quad::verts.size();
    indexCount = n + n * Quad::verts.size();

    vertices.clear();
    vertices.reserve(vertexCount);
    indices.clear();
    indices.reserve(indexCount);

    for (size_t i = 0; i < vertexCount; i+=4) {
        vertices.push_back(Quad::verts[0]);
        colors.push_back(Quad::colors[0]);

        vertices.push_back(Quad::verts[1]);
        colors.push_back(Quad::colors[1]);

        vertices.push_back(Quad::verts[2]);
        colors.push_back(Quad::colors[2]);

        vertices.push_back(Quad::verts[3]);
        colors.push_back(Quad::colors[3]);
    }

    for (size_t i = 0; i < indexCount; i+=5) {
        indices.push_back(Quad::elements[0]);
        indices.push_back(Quad::elements[1]);
        indices.push_back(Quad::elements[2]);
        indices.push_back(Quad::elements[3]);
        indices.push_back(static_cast<unsigned short>(0xFFFF));
    }
//
//    m_slices_vao.addVbo(vertices, 0);
//    m_slices_vao.addVbo(colors, 1);
//    m_slices_vao.setIndexBuffer(indices);

    gl_log("Created %d slices, %d vertices, %d indices.",
           n, vertexCount, indexCount);
}

