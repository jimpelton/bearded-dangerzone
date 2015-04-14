#include <bd/util/transformable.h>
#include <iostream>

namespace bd
{


//////////////////////////////////////////////////////////////////////////////
Transformable::Transformable()
    : m_transform{ }
    , m_children{ }
{
}


//////////////////////////////////////////////////////////////////////////////
Transformable::~Transformable()
{
}


//////////////////////////////////////////////////////////////////////////////      
void Transformable::update(Transformable * parent)
{
    if (parent != nullptr)
        m_transform.update(parent->transform().matrix());
    else
        m_transform.update(glm::mat4(1.0));

    for (Transformable *c : m_children)
    {
        c->update(this);
    }
}


///////////////////////////////////////////////////////////////////////////////
void Transformable::addChild(Transformable *c)
{
    if (c != nullptr)
        m_children.push_back(c);
    else
        std::cerr << "tried to push nullptr." << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
const Transform& Transformable::transform() const
{
    return m_transform;
}


///////////////////////////////////////////////////////////////////////////////
void Transformable::scale(const glm::vec3 &by)
{
    m_transform.scale(by);
}


///////////////////////////////////////////////////////////////////////////////
void Transformable::rotate(const glm::quat &amt)
{
    m_transform.rotate(amt);
}


///////////////////////////////////////////////////////////////////////////////
void Transformable::translate(const glm::vec3 &amt)
{
    m_transform.translate(amt);
}


///////////////////////////////////////////////////////////////////////////////
void Transformable::position(const glm::vec3 &pos)
{
    m_transform.position(pos);
}


} // namespace bd

