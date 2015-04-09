#ifndef aabb_h__
#define aabb_h__

#include "transform.h"

#include <glm/glm.hpp>

namespace bd {

class BoundingBox 
{
public:
    BoundingBox();
    ~BoundingBox();

    
private:
    glm::vec3 m_min;
    glm::vec3 m_max;

    Transform m_tr;

};

        

} // namespace bd
#endif // aabb_h__
