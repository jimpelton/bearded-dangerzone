#include "create_vao.h"

#include "axis_enum.h"
#include "constants.h"

#include <bd/geo/axis.h>
#include <bd/log/logger.h>
#include <bd/geo/BBox.h>

#include <glm/glm.hpp>

namespace subvol
{

namespace
{


/*////////////////////////////////////////////////////////////////////////////////
//   createQuads_* Implementations
////////////////////////////////////////////////////////////////////////////////*/


/// \brief Create quads perpendicular to X-axis within the region with diagonal
///        from min to max.
///
/// \note Quad vertices are returned in triangle strip winding order.
///
/// \param[in] min Minimum corner of first plane
/// \param[in] max Maximum corner of first plane.
/// \verbatim
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
/// \endverbatim
//void createQuads_X(std::vector<glm::vec4> &quads,
//                   glm::vec3 const &min, glm::vec3 const &max,
//                   decrement_delta<float> &nextDelta)
//{
//
//  while (nextDelta.hasNext()) {
//    float offset = nextDelta();
//    quads.push_back({ offset, min.y, max.z, 1 });   // ll
//    quads.push_back({ offset, min.y, min.z, 1 });   // lr
//    quads.push_back({ offset, max.y, max.z, 1 });   // ul
//    quads.push_back({ offset, max.y, min.z, 1 });   // ur
//  }
//
//}


/// \brief Create quads perpendicular to Y-axis within the region with diagonal
///        from min to max.
///
/// \note Quad vertices are returned in triangle strip winding order.
///
/// \param[in] min Minimum corner of first plane
/// \param[in] max Maximum corner of first plane.
/// \verbatim
///  x.min         x.max
///  z.min         z.min
///    +------------+
///    |            |
///    |            |
///    |            |
///    |            |
///    +------------+
///  x.min         x.max
///  z.max         z.max
/// \endverbatim
//void createQuads_Y(std::vector<glm::vec4> &quads,
//                   glm::vec3 const &min, glm::vec3 const &max,
//                   decrement_delta<float> &nextDelta)
//{
//
//  while (nextDelta.hasNext()) {
//    float offset = nextDelta();
//    quads.push_back({ min.x, offset, max.z, 1 });   // ll
//    quads.push_back({ max.x, offset, max.z, 1 });   // lr
//    quads.push_back({ min.x, offset, min.z, 1 });   // ul
//    quads.push_back({ max.x, offset, min.z, 1 });   // ur
//  }
//
//}


/// \brief Create quads perpendicular to Z-axis within the region with diagonal
///        from min to max.
///
/// \note Quad vertices are returned in triangle strip winding order.
///
/// \param[in] min Minimum corner of region
/// \param[in] max Maximum corner of region.
///
/// \verbatim
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
/// \endverbatim
//void createQuads_Z(std::vector<glm::vec4> &quads,
//                   glm::vec3 const &min, glm::vec3 const &max,
//                   decrement_delta<float> &nextDelta)
//{
//
//  while (nextDelta.hasNext()) {
//    float offset = nextDelta();
//    quads.push_back({ min.x, min.y, offset, 1 });   // ll
//    quads.push_back({ max.x, min.y, offset, 1 });   // lr
//    quads.push_back({ min.x, max.y, offset, 1 });   // ul
//    quads.push_back({ max.x, max.y, offset, 1 });   // ur
//  }
//}

} // namespace

/*///////////////////////////////////////////////////////////////////////////////*/

void
genQuadVao(bd::VertexArrayObject &vao, glm::vec3 const &min, glm::vec3 const &max,
           glm::u64vec3 const &numSlices)
{
  assert((numSlices.x == numSlices.y && numSlices.y == numSlices.z) &&
           "numSlices must be equal in all 3 dimensions");

  std::vector<glm::vec4> vbuf;
  std::vector<glm::vec4> texbuf;
  std::vector<uint16_t> elebuf;

  // Create two sets of slices one for the + and - viewing direction
  // for the vertex buffer.

  // Looking towards +X axis, in the YZ plane.
  createQuads(vbuf, min, max, numSlices.x, Axis::X);
  // Looking Towards -X axis, in the YZ plane.
  createQuads_Reversed(vbuf, min, max, numSlices.x, Axis::X);

  // Looking Towards +Y axis, in the XZ plane.
  createQuads(vbuf, min, max, numSlices.y, Axis::Y);
  // Looking Towards -Y axis, in the XZ plane.
  createQuads_Reversed(vbuf, min, max, numSlices.y, Axis::Y);

  // Looking Towards +Z axis, in the XY plane.
  createQuads(vbuf, min, max, numSlices.z, Axis::Z);
  // Looking Towards -Z axis, in the XY plane.
  createQuads_Reversed(vbuf, min, max, numSlices.z, Axis::Z);

  vao.addVbo(vbuf, VERTEX_COORD_ATTR, bd::VertexArrayObject::Usage::Static_Draw);


  // Along X, in the YZ plane.
  createQuads(texbuf, { 0, 0, 0 }, { 1, 1, 1 }, numSlices.x, Axis::X);
  createQuads_Reversed(texbuf, { 0, 0, 0 }, { 1, 1, 1 }, numSlices.x, Axis::X);

  // Along Y, in the XZ plane.
  createQuads(texbuf, { 0, 0, 0 }, { 1, 1, 1 }, numSlices.y, Axis::Y);
  createQuads_Reversed(texbuf, { 0, 0, 0 }, { 1, 1, 1 }, numSlices.y, Axis::Y);

  // Along Z, in the XY plane.
  createQuads(texbuf, { 0, 0, 0 }, { 1, 1, 1 }, numSlices.z, Axis::Z);
  createQuads_Reversed(texbuf, { 0, 0, 0 }, { 1, 1, 1 }, numSlices.z, Axis::Z);

  vao.addVbo(texbuf, VERTEX_COLOR_ATTR, bd::VertexArrayObject::Usage::Static_Draw);

  // Create element indexes just for x dimension right now
  createElementIdx(elebuf, numSlices.x );

  // Element index buffer
  vao.setIndexBuffer(elebuf.data(),
                     elebuf.size(),
                     bd::VertexArrayObject::Usage::Static_Draw);

}


void
createQuads(std::vector<glm::vec4> &quads,
            glm::vec3 const &min, glm::vec3 const &max,
            size_t numPlanes, Axis a)
{

  float delta{ 0 };

  switch (a) {
    case Axis::X:
    {
      delta = (max.x - min.x) / static_cast<float>(numPlanes);
      decrement_delta<float> dd(max.x, delta, min.x);
      while (dd.hasNext()) {
        float offset = dd();
        quads.push_back({ offset, min.y, max.z, 1 });   // ll
        quads.push_back({ offset, min.y, min.z, 1 });   // lr
        quads.push_back({ offset, max.y, max.z, 1 });   // ul
        quads.push_back({ offset, max.y, min.z, 1 });   // ur
      }
        break;
    }

    case Axis::Y:
    {
      delta = (max.y - min.y) / static_cast<float>(numPlanes);
      decrement_delta<float> dd(max.y, delta, min.y);
      while (dd.hasNext()) {
        float offset = dd();
        quads.push_back({ min.x, offset, max.z, 1 });   // ll
        quads.push_back({ max.x, offset, max.z, 1 });   // lr
        quads.push_back({ min.x, offset, min.z, 1 });   // ul
        quads.push_back({ max.x, offset, min.z, 1 });   // ur
      }
          break;
    }

    case Axis::Z:
    {
      delta = (max.z - min.z) / static_cast<float>(numPlanes);
      decrement_delta<float> dd(max.z, delta, min.z);
      while (dd.hasNext()) {
        float offset = dd();
        quads.push_back({ min.x, min.y, offset, 1 });   // ll
        quads.push_back({ max.x, min.y, offset, 1 });   // lr
        quads.push_back({ min.x, max.y, offset, 1 });   // ul
        quads.push_back({ max.x, max.y, offset, 1 });   // ur
      }
      break;
    }
      // default: break;
  }

}

void
createQuads_Reversed(std::vector<glm::vec4> &quads,
                     glm::vec3 const &min, glm::vec3 const &max,
                     size_t numPlanes, Axis a)
{
  float delta{ 0 };

  switch (a) {
    case Axis::X:  // -YZ
    {
      delta = (max.x - min.x) / static_cast<float>(numPlanes);
      accum_delta<float> ad(min.x, delta, max.x);
      while (ad.hasNext()) {
        float offset = ad();
        quads.push_back({ offset, min.y, min.z, 1 });   // ll
        quads.push_back({ offset, min.y, max.z, 1 });   // lr
        quads.push_back({ offset, max.y, min.z, 1 });   // ul
        quads.push_back({ offset, max.y, max.z, 1 });   // ur
      }
      break;
    }

    case Axis::Y:  // -XZ
    {
      delta = (max.y - min.y) / static_cast<float>(numPlanes);
      accum_delta<float> ad(min.y, delta, max.y);
      while (ad.hasNext()) {
        float offset = ad();
        quads.push_back({ min.x, offset, min.z, 1 });   // ll
        quads.push_back({ max.x, offset, min.z, 1 });   // lr
        quads.push_back({ min.x, offset, max.z, 1 });   // ul
        quads.push_back({ max.x, offset, max.z, 1 });   // ur
      }
      break;
    }

    case Axis::Z: {
      delta = (max.z - min.z) / static_cast<float>(numPlanes);
      accum_delta<float> ad(min.z, delta, max.z);
      while (ad.hasNext()) {
        float offset = ad();
        quads.push_back({ max.x, min.y, offset, 1 });   // ll
        quads.push_back({ min.x, min.y, offset, 1 });   // lr
        quads.push_back({ max.x, max.y, offset, 1 });   // ul
        quads.push_back({ min.x, max.y, offset, 1 });   // ur
      }
      break;
    }
  }

}

////////////////////////////////////////////////////////////////////////////////
void createElementIdx(std::vector<unsigned short> &elebuf, unsigned short numSlices)
{

//  size_t totalElems{ numQuads * 5 };  // 4 verts + 1 restart symbol per quad.

  elebuf.clear();
//  elebuf.reserve(totalElems);

  // Creates element indices just for X dimension becuase we are using same
  // number of slices for each dimension.
  for (unsigned short i{ 0 }; i < numSlices; ++i) {
    elebuf.push_back(0 + 4 * i);
    elebuf.push_back(1 + 4 * i);
    elebuf.push_back(2 + 4 * i);
    elebuf.push_back(3 + 4 * i);
    elebuf.push_back(0xFFFF);
  }

//  for (size_t i{ 0 }; i < numSlices.y; ++i) {
//    elebuf.push_back(0 + 4 * i);
//    elebuf.push_back(1 + 4 * i);
//    elebuf.push_back(2 + 4 * i);
//    elebuf.push_back(3 + 4 * i);
//    elebuf.push_back(0xFFFF);
//  }
//
//  for (size_t i{ 0 }; i < numSlices.z; ++i) {
//    elebuf.push_back(0 + 4 * i);
//    elebuf.push_back(1 + 4 * i);
//    elebuf.push_back(2 + 4 * i);
//    elebuf.push_back(3 + 4 * i);
//    elebuf.push_back(0xFFFF);
//  }
}


///////////////////////////////////////////////////////////////////////////////
void genAxisVao(bd::VertexArrayObject &vao)
{
  bd::Info() <<  "Generating axis vertex buffers.";

  using Axis = bd::CoordinateAxis;

  // vertex positions into attribute 0
  vao.addVbo((float *) (Axis::verts.data()),
             Axis::verts.size() * Axis::vert_element_size,
             Axis::vert_element_size,
             VERTEX_COORD_ATTR,
             bd::VertexArrayObject::Usage::Static_Draw); // attr 0

  // vertex colors into attribute 1
  vao.addVbo((float *) (Axis::colors.data()),
             Axis::colors.size()*3,
             3,   // 3 floats per color
             VERTEX_COLOR_ATTR,
             bd::VertexArrayObject::Usage::Static_Draw);  // attr 1
}


///////////////////////////////////////////////////////////////////////////////
void genBoxVao(bd::VertexArrayObject &vao)
{
  bd::Info() << "Generating bounding box vertex buffers.";

  // positions as vertex attribute 0
  vao.addVbo((float *) (bd::Box::vertices.data()),
             bd::Box::vertices.size()*bd::Box::vert_element_size,
             bd::Box::vert_element_size,
             VERTEX_COORD_ATTR,
             bd::VertexArrayObject::Usage::Static_Draw);

  // colors as vertex attribute 1
  vao.addVbo((float *) bd::Box::colors.data(),
             bd::Box::colors.size()*3,
             3,
             VERTEX_COLOR_ATTR,
             bd::VertexArrayObject::Usage::Static_Draw);

  vao.setIndexBuffer((unsigned short *) bd::Box::elements.data(),
                     bd::Box::elements.size(),
                     bd::VertexArrayObject::Usage::Static_Draw);

}




} // namespace subvol



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


