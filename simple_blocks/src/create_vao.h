#ifndef create_vao_h__
#define create_vao_h__

#include "axis_enum.h"

#include <bd/graphics/vertexarrayobject.h>

#include <glm/fwd.hpp>

#include <vector>
#include <type_traits>

//namespace vert
//{

///////////////////////////////////////////////////////////////////////////
/// \brief Generates (max-start)/d Ts.
///        Returns values in sequence from [start .. max), non-inclusive on the right.
///////////////////////////////////////////////////////////////////////////
template<typename T,
        typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
class accum_delta {
public:
    accum_delta(T start, T d, T max)
        : m_delta{ d }
        , m_next{ start }
        , m_max{ max }
    { }

    T operator()() {
        T r = m_next;
        m_next += m_delta;

        return r;
    }

    /// \return True until max has been returned by operator().
    bool hasNext() {
        return m_next < m_max;
    }

private:
    T m_delta;  
    T m_next;   
    T m_max;    
};


void genQuadVao(bd::VertexArrayObject &vao, const glm::vec3 &min, const glm::vec3 &max,
                const glm::u64vec3 &numSlices);
////////////////////////////////////////////////////////////////////////////////
/// \brief Returns verts for quads that span region with
///        diagonal running from \c min to \c max
///
/// \param quads[out] std::vector of vec4 that verts are returned in.
/// \param min[in] Minimum corner of region.
/// \param max[in] Maximum corner of region.
/// \param numPlanes[in] Number of planes to pack into region.
/// \param a[in] Axis perpendicular to the planes.
////////////////////////////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////////////////////////
/// \brief Returns the 4 element array indices for the vertex of slice \c idx. 
///////////////////////////////////////////////////////////////////////////////
//glm::u16vec4 sliceIndexToElements(uint16_t idx);
//
//float sliceIndexToWorldPos(unsigned idx, float start, float delta);
//
//float start(size_t num_slices, float min, float max);
//
/////////////////////////////////////////////////////////////////////////////////
///// \brief Returns the spacing between \c num_slices things that fit evenly
/////  between min,max
/////////////////////////////////////////////////////////////////////////////////
//float delta(size_t num_slices, float min, float max);
//
//
//void create_verts_xy(size_t num_slices, std::vector<glm::vec4> &vbuf);
//void create_verts_xz(size_t numSlices, std::vector<glm::vec4> &vbuf);
//void create_verts_yz(size_t numSlices, std::vector<glm::vec4> &vbuf);
//
//void create_texbuf_xy(size_t num_slices, std::vector<glm::vec4> &texbuf);
//void create_texbuf_xz(size_t numSlices, std::vector<glm::vec4> &texbuf);
//void create_texbuf_yz(size_t numSlices, std::vector<glm::vec4> &texbuf);
//
//void create_elementIndices(size_t numSlices, std::vector<uint16_t> &elebuf);
//
//void create_elementIndicesReversed(size_t numSlices, std::vector<uint16_t>::iterator &start,
//    std::vector<uint16_t>::iterator &end);

//} // namepace vert

#endif // !create_vao_h__
