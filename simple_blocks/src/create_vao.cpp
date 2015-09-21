
#include "create_vao.h"
#include "axis_enum.h"

#include <bd/geo/quad.h> 

#include <glm/glm.hpp>

#include <algorithm>
#include <cassert>


//namespace vert
//{

////////////////////////////////////////////////////////////////////////////////
/// \brief Create quads perpendicular to X-axis
///
///  y.min         y.max
///  z.max         z.max
///    +------------+
///    |            |
///    |            |
///    |            |
///    |            |
///    +------------+
///  y.min         y.max
///  z.min         z.min
////////////////////////////////////////////////////////////////////////////////
void createQuads_X(std::vector<glm::vec4> &quads, const glm::vec3 &min,
                   const glm::vec3 &max, accum_delta<float> &nextDelta) {

  while(nextDelta.hasNext()) {
    float offset = nextDelta();
    quads.push_back({offset, min.y, min.z, 1});   // ll
    quads.push_back({offset, max.y, min.z, 1});   // lr
    quads.push_back({offset, min.y, max.z, 1});   // ul
    quads.push_back({offset, max.y, max.z, 1});   // ur
  }

}

////////////////////////////////////////////////////////////////////////////////
/// \brief Create quads perpendicular to Y-axis
///
///  x.min         x.max
///  z.max         z.max
///    +------------+
///    |            |
///    |            |
///    |            |
///    |            |
///    +------------+
///  x.min         x.max
///  z.min         z.min
////////////////////////////////////////////////////////////////////////////////
void createQuads_Y(std::vector<glm::vec4> &quads, const glm::vec3 &min,
                   const glm::vec3 &max, accum_delta<float> &nextDelta) {

  while(nextDelta.hasNext()) {
    float offset = nextDelta();
    quads.push_back({min.x, offset, min.z, 1});   // ll
    quads.push_back({max.x, offset, min.z, 1});   // lr
    quads.push_back({min.x, offset, max.z, 1});   // ul
    quads.push_back({max.x, offset, max.z, 1});   // ur
  }

}

////////////////////////////////////////////////////////////////////////////////
/// \brief Create quads perpendicular to Z-axis
///
///  x.min         x.max
///  y.max         y.max
///    +------------+
///    |            |
///    |            |
///    |            |
///    |            |
///    +------------+
///  x.min         x.max
///  y.min         y.min
////////////////////////////////////////////////////////////////////////////////
void createQuads_Z(std::vector<glm::vec4> &quads, const glm::vec3 &min,
                   const glm::vec3 &max, accum_delta<float> &nextDelta) {

  while(nextDelta.hasNext()) {
    float offset = nextDelta();
    quads.push_back({min.x, min.y, offset, 1});   // ll
    quads.push_back({max.x, min.y, offset, 1});   // lr
    quads.push_back({min.x, max.y, offset, 1});   // ul
    quads.push_back({max.x, max.y, offset, 1});   // ur
  }

}

///////////////////////////////////////////////////////////////////////////////
/// \brief Create quad proxy geometry along axis \c a, within bounding box with
///        corners \c min and \c max.
/// \note Quads are created in the region R=[min+delta, max-delta].
/// \param quads[out] Storage for created quads to be returned in.
/// \param numPlanes[in] Number of quads to create.
/// \param a[in] Quads created perpendicular to \c a.
///////////////////////////////////////////////////////////////////////////////
void createQuads(std::vector<glm::vec4> &quads, const glm::vec3 &min, 
                 const glm::vec3 &max, size_t numPlanes, Axis a) {

  quads.clear();
  quads.reserve(numPlanes);
  float delta{ 0 };

  switch(a) {
    case Axis::X:
      {
        delta = (max.x - min.x) / static_cast<float>(numPlanes);
        accum_delta<float> ad(min.x, delta, max.x);
        createQuads_X(quads, min, max, ad);
        break;
      }
    case Axis::Y:
      {
        delta = (max.y - min.y) / static_cast<float>(numPlanes);
        accum_delta<float> ad(min.y, delta, max.y);
        createQuads_Y(quads, min, max, ad);
        break;
      }
    case Axis::Z:
      {
        delta = (max.z - min.z) / static_cast<float>(numPlanes);
        accum_delta<float> ad(min.z, delta, max.z);
        createQuads_Z(quads, min, max, ad);
        break;
      }
      // default: break;
  }

}


void createElementIdx(std::vector<unsigned short> &elebuf) {

}





