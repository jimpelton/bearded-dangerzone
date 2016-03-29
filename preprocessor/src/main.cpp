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






template<typename Ty>
void
generateIndexFile(const CommandLineOptions &clo)
{

  float minmax[2];
  minmax[0] = clo.tmin;
  minmax[1] = clo.tmax;

  std::shared_ptr<bd::IndexFile> indexFile{
      bd::IndexFile::fromRawFile(
        clo.inFilePath,
        bd::DataTypesMap.at(clo.dataType),
        clo.vol_dims,
        clo.num_blks,
        minmax)
  };

  switch(clo.outputFileType){
  case OutputType::Ascii:
    indexFile->writeAsciiIndexFile(clo.outFilePath);
    break;

  case OutputType::Binary:
    indexFile->writeBinaryIndexFile(clo.outFilePath);
    break;
  }

  if (clo.printBlocks) {
    std::stringstream ss;
    ss << "{\n";
    for (std::shared_ptr<bd::FileBlock> block : indexFile->blocks()) {
      std::cout << *block << std::endl;
    }
    ss << "}\n";
  }
}

template<typename Ty>
void
readIndexFile(const CommandLineOptions & clo)
{

  std::shared_ptr<bd::IndexFile> index{
      bd::IndexFile::fromBinaryIndexFile(clo.inFilePath)
  };

  index->writeAsciiIndexFile(clo.outFilePath);

}

template<typename Ty>
void
execute(const CommandLineOptions &clo)
{
  if (clo.actionType == ActionType::GENERATE) {
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
    exit(1);
  }

  if (clo.actionType == ActionType::GENERATE) {
    if (!clo.datFilePath.empty()) {
      bd::DatFileData datfile;
      bd::parseDat(clo.datFilePath, datfile);
      clo.vol_dims[0] = datfile.rX;
      clo.vol_dims[1] = datfile.rY;
      clo.vol_dims[2] = datfile.rZ;
      clo.dataType = bd::to_string(datfile.dataType);
      std::cout << ".dat file: \n" << datfile << "\n.";
    }
  }

  printThem(clo); // print cmd line options

  bd::DataType type;
  try {
    type = bd::DataTypesMap.at(clo.dataType);
  } catch (std::exception e) {
    std::cerr << e.what() << std::endl;
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
    std::cerr << "Unsupported/unknown datatype: " << clo.dataType << ".\n Executing with float data type as default.";
    execute<float>(clo);
    break;
  }


  return 0;
}

