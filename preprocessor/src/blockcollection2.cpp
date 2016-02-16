
#include "blockcollection2.h"

#include <bd/log/gl_log.h>
#include <bd/util/util.h>

glm::u64vec3 BlockCollection2::m_blockDims{ 0, 0, 0 };
glm::u64vec3 BlockCollection2::m_volDims{ 0, 0, 0 };
glm::u64vec3 BlockCollection2::m_numBlocks{ 0, 0, 0 };

BlockCollection2::BlockCollection2()
{
}

BlockCollection2::~BlockCollection2()
{
}


///////////////////////////////////////////////////////////////////////////////
glm::u64vec3
BlockCollection2::blockDims()
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
BlockCollection2::volDims()
{
  return m_volDims;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockCollection2::volDims(const glm::u64vec3& voldims)
{
  m_volDims = voldims;
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
        glm::vec3 worldLoc{ (wld_dims * glm::vec3(blkId)) - 0.5f }; // - 0.5f;
        // origin (centroid) in world coordiates
        glm::vec3 blk_origin{ (worldLoc + (worldLoc + wld_dims)) * 0.5f };

        bd::Block blk{ glm::u64vec3(bx, by, bz), wld_dims, blk_origin };
        m_blocks.push_back(blk);
      }

  gl_log("Finished block init: total blocks is %d.", m_blocks.size());
}


///////////////////////////////////////////////////////////////////////////////
void BlockCollection2::filterBlocks(const float* image,
                                   float tmin, float tmax)
{
  size_t blkPoints{ bd::vecCompMult(m_blockDims) };

  //float* image{ new float[blkPoints] };

  for (auto& b : m_blocks) {

    //fillBlockData(b.ijk(), data, image);
    // Compute average and determine if Block is empty.
    float avg{ 0.0f };
    for (size_t i = 0; i < blkPoints; ++i) {
      avg += image[i];
    }
    avg /= blkPoints;
    b.avg(avg);

    //TODO: call filter function.
    if (avg < tmin || avg > tmax) {
      b.empty(true);
    } else {
      b.empty(false);
      m_nonEmptyBlocks.push_back(&b);
    }
  } // for auto

  //delete[] image;
  gl_log("%d/%d blocks marked empty.",
    m_blocks.size() - m_nonEmptyBlocks.size(), m_blocks.size());
}


const std::vector<bd::Block>&
BlockCollection2::blocks()
{
  return m_blocks;
}


const std::vector<bd::Block *>&
BlockCollection2::nonEmptyBlocks()
{
  return m_nonEmptyBlocks;
}


///////////////////////////////////////////////////////////////////////////////
// Private Members
///////////////////////////////////////////////////////////////////////////////


void BlockCollection2::fillBlockData(glm::u64vec3 ijk, const float* in_data,
                                    float* out_blockData)
{
  size_t imageIdx{ 0 };

  // block start = block index * block size
  glm::u64vec3 bst{ ijk * m_blockDims };

  // block end = block start + block size
  glm::u64vec3 end{ bst + m_blockDims };

  for (auto k = bst.z; k < end.z; ++k)
    for (auto j = bst.y; j < end.y; ++j)
      for (auto i = bst.x; i < end.x; ++i) {
        size_t dataIdx{ bd::to1D(i, j, k, m_volDims.x, m_volDims.y) };
        float val{ in_data[dataIdx] };
        out_blockData[imageIdx++] = val;
      }
}


