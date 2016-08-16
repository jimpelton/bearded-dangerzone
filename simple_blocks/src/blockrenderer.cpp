//
// Created by jim on 10/22/15.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "blockrenderer.h"
#include "colormap.h"
#include "constants.h"
#include "nvpm.h"

#include <bd/util/ordinal.h>
#include <glm/gtx/string_cast.hpp>
#include <bd/geo/axis.h>
//#include <bd/log/logger.h>

namespace subvol
{

BlockRenderer::BlockRenderer()
  : BlockRenderer(0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)
{}


////////////////////////////////////////////////////////////////////////////////
BlockRenderer::BlockRenderer(int numSlices,
                             std::shared_ptr<bd::ShaderProgram> volumeShader,
                             std::shared_ptr<bd::ShaderProgram> wireframeShader,
                             std::vector<bd::Block *> *blocks,
                             std::shared_ptr<bd::VertexArrayObject> blocksVAO,
                             std::shared_ptr<bd::VertexArrayObject> bboxVAO,
                             std::shared_ptr<bd::VertexArrayObject> axisVao)
  : Renderer()
    , m_numSlicesPerBlock{ numSlices }
    , m_tfuncScaleValue{ 1.0f }
    , m_drawNonEmptyBoundingBoxes{ false }
    , m_backgroundColor{ 0.0f }
    , m_volumeShader{ std::move(volumeShader) }
    , m_wireframeShader{ std::move(wireframeShader) }
    , m_blocks{ blocks }
    , m_colorMapTexture{ nullptr }
    , m_quadsVao{ std::move(blocksVAO) }
    , m_boxesVao{ std::move(bboxVAO) }
    , m_axisVao{ std::move(axisVao) }
{

}


////////////////////////////////////////////////////////////////////////////////
BlockRenderer::~BlockRenderer()
{}


////////////////////////////////////////////////////////////////////////////////
bool BlockRenderer::init()
{
  m_volumeShader->bind();
  m_volumeShader->setUniform(VOLUME_SAMPLER_UNIFORM_STR, BLOCK_TEXTURE_UNIT);
  m_volumeShader->setUniform(TRANSF_SAMPLER_UNIFORM_STR, TRANSF_TEXTURE_UNIT);
  m_volumeShader->setUniform(VOLUME_TRANSF_UNIFORM_STR, 1.0f);

  if (m_colorMapTexture == nullptr)
    setTFuncTexture(ColorMapManager::getMapTextureByName("RAINBOW"));

  return false;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::setTFuncTexture(bd::Texture const &tfunc)
{
  // bind tfunc to the transfer texture unit.
  tfunc.bind(TRANSF_TEXTURE_UNIT);
  m_colorMapTexture = &tfunc;
}


////////////////////////////////////////////////////////////////////////////////
void BlockRenderer::setTfuncScaleValue(float val)
{
  m_tfuncScaleValue = val;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::setBackgroundColor(const glm::vec3 &c)
{
  m_backgroundColor = c;
  glClearColor(c.r, c.g, c.b, 0.0f);
}


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::shouldDrawNonEmptyBoundingBoxes(bool b)
{
  m_drawNonEmptyBoundingBoxes = b;
}


////////////////////////////////////////////////////////////////////////////////
void BlockRenderer::drawNonEmptyBoundingBoxes()
{
//  m_wireframeShader->bind();
  m_boxesVao->bind();
  for (auto *b : *m_blocks) {
    setWorldMatrix(b->transform());
    m_wireframeShader->setUniform(WIREFRAME_MVP_MATRIX_UNIFORM_STR,
                                  getWorldViewProjectionMatrix());
    gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid *) 0));
    gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid *) (4 * sizeof(GLushort))));
    gl_check(glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid *) (8 * sizeof(GLushort))));
  }
}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::drawSlices(int baseVertex)
{
  // Begin NVPM work profiling
  perf_workBegin();
  gl_check(glDrawElementsBaseVertex(GL_TRIANGLE_STRIP,
                                    ELEMENTS_PER_QUAD * m_numSlicesPerBlock, // count
                                    GL_UNSIGNED_SHORT,                       // type
                                    0,                                       // element offset
                                    baseVertex));                            // vertex offset
  // End NVPM work profiling.
  perf_workEnd();

}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::drawNonEmptyBlocks_Forward()
{
  // Compute the SliceSet and offset into the vertex buffer of that slice set.
  GLint const baseVertex{ computeBaseVertexFromViewDir() };

  m_quadsVao->bind();
  m_volumeShader->bind();
  m_colorMapTexture->bind(TRANSF_TEXTURE_UNIT);
  m_volumeShader->setUniform(VOLUME_TRANSF_UNIFORM_STR, m_tfuncScaleValue);

  // Start an NVPM profiling frame
  perf_frameBegin();

  for (auto *b : *m_blocks) {
    setWorldMatrix(b->transform());
    b->texture().bind(BLOCK_TEXTURE_UNIT);
    m_volumeShader->setUniform(VOLUME_MVP_MATRIX_UNIFORM_STR, getWorldViewProjectionMatrix());
    drawSlices(baseVertex);
  }

  // End the NVPM profiling frame.
  perf_frameEnd();

}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::drawNonEmptyBlocks()
{
  // Sort the blocks by their distance from the camera.
  // The origin of each block is used.
  glm::vec3 const eye{ getCamera().getEye() };
  std::sort(m_blocks->begin(), m_blocks->end(),
            [&eye](bd::Block *a, bd::Block *b) {
              float a_dist = glm::distance(eye, a->origin());
              float b_dist = glm::distance(eye, b->origin());
              return a_dist > b_dist;
            });


  gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  setWorldMatrix(glm::mat4{ 1.0f });
  m_axisVao->bind();
  m_wireframeShader->bind();
  m_wireframeShader->setUniform("mvp", getWorldViewProjectionMatrix());
  gl_check(glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(bd::CoordinateAxis::verts.size())));
