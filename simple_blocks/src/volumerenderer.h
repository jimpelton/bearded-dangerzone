//
// Created by jim on 10/22/15.
//

#ifndef volumerenderer_h__
#define volumerenderer_h__

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/fwd.hpp>

#include <bd/graphics/view.h>


class VolumeRenderer {

public:
  VolumeRenderer();
  virtual ~VolumeRenderer();

  virtual void renderSingleFrame();

  GLFWwindow *init();


private:
  void setRotation(const glm::vec2 &dR);
  
  bd::View m_view;

  int m_mouseSpeed;  ///< Scalar mouse sensitivity.
  int m_fov_deg;     ///< Field of view, degrees.





};


#endif //volumerenderer_h__
