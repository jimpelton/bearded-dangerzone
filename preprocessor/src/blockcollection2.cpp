
#include "blockcollection2.h"


// Static members init
glm::u64vec3 BlockCollection2::m_blockDims{ 0, 0, 0 };
glm::u64vec3 BlockCollection2::m_volDims{ 0, 0, 0 };
glm::u64vec3 BlockCollection2::m_numBlocks{ 0, 0, 0 };

// Class members impl

///////////////////////////////////////////////////////////////////////////////
BlockCollection2::BlockCollection2()
{
}


///////////////////////////////////////////////////////////////////////////////
BlockCollection2::~BlockCollection2()
{
}


///////////////////////////////////////////////////////////////////////////////
glm::u64vec3
BlockCollection2::blockDims() const
{
  return m_blockDims;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockCollection2::blockDims(const glm::u64vec3& dims)
{
  m_blockDims = dims;
}


///////////////////////////////////////////////////////////////////////////////
glm::u64vec3
BlockCollection2::volDims() const
{
  return m_volDims;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockCollection2::volDims(const glm::u64vec3& voldims)
{
  m_volDims = voldims;
}

glm::u64vec3
BlockCollection2::numBlocks() const
{
  return m_numBlocks;
}

///////////////////////////////////////////////////////////////////////////////
// nb: number of blocks
// vd: volume voxel dimensions
// blocks: out parameter to be filled with blocks.
void
BlockCollection2::initBlocks(glm::u64vec3 nb, glm::u64vec3 vd)
{
  m_blockDims = vd / nb;
  m_volDims = vd;
  m_numBlocks = nb;

  // block world dims
  glm::vec3 wld_dims{ 1.0f / glm::vec3(nb) };

  gl_log("Starting block init: Number of blocks: %dx%dx%d, "
    "Volume dimensions: %dx%dx%d Block dimensions: %.2f,%.2f,%.2f",
    nb.x, nb.y, nb.z,
    vd.x, vd.y, vd.z,
    wld_dims.x, wld_dims.y, wld_dims.z)  ;

  // Loop through all our blocks (identified by <bx,by,bz>) and populate block fields.
  for (auto bz = 0ull; bz < nb.z; ++bz)
    for (auto by = 0ull; by < nb.y; ++by)
      for (auto bx = 0ull; bx < nb.x; ++bx) {
        // i,j,k block identifier
        glm::u64vec3 blkId{ bx, by, bz };
        // lower left corner in world coordinates
        glm::vec3 worldLoc{ wld_dims * glm::vec3(blkId) - 0.5f }; // - 0.5f;
        // origin (centroid) in world coordiates
        glm::vec3 blk_origin{ (worldLoc + (worldLoc + wld_dims)) * 0.5f };

        bd::Block blk{ glm::u64vec3(bx, by, bz), wld_dims, blk_origin };
        m_blocks.push_back(blk);
      }

  gl_log("Finished block init: total blocks is %d.", m_blocks.size());
}


const std::vector<bd::Block>&
BlockCollection2::blocks() const
{
  return m_blocks;
}


const std::vector<bd::Block *>&
BlockCollection2::nonEmptyBlocks() const
{
  return m_nonEmptyBlocks;
}

