//
// Created by jim on 10/22/15.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "blockrenderer.h"
#include "colormap.h"
#include "constants.h"
#include "messages/messagebroker.h"

#include <bd/util/ordinal.h>
#include <bd/geo/axis.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>
#include <bd/log/gl_log.h>

#ifdef USE_NV_TOOLS
#include <nvToolsExt.h>
#endif

namespace subvol
{

BlockRenderer::BlockRenderer()
    : BlockRenderer({ }, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)
{
}


////////////////////////////////////////////////////////////////////////////////
BlockRenderer::BlockRenderer(glm::u64vec3 numSlices,
                             std::shared_ptr<bd::ShaderProgram> volumeShader,
                             std::shared_ptr<bd::ShaderProgram> volumeShaderLighting,
                             std::shared_ptr<bd::ShaderProgram> wireframeShader,
                             std::shared_ptr<BlockCollection> blockCollection,
                             std::shared_ptr<bd::VertexArrayObject> blocksVAO,
                             std::shared_ptr<bd::VertexArrayObject> bboxVAO,
                             std::shared_ptr<bd::VertexArrayObject> axisVao)
    : Renderer()
    , Recipient("BlockRenderer")

    , m_numSlicesPerBlock{ numSlices }
    , m_tfuncScaleValue{ 1.0f }
    , m_drawNonEmptyBoundingBoxes{ false }
    , m_drawNonEmptySlices{ true }
    , m_shouldUseLighting{ false }
    , m_backgroundColor{ 0.0f }

    , m_colorMapTexture{ nullptr }

    , m_currentShader{ nullptr }
    , m_volumeShader{ std::move(volumeShader) }
    , m_volumeShaderLighting{ std::move(volumeShaderLighting) }
    , m_wireframeShader{ std::move(wireframeShader) }

    , m_quadsVao{ std::move(blocksVAO) }
    , m_boxesVao{ std::move(bboxVAO) }
    , m_axisVao{ std::move(axisVao) }

