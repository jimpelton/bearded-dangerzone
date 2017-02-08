//
// Created by jim on 1/6/17.
//

#include "blockloader.h"

#include <bd/graphics/texture.h>
#include <bd/log/logger.h>
#include <bd/util/util.h>

#include <algorithm>
#include <fstream>
#include <bd/volume/block.h>

namespace subvol
{

BlockLoader::BlockLoader(BLThreadData *threadParams, bd::Volume const &volume)
  : m_stopThread{ false }
  , m_gpuEmpty{ }
  , m_gpu{ }
  , m_mainEmpty{ }
  , m_main{ }
  , m_maxGpuBlocks{ threadParams->maxGpuBlocks }
  , m_maxMainBlocks{ threadParams->maxCpuBlocks }
  , m_sizeType{ threadParams->size }
  , m_slabWidth{ threadParams->slabDims[0] }
  , m_slabHeight{ threadParams->slabDims[1] }
  , m_volMin{ volume.min() }
  , m_volDiff{ volume.max() - volume.min() }
{
//  m_volMin = volume.min();
//  m_volDiff = volume.max() - volume.min();
}


BlockLoader::~BlockLoader()
{
//  if (dptr)
//  {
//    if (dptr->buffers)
//      delete dptr->buffers;
//
//    if (dptr->texs)
//      delete dptr->texs;
//  }
}


namespace
{

bd::Block *
removeLastInvisibleBlock(std::list<bd::Block *> &list)
{
  auto rend = list.end();
  auto not_vis =
      std::find_if(list.begin(), rend,
                   [](bd::Block *be) -> bool {
                     return ( be->status() & bd::Block::VISIBLE ) == 0;
                   });

  if (not_vis == rend) {
    // no non-visible blocks in the cpu queue
    // (only happens if size m_gpu == size cpu)
    return nullptr;
  }

//  std::advance(not_vis, 1);
//  auto byebye = list.erase( not_vis.base() );
  auto byebye = list.erase(not_vis);
  return *byebye;
}


bool
isInList(bd::Block *b, std::list<bd::Block *> &list)
{
  auto end = list.end();
  auto found = std::find(list.begin(), end, b);
  return found != end;
}

void
printBar(size_t ticks, char const *prefix)
{
//  unsigned char bar[81] = { 255 }; // white space
  std::cout << '\r' << prefix << " cache: ";
  for (int i=0; i<ticks; ++i) {
    std::cout << '.'; //static_cast<char>(178);
  }
}



} // namespace

using block_type = typename bd::Block;

void
BlockLoader::fileWithBufferFromEmptyBlock(block_type *b)
{
  block_type *empty = m_mainEmpty.front();
  m_mainEmpty.pop();

  auto index = empty->fileBlock().block_index;

  // evict empty from m_main.
  auto it = m_main.find(index);
  assert(it != m_main.end());
  m_main.erase(it);

  bd::Texture *t = nullptr;
  auto gpuIt = m_gpu.find(index);
  assert(gpuIt != m_gpu.end());
  m_gpu.erase(index);


  char *pixels = empty->pixelData();
  empty->pixelData(nullptr);
  assert(pixels != nullptr);
  b->pixelData(pixels);

  fillBlockData(b, &m_raw, m_sizeType, m_slabWidth, m_slabHeight);
  m_main.insert(std::make_pair(b->fileBlock().block_index, b));
}


int
BlockLoader::operator()(std::string const &filename)
{
  bd::Dbg() << "Load thread started.";
  std::ifstream raw{ filename, std::ios::binary };
  if (!raw.is_open()) {
    return -1;
  }

  // GPU blocks have valid pixelData ptrs and valid texture ptrs.
  // CPU blocks have valid pixelData ptrs, and may have valid texture ptrs.
  // (if they are visible).

//  std::vector<bd::Texture *> * const texs = dptr->texs;
//  std::vector<char *> * const buffers = dptr->buffers;


  while (!m_stopThread) {
    // get a block marked as visible
    bd::Block *b{ waitPopLoadQueue() };

    if (!b) {
      bd::Info() << "Exiting load loop.";
      continue;
    }

    if (!b->visible()) {
      handleEmptyBlock(b);
    } 
    else
    {
      handleVisibleBlocks(b);
    }

    bd::Dbg() << "Processing block " << b->fileBlock().block_index
              << " (" << b->status() << ")";


    auto idx{ b->fileBlock().block_index };

    // First we load the block: If the block is not in memory at all we must load it to
    // main memory. find a spot for it.
    // First, check to see if there are any free blocks on the gpu. If there are,
    // then there are also free blocks on the cpu. We must remove the empty block
    // from both empties lists, remove it from both non-empties lists and then
    // put our non-empty block in both visible lists.

    if (m_gpu.find(idx) == m_gpu.end()) {
      if (m_main.find(idx) == m_main.end()) {
        if (m_gpuEmpty.size() > 0) {
          
          // get an empty block and fill, remove from cpu empty as well
        } else if (m_mainEmpty.size() > 0) {
          // put it in the cpu anyway
        }
        assert(m_mainEmpty.size() > 0);
        fileWithBufferFromEmptyBlock(b);
        queueBlockAtFront(b);
      }
    } else {
        // is in main & not gpu
      pushLoadablesQueue(b);
    }





//////////////////////////////////////////////////////////
    {
      // Not in GPU, check the CPU cache
      if (!m_main.exists(idx)) {
        // Not in CPU, or GPU
        // b must be loaded to CPU.
        char *pixData{ nullptr };
        if (cpu.size() == maxMainBlocks || buffers->size() == 0) {
          // Cpu full, evict a non-visible block
          bd::Block *notvis{ removeLastInvisibleBlock(cpu) };
          if (notvis) {
            // there was a non-visible block in the cpu cache, 
            // take it's pixel buffer.
            bd::Dbg() << "Removed block " << notvis->fileBlock().block_index
              << " (" << notvis->status() << ") from CPU cache.";
            pixData = notvis->pixelData();
            notvis->pixelData(nullptr);
          }
        } else {
          // the cpu cache is not full, so grab a free buffer.
          if (buffers->size() > 0) {
            pixData = buffers->back();
            buffers->pop_back();
          }
        }

        if (pixData) {
          b->pixelData(pixData);
          fillBlockData(b, &raw, sizeType, slabWidth, slabHeight);
          // loaded to memory, so put in cpu cache.
          cpu.push_front(b);
          // put back to load queue so it will be processed for GPU.
          queueBlockAtFront(b);
          bd::Dbg() << "Block " << b->fileBlock().block_index
            << " (" << b->status() << ") ready for texture.";
        }

      } // ! cpu list
      else {
        // Is in cpu, but not in GPU
        // Give b a texture so it will be uploaded to GPU
        bd::Dbg() << "Block " << b->fileBlock().block_index
                  << " (" << b->status() << ") " "found in CPU cache.";
        bd::Texture *tex{ nullptr };
        
        if (m_gpu.size() == maxGpuBlocks || texs->size() == 0) {

          // no textures, evict non-vis from m_gpu
          bd::Block *notvis{ removeGpuLastInvisible() };
          if (notvis) {
            // nonvis block found, give it a texture
            tex = notvis->removeTexture();
            bd::Dbg() << "Removed block " << notvis->fileBlock().block_index
                      << " (" << notvis->status() << ") from GPU cache.";
          }

        } else {
          // m_gpu cache not full yet, so grab a texture from cache.
          if (texs->size() > 0) {
            tex = texs->back();
            texs->pop_back();
          }
        }

        if (tex) {
          b->texture(tex);
          pushLoadablesQueue(b);
          bd::Dbg() << "Queued block " << b->fileBlock().block_index
            << " (" << b->status() << ") for GPU upload.";
        }
      }
    } // ! m_gpu list
    else {
      // the block is already in m_gpu cache
      bd::Dbg() << "Block " << b->fileBlock().block_index
                << " (" << b->status() << ") " " found in GPU cache.";
//      if (b->status() & bd::Block::GPU_WAIT) {
//      }
    }

    std::cout <<
              "Cpu: " << cpu.size() << "/" << maxMainBlocks << "\n"
                  "Gpu: " << m_gpu.size() << "/" << maxGpuBlocks << "\n"
                  "LdQ: " << m_loadQueue.size() << "\n"
                  "Ldb: " << m_loadables.size() << "\n"
                  "Tex: " << texs->size() << "\n"
                  "Buf: " << buffers->size() << std::endl;

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

  bd::Block *b{ m_loadQueue.back() };
  m_loadQueue.pop_back();

  return b;
}


bd::Block *
BlockLoader::removeGpuLastInvisible()
{
  std::unique_lock<std::mutex>(m_gpuMutex);
  return removeLastInvisibleBlock(m_gpu);
}


bd::Block *
BlockLoader::removeGpuBlockReverse(bd::Block *b)
{
  std::unique_lock<std::mutex>(m_gpuMutex);
  auto found = std::find(m_gpu.begin(), m_gpu.end(), b);
  if (found == m_gpu.end()) {
    return nullptr;
  }
  
  m_gpu.erase(found);
  return *found;
}


void
BlockLoader::handleEmptyBlock(bd::Block *b)
{

  auto idx = b->index();
  if (m_gpu.find(idx) != m_gpu.end())
  {
    m_gpuEmpty.put(b);
  }
}


void
BlockLoader::handleVisibleBlock(bd::Block *b)
{
  
}

bool
BlockLoader::isInGpuList(bd::Block *b)
{
  std::unique_lock<std::mutex>(m_gpuMutex);
  return isInList(b, m_gpu);
}

void
BlockLoader::queueBlockAtFront(bd::Block *b)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  m_loadQueue.push_back(b);
  m_wait.notify_all();
}


void
BlockLoader::queueAll(std::vector<bd::Block *> &visibleBlocks, 
  std::vector<bd::Block*>& nonVisibleBlocks)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  m_loadQueue.clear();

