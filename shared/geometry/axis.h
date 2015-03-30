#ifndef axis_h__
#define axis_h__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace bd {
    namespace geometry {
        ; // <-- stop VS from indenting (so rediculous!)

        class Axis
        {
        public:
            static const std::array<glm::vec4, 4> verts;
            static const std::array<unsigned short, 6> elements;
            static const unsigned int vert_element_size = 4;

        public:
            Axis()
                : Axis(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f))
            { }

            Axis(const glm::vec3 &center, const glm::vec3 &dims)
                : m_model{ glm::translate(glm::mat4(1.0f), center) * glm::scale(glm::mat4(1.0f), dims) }
                , m_colorX{ glm::vec3(1, 0, 0) }
                , m_colorY{ glm::vec3(0, 1, 0) }
                , m_colorZ{ glm::vec3(0, 0, 1) }
            { }

            ~Axis() { }

            const glm::mat4& model() const { return m_model; }
            const glm::vec3& colorX() const { return m_colorX; }
            const glm::vec3& colorY() const { return m_colorY; }
            const glm::vec3& colorZ() const { return m_colorZ; }

        private:
            glm::mat4 m_model;
            glm::vec3 m_colorX;
            glm::vec3 m_colorY;
            glm::vec3 m_colorZ;
        };
    }
} /* namespace */

#endif /* ifndef axis_h__ */