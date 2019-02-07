#ifndef indexfile_h__
#define indexfile_h__

#include <bd/io/fileblock.h>
#include <bd/io/indexfileheader.h>
#include <bd/volume/volume.h>


#include <iostream>
#include <cstdint>
#include <fstream>
#include <memory>
#include <vector>


namespace bd
{
namespace
{
///< Magic number for the file (ascii 'SV')
uint16_t const MAGIC{ 7376 };
/// \brief The version of the IndexFile
uint16_t const VERSION{ 13 };
/// \brief Length of the IndexFileHeader in bytes.
uint32_t const HEAD_LEN{ sizeof(IndexFileHeader) };
} // namespace


/// \brief Generate an index file from the provided FileBlockCollection. The
///        IndexFile can be written to disk in either ASCII or binary format.
class IndexFile
{
public:

  /// \brief Create IndexFile from an existing binary index file.
  /// \returns A unique_ptr to the IndexFile created or nullptr on failure.
  static std::unique_ptr<IndexFile>
  fromBinaryIndexFile(std::string const &path, bool &ok);


  /// \brief Create an empty IndexFile with empty header.
  IndexFile();


  ~IndexFile();


  bd::FileBlock const &
  operator[](size_t idx) const {
    return m_fileBlocks[idx];
  }

  bd::FileBlock &
  operator[](size_t idx)  {
    return m_fileBlocks[idx];
  }

  /// \brief Write binary index file to ostream \c os.
  void
  writeBinaryIndexFile(std::ostream &os) const;


  /// \brief Write binary index file to the file at \c outpath.
  void
  writeBinaryIndexFile(std::string const &outpath) const;


  /// \brief Write ascii index file to ostream \c os.
  void
  writeAsciiIndexFile(std::ostream &os) const;


  /// \brief Write ascii index file to the file at \c outpath.
  void
  writeAsciiIndexFile(std::string const &outpath) const;


  /// \brief Get the IndexFileHeader for the index file.
  IndexFileHeader const&
  getHeader() const;


  std::vector<FileBlock> const&
  getFileBlocks() const;


  std::vector<FileBlock>&
  getFileBlocks();


  bd::Volume const&
  getVolume() const;


  bd::Volume&
  getVolume();

  void
  setVolume(bd::Volume const &);


  /// \brief Get the path to the file.
  /// \Note this is not guaranteed to be an absolute or relative path :( .
  std::string const &
  getPath();

  std::string
  getRawFileName();

  std::string
  getTFFileName();

  void
  setRawFileName(std::string const &);

  void
  setTFFileName(std::string const &);

  /// Initialize this indexfile with datatype t.
  /// \param t
  void
  init(DataType t);


private:


  /// \brief Read binary index file and populate \c a collection with blocks
  bool
  readBinaryIndexFile();


  void
  initHeader(DataType dt);


  IndexFileHeader m_header;
  std::string m_fileName;
  std::vector<bd::FileBlock> m_fileBlocks;
  bd::Volume m_volume;
}; // class IndexFile
} // namespace bd


#endif // !indexfile_h__
