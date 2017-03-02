//
// Created by jim on 1/6/17.
//

#ifndef bd_blockloader_h
#define bd_blockloader_h

#include <bd/volume/block.h>
#include <bd/volume/volume.h>
#include <bd/util/util.h>

#include <string>
#include <atomic>
#include <vector>
#include <list>
#include <unordered_map>
#include <queue>
#include <condition_variable>
#include <set>
#include <fstream>

namespace subvol
{

struct BLThreadData
{
  BLThreadData()
      : maxGpuBlocks{0}
      , maxCpuBlocks{0}
      , type{ bd::DataType::UnsignedCharacter }
      , slabDims{0,0}
      , filename{ }
      , texs{ nullptr }
      , buffers{ nullptr } { }

  size_t maxGpuBlocks;
  size_t maxCpuBlocks;
  // size of data elements on disk
  bd::DataType type;
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

//template<class VTy> class BlockReaderSpec;

class BlockReader
{
public:
  BlockReader(){ }
  virtual ~BlockReader() { }

  virtual void
  fillBlockData(bd::Block *b, std::istream *infile,
                size_t vX, size_t vY,
                double vMin, double vDiff) const = 0;

};


template<class VTy>
class BlockReaderSpec : public BlockReader
{
public:

  virtual void
  fillBlockData(bd::Block *b, std::istream *infile,
                size_t vX, size_t vY,
                double vMin, double vDiff) const override
  {
    // block's dimensions in voxels
    glm::u64vec3 const be{ b->voxel_extent() };
    // start element = block index w/in volume * block size
    glm::u64vec3 const start{ b->ijk() * be };
    // block end element = block voxel start voxelDims + block size
    glm::u64vec3 const end{ start + be };

    size_t const blockRowLength{ be.x };
    size_t const sizeType = sizeof(VTy);
    size_t const rowBytes{ blockRowLength * sizeType };


    // byte offset into file to read from
    size_t offset{ b->fileBlock().data_offset };


    // allocate temp space for the block (the entire block is brought into mem).
    uint64_t const buf_len{ be.x * be.y * be.z };
    VTy * const disk_buf{ new VTy[buf_len] };


    // Loop through rows and slabs of volume reading rows of voxels into memory.
    char *temp = reinterpret_cast<char *>(disk_buf);
    for (uint64_t slab = start.z; slab < end.z; ++slab) {
      for (uint64_t row = start.y; row < end.y; ++row) {

        // seek to start of row
        infile->seekg(offset);

        // read the bytes of current row
        infile->read(temp, rowBytes);
        temp += rowBytes;

        // offset of next row
        offset = bd::to1D(start.x, row + 1, slab, vX, vY);
        offset *= sizeType;
      }
    }

    float * const pixelData = reinterpret_cast<float *>(b->pixelData());
    //Normalize the data prior to generating the texture.
    for (size_t idx{ 0 }; idx < buf_len; ++idx) {
      pixelData[idx] = (disk_buf[idx] - vMin) / vDiff;
    }

    delete [] disk_buf;
  }

};

class BlockReaderFactory
{
public:
  using T = bd::DataType;

  static
  BlockReader *
  New(bd::DataType ty)
  {
    switch(ty){
      case T::UnsignedCharacter:
        return new BlockReaderSpec<uint8_t>();
        break;
      case T::Character:
        return new BlockReaderSpec<int8_t>();
        break;
      case T::UnsignedShort:
        return new BlockReaderSpec<uint16_t>();
        break;
      case T::Short:
        return new BlockReaderSpec<int16_t>();
        break;
      case T::Float:
      default:
        return new BlockReaderSpec<float>();
        break;
    }
  }
};


/// Threaded load block data from disk. Blocks to load are put into a queue by
/// a thread.
class BlockLoader
{
public:

  BlockLoader(BLThreadData *, bd::Volume const &);


  ~BlockLoader();


  int
  operator()();


  void
  stop();

  


  /// \brief Enqueue the provided blocks for loading.
  /// First the non-vis blocks are pushed, then the
  /// vis blocks.
  void
  queueClassified(std::vector<bd::Block *> const &visible,
                  std::vector<bd::Block *> const &empty);


  /// \brief get the next block that is ready to load to gpu.
  /// \returns nullptr if no blocks in queue, or the next loadable block.
  bd::Block *
  getNextGpuReadyBlock();

  void
  pushGpuResidentBlock(bd::Block *);


  void
  clearLoadQueue();

private:

  void
  queueBlockAtFrontOfLoadQueue(bd::Block *block);

  bd::Block* 
  waitPopLoadQueue();


//  bd::Block *
//  removeGpuLastInvisible();

//  bd::Block *
//  removeGpuBlockReverse(bd::Block *);

//  void
//  swapPixel(bd::Block *src, bd::Block *dest);


//  void
//  swapTexture(bd::Block *src, bd::Block *dest);

  /// Find empty blocks in the resident blocks map \c r, and update the
  /// the empty-resident set, er.
  /// \param r The map to find blocks in.
  /// \param er The empty-resident set.
  /// \return Number of empty blocks found.
  size_t
  findEmptyBlocks(std::unordered_map<uint64_t, bd::Block*> const &r,
                  std::set<bd::Block *> &er);


//  void
//  handleEmptyBlock(bd::Block *);


//  void
//  handleVisibleBlock(bd::Block *);


//  void
//  handleVisible_NotInGPU_IsInMain(bd::Block *b);


//  void
//  handleVisible_NotInGPU_NotInMain(bd::Block *b);

//  bool
//  isInGpuList(bd::Block*);


  /// Push a block that is ready for loading to the GPU.
  /// \param b
  void
  pushGPUReadyQueue(bd::Block *b);


//  void
//  fillBlockData(bd::Block *b, std::istream *infile, size_t szTy, size_t vX, size_t vY) const;

//  void
//  fileWithBufferFromEmptyBlock(bd::Block *b);

  std::atomic_bool m_stopThread;

  /// E-resident on gpu
  /// Also E-resident on CPU
  std::set<bd::Block *> m_gpuEmpty;

  /// NE-resident on gpu.
  /// Also NE-resident on cpu.
  std::unordered_map<uint64_t, bd::Block *> m_gpu;

  /// E-resident on cpu.
  /// Could be E-resident on gpu (in gpu empty list).
  std::set<bd::Block *> m_mainEmpty;

  /// NE-resident on cpu.
  std::unordered_map<uint64_t, bd::Block *> m_main;

  /// Buffer of reserve textures.
  std::vector<bd::Texture *> m_texs;

  /// Buffer of reserve buffers.
  std::vector<char *> m_buffs;

  /// Blocks that will be examined for loading.
  std::vector<bd::Block *> m_loadQueue;

  ///< Blocks with GPU_WAIT status.
  std::queue<bd::Block *> m_gpuReadyQueue;

  std::mutex m_gpuMutex;
  std::mutex m_gpuReadyMutex;
  std::mutex m_loadQueueMutex;

  std::condition_variable_any m_wait;

//  BLThreadData *dptr;


  size_t const m_maxGpuBlocks;
  size_t const m_maxMainBlocks;
  size_t const m_sizeType;
  size_t const m_slabWidth;
  size_t const m_slabHeight;

  double const m_volMin;
  double const m_volDiff;                  ///< diff = volMax - volMin

  std::string m_fileName;
  std::ifstream raw;

  BlockReader *m_reader;

}; // class BlockLoader

} // namespace subvol










#endif //! bd_blockloader_h
