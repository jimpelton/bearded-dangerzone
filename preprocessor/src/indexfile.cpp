
#include "indexfile.h"
#include "blockcollection2.h"

#include <glm/glm.hpp>

#include <istream>
#include <ostream>

namespace
{
  const unsigned short MAGIC{ 7376 }; ///< Magic number for the file
  const unsigned short VERSION{ 1 };
  const unsigned int HEAD_LEN{ sizeof(IndexFileHeader) };
  
};


void 
read(std::istream &is, BlockCollection2 &collection)
{
  // read header
  IndexFileHeader ifh;
  is.seekg(0, std::ios::beg);
  is.read(reinterpret_cast<char*>(&ifh), sizeof(IndexFileHeader));
  
  size_t numBlocks{ ifh.numblocks[0] * ifh.numblocks[1] * ifh.numblocks[2] };

  // read many blocks!
  FileBlock fb;
  for (size_t i = 0; i < numBlocks; ++i) {
    is.read(reinterpret_cast<char*>(&fb), sizeof(FileBlock));
    //TODO: put into BlockCollection.
  }

}

void 
writeHeader(std::ostream &os, const BlockCollection2 &collection)
{
 
  const IndexFileHeader ifh{ 
    MAGIC, VERSION, HEAD_LEN, 
    collection.numBlocks().x, 
    collection.numBlocks().y, 
    collection.numBlocks().z };
 
  os.write(reinterpret_cast<const char*>(&ifh), sizeof(IndexFileHeader));
}

void writeSingleBlockHeader(std::ostream & os, const FileBlock &block)
{
  os.write(reinterpret_cast<const char*>(&block), sizeof(FileBlock));
}

void 
write(std::ostream &os, const BlockCollection2 &collection)
{
  writeHeader(os, collection);

//  size_t offset{ os.tellp() };

//  glm::u64vec3 dims{ collection.blockDims() };
//  glm::u64vec3 nblk{ collection.numBlocks() };

//  FileBlock fblk;
  for (const FileBlock &b : collection.blocks()) {
    writeSingleBlockHeader(os, b);

    //const glm::u64vec3 & ijk{ bd_block.ijk() };

    //fblk.block_index = b.block_index; // bd::to1D(b.x, ijk.y, ijk.z, nblk.x, nblk.y);

    //
    //fblk.data_offset = 0;

    //fblk.voxel_dims[0] = dims.x;
    //fblk.voxel_dims[1] = dims.y;
    //fblk.voxel_dims[2] = dims.z;

    ////TODO: blockcollection2 uses struct FileBlock
    //fblk.world_pos[0] = 0.0f;
    //fblk.world_pos[1] = 0.0f;
    //fblk.world_pos[2] = 0.0f;

    //fblk.avg_val = b.avg_val;

    //fblk.is_empty = b.is_empty;
  }

}

std::ostream& operator<<(std::ostream & os, const FileBlock &block)
{
  os << "{ Index: " << block.block_index <<
    "\n Data Offset: " << block.data_offset <<
    "\n Voxel dims: " << block.voxel_dims[0] << "x" << block.voxel_dims[1] << "x" << block.voxel_dims[2] <<
    "\n World pos: " << block.world_pos[0] << ", " << block.world_pos[1] << ", " << block.world_pos[2] <<
    "\n Avg val: " << block.avg_val <<
    "\n Empty: " << (block.is_empty ? "True" : "False") << " }";

  return os;
}