  for (size_t i{ 0 }; i < nonVisibleBlocks.size(); ++i) {
    m_loadQueue.push_back(nonVisibleBlocks[i]);
  }

  bd::Dbg() << "Queueing " << visibleBlocks.size() << " for loading.";
  for (size_t i = 0; i < visibleBlocks.size(); ++i) {
    m_loadQueue.push_back(visibleBlocks[i]);
  }

  std::sort(m_loadQueue.begin(), m_loadQueue.end(),
            [](bd::Block *lhs, bd::Block *rhs) -> bool {
              return lhs->fileBlock().rov > rhs->fileBlock().rov;
            });

//  std::sort(m_loadQueue.begin(), m_loadQueue.end(),
//            [](bd::Block *lhs, bd::Block *rhs) -> bool {
//              return lhs->fileBlock().block_index > rhs->fileBlock().block_index;
//            });

  m_wait.notify_all();
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
BlockLoader::pushGpuResBlock(bd::Block *b)
{
  std::unique_lock<std::mutex> lock(m_gpuMutex);
  m_gpu.push_back(b);
}


void
BlockLoader::clearCache()
{
  std::unique_lock<std::mutex> lock(m_mutex);
  m_loadQueue.clear();
//  std::queue<bd::Block *> q;
//  m_loadQueue.swap(q);
}



void
BlockLoader::pushLoadablesQueue(bd::Block *b)
{
  std::unique_lock<std::mutex> lock(m_loadablesMutex);
  m_loadables.push(b);
}

void
BlockLoader::fillBlockData(bd::Block *b, std::istream *infile,
                           size_t sizeType, size_t vX, size_t vY) const
{

  // block's dimensions in voxels
  glm::u64vec3 const be{ b->voxel_extent() };
  // start element = block index w/in volume * block size
  glm::u64vec3 const start{ b->ijk() * be };
  // block end element = block voxel start voxelDims + block size
  glm::u64vec3 const end{ start + be };

  size_t const blockRowLength{ be.x };
  //size_t const sizeType{ to_sizeType(b->texture()->dataType()) };

  // byte offset into file to read from
  size_t offset{ b->fileBlock().data_offset };

  uint64_t const buf_len{ be.x * be.y * be.z };
  char * const disk_buf{ new char[buf_len] }; 
  char *temp = disk_buf;
  // Loop through rows and slabs of volume reading rows of voxels into memory.
  for (uint64_t slab = start.z; slab < end.z; ++slab) {
    for (uint64_t row = start.y; row < end.y; ++row) {

      // seek to start of row
      infile->seekg(offset);

      // read the bytes of current row
      infile->read(temp, blockRowLength * sizeType);
      temp += blockRowLength;

      // offset of next row
      offset = bd::to1D(start.x, row + 1, slab, vX, vY);
      offset *= sizeType;
    }
  }

  float * const pixelData = reinterpret_cast<float *>(b->pixelData());
  //Normalize the data prior to generating the texture.
  for (size_t idx{ 0 }; idx < buf_len; ++idx) {
    pixelData[idx] = (disk_buf[idx] - m_volMin) / m_volDiff;
  }

  delete [] disk_buf;

}

} // namespace bd