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
#include <bd/log/gl_log.h>
//#include <bd/log/logger.h>

namespace subvol
{


#define MAX_MILLIS_SINCE_LAST_JOB 100000000

BlockRenderer::BlockRenderer()
    : BlockRenderer(0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)
{
}


////////////////////////////////////////////////////////////////////////////////
BlockRenderer::BlockRenderer(int numSlices,
                             std::shared_ptr<bd::ShaderProgram> volumeShader,
                             std::shared_ptr<bd::ShaderProgram> volumeShaderLighting,
                             std::shared_ptr<bd::ShaderProgram> wireframeShader,
                             subvol::BlockCollection *blockCollection,
                             std::shared_ptr<bd::VertexArrayObject> blocksVAO,
                             std::shared_ptr<bd::VertexArrayObject> bboxVAO,
                             std::shared_ptr<bd::VertexArrayObject> axisVao)
    : Renderer()

    , m_numSlicesPerBlock{ numSlices }
    , m_tfuncScaleValue{ 1.0f }

    , m_rov_min{ 0.0 }
    , m_rov_max{ 0.0 }
   
    , m_timeOfLastJob{ 0 }

    , m_drawNonEmptyBoundingBoxes{ false }
    , m_drawNonEmptySlices{ true }
    , m_ROVRangeChanged{ false }
    , m_cacheNeedsUpdating{ false }
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
    , m_collection{ blockCollection }

