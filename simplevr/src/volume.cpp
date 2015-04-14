
#include "volume.h"

#include <bd/util/util.h>

#include <algorithm>


///////////////////////////////////////////////////////////////////////////////
Volume::Volume()
    : Volume( glm::u64vec3(0), glm::u64vec3(1,1,1) )
{
}


///////////////////////////////////////////////////////////////////////////////
Volume::Volume(const glm::u64vec3 &dims, const glm::u64vec3 &bs)
    : Transformable()
    , m_numVox{ dims }
{
    // normalize to the longest dimension
    auto largest = std::max({ dims.x, dims.y, dims.z });
    if (largest != 0) {
        m_dims_world =
            glm::vec3{ dims } / static_cast<float>(largest);
    }

    addChild(&m_collection);

    m_collection.initBlocks(bs, dims);

}


///////////////////////////////////////////////////////////////////////////////
Volume::~Volume()
{
}


///////////////////////////////////////////////////////////////////////////////
glm::vec3 Volume::worldDims() const
{
    return m_dims_world;
}


///////////////////////////////////////////////////////////////////////////////
glm::u64vec3 Volume::numVox() const
{
    return m_numVox;
}


///////////////////////////////////////////////////////////////////////////////
unsigned long long Volume::totalVox()
{
    return bd::vecCompMult(m_numVox);
}

///////////////////////////////////////////////////////////////////////////////
const BlocksCollection& Volume::collection() const
{
    return m_collection;
}
