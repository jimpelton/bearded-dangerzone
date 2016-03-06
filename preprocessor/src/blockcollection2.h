#ifndef block_collection_h__
#define block_collection_h__

#include "fileblock.h"

#include <bd/util/util.h>
#include <bd/log/gl_log.h>

#include <glm/glm.hpp>

#include <istream>
#include <iostream>
#include <vector>



//////////////////////////////////////////////////////////////////////////////
/// \tparam Ty Data type in the istream this BlockCollection will be
///            generated from.
//////////////////////////////////////////////////////////////////////////////
template<typename Ty>
class BlockCollection2
{

public:

  BlockCollection2();

  ~BlockCollection2();

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Marks blocks as empty and uploads GL textures if average is outside of [tmin..tmax].
  /// \param rawFile[in] Volume data set
  /// \param tmin[in] min average block value to filter against.
  /// \param tmax[in] max average block value to filter against.
  ///////////////////////////////////////////////////////////////////////////////
  //TODO: filterblocks takes Functor for thresholding.
  void
  filterBlocks(std::istream &rawFile, float tmin = 0.0f, float tmax = 1.0f);


  //////////////////////////////////////////////////////////////////////////////
  /// \brief Fills \c out_blockData with part of \c in_data corresponding to 
  ///        block (i,j,k).
  /// \param index[in]     ijk coords of the block whos data to get.
  /// \param infile[in] The raw data file.
  /// \param out[out]   Destination space for block data.
  //////////////////////////////////////////////////////////////////////////////
  void
  fillBlockData(FileBlock &b, glm::u64vec3 index, std::istream &infile, Ty* out);


  //////////////////////////////////////////////////////////////////////////////
  /// \brief Read offset
  /// \param nb[in]      Number of blocks in x,y,z directions.
  /// \param vd[in]      Volume dimensions
  //////////////////////////////////////////////////////////////////////////////
  void
  readRow(std::istream &infile, size_t offset, size_t rowsize, Ty *rowBuffer);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Initializes \c nb blocks so that they fit within the extent of \c vd.
  /// \param nb[in]      Number of blocks in x,y,z directions.
  /// \param vd[in]      Volume dimensions
  //////////////////////////////////////////////////////////////////////////////
  void
  initBlocks(glm::u64vec3 nb, glm::u64vec3 vd);


  //////////////////////////////////////////////////////////////////////////////
  void
  addBlock(const FileBlock &b);


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


  //////////////////////////////////////////////////////////////////////////////
  /// \brief Get the number of blocks along each axis.
  //////////////////////////////////////////////////////////////////////////////
  glm::u64vec3
  numBlocks() const;


  const std::vector<FileBlock>&
  blocks() const;


  const std::vector<FileBlock *>&
  nonEmptyBlocks() const;

private:

  glm::u64vec3 m_blockDims; ///< Dimensions of a block in something.
  glm::u64vec3 m_volDims;   ///< Volume dimensions (# data points).
  glm::u64vec3 m_numBlocks; ///< Number of blocks volume is divided into.

  Ty m_max;
  Ty m_min;
  float m_avg;

  std::vector<FileBlock> m_blocks;
  std::vector<FileBlock *> m_nonEmptyBlocks;
};

///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
BlockCollection2<Ty>::BlockCollection2()
{
}


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
BlockCollection2<Ty>::~BlockCollection2()
{
}


///////////////////////////////////////////////////////////////////////////////
// nb: number of blocks
// vd: volume voxel dimensions
// blocks: out parameter to be filled with blocks.
template<typename Ty>
void
BlockCollection2<Ty>::initBlocks(glm::u64vec3 nb, glm::u64vec3 vd)
{
  m_blockDims = vd / nb;
  m_volDims = vd;
  m_numBlocks = nb;

  // block world dims
  glm::vec3 wld_dims{ 1.0f / glm::vec3(nb) };

  gl_log("Starting block init: Number of blocks: %dx%dx%d, "
      "Volume dimensions: %dx%dx%d Block dimensions: %.2f,%.2f,%.2f",
      nb.x, nb.y, nb.z,
      vd.x, vd.y, vd.z,
      wld_dims.x, wld_dims.y, wld_dims.z);

  // Loop through all our blocks (identified by <bx,by,bz>) and populate block fields.
  for (auto bz = 0ull; bz < nb.z; ++bz)
    for (auto by = 0ull; by < nb.y; ++by)
      for (auto bx = 0ull; bx < nb.x; ++bx) {
        // i,j,k block identifier
        glm::u64vec3 blkId{ bx, by, bz };
        // lower left corner in world coordinates
        glm::vec3 worldLoc{ wld_dims * glm::vec3(blkId) - 0.5f }; // - 0.5f;
        // origin (centroid) in world coordiates
        glm::vec3 blk_origin{ (worldLoc + (worldLoc + wld_dims)) * 0.5f };

        FileBlock blk;
        blk.block_index = bd::to1D(bx, by, bz, nb.x, nb.y);
        blk.voxel_dims[0] = (uint32_t) m_blockDims.x;
        blk.voxel_dims[1] = (uint32_t) m_blockDims.y;
        blk.voxel_dims[2] = (uint32_t) m_blockDims.z;
        blk.world_pos[0] = blk_origin.x;
        blk.world_pos[1] = blk_origin.y;
        blk.world_pos[2] = blk_origin.z;

        m_blocks.push_back(blk);
      }

  gl_log("Finished block init: total blocks is %d.", m_blocks.size());
}


