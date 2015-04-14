
#ifndef geometry_h__
#define geometry_h__

#include <glm/glm.hpp>

#include <vector>

void makeBlockSlices(int numslices,
                     std::vector<glm::vec4>& vertices,
                     std::vector<unsigned short>& indexes);

//void initAxisVbos(unsigned int vaoId, unsigned int axis_vboId[2]);

#endif // geometry_h__