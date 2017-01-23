//
// Created by jim on 1/6/17.
//

#include "blockloader.h"

#include <bd/volume/block.h>
#include <bd/graphics/texture.h>
#include <bd/log/logger.h>
#include <bd/util/util.h>

#include <list>
#include <vector>
#include <algorithm>
#include <fstream>
#include <queue>

namespace subvol
{

BlockLoader::BlockLoader(BLThreadData *threadParams)
  : m_stopThread{ false }
{
  dptr = threadParams;
}

//  BlockLoader::BlockLoader(BlockLoader const&)
//  {
//  }
//
//  BlockLoader& BlockLoader::operator=(BlockLoader const&)
//  {
//  }
//
//  BlockLoader& BlockLoader::operator=(BlockLoader const&&)
//  {
//  }

  BlockLoader::~BlockLoader()
{
  if (dptr)
  {
    if (dptr->buffers)
      delete dptr->buffers;

    if (dptr->texs)
      delete dptr->texs;
  }
}


namespace
{

bd::Block *
removeLastInvisibleBlock(std::list<bd::Block *> &list)
{
  auto rend = list.rend();
  std::list<bd::Block *>::reverse_iterator not_vis =
      std::find_if(list.rbegin(), rend,
                   [](bd::Block *be) -> bool {
                     return ( be->status() & bd::Block::VISIBLE ) == 0;
                   });

  if (not_vis == rend) {
    // no non-visible blocks in the cpu queue
    // (only happens if size gpu == size cpu)
    return nullptr;
  }

  auto byebye = list.erase(( not_vis.base()-- ));
  return *byebye;
}


bool
isInList(bd::Block *b, std::list<bd::Block *> &list)
{
  auto end = list.end();
  auto found = std::find(list.begin(), end, b);
  return found != end;
}
}


int
BlockLoader::operator()()
{
  bd::Dbg() << "Load thread started.";
  std::ifstream raw{ dptr->filename, std::ios::binary };
  if (!raw.is_open()) {
    return -1;
  }

  // GPU blocks have valid pixelData ptrs and valid texture ptrs.
  std::list<bd::Block *> gpu;
  // CPU blocks have valid pixelData ptrs, and may have valid texture ptrs.
  // (if they are visible).
  std::list<bd::Block *> cpu;

  std::vector<bd::Texture *> * const texs = dptr->texs;
  std::vector<char *> * const buffers = dptr->buffers;
  size_t const maxGpuBlocks = dptr->maxGpuBlocks;
  size_t const maxMainBlocks = dptr->maxCpuBlocks;
  size_t const slabWidth = dptr->slabDims[0];
  size_t const slabHeight = dptr->slabDims[1];

  while (!m_stopThread) {

    // get a block marked as visible
    bd::Block *b{ waitPopLoadQueue() };

    if (!b) {
      continue;
    }

    if (!isInList(b, gpu)) {
      // Not in GPU, check the CPU cache
      if (!isInList(b, cpu)) {
        // Not in CPU, or GPU
        // b must be loaded to CPU.
        char *pixData{ nullptr };
        if (cpu.size() == maxMainBlocks) {
          // Cpu full, evict a non-visible block
          bd::Block *notvis{ removeLastInvisibleBlock(cpu) };
          if (notvis) {
            // there was a non-visible block in the cpu cache, 
            // take it's pixel buffer.
            pixData = notvis->pixelData();
            notvis->pixelData(nullptr);
          }
        } else {
          // the cpu cache is not full, so grab a free buffer.
          pixData = buffers->back();
          buffers->pop_back();
        }

        b->pixelData(pixData);
        fillBlockData(b, &raw, slabWidth, slabHeight);
        // loaded to memory, so put in cpu cache.
        cpu.push_front(b);
        // put back to load queue so it can be processed for GPU.
        queueBlock(b);
        bd::Dbg() << "Block " << b->fileBlock().block_index << " ready for texture.";

      } // ! cpu list
      else {
        // Is in cpu, but not in GPU
        // Give b a texture so it will be uploaded to GPU
        bd::Texture *tex{ nullptr };
        if (gpu.size() == maxGpuBlocks) {

          // no textures, evict non-vis from gpu
          bd::Block *notvis{ removeLastInvisibleBlock(gpu) };
          if (notvis) {
            // nonvis block found, give it a texture
            tex = notvis->texture();
            notvis->texture(nullptr);
          }

        } else {
          // gpu cache not full yet, so grab a texture from cache.
          tex = texs->back();
          texs->pop_back();
        }

        b->texture(tex);
        gpu.push_front(b);
      }
    } // ! gpu list
    else {
      // the block is already in gpu cache
      // TODO: if block is not gpu resident, push it to loadables...
      if (b->status() & bd::Block::GPU_WAIT) {
        m_loadables.push(b);
        bd::Dbg() << "Queued block " << b->fileBlock().block_index << " for gpu upload.";
      }
    }
  } // while

  raw.close();
  bd::Dbg() << "Exiting block loader thread.";
  return 0;

} // operator()

void
BlockLoader::stop()
{
  m_stopThread = true;
  m_wait.notify_one();
}


bd::Block *
BlockLoader::waitPopLoadQueue()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  while (m_loadQueue.size() == 0 && !m_stopThread) {
    m_wait.wait(m_mutex);
  }
  if (m_stopThread) {
    return nullptr;
  }

