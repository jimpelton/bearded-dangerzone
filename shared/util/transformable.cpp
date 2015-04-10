#include "transformable.h"

namespace bd
{
;

//////////////////////////////////////////////////////////////////////////////
Transformable::Transformable()
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


//////////////////////////////////////////////////////////////////////////////
//Transformable* Transformable::parent()
//{
//    return m_parent;
//}

Transform& Transformable::transform()
{
    return m_transform;
}

} // namespace bd

