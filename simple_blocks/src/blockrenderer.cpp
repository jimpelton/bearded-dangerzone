//
// Created by jim on 10/22/15.
//

#include "blockrenderer.h"
#include "nvpm.h"

#include <bd/log/gl_log.h>
#include <bd/geo/quad.h>
#include <bd/util/ordinal.h>


namespace {

  const int ELEMENTS_PER_QUAD{ 5 }; //< 5 elements = 4 verts + 1 restart symbol

  const glm::vec3 X_AXIS{ 1.0f, 0.0f, 0.0f };
  const glm::vec3 Y_AXIS{ 0.0f, 1.0f, 0.0f };
  const glm::vec3 Z_AXIS{ 0.0f, 0.0f, 1.0f };

  const int BLOCK_TEXTURE_UNIT = 0;
  const int TRANSF_TEXTURE_UNIT = 1;

//  const int BLOCK_TEXTURE_SAMPLER_UNIFORM = 0;
//  const int TRANSF_TEXTURE_SAMPLER_UNIFORM = 1;

  const char* VOLUME_MVP_MATRIX_UNIFORM_STR = "mvp";
  const char* VOLUME_TRANSF_UNIFORM_STR = "tfScalingVal";

  const char* WIREFRAME_MVP_MATRIX_UNIFORM_STR = "mvp";
}

BlockRenderer::BlockRenderer()
  : BlockRenderer(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) { }


////////////////////////////////////////////////////////////////////////////////
BlockRenderer::BlockRenderer
(
  std::shared_ptr<bd::ShaderProgram> volumeShader,
  std::shared_ptr<bd::ShaderProgram> wireframeShader,
  std::shared_ptr<bd::BlockCollection> blockCollection,
  std::shared_ptr<bd::Texture> tfuncTexture,
  std::shared_ptr<bd::VertexArrayObject> blocksVAO,
  std::shared_ptr<bd::VertexArrayObject> bboxVAO
) : m_volumeShader   { volumeShader }
  , m_wireframeShader{ wireframeShader }
  , m_blockCollection{ blockCollection }
  , m_tfuncTexture   { tfuncTexture }
  , m_quadsVao       { blocksVAO }
  , m_boxesVao       { bboxVAO }
{ }


////////////////////////////////////////////////////////////////////////////////
BlockRenderer::~BlockRenderer() { }


////////////////////////////////////////////////////////////////////////////////
bool BlockRenderer::init() {
//  m_quadsVao.create();
//  genQuadVao(m_quadsVao, {-0.5f,-0.5f,-0.5f}, {0.5f, 0.5f, 0.5f},
//             {m_numSlicesPerBlock, m_numSlicesPerBlock, m_numSlicesPerBlock});
  m_volumeShader->setUniform("volume_sampler", BLOCK_TEXTURE_UNIT);
  m_volumeShader->setUniform("tf_sampler", TRANSF_TEXTURE_UNIT);
  return false;
}


////////////////////////////////////////////////////////////////////////////////
void BlockRenderer::setTfuncScaleValue(float val) {
  m_tfuncScaleValue = val;
}


////////////////////////////////////////////////////////////////////////////////
void BlockRenderer::setViewMatrix(const glm::mat4 &viewMatrix) {
  m_viewMatrix = viewMatrix;
}


////////////////////////////////////////////////////////////////////////////////
void BlockRenderer::setNumSlices(const int n) {
  m_numSlicesPerBlock = n;
}


////////////////////////////////////////////////////////////////////////////////
void BlockRenderer::drawNonEmptyBoundingBoxes() {
  for (auto *b : m_blockCollection->nonEmptyBlocks()) {
    glm::mat4 mmvp = m_viewMatrix * b->transform().matrix();
    m_wireframeShader->setUniform(WIREFRAME_MVP_MATRIX_UNIFORM_STR, mmvp);

    gl_check(glDrawElements(GL_LINE_LOOP,
                            4,
                            GL_UNSIGNED_SHORT,
                            (GLvoid *) 0));

    gl_check(glDrawElements(GL_LINE_LOOP,
                            4,
                            GL_UNSIGNED_SHORT,
                            (GLvoid *) (4 * sizeof(GLushort))));

    gl_check(glDrawElements(GL_LINES,
                            8,
                            GL_UNSIGNED_SHORT,
                            (GLvoid *) (8 * sizeof(GLushort))));
  }
}


////////////////////////////////////////////////////////////////////////////////
void BlockRenderer::drawSlices(int baseVertex) {
  gl_check(glDisable(GL_DEPTH_TEST));

  perf_workBegin();
  gl_check(glDrawElementsBaseVertex(GL_TRIANGLE_STRIP,
                                    ELEMENTS_PER_QUAD * m_numSlicesPerBlock,
                                    GL_UNSIGNED_SHORT,
                                    0,
                                    baseVertex));
  perf_workEnd();

  gl_check(glEnable(GL_DEPTH_TEST));
}


////////////////////////////////////////////////////////////////////////////////
void BlockRenderer::drawNonEmptyBlocks_Forward() {

  //glm::vec4 viewdir{ glm::normalize(g_camera.getViewMatrix()[2]) };
  //GLint baseVertex{ computeBaseVertexFromSliceSet(g_selectedSliceSet) };
  GLint baseVertex{ 0 };

  // begin NVPM profiling
  perf_frameBegin();

  for (auto *b : m_blockCollection->nonEmptyBlocks()) {
    b->texture().bind(BLOCK_TEXTURE_UNIT);
    glm::mat4 wmvp = m_viewMatrix * b->transform().matrix();
    m_volumeShader->setUniform(VOLUME_MVP_MATRIX_UNIFORM_STR, wmvp);
    m_volumeShader->setUniform(VOLUME_TRANSF_UNIFORM_STR, m_tfuncScaleValue);
    drawSlices(baseVertex);
  }

  // end NVPM profiling
  perf_frameEnd();

}


////////////////////////////////////////////////////////////////////////////////
void BlockRenderer::drawNonEmptyBlocks() {

  //TODO: sort quads farthest to nearest.
  m_quadsVao->bind();
  m_volumeShader->bind();
  m_tfuncTexture->bind(TRANSF_TEXTURE_UNIT);
  drawNonEmptyBlocks_Forward();

}


////////////////////////////////////////////////////////////////////////////////
int BlockRenderer::computeBaseVertexFromViewDir(const glm::vec3 &viewdir) {
  glm::vec3 absViewDir{ glm::abs(viewdir) };

//  bool isNeg{ viewdir.x < 0 };
  SliceSet newSelected = SliceSet::YZ;
  float longest{ absViewDir.x };

  if (absViewDir.y > longest) {
//    isNeg = viewdir.y < 0;
    newSelected = SliceSet::XZ;
    longest = absViewDir.y;
  }
  if (absViewDir.z > longest) {
//    isNeg = viewdir.z < 0;
    newSelected = SliceSet::XY;
  }

  // Compute base vertex VBO offset.
  int baseVertex{ 0 };
  switch (newSelected) {
//  case SliceSet::XY:
//    baseVertex = 0;
//    break;
    case SliceSet::XZ:
      baseVertex = bd::Quad::vert_element_size * m_numSlicesPerBlock;
      break;
    case SliceSet::YZ:
      baseVertex = 2 * bd::Quad::vert_element_size * m_numSlicesPerBlock;
      break;
    default:
      break;
  }

  if (newSelected != m_selectedSliceSet) {
    std::cout << "Switched slice set: " << /* (isNeg ? '-' : '+') << */
        newSelected << '\n';
  }

  m_selectedSliceSet = newSelected;

  return baseVertex;
}



