

#include "view.h"

#include <GL/glew.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace bd{
;

View::View() 
    : m_position{ 0.0f }
    , m_viewport{ 0 }
    , m_rotation{ }
    , m_proj{ 1.0f }
    , m_view{ 1.0f }
    , m_viewDirty{ true }
{
}


///////////////////////////////////////////////////////////////////////////////
View::~View()
{
}

//void View::setMouseRotation(const glm::vec2& cpos)
//{
//    glm::vec2 dr{ m_cursorPos - cpos };
//
//    glm::quat rotX{ glm::angleAxis<float>(
//        glm::radians(dr.y) * m_mouseSpeed,
//        glm::vec3(1, 0, 0)
//        ) };
//
//    glm::quat rotY{ glm::angleAxis<float>(
//        glm::radians(-dr.x) * m_mouseSpeed,
//        glm::vec3(0, 1, 0)
//        ) };
//
//    m_rotation = (rotX * rotY) * m_rotation;
//
//    m_viewDirty = true;
//}
//
//void View::setCursorPos(const glm::vec2& cpos)
//{
//    m_cursorPos = cpos;
//}


///////////////////////////////////////////////////////////////////////////////
void View::setViewport(int x, int y, int w, int h)
{
    m_viewport = glm::uvec4(x, y, w, h);
    glViewport(x, y, w, h);

    m_viewDirty = true;
}


///////////////////////////////////////////////////////////////////////////////
void View::setPosition(const glm::vec3& pos)
{
    m_position = pos;
}


///////////////////////////////////////////////////////////////////////////////
glm::vec3 View::getPosition() const
{
    return m_position;
}


///////////////////////////////////////////////////////////////////////////////
void View::translate(const glm::vec3 & delta)
{
    m_position += delta;
    m_viewDirty = true;
}


///////////////////////////////////////////////////////////////////////////////
void View::setRotation(const glm::quat &rot)
{
    m_rotation = rot;
    m_viewDirty = true;
}


///////////////////////////////////////////////////////////////////////////////
glm::quat View::getRotation() const
{
    return m_rotation;
}


///////////////////////////////////////////////////////////////////////////////
void View::rotate(const glm::quat &rot)
{
    m_rotation *= rot;
    m_viewDirty = true;
}


///////////////////////////////////////////////////////////////////////////////
void View::setProjectionMatrix(float fov, float aspect_rat, float z_near, float z_far)
{
    m_proj = glm::perspective(fov, aspect_rat, z_near, z_far);
}


///////////////////////////////////////////////////////////////////////////////
glm::mat4 View::getProjectionMatrix() const
{
    return m_proj;
}


///////////////////////////////////////////////////////////////////////////////
glm::mat4 View::getViewMatrix() const
{
    return m_view;
}


///////////////////////////////////////////////////////////////////////////////
void View::updateViewMatrix()
{
    glm::mat4 tr = glm::translate(glm::mat4(1.0), -(m_position));
    glm::mat4 rot = glm::transpose(glm::toMat4(m_rotation));

    m_view = rot * tr;

    m_viewDirty = false;
}

} // namespace bd