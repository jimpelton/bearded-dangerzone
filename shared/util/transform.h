#ifndef translation_h__
#define translation_h__

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace bd {

class Transform {
public:
    Transform();
    ~Transform();

    void position(glm::vec3 pos);
    
    void rotate(float amt, glm::vec3 axis);
    void rotate(glm::quat rot);

    void scale(glm::vec3 by);

    void lookat(glm::vec3 point, glm::vec3 up);

    glm::mat4 update();
    
    //Transform* parent();

private:
    //Transform *m_parent;
    glm::vec3 m_pos;
    glm::quat m_rot;
    glm::vec3 m_scale;

    glm::vec3 m_oldpos;
    glm::quat m_oldrot;
    glm::vec3 m_oldscale;

    glm::mat4 m_transform;
    


    bool m_isDirty;
};

} // namespace bd
#endif // translation_h__
