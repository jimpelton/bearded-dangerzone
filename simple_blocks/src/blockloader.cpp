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
#include "messages/recipient.h"
#include "messages/messagebroker.h"


namespace subvol
{

BlockLoader::BlockLoader(BLThreadData *threadParams, bd::Volume const &volume)
  : m_stopThread{ false }
  , m_gpu()
  , m_main()
  , m_texs()
  , m_buffs()
  , m_loadQueue{ }
  , m_gpuReadyQueue{ }
  , m_maxGpuBlocks{ threadParams->maxGpuBlocks }
  , m_maxMainBlocks{ threadParams->maxCpuBlocks }
  , m_sizeType{ bd::to_sizeType(threadParams->type) }
  , m_slabDims{ threadParams->slabDims[0], threadParams->slabDims[1] }
  , m_volMin{ volume.min() }
  , m_volDiff{ volume.max() - volume.min() }
  , m_fileName{ threadParams->filename }
  , m_reader{ nullptr }
{
  m_reader = BlockReaderFactory::New(threadParams->type);
  m_texs = *(threadParams->texs);
  m_buffs = *(threadParams->buffers);
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



int
BlockLoader::operator()()
{
  bd::Info() << "Load thread started.";
  raw.open(m_fileName, std::ios::binary);
  if (!raw.is_open()) {
    bd::Err() << "The raw file " << m_fileName
      << " could not be opened. Exiting loader loop.";
    return -1;
  }


  while (!m_stopThread) {

    BlockCacheStatsMessage *m{ new BlockCacheStatsMessage };
    m->CpuCacheSize = m_main.size();

    m_gpuMutex.lock();
    m->GpuCacheSize = m_gpu.size();
    m_gpuMutex.unlock();

    m_loadQueueMutex.lock();
    m->CpuLoadQueueSize = m_loadQueue.size();
    m_loadQueueMutex.unlock();

    m->CpuBuffersAvailable = m_buffs.size();
    m->GpuTexturesAvailable = m_texs.size();
    Broker::send(m);

    // get a block marked as visible
    bd::Block *b{ waitPopLoadQueue() };
    if (!b) {
      bd::Info() << "nullptr found in the load queue. Exiting loader loop.";
      break;
    }

    b->pixelData(m_buffs.back());
    m_buffs.pop_back();
    m_reader->fillBlockData(b->pixelData(),
                            &raw,
                            b->fileBlock().data_offset,
                            b->fileBlock().voxel_dims,
                            b->fileBlock().ijk_index,
                            m_slabDims,
                            m_volMin,
                            m_volDiff);
    m_main.insert(std::make_pair(b->index(), b));

    if (!m_texs.empty()) {
      b->texture(m_texs.back());
      m_texs.pop_back();
      pushGPUReadyQueue(b);
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
  std::unique_lock<std::mutex> lock(m_loadQueueMutex);
  while (m_loadQueue.size() == 0 && !m_stopThread) {
    m_wait.wait(m_loadQueueMutex);
  }
  if (m_stopThread) {
    return nullptr;
  }

  bd::Block *b{ m_loadQueue.back() };
  assert(b != nullptr && "A null block was found in the load queue");
  m_loadQueue.pop_back();

  return b;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::queueClassified(std::vector<bd::Block *> const &visible,
                             std::vector<bd::Block *> const &empty)
{
  bd::Dbg() << "Visible: " << visible.size() << ", empty: " << empty.size();
  // we hold the load queue mutex here because the load thread shouldn't be doing
  // any work while we sort (literally) things out.
  std::unique_lock<std::mutex> lock(m_loadQueueMutex);
  m_loadQueue.clear();

  {
    // clear the gpu ready queue.
    std::unique_lock<std::mutex> lock_gpuReady(m_gpuReadyMutex);
    std::queue<bd::Block *> empty_q;
    m_gpuReadyQueue.swap(empty_q);
  }

  // remove only empty blocks from the GPU and put the textures 
  // back in the list of available textures.
  removeEmptyBlocksFromGpu();

  // figure out if we must evict blocks from main memory.
  // queue all blocks not in main memory for loading by the loader thread.
  // if the block is already in main, then assign it a texture.
  for (size_t i{ 0 }; i < visible.size(); ++i) {
    bd::Block *vis{ visible[i] };
    assert(vis != nullptr && "Block was null when iterating visible blocks.");
    if (m_main.find(vis->index()) == m_main.end()) {
      // The block is not in main, so it needs to be loaded from disk, pushed to main,
      // and finally pushed to the gpu ready queue.
      // The load thread (running in operator()) pushes to the
      // gpu ready queue and the render thread pops from the gpu ready queue and uploads
      // to the gpu, then pushes the block pointer to the gpu resident queue.
      assert(m_gpu.find(vis->index()) == m_gpu.end() &&
             "Block is not in main, but is in gpu!");
      m_loadQueue.push_back(vis);
    }
    else if (m_gpu.find(vis->index()) == m_gpu.end()) {
      // The block is not on the gpu yet, but it is in main,
      // so push to the gpu queue. If it has a texture, it is ready to go, 
      // but if it needs a texture, give it one.
      if (vis->texture() != nullptr) {
        pushGPUReadyQueue(vis);
      }
      else if (m_texs.size() > 0) {
        vis->texture(m_texs.back());
        m_texs.pop_back();
        pushGPUReadyQueue(vis);
      }
    }
  } // for



  // if the load queue is larger than the number of available textures,
  // this means we won't be able to load them all to the gpu. Scan the
  // main for empties with textures (there probably won't be any)
  // we we must evict empty blocks from main and recover their texture and pixel buffers.

  long long num_to_evict{ static_cast<long long>(m_loadQueue.size()) -
                              static_cast<long long>(m_texs.size()) };
  if (num_to_evict > 0) {
    bd::Dbg() << "Need to evict " << num_to_evict
      << " blocks (LQ Size: " << m_loadQueue.size()
      << ", Texs avail: " << m_texs.size() << ").";

    // We have more blocks than there are available memory slots, so we need to
    // evict some empties.
    auto it = m_main.begin();
    auto end = m_main.end();
    while (num_to_evict > 0 && it != end) {
      bd::Block *b{ it->second };
      assert(b != nullptr && "Block was null when iterating main to evict empty blocks.");
      if (b->empty()) {
        char *buff{ b->removePixelData() };
        m_buffs.push_back(buff);
        it = m_main.erase(it);
        --num_to_evict;
      }
      else {
        ++it;
      }
    }

    // If we did not evict enough blocks from main to fit the entire load queue
    // into memory, then pop all the blocks out of the load queue that are
    // remaining.
    while (num_to_evict > 0) {
      m_loadQueue.pop_back();
      num_to_evict--;
    }
  }

  // sort blocks in ROV descending order
  std::sort(m_loadQueue.begin(), m_loadQueue.end(),
            [](bd::Block *lhs, bd::Block *rhs) -> bool {
    return lhs->fileBlock().rov > rhs->fileBlock().rov;
  });

  m_wait.notify_all();
}


///////////////////////////////////////////////////////////////////////////////
bd::Block *
BlockLoader::getNextGpuReadyBlock()
{
  //we are on the render thread since it is the only thread that
  //may upload texture data to OGL server.
  std::unique_lock<std::mutex> lock(m_gpuReadyMutex);
  bd::Block *b{ nullptr };
  if (m_gpuReadyQueue.size() > 0) {
    b = m_gpuReadyQueue.front();
    assert(b != nullptr && "Block was null in gpuReadyQueue()");
    m_gpuReadyQueue.pop();
  }

  return b;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::pushGpuResidentBlock(bd::Block *b)
{
  assert(b != nullptr && "Block was nullptr!");
  assert(b->texture() != nullptr && "Block had a null texture!");

  std::unique_lock<std::mutex> lock(m_gpuMutex);
  m_gpu.insert(std::make_pair(b->index(), b));
}


///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::clearLoadQueue()
{
  std::unique_lock<std::mutex> lock(m_loadQueueMutex);
  m_loadQueue.clear();
}


///////////////////////////////////////////////////////////////////////////////
size_t
BlockLoader::maxMainBlocks()
{
  return m_maxMainBlocks;
}


///////////////////////////////////////////////////////////////////////////////
size_t 
BlockLoader::maxGpuBlocks()
{
  return m_maxGpuBlocks;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::removeEmptyBlocksFromGpu()
{
  for (auto it = m_gpu.begin(); it != m_gpu.end(); ) {
    bd::Block *b{ it->second };
    assert(b != nullptr && "Block was null when iterating gpu blocks.");
    if (b->empty()) {
      bd::Texture *e{ b->removeTexture() };
      assert(e != nullptr && "A block in the GPU list had a null texture.");
      m_texs.push_back(e);
      it = m_gpu.erase(it);
    }
    else {
      ++it;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::pushGPUReadyQueue(bd::Block *b)
{
  assert(b != nullptr && "Block was nullptr!");
  assert(b->texture() != nullptr && "Block had a null texture!");

  std::unique_lock<std::mutex> lock(m_gpuReadyMutex);
  m_gpuReadyQueue.push(b);
}

///////////////////////////////////////////////////////////////////////////////
//void
//BlockLoader::fillBlockData(bd::Block *b, std::istream *infile,
//                           size_t sizeType, size_t vX, size_t vY) const
//{
//
//  // block's dimensions in voxels
//  glm::u64vec3 const be{ b->voxel_extent() };
//  // start element = block index w/in volume * block size
//  glm::u64vec3 const start{ b->ijk() * be };
//  // block end element = block voxel start voxelDims + block size
//  glm::u64vec3 const end{ start + be };
//
//  size_t const blockRowLength{ be.x };
//  //size_t const sizeType{ to_sizeType(b->texture()->dataType()) };
//
//  // byte offset into file to read from
//  size_t offset{ b->fileBlock().data_offset };
//
//  uint64_t const buf_elems{ be.x * be.y * be.z };
//  //TODO: support for than char*
//  char * const disk_buf{ new char[buf_elems] };
//  char *temp = disk_buf;
//  // Loop through rows and slabs of volume reading rows of voxels into memory.
//  for (uint64_t slab = start.z; slab < end.z; ++slab) {
//    for (uint64_t row = start.y; row < end.y; ++row) {
//
//      // seek to start of row
//      infile->seekg(offset);
//
//      // read the bytes of current row
//      infile->read(temp, blockRowLength * sizeType);
//      temp += blockRowLength;
//
//      // offset of next row
//      offset = bd::to1D(start.x, row + 1, slab, vX, vY);
//      offset *= sizeType;
//    }
//  }
//
//  float * const pixelData = reinterpret_cast<float *>(b->pixelData());
//  //Normalize the data prior to generating the texture.
//  for (size_t idx{ 0 }; idx < buf_elems; ++idx) {
//    pixelData[idx] = (disk_buf[idx] - m_volMin) / m_volDiff;
//  }
//
//  delete [] disk_buf;
//
//}

} // namespace bd








//    uint64_t idx{ b->fileBlock().block_index };
//
//    // First we load the block: If the block is not in memory at all we must load it to
//    // main memory. find a spot for it.
//    // First, check to see if there are any free blocks on the gpu. If there are,
//    // then there are also free blocks on the cpu. We must remove the empty block
//    // from both empties lists, remove it from both non-empties lists and then
//    // put our non-empty block in both visible lists.
//
//    if (m_gpu.find(idx) == m_gpu.end()) {
//      if (m_main.find(idx) == m_main.end()) {
//        if (m_gpuEmpty.size() > 0) {
//
//          // get an empty block and fill, remove from cpu empty as well
//        } else if (m_mainEmpty.size() > 0) {
//          // put it in the cpu anyway
//        }
//        assert(m_mainEmpty.size() > 0);
//        fileWithBufferFromEmptyBlock(b);
//        queueBlockAtFront(b);
//      }
//    } else {
//        // is in main & not gpu
//      pushLoadablesQueue(b);
//    }
//
//
//
//
//
////////////////////////////////////////////////////////////
//    {
//      // Not in GPU, check the CPU cache
//      if (!m_main.exists(idx)) {
//        // Not in CPU, or GPU
//        // b must be loaded to CPU.
//        char *pixData{ nullptr };
//        if (cpu.size() == maxMainBlocks || buffers->size() == 0) {
//          // Cpu full, evict a non-visible block
//          bd::Block *notvis{ removeLastInvisibleBlock(cpu) };
//          if (notvis) {
//            // there was a non-visible block in the cpu cache,
//            // take it's pixel buffer.
//            bd::Dbg() << "Removed block " << notvis->fileBlock().block_index
//              << " (" << notvis->status() << ") from CPU cache.";
//            pixData = notvis->pixelData();
//            notvis->pixelData(nullptr);
//          }
//        } else {
//          // the cpu cache is not full, so grab a free buffer.
//          if (buffers->size() > 0) {
//            pixData = buffers->back();
//            buffers->pop_back();
//          }
//        }
//
//        if (pixData) {
//          b->pixelData(pixData);
//          fillBlockData(b, &raw, sizeType, slabWidth, slabHeight);
//          // loaded to memory, so put in cpu cache.
//          cpu.push_front(b);
//          // put back to load queue so it will be processed for GPU.
//          queueBlockAtFront(b);
//          bd::Dbg() << "Block " << b->fileBlock().block_index
//            << " (" << b->status() << ") ready for texture.";
//        }
//
//      } // ! cpu list
//      else {
//        // Is in cpu, but not in GPU
//        // Give b a texture so it will be uploaded to GPU
//        bd::Dbg() << "Block " << b->fileBlock().block_index
//                  << " (" << b->status() << ") " "found in CPU cache.";
//        bd::Texture *tex{ nullptr };
//
//        if (m_gpu.size() == maxGpuBlocks || texs->size() == 0) {
//
//          // no textures, evict non-vis from m_gpu
//          bd::Block *notvis{ removeGpuLastInvisible() };
//          if (notvis) {
//            // nonvis block found, give it a texture
//            tex = notvis->removeTexture();
//            bd::Dbg() << "Removed block " << notvis->fileBlock().block_index
//                      << " (" << notvis->status() << ") from GPU cache.";
//          }
//
//        } else {
//          // m_gpu cache not full yet, so grab a texture from cache.
//          if (texs->size() > 0) {
//            tex = texs->back();
//            texs->pop_back();
//          }
//        }
//
//        if (tex) {
//          b->texture(tex);
//          pushLoadablesQueue(b);
//          bd::Dbg() << "Queued block " << b->fileBlock().block_index
//            << " (" << b->status() << ") for GPU upload.";
//        }
//      }
//    } // ! m_gpu list
//    else {
//      // the block is already in m_gpu cache
//      bd::Dbg() << "Block " << b->fileBlock().block_index
//                << " (" << b->status() << ") " " found in GPU cache.";
////      if (b->status() & bd::Block::GPU_WAIT) {
////      }
//    }

