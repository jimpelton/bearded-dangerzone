#ifndef util_h__
#define util_h__

#include <glm/fwd.hpp>

#include <memory>
#include <string>

namespace bd
{
///////////////////////////////////////////////////////////////////////////////
/// \brief Convert hue, saturation and value to red, green, blue.
/// \param h Hue value
/// \param s Saturation value
/// \param v Value value ;)
/// \param[out] Returns the corresponding rgb value for h,s,v.
///////////////////////////////////////////////////////////////////////////////
void hsvToRgb(float h, float s, float v, glm::vec3& rgb);


///////////////////////////////////////////////////////////////////////////////
/// \brief Convert \c x,y,z to 1D coordinate.
///////////////////////////////////////////////////////////////////////////////
size_t to1D(size_t col, size_t row, size_t slab, size_t maxCols, size_t maxRows);


///////////////////////////////////////////////////////////////////////////////
/// \brief Compute product of components of \c v.
///////////////////////////////////////////////////////////////////////////////
unsigned long long vecCompMult(const glm::u64vec3 &v);


//template<class VecType, class NumberType,
//         typename =
//         typename std::enable_if<
//             std::is_arithmetic<NumberType>::value, NumberType>::type>
//NumberType lerp(VecType x, VecType y, NumberType a);


} // namespace bd

#endif // !util_h__


