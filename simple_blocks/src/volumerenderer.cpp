//
// Created by jim on 10/22/15.
//

#include <bd/log/gl_log.h>
#include <bd/geo/quad.h>

#include "volumerenderer.h"
#include "nvpm.h"
#include "create_vao.h"

namespace {
  const int ELEMENTS_PER_QUAD{ 5 };

  const glm::vec3 X_AXIS{ 1.0f, 0.0f, 0.0f };
  const glm::vec3 Y_AXIS{ 0.0f, 1.0f, 0.0f };
  const glm::vec3 Z_AXIS{ 0.0f, 0.0f, 1.0f };

  const std::string VERTSHADER_PATH{
          "shaders/vert_vertexcolor_passthrough.glsl"
  };

  const std::string FRAGSHADER_PATH{
          "shaders/frag_volumesampler_noshading.glsl"
  };
}



////////////////////////////////////////////////////////////////////////////////
VolumeRenderer::VolumeRenderer() { }


////////////////////////////////////////////////////////////////////////////////
VolumeRenderer::~VolumeRenderer() { }


////////////////////////////////////////////////////////////////////////////////
void VolumeRenderer::renderSingleFrame() {
  m_quadsVao.bind();
  drawNonEmptyBlocks();
  m_quadsVao.unbind();
}


//////////////////////////////////////////////////////////////////////////////////
bool VolumeRenderer::init() {
  m_quadsVao.create();
  genQuadVao(m_quadsVao, {-0.5f,-0.5f,-0.5f}, {0.5f, 0.5f, 0.5f},
             {m_numSlices, m_numSlices, m_numSlices});
}


////////////////////////////////////////////////////////////////////////////////
void VolumeRenderer::setTfuncScaleValue(float val) {
  m_tfuncScaleValue = val;
}


////////////////////////////////////////////////////////////////////////////////
void VolumeRenderer::setViewMatrix(const glm::mat4 &viewMatrix) {
  m_viewMatrix = viewMatrix;
}


////////////////////////////////////////////////////////////////////////////////
void VolumeRenderer::setNumSlices(const int n) {
  m_numSlices = n;
}


////////////////////////////////////////////////////////////////////////////////
void VolumeRenderer::drawSlices(int baseVertex) {
  gl_check(glDisable(GL_DEPTH_TEST));

  perf_workBegin();
  gl_check(glDrawElementsBaseVertex(GL_TRIANGLE_STRIP,
                                    ELEMENTS_PER_QUAD * m_numSlices,
                                    GL_UNSIGNED_SHORT,
                                    0,
                                    baseVertex));
  perf_workEnd();

  gl_check(glEnable(GL_DEPTH_TEST));
}


////////////////////////////////////////////////////////////////////////////////
void VolumeRenderer::drawNonEmptyBlocks_Forward() {

  //glm::vec4 viewdir{ glm::normalize(g_camera.getViewMatrix()[2]) };
  //GLint baseVertex{ computeBaseVertexFromSliceSet(g_selectedSliceSet) };
  GLint baseVertex{ 0 };

  // begin NVPM profiling
  perf_frameBegin();

  for (auto *b : m_blockCollection.nonEmptyBlocks()) {
    b->texture().bind(0);
    glm::mat4 wmvp = m_viewMatrix * b->transform().matrix();
    m_volumeShader.setUniform("mvp", wmvp);
    m_volumeShader.setUniform("tfScalingVal", m_tfuncScaleValue);
    drawSlices(baseVertex);
  }

  // end NVPM profiling
  perf_frameEnd();

}


////////////////////////////////////////////////////////////////////////////////
void VolumeRenderer::drawNonEmptyBlocks() {

//  if (g_toggleWireFrame) {
//    gl_check(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
//  }

  //TODO: sort quads farthest to nearest.
  m_volumeShader.bind();
  m_tfuncTexture.bind(1);
  drawNonEmptyBlocks_Forward();

//  if (g_toggleWireFrame) {
//    gl_check(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
//  }

}


////////////////////////////////////////////////////////////////////////////////
int VolumeRenderer::computeBaseVertexFromViewDir(const glm::vec3 &viewdir) {
  glm::vec3 absViewDir{ glm::abs(viewdir) };

//  bool isNeg{ viewdir.x < 0 };
  SliceSet selected = SliceSet::YZ;
  float longest{ absViewDir.x };

  if (absViewDir.y > longest) {
//    isNeg = viewdir.y < 0;
    selected = SliceSet::XZ;
    longest = absViewDir.y;
  }
  if (absViewDir.z > longest) {
//    isNeg = viewdir.z < 0;
    selected = SliceSet::XY;
  }

  // Compute base vertex offset.
  GLint baseVertex{ 0 };
  switch (selected) {
//  case SliceSet::XY:
//    baseVertex = 0;
//    break;
    case SliceSet::XZ:
      baseVertex = bd::Quad::vert_element_size * m_numSlices;
      break;
    case SliceSet::YZ:
      baseVertex = 2 * bd::Quad::vert_element_size * m_numSlices;
      break;
    default:
      break;
  }

  if (selected != m_selectedSliceSet) {
    std::cout << "Switched slice set: " << /* (isNeg ? '-' : '+') << */
    m_selectedSliceSet << '\n';
  }

  m_selectedSliceSet = selected;

  return baseVertex;
}



