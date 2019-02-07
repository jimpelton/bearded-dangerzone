

#ifndef bd_color_h
#define bd_color_h

#include <string>
#include <sstream>
#include <ostream>

namespace bd 
{

struct Color
{
  double r, g, b;

  bool
  operator==(Color const &rhs) const
  {
    return r == rhs.r &&
        g == rhs.g &&
        b == rhs.b;
  }

  bool
  operator!=(Color const &rhs) const
  {
    return !( rhs == *this );
  }

  std::string
  to_string() const
  {
    std::stringstream ss;
    ss << "{R: " << r << ", G: " << g << ", B: " << b << '}';
    return ss.str();
  }

}; // struct Color

std::ostream& 
operator<<(std::ostream &os, bd::Color const &c);


} // namespace bd

#endif // ! bd_color_h
