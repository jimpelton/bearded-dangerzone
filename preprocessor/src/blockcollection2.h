#ifndef block_collection_h__
#define block_collection_h__

#include "indexfile.h"


#include <bd/util/util.h>
#include <bd/log/gl_log.h>

#include <glm/glm.hpp>

#include <istream>
#include <iostream>
#include <vector>


class BlockCollection2
{

public:

  BlockCollection2();

  ~BlockCollection2();

  /////////////////////////////////////////////////////////////////////////////////
  /// \brief Set this BlockCollection2's dimensions in voxels
  /////////////////////////////////////////////////////////////////////////////////
  void
  blockDims(const glm::u64vec3& dims);

  
  /////////////////////////////////////////////////////////////////////////////////
  /// \brief Get this BlockCollection2's dimensions in voxels
  /////////////////////////////////////////////////////////////////////////////////
  glm::u64vec3
  blockDims() const;


  /////////////////////////////////////////////////////////////////////////////////
  /// \brief Get the volume's dimensions in voxels
  /////////////////////////////////////////////////////////////////////////////////
  glm::u64vec3
  volDims() const;

  
  /////////////////////////////////////////////////////////////////////////////////
  /// \brief Set the volume's dimensions in voxels
  /////////////////////////////////////////////////////////////////////////////////
  void
  volDims(const glm::u64vec3& voldims);
  //TODO: move volDims() out of block class (yeah...probably need to make a class representing a volume).


  /////////////////////////////////////////////////////////////////////////////////
  /// \brief Get the number of blocks along each axis.
  /////////////////////////////////////////////////////////////////////////////////
  glm::u64vec3
  numBlocks() const;


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
  

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Fills \c out_blockData with part of \c in_data corresponding to 
  ///        block (i,j,k).
  ///
  /// \param ijk[in]     ijk coords of the block whos data to get.
  /// \param bsz[in]     The size of the block data.
  /// \param infile[in] The raw data file.
  /// \param out[out]   Destination space for block data.
  //////////////////////////////////////////////////////////////////////////////
  template<typename Ty>
  void 
  fillBlockData(FileBlock &b, glm::u64vec3 index, std::istream &infile, Ty* out);


  const std::vector<FileBlock>&
  blocks() const;


  const std::vector<FileBlock *>&
  nonEmptyBlocks() const;

private:

  glm::u64vec3 m_blockDims; ///< Dimensions of a block in something.
  glm::u64vec3 m_volDims;   ///< Volume dimensions (# data points).
  glm::u64vec3 m_numBlocks; ///< Number of blocks volume is divided into.

  std::vector<FileBlock> m_blocks;
  std::vector<FileBlock *> m_nonEmptyBlocks;
};


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
BlockCollection2::filterBlocks(std::istream &rawFile, float tmin, float tmax)
{
  // total voxels per block
  size_t blkPoints{ m_blockDims.x * m_blockDims.y * m_blockDims.z };

  Ty * image{ new Ty[blkPoints] };

  for (FileBlock& b : m_blocks) {

    // Convert 1D block index to 3D i,j,k indices.
    glm::u64 z = b.block_index %  m_numBlocks.z;
    glm::u64 y = (b.block_index / m_numBlocks.z) % m_numBlocks.y;
    glm::u64 x = b.block_index / (m_numBlocks.y  * m_numBlocks.z);
    
    fillBlockData<Ty>(b, { x,y,z }, rawFile, image);
    
    //TODO: call filter function.
    
    // Compute average and determine if Block is empty.
    float avg{ 0.0f };
    for (size_t i = 0; i < blkPoints; ++i) {
      avg += image[i];
    }
    avg /= blkPoints;
    b.avg_val = avg;


    if (avg < tmin || avg > tmax) {
      b.is_empty = true;
    } else {
      b.is_empty = false;
      m_nonEmptyBlocks.push_back(&b);
    }

  } // for (FileBlock...

  delete [] image;

  gl_log("%d/%d blocks marked empty.",
      m_blocks.size() - m_nonEmptyBlocks.size(), m_blocks.size());
}


template<typename Ty>
void BlockCollection2::fillBlockData(FileBlock &block, glm::u64vec3 index,
    std::istream &infile, Ty* blockBuffer)
{

  // start element = block index w/in volume * block size
  const glm::u64vec3 start{ index * m_blockDims };

  // block end element = block voxel start dims + block size
  const glm::u64vec3 end{ start + m_blockDims };


  const size_t blockRowLength{ m_blockDims.x };
  const size_t blockRowBytes{ blockRowLength * sizeof(Ty) };
  
  size_t byteIndex{ bd::to1D(start.x, start.y, start.z, m_volDims.x, m_volDims.y) * sizeof(Ty) };
  block.data_offset = byteIndex;

  std::cout << "Block Row Length: " << blockRowLength << 
      " Block Row Bytes: " << blockRowBytes << std::endl;
  
  for (auto slab = start.z; slab < end.z; ++slab) {
    for (auto row = start.y; row < end.y; ++row) {

      // convert element index to byte index in file.
      byteIndex = bd::to1D(start.x, row, slab, m_volDims.x, m_volDims.y) * sizeof(Ty);

      infile.seekg(byteIndex, infile.beg);

      // read the bytes of current row
      infile.read(reinterpret_cast<char*>(blockBuffer), blockRowBytes);
      blockBuffer += blockRowLength;

    }
  }
}



#endif // !block_collection_h__


