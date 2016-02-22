#ifndef block_collection_h__
#define block_collection_h__


#include <bd/volume/block.h>
#include <bd/util/util.h>
#include <bd/log/gl_log.h>

#include <istream>
#include <iostream>

class BlockCollection2
{

public:

  BlockCollection2();

  ~BlockCollection2();

  /////////////////////////////////////////////////////////////////////////////////
  /// \brief Set/get this BlockCollection2's dimensions in voxels
  /////////////////////////////////////////////////////////////////////////////////
  void
  blockDims(const glm::u64vec3& dims);

  glm::u64vec3
  blockDims();


  /////////////////////////////////////////////////////////////////////////////////
  /// \brief Set/get the volume's dimensions in voxels
  /////////////////////////////////////////////////////////////////////////////////
  glm::u64vec3
  volDims() const;

  void
  volDims(const glm::u64vec3& voldims);
  //TODO: move volDims() out of block class (yeah...probably need to make a class representing a volume).


  /////////////////////////////////////////////////////////////////////////////////
  /// \brief Initializes \c nb blocks so that they fit within the extent of \c vd.
  /// \param nb[in]      Number of blocks in x,y,z directions.
  /// \param vd[in]      Volume dimensions
  /// \param blocks[out] Vector that new blocks are pushed onto.
  ///////////////////////////////////////////////////////////////////////////////
  void
  initBlocks(glm::u64vec3 nb, glm::u64vec3 vd);


  /////////////////////////////////////////////////////////////////////////////////
  /// \brief Marks blocks as empty and uploads GL textures if average is outside of [tmin..tmax].
  /// \param data[in] Volume data set
  /// \param tmin[in] min average block value to filter against.
  /// \param tmax[in] max average block value to filter against.
  /// \param sampler[in] The sampler location of the block texture sampler.
  ///////////////////////////////////////////////////////////////////////////////
  //TODO: filterblocks takes Functor for thresholding.
  template<typename Ty>
  void
  filterBlocks(std::istream &rawFile, float tmin = 0.0f, float tmax = 1.0f);
  

  const std::vector<bd::Block>& 
  blocks() const;

  const std::vector<bd::Block *>& 
  nonEmptyBlocks() const;


  //////////////////////////////////////////////////////////////////////////////
  /// \brief Fills \c out_blockData with part of \c in_data corresponding to 
  ///        block (i,j,k).
  ///
  /// \param ijk[in]     ijk coords of the block whos data to get.
  /// \param bsz[in]     The size of the block data.
  /// \param volsz[in]   The size of the volume data s.t.
  ///                    volsz.x*volsz.y*volsz.z == length(in_data).
  /// \param in_data[in] Source data
  /// \param out[out] Destination space for data.
  //////////////////////////////////////////////////////////////////////////////
  template<typename Ty>
  void 
  fillBlockData(glm::u64vec3 ijk, std::istream &infile, Ty* out);

private:

  static glm::u64vec3 m_blockDims; ///< Dimensions of a block in something.
  static glm::u64vec3 m_volDims; ///< Volume dimensions (# data points).
  static glm::u64vec3 m_numBlocks; ///< Number of blocks volume is divided into.

  std::vector<bd::Block> m_blocks;
  std::vector<bd::Block *> m_nonEmptyBlocks;
};


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
BlockCollection2::filterBlocks(std::istream &rawFile, float tmin, float tmax)

{
  size_t blkPoints{ bd::vecCompMult(m_blockDims) };

  Ty * image{ new Ty[blkPoints] };

  for (auto& b : m_blocks) {

    fillBlockData<Ty>(b.ijk(), rawFile, image);

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


template<typename Ty>
void BlockCollection2::fillBlockData(glm::u64vec3 blockIndex, std::istream &infile,
    Ty* blockBuffer)
{

  // start element = block index w/in volume * block size
  const glm::u64vec3 start{blockIndex * m_blockDims };

  // block end = block voxel start dims + block size
  const glm::u64vec3 end{ start+ m_blockDims };

  const size_t rowLength{ m_blockDims.x };
  const size_t rowBytes{ rowLength * sizeof(Ty) };
  std::cout << "Row Length: " << rowLength << 
      " Row Bytes: " << rowBytes << std::endl;
  
  for (auto slab = start.z; slab<end.z; ++slab) {
    for (auto row = start.y; row<end.y; ++row) {

      // convert element index to byte index in file.
      size_t byteIndex{ 
          bd::to1D(start.x, row, slab, m_volDims.x, m_volDims.y) * sizeof(Ty) };
      
      //std::cout << "byte index: " << byteIndex << std::endl;

      infile.seekg(byteIndex, infile.beg);

      // read the bytes of row current row
      infile.read(reinterpret_cast<char*>(blockBuffer), rowBytes);
      blockBuffer += rowLength;

    }
  }
}



#endif // !block_collection_h__


