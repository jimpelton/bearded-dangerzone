

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
    Quad()
        : Quad(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f))
    { }

    Quad(const glm::vec3 &lowerLeft, const glm::vec2 &dims)
        : Quad(lowerLeft, dims, glm::vec3(1.0f, 1.0f, 1.0f))
    { }

    Quad(const glm::vec3 &lowerLeft, const glm::vec2 &dims, const glm::vec3 &color)
        : m_model( glm::translate(glm::mat4(1.0f), 
            lowerLeft + glm::vec3(dims / 2.0f, 0.0f)) * 
            glm::scale(glm::mat4(1.0f), glm::vec3(dims, 1.0f)))
        , m_color(color)
        , m_dims(dims)
    { }

    ~Quad() { }
public:

    glm::mat4& model() { return m_model; }
    void model(glm::mat4& m) { m_model = m; }

    glm::vec3& color() { return m_color; }
    void color(glm::vec3& c) { m_color = c; }

    void lowerLeft(glm::vec3 &pos) {
        m_model = glm::translate(glm::mat4(1.0f),
            pos + glm::vec3(m_dims / 2.0f, 0.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(m_dims, 1.0f));
    }

private:
    glm::mat4 m_model;
    glm::vec3 m_color;
    glm::vec2 m_dims;
};

} } } /* namespace */

#endif