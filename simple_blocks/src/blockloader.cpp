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
  , m_texs{ *(threadParams->texs) }
  , m_buffs{ *(threadParams->buffers) }
  , m_loadQueue{ }
  , m_gpuReadyQueue{ }
  , m_maxGpuBlocks{ threadParams->maxGpuBlocks }
  , m_maxMainBlocks{ threadParams->maxCpuBlocks }
  , m_sizeType{ threadParams->size }
  , m_slabWidth{ threadParams->slabDims[0] }
  , m_slabHeight{ threadParams->slabDims[1] }
  , m_volMin{ volume.min() }
  , m_volDiff{ volume.max() - volume.min() }
  , m_fileName{ threadParams->filename }
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

//bd::Block *
//removeLastInvisibleBlock(std::list<bd::Block *> &list)
//{
//  auto rend = list.end();
//  auto not_vis =
//      std::find_if(list.begin(), rend,
//                   [](bd::Block *be) -> bool {
//                     return ( be->status() & bd::Block::VISIBLE ) == 0;
//                   });
//
//  if (not_vis == rend) {
//    // no non-visible blocks in the cpu queue
//    // (only happens if size m_gpu == size cpu)
//    return nullptr;
//  }
//
////  std::advance(not_vis, 1);
////  auto byebye = list.erase( not_vis.base() );
//  auto byebye = list.erase(not_vis);
//  return *byebye;
//}


//bool
//isInList(bd::Block *b, std::list<bd::Block *> &list)
//{
//  auto end = list.end();
//  auto found = std::find(list.begin(), end, b);
//  return found != end;
//}

//void
//printBar(size_t ticks, char const *prefix)
//{
////  unsigned char bar[81] = { 255 }; // white space
//  std::cout << '\r' << prefix << " cache: ";
//  for (size_t i=0; i<ticks; ++i) {
//    std::cout << '.'; //static_cast<char>(178);
//  }
//}



} // namespace


//void
//BlockLoader::fileWithBufferFromEmptyBlock(bd::Block *b)
//{
//  block_type *empty = m_mainEmpty.front();
//  m_mainEmpty.pop();
//
//  auto index = empty->fileBlock().block_index;
//
//  // evict empty from m_main.
//  auto it = m_main.find(index);
//  assert(it != m_main.end());
//  m_main.erase(it);
//
//  bd::Texture *t = nullptr;
//  auto gpuIt = m_gpu.find(index);
//  assert(gpuIt != m_gpu.end());
//  m_gpu.erase(index);
//
//
//  char *pixels = empty->pixelData();
//  empty->pixelData(nullptr);
//  assert(pixels != nullptr);
//  b->pixelData(pixels);
//
//  fillBlockData(b, &m_raw, m_sizeType, m_slabWidth, m_slabHeight);
//  m_main.insert(std::make_pair(b->fileBlock().block_index, b));
//}


