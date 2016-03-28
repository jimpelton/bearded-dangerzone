////////////////////////////////////////////////////////////////////////////////
// Preprocessor
// Generates index files for simple_blocks viewer.
// Index file format
////////////////////////////////////////////////////////////////////////////////

#include "cmdline.h"

#include <bd/util/util.h>
#include <bd/file/parsedat.h>
#include <bd/volume/indexfile.h>
#include <bd/volume/blockcollection2.h>
#include <bd/log/gl_log.h>

#include <iostream>

std::ifstream g_rawFile;
std::ofstream g_outFile;


///////////////////////////////////////////////////////////////////////////////
void cleanUp()
{
  bd::gl_log_close();

  if (g_rawFile.is_open()) {
    g_rawFile.close();
  }

  if (g_outFile.is_open()) {
    g_outFile.flush();
    g_outFile.close();
  }

}


///////////////////////////////////////////////////////////////////////////////
template<typename Ty>
size_t
blockBytes(const glm::u64vec3 &dims)
{
  return dims.x * dims.y * dims.z * sizeof(Ty);
}

template<typename Ty>
void
generateIndexFile(const CommandLineOptions &clo)
{
  bd::BlockCollection2<Ty> collection{
      glm::u64vec3{clo.vol_w, clo.vol_h, clo.vol_d},
      glm::u64vec3{clo.numblk_x, clo.numblk_y, clo.numblk_z}
  };

  // open raw file
  g_rawFile.open(clo.filePath, std::ios::in | std::ios::binary);
  if (! g_rawFile.is_open()) {
    std::cerr << clo.filePath << " not found." << std::endl;
    cleanUp();
    exit(1);
  }

  // filter the blocks
  collection.filterBlocks(g_rawFile, clo.tmin, clo.tmax);
  bd::IndexFile<Ty> indexFile{ collection };

  if (clo.outputFileType == OutputType::Ascii) {
    g_outFile.open(clo.outFilePath);
    indexFile.writeAscii(g_outFile);
  } else {
    // default to binary output file.
    g_outFile.open(clo.outFilePath, std::ios::binary);
    indexFile.writeBinary(g_outFile);
  }

  if (clo.printBlocks) {
    for (auto &block : collection.blocks()) {
      std::cout << block << std::endl;
    }
  }

}

template<typename Ty>
void
readIndexFile(const CommandLineOptions & clo, const bd::IndexFileHeader &header)
{
//  if (clo.outputFileType == "ascii") {
//    std::cerr << "Reading the ascii index file type isn't implemented.\n "
//        "However, not all hope is lost! Because it is ASCII text, you can \n"
//        "open it in a text editor and read it manually :) ." <<
//    std::endl;
//    cleanUp();
//    exit(1);
//  }

  // open index file (binary)
  std::ifstream inFile{ clo.filePath, std::ios::binary };
  if (! inFile.is_open()){
    std::cerr << "The file " << clo.filePath << " could not be opened." << std::endl;
    cleanUp();
    exit(1);
  }

  // get the header so we know params for BlockCollection object.
  bd::BlockCollection2<Ty> collection{
      { clo.vol_w, clo.vol_h, clo.vol_d },
      { clo.numblk_x, clo.numblk_y, clo.numblk_z }};

  // Read the rest of file into BlockCollection
  bd::IndexFile<Ty> index{ collection, header };
  index.readBinary(inFile);

  // Print the collection to stdout.
  for (auto &block : collection.blocks()) {
    std::cout << block << std::endl;
  }
}

template<typename Ty>
void
execute(const CommandLineOptions &clo)
{
  if (clo.actionType == ActionType::WRITE) {
    generateIndexFile<Ty>(clo);
  } else {
    readIndexFile<Ty>(clo);
  }
}


///////////////////////////////////////////////////////////////////////////////
int
main(int argc, const char *argv[])
{
  bd::gl_log_restart();

  CommandLineOptions clo;
  if (parseThem(argc, argv, clo) == 0) {
    gl_log_err("Command line parse error, exiting.");
    cleanUp();
    exit(1);
  }

  if (clo.actionType == ActionType::WRITE) {
    bd::DatFileData datfile;
    if (!clo.datFilePath.empty()) {
      bd::parseDat(clo.datFilePath, datfile);
      clo.vol_w = datfile.rX;
      clo.vol_h = datfile.rY;
      clo.vol_d = datfile.rZ;
      clo.type = bd::to_string(datfile.dataType);
      std::cout << datfile << "\n.";
    }

  } else {
    // if reading, we don't care what the data type actually is, because
    // we will never open the volume data.
    clo.type = bd::to_string(bd::DataType::Float);
  }

  printThem(clo); // print cmd line options
  bd::DataType type;
  try {
    type = bd::DataTypesMap.at(clo.type);
  } catch (std::exception e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }

  switch (type) {

  case bd::DataType::UnsignedCharacter:
    execute<unsigned char>(clo);
    break;

  case bd::DataType::UnsignedShort:
    execute<unsigned short>(clo);
    break;

  case bd::DataType::Float:
    execute<float>(clo);
    break;

  default:
    std::cerr << "Unsupported/unknown datatype: " << clo.type << ".\n";
    break;
  }


  cleanUp();
  return 0;
}

