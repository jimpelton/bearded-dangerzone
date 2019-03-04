//
// Created by jim on 2/16/19.
//

#include "blockraycaster.h"
#include "constants.h"
#include "messages/messagebroker.h"

#include <bd/graphics/shader.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/geo/mesh.h>
#include <bd/log/gl_log.h>

#include <GL/glew.h>

#include <memory>
#include "bd/geo/axis.h"

namespace subvol
{
namespace renderer
{

namespace {
  const std::vector<glm::vec3> cube_verts{
      {
          glm::vec3{ -0.5f, -0.5f, 0.5f},
          glm::vec3{ 0.5f, -0.5f, 0.5f },
          glm::vec3{ 0.5f, 0.5f, 0.5f },
          glm::vec3{ -0.5f, 0.5f, 0.5f },
          glm::vec3{ -0.5f, -0.5f, -0.5f },
          glm::vec3{ 0.5f, -0.5f, -0.5f },
          glm::vec3{ 0.5f, 0.5f, -0.5f },
          glm::vec3{ -0.5f, 0.5f, -0.5f }
      }
  };

  const std::vector<unsigned short> cube_indices{
      {
          // front
          0, 1, 2,
          0, 2, 3,
          // right
          1, 5, 6,
          1, 6, 2,
          // back
          5, 4, 7,
          5, 7, 6,
          // left
          4, 0, 3,
          4, 3, 7,
          // top
          2, 6, 7,
          2, 7, 3,
          // bottom
          4, 5, 1,
          4, 1, 0,
      }
  };
}

BlockingRaycaster::BlockingRaycaster(std::shared_ptr<subvol::BlockCollection> bc, bd::Volume const &v)
  : Recipient("blocking ray caster")
  , m_alphaBlending{}
  , m_wireframeShader{}
  , m_blockCollection{ std::move(bc) }
  , m_cube{ cube_verts, cube_indices }
  , m_axis{ }
  , m_volume{ v }
{
}


///////////////////////////////////////////////////////////////////////////////
BlockingRaycaster::~BlockingRaycaster() noexcept
{
  Broker::unsubscribeRecipient(this);
}


///////////////////////////////////////////////////////////////////////////////
bool
BlockingRaycaster::initialize()
{
  Broker::subscribeRecipient(this);
  initShaders();
  return true;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockingRaycaster::draw()
{
  gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  sortBlocks();
  // drawAxis();
//  if (_drawNonEmptyBoundingBoxes) {
    drawNonEmptyBoundingBoxes();
//  }
  drawNonEmptyBlocks();
}


///////////////////////////////////////////////////////////////////////////////
void
BlockingRaycaster::drawNonEmptyBoundingBoxes()
{

//   size_t const nblk{ ;
   auto &blocks = m_blockCollection->getNonEmptyBlocks();
   auto nblk = blocks.size();
  m_wireframeShader->bind();
   for (size_t i{ 0 }; i < nblk; ++i) {

     bd::Block *b{ ( blocks[i] ) };

     setWorldMatrix(b->transform());
     m_wireframeShader->setUniform(WIREFRAME_MVP_MATRIX_UNIFORM_STR,
                                   getWorldViewProjectionMatrix());

      m_box.draw();
   }
   m_wireframeShader->unbind();
}


///////////////////////////////////////////////////////////////////////////////
void
BlockingRaycaster::drawAxis()
{
  m_wireframeShader->bind();
  m_wireframeShader->setUniform(WIREFRAME_MVP_MATRIX_UNIFORM_STR, getWorldViewProjectionMatrix());
  m_axis.draw();
  m_wireframeShader->unbind();
}


///////////////////////////////////////////////////////////////////////////////
void
BlockingRaycaster::drawNonEmptyBlocks()
{
  std::vector<bd::Block*> const &non_empties = m_blockCollection->getNonEmptyBlocks();
  m_alphaBlending->bind();
  for (auto &b : non_empties) {
    if (b->status() & bd::Block::GPU_RES) {
      setWorldMatrix(b->transform());
      b->texture()->bind(BLOCK_TEXTURE_UNIT);
      setUniforms(*b);
      gl_check(glBindSampler(m_volumeSampler, BLOCK_TEXTURE_UNIT));
      m_cube.draw();
    }
  }
  m_alphaBlending->unbind();
}


///////////////////////////////////////////////////////////////////////////////
void
BlockingRaycaster::setUniforms(bd::Block const &b)
{
  auto top = b.worldDims(); //glm::vec3{1.0, 1.0, 1.0};
  auto bot = -top;
  glm::mat3 mv = glm::mat3{getViewMatrix() * getWorldMatrix()};
  glm::mat3 normalMatrix = glm::transpose(glm::inverse(mv));

  m_alphaBlending->setUniform("ViewMatrix", getViewMatrix());
  m_alphaBlending->setUniform("MVP", getWorldViewProjectionMatrix());
  m_alphaBlending->setUniform("NormalMatrix", normalMatrix);
  m_alphaBlending->setUniform("aspect_ratio", getAspectRatio());
  m_alphaBlending->setUniform("focal_length", getFocalLength());
  m_alphaBlending->setUniform("viewport_size", glm::vec2{getViewPortWidth(), getViewPortHeight()});
  m_alphaBlending->setUniform("ray_origin", getCamera().getEye());
  m_alphaBlending->setUniform("top", top);
  m_alphaBlending->setUniform("bottom", bot);
  m_alphaBlending->setUniform("background_colour", glm::vec3{_backgroundColor});
  m_alphaBlending->setUniform("light_position", _shaderLightPos);
  m_alphaBlending->setUniform("material_colour", _shaderMat);
  m_alphaBlending->setUniform("step_length", 0.01f);
//  m_alphaBlending->setUniform("threshold", 200.f);
  m_alphaBlending->setUniform("gamma", 2.2f);
  m_alphaBlending->setUniform("volume", BLOCK_TEXTURE_UNIT);
//  m_alphaBlending->setUniform("jitter", 1);

  // glClear(GL_COLOR_BUFFER_BIT);
}


///////////////////////////////////////////////////////////////////////////////
void
BlockingRaycaster::sortBlocks()
{
  glm::vec3 const eye{ getCamera().getEye() };

  // Sort the blocks by their distance from the camera.
  // The origin of each block is used.
  std::vector<bd::Block*> &non_empties{ m_blockCollection->getNonEmptyBlocks() };
  std::sort(non_empties.begin(), non_empties.end(),
            [&eye](bd::Block *a, bd::Block *b) {
              float a_dist = glm::distance(eye, a->origin());
              float b_dist = glm::distance(eye, b->origin());
              return a_dist > b_dist;
            });
}


///////////////////////////////////////////////////////////////////////////////
void
BlockingRaycaster::initShaders()
{
  m_alphaBlending = std::unique_ptr<bd::ShaderProgram>( new bd::ShaderProgram() );
  auto pid = m_alphaBlending->linkProgram("shaders/maximum_intensity_projection.vert", "shaders/maximum_intensity_projection.frag");
  if (pid == 0) {
    throw std::runtime_error("could not initialize shaders");
  }

  GLuint sampler_state{ 0 };
  gl_check(glGenSamplers(1, &sampler_state));
  if (sampler_state == 0) {
    throw std::runtime_error("could not generate a sampler object");
  }

  gl_check(glSamplerParameteri(sampler_state, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  gl_check(glSamplerParameteri(sampler_state, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  gl_check(glSamplerParameteri(sampler_state, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
  gl_check(glSamplerParameteri(sampler_state, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
  gl_check(glSamplerParameteri(sampler_state, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER));
//  gl_check(glSamplerParameterf(sampler_state, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f));
  gl_check(glBindSampler(sampler_state, BLOCK_TEXTURE_UNIT));
  m_volumeSampler = sampler_state;
  m_alphaBlending->unbind();
  m_wireframeShader = std::unique_ptr<bd::ShaderProgram>(new bd::ShaderProgram());
  pid = m_wireframeShader->linkProgram("shaders/vert_vertexcolor_passthrough.glsl", "shaders/frag_vertcolor.glsl");
  if (pid == 0) {
    throw std::runtime_error("could not initialize vertex color shader");
  }
  m_wireframeShader->unbind();
}

// void BlockingRaycaster::initAxisVao()
// {
//   using BDAxis = bd::CoordinateAxis;
//   // vertex positions into attribute 0
//   vao.addVbo((float *) ( BDAxis::verts.data()),
//              BDAxis::verts.size()*BDAxis::vert_element_size,
//              BDAxis::vert_element_size,
//              VERTEX_COORD_ATTR,
//              bd::VertexArrayObject::Usage::Static_Draw); // attr 0
//
//   // vertex colors into attribute 1
//   vao.addVbo((float *) ( BDAxis::colors.data()),
//              BDAxis::colors.size()*3,
//              3, // 3 floats per color
//              VERTEX_COLOR_ATTR,
//              bd::VertexArrayObject::Usage::Static_Draw); // attr 1
// }
//

///////////////////////////////////////////////////////////////////////////////
void
BlockingRaycaster::handle_ROVChangingMessage(ROVChangingMessage &r)
{
  // we are on the delivery thread here.
  m_rangeChanging = r.IsChanging;
//  if (b) {
//    m_collection->pauseLoaderThread();
//  } else {
  // if rov is not changing anymore, then yayy! update with new visible blocks.
  if (!r.IsChanging) {
    m_blockCollection->updateBlockCache();
  }
}



}} // namespace subvol namespace renderer

