//
// Created by Jim Pelton on 4/26/16.
//

#include <bd/volume/volume.h>

#include <sstream>
#include "bd/io/indexfileheader.h"


namespace bd
{
namespace
{

/// \brief Compute the world dimensions between 0 and 1 along each
///  axis. The aspect ratio is maintained.
glm::vec3
calculateWorldDims(glm::u64vec3 voxelDims)
{
  glm::vec3 worldDims{ 0 };

  glm::u64 longest{ voxelDims.x };
  if (longest < voxelDims.y) {
    longest = voxelDims.y;
  }
  if (longest < voxelDims.z) {
    longest = voxelDims.z;
  }

  worldDims.x = voxelDims.x / glm::f32(longest);
  worldDims.y = voxelDims.y / glm::f32(longest);
  worldDims.z = voxelDims.z / glm::f32(longest);

  return worldDims;
}
}


///////////////////////////////////////////////////////////////////////////////
Volume::Volume()
  : Volume({ }, { 1, 1, 1 })
{
}


///////////////////////////////////////////////////////////////////////////////
Volume::Volume(glm::u64vec3 const &voxelDims, glm::u64vec3 const &numBlocks)
  : m_blockDims{ }
  , m_blockCount{ }
  , m_voxelDims{ voxelDims }
  , m_worldDims{ 0, 0, 0 }
  , m_volEmptyVoxels{ 0 }
  , m_volMax{ std::numeric_limits<double>::lowest() }
  , m_volMin{ std::numeric_limits<double>::max() }
  , m_volAvg{ 0.0 }
  , m_volTot{ 0.0 }
  , m_rovMin{ 0.0 }
  , m_rovMax{ 0.0 }
{
  glm::u64vec3 nb{ numBlocks };
  if (numBlocks.x == 0 || numBlocks.y == 0 || numBlocks.z == 0) {
    nb.x = nb.y = nb.z = 1;
  }

  m_blockDims = voxelDims / nb;
  m_blockCount = nb;

  m_worldDims = calculateWorldDims(voxelDims);
}


///////////////////////////////////////////////////////////////////////////////
Volume::Volume(Volume const &other)
  : m_blockDims{ other.m_blockDims }
  , m_blockCount{ other.m_blockCount }
  , m_voxelDims{ other.m_voxelDims }
  , m_worldDims{ other.m_worldDims }
  , m_volEmptyVoxels{ other.m_volEmptyVoxels }
  , m_volMax{ other.m_volMax }
  , m_volMin{ other.m_volMin }
  , m_volAvg{ other.m_volAvg }
  , m_volTot{ other.m_volTot }
  , m_rovMin{ other.m_rovMin }
  , m_rovMax{ other.m_rovMax }
{
}


///////////////////////////////////////////////////////////////////////////////
Volume::~Volume()
{
}


///////////////////////////////////////////////////////////////////////////////
const glm::u64vec3&
Volume::block_dims() const
{
  return m_blockDims;
}


///////////////////////////////////////////////////////////////////////////////
const glm::u64vec3&
Volume::block_count() const
{
  return m_blockCount;
}


///////////////////////////////////////////////////////////////////////////////
void
Volume::block_count(const glm::u64vec3 &nb)
{
  m_blockDims = m_voxelDims / nb;
  m_blockCount = nb;
}


uint64_t
Volume::total_block_count() const
{
  return m_blockCount.x * m_blockCount.y * m_blockCount.z;
}


///////////////////////////////////////////////////////////////////////////////
const glm::u64vec3&
Volume::voxelDims() const
{
  return m_voxelDims;
}


///////////////////////////////////////////////////////////////////////////////
void
Volume::voxelDims(const glm::u64vec3 &voxdims)
{
  m_worldDims = calculateWorldDims(voxdims);
  m_blockDims = voxdims / m_blockCount;
  m_voxelDims = voxdims;
}


///////////////////////////////////////////////////////////////////////////////
glm::u64vec3 const&
Volume::voxelDims()
{
  return m_voxelDims;
}


///////////////////////////////////////////////////////////////////////////////
void
Volume::worldDims(glm::f32vec3 const &wd)
{
  m_worldDims = wd;
}


///////////////////////////////////////////////////////////////////////////////
glm::f32vec3 const&
Volume::worldDims() const
{
  return m_worldDims;
}


///////////////////////////////////////////////////////////////////////////////
double
Volume::min() const
{
  return m_volMin;
}


///////////////////////////////////////////////////////////////////////////////
void
Volume::min(double min)
{
  m_volMin = min;
}


///////////////////////////////////////////////////////////////////////////////
double
Volume::max() const
{
  return m_volMax;
}


///////////////////////////////////////////////////////////////////////////////
void
Volume::max(double max)
{
  m_volMax = max;
}


///////////////////////////////////////////////////////////////////////////////
double
Volume::avg() const
{
  return m_volAvg;
}


///////////////////////////////////////////////////////////////////////////////
void
Volume::avg(double avg)
{
  m_volAvg = avg;
}


///////////////////////////////////////////////////////////////////////////////
double
Volume::total() const
{
  return m_volTot;
}


///////////////////////////////////////////////////////////////////////////////
void
Volume::total(double t)
{
  m_volTot = t;
}


///////////////////////////////////////////////////////////////////////////////
void
Volume::rovMin(double r)
{
  m_rovMin = r;
}


///////////////////////////////////////////////////////////////////////////////
double
Volume::rovMin() const
{
  return m_rovMin;
}


///////////////////////////////////////////////////////////////////////////////
void
Volume::rovMax(double r)
{
  m_rovMax = r;
}


///////////////////////////////////////////////////////////////////////////////
double
Volume::rovMax() const
{
  return m_rovMax;
}


///////////////////////////////////////////////////////////////////////////////
void
Volume::numEmptyVoxels(uint64_t empty)
{
  m_volEmptyVoxels = empty;
}


///////////////////////////////////////////////////////////////////////////////
uint64_t
Volume::numEmptyVoxels() const
{
  return m_volEmptyVoxels;
}


///////////////////////////////////////////////////////////////////////////////
glm::u64vec3
Volume::blocksExtent() const
{
  return m_blockCount * m_blockDims; // component-wise multiply
}


std::ostream&
operator<<(std::ostream &os, bd::Volume const &v)
{
  os <<
    "\"volume\": {\n"
    "  \"num_blocks\": [" << v.block_count().x << ", " << v.block_count().y << ", "
    << v.block_count().z << "],\n"
    "  \"volume_extent\": [" << v.voxelDims().x << ", " << v.voxelDims().y
    << ", " << v.voxelDims().z << "],\n"
    "  \"blocks_extent\": [" << v.blocksExtent().x << ", " << v.blocksExtent().y
    << ", " << v.blocksExtent().z << "],\n"
    "  \"vol_empty_voxels\": " << v.numEmptyVoxels() << ",\n"
    "  \"volume_world_dims\": [" << v.worldDims().x << ", "
    << v.worldDims().y << ", " << v.worldDims().z << "],\n"
    "  \"vol_min\": " << std::fixed << v.min() << ",\n"
    "  \"vol_max\": " << std::fixed << v.max() << ",\n"
    "  \"vol_avg\": " << std::fixed << v.avg() << ",\n"
    "  \"vol_total\": " << std::fixed << v.total() << ",\n"
    "  \"rov_min\": " << std::fixed << v.rovMin() << ",\n"
    "  \"rov_max\": " << std::fixed << v.rovMax() << "\n"
    "}";

  return os;
}
} // namespace bd