//
/////////////////////////////////////////////////////////////////////////////////
//float sliceIndexToWorldPos(unsigned idx, float start, float delta)
//{
//    return start + delta*idx;
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
//glm::u16vec4 sliceIndexToElements(uint16_t idx)
//{
//    const uint16_t stride = 4;
//    return glm::u16vec4(0, 1, 3, 2) + uint16_t(stride*idx);
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
//float start(size_t num_slices, float min, float max)
//{
//    //    float span = std::abs(max - min);
//    //float s = -1.0f * delta(num_slices, min, max) * (num_slices / 2);
//    //return s;
//    return num_slices > 1 ? min : 0.0f;
//    //    return -1.0f * delta(num_slices, min, max);
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
//float delta(size_t num_slices, float min, float max)
//{
//    if (num_slices <= 1) {
//        return 0.0f;
//    }
//
//    //return (max - min) / float(num_slices-1);
//    return (max - min) / float(num_slices);
//}
//
////TODO: use texbuf_adjust instead of duplicated code in texbuf vbo create methods.
//void texbuf_adjust(std::vector<glm::vec4> &texbuf)
//{
//    float diff = 1 - VOL_MAX;
//    std::for_each(texbuf.begin(), texbuf.end(),
//        [diff](glm::vec4 &v){ v += glm::vec4(diff, diff, diff, 0.0f); });
//}
//
/////////////////////////////////////////////////////////////////////////////////
//void create_verts_xy(size_t numSlices, std::vector<glm::vec4> &vbuf)
//{
//    const int z_axis_idx = 2;
//    create_verts_helper(numSlices, vbuf, z_axis_idx, bd::Quad::verts_xy);
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
//void create_verts_xz(size_t numSlices, std::vector<glm::vec4> &vbuf)
//{
//    const int y_axis_idx = 1;
//    create_verts_helper(numSlices, vbuf, y_axis_idx, bd::Quad::verts_xz);
//
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
//void create_verts_yz(size_t numSlices, std::vector<glm::vec4> &vbuf)
//{
//    const int x_axis_idx = 0;
//    create_verts_helper(numSlices, vbuf, x_axis_idx, bd::Quad::verts_yz);
//
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
//void create_texbuf_xy(size_t numSlices, std::vector<glm::vec4> &texbuf)
//{
//    create_verts_xy(numSlices, texbuf);
//
//    //TODO: float diff = 1-VOL_MAX doesn't work for all negative VOL_MAXes.
//    float diff = 1 - VOL_MAX;
//    std::for_each(texbuf.begin(), texbuf.end(),
//        [diff](glm::vec4 &v){ v += glm::vec4(diff, diff, diff, 0.0f); });
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
//void create_texbuf_xz(size_t numSlices, std::vector<glm::vec4> &texbuf)
//{
//    create_verts_xz(numSlices, texbuf);
//
//    //TODO: float diff = 1-VOL_MAX doesn't work for all negative VOL_MAXes.
//    float diff = 1 - VOL_MAX;
//    std::for_each(texbuf.begin(), texbuf.end(),
//        [diff](glm::vec4 &v){ v += glm::vec4(diff, diff, diff, 0.0f); });
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
//void create_texbuf_yz(size_t numSlices, std::vector<glm::vec4> &texbuf)
//{
//    create_verts_yz(numSlices, texbuf);
//
//    //TODO: float diff = 1-VOL_MAX doesn't work for all negative VOL_MAXes.
//    float diff = 1 - VOL_MAX;
//    std::for_each(texbuf.begin(), texbuf.end(),
//        [diff](glm::vec4 &v){ v += glm::vec4(diff, diff, diff, 0.0f); });
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
//void create_elementIndices(size_t numSlices, std::vector<uint16_t> &elebuf)
//{
//    elebuf.resize(numSlices * 5);
//    auto it = elebuf.begin();
//    auto end = elebuf.end();
//    int i = 0;
//    for (; it != end; it += 5, ++i){
//        *it = uint16_t(0 + 4 * i);
//        *(it + 1) = uint16_t(1 + 4 * i);
//        *(it + 2) = uint16_t(3 + 4 * i);
//        *(it + 3) = uint16_t(2 + 4 * i);
//        *(it + 4) = uint16_t(0xFFFF);  // Special restart symbol.
//    }
//}


///////////////////////////////////////////////////////////////////////////////
//void create_elementIndicesReversed(size_t numSlices, std::vector<uint16_t>::iterator &start,
//    std::vector<uint16_t>::iterator &end)
//{
//    assert("std::distance(start,end) >= numSlices" && std::distance(start, end) >= numSlices);
//
//    uint16_t i = static_cast<uint16_t>(numSlices - 1);
//    for (; start != end; start += 5, --i){
//        *start = uint16_t(0 + 4 * i);
//        *(start + 1) = uint16_t(1 + 4 * i);
//        *(start + 2) = uint16_t(3 + 4 * i);
//        *(start + 3) = uint16_t(2 + 4 * i);
//        *(start + 4) = uint16_t(0xFFFF);  // Special restart symbol.
//    }
//}
//
/////////////////////////////////////////////////////////////////////////////////
//void create_verts_helper(size_t numSlices, std::vector<glm::vec4> &vbuf,
//    int axis, const std::array<glm::vec4, 4> &vertsProto)
//{
//    float st = start(numSlices, VOL_MIN, VOL_MAX);
//    float del = delta(numSlices, VOL_MIN, VOL_MAX);
//    size_t numVerts = numSlices * bd::Quad::vert_element_size;
//
//    vbuf.resize(numVerts);
//
//    std::vector<glm::vec4>::iterator vbufIter{ vbuf.begin() };
//    for (int i = 0; i < numSlices; ++i) {
//        float pos{ sliceIndexToWorldPos(i, st, del) };
//        std::array<glm::vec4, 4> verts(vertsProto);
//        std::for_each(verts.begin(), verts.end(), [pos, axis](glm::vec4 &vv){ vv[axis] = pos; });
//        std::copy(verts.begin(), verts.end(), vbufIter);
//        vbufIter += verts.size();
//    }
//}

//} // namespace vert


