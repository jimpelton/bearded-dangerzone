#ifndef util_h__
#define util_h__

#include <glm/glm.hpp>

#include <memory>
#include <string>

namespace bd {

void hsvToRgb(float h, float s, float v, glm::vec3 &rgb);


///////////////////////////////////////////////////////////////////////////////
// \brief Convert \c x,y,z to 1D coordinate.
///////////////////////////////////////////////////////////////////////////////
size_t to1D(size_t x, size_t y, size_t z, size_t maxX, size_t maxY);


///////////////////////////////////////////////////////////////////////////////
// \brief Compute product of components of \c v.
///////////////////////////////////////////////////////////////////////////////
unsigned long long vecCompMult(glm::u64vec3 v);

std::unique_ptr<float []> readVolumeData(const std::string &dtype,
    const std::string &fpath, size_t volx, size_t voly, size_t volz);

} // namespace bd

#endif
