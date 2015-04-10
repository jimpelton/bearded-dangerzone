#include <util/transform.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace bd {
;

const glm::mat4 IDENTITY{ 1.0f };

Transform::Transform()
    : m_pos{ 0.0f }
    , m_rot{ }
    , m_scale{ 1.0f }
    , m_isDirty{ false }
{
}

Transform::Transform(Transform *parent)
    : m_pos{ 0.0f }
    , m_rot{ }
    , m_scale{ 1.0f }
    , m_isDirty{ false }
{
}

Transform::~Transform()
{

}

void Transform::position(glm::vec3 pos)
{
    m_pos = pos;
}

void Transform::rotate(float amt, glm::vec3 axis)
{
    glm::quat q{ amt, axis };
    rotate(q);
}

void Transform::rotate(glm::quat rot)
{
    m_rot = rot;
}

void Transform::scale(glm::vec3 s)
{
    m_scale = s;
}

void Transform::lookat(glm::vec3 point, glm::vec3 up)
{

}

glm::mat4 Transform::update()
{
    if (m_isDirty) {
        glm::mat4 s = glm::scale(IDENTITY, m_scale);
        glm::mat4 r = glm::toMat4(m_rot);
        glm::mat4 t = glm::translate(IDENTITY, m_pos);

        m_transform = t * r * s;
    }
    return m_transform;
}

Transform* Transform::parent() 
{
    return m_parent;
}

}