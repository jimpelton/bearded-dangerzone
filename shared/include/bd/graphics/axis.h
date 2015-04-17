#ifndef axis_h__
#define axis_h__


#include <bd/scene/transformable.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace bd {


class Axis : public Transformable {

public:
    static const std::array<glm::vec4, 6> verts;
    static const std::array<glm::vec3, 6> colors;
    static const std::array<unsigned short, 6> elements;
    static const unsigned int vert_element_size = 4;

public:
    Axis();

    Axis(const glm::vec3 &center, const glm::vec3 &dims);

    ~Axis() { }

    const glm::vec3& colorX() const
    {
        return m_colorX;
    }

    const glm::vec3& colorY() const
    {
        return m_colorY;
    }

    const glm::vec3& colorZ() const
    {
        return m_colorZ;
    }

private:
    glm::vec3 m_colorX;
    glm::vec3 m_colorY;
    glm::vec3 m_colorZ;
};

} /* namespace */

#endif /* ifndef axis_h__ */
