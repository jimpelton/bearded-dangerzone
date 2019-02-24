#ifndef mesh_h__
#define mesh_h__

#include <bd/graphics/vertexarrayobject.h>
#include <bd/graphics/drawable.h>

#include <vector>

namespace bd
{

class Mesh : public bd::Drawable
{
public:
  Mesh();
  Mesh(const std::vector<float> &vertices,
      unsigned int elements_per_vert, const std::vector<unsigned short> &indices);
  ~Mesh();

  void init();

  void draw() override;

private:
  size_t m_verts_count;
  size_t m_indices_count;
  VertexArrayObject m_vao;
};
} /* namespace bd */

#endif /* mesh_h__ */

