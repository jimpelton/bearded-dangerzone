
#include "volume.h"

#include <bd/util/util.h>

#include <algorithm>
#include <sstream>


///////////////////////////////////////////////////////////////////////////////
Volume::Volume()
    : Volume(glm::u64vec3(0))
{
}


///////////////////////////////////////////////////////////////////////////////
Volume::Volume(const glm::u64vec3 &dims)
    : m_numVox{ dims }
    , m_dims_world{}
{
    // normalize to the longest dimension
    auto largest = std::max({ dims.x, dims.y, dims.z });
    if (largest != 0) {
        m_dims_world =
            glm::vec3{ dims } / static_cast<float>(largest);
    }
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


std::string Volume::to_string() const
{
    std::stringstream ss;
    ss << bd::Transformable::to_string() <<
    " {vox: " << m_numVox.x << "x" << m_numVox.y << "x" << m_numVox.z <<
    "} {dims: " <<m_dims_world.x << "x" << m_dims_world.y << "x" << m_dims_world.z
    << "}";

    return ss.str();
}
