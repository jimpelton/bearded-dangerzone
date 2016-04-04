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

#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>


template<typename Ty>
void
generateIndexFile(const CommandLineOptions &clo)
{

  float minmax[2];
  minmax[0] = clo.tmin;
  minmax[1] = clo.tmax;
  //const size_t bufsz = 67'108'864;
  try {
    std::shared_ptr<bd::IndexFile> indexFile{
        bd::IndexFile::fromRawFile(
            clo.inFile,
            clo.bufferSize,
            bd::to_dataType(clo.dataType),
            clo.vol_dims,
            clo.num_blks,
            minmax)
    };

    switch (clo.outputFileType) {

    case OutputType::Ascii: {
      std::stringstream outFileName;
      outFileName << clo.outFilePath << '/' << clo.outFilePrefix << '_' <<
          clo.num_blks[0] << '-' << clo.num_blks[1] << '-' << clo.num_blks[2] << '_' <<
          clo.tmin << '-' << clo.tmax << ".json";
      indexFile->writeAsciiIndexFile(outFileName.str());
      break;
    }
    case OutputType::Binary: {
      std::stringstream outFileName;
      outFileName << clo.outFilePath << '/' << clo.outFilePrefix << '_' <<
          clo.num_blks[0] << '-' << clo.num_blks[1] << '-' << clo.num_blks[2] << '_' <<
          clo.tmin << '-' << clo.tmax << ".bin";
      indexFile->writeBinaryIndexFile(outFileName.str());
      break;
    }
    }

    if (clo.printBlocks) {
      std::stringstream ss;
      ss << "{\n";
      for (std::shared_ptr<bd::FileBlock> block : indexFile->blocks()) {
        std::cout << *block << std::endl;
      }
      ss << "}\n";
    }
  } catch (std::runtime_error e) {
    std::cerr << e.what() << std::endl;
    gl_log_err("%s", e.what());
  }
}

template<typename Ty>
void
readIndexFile(const CommandLineOptions & clo)
{

  std::shared_ptr<bd::IndexFile> index{
      bd::IndexFile::fromBinaryIndexFile(clo.inFile)
  };

  auto startName = clo.inFile.rfind('/')+1;
  auto endName = startName + (clo.inFile.size() - clo.inFile.rfind('.'));
  std::string name(clo.inFile, startName, endName);
  name += ".json";

  index->writeAsciiIndexFile(clo.outFilePath + '/' + name);
}

template<typename Ty>
void
execute(const CommandLineOptions &clo)
{
  if (clo.actionType == ActionType::Generate) {
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

  if (clo.actionType == ActionType::Generate) {
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

  std::cout << clo << std::endl; // print cmd line options

  bd::DataType type{ bd::to_dataType(clo.dataType) };
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
    std::cerr << "Unsupported/unknown datatype: " << clo.dataType << ".\n Exiting...";
    return 1;
    break;
  }


  return 0;
}

