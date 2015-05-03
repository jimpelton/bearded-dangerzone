#ifndef create_vao_h__
#define create_vao_h__

#include <glm/fwd.hpp>

#include <vector>


glm::u16vec4 sliceIndexToElements(unsigned idx);

float sliceIndexToWorldPos(unsigned idx, float start, float delta);

float start(size_t num_slices, float min, float max);

float delta(size_t num_slices, float min, float max);

void create_verts_xy(size_t num_slices, std::vector<glm::vec4> &vbuf); 

void  create_texbuf_xy(size_t num_slices, std::vector<glm::vec4> &texbuf);

//    std::vector<glm::vec3> &teccoords, std::vector<glm::u16vec4> &elebuf, 
//    int eleIdxContinuation);
//
//int create_xz(size_t num_slices, std::vector<glm::vec4> &slices,
//    std::vector<glm::vec3> &teccoords, std::vector<glm::u16vec4> &elebuf,
//    int eleIdxContinuation);
//
//int create_yz(size_t num_slices, std::vector<glm::vec4> &slices,
//    std::vector<glm::vec3> &teccoords, std::vector<glm::u16vec4> &elebuf,
//    int eleIdxContinuation);

#endif // !create_vao_h__