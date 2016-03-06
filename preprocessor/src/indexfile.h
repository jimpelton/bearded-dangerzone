#ifndef indexfile_h__
#define indexfile_h__


#include "fileblock.h"
#include "blockcollection2.h"

#include <iostream>
#include <cstdint>


///////////////////////////////////////////////////////////////////////////////
//   Index file header layout
//   -----------------------------------------
//   magic number        | 2 bytes unsigned (7376 --> characters "sv")
//   version             | 2 bytes unsigned
//   header length       | 4 bytes unsigned
//   -----------------------------------------
//   Volume statistics   | not implemented yet
//   -----------------------------------------
///////////////////////////////////////////////////////////////////////////////
struct IndexFileHeader
{
  uint16_t magic_number;
  uint16_t version;
  uint32_t header_length;
  uint64_t numblocks[3];
//   -----------------------------------------
  float vol_avg;
  float vol_min;
  float vol_max;
};

namespace
{
  const unsigned short MAGIC{ 7376 }; ///< Magic number for the file
  const unsigned short VERSION{ 1 };
  const unsigned int HEAD_LEN{ sizeof(IndexFileHeader) };

}  // namespace


///////////////////////////////////////////////////////////////////////////////
/// \brief Read binary index file from \c is and populate \c collection with 
///        blocks.
///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void 
readBinary(std::istream &is, BlockCollection2<Ty> &collection);


///////////////////////////////////////////////////////////////////////////////
/// \brief Write the binary header for index file.
///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
writeIndexFileHeaderBinary(std::ostream &os, const BlockCollection2<Ty> &collection);


///////////////////////////////////////////////////////////////////////////////
/// \brief Write single block binary to \c os.
///////////////////////////////////////////////////////////////////////////////
void
writeSingleBlockHeaderBinary(std::ostream &os, const FileBlock &block);


///////////////////////////////////////////////////////////////////////////////
/// \brief Write binary index file to ostream \c os.
///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
writeBinary(std::ostream &os, const BlockCollection2<Ty> &collection);


///////////////////////////////////////////////////////////////////////////////
/// \brief Write ascii index file to ostream \c os.
///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
writeAscii(std::ostream &os, const BlockCollection2<Ty> &collection);




///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
IndexFileHeader
getHeaderFromCollection(const BlockCollection2<Ty> &collection)
{
  IndexFileHeader ifh{
      MAGIC, VERSION, HEAD_LEN,
      collection.numBlocks().x,
      collection.numBlocks().y,
      collection.numBlocks().z };

  return ifh;
}


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
readBinary(std::istream &is, BlockCollection2<Ty> &collection)
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
template<typename Ty>
void
writeIndexFileHeaderBinary(std::ostream &os, const BlockCollection2<Ty> &collection)
{
  const IndexFileHeader ifh = getHeaderFromCollection(collection);

  os.write(reinterpret_cast<const char*>(&ifh), sizeof(IndexFileHeader));
}


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
writeBinary(std::ostream &os, const BlockCollection2<Ty> &collection)
{
  writeIndexFileHeaderBinary(os, collection);

  for (const FileBlock &b : collection.blocks()) {
    writeSingleBlockHeaderBinary(os, b);
  }
}



///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
writeAscii(std::ostream &os, const BlockCollection2<Ty> &collection)
{
  os << getHeaderFromCollection(collection) << "\n";
  for (const FileBlock &b : collection.blocks()) {
    os << b << "\n";
  }
}
std::ostream&
operator<<(std::ostream & os, const IndexFileHeader &h);

#endif //! indexfile_h__
