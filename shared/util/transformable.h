#ifndef transformable_h__
#define transformable_h__

#include "util/transform.h"

#include <vector>

namespace bd {
;

class Transformable 
{
    Transformable();
    //Transformable(Transformable* parent);

    virtual ~Transformable();


    void update(Transformable *parent);
    //Transformable *parent();
    Transform* transform();
private:
    //Transformable* parent;
    Transform m_transform;

    std::vector<Transformable*> m_children;
    
};

}

#endif