

#include "block.h"

#include <bd/scene/transformable.h>
#include <bd/util/util.h>

#include <glm/gtx/string_cast.inl>

#include <sstream>


///////////////////////////////////////////////////////////////////////////////
//Block::Block()
//    : Block(glm::u64vec3(0,0,0), glm::vec3(0.0f, 0.0f, 0.0f))
//{
//}

Block::Block(const glm::u64vec3 &ijk, const glm::vec3 &dims, const glm::vec3 &origin)
    : m_ijk{ ijk }
    , m_empty{ false }
    , m_avg{ 0.0f }
    , m_tex{  }
{
    transform().scale(dims);
    transform().origin(origin);
    update();
}


///////////////////////////////////////////////////////////////////////////////
Block::~Block()
{
}

//void Block::draw()
//{
//
//}

///////////////////////////////////////////////////////////////////////////////
glm::u64vec3 Block::ijk() const
{
    return m_ijk;
}


///////////////////////////////////////////////////////////////////////////////
void Block::ijk(const glm::u64vec3 &ijk)
{
    m_ijk = ijk;
}


///////////////////////////////////////////////////////////////////////////////
bool Block::empty() const
{
    return m_empty;
}


///////////////////////////////////////////////////////////////////////////////
void Block::empty(bool b)
{
    m_empty = b;
}


///////////////////////////////////////////////////////////////////////////////
float Block::avg() const
{
    return m_avg;
}


///////////////////////////////////////////////////////////////////////////////
void Block::avg(float a)
{
    m_avg = a;
}

Texture& Block::texture()
{
    return m_tex;
}


///////////////////////////////////////////////////////////////////////////////
std::string Block::to_string() const
{
    std::stringstream ss;
    ss <<  "ijk: (" << m_ijk.x << ',' << m_ijk.y << ',' << m_ijk.z << ")\n"
        "Origin: " << m_transform.origin().x << ',' << m_transform.origin().y << ',' <<
        m_transform.origin().z <<
        "Empty: " << (m_empty ? "True\n" : "False\n");

    return ss.str();
}

std::ostream& operator<<(std::ostream &os, const Block &b)
{
    return os << b.to_string();
}

