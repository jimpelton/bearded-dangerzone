#include <GLFW/glfw3.h>

#include "blockcollection.h"
#include "blockloader.h"

#include <bd/log/gl_log.h>
#include <bd/log/logger.h>
#include <bd/util/util.h>
#include <bd/io/indexfile.h>

namespace subvol
{

using bd::Block;
using bd::IndexFile;
using bd::FileBlock;


///////////////////////////////////////////////////////////////////////////////
//BlockCollection::BlockCollection()
//    : BlockCollection(nullptr)
//{
//}


BlockCollection::BlockCollection(BlockLoader *loader,
                                 IndexFile const &index)
    : m_blocks()
    , m_nonEmptyBlocks()
    , m_volume{ }
    , m_loader{ loader }
{
  // This is probably a bad place for this, I know.
  m_loaderFuture =
      std::async(std::launch::async,
                 [loader]() -> int { return ( *loader )(); });

  m_volume = index.getVolume();
  initBlocksFromFileBlocks(index.getFileBlocks(),
                           m_volume.worldDims(),
                           m_volume.block_count());
}


///////////////////////////////////////////////////////////////////////////////
BlockCollection::~BlockCollection()
{
  if (m_loader) {
    delete m_loader;
  }
}


///////////////////////////////////////////////////////////////////////////////
//void
//BlockCollection::initBlocksFromIndexFile(bd::IndexFile const &index)
//{
//  m_volume = index.getVolume();
//
//  initBlocksFromFileBlocks(index.getFileBlocks(),
//                           m_volume.worldDims(),
//                           m_volume.block_count());
//
////  for (size_t i{ 0 }; i < m_blocks.size(); ++i) {
////    m_blocks[i]->pixelData(( *buffers )[i]);
////  }
////
////  for (size_t i{ 0 }; i < texs->size(); ++i) {
////    m_blocks[i]->texture(( *texs )[i]);
////    m_nonEmptyBlocks.push_back(m_blocks[i]);
////  }
//
////  m_loader->queueClassified(m_nonEmptyBlocks, m_emptyBlocks);
//
//}


///////////////////////////////////////////////////////////////////////////////
void
BlockCollection::initBlocksFromFileBlocks(std::vector<FileBlock> const &fileBlocks,
                                          glm::f32vec3 const &vd,
                                          glm::u64vec3 const &nb)
{

  if (fileBlocks.size() == 0) {
    bd::Warn() << "No blocks in list of file blocks to initialize.";
    return;
  }

  m_blocks.reserve(fileBlocks.size());
  m_nonEmptyBlocks.reserve(fileBlocks.size());

  int every = 0.1f * fileBlocks.size();
  every = every == 0 ? 1 : every;

  auto idx = 0ull;
  for (auto k = 0ull; k < nb.z; ++k) {
    for (auto j = 0ull; j < nb.y; ++j) {
      for (auto i = 0ull; i < nb.x; ++i) {

        if (idx % every == 0) {
          std::cout << "\rCreating block " << idx;
        }

        Block *block{ new Block{{ i, j, k }, fileBlocks[idx] }};
        m_blocks.push_back(block);

        idx++;
      }
    }
  }

  std::cout << std::endl;

}


///////////////////////////////////////////////////////////////////////////////
//void
//BlockCollection::filterBlocks(std::function<bool(Block const &)> isEmpty)
//{
//  m_nonEmptyBlocks.clear();
//
//  for (Block *b : m_blocks) {
//    uint64_t idx{ b->fileBlock().block_index };
//
//    if (! isEmpty(*b)) {
//      b->visible(true);
//      m_nonEmptyBlocks.push_back(b);
////      m_man->asyncLoadBlock(b);
//    } else {
//      b->visible(false);
//    }
//  }
//}


///////////////////////////////////////////////////////////////////////////////
void
BlockCollection::filterBlocksByROVRange(double rov_min, double rov_max)
{
  m_nonEmptyBlocks.clear();
  m_emptyBlocks.clear();

//  size_t bytes{ 0 };

  size_t nBlk{ m_blocks.size() };
  for (size_t i{ 0 }; i < nBlk; ++i) {

    bd::Block *b{ m_blocks[i] };
    double rov = b->fileBlock().rov;

    if (rov >= rov_min && rov <= rov_max) {
      b->empty(false);
      m_nonEmptyBlocks.push_back(b);
    } else {
      b->empty(true);
      m_emptyBlocks.push_back(b);
    }
  } // for

  m_visibleBlocksCb(m_nonEmptyBlocks.size());
}


///////////////////////////////////////////////////////////////////////////////
void
BlockCollection::updateBlockCache()
{
  m_loader->queueClassified(m_nonEmptyBlocks, m_emptyBlocks);
}


///////////////////////////////////////////////////////////////////////////////
Block *
BlockCollection::nextLoadableBlock()
{
  return m_loader->getNextGpuReadyBlock();
}


///////////////////////////////////////////////////////////////////////////////
void
BlockCollection::pauseLoaderThread()
{
  m_loader->clearLoadQueue();
}


///////////////////////////////////////////////////////////////////////////////
void
BlockCollection::loadSomeBlocks()
{
  uint64_t t{ 0 };
  uint64_t const MAX_JOB_LENGTH_MS = 10000000;
  bd::Block *b{ nullptr };
  int i{ 0 };
  while (t < MAX_JOB_LENGTH_MS && ( b = nextLoadableBlock())) {
    uint64_t start{ glfwGetTimerValue() };
    b->sendToGpu();
    m_loader->pushGpuResidentBlock(b);
//    bd::Dbg() << "Sent " << b->fileBlock().block_index
//              << " (" << b->status() << ") to GPU.";
    uint64_t timeToLoadBlock{ glfwGetTimerValue() - start };
    t += timeToLoadBlock;
    ++i;
  }

//  bd::Dbg() << "Loaded " << i << " blocks.";

}

///////////////////////////////////////////////////////////////////////////////
//bool
//BlockCollection::initBlockTextures(std::string const &file, bd::DataType type)
//{
//  if (m_blocks.size() == 0)
//  {
//    bd::Warn() << "No blocks to initialize textures for.";
//    return true;
//  }
//
//  bool rval = false;
//
//
//  
//  switch (type) {
//    case DataType::UnsignedCharacter:
//      rval = do_initBlockTextures<unsigned char>(file);
//      break;
//    case DataType::UnsignedShort:
//      rval = do_initBlockTextures<unsigned short>(file);
//      break;
//    case DataType::Double:
//      rval = do_initBlockTextures<double>(file);
//      break;
//    case DataType::Float:
//    default:
//      rval = do_initBlockTextures<float>(file);
//      break;
//  }
//
//  return rval;
//
//}


///////////////////////////////////////////////////////////////////////////////
std::vector<Block *> const &
BlockCollection::blocks() const
{
  return m_blocks;
}


///////////////////////////////////////////////////////////////////////////////
std::vector<Block *> &
BlockCollection::blocks()
{
  return m_blocks;
}


///////////////////////////////////////////////////////////////////////////////
std::vector<Block *> const &
BlockCollection::nonEmptyBlocks() const
{
  return m_nonEmptyBlocks;
}


///////////////////////////////////////////////////////////////////////////////
std::vector<Block *> &
BlockCollection::nonEmptyBlocks()
{
  return m_nonEmptyBlocks;
}


///////////////////////////////////////////////////////////////////////////////
uint64_t
BlockCollection::findLargestBlock(std::vector<Block *> &blocks)
{
  // find the largest element by comparing voxel counts for each nonEmptyBlock.
  auto largest =
      std::max_element(
          blocks.begin(),
          blocks.end(),

          [](Block *lhs, Block *rhs) -> bool {

            // compare number of voxels

            size_t lhs_vox =
                lhs->voxel_extent().x *
                    lhs->voxel_extent().y *
                    lhs->voxel_extent().z;

            size_t rhs_vox =
                rhs->voxel_extent().x *
                    rhs->voxel_extent().y *
                    rhs->voxel_extent().z;

            return lhs_vox < rhs_vox;
          });

  Block *blk{ *largest };
  return blk->voxel_extent().x * blk->voxel_extent().y * blk->voxel_extent().z;

}


void
BlockCollection::setVisibleBlocksCallback(std::function<void(size_t)> &func)
{
  m_visibleBlocksCb = func;
}


//IndexFile const &
//BlockCollection::indexFile() const
//{
//  return *m_indexFile;
//}


///////////////////////////////////////////////////////////////////////////////
//glm::u64vec3
//BlockCollection::blockDims()
//{
//  return m_blockDims;
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
//void
//BlockCollection::blockDims(const glm::u64vec3& voxelDims)
//{
//  m_blockDims = voxelDims;
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
//glm::u64vec3
//BlockCollection::volDims()
//{
//  return m_voxelDims;
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
//void
//BlockCollection::volDims(const glm::u64vec3& voldims)
//{
//  m_voxelDims = voldims;
//}


///////////////////////////////////////////////////////////////////////////////
// nb: number of blocks
// vd: volume voxel dimensions
// blocks: out parameter to be filled with blocks.
//void
//BlockCollection::initBlocks(glm::u64vec3 nb, glm::u64vec3 vd)
//{
//  m_blockDims = vd / nb;
//  m_voxelDims = vd;
//  m_numBlocks = nb;
//
//  // block world voxelDims
//  glm::vec3 wld_dims{ 1.0f / glm::vec3(nb) };
//
//
//  Dbg() << "Starting block init: "
//      "Number of blocks: " <<
//      nb.x << ", " << nb.y << ", " << nb.z <<
//      " Volume dimensions: "
//          <<  vd.x << ", " << vd.y << ", " << vd.z <<
//      " Block dimensions: "
//          << ", " << wld_dims.x << ", " << wld_dims.y << ", " << wld_dims.z;
//
//  // Loop through all our blocks (identified by <bx,by,bz>) and populate block fields.
//  for (auto bz = 0ull; bz < nb.z; ++bz)
//    for (auto by = 0ull; by < nb.y; ++by)
//      for (auto bx = 0ull; bx < nb.x; ++bx) {
//        // i,j,k block identifier
//        glm::u64vec3 blkId{ bx, by, bz };
//        // lower left corner in world coordinates
//        glm::vec3 worldLoc{ (wld_dims * glm::vec3(blkId)) - 0.5f }; // - 0.5f;
//        // origin (centroid) in world coordiates
//        glm::vec3 blk_origin{ (worldLoc + (worldLoc + wld_dims)) * 0.5f };
//
//        Block blk{ glm::u64vec3{bx, by, bz}, , blk_origin };
//        m_blocks.push_back(blk);
//      }
//
//  Dbg() << "Finished block init: total blocks is " << m_blocks.size();
//}



///////////////////////////////////////////////////////////////////////////////
//void BlockCollection::filterBlocks(const float* data, /*unsigned int sampler,*/
//                                   float tmin, float tmax)
//{
//  size_t blkPoints{ bd::vecCompMult(m_blockDims) };
//
//  float* image{ new float[blkPoints] };
//
//  for (auto& b : m_blocks) {
//
//    glm::u64vec3 ijk{ b.ijk() };
//    fillBlockData(ijk, data, image);
//    float avg{ 0.0f };
//    for (size_t i = 0; i < blkPoints; ++i) {
//      avg += image[i];
//    }
//    avg /= blkPoints;
//    b.avg(avg);
//
//    //TODO: call filter function.
//    if (avg < tmin || avg > tmax) {
//      b.empty(true);
//    } else {
//      //      b.texture().samplerLocation(sampler);
//      //      b.texture().textureUnit(0);
//      b.texture().genGLTex3d(image, Texture::Format::RED, Texture::Format::RED,
//         m_blockDims.x, m_blockDims.y, m_blockDims.z);
//
//      if (b.texture().id() == 0) {
//        Err() << "Failed to allocate a gl texture for block ("
//            << ijk.x << ", " << ijk.y << ", " << ijk.z << ").";
//      }
//
//      m_nonEmptyBlocks.push_back(&b);
//    }
//  } // for auto
//
//  delete[] image;
//  Info() << m_blocks.size() - m_nonEmptyBlocks.size() << "/"
//      << m_blocks.size() << " blocks marked empty.";
//}

//void
//BlockCollection::fillBlockData(const glm::u64vec3 &ijk,
//    const glm::u64vec3 &vox_dims,
//    const float* in_data,
//    float* out_blockData)
//{
//  size_t imageIdx{ 0 };
//
//  // block start = block index * block size
//  glm::u64vec3 bst{ ijk * vox_dims };
//
//  // block end = block start + block size
//  glm::u64vec3 end{ bst + vox_dims };
//
//  auto vol_dims_x = m_indexFile->getHeader().volume_extent[0];
//  auto vol_dims_y = m_indexFile->getHeader().volume_extent[1];
//
//  for (auto k = bst.z; k < end.z; ++k)
//    for (auto j = bst.y; j < end.y; ++j)
//      for (auto i = bst.x; i < end.x; ++i) {
//        size_t dataIdx{ bd::to1D(i, j, k, vol_dims_x, vol_dims_y) };
//        float val{ in_data[dataIdx] };
//        out_blockData[imageIdx++] = val;
//      }
//}


} // namespace bd


