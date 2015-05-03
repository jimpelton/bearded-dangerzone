#ifndef quad_h__
#define quad_h__

#include <bd/scene/transformable.h>
#include <bd/graphics/drawable.h>

#include <glm/glm.hpp>

#include <array>

namespace bd {

class Quad : public Transformable, public IDrawable {
public:
    static const std::array<glm::vec4, 4> verts_xy; ///< initially in xy-plane
    static const std::array<glm::vec4, 4> verts_yz; ///< initially in yz-plane
    static const std::array<glm::vec4, 4> verts_xz; ///< initially in xz-plane

    static const std::array<glm::vec3, 4> texcoords_xy;
    static const std::array<glm::vec3, 4> texcoords_yz;
    static const std::array<glm::vec3, 4> texcoords_xz;

    static const std::array<unsigned short, 4> elements;

    static const std::array<glm::vec3, 4> colors;

    static const unsigned int vert_element_size = 4; ///< number of elements in single vertex

    Quad();
    virtual ~Quad();

    virtual void draw();
};

} // namespace bd

#endif // !quad_h__
