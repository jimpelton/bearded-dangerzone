#ifndef indexfile_h__
#define indexfile_h__

///////////////////////////////////////////////////////////////////////////////
// File meta data (header)
//   -----------------------------------------
//   magic number        | 2 bytes unsigned (7376 --> characters "sv")
//   version             | 2 bytes unsigned
//   header length       | 4 bytes unsigned
//   -----------------------------------------
//   Volume statistics   | ???
//   -----------------------------------------
//   Block metadata
//   Num blocks X        | 8 bytes unsigned
//   Num blocks Y        | 8 bytes unsigned
//   Num blocks Z        | 8 bytes unsigned
//   -----------------------------------------
//   For each block:
//   block index       | 8 bytes unsigned
//   block st. offset  | 8 bytes unsigned
//   --
//   block dims X      | 4 bytes unsigned
//   block dims Y      | 4 bytes unsigned
//   block dims Z      | 4 bytes unsigned
//   --
//   block X pos       | 4 bytes float
//   block Y pos       | 4 bytes float
//   block Z pos       | 4 bytes float
//   --
//   avg val           | 4 bytes float
//   isEmpty           | 4 bytes bool
//   -----------------------------------------
//   Dat file section   
//   DAT file sz         | 2 bytes unsigned
//   DAT contents        | n bytes ascii with unix newline chars
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdint>



struct IndexFileHeader
{
  uint16_t magic_number;
  uint16_t version;
  uint32_t header_length;
  uint64_t numblocks[3];
};

struct FileBlock
{
  uint64_t block_index;    ///< The 1D idx of this block (derived from the i,j,k block-grid coordinates).
  uint64_t data_offset;    ///< Offset into the file that the block data starts.
  uint32_t voxel_dims[3];  ///< Dimensions of this block in voxels.
  float world_pos[3];      ///< Cordinates within canonical cube.
  float avg_val;           ///< Average value within this block.
  uint32_t is_empty;       ///< If this block is empty or not.
};



namespace bd {
  class Block;
}

class BlockCollection2;


void 
read(std::istream &is, BlockCollection2 &collection);


void 
writeHeaderOnly(std::ostream &os, const BlockCollection2 &collection);


void 
writeSingleBlock(std::ostream &os, const bd::Block &block);


void 
write(std::ostream &os, const BlockCollection2 &collection);



#endif indexfile_h__
