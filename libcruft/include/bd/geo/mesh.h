#ifndef mesh_h__
#define mesh_h__

#include <vector>

namespace bd
{
struct MeshData
{
  std::vector<float> m_positions;
  std::vector<float> m_normals;
  std::vector<float> m_texcoords;
  std::vector<unsigned int> m_indices;
  std::vector<int> m_material_ids; // per-mesh material ID
};

class Mesh
{
public:
  Mesh(MeshData const& data);
  ~Mesh();

  void init();

private:
  MeshData m_data;
};
} /* namespace bd */

#endif /* mesh_h__ */

