#ifndef create_vao_h__
#define create_vao_h__

#include "axis_enum.h"

#include <bd/graphics/vertexarrayobject.h>

#include <glm/fwd.hpp>

#include <vector>

namespace bd
{
class Volume;
}

namespace subvol
{
struct VertexFormat
{
  VertexFormat()
      : VertexFormat({ 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f })
  {
  }


  VertexFormat(glm::vec4 const &pos, glm::vec3 const &uv)
      : pos{ pos }
      , uv{ uv }
  {
  }


  glm::vec4 pos;
  glm::vec3 uv;
};


/// \brief Create slices for each axis with in a region. The min, max of the
///        region boundingbox is given by min and max vectors.
/// \verbatim
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
/// \endverbatim
///
///
/// \param vao[out] VertexArrayObject
/// \param min[in] Min corner of region
/// \param max[in] Max corner of region
/// \param numSlices[in] Number of slices per each axis.


glm::u64vec3
genQuadVao(bd::VertexArrayObject &vao,
           bd::Volume const &v,
           glm::vec3 const &smod);


void
createQuads(std::vector<VertexFormat> &verts,
            size_t numSlices,
            float delta,
            Axis a);

void
createQuadsReversed(std::vector<VertexFormat> &verts,
            size_t numSlices,
            float delta,
            Axis a);

/// \brief Create element indexes for the verts returned by \c createQuads()
///
/// \note Elements are separated by restart symbol 0xFFFF.
///
/// \param elebuf[out] Vector for returning element indexes.
/// \param numQuads[in] Number of quads in each block.
void
createElementIdx(std::vector<unsigned short> &elebuf,
                 size_t numslices);


/// \brief Generate the vertex buffers for coordinate axis widget
void
genAxisVao(bd::VertexArrayObject &vao);


/// \brief Generate the vertex buffers for bounding box around the blocks
void
genBoxVao(bd::VertexArrayObject &vao);
}

#endif // !create_vao_h__
