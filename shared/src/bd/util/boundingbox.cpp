#include <bd/util/boundingbox.h>

namespace bd {



BoundingBox::BoundingBox()
{

}

BoundingBox::~BoundingBox()
{

}

glm::vec3 BoundingBox::min()
{
    return m_min;
}

glm::vec3 BoundingBox::max()
{
    return m_max;
}

} // namespace bd