int
BlockLoader::operator()()
{
  bd::Dbg() << "Load thread started.";
  raw.open(m_fileName, std::ios::binary);
  if (!raw.is_open()) {
    bd::Err() << "The raw file " << m_fileName
              << " could not be opened. Exiting loader loop.";
    return -1;
  }


  while (!m_stopThread) {
    // get a block marked as visible
    bd::Block *b{ waitPopLoadQueue() };

    if (!b) {
      bd::Info() << "Exiting load loop.";
      continue;
    }

    bd::Dbg() << "Loading " << (b->empty() ? "empty" : "non-empty")
              << " block " << b->index()
              << " (" << b->status() << ").";


    if (b->empty()) {
      handleEmptyBlock(b);
    } else {
      handleVisibleBlock(b);
    }


    std::cout << "Cpu: " << m_main.size() << "/" << m_maxMainBlocks << "\n"
        "Gpu: " << m_gpu.size() << "/" << m_maxGpuBlocks << "\n"
        "LdQ: " << m_loadQueue.size() << "\n"
        "Ldb: " << m_gpuReadyQueue.size() << "\n"
        "Buf: " << m_buffs.size() << "\n"
        "Tex: " << m_texs.size() << std::endl;

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
  m_loadQueue.pop_back();

  return b;
}


//bd::Block *
//BlockLoader::removeGpuLastInvisible()
//{
//  std::unique_lock<std::mutex>(m_gpuMutex);
//  return removeLastInvisibleBlock(m_gpu);
//}


//bd::Block *
//BlockLoader::removeGpuBlockReverse(bd::Block *b)
//{
//  std::unique_lock<std::mutex>(m_gpuMutex);
//  auto found = std::find(m_gpu.begin(), m_gpu.end(), b);
//  if (found == m_gpu.end()) {
//    return nullptr;
//  }
//
//  m_gpu.erase(found);
//  return *found;
//}


size_t
BlockLoader::findEmptyBlocks(std::unordered_map<uint64_t, bd::Block *> const &r,
                             std::set<bd::Block *> &er)
{
  size_t found{ 0 };
  for (auto &kv : r) {
    if (kv.second->empty()) {
      er.insert(kv.second);
      ++found;
    }
  }
  return found;
}


//void
//BlockLoader::swapTexture(bd::Block *src, bd::Block *dest)
//{
//  assert(src != nullptr);
//  assert(dest != nullptr);
//  assert(src->texture() != nullptr);
//  assert(dest->texture() == nullptr);
//
//  bd::Texture *p{ src->texture() };
//  src->texture(dest->texture());
//  dest->texture(p);
//
//}

///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::swapPixel(bd::Block *src, bd::Block *dest)
{
  assert(src != nullptr);
  assert(dest != nullptr);
  assert(src->pixelData() != nullptr);
  assert(dest->pixelData() == nullptr);

  char *p{ src->pixelData() };
  src->pixelData(dest->pixelData());
  dest->pixelData(p);
}


///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::handleEmptyBlock(bd::Block *b)
{
  auto idx = b->index();
  if (m_gpu.find(idx) != m_gpu.end())
  {
    // if it is in gpu, then it better be in main.
    assert(m_main.find(idx) != m_main.end());
    m_gpuEmpty.insert(b);
    m_mainEmpty.insert(b);
  } else if (m_main.find(idx) != m_main.end()) {
    // if it is in main, but not in gpu, then it better not be
    // in gpu empty.
    assert(m_gpuEmpty.find(b) == m_gpuEmpty.end());
    // if it is in main, then put it in main empty
    m_mainEmpty.insert(b);
  } else {
    // it is not in either cpu or main, so remove from empties
    m_mainEmpty.erase(b);
    m_gpuEmpty.erase(b);
  }
}


///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::handleVisible_NotInGPU_IsInMain(bd::Block *b)
{
  auto stealEmptyBlockTexture = [b](decltype(m_mainEmpty) &main_e,
                                    decltype(m_gpu) &gpu,
                                    decltype(m_gpuEmpty) &gpu_e) -> void
  {
    // grab empty gpu block.
    auto emptyIt = gpu_e.begin();
    assert(emptyIt != gpu_e.end());
    bd::Block *eblk{ *emptyIt };
    assert(eblk->pixelData() != nullptr);
    assert(eblk->texture() != nullptr);

    bd::Dbg() << "Stealing block " << eblk->index() << " texture.";
    b->texture(eblk->texture());
    eblk->texture(nullptr);

    gpu.erase(eblk->index());
    main_e.erase(eblk);
    gpu_e.erase(emptyIt);
  };

  assert(b->pixelData() != nullptr);

  if (m_gpuEmpty.size() > 0) {
    stealEmptyBlockTexture(m_mainEmpty, m_gpu, m_gpuEmpty);
  } else {
    size_t found{ findEmptyBlocks(m_gpu, m_gpuEmpty) };
    if (found > 0) {
      stealEmptyBlockTexture(m_mainEmpty, m_gpu, m_gpuEmpty);
    } else {
      if (m_texs.size() > 0) {
        bd::Texture *tex{ m_texs.back() };
        m_texs.pop_back();
        b->texture(tex);
      }
    }
  }
//  assert(b->texture() != nullptr);
  if (b->texture() != nullptr) {
    pushGPUReadyQueue(b);
  } else {
    bd::Dbg() << "Could not add block " << b->index() << " (" << b->status() << ") "
              << "no free textures or empty blocks available.";
  }

//  bd::Block *empty{ nullptr };
//  bd::Texture *tex{ nullptr };
//    // find empty block on GPU
//    auto first = m_gpuEmpty.begin();
//
//    if (first == m_gpuEmpty.end()) {
//      size_t found{ findEmptyBlocks(m_gpu, m_gpuEmpty) };
//      if (found == 0) {
//        tex = m_texs.back();
//        m_texs.pop_back();
//      } else {
//
//      }
//
//      bd::Dbg() << "Can't load visible block " << b->index() << " that is already on cpu, no room on GPU";
//      return;
//    }
//
//    empty = { *first };
//    m_gpuEmpty.erase(first);
//    m_mainEmpty.erase(empty);
//    m_gpu.erase(empty->index());
//    assert(m_main.find(empty->index()) != m_main.end());
//
//  char *pixels{ empty->pixelData() };
//  b->pixelData(pixels);
//
//  bd::Texture *tex{ b->texture() };
//  b->texture(tex);
//  pushGPUReadyQueue(b);
}


///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::handleVisible_NotInGPU_NotInMain(bd::Block *b)
{
//  bd::Block *empty{ nullptr };
//  auto first = m_gpuEmpty.begin();
//  if (first == m_gpuEmpty.end()) {
//    bd::Dbg() << "Can't load visible block " << b->index() << " from disk, no room on Gpu.";
//    return;
//  }

  auto stealEmptyBlockPixelData = [&b,this](decltype(m_mainEmpty) &main_e,
                                       decltype(m_main) &main) {

    auto emptyIt = main_e.begin();

    assert((*emptyIt)->pixelData() != nullptr);
    assert(b->pixelData() == nullptr);
    assert(b->texture() == nullptr);

    char *pix{ (*emptyIt)->removePixelData() };
    b->pixelData(pix);

    fillBlockData(b, &(this->raw), this->m_sizeType, this->m_slabWidth, this->m_slabHeight);

    main.erase((*emptyIt)->index());
    main_e.erase(emptyIt);
    main.insert(std::make_pair(b->index(), b));

  };


  if (m_mainEmpty.size() > 0) {
    stealEmptyBlockPixelData(m_mainEmpty, m_main);
  } else {
    // If no blocks are e-resident in main's empty list, it is still possible that
    // e-resdient blocks exist in main, but haven't been placed into
    // the empty list.
    size_t found{ findEmptyBlocks(m_main, m_mainEmpty) };
    if (found != 0) {
      stealEmptyBlockPixelData(m_mainEmpty, m_main);
    } else {
      // we didn't find any empty blocks, our last ditch effort is to see if we have
      // buffers in the buffer buffer (as is the case with first run).
      if (m_buffs.size() > 0) {

        assert(b->pixelData() == nullptr);

        char *buf{ m_buffs.back() };
        m_buffs.pop_back();
        b->pixelData(buf);

        fillBlockData(b, &raw, m_sizeType, m_slabWidth, m_slabHeight);
        m_main.insert(std::make_pair(b->index(), b));
      } else {
        bd::Dbg() << "Block " << b->index() << " not loaded into cpu, no free buffers.";
        return;
      }
    }
  }

  // Enqueue this block, so that when the load loop comes back around it can get
  // setup for loading to the GPU.
  queueBlockAtFrontOfLoadQueue(b);
}


///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::handleVisibleBlock(bd::Block *b)
{
  auto idx = b->index();
  if (m_gpu.find(idx) != m_gpu.end()) {
    // it is in the gpu list already, so nothing needs
    // to be done to it, it is already on the gpu and ready to
    // be drawn.
    assert(m_main.find(idx) != m_main.end());
    assert(m_mainEmpty.find(b) == m_mainEmpty.end());
    assert(m_gpuEmpty.find(b) == m_gpuEmpty.end());
    return;
  } else if (m_main.find(idx) != m_main.end()) {
    // if is not in the gpu list, but is in the cpu list
    handleVisible_NotInGPU_IsInMain(b);
  } else {
    // not in cpu or gpu.
    handleVisible_NotInGPU_NotInMain(b);
  }
}

//bool
//BlockLoader::isInGpuList(bd::Block *b)
//{
//  std::unique_lock<std::mutex>(m_gpuMutex);
//  return isInList(b, m_gpu);
//}


///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::queueBlockAtFrontOfLoadQueue(bd::Block *b)
{
  std::unique_lock<std::mutex> lock(m_loadQueueMutex);
  m_loadQueue.push_back(b);
  m_wait.notify_all();
}


///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::queueClassified(std::vector<bd::Block *> const &visible,
                             std::vector<bd::Block *> const &empty)
{
  std::unique_lock<std::mutex> lock(m_loadQueueMutex);
  m_loadQueue.clear();

  bd::Dbg() << "Queueing " << visible.size() << " visible blocks for processing.";
  for (size_t i = 0; i < visible.size(); ++i) {
    m_loadQueue.push_back(visible[i]);
  }

  std::sort(m_loadQueue.begin(), m_loadQueue.end(),
            [](bd::Block *lhs, bd::Block *rhs) -> bool {
              return lhs->fileBlock().rov > rhs->fileBlock().rov;
            });

//  bd::Dbg() << "Queueing " << empty.size() << " empty blocks for processing.";
//  for (size_t i{ 0 }; i < empty.size(); ++i) {
//    m_loadQueue.push_back(empty[i]);
//  }



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


///////////////////////////////////////////////////////////////////////////////
bd::Block *
BlockLoader::getNextGpuReadyBlock()
{
  std::unique_lock<std::mutex> lock(m_gpuReadyMutex);
  bd::Block *b{ nullptr };
  if (m_gpuReadyQueue.size() > 0) {
    b = m_gpuReadyQueue.front();
    m_gpuReadyQueue.pop();
  }

  return b;
}


///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::pushGpuResidentBlock(bd::Block *b)
{
  std::unique_lock<std::mutex> lock(m_gpuMutex);
  assert(m_gpu.find(b->index()) == m_gpu.end() &&
             ("Block: " + std::to_string(b->index())).c_str());
  m_gpu.insert(std::make_pair(b->index(), b));
}


///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::clearLoadQueue()
{
  std::unique_lock<std::mutex> lock(m_loadQueueMutex);
  m_loadQueue.clear();
//  std::queue<bd::Block *> q;
//  m_loadQueue.swap(q);
}



///////////////////////////////////////////////////////////////////////////////
void
BlockLoader::pushGPUReadyQueue(bd::Block *b)
{
  std::unique_lock<std::mutex> lock(m_gpuReadyMutex);
  m_gpuReadyQueue.push(b);
}

///////////////////////////////////////////////////////////////////////////////
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
  //TODO: support for than char*
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

