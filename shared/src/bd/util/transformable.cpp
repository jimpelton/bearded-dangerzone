#include <bd/util/transformable.h>
#include <iostream>

namespace bd
{


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
    m_transform.update(glm::mat4(1.0));
    updateChildren();
}


///////////////////////////////////////////////////////////////////////////////
void Transformable::addChild(Transformable *c)
{
    assert(c != nullptr);
    m_children.push_back(c);
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

Transform& Transformable::transform()
{
    return m_transform;
}

///////////////////////////////////////////////////////////////////////////////
//const Transform& Transformable::transform() const
//{
//    return m_transform;
//}

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