//  g_wireframeShader->unbind();
  m_axisVao->unbind();


  if (m_drawNonEmptyBoundingBoxes)
    drawNonEmptyBoundingBoxes();

  drawNonEmptyBlocks_Forward();
}


////////////////////////////////////////////////////////////////////////////////
int
BlockRenderer::computeBaseVertexFromViewDir()
{
  glm::vec3 const viewdir{ glm::normalize(getCamera().getLookAt() - getCamera().getEye()) };
  glm::vec3 const absViewDir{ glm::abs(viewdir) };

  SliceSet newSelected{ SliceSet::YZ };
  bool isPos{ viewdir.x > 0 };
  float longest{ absViewDir.x };

  if (absViewDir.y > longest) {
    newSelected = SliceSet::XZ;
    isPos = viewdir.y > 0;
    longest = absViewDir.y;
  }
  if (absViewDir.z > longest) {
    newSelected = SliceSet::XY;
    isPos = viewdir.z > 0;
  }

  // Compute base vertex VBO offset.
  int const elements_per_vertex{ 4 };
  int baseVertex{ 0 };
  switch (newSelected) {
    case SliceSet::YZ:
      if (isPos) {
        baseVertex = 0;
      } else {
        baseVertex = 1 * elements_per_vertex * m_numSlicesPerBlock;
      }
      break;
    case SliceSet::XZ:
      if (isPos) {
        baseVertex = 2 * elements_per_vertex * m_numSlicesPerBlock;
      } else {
        baseVertex = 3 * elements_per_vertex * m_numSlicesPerBlock;
      }
      break;

    case SliceSet::XY:
      if (isPos) {
        baseVertex = 4 * elements_per_vertex * m_numSlicesPerBlock;
      } else {
        baseVertex = 5 * elements_per_vertex * m_numSlicesPerBlock;
      }
      break;

    default:
      break;
  }

  if (newSelected != m_selectedSliceSet) {
    std::cout << " Switched slice set: " << (isPos ? '+' : '-') <<
              newSelected << " Base vertex: " << baseVertex << '\n';
  }

  m_selectedSliceSet = newSelected;

  return baseVertex;
}


} // namespace subvol
