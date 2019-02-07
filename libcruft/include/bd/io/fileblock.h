//
// Created by Jim Pelton on 3/5/16.
//

#ifndef bd_fileblock_h
#define bd_fileblock_h

#include <cstdint>
#include <ostream>
#include <limits>
#include <string>

//#include <atomic>


namespace bd
{

///   \brief Describes a Block in the IndexFile.
/// 
///   \note All values in the FileBlock struct are initialized to 0 by the c'tor,
///         except for min_val and max_val. min_val is init'd to its max value, and
///         max_val is init'd to its lowest possible value.
/// 
struct FileBlock
{


  FileBlock()
      : block_index{ 0 }
      , ijk_index{ 0 }
      , data_offset{ 0 }
      , data_bytes{ 0 }
      , voxel_dims{ 0 }
      , world_dims{ 0 }
      , world_oigin{ 0 }
      , min_val{ std::numeric_limits< decltype(min_val) >::max() }
      , max_val{ std::numeric_limits< decltype(max_val) >::lowest() }
      , avg_val{ 0.0 }
      , total_val{ 0.0 }
      , rov{ 0 }
      , empty_voxels{ 0 }
      , is_empty{ 0 }
  { }


  FileBlock(const FileBlock& other)
      : block_index{ other.block_index }
      , ijk_index{ other.ijk_index[0], other.ijk_index[1], other.ijk_index[2] }
      , data_offset{ other.data_offset }
      , data_bytes{ other.data_bytes }
      , voxel_dims{ other.voxel_dims[0], other.voxel_dims[1], other.voxel_dims[2] }
      , world_dims{ other.world_dims[0], other.world_dims[1], other.world_dims[2] }
      , world_oigin{ other.world_oigin[0], other.world_oigin[1], other.world_oigin[2] }
      , min_val{ other.min_val }
      , max_val{  other.max_val }
      , avg_val{ other.avg_val }
      , total_val{ other.total_val }
      , rov{ other.rov }
      , empty_voxels{ other.empty_voxels }
      , is_empty{ other.is_empty }
  {
  }

  std::string to_string() const;


  uint64_t block_index;    ///< The 1D idx of this block (derived from the i,j,k block-grid coordinates).
  uint64_t ijk_index[3];   ///< 3D idx of this block.
  uint64_t data_offset;    ///< Offset into the raw file that the block data starts.
  uint64_t data_bytes;     ///< Size in bytes of this blocks data.
  uint64_t voxel_dims[3];  ///< Dimensions of this block in voxels.
  double world_dims[3];    ///< Dims of this block in world coordinates
  double world_oigin[3];   ///< Center coordinates within canonical cube.
  double min_val;          ///< The min value found in this block.
  double max_val;          ///< The largest value found in this block.
  double avg_val;          ///< Average value within this block.
  double total_val;        ///< The total value of the block voxels.
  double rov;              ///< Block ratio-of-visibility
  uint64_t empty_voxels;   ///< Number of empty (ie, irrelevent) voxels in this block.
  uint32_t is_empty;       ///< If this block is empty or not.

}; // struct FileBlock

std::ostream& operator<<(std::ostream&, const FileBlock&);

} // namespace bd

#endif //! bd_fileblock_h
