#ifndef quad_h__
#define quad_h__

#include <bd/scene/transformable.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace bd {

class Quad : public Transformable {
public:
    static const std::array<glm::vec4, 4> verts;
    static const std::array<unsigned short, 4> elements;
    static const std::array<glm::vec3, 4> colors;
    static const unsigned int vert_element_size = 4;

public:
    Quad();

    virtual ~Quad();

public:

    const glm::vec3& cColor() const
    {
        return m_color;
    }

    void color(const glm::vec3 &c)
    {
        m_color = c;
    }

    glm::vec3& color()
    {
        return m_color;
    }

private:
    //glm::mat4 m_transform;
    //glm::mat4 m_scale;

    glm::vec3 m_color;
    glm::vec2 m_dims;
};

} /* namespace bd */

#endif
