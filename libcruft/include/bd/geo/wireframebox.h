#ifndef bd_wireframebox_h__
#define bd_wireframebox_h__


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

namespace bd {

class WireframeBox {
public:
    static const std::array<glm::vec4, 8> vertices;
    static const std::array<glm::vec3, 8> colors;
    static const std::array<unsigned short, 16> elements;
    static const unsigned int vert_element_size = 4;

    WireframeBox();
    ~WireframeBox();

//    virtual void draw() override;
};

} // namspace bd

#endif // ! bd_wireframebox_h__
