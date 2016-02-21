////////////////////////////////////////////////////////////////////////////////
// Preprocessor
// Generates index files for simple_blocks viewer.
// Index file format
////////////////////////////////////////////////////////////////////////////////

#include "cmdline.h"
#include "blockcollection2.h"

#include <bd/util/util.h>
#include <bd/file/parsedat.h>

#include <bd/log/gl_log.h>
#include <iostream>


template<typename Ty>
size_t blockBytes(const glm::u64vec3 &dims)
{
  return bd::vecCompMult(dims) * sizeof(Ty);
}

void cleanupAndExit(int err)
{
  bd::gl_log_close();
  exit(err);
}

int main(int argc, const char *argv[])
{
  bd::gl_log_restart();

  CommandLineOptions clo;
  if (parseThem(argc, argv, clo) == 0) {
    gl_log_err("Command line parse error, exiting.");
    return 1;
  }

  // print CL options
  printThem(clo);
  bd::DatFileData datfile;
  if (! clo.datFilePath.empty()) {
    bd::parseDat(clo.datFilePath, datfile);
    clo.vol_w = datfile.rX;
    clo.vol_h = datfile.rY;
    clo.vol_d = datfile.rZ;
    clo.type = bd::to_string(datfile.dataType);
    std::cout << datfile << std::endl;
  }

  std::ifstream rawFile{ clo.filePath, std::ios::in | std::ios::binary };
  if (! rawFile.is_open()) {
    std::cerr << clo.filePath << " not found." << std::endl;
    cleanupAndExit(1);
  }

  BlockCollection2 collection{ };
  collection.initBlocks({clo.numblk_x, clo.numblk_y, clo.numblk_z},
      {clo.vol_w, clo.vol_h, clo.vol_d});


  switch(datfile.dataType) {

  case (bd::DataType::UnsignedCharacter):
    collection.filterBlocks<unsigned char>(rawFile, clo.tmin, clo.tmax);
    break;

  case (bd::DataType::UnsignedShort):
    collection.filterBlocks<unsigned short>(rawFile, clo.tmin, clo.tmax);
    break;

  case (bd::DataType::Float):
    collection.filterBlocks<float>(rawFile, clo.tmin, clo.tmax);
    break;

  default:
    std::cerr << "Unsupported/unknown datatype: " << bd::to_string(datfile.dataType) << std::endl;
    cleanupAndExit(1);
    break;
  }

  for (auto &block : collection.blocks()) {
    std::cout << block << std::endl;
  }

  return 0;
}

//template<typename Ty>
//class BlockBuffer
//{
//public:
//  BlockBuffer() : m_data{ nullptr }, m_blockId{ } { }
//
//  /// \brief Convert 3D i,j,k coords into 1D index.
//  size_t
//  blockNumber()
//  {
//    return m_blockId.x +
//        BlockCollection::g_numBlocks.x *
//            (m_blockId.y + BlockCollection::g_numBlocks.y * m_blockId.z);
//  }
//
//  size_t
//  numElements()
//  {
//    return m_dims.x * m_dims.y * m_dims.z;
//  }
//
//
//  Ty *m_data;              ///< Ptr to this buffer data.
//
//  glm::u64vec3 m_dims;     ///< Size of this BlockBuffer in elements along each axis.
//  glm::u64vec3 m_blockId;  ///< i,j,k index of this BlockBuffer.
//
//};


////////////////////////////////////////////////////////////////////////////////
/// \brief Read volume data from \c data and copy into given \c BlockBuffer.
////////////////////////////////////////////////////////////////////////////////
//template<typename Ty>
//void
//readBlock(const glm::u64vec3 & blockId, std::ifstream &inFile, BlockBuffer<Ty> & dest)
//{
//  const size_t voxels{ };
//  const glm::u64vec3 dims{ dest.m_dims };
//  const size_t rowSize{ dims.x };
//
//  Ty *buf{ dest.m_data };
//
//  inFile.seekg(dest.blockNumber() * voxels * sizeof(Ty));
//
//  for(size_t z{ 0 }; z < dims.z; ++z) {
//    for(size_t y{ 0 }; y<dims.y; ++y) {
//      inFile.read(reinterpret_cast<char*>(buf), rowSize * sizeof(Ty));
//      inFile.seekg( ,inFile.cur);
//      buf += rowSize;
//    }
//  }
//}
//
//
//template<typename Ty>
//void
//saveBlock(std::ostream &out, BlockBuffer<Ty> &block)
//{
//
//  size_t voxels{ block.numElements() };
//  Ty *buf{ block.m_data };
//
//  out.write(reinterpret_cast<char*>(buf), voxels * sizeof(Ty));
//  out.flush();
//
//}


