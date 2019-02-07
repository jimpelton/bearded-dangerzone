


#include <bd/util/color.h>

namespace bd
{

std::ostream& 
operator<<(std::ostream &os, bd::Color const &c)
{
  return os << c.to_string();
}


} // namespace
