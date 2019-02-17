#ifndef mesh_h__
#define mesh_h__

#include <bd/graphics/vertexarrayobject.h>

#include <vector>

namespace bd
{

class Mesh
{
public:
  Mesh();
  Mesh(const std::vector<float> &vertices,
      unsigned int elements_per_vert, const std::vector<unsigned short> &indices);
  ~Mesh();

  void init();

  void draw();

private:
  size_t m_verts_count;
  size_t m_indices_count;
  bd::VertexArrayObject m_vao;
  enum AttributeArray{POSITION=0, NORMAL=1, TEXCOORD=2};
};
} /* namespace bd */

#endif /* mesh_h__ */

