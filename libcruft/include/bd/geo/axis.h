#ifndef bd_axis_h__
#define bd_axis_h__


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "bd/graphics/vertexarrayobject.h"
#include "bd/graphics/drawable.h"

#include <array>

namespace bd
{
class CoordinateAxis : public bd::Drawable

{
public:
  static const std::array<glm::vec4, 6> verts;
  static const std::array<glm::vec3, 6> colors;
  static const std::array<unsigned short, 6> elements;
  static const unsigned int vert_element_size = 4;

  CoordinateAxis();

  virtual ~CoordinateAxis() { }

  void draw() override;

private:
  VertexArrayObject m_vao;

};
} // namespace bd

#endif // !bd_axis_h__


