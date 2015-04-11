
#include "volume.h"

#include <util/util.h>

#include <algorithm>



Volume::Volume()
    : Transformable()
{
}

Volume::Volume(const glm::u64vec3 &dims) 
    : Transformable()
    , m_numVox{ dims }
{
    // normalize to the longest dimension
    auto largest = std::max({ dims.x, dims.y, dims.z });
    if (largest != 0) {
        m_dims_world =
            glm::vec3{ dims } / static_cast<float>(largest);
    }
}

Volume::~Volume()
{

}


glm::vec3 Volume::worldDims() const
{
    return m_dims_world;
}


glm::u64vec3 Volume::numVox() const
{
    return m_numVox;
}


unsigned long long Volume::totalVox()
{
    return bd::vecCompMult(m_numVox);
}
