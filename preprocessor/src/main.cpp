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


std::ifstream g_rawFile;


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
size_t blockBytes(const glm::u64vec3 &dims)
{
  return dims.x * dims.y * dims.z * sizeof(Ty);
}


///////////////////////////////////////////////////////////////////////////////
void cleanUp()
{
  bd::gl_log_close();

  if (g_rawFile.is_open())
    g_rawFile.close();
}


///////////////////////////////////////////////////////////////////////////////
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

  
  g_rawFile.open(clo.filePath, std::ios::in | std::ios::binary);
  if (! g_rawFile.is_open()) {
    std::cerr << clo.filePath << " not found." << std::endl;
    cleanUp();
    exit(1);
  }

  
  BlockCollection2 collection{ };
  collection.initBlocks(
      glm::u64vec3{ clo.numblk_x, clo.numblk_y, clo.numblk_z },
      glm::u64vec3{ clo.vol_w, clo.vol_h, clo.vol_d }
  );

  switch(datfile.dataType) {

  case bd::DataType::UnsignedCharacter:
    collection.filterBlocks<unsigned char>(g_rawFile, clo.tmin, clo.tmax);
    break;

  case bd::DataType::UnsignedShort:
    collection.filterBlocks<unsigned short>(g_rawFile, clo.tmin, clo.tmax);
    break;

  case bd::DataType::Float:
    collection.filterBlocks<float>(g_rawFile, clo.tmin, clo.tmax);
    break;

  default:
    std::cerr << 
        "Unsupported/unknown datatype: " << 
        bd::to_string(datfile.dataType) << std::endl;

    cleanUp();
    exit(1);
    break;
  }

  if (clo.printBlocks) {
    for (auto &block : collection.blocks()) {
      std::cout << block << std::endl;
    }
  }
  
  cleanUp();
  return 0;
}

