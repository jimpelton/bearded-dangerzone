

#include "block.h"

#include <sstream>

std::string Block::to_string() const
{
    std::stringstream ss;
    ss << "Idx: " << bidx << "\n"
        "Loc: " << glm::to_string(loc) << "\n"
        "Min: " << glm::to_string(min) << "\n"
        //"Dims: " << glm::to_string(dims) << "\n"
        "Avg: " << avg << "\n"
        "Empty: " << empty << " (" << avg << ")";
    return ss.str();
}
