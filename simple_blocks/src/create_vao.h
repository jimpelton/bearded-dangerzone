#ifndef create_vao_h__
#define create_vao_h__

#include "axis_enum.h"

#include <glm/fwd.hpp>

#include <vector>

//namespace vert
//{

///////////////////////////////////////////////////////////////////////////
/// \brief Generator that produces Ts spaced by \c delta amount and
/// starting at \c start. Returns [start .. max), inclusive no both ends.
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
    T m_delta;  // delta
    T m_next;   // value
    T m_max;    //max
};

void createQuads_X(std::vector<glm::vec4> &quads, const glm::vec3 &min,
                   const glm::vec3 &max, accum_delta<float> &nextDelta);


void createQuads_Y(std::vector<glm::vec4> &quads, const glm::vec3 &min,
                   const glm::vec3 &max, accum_delta<float> &nextDelta);


void createQuads_Z(std::vector<glm::vec4> &quads, const glm::vec3 &min,
                   const glm::vec3 &max, accum_delta<float> &nextDelta);


void createQuads(std::vector<glm::vec4> &quads, const glm::vec3 &min,
                 const glm::vec3 &max, size_t numPlanes, Axis a);


void createElementIdx(std::vector<unsigned short> &elebuf );





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