//////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
BlockCollection2<Ty>::addBlock(const FileBlock &b)
{
  m_blocks.push_back(b);

  if (b.is_empty) {
    FileBlock *pB = &m_blocks.back();
    m_nonEmptyBlocks.push_back(pB);
  }

}


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
glm::u64vec3
BlockCollection2<Ty>::blockDims() const
{
  return m_blockDims;
}


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
BlockCollection2<Ty>::blockDims(const glm::u64vec3& dims)
{
  m_blockDims = dims;
}


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
glm::u64vec3
BlockCollection2<Ty>::volDims() const
{
  return m_volDims;
}


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
BlockCollection2<Ty>::volDims(const glm::u64vec3& voldims)
{
  m_volDims = voldims;
}

template<typename Ty>
glm::u64vec3
BlockCollection2<Ty>::numBlocks() const
{
  return m_numBlocks;
}


template<typename Ty>
const std::vector<FileBlock>&
BlockCollection2<Ty>::blocks() const
{
  return m_blocks;
}


template<typename Ty>
const std::vector<FileBlock *>&
BlockCollection2<Ty>::nonEmptyBlocks() const
{
  return m_nonEmptyBlocks;
}


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
BlockCollection2<Ty>::filterBlocks(std::istream &rawFile, float tmin, float tmax)
{
  // total voxels per block
  size_t blkPoints{ m_blockDims.x * m_blockDims.y * m_blockDims.z };

  Ty * image{ new Ty[blkPoints] };



  for (FileBlock& b : m_blocks) {

    // Convert 1D block index to 3D i,j,k indices.
    glm::u64 z = b.block_index %  m_numBlocks.z;
    glm::u64 y = (b.block_index / m_numBlocks.z) % m_numBlocks.y;
    glm::u64 x = b.block_index / (m_numBlocks.y  * m_numBlocks.z);
    
    fillBlockData(b, { x,y,z }, rawFile, image);
    
    //TODO: call filter function.
    
    // Compute average and determine if Block is empty.
    float avg{ 0.0f };
    for (size_t i = 0; i < blkPoints; ++i) {
      Ty val{ image[i] };
      b.max_val = std::max<Ty>(b.max_val, val);
      b.min_val = std::min<Ty>(b.max_val, val);

      m_avg += val;
      m_min = std::min<Ty>(m_min, val);
      m_max = std::max<Ty>(m_max, val);

      avg += image[i];
    }
    avg /= blkPoints;
    b.avg_val = avg;


    if (avg < tmin || avg > tmax) {
      b.is_empty = 1;
    } else {
      b.is_empty = 0;
      m_nonEmptyBlocks.push_back(&b);
    }

  } // for (FileBlock...

  m_avg /= blkPoints * m_blocks.size();

  delete [] image;

  gl_log("%d/%d blocks marked empty.",
      m_blocks.size() - m_nonEmptyBlocks.size(), m_blocks.size());
}

template<typename Ty>
void
BlockCollection2<Ty>::readRow(std::istream &infile, size_t offset, size_t rowsize,
    Ty *rowBuffer)
{

  // seek to start of row
  infile.seekg(offset, infile.beg);

  // read the bytes of current row
  infile.read(reinterpret_cast<char*>(rowBuffer), rowsize);
}

template<typename Ty>
void
BlockCollection2<Ty>::fillBlockData(FileBlock &block, glm::u64vec3 index,
    std::istream &infile, Ty* blockBuffer)
{

  // start element = block index w/in volume * block size
  const glm::u64vec3 start{ index * m_blockDims };

  // block end element = block voxel start dims + block size
  const glm::u64vec3 end{ start + m_blockDims };


  const size_t blockRowLength{ m_blockDims.x };
  const size_t blockRowBytes{ blockRowLength * sizeof(Ty) };
  
  size_t rowOff{bd::to1D(start.x, start.y, start.z, m_volDims.x, m_volDims.y) * sizeof(Ty) };
  block.data_offset = rowOff;

  for (auto slab = start.z; slab < end.z; ++slab) {
    for (auto row = start.y; row < end.y; ++row) {

      // convert element index to byte index in file.
      rowOff = bd::to1D(start.x, row, slab, m_volDims.x, m_volDims.y) * sizeof(Ty);
      readRow(infile, rowOff, blockRowBytes, blockBuffer);
      blockBuffer += blockRowLength;

    }
  }
}



#endif // !block_collection_h__


