

#include "block.h"

#include <glm/gtx/string_cast.inl>

#include <sstream>

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
