#include "transformable.h"

namespace bd
{
;

//////////////////////////////////////////////////////////////////////////////
Transformable::Transformable()
    : m_parent{ nullptr }
{
}


//////////////////////////////////////////////////////////////////////////////
Transformable::~Transformable()
{
}


//////////////////////////////////////////////////////////////////////////////
void Transformable::update(Transformable * parent)
{
    m_transform->update();

    if (parent != nullptr) {
        Transform *trans = parent->transform();
    }
    

    for (auto &t : m_children)
    {
        t->update(this);
    }
}


//////////////////////////////////////////////////////////////////////////////
Transformable* Transformable::parent()
{
    return m_parent;
}

Transform* Transformable::transform()
{
    return & m_transform;
}

} // namespace bd

