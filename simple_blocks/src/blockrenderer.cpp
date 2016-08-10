//
// Created by jim on 10/22/15.
//

#include "blockrenderer.h"
#include "colormap.h"
#include "constants.h"
#include "nvpm.h"

#include <bd/geo/quad.h>
#include <bd/util/ordinal.h>
#include <glm/gtx/string_cast.hpp>
//#include <bd/log/logger.h>

namespace subvol
{

BlockRenderer::BlockRenderer()
  : BlockRenderer(0, nullptr, nullptr, nullptr, nullptr, nullptr)
{}


////////////////////////////////////////////////////////////////////////////////
BlockRenderer::BlockRenderer(int numSlices,
                             std::shared_ptr<bd::ShaderProgram> volumeShader,
                             std::shared_ptr<bd::ShaderProgram> wireframeShader,
                             std::vector<bd::Block *> *blocks,
                             std::shared_ptr<bd::VertexArrayObject> blocksVAO,
                             std::shared_ptr<bd::VertexArrayObject> bboxVAO)
  : Renderer()
    , m_numSlicesPerBlock{ numSlices }
    , m_tfuncScaleValue{ 1.0f }
    , m_drawNonEmptyBoundingBoxes{ false }
    , m_backgroundColor{ 0.0f }
    , m_volumeShader{ std::move(volumeShader) }
    , m_wireframeShader{ std::move(wireframeShader) }
    , m_blocks{ blocks }
    , m_colorMapTexture{ nullptr }
    , m_quadsVao{ blocksVAO }
    , m_boxesVao{ bboxVAO }
{

}


////////////////////////////////////////////////////////////////////////////////
BlockRenderer::~BlockRenderer()
{}


////////////////////////////////////////////////////////////////////////////////
bool BlockRenderer::init()
{
  m_volumeShader->bind();
  setTFuncTexture(*ColorMap::getDefaultMapTexture("RAINBOW"));
  m_volumeShader->setUniform(VOLUME_SAMPLER_UNIFORM_STR, BLOCK_TEXTURE_UNIT);
  m_volumeShader->setUniform(TRANSF_SAMPLER_UNIFORM_STR, TRANSF_TEXTURE_UNIT);
  m_volumeShader->setUniform(VOLUME_TRANSF_UNIFORM_STR, 1.0f);

  return false;
}


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




////////////////////////////////////////////////////////////////////////////////
//void BlockRenderer::setNumSlices(const int n) {
//  m_numSlicesPerBlock = n;
//}


void
BlockRenderer::setBackgroundColor(const glm::vec3 &c)
{
  m_backgroundColor = c;
  glClearColor(c.r, c.g, c.b, 0.0f);
}


////////////////////////////////////////////////////////////////////////////////
void BlockRenderer::drawNonEmptyBoundingBoxes()
{

  m_wireframeShader->bind();
  m_boxesVao->bind();

  for (auto *b : *m_blocks) {

    setWorldMatrix(b->transform());

    m_wireframeShader->setUniform(WIREFRAME_MVP_MATRIX_UNIFORM_STR,
                                  getWorldViewProjectionMatrix());

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
void
BlockRenderer::drawSlices(int baseVertex)
{
  m_volumeShader->setUniform(VOLUME_MVP_MATRIX_UNIFORM_STR, getWorldViewProjectionMatrix());
  m_volumeShader->setUniform(VOLUME_TRANSF_UNIFORM_STR, m_tfuncScaleValue);
  // Begin NVPM work profiling
  perf_workBegin();
  gl_check(glDrawElementsBaseVertex(GL_TRIANGLE_STRIP,
                                    ELEMENTS_PER_QUAD * m_numSlicesPerBlock, // count
                                    GL_UNSIGNED_SHORT,                       // type
                                    0,
                                    baseVertex));
  // End NVPM work profiling.
  perf_workEnd();

//  gl_check(glEnable(GL_DEPTH_TEST));
}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::drawNonEmptyBlocks_Forward()
{
  // Sort the blocks by their distance from the camera.
  // The origin of each block is used.
  glm::vec3 const eye{ getCamera().getEye() };
  std::sort(m_blocks->begin(), m_blocks->end(),
            [&eye](bd::Block *a, bd::Block *b) {
              float a_dist = glm::distance(eye, a->origin());
              float b_dist = glm::distance(eye, b->origin());
              return a_dist < b_dist;
            });

  if (m_drawNonEmptyBoundingBoxes) {
    drawNonEmptyBoundingBoxes();
  }
  // Compute the SliceSet and offset into the vertex buffer of that slice set.
  GLint const baseVertex{ computeBaseVertexFromViewDir() };

  // Start an NVPM profiling frame
  perf_frameBegin();

  for (auto *b : *m_blocks) {

    setWorldMatrix(b->transform());
    b->texture().bind(BLOCK_TEXTURE_UNIT);


    drawSlices(baseVertex);

  }

  // End the NVPM profiling frame.
  perf_frameEnd();

}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::drawNonEmptyBlocks()
{
  m_quadsVao->bind();
  m_volumeShader->bind();
  m_colorMapTexture->bind(TRANSF_TEXTURE_UNIT);
  drawNonEmptyBlocks_Forward();
}


////////////////////////////////////////////////////////////////////////////////
int
BlockRenderer::computeBaseVertexFromViewDir()
{
  glm::vec3 const viewdir{ glm::normalize(getCamera().getLookAt() - getCamera().getEye()) };
  glm::vec3 const absViewDir{ glm::abs(viewdir) };

  bool isPos{ viewdir.x > 0 };
  SliceSet newSelected{ SliceSet::YZ };
  float longest{ absViewDir.x };

  if (absViewDir.y > longest) {
    isPos = viewdir.y > 0;
    newSelected = SliceSet::XZ;
    longest = absViewDir.y;
  }
  if (absViewDir.z > longest) {
    isPos = viewdir.z > 0;
    newSelected = SliceSet::XY;
  }

  // Compute base vertex VBO offset.
  int baseVertex{ 0 };
  switch (newSelected) {
    case SliceSet::YZ:
      if (isPos) {
        baseVertex = 0;                                                     // works
      } else {
//        baseVertex = 1 * 5 * m_numSlicesPerBlock;
        baseVertex = 1 * bd::Quad::vert_element_size * m_numSlicesPerBlock;
      }
      break;
    case SliceSet::XZ:
      if (isPos) {
//        baseVertex = 2 * 5 * m_numSlicesPerBlock;
        baseVertex = 2 * bd::Quad::vert_element_size * m_numSlicesPerBlock;
      } else {
//        baseVertex = 3 * 5 * m_numSlicesPerBlock;
        baseVertex = 3 * bd::Quad::vert_element_size * m_numSlicesPerBlock;  // works
      }
      break;

    case SliceSet::XY:
      if (isPos) {
//        baseVertex = 4 * 5 * m_numSlicesPerBlock;
        baseVertex = 4 * bd::Quad::vert_element_size * m_numSlicesPerBlock;  // works
      } else {
//        baseVertex = 5 * 5 * m_numSlicesPerBlock;
        baseVertex = 5 * bd::Quad::vert_element_size * m_numSlicesPerBlock;
      }
      break;

//    default:
//      break;
  }

  if (newSelected != m_selectedSliceSet) {
    std::cout << "Switched slice set: " << (isPos ? '+' : '-') <<
              newSelected << " Base vertex: " << baseVertex << '\n';
  }

  m_selectedSliceSet = newSelected;

  return baseVertex;
}

//void
//BlockRenderer::setInitialGLState()
//{
//  bd::Info() << "Initializing gl state.";
//  gl_check(glClearColor(0.15f, 0.15f, 0.15f, 0.0f));
//
////  gl_check(glEnable(GL_CULL_FACE));
////  gl_check(glCullFace(GL_FRONT));
//  gl_check(glDisable(GL_CULL_FACE));
//
//  gl_check(glEnable(GL_DEPTH_TEST));
//  gl_check(glDepthFunc(GL_LESS));
//
//  gl_check(glEnable(GL_BLEND));
//  gl_check(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
//
//  gl_check(glEnable(GL_PRIMITIVE_RESTART));
//  gl_check(glPrimitiveRestartIndex(0xFFFF));
//}

} // namespace subvol