    , m_nonEmptyBlocks{ nullptr }
    , m_blocks{ nullptr }
{
  m_blocks = &(m_collection->blocks());
  m_nonEmptyBlocks = &(m_collection->nonEmptyBlocks());
  //m_renderableBlocks.reserve(m_collection->blocks().size());
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
  m_volumeShader->bind();
  m_volumeShader->setUniform(VOLUME_SAMPLER_UNIFORM_STR, BLOCK_TEXTURE_UNIT);
  m_volumeShader->setUniform(TRANSF_SAMPLER_UNIFORM_STR, TRANSF_TEXTURE_UNIT);
  m_volumeShader->setUniform(VOLUME_TRANSF_SCALER_UNIFORM_STR, 1.0f);

  m_volumeShaderLighting->bind();
  m_volumeShaderLighting->setUniform(VOLUME_SAMPLER_UNIFORM_STR, BLOCK_TEXTURE_UNIT);
  m_volumeShaderLighting->setUniform(TRANSF_SAMPLER_UNIFORM_STR, TRANSF_TEXTURE_UNIT);
  m_volumeShaderLighting->setUniform(VOLUME_TRANSF_SCALER_UNIFORM_STR, 1.0f);
  setShaderLightPos(glm::normalize(glm::vec3{ 1.0f, 1.0f, 1.0f }));
  setShaderNShiney(1.0f);
  setShaderMaterial({ 1.0f, 1.0f, 1.0f });

  // sets m_currentShader depending on m_shouldUseLighting.
  setShouldUseLighting(m_shouldUseLighting);
  filterBlocksByROV();

//  if (m_colorMapTexture == nullptr)
//    setColorMapTexture(ColorMapManager::getMapByName("WHITE_TO_BLACK").getTexture());

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
void
BlockRenderer::setBackgroundColor(const glm::vec3 &c)
{
  m_backgroundColor = c;
  glClearColor(c.r, c.g, c.b, 0.0f);
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

//void
//BlockRenderer::setShaderViewVec(glm::vec3 const &V)
//{
//  m_volumeShaderLighting->setUniform("V", V);
//}


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
BlockRenderer::setROVRange(double min, double max)
{
  m_rov_min = min;
  m_rov_max = max;
  m_ROVRangeChanged = true;

}


////////////////////////////////////////////////////////////////////////////////
unsigned long long int
BlockRenderer::getNumBlocks() const
{
  return m_collection->blocks().size();
}


////////////////////////////////////////////////////////////////////////////////
unsigned long long int
BlockRenderer::getNumBlocksShown() const
{
  return m_collection->nonEmptyBlocks().size();
}


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::setDrawNonEmptySlices(bool b)
{
  m_drawNonEmptySlices = b;
}
 
////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::setROVChanging(bool b)
{
  m_ROVChanging = b;
  if (b) {
    m_collection->pauseLoaderThread();
  } else {
    // if rov is not changing anymore, then yayy! update with new visible blocks.
    m_collection->updateBlockCache();
  }
}


////////////////////////////////////////////////////////////////////////////////
//void
//BlockRenderer::setIsRotating(bool b)
//{
//  m_ROVChanging = b;
//}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::draw()
{
  // We need to draw in reverse-visibility order (painters algorithm!)
  // so the transparency looks correct.

  if(m_ROVRangeChanged) {
    filterBlocksByROV();
    m_ROVRangeChanged = false;
  }

//  if (m_cacheNeedsUpdating) {
//    std::cout << "++++++Updating the block cache++++++" << std::endl;
//    m_collection->updateBlockCache();
//    m_cacheNeedsUpdating = false;
//  }

  
  if (glfwGetTimerValue() - m_timeOfLastJob > MAX_MILLIS_SINCE_LAST_JOB) {
    // load some blocks to the m_gpu if any available.
    m_collection->loadSomeBlocks();
    m_timeOfLastJob = glfwGetTimerValue();
  }


  //TODO: only sort if rotated beyond a limit.
  sortBlocks();


  // Side effect: recalculation of world-view-projection matrix.
  setWorldMatrix(glm::mat4{ 1.0f });

  gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  drawAxis();

  if (m_drawNonEmptyBoundingBoxes || m_ROVChanging) {
    drawNonEmptyBoundingBoxes();
  }

  if (m_drawNonEmptySlices && !m_ROVChanging) {
    drawNonEmptyBlocks_Forward();
  }

}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::drawNonEmptyBoundingBoxes()
{
  m_wireframeShader->bind();
  m_boxesVao->bind();
  size_t nblk{ m_nonEmptyBlocks->size() };
  for(size_t i{ 0 }; i < nblk; ++i) {

    bd::Block *b{ (*m_nonEmptyBlocks)[i] };

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


///////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::updateCache()
{
  m_cacheNeedsUpdating = true;
}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::drawSlices(int baseVertex)
{
  // Begin NVPM work profiling
  perf_workBegin();
  gl_check(
      glDrawElementsBaseVertex(GL_TRIANGLE_STRIP,
                               ELEMENTS_PER_QUAD * m_numSlicesPerBlock, // count
                               GL_UNSIGNED_SHORT,                       // type
                               0,                                       // element offset
                               baseVertex));                            // vertex offset
  // End NVPM work profiling.
  perf_workEnd();

}


////////////////////////////////////////////////////////////////////////////////
void
BlockRenderer::drawAxis()
{
  m_wireframeShader->bind();
  m_axisVao->bind();
  m_wireframeShader->setUniform("mvp", getWorldViewProjectionMatrix());
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

  GLint const
      baseVertex{ computeBaseVertexFromViewDir(viewdir) };

  m_currentShader->bind();
  if (m_shouldUseLighting) {
    m_currentShader->setUniform(LIGHTING_VIEW_DIR_UNIFORM_STR, viewdir);
  }

  m_quadsVao->bind();

  // Start an NVPM profiling frame
  perf_frameBegin();

  size_t nBlk{ m_nonEmptyBlocks->size() };

  for(size_t i{ 0 }; i < nBlk; ++i) {

    bd::Block *b{ (*m_nonEmptyBlocks)[i] };

    if (b->status() & bd::Block::GPU_RES) {
      setWorldMatrix(b->transform());
      b->texture()->bind(BLOCK_TEXTURE_UNIT);
      m_currentShader->setUniform(VOLUME_MVP_MATRIX_UNIFORM_STR,
                                  getWorldViewProjectionMatrix());

      drawSlices(baseVertex);
    }

  }

  // End the NVPM profiling frame.
  perf_frameEnd();
//  m_quadsVao->unbind();
//  m_currentShader->unbind();
}


////////////////////////////////////////////////////////////////////////////////
int
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
    std::cout << " Switched slice set: " << ( isPos ? '+' : '-' ) <<
              newSelected << " Base vertex: " << baseVertex << '\n';
  }


  m_selectedSliceSet = newSelected;

  return baseVertex;
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

void
BlockRenderer::filterBlocksByROV()
{
  m_collection->filterBlocksByROVRange(m_rov_min, m_rov_max);
}






} // namespace subvol
