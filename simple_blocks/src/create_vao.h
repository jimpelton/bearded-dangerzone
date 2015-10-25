#ifndef create_vao_h__
#define create_vao_h__

#include "axis_enum.h"

#include <bd/graphics/vertexarrayobject.h>

#include <glm/fwd.hpp>

#include <vector>
#include <type_traits>


///////////////////////////////////////////////////////////////////////////
/// \brief Generates (max-start)/d Ts.
///        Returns values in sequence from [start .. max), non-inclusive on the right.
///////////////////////////////////////////////////////////////////////////
template<typename T,
         typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
class accum_delta {
public:

  ///////////////////////////////////////////////////////////////////////////
  accum_delta(T start, T d, T max)
      : m_delta{ d }, m_next{ start }, m_max{ max } { }


  ///////////////////////////////////////////////////////////////////////////
  T operator()() {
    T r = m_next;
    m_next += m_delta;

    return r;
  }


  ///////////////////////////////////////////////////////////////////////////
  /// \return True until max has been returned by operator().
  ///////////////////////////////////////////////////////////////////////////
  bool hasNext() {
    return m_next < m_max;
  }


private:
  T m_delta;
  T m_next;
  T m_max;
};


///////////////////////////////////////////////////////////////////////////////
/// \brief Create slices for each axis with in a region. The min, max of the
///        region boundingbox is given by min and max vectors.
///
///     {min.x, max.y, max.z}
///           +---------------+ max
///          /               /|
///         /               / |
/// {min.x, max.y, min.z}  /  |
///       +--------------+` {max.x, max.y, min.z}
///       |              |    |
///       |              |    |
///       |              |    + {max.x, min.y, max.z}
///       |              |   /
///       |              |  /
///       |              | /
///  min  +--------------+`  {max.x,  min.y, min.z}
///
///
/// \param vao[out] VertexArrayObject
/// \param min[in] Min corner of region
/// \param max[in] Max corner of region
/// \param numSlices[in] Number of slices per each axis.
////////////////////////////////////////////////////////////////////////////////
void genQuadVao(bd::VertexArrayObject &vao, const glm::vec3 &min,
                const glm::vec3 &max,
                const glm::u64vec3 &numSlices);


///////////////////////////////////////////////////////////////////////////////
/// \brief Create quad proxy geometry along axis \c a, within bounding box with
///        corners \c min and \c max.
///
/// \note Quads are created in the region R=[min+delta, max-delta].
///
/// \param quads[out] Storage for created quads to be returned in.
/// \param min[in] Minimum corner of bounding region
/// \param max[in] Maximum corner of bounding region.
/// \param numPlanes[in] Number of quads to create.
/// \param a[in] Quads created perpendicular to \c a.
///////////////////////////////////////////////////////////////////////////////
void createQuads(std::vector<glm::vec4> &quads, const glm::vec3 &min,
                 const glm::vec3 &max, size_t numPlanes, Axis a);


////////////////////////////////////////////////////////////////////////////////
/// \brief Create element indexes for the verts returned by \c createQuads()
///
/// \note Elements are separated by restart symbol 0xFFFF.
///
/// \param elebuf[out] Vector for returning element indexes.
/// \param numQuads[in] Number of quads to generate elements for.
////////////////////////////////////////////////////////////////////////////////
void createElementIdx(std::vector<unsigned short> &elebuf, size_t numQuads);


//} // namepace vert

#endif // !create_vao_h__
