//
// Created by jim on 10/22/15.
//

#include "volumerenderer.h"

VolumeRenderer::VolumeRenderer() { }

VolumeRenderer::~VolumeRenderer() { }

void VolumeRenderer::renderSingleFrame() { }

GLFWwindow* VolumeRenderer::init() { }


////////////////////////////////////////////////////////////////////////////////
void VolumeRenderer::setRotation(const glm::vec2 &dr) {
  glm::quat rotX = glm::angleAxis<float>(
      glm::radians(-dr.y) * m_mouseSpeed,
      glm::vec3(1, 0, 0)
  );

  glm::quat rotY = glm::angleAxis<float>(
      glm::radians(dr.x) * m_mouseSpeed,
      glm::vec3(0, 1, 0)
  );

  m_view.rotate(rotX * rotY);
}
