
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

//  IndexFileHeader 
//  getHeaderFromCollection(const BlockCollection2 &);
  
  IndexFileHeader
  getHeaderFromCollection(const BlockCollection2 &collection)
  {
    IndexFileHeader ifh{
      MAGIC, VERSION, HEAD_LEN,
      collection.numBlocks().x,
      collection.numBlocks().y,
      collection.numBlocks().z };

    return ifh;
  }
  
}  // namespace 



void 
readBinary(std::istream &is, BlockCollection2 &collection)
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
    collection.addBlock(fb);
  }

}


///////////////////////////////////////////////////////////////////////////////
void 
writeIndexFileHeaderBinary(std::ostream &os, const BlockCollection2 &collection)
{
  const IndexFileHeader ifh = getHeaderFromCollection(collection); 
 
  os.write(reinterpret_cast<const char*>(&ifh), sizeof(IndexFileHeader));
}


///////////////////////////////////////////////////////////////////////////////
void 
writeSingleBlockHeaderBinary(std::ostream & os, const FileBlock &block)
{
  os.write(reinterpret_cast<const char*>(&block), sizeof(FileBlock));
}


///////////////////////////////////////////////////////////////////////////////
void 
writeBinary(std::ostream &os, const BlockCollection2 &collection)
{
  writeIndexFileHeaderBinary(os, collection);

  for (const FileBlock &b : collection.blocks()) {
    writeSingleBlockHeaderBinary(os, b);
  }
}


///////////////////////////////////////////////////////////////////////////////
void
writeAscii(std::ostream &os, const BlockCollection2 &collection)
{
  os << getHeaderFromCollection(collection) << "\n";
  for (const FileBlock &b : collection.blocks()) {
    os << b << "\n";
  }
}


///////////////////////////////////////////////////////////////////////////////
std::ostream& 
operator<<(std::ostream & os, const FileBlock &block)
{
  os << 
    "{ Index: " << block.block_index <<
    "\n  Data Offset: " << block.data_offset <<
    "\n  Voxel dims: " << block.voxel_dims[0] << "x" << block.voxel_dims[1] << "x" << block.voxel_dims[2] <<
    "\n  World pos: " << block.world_pos[0] << ", " << block.world_pos[1] << ", " << block.world_pos[2] <<
    "\n  Avg val: " << block.avg_val <<
    "\n  Empty: " << (block.is_empty ? "True" : "False") << " }";

  return os;
}

std::ostream&
operator<<(std::ostream & os, const IndexFileHeader &h)
{
  os <<
    "{ Magic: " << h.magic_number <<
    "\n  Version: " << h.version <<
    "\n  Header Length: " << h.header_length <<
    "\n  Number o' Blocks: " << h.numblocks[0] << "x" << h.numblocks[1] << "x" << h.numblocks[2] << 
    " }";

  return os;
}