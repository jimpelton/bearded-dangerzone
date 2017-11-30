#include "create_vao.h"

#include "axis_enum.h"
#include "constants.h"

#include <bd/geo/axis.h>
#include <bd/log/logger.h>
#include <bd/geo/wireframebox.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <fstream>
#include <bd/volume/volume.h>
#include <bd/util/ordinal.h>

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

    struct BBox
    {
      BBox() : BBox({ 1.0f, 1.0f, 1.0f })
      {
      }

      BBox(glm::vec3 const& aspect)
      {
        verts[0] = VertexFormat(glm::vec4(glm::vec3(-0.5, -0.5, -0.5) * aspect, 1.0),
                                glm::vec3(0, 0, 0)); // left, bottom, back
        verts[1] = VertexFormat(glm::vec4(glm::vec3(0.5, -0.5, -0.5) * aspect, 1.0),
                                glm::vec3(1, 0, 0)); // right, bottom, back
        verts[2] = VertexFormat(glm::vec4(glm::vec3(0.5, 0.5, -0.5) * aspect, 1.0),
                                glm::vec3(1, 1, 0)); // right, top, back
        verts[3] = VertexFormat(glm::vec4(glm::vec3(-0.5, 0.5, -0.5) * aspect, 1.0),
                                glm::vec3(0, 1, 0)); // left, top, back

        verts[4] = VertexFormat(glm::vec4(glm::vec3(-0.5, -0.5, 0.5) * aspect, 1.0),
                                glm::vec3(0, 0, 1)); // left, bottom, front
        verts[5] = VertexFormat(glm::vec4(glm::vec3(0.5, -0.5, 0.5) * aspect, 1.0),
                                glm::vec3(1, 0, 1)); // right, bottom, front
        verts[6] = VertexFormat(glm::vec4(glm::vec3(0.5, 0.5, 0.5) * aspect, 1.0),
                                glm::vec3(1, 1, 1)); // right, top, front
        verts[7] = VertexFormat(glm::vec4(glm::vec3(-0.5, 0.5, 0.5) * aspect, 1.0),
                                glm::vec3(0, 1, 1)); // left, top, front
      }

      VertexFormat verts[8];
    }; // class BBox


    float
    getDelta(bd::Volume const& v,
             float samplingModifer,
             Axis a)
    {
      return 1.0f / (samplingModifer * v.worldDims()[bd::ordinal<Axis>(a)] * 1.414213562f); // 1.414213562f = sqrt(2)
    }


    VertexFormat
    interpolateVertex(VertexFormat const& v1,
                      VertexFormat const& v2,
                      float depth)
    {
      VertexFormat r;

      r.pos = (1.0f - depth) * v1.pos + depth * v2.pos;
      r.uv = (1.0f - depth) * v1.uv + depth * v2.uv;

      return r;
    }


  } // namespace


  /////////////////////////////////////////////////////////////////////////////////
  glm::u64vec3
  genQuadVao(bd::VertexArrayObject& vao,
             bd::Volume const& v,
             float smod)
  {

    std::vector<VertexFormat> vbuf;
    std::vector<uint16_t> elebuf;

    glm::u64vec3 sliceCounts;

    float delta{ getDelta(v, smod, Axis::X) };
    size_t numSlices{ static_cast<size_t>(std::floor(1.0f / delta)) };
    delta = 1.0f / (numSlices - 1);
    bd::Dbg() << "Slices on X: " << numSlices << " with delta: " << delta;
    createQuads(vbuf, numSlices, delta, false, Axis::X);
    createQuads(vbuf, numSlices, delta, true, Axis::X);
    createElementIdx(elebuf, numSlices);
    sliceCounts.x = numSlices;

    delta = getDelta(v, smod, Axis::Y);
    numSlices = static_cast<size_t>(std::floor(1.0f / delta));
    delta = 1.0f / (numSlices - 1);
    bd::Dbg() << "Slices on Y: " << numSlices << " with delta: " << delta;
    createQuads(vbuf, numSlices, delta, false, Axis::Y);
    createQuads(vbuf, numSlices, delta, true, Axis::Y);
    createElementIdx(elebuf, numSlices);
    sliceCounts.y = numSlices;

    delta = getDelta(v, smod, Axis::Z);
    numSlices = static_cast<size_t>(std::floor(1.0f / delta));
    delta = 1.0f / (numSlices - 1);
    bd::Dbg() << "Slices on Z: " << numSlices << " with delta: " << delta;
    createQuads(vbuf, numSlices, delta, false, Axis::Z);
    createQuads(vbuf, numSlices, delta, true, Axis::Z);
    createElementIdx(elebuf, numSlices);
    sliceCounts.z = numSlices;

    std::vector<glm::vec4> verts, texs;
    for (auto &vf : vbuf) {
      verts.push_back(vf.pos);
      texs.push_back({ vf.uv, 1.0 });
    }
    vao.addVbo(verts, VERTEX_COORD_ATTR, bd::VertexArrayObject::Usage::Static_Draw);
    vao.addVbo(texs, VERTEX_COLOR_ATTR, bd::VertexArrayObject::Usage::Static_Draw);
    // Element index buffer
    vao.setIndexBuffer(elebuf.data(),
                       elebuf.size(),
                       bd::VertexArrayObject::Usage::Static_Draw);

    return sliceCounts;

    //  glm::vec3 const mmin{ min };
    //  glm::vec3 const mmax{ max };
    // Create two sets of slices one for the + and - viewing direction
    // for the vertex buffer.
    // Looking towards +X axis, in the YZ plane.
    //  createQuads(vbuf, mmin, mmax, numSlices.x, Axis::X);
    // Looking Towards -X axis, in the YZ plane.
    //  createQuads_Reversed(vbuf, mmin, mmax, numSlices.x, Axis::X);
    // Looking Towards +Y axis, in the XZ plane.
    //  createQuads(vbuf, mmin, mmax, numSlices.y, Axis::Y);
    // Looking Towards -Y axis, in the XZ plane.
    //  createQuads_Reversed(vbuf, mmin, mmax, numSlices.y, Axis::Y);
    // Looking Towards +Z axis, in the XY plane.
    //  createQuads(vbuf, mmin, mmax, numSlices.z, Axis::Z);
    // Looking Towards -Z axis, in the XY plane.
    //  createQuads_Reversed(vbuf, mmin, mmax, numSlices.z, Axis::Z);
    //  vao.addVbo(vbuf, VERTEX_COORD_ATTR, bd::VertexArrayObject::Usage::Static_Draw);
    //  std::ofstream versFile("vertsfile.txt");
    //  size_t pp{ 0 };
    //  for (glm::vec4 const &v : vbuf) {
    //    if (pp == 4) {
    //     versFile << '\n';
    //      pp = 0;
    //    }
    //    versFile << glm::to_string(v) << ", ";
    //    pp++;
    //  }
    //  versFile.flush();
    //  versFile.close();
    //  glm::vec3 const texMin{ 0.0, 0.0, 0.0 };
    //  glm::vec3 const texMax{ 1.0, 1.0, 1.0 };
    // Along X, in the YZ plane.
    //  createQuads(texbuf, texMin, texMax, numSlices.x, Axis::X);
    //  createQuads_Reversed(texbuf, texMin, texMax, numSlices.x, Axis::X);
    // Along Y, in the XZ plane.
    //  createQuads(texbuf, texMin, texMax, numSlices.y, Axis::Y);
    //  createQuads_Reversed(texbuf, texMin, texMax, numSlices.y, Axis::Y);
    // Along Z, in the XY plane.
    //  createQuads(texbuf, texMin, texMax, numSlices.z, Axis::Z);
    //  createQuads_Reversed(texbuf, texMin, texMax, numSlices.z, Axis::Z);
    //  vao.addVbo(texbuf, VERTEX_COLOR_ATTR, bd::VertexArrayObject::Usage::Static_Draw);
    //  std::ofstream texsFile("texsfile.txt");
    //  pp = 0;
    //  for (glm::vec4 const &v : texbuf) {
    //    if (pp == 4) {
    //      texsFile << '\n';
    //      pp = 0;
    //    }
    //    texsFile << glm::to_string(v) << ", ";
    //    pp++;
    //  }
    //  texsFile.flush();
    //  texsFile.close();
  }


  void
  createQuads(std::vector<VertexFormat>& verts,
//              bd::Volume const& v,
              size_t numSlices,
              float delta,
              bool flip,
              Axis a)
  {
    float depth{ 0.0f };

    if (flip) {
      delta *= -1;
      depth = 1.0f;
    }

    BBox volBox;

    std::array<VertexFormat, 4> sliceVerts;
    switch (a) {
    case Axis::X:
      for (size_t x{ 0 }; x < numSlices; ++x) {
        sliceVerts[0] = interpolateVertex(volBox.verts[0], volBox.verts[1], depth);
        sliceVerts[1] = interpolateVertex(volBox.verts[3], volBox.verts[2], depth);
        sliceVerts[2] = interpolateVertex(volBox.verts[7], volBox.verts[6], depth);
        sliceVerts[3] = interpolateVertex(volBox.verts[4], volBox.verts[5], depth);
        for (auto &v : sliceVerts) {
          verts.push_back(v);
        }

        depth += delta;
      }
      break;
    case Axis::Y:
      for (size_t y{ 0 }; y < numSlices; ++y) {
        sliceVerts[0] = interpolateVertex(volBox.verts[0], volBox.verts[3], depth);
        sliceVerts[1] = interpolateVertex(volBox.verts[1], volBox.verts[2], depth);
        sliceVerts[2] = interpolateVertex(volBox.verts[4], volBox.verts[7], depth);
        sliceVerts[3] = interpolateVertex(volBox.verts[5], volBox.verts[6], depth);
        for (auto &v : sliceVerts) {
          verts.push_back(v);
        }

        depth += delta;
      }
      break;
    case Axis::Z:
      for (size_t z{ 0 }; z < numSlices; ++z) {
        sliceVerts[0] = interpolateVertex(volBox.verts[0], volBox.verts[4], depth);
        sliceVerts[1] = interpolateVertex(volBox.verts[1], volBox.verts[5], depth);
        sliceVerts[2] = interpolateVertex(volBox.verts[3], volBox.verts[7], depth);
        sliceVerts[3] = interpolateVertex(volBox.verts[2], volBox.verts[6], depth);
        for (auto &v : sliceVerts) {
          verts.push_back(v);
        }

        depth += delta;
      }
      break;
    }
  }

  //void
  //createQuads(std::vector<glm::vec4> &quads,
  //            glm::vec3 const &min, glm::vec3 const &max,
  //            size_t numPlanes, Axis a)
  //{
  //
  //  float delta{ 0 };
  //  size_t planes = 0;
  //  switch (a) {
  //    case Axis::X:
  //    {
  //      delta = (max.x - min.x) / static_cast<float>(numPlanes);
  //      float tmax = max.x - delta;
  //      while( planes < numPlanes - 1) {
  //        float const offset = tmax; // - delta;
  //        quads.push_back({ offset, min.y, max.z, 1 });   // ll
  //        quads.push_back({ offset, min.y, min.z, 1 });   // lr
  //        quads.push_back({ offset, max.y, max.z, 1 });   // ul
  //        quads.push_back({ offset, max.y, min.z, 1 });   // ur
  //        tmax = tmax - delta;
  //        planes += 1;
  //      }
  //        break;
  //    }
  //
  //    case Axis::Y:
  //    {
  //      delta = (max.y - min.y) / static_cast<float>(numPlanes);
  //      float  tmax = max.y - delta;
  //      while (planes < numPlanes - 1) {
  //        float const offset = tmax; // - delta;
  //        quads.push_back({ min.x, offset, max.z, 1 });   // ll
  //        quads.push_back({ max.x, offset, max.z, 1 });   // lr
  //        quads.push_back({ min.x, offset, min.z, 1 });   // ul
  //        quads.push_back({ max.x, offset, min.z, 1 });   // ur
  //        tmax = tmax - delta;
  //        planes += 1;
  //      }
  //          break;
  //    }
  //
  //    case Axis::Z:
  //    {
  //      delta = (max.z - min.z) / static_cast<float>(numPlanes);
  //      float tmax = max.z - delta;
  //      while (planes < numPlanes - 1) {
  //        float const offset = tmax;// - delta;
  //        quads.push_back({ min.x, min.y, offset, 1 });   // ll
  //        quads.push_back({ max.x, min.y, offset, 1 });   // lr
  //        quads.push_back({ min.x, max.y, offset, 1 });   // ul
  //        quads.push_back({ max.x, max.y, offset, 1 });   // ur
  //        tmax = tmax - delta;
  //        planes += 1;
  //      }
  //      break;
  //    }
  //      // default: break;
  //  }
  //
  //}
  //
  //void
  //createQuads_Reversed(std::vector<glm::vec4> &quads,
  //                     glm::vec3 const &min, glm::vec3 const &max,
  //                     size_t numPlanes, Axis a)
  //{
  //  float delta{ 0 };
  //  size_t planes{ 0 };
  //  switch (a) {
  //    case Axis::X:  // -YZ
  //    {
  //      delta = (max.x - min.x) / static_cast<float>(numPlanes);
  //      float tmin{ min.x + delta };
  //      while (planes < numPlanes - 1) {
  //        float const offset = tmin;
  //        quads.push_back({ offset, min.y, min.z, 1 });   // ll
  //        quads.push_back({ offset, min.y, max.z, 1 });   // lr
  //        quads.push_back({ offset, max.y, min.z, 1 });   // ul
  //        quads.push_back({ offset, max.y, max.z, 1 });   // ur
  //        planes += 1;
  //        tmin += delta;
  //      }
  //      break;
  //    }
  //
  //    case Axis::Y:  // -XZ
  //    {
  //      delta = (max.y - min.y) / static_cast<float>(numPlanes);
  //      float tmin{ min.y + delta };
  //      while (planes < numPlanes - 1) {
  //        float const offset = tmin;
  //        quads.push_back({ min.x, offset, min.z, 1 });   // ll
  //        quads.push_back({ max.x, offset, min.z, 1 });   // lr
  //        quads.push_back({ min.x, offset, max.z, 1 });   // ul
  //        quads.push_back({ max.x, offset, max.z, 1 });   // ur
  //        planes += 1;
  //        tmin += delta;
  //      }
  //      break;
  //    }
  //
  //    case Axis::Z: {
  //      delta = (max.z - min.z) / static_cast<float>(numPlanes);
  //      float tmin{ min.z + delta };
  //      while (planes < numPlanes - 1) {
  //        float const offset = tmin;
  //        quads.push_back({ max.x, min.y, offset, 1 });   // ll
  //        quads.push_back({ min.x, min.y, offset, 1 });   // lr
  //        quads.push_back({ max.x, max.y, offset, 1 });   // ul
  //        quads.push_back({ min.x, max.y, offset, 1 });   // ur
  //        planes += 1;
  //        tmin += delta;
  //      }
  //      break;
  //    }
  //  }
  //
  //}
  //
  ////////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////
  void createElementIdx(std::vector<unsigned short>& elebuf,
                        size_t numSlices)
  {
//    elebuf.clear();
    // Creates element indices just for X dimension becuase we are using same
    // number of slices for each dimension.
    for (unsigned short i{ 0 }; i < numSlices; ++i) {
      elebuf.push_back(0 + 4 * i);
      elebuf.push_back(1 + 4 * i);
      elebuf.push_back(2 + 4 * i);
      elebuf.push_back(3 + 4 * i);
      elebuf.push_back(0xFFFF);
    }
  }


  ///////////////////////////////////////////////////////////////////////////////
  void genAxisVao(bd::VertexArrayObject& vao)
  {
    bd::Info() << "Generating axis vertex buffers.";

    using BDAxis = bd::CoordinateAxis;

    // vertex positions into attribute 0
    vao.addVbo((float *)(BDAxis::verts.data()),
               BDAxis::verts.size() * BDAxis::vert_element_size,
               BDAxis::vert_element_size,
               VERTEX_COORD_ATTR,
               bd::VertexArrayObject::Usage::Static_Draw); // attr 0

    // vertex colors into attribute 1
    vao.addVbo((float *)(BDAxis::colors.data()),
               BDAxis::colors.size() * 3,
               3, // 3 floats per color
               VERTEX_COLOR_ATTR,
               bd::VertexArrayObject::Usage::Static_Draw); // attr 1
  }


  ///////////////////////////////////////////////////////////////////////////////
  void genBoxVao(bd::VertexArrayObject& vao)
  {
    bd::Info() << "Generating bounding box vertex buffers.";

    // positions as vertex attribute 0
    vao.addVbo((float *)(bd::WireframeBox::vertices.data()),
               bd::WireframeBox::vertices.size() * bd::WireframeBox::vert_element_size,
               bd::WireframeBox::vert_element_size,
               VERTEX_COORD_ATTR,
               bd::VertexArrayObject::Usage::Static_Draw);

    // colors as vertex attribute 1
    vao.addVbo((float *)bd::WireframeBox::colors.data(),
               bd::WireframeBox::colors.size() * 3,
               3,
               VERTEX_COLOR_ATTR,
               bd::VertexArrayObject::Usage::Static_Draw);

    vao.setIndexBuffer((unsigned short *)bd::WireframeBox::elements.data(),
                       bd::WireframeBox::elements.size(),
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