    , m_collection{ std::move(blockCollection) }
    , m_nonEmptyBlocks{ nullptr }
    , m_blocks{ nullptr }
{
  m_blocks = &( m_collection->getBlocks());
  m_nonEmptyBlocks = &( m_collection->getNonEmptyBlocks());
  init();
}


////////////////////////////////////////////////////////////////////////////////
BlockRenderer::~BlockRenderer()
{
}


////////////////////////////////////////////////////////////////////////////////
bool
BlockRenderer::init()
{
  Broker::subscribeRecipient(this);

  m_volumeShader->bind();
  m_volumeShader->setUniform(VOLUME_SAMPLER_UNIFORM_STR, BLOCK_TEXTURE_UNIT);
  m_volumeShader->setUniform(TRANSF_SAMPLER_UNIFORM_STR, TRANSF_TEXTURE_UNIT);
  m_volumeShader->setUniform(VOLUME_TRANSF_SCALER_UNIFORM_STR, 1.0f);

  m_volumeShaderLighting->bind();
  m_volumeShaderLighting->setUniform(VOLUME_SAMPLER_UNIFORM_STR, BLOCK_TEXTURE_UNIT);
  m_volumeShaderLighting->setUniform(TRANSF_SAMPLER_UNIFORM_STR, TRANSF_TEXTURE_UNIT);
  m_volumeShaderLighting->setUniform(VOLUME_TRANSF_SCALER_UNIFORM_STR, 1.0f);
  setShaderLightPos(glm::normalize(glm::vec3{ 1.0f, 1.0f, 1.0f }));
  setShaderNShiney(1.1f);
  setShaderMaterial({ 0.15f, 0.65f, 0.75f });

  // sets m_currentShader depending on m_shouldUseLighting.
  setShouldUseLighting(m_shouldUseLighting);

  GLuint sampler_state{ 0 };
  gl_check(glGenSamplers(1, &sampler_state));
  if (sampler_state == 0) {
    bd::Err() << "Could not generate a sampler object.";
    return false;
  }
  gl_check(glSamplerParameteri(sampler_state, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  gl_check(glSamplerParameteri(sampler_state, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  gl_check(glSamplerParameteri(sampler_state, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
  gl_check(glSamplerParameteri(sampler_state, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
  gl_check(glSamplerParameteri(sampler_state, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER));
//  gl_check(glSamplerParameterf(sampler_state, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f));
  gl_check(glBindSampler(sampler_state, BLOCK_TEXTURE_UNIT));
  m_sampler_state = sampler_state;

  return true;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::setColorMapTexture(bd::Texture const &tfunc)
{
  // bind tfunc to the transfer texture unit.
  tfunc.bind(TRANSF_TEXTURE_UNIT);
  m_colorMapTexture = &tfunc;
//  m_colorMapTexture->bind(TRANSF_TEXTURE_UNIT);
}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::setColorMapScaleValue(float val)
{
  m_tfuncScaleValue = val;
  m_currentShader->setUniform(VOLUME_TRANSF_SCALER_UNIFORM_STR, m_tfuncScaleValue);
}


///////////////////////////////////////////////////////////////////////////////
float
BlockRenderer::getColorMapScaleValue() const
{
  return m_tfuncScaleValue;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::setBackgroundColor(const glm::vec3 &c)
{
  m_backgroundColor = c;
  glClearColor(c.r, c.g, c.b, 0.0f);
}


///////////////////////////////////////////////////////////////////////////////
glm::vec3 const &
BlockRenderer::getBackgroundColor() const
{
  return m_backgroundColor;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::setShouldUseLighting(bool b)
{
  if (b) {
    m_currentShader->unbind();
    m_currentShader = m_volumeShaderLighting.get();
    m_currentShader->bind();
    m_currentShader->setUniform(VOLUME_TRANSF_SCALER_UNIFORM_STR, m_tfuncScaleValue);
  } else {
    m_currentShader->unbind();
    m_currentShader = m_volumeShader.get();
    m_currentShader->bind();
    m_currentShader->setUniform(VOLUME_TRANSF_SCALER_UNIFORM_STR, m_tfuncScaleValue);
  }
  m_shouldUseLighting = b;
}


///////////////////////////////////////////////////////////////////////////////
bool
BlockRenderer::getShouldUseLighting() const
{
  return m_shouldUseLighting;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::setShaderNShiney(float n)
{
  m_volumeShaderLighting->setUniform(LIGHTING_N_SHINEY_UNIFORM_STR, n);
}


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::setShaderLightPos(glm::vec3 const &L)
{
  m_volumeShaderLighting->setUniform(LIGHTING_LIGHT_POS_UNIFORM_STR, L);
}


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::setShaderMaterial(glm::vec3 const &M)
{
  m_volumeShaderLighting->setUniform(LIGHTING_MAT_UNIFORM_STR, M);
}


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::setDrawNonEmptyBoundingBoxes(bool b)
{
  m_drawNonEmptyBoundingBoxes = b;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::setDrawNonEmptySlices(bool b)
{
  m_drawNonEmptySlices = b;
}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::draw()
{
  // We need to draw in reverse-visibility order (painters algorithm!)
  // so the transparency looks correct.
  sortBlocks();
  //TODO: only sort if rotated beyond a limit.


  // Side effect: recalculation of world-view-projection matrix.
  setWorldMatrix(glm::mat4{ 1.0f });

  gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  drawAxis();

  if (m_drawNonEmptyBoundingBoxes || m_rangeChanging) {
    drawNonEmptyBoundingBoxes();
  }

  if (m_drawNonEmptySlices && !m_rangeChanging) {
    drawNonEmptyBlocks_Forward();
  }

}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::drawNonEmptyBoundingBoxes()
{
  m_wireframeShader->bind();
  m_boxesVao->bind();
  size_t const nblk{ m_nonEmptyBlocks->size() };
  for (size_t i{ 0 }; i < nblk; ++i) {

    bd::Block *b{ ( *m_nonEmptyBlocks )[i] };

    setWorldMatrix(b->transform());
    m_wireframeShader->setUniform(WIREFRAME_MVP_MATRIX_UNIFORM_STR,
                                  getWorldViewProjectionMatrix());
    gl_check(glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid *) 0));
    gl_check(glDrawElements(GL_LINE_LOOP,
                            4,
                            GL_UNSIGNED_SHORT,
                            (GLvoid *) ( 4 * sizeof(GLushort))));
    gl_check(glDrawElements(GL_LINES,
                            8,
                            GL_UNSIGNED_SHORT,
                            (GLvoid *) ( 8 * sizeof(GLushort))));
  }
//  m_boxesVao->unbind();
//  m_wireframeShader->unbind();
}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::drawSlices(int baseVertex, int elementOffset, unsigned int numSlices) const
{
  // Begin NVPM work profiling
//  perf_workBegin();
  gl_check(
      glDrawElementsBaseVertex(GL_TRIANGLE_STRIP,
                               ELEMENTS_PER_QUAD * numSlices, // count
                               GL_UNSIGNED_SHORT,              // type
                               (void *) elementOffset,        // element offset
                               baseVertex));                  // vertex offset
  // End NVPM work profiling.
//  perf_workEnd();

}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::drawAxis() const
{
  m_wireframeShader->bind();
  m_axisVao->bind();
  m_wireframeShader
      ->setUniform(WIREFRAME_MVP_MATRIX_UNIFORM_STR, getWorldViewProjectionMatrix());
  gl_check(glDrawArrays(GL_LINES,
                        0,
                        static_cast<GLsizei>(bd::CoordinateAxis::verts.size())));
//  m_axisVao->unbind();
//  m_wireframeShader->unbind();
}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::drawNonEmptyBlocks_Forward()
{
  /* Compute the SliceSet and offset into the vertex buffer 
   * of that slice set. */

  glm::vec3 const
      viewdir{ glm::normalize(getCamera().getLookAt() - getCamera().getEye()) };

  std::pair<int, int> const
      baseVertex{ computeBaseVertexFromViewDir(viewdir) };

  m_currentShader->bind();
  if (m_shouldUseLighting) {
    m_currentShader->setUniform(LIGHTING_VIEW_DIR_UNIFORM_STR, viewdir);
  }

  m_quadsVao->bind();

  size_t const nBlk{ m_nonEmptyBlocks->size() };
  NVTOOLS_PUSH_RANGE("DrawNonEmptyBlocks", 0);
  for (size_t i{ 0 }; i < nBlk; ++i) {
    bd::Block *b{ ( *m_nonEmptyBlocks )[i] };

    // only render if the block's texture data has been uploaded to GPU.
    if (b->status() & bd::Block::GPU_RES) {
      setWorldMatrix(b->transform());
      b->texture()->bind(BLOCK_TEXTURE_UNIT);
      gl_check(glBindSampler(m_sampler_state, BLOCK_TEXTURE_UNIT));

      m_currentShader->setUniform(VOLUME_MVP_MATRIX_UNIFORM_STR,
                                  getWorldViewProjectionMatrix());

      drawSlices(baseVertex.first, baseVertex.second,
                 m_numSlicesPerBlock[bd::ordinal<SliceSet>(m_selectedSliceSet)]);
    }
  }
  NVTOOLS_POP_RANGE

//  m_quadsVao->unbind();
//  m_currentShader->unbind();
}


////////////////////////////////////////////////////////////////////////////////
std::pair<int, int>
BlockRenderer::computeBaseVertexFromViewDir(glm::vec3 const &viewdir)
{
  glm::vec3 const absViewDir{ glm::abs(viewdir) };
  SliceSet newSelected{ SliceSet::YZ };
  bool isPos{ viewdir.x > 0 };
  float longest{ absViewDir.x };

  // find longest component in view vector.
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
  int const verts_per_quad{ 4 };
  unsigned long long baseVertex{ 0uL };
  unsigned long long elementOffset{ 0 };

  glm::u64 numSlices{ m_numSlicesPerBlock[bd::ordinal<SliceSet>(newSelected)] };

  switch (newSelected) {

    case SliceSet::YZ:
      if (isPos) {
        baseVertex = 0;
      } else {
        baseVertex = 1 * verts_per_quad * numSlices;
      }
      elementOffset = 0;
      break;

    case SliceSet::XZ:
      if (isPos) {
        baseVertex = 2 * verts_per_quad * numSlices;
      } else {
        baseVertex = 3 * verts_per_quad * numSlices;
      }
      elementOffset = 1 * ELEMENTS_PER_QUAD * numSlices;
      break;

    case SliceSet::XY:
      if (isPos) {
        baseVertex = 4 * verts_per_quad * numSlices;
      } else {
        baseVertex = 5 * verts_per_quad * numSlices;
      }
      elementOffset = 2 * ELEMENTS_PER_QUAD * numSlices;
      break;

    default:
      break;
  }

  if (newSelected != m_selectedSliceSet) {
    std::cout << " Switched slice set: " << ( isPos ? '+' : '-' ) <<
              newSelected << " Base vertex: " << baseVertex << '\n';
  }

  m_selectedSliceSet = newSelected;

  return std::make_pair(baseVertex, elementOffset);

}


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::sortBlocks()
{
  glm::vec3 const eye{ getCamera().getEye() };

  // Sort the blocks by their distance from the camera.
  // The origin of each block is used.
  std::sort(m_nonEmptyBlocks->begin(), m_nonEmptyBlocks->end(),
            [&eye](bd::Block *a, bd::Block *b) {
              float a_dist = glm::distance(eye, a->origin());
              float b_dist = glm::distance(eye, b->origin());
              return a_dist > b_dist;
            });
}


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::handle_ROVChangingMessage(ROVChangingMessage &r)
{
  // we are on the delivery thread here.
  m_rangeChanging = r.IsChanging;
//  if (b) {
//    m_collection->pauseLoaderThread();
//  } else {
  // if rov is not changing anymore, then yayy! update with new visible blocks.
  if (!r.IsChanging) {
    m_collection->updateBlockCache();
  }

}


} // namespace subvol
