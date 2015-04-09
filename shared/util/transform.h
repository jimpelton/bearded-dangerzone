#ifndef translation_h__
#define translation_h__

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace bd {

class Transform {
public:
    Transform();
    ~Transform();

    void translate(glm::vec3 to);
    
    void rotate(glm::vec3 axis, float amt);
    
    void scale(glm::vec3 by);

    void lookat(glm::vec3 point, glm::vec3 up);

    void update();
    
    glm::mat4 doTransform();

    Transform* parent() const { return m_parent; }
    void parent(Transform *p) { m_parent = p; }




private:
    Transform *m_parent;
    glm::vec3 m_pos;
    glm::quat m_rot;
    glm::vec3 m_scale;
    bool m_isDirty;
};

} // namespace bd
#endif // translation_h__
