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

  /**
   *
   * @param buffer The pixel buffer
   * @param infile The file to read from
   * @param offset The byte offset into the file to start reading at
   * @param be The block extent in voxels
   * @param ijk The block index
   * @param ve The extent of a slab in the volume
   * @param vMin The min value in the volume
   * @param vDiff The difference of volume max and volume min.
   */
  virtual void
  fillBlockData(char * buffer,
                std::istream *infile,
                uint64_t offset,
                uint64_t const be[3],
                uint64_t const ijk[3],
                uint64_t const ve[2],
                double vMin, double vDiff) = 0;

};
//
//
template<class VTy>
class BlockReaderSpec : public BlockReader
{
public:
	BlockReaderSpec() : disk_buf{ nullptr }, buf_len{ 0 } { }

  virtual ~BlockReaderSpec()
  {
    if (disk_buf) {
      delete [] disk_buf;
    }
  }

  virtual void
  fillBlockData(char *b,
                std::istream *infile,
                uint64_t offset,
                uint64_t const be[3],
                uint64_t const ijk[3],
                uint64_t const ve[2],
                double vMin, double vDiff) override
  {
    if (! disk_buf) {
      // allocate temp space for the block (the entire block is brought into mem).
      buf_len = be[0] * be[1] * be[2];
      disk_buf = new VTy[ buf_len ];
    }

    // start element = block index w/in volume * block size
    // (this works because all blocks are the same size).
    glm::u64vec3 const start{ ijk[0] * be[0],
                              ijk[1] * be[1],
                              ijk[2] * be[2] };

    // block end element = block voxel start voxelDims + block size
    glm::u64vec3 const end{ start[0] + be[0],
                            start[1] + be[1],
                            start[2] + be[2] };

    // the row length of each block is the extent in the X dimension
    size_t const blockRowLength{ be[0] };
    size_t const sizeType = sizeof(VTy);
    size_t const rowBytes{ blockRowLength * sizeType };

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
        offset = bd::to1D(start.x, row + 1, slab, ve[0], ve[1]);
        offset *= sizeType;
      }
    }

    float * const pixelData = reinterpret_cast<float *>(b);
    //Normalize the data prior to generating the texture.
    for (size_t idx{ 0 }; idx < buf_len; ++idx) {
      pixelData[idx] = static_cast<float>( (disk_buf[idx] - vMin) / vDiff );
    }

  }
private:
  VTy *disk_buf;
  size_t buf_len;

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
      case T::Character:
        return new BlockReaderSpec<int8_t>();
      case T::UnsignedShort:
        return new BlockReaderSpec<uint16_t>();
      case T::Short:
        return new BlockReaderSpec<int16_t>();
      case T::Float:
      default:
        return new BlockReaderSpec<float>();
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

  bd::Block*
  waitPopLoadQueue();


  /// \brief Loop through gpu blocks (m_gpu) and remove any that are empty.
  /// Place textures back into m_texs array.
  void
  removeEmptyBlocksFromGpu();


  /// Push a block that is ready for loading to the GPU.
  /// \param b
  void
  pushGPUReadyQueue(bd::Block *b);


  std::atomic_bool m_stopThread;

  /// NE-resident on gpu.
  /// Also NE-resident on cpu.
  std::unordered_map<uint64_t, bd::Block *> m_gpu;

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

  size_t const m_maxGpuBlocks;
  size_t const m_maxMainBlocks;
  size_t const m_sizeType;

  ///< Dimensions of the volume slabs (x and y dims of volume)
  uint64_t m_slabDims[2];

  double const m_volMin;
  double const m_volDiff;                  ///< diff = volMax - volMin

  std::string m_fileName;
  std::ifstream raw;

  BlockReader *m_reader;

}; // class BlockLoader

} // namespace subvol










#endif //! bd_blockloader_h
