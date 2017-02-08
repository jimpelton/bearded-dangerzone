//
// Created by jim on 1/6/17.
//

#ifndef bd_blockloader_h
#define bd_blockloader_h

#include <bd/volume/block.h>
#include <bd/volume/volume.h>

#include <string>
#include <atomic>
#include <vector>
#include <list>
#include <unordered_map>
#include <queue>
#include <condition_variable>

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

template <class Key, class Value>
class lru_cache
{
public:
  using value_type = typename std::pair<Key, Value>;
  using value_it = typename std::list<value_type>::iterator;
  using operation_guard = typename std::lock_guard<std::mutex>;

  lru_cache(size_t max_size) : max_cache_size{max_size} {
    if (max_size == 0) {
      max_cache_size = std::numeric_limits<size_t>::max();
    }
  }

  void put(const Key& key, const Value& value) {
    operation_guard og{safe_op};
    auto it = cache_items_map.find(key);

    if (it == cache_items_map.end()) {
      if (cache_items_map.size() + 1 > max_cache_size) {
        // remove the last element from cache
        auto last = cache_items_list.crbegin();

        cache_items_map.erase(last->first);
        cache_items_list.pop_back();
      }

      cache_items_list.push_front(std::make_pair(key, value));
      cache_items_map[key] = cache_items_list.begin();
    }
    else {
      it->second->second = value;
      cache_items_list.splice(cache_items_list.cbegin(), cache_items_list,
                              it->second);
    }
  }

  const Value& get(const Key& key) const {
    operation_guard og{safe_op};
    auto it = cache_items_map.find(key);

    if (it == cache_items_map.end()) {
      throw std::range_error("No such key in the cache");
    }
    else {
      cache_items_list.splice(cache_items_list.begin(), cache_items_list,
                              it->second);

      return it->second->second;
    }
  }

  bool exists(const Key& key) const noexcept {
    operation_guard og{safe_op};

    return cache_items_map.find(key) != cache_items_map.end();
  }

  size_t size() const noexcept {
    operation_guard og{safe_op};

    return cache_items_map.size();
  }

private:
  mutable std::list<value_type> cache_items_list;
  std::unordered_map<Key, value_it> cache_items_map;
  size_t max_cache_size;
  mutable std::mutex safe_op;

};

/// Threaded load block data from disk. Blocks to load are put into a queue by
/// a thread. 
class BlockLoader
{
public:

  BlockLoader(BLThreadData *, bd::Volume const &);



  ~BlockLoader();


  int
  operator()(std::string const &);


  void
  stop();

  
  void
  queueBlockAtFront(bd::Block *block);


  /// \brief Enqueue the provided blocks for loading.
  void
  queueAll(std::vector<bd::Block *> &visibleBlocks, std::vector<bd::Block*>& nonVisibleBlocks);


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

  void 
  handleEmptyBlock(bd::Block *);

  void
  handleVisisbleBlock(bd::Block *);
 

  bool
  isInGpuList(bd::Block*);


  void
  pushLoadablesQueue(bd::Block *b);

  void
  fillBlockData(bd::Block *b, std::istream *infile, size_t szTy, size_t vX, size_t vY) const;

  void
  fileWithBufferFromEmptyBlock(bd::Block *b);


  std::vector<bd::Block *> m_loadQueue;   ///< Blocks that will be examined for loading.
  std::queue<bd::Block *> m_loadables;   ///< Blocks with GPU_WAIT status.

  std::queue<uint64_t, bd::Block *> m_gpuEmpty;
  std::unordered_map<uint64_t, bd::Block *> m_gpu;
  std::queue<bd::Block *> m_mainEmpty;
  std::unordered_map<uint64_t, bd::Block *> m_main;



  std::atomic_bool m_stopThread;
  std::mutex m_gpuMutex;
  std::mutex m_loadablesMutex;
  std::mutex m_mutex;

  std::condition_variable_any m_wait;

//  BLThreadData *dptr;


  size_t const m_maxGpuBlocks;
  size_t const m_maxMainBlocks;
  size_t const m_sizeType;
  size_t const m_slabWidth;
  size_t const m_slabHeight;

  double const m_volMin;
  double const m_volDiff;                  ///< diff = volMax - volMin

}; // class BlockLoader

}
#endif //! bd_blockloader_h
