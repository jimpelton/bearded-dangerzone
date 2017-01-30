//
// Created by jim on 1/6/17.
//

#ifndef bd_blockloader_h
#define bd_blockloader_h

#include <bd/volume/block.h>
#include <bd/datastructure/blockingqueue.h>
#include <bd/volume/volume.h>

#include <string>
#include <atomic>
#include <vector>
#include <list>

namespace subvol
{

struct BLThreadData
{
  BLThreadData()
      : maxGpuBlocks{0}
      , maxCpuBlocks{0}
      , size{ 1 }
      , slabDims{0,0}
      , filename{ }
      , texs{ nullptr }
      , buffers{ nullptr } { }

  size_t maxGpuBlocks;
  size_t maxCpuBlocks;
  // size of data elements on disk
  size_t size;
  // x, y dims of volume slab
  size_t slabDims[2];

  std::string filename;
  std::vector<bd::Texture *> *texs;
  std::vector<char *> *buffers;

};

/// Threaded load block data from disk. Blocks to load are put into a queue by
/// a thread. 
class BlockLoader
{
public:

  BlockLoader(BLThreadData *, bd::Volume const &);

//  BlockLoader(BlockLoader const &);
//  BlockLoader(BlockLoader const &&);
//  
//  BlockLoader &
//    operator=(BlockLoader const &);
//
//  BlockLoader &
//    operator=(BlockLoader const &&);

  ~BlockLoader();


  int
  operator()();


  void
  stop();

  
  void
  queueBlockAtFront(bd::Block *block);


  /// \brief Enqueue the provided blocks for loading.
  void
  queueAll(std::vector<bd::Block *> &visibleBlocks);


  /// \brief get the next block that is ready to load to gpu.
  /// \returns nullptr if no blocks in queue, or the next loadable block.
  bd::Block *
  getNextGpuBlock();

  void
  pushGpuResBlock(bd::Block *);


  void
  clearCache();

private:


  bd::Block* 
  waitPopLoadQueue();

  bd::Block *
  removeGpuLastInvisible();

  bd::Block *
  removeGpuBlockReverse(bd::Block *);

 

  bool
  isInGpuList(bd::Block*);


  void
  pushLoadablesQueue(bd::Block *b);

  void
  fillBlockData(bd::Block *b, std::istream *infile, size_t szTy, size_t vX, size_t vY) const;


  std::vector<bd::Block *> m_loadQueue;   ///< Blocks that will be examined for loading.
  std::queue<bd::Block *> m_loadables;   ///< Blocks with GPU_WAIT status.
  std::list<bd::Block *> m_gpu;          ///< Blocks with GPU_RES status.

  std::atomic_bool m_stopThread;
  std::mutex m_gpuMutex;
  std::mutex m_loadablesMutex;
  std::mutex m_mutex;

  std::condition_variable_any m_wait;

  BLThreadData *dptr;

  double m_volMin;
  double m_volDiff;                  ///< diff = volMax - volMin

}; // class BlockLoader

}
#endif //! bd_blockloader_h
