#include <bd/graphics/BBox.h>

#include <array>
#include <GL/glew.h>

namespace bd {

const std::array<float, 32> Box::vertices {
    -0.5, -0.5, -0.5, 1.0,
    0.5, -0.5, -0.5, 1.0,
    0.5, 0.5, -0.5, 1.0,
    -0.5, 0.5, -0.5, 1.0,
    -0.5, -0.5, 0.5, 1.0,
    0.5, -0.5, 0.5, 1.0,
    0.5, 0.5, 0.5, 1.0,
    -0.5, 0.5, 0.5, 1.0,
};

const std::array<unsigned short, 16> Box::elements {
    0, 1, 2, 3,
    4, 5, 6, 7,
    0, 4, 1, 5, 2, 6, 3, 7
};

Box::Box() {}
Box::~Box() { }

void Box::draw()
{
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
}


} // namespace bd

