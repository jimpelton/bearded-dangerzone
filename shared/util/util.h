#ifndef util_h__
#define util_h__

#include <glm/glm.hpp>

namespace bd {
namespace util {

    void hsvToRgb(float h, float s, float v, glm::vec3 &rgb);

    

    size_t to1D(size_t x, size_t y, size_t z, size_t maxX, size_t maxY);
   
} // namespace util
} // namespace bd

#endif
