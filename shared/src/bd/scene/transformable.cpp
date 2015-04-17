#include <bd/scene/transformable.h>

#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <glm/gtx/quaternion.hpp>

namespace bd
{
namespace {
    const glm::mat4 I4x4{ 1.0f };
}


//////////////////////////////////////////////////////////////////////////////
Transformable::Transformable()
    : BDObj()
    , m_transform{ }
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
    assert(parent != nullptr);
    m_transform.update(parent->transform().matrix());
    updateChildren();
}

void Transformable::update()
{
    m_transform.update(m_transform.matrix());

    updateChildren();
}


///////////////////////////////////////////////////////////////////////////////
void Transformable::addChild(Transformable *c)
{
    assert(c != nullptr);
    m_children.push_back(c);
}


Transform& Transformable::transform()
{
    return m_transform;
}


const std::vector<Transformable*>& Transformable::children() const
{
    return m_children;
}

std::string Transformable::to_string() const
{
    return BDObj::to_string() + " { children: " + std::to_string(m_children.size()) + " }";
}

void Transformable::updateChildren()
{
    for (auto &t : m_children)
    {
        t->update(this);
    }
}

} // namespace bd



