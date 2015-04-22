

#include "block.h"

#include <bd/scene/transformable.h>

#include <glm/gtx/string_cast.inl>

#include <sstream>


///////////////////////////////////////////////////////////////////////////////
Block::Block()
    : Transformable()
    , m_bidx{ 0 }
    , m_loc{ 0, 0, 0 }
    , m_min{ 0.0f, 0.0f, 0.0f }
    , m_avg{ 0.0f }
    , m_empty{ false }
    , m_texid{ 0 }
{
}


///////////////////////////////////////////////////////////////////////////////
Block::Block(size_t bidx, const glm::u64vec3 &voxloc,
        const glm::u64vec3 &numvox, const glm::vec3 &worldMinCoords,
        const glm::vec3 &color /*= glm::vec3(1, 1, 1)*/ )
    : Transformable()
    , m_bidx{bidx}
    , m_loc{voxloc}
    , m_min{worldMinCoords}
    , m_numvox{ numvox }
    , m_avg{ 0.0f }
    , m_empty{ true }
    , m_texid{ 0 }
{
}


///////////////////////////////////////////////////////////////////////////////
Block::~Block()
{
}


///////////////////////////////////////////////////////////////////////////////
std::string Block::to_string() const
{
    std::stringstream ss;

    ss << "Idx: " << m_bidx << "\n"
        "Loc: " << glm::to_string(m_loc) << "\n"
        "Min: " << glm::to_string(m_min) << "\n"
        //"Dims: " << glm::to_string(worldDims) << "\n"
        "Empty: " << m_empty << " (" << m_avg << ")";

    return ss.str();
}

size_t Block::bidx() const
{
    return m_bidx;
}

void Block::bidx(size_t idx)
{
    m_bidx = idx;
}

glm::u64vec3 Block::loc() const
{
    return m_loc;
}

void Block::loc(glm::u64vec3 l)
{
    m_loc = l;
}

glm::vec3 Block::min() const
{
    return m_min;
}

void Block::min(glm::vec3 m)
{
    m_min = m;
}

float Block::avg() const
{
    return m_avg;
}

void Block::avg(float a)
{
    m_avg = a;
}

bool Block::empty() const
{
    return m_empty;
}

void Block::empty(bool e)
{
    m_empty = e;
}

unsigned int Block::texid() const
{
    return m_texid;
}

void Block::texid(unsigned int id)
{
    m_texid = id;
}

glm::u64vec3 Block::numVox() const
{
    return m_numvox;
}
