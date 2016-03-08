
#include "indexfile.h"
#include "fileblock.h"
#include "blockcollection2.h"

#include <glm/glm.hpp>

#include <istream>
#include <ostream>


///////////////////////////////////////////////////////////////////////////////
std::ostream& 
operator<<(std::ostream & os, const FileBlock &block)
{
  os << 
    "{"
    "\n  Index: " << block.block_index <<
    "\n  Data Offset: " << block.data_offset <<
    "\n  Voxel dims: " << block.voxel_dims[0] << "x" << block.voxel_dims[1] << "x" << block.voxel_dims[2] <<
    "\n  World pos: " << block.world_pos[0] << ", " << block.world_pos[1] << ", " << block.world_pos[2] <<
    "\n  Min val: " << block.min_val <<
    "\n  Max val: " << block.max_val <<
    "\n  Avg val: " << block.avg_val <<
    "\n  Empty: " << (block.is_empty ? "True" : "False") <<
    "\n}";

  return os;
}

std::ostream&
operator<<(std::ostream & os, const IndexFileHeader &h)
{
  os <<
    "{"
    "\n  Magic: " << h.magic_number <<
    "\n  Version: " << h.version <<
    "\n  Header Length: " << h.header_length <<
    "\n  Number o' Blocks: " << h.numblocks[0] << "x" << h.numblocks[1] << "x" << h.numblocks[2] <<
    "\n  Vol Avg: " << h.vol_avg <<
    "\n  Vol Min: " << h.vol_min <<
    "\n  Vol Max: " << h.vol_max <<
    "\n}";

  return os;
}