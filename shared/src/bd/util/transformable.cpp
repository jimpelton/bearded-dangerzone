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
    if (parent != nullptr)
        m_transform.update(parent->transform().matrix());
    else
        m_transform.update(glm::mat4(1.0));

    for (int i=0; i<m_children.size(); ++i) {
        Transformable *c = m_children[i];

        if (c==nullptr)
            std::cerr<<"Oh my, a null child snuck in!" << std::endl;
        else
            m_children[i]->update(this);
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

const std::vector<Transformable*>& Transformable::children() const
{
    return m_children;
}

std::string Transformable::to_string() const
{
    return BDObj::to_string() + " { children: " + std::to_string(m_children.size()) + " }";
}
} // namespace bd

