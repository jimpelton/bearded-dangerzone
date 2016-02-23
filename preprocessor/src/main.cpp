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
  collection.initBlocks({ clo.numblk_x, clo.numblk_y, clo.numblk_z },
      { clo.vol_w, clo.vol_h, clo.vol_d });


  switch(datfile.dataType) {

  case bd::DataType::UnsignedCharacter:
    collection.filterBlocks<unsigned char>(rawFile, clo.tmin, clo.tmax);
    break;

  case bd::DataType::UnsignedShort:
    collection.filterBlocks<unsigned short>(rawFile, clo.tmin, clo.tmax);
    break;

  case bd::DataType::Float:
    collection.filterBlocks<float>(rawFile, clo.tmin, clo.tmax);
    break;

  default:
    std::cerr << "Unsupported/unknown datatype: " << 
        bd::to_string(datfile.dataType) << std::endl;
    cleanupAndExit(1);
    break;
  }

  

  if (clo.printBlocks) {
    for (auto &block : collection.blocks()) {
      std::cout << block << std::endl;
    }
  }
  

  return 0;
}

