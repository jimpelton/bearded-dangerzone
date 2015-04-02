#ifndef quad_h__
#define quad_h__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace bd {
namespace geo {
class Quad {
public:
    static const std::array<glm::vec4, 4> verts;
    static const std::array<unsigned short, 4> elements;
    static const unsigned int vert_element_size = 4;

public:
    Quad() { }

    ~Quad() { }

public:
    const glm::mat4& translate() const
    {
        return m_transform;
    }

    void translate(const glm::mat4 &m)
    {
        m_transform = m;
    }

    const glm::mat4& scale() const
    {
        return m_scale;
    }

    void scale(const glm::mat4 &m)
    {
        m_scale = m;
    }

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
    glm::mat4 m_transform;
    glm::mat4 m_scale;

    glm::vec3 m_color;
    glm::vec2 m_dims;
};
}
} /* namespace */

#endif