  bd::Block *b{ m_loadQueue.front() };
  m_loadQueue.pop();

  return b;
}


void
BlockLoader::queueBlock(bd::Block *b)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  m_loadQueue.push(b);
}


void
BlockLoader::queueAll(std::vector<bd::Block *> &visibleBlocks)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  std::queue<bd::Block *> q;
  m_loadQueue.swap(q);

  for (size_t i = 0; i < visibleBlocks.size(); ++i) {
    m_loadQueue.push(visibleBlocks[i]);
  }

  lock.release();
  m_wait.notify_one();
}


//void
//BlockLoader::getBlocksToLoad(std::vector<bd::Block *> &blocks)   
//{
//  std::unique_lock<std::mutex> lock(m_loadablesMutex);
//  std::copy(m_loadables.begin(), m_loadables.end(), blocks.begin());
//}


bd::Block *
BlockLoader::getNextGpuBlock()
{
  std::unique_lock<std::mutex> lock(m_loadablesMutex);
  bd::Block *b{ nullptr };
  if (m_loadables.size() > 0) {
    b = m_loadables.front();
    m_loadables.pop();
  }

  return b;
}


void
BlockLoader::fillBlockData(bd::Block *b, std::istream *infile, size_t vX, size_t vY) const
{
  char *blockBuffer{ b->pixelData() };

  // block's dimensions in voxels
  glm::u64vec3 const be{ b->voxel_extent() };
  // start element = block index w/in volume * block size
  glm::u64vec3 const start{ b->ijk() * be };
  // block end element = block voxel start voxelDims + block size
  glm::u64vec3 const end{ start + be };

  size_t const blockRowLength{ be.x };
  size_t const sizeType{ to_sizeType(b->texture()->dataType()) };

  // byte offset into file to read from
  size_t offset{ b->fileBlock().data_offset };

  // Loop through rows and slabs of volume reading rows of voxels into memory.
  for (auto slab = start.z; slab < end.z; ++slab) {
    for (auto row = start.y; row < end.y; ++row) {

      // seek to start of row
      infile->seekg(offset);

      // read the bytes of current row
      infile->read(blockBuffer, blockRowLength * sizeType);
      blockBuffer += blockRowLength;

      // offset of next row
      offset = bd::to1D(start.x, row + 1, slab, vX, vY);
      offset *= sizeType;
    }
  }
}

} // namespace bd