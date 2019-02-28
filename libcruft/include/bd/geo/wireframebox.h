#ifndef bd_wireframebox_h__
#define bd_wireframebox_h__

#include <bd/graphics/drawable.h>
#include <bd/graphics/vertexarrayobject.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

namespace bd {

class WireframeBox : public Drawable {
public:
    static const std::array<glm::vec4, 8> vertices;
    static const std::array<glm::vec3, 8> colors;
    static const std::array<unsigned short, 16> elements;
    static const unsigned int vert_element_size = 4;

    WireframeBox();
    ~WireframeBox();

   void draw() override;

private:
  VertexArrayObject m_vao;
};

} // namspace bd

#endif // ! bd_wireframebox_h__
