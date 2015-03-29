

#ifndef quad_h__
#define quad_h__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace bearded { namespace dangerzone { namespace geometry { 

class Quad {
public:
    static const std::array<glm::vec4, 4> verts;
    static const std::array<unsigned short, 4> elements;
    static const unsigned int vert_element_size = 4;

public:
    Quad() { }

     ~Quad() { }

public:
    glm::mat4& translate() { return m_transform; }
    void translate(glm::mat4& m) { m_transform = m; }

    glm::mat4& scale() { return m_scale; }
    void scale(glm::mat4& m) { m_scale = m; }

    glm::vec3& color() { return m_color; }
    void color(glm::vec3& c) { m_color = c; }

private:
    glm::mat4 m_transform;
    glm::mat4 m_scale;
    
    glm::vec3 m_color;
    glm::vec2 m_dims;
};

} } } /* namespace */

#endif