#ifndef create_vao_h__
#define create_vao_h__

#include <glm/fwd.hpp>

#include <vector>

namespace vert
{
///////////////////////////////////////////////////////////////////////////////
/// \brief Returns the 4 element array indices for the vertex of slice \c idx. 
///////////////////////////////////////////////////////////////////////////////
glm::u16vec4 sliceIndexToElements(uint16_t idx);

float sliceIndexToWorldPos(unsigned idx, float start, float delta);

float start(size_t num_slices, float min, float max);

///////////////////////////////////////////////////////////////////////////////
/// \brief Returns the spacing between \c num_slices things that fit evenly
///  between min,max 
///////////////////////////////////////////////////////////////////////////////
float delta(size_t num_slices, float min, float max);

void create_verts_xy(size_t num_slices, std::vector<glm::vec4> &vbuf);
void create_verts_xz(size_t numSlices, std::vector<glm::vec4> &vbuf);
void create_verts_yz(size_t numSlices, std::vector<glm::vec4> &vbuf);

void create_texbuf_xy(size_t num_slices, std::vector<glm::vec4> &texbuf);
void create_texbuf_xz(size_t numSlices, std::vector<glm::vec4> &texbuf);
void create_texbuf_yz(size_t numSlices, std::vector<glm::vec4> &texbuf);

void create_elementIndices(size_t numSlices, std::vector<uint16_t> &elebuf);

void create_elementIndicesReversed(size_t numSlices, std::vector<uint16_t>::iterator &start,
    std::vector<uint16_t>::iterator &end);

} // namepace vert

#endif // !create_vao_h__