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

std::ostream&
operator<<(std::ostream & os, const IndexFileHeader &h);

namespace
{
  const unsigned short MAGIC{ 7376 }; ///< Magic number for the file
  const unsigned short VERSION{ 1 };
  const unsigned int HEAD_LEN{ sizeof(IndexFileHeader) };

}  // namespace

template<typename Ty>
class IndexFile {

public:
  IndexFile(const BlockCollection2<Ty> &collection);
  ~IndexFile();

///////////////////////////////////////////////////////////////////////////////
/// \brief Read binary index file from \c is and populate \c collection with 
///        blocks.
///////////////////////////////////////////////////////////////////////////////
  void
  readBinary(std::istream& is);

///////////////////////////////////////////////////////////////////////////////
/// \brief Write the binary header for index file.
///////////////////////////////////////////////////////////////////////////////
  void
  writeIndexFileHeaderBinary(std::ostream& os);

///////////////////////////////////////////////////////////////////////////////
/// \brief Write single block binary to \c os.
///////////////////////////////////////////////////////////////////////////////
  void
  writeSingleBlockHeaderBinary(std::ostream& os, const FileBlock& block);

///////////////////////////////////////////////////////////////////////////////
/// \brief Write binary index file to ostream \c os.
///////////////////////////////////////////////////////////////////////////////
  void
  writeBinary(std::ostream& os);

///////////////////////////////////////////////////////////////////////////////
/// \brief Write ascii index file to ostream \c os.
///////////////////////////////////////////////////////////////////////////////
  void
  writeAscii(std::ostream& os);

  static IndexFileHeader
  getHeaderFromCollection(const BlockCollection2<Ty> &collection);

  const IndexFileHeader &
  getHeader() const;

private:
  BlockCollection2<Ty> m_collection;
  IndexFileHeader m_header;

};


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
IndexFile<Ty>::IndexFile(const BlockCollection2<Ty> &collection)
  : m_collection{ collection }
  , m_header{ IndexFile<Ty>::getHeaderFromCollection( collection ) }
{ }


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
IndexFile<Ty>::~IndexFile()
{ }


///////////////////////////////////////////////////////////////////////////////
// static member
template<typename Ty>
IndexFileHeader
IndexFile<Ty>::getHeaderFromCollection(const BlockCollection2<Ty> &collection)
{
  IndexFileHeader ifh{
      MAGIC, VERSION, HEAD_LEN,
      collection.numBlocks().x,
      collection.numBlocks().y,
      collection.numBlocks().z };

  return ifh;
}

template<typename Ty>
const IndexFileHeader &
IndexFile<Ty>::getHeader() const
{
  return m_header;
}


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
IndexFile<Ty>::readBinary(std::istream &is)
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
    m_collection.addBlock(fb);
  }

}


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
IndexFile<Ty>::writeIndexFileHeaderBinary(std::ostream &os)
{
  //const IndexFileHeader ifh = getHeaderFromCollection(collection);

  os.write(reinterpret_cast<const char*>(&m_header), sizeof(IndexFileHeader));
}


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
IndexFile<Ty>::writeSingleBlockHeaderBinary(std::ostream & os, const FileBlock &block)
{
  os.write(reinterpret_cast<const char*>(&block), sizeof(FileBlock));
}


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
IndexFile<Ty>::writeBinary(std::ostream &os)
{
  writeIndexFileHeaderBinary(os);

  for (const FileBlock &b : m_collection.blocks()) {
    writeSingleBlockHeaderBinary(os, b);
  }
}



///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
IndexFile<Ty>::writeAscii(std::ostream &os)
{
  os << m_header << "\n";
  for (const FileBlock &b : m_collection.blocks()) {
    os << b << "\n";
  }
}



#endif //! indexfile_h__
