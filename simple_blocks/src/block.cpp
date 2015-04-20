

#include "block.h"

#include <bd/scene/transformable.h>
#include <bd/util/util.h>

#include <glm/gtx/string_cast.inl>

#include <sstream>


///////////////////////////////////////////////////////////////////////////////
Block::Block()
    : Block(glm::u64vec3(0,0,0), glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Block::Block(const glm::u64vec3 &ijk, const glm::vec3 &origin)
    : m_ijk{ ijk }, m_origin{ origin }
{
}


///////////////////////////////////////////////////////////////////////////////
Block::~Block()
{
}



glm::vec3 Block::origin() const
{
    return m_origin;
}

void Block::origin(const glm::vec3 &o)
{
    m_origin = o;
}

glm::u64vec3 Block::ijk() const
{
    return m_ijk;
}

void Block::ijk(const glm::u64vec3 &ijk)
{
    m_ijk = ijk;
}

///////////////////////////////////////////////////////////////////////////////
std::string Block::to_string() const
{
    std::stringstream ss;

    ss <<  "ijk: (" << m_ijk.x << ',' << m_ijk.y << ',' << m_ijk.z << ")\n"
        "Origin: " << m_origin.x << ',' << m_origin.y << ',' << m_origin.z ;

    return ss.str();
}

