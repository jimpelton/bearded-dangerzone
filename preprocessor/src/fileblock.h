//
// Created by Jim Pelton on 3/5/16.
//

#ifndef fileblock_h__
#define fileblock_h__

#include <cstdint>
#include <ostream>

///////////////////////////////////////////////////////////////////////////////
///   \brief Describes a block in the IndexFile.
/// 
///   All values in the FileBlock struct are initialized to 0 by the c'tor.
/// 

///   -----------------------------------------
///   For each block:
///   block index       | 8 bytes unsigned
///   block st. offset  | 8 bytes unsigned
///   --
///   block dims X      | 4 bytes unsigned
///   block dims Y      | 4 bytes unsigned
///   block dims Z      | 4 bytes unsigned
///   --
///   block X pos       | 4 bytes float
///   block Y pos       | 4 bytes float
///   block Z pos       | 4 bytes float
///   --
///   max val           | 4 bytes float
///   min val           | 4 bytes float
///   avg val           | 4 bytes float
///
///   isEmpty           | 4 bytes unsigned
///   -----------------------------------------
///   Dat file section (unimplemented)
///   DAT file sz         | 2 bytes unsigned
///   DAT contents        | n bytes ascii with unix newline chars
///////////////////////////////////////////////////////////////////////////////
struct FileBlock
{

  FileBlock()
    : block_index{ 0 }
    , data_offset{ 0 }
    , voxel_dims{ 0 }
    , world_pos{ 0 }
    , min_val{ 0.0f }
    , max_val{ 0.0f }
    , avg_val{ 0.0f }
    , is_empty{ 0 }
    { }

  uint64_t block_index;    ///< The 1D idx of this block (derived from the i,j,k block-grid coordinates).
  uint64_t data_offset;    ///< Offset into the raw file that the block data starts.
  uint32_t voxel_dims[3];  ///< Dimensions of this block in voxels.
  float world_pos[3];      ///< Cordinates within canonical cube.
  float min_val;           ///< The min value found in this block.
  float max_val;           ///< The largest value found in this block.
  float avg_val;           ///< Average value within this block.
  uint32_t is_empty;        ///< If this block is empty or not.

};


std::ostream&
operator<<(std::ostream &, const FileBlock &);


#endif //! fileblock_h__
