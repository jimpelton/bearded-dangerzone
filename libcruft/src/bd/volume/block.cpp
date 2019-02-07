#include <bd/volume/block.h>

#include <bd/log/gl_log.h>
#include <bd/util/util.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sstream>


namespace bd
{



///////////////////////////////////////////////////////////////////////////////
Block::Block(const glm::u64vec3& ijk, const FileBlock &fb)
  : m_fb{ fb }
  , m_ijk{ fb.ijk_index[0], fb.ijk_index[1], fb.ijk_index[2] }
  , m_origin{ fb.world_oigin[0], fb.world_oigin[1], fb.world_oigin[2] }
  , m_transform{ 1.0f }  // identity matrix
  , m_tex{ nullptr }
  , m_pixelData{ nullptr }
  , m_status{ 0x0 }
  , m_isVisible{ false }
{

  glm::vec3 wld_dims{ fb.world_dims[0], fb.world_dims[1], fb.world_dims[2] };

  glm::mat4 s{ glm::scale(glm::mat4{ 1.0f }, wld_dims) };

  glm::mat4 t{ glm::translate(glm::mat4{ 1.0f }, m_origin) };

  m_transform = t * s;

}


///////////////////////////////////////////////////////////////////////////////
Block::~Block()
{
}

//void
//Block::evictFromGpu()
//{
//  pixelData(nullptr);
//  visible(false);
//}


void
Block::sendToGpu()
{
  if (m_status & GPU_WAIT)
    m_tex->subImage3D(m_pixelData);

  m_status |= GPU_RES;
  m_status &= ~GPU_WAIT;
}


///////////////////////////////////////////////////////////////////////////////
//bool
//Block::visible() const
//{
//  return m_isVisible;
//}


///////////////////////////////////////////////////////////////////////////////
//void
//Block::visible(bool v)
//{
//  if (!v) {
//    m_status &= ~NOT_EMPTY;
//  } else {
//    m_status |= NOT_EMPTY;
//  }
//  m_isVisible = v;
//}


///////////////////////////////////////////////////////////////////////////////
const FileBlock&
Block::fileBlock() const
{
  return m_fb;
}


uint64_t
Block::index() const
{
  return m_fb.block_index;
}

///////////////////////////////////////////////////////////////////////////////
const glm::u64vec3&
Block::ijk() const
{
  return m_ijk;
}


///////////////////////////////////////////////////////////////////////////////
void
Block::ijk(const glm::u64vec3& ijk)
{
  m_ijk = ijk;
}


///////////////////////////////////////////////////////////////////////////////
bool
Block::empty() const
{
  return m_fb.is_empty == 1;
}


void
Block::empty(bool isEmpty)
{
  m_fb.is_empty = (decltype(m_fb.is_empty)) isEmpty;

  if (!isEmpty) {
    m_status |= NOT_EMPTY;
  } else {
    m_status &= ~NOT_EMPTY;
  }

}


///////////////////////////////////////////////////////////////////////////////
glm::vec3 const &
Block::origin() const
{
  return m_origin;
}


///////////////////////////////////////////////////////////////////////////////
double
Block::avg() const
{
  return m_fb.avg_val;
}


///////////////////////////////////////////////////////////////////////////////
bd::Texture *
Block::texture() 
{
  return m_tex;
}


///////////////////////////////////////////////////////////////////////////////
void
Block::texture(bd::Texture * tex)
{
  if (tex){
    m_status |= GPU_WAIT;
  } else {
    // if we are removing our texture, we aren't gpu resident anymore.
    m_status &= ~(GPU_RES | GPU_WAIT);
  }

  m_tex = tex;
}


///////////////////////////////////////////////////////////////////////////////
bd::Texture *
Block::removeTexture()   
{
  Texture *rval{ m_tex };
  texture(nullptr);
  return rval;
}


///////////////////////////////////////////////////////////////////////////////
glm::mat4&
Block::transform()
{
  return m_transform;
}


///////////////////////////////////////////////////////////////////////////////
glm::u64vec3
Block::voxel_extent() const
{
  return { m_fb.voxel_dims[0], m_fb.voxel_dims[1], m_fb.voxel_dims[2] };
}


///////////////////////////////////////////////////////////////////////////////
size_t 
Block::byteSize() const
{
  return m_fb.data_bytes;
}


///////////////////////////////////////////////////////////////////////////////
int
Block::status() const
{
  return m_status;
}


///////////////////////////////////////////////////////////////////////////////
char* 
Block::pixelData() 
{
  return m_pixelData;
}


///////////////////////////////////////////////////////////////////////////////
void
Block::pixelData(char *data)
{
  if (data) {
    // if we have texture data, we know we have CPU residency.
    m_status |= CPU_RES;
  } else {
    // data is nullptr, so block is not cpu or gpu res
    m_status &= ~(CPU_RES | GPU_RES);
  }

  m_pixelData = data;
}


char *
Block::removePixelData()
{
  char *p{ pixelData() };
  pixelData(nullptr);

  return p;
}


///////////////////////////////////////////////////////////////////////////////
std::string
Block::to_string() const
{
  std::stringstream ss;
  ss << "{ ijk: ("
     << m_ijk.x << ',' << m_ijk.y << ',' << m_ijk.z << "),\n"
         "Origin: ("
     << m_fb.world_oigin[0]
     << ',' << m_fb.world_oigin[1]
     << ',' << m_fb.world_oigin[2] << "),\n"
         "Empty: " << (empty() ? "True" : "False") << "\n"
         "Texture: " << m_tex << "\n"
         "Status: " << std::ios::hex << m_status << " }";

  return ss.str();
}


///////////////////////////////////////////////////////////////////////////////
std::ostream&
operator<<(std::ostream& os, const Block& b)
{
  return os << b.to_string();
}

} // namespace bd
