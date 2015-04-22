#include <bd/scene/transform.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>



namespace bd {


const glm::mat4 I4x4{ 1.0f };

//////////////////////////////////////////////////////////////////////////////
Transform::Transform()
    : m_pos{ 0.0f }
    , m_rot{ }
    , m_scale{ 1.0f }
    , m_transform{ I4x4 }
    , m_isDirty{ false }
{
}


//////////////////////////////////////////////////////////////////////////////
Transform::~Transform()
{
}


//////////////////////////////////////////////////////////////////////////////
void Transform::origin(glm::vec3 pos)
{
    m_pos = pos;
    m_isDirty = true;
}


///////////////////////////////////////////////////////////////////////////////
void Transform::translate(glm::vec3 amt)
{
    m_pos += amt;
    m_isDirty = true;
}


//////////////////////////////////////////////////////////////////////////////
void Transform::orientation(glm::quat o)
{
    m_rot = o;
    m_isDirty = true;
}


//////////////////////////////////////////////////////////////////////////////
void Transform::rotate(float amt, glm::vec3 axis)
{
    glm::quat q{ amt, axis };
    rotate(q);
}


//////////////////////////////////////////////////////////////////////////////
void Transform::rotate(glm::quat amt)
{
    m_rot *= amt;
    m_isDirty = true;
}


//////////////////////////////////////////////////////////////////////////////
void Transform::scale(glm::vec3 s)
{
    m_scale = s;
    m_isDirty = true;
}


//////////////////////////////////////////////////////////////////////////////
void Transform::lookat(glm::vec3 up, glm::vec3 point)
{
    glm::quat q{ up, point };
    m_rot = q;
    m_isDirty = true;
}


//////////////////////////////////////////////////////////////////////////////
glm::mat4 Transform::update(const glm::mat4 &p)
{
    if (m_isDirty) {
        glm::mat4 s = glm::scale(p, m_scale);
        glm::mat4 r = p * glm::toMat4(m_rot);
        glm::mat4 t = glm::translate(p, m_pos);

        m_transform =  /*p **/ t * r * s;

        m_isDirty = false;
    }

    return m_transform;
}


//////////////////////////////////////////////////////////////////////////////
const glm::mat4& Transform::matrix() const
{
    return m_transform;
}


const glm::vec3& Transform::origin() const
{
    return m_pos;
}


const glm::vec3& Transform::scale() const
{
    return m_scale;
}


const glm::quat& Transform::rotation() const
{
    return m_rot;
}


} // namespace bd
