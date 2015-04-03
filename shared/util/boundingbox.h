#ifndef aabb_h__
#define aabb_h__

#include <glm/glm.hpp>

namespace bd{

class BoundingBox 
{
public:
    BoundingBox();
    ~BoundingBox();


    void translate(glm::vec3 center);
    void scale(glm::vec3 by);
        
private:
    glm::vec3 m_min;
    glm::vec3 m_max;

    glm::mat4 m_transform;

};

        

} // namespace bd
#endif // aabb_h__
