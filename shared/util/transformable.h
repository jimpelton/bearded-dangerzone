#ifndef transformable_h__
#define transformable_h__

#include "util/transform.h"

#include <vector>

namespace bd {
;

class Transformable 
{
public:
    Transformable();

    virtual ~Transformable();

    virtual void update(Transformable *parent);

    Transform& transform();

private:
    
    Transform m_transform;

    std::vector<Transformable*> m_children;
    
};

} // namespace bd

#endif