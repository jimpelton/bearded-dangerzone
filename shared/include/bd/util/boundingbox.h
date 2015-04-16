#ifndef boundingbox_h__
#define boundingbox_h__

#include <bd/scene/transformable.h>

#include <glm/glm.hpp>

namespace bd {

class BoundingBox : public Transformable
{
public:
    BoundingBox();
    ~BoundingBox();

    glm::vec3 min();
    glm::vec3 max();

private:
    glm::vec3 m_min;
    glm::vec3 m_max;

};

        

} // namespace bd
#endif // !boundingbox_h__
