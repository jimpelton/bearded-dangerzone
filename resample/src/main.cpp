//
// Created by jim on 2/19/17.
//

#include "cmdline.h"
#include "grid.h"

#include <bd/log/logger.h>
#include <bd/io/datfile.h>

#include <iostream>
#include <fstream>
#include <cstring>

template<class Ty>
void
go(resample::CommandLineOptions &cmdOpts, std::ifstream *inFile, std::ofstream *outFile)
{
    size_t orig_c{ cmdOpts.vol_dims[0] }, new_c{ cmdOpts.new_vol_dims[0] }; // col
    size_t orig_r{ cmdOpts.vol_dims[1] }, new_r{ cmdOpts.new_vol_dims[1] }; // row
    size_t orig_s{ cmdOpts.vol_dims[2] }, new_s{ cmdOpts.new_vol_dims[2] }; // slab

    // read original data into memory
//    inFile->seekg(0, std::ios::end);
//    std::ifstream::pos_type fileSize{ inFile->tellg() };
//    inFile->seekg(0, std::ios::beg);
    size_t fileSize{ orig_c * orig_r * orig_s * sizeof(Ty) };
    char *image{ new char[fileSize] };
    inFile->read(image, fileSize);
    inFile->close();


    resample::Grid<Ty> grid{ 
      orig_c, orig_r, orig_s, reinterpret_cast<Ty*>(image) };

    size_t slabSize{ new_c * new_r };
    Ty *slab{ new Ty[slabSize] };
    std::memset(slab, 0, slabSize * sizeof(Ty));


    for (size_t s{ 0 }; s < new_s; ++s) {
      for (size_t r{ 0 }; r < new_r; r++) {
        for (size_t c{ 0 }; c < new_c; c++) {

          Ty ival{
              grid.interpolate({ s / float(new_s), r / float(new_r), c / float(new_c) }) };

          slab[c + r * new_c] = ival;

        } //for c
      } // for r

      outFile->write(reinterpret_cast<char *>(slab), slabSize * sizeof(Ty));
      if (s % 10 == 0) {
        std::cout << "\r Wrote slab: " << s << std::flush;
      }

      memset(slab, 0, slabSize * sizeof(Ty));

    } // for(s

    std::cout << std::endl;

    outFile->flush();
    outFile->close();

    delete[] image;
    delete[] slab;
  
}

int main(int argc, char const **argv)
{
  resample::CommandLineOptions cmdOpts;
  if (resample::parseThem(argc, argv, cmdOpts) == 0) {
    std::cerr << "Please use -h for usage." << std::endl;
    return 1;
  }


  std::ifstream inFile(cmdOpts.inFilePath, std::ios::binary);
  if (!inFile.is_open()) {
    bd::Err() << "Input file was not opened: " << cmdOpts.inFilePath;
    return 1;
  }


  std::ofstream outFile(cmdOpts.outFilePath, std::ios::binary);
  if (!outFile.is_open()) {
    bd::Err() << "Output file was not opened: " << cmdOpts.outFilePath << std::endl;
    return 1;
  }

  if (!cmdOpts.datFilePath.empty()) {

    // check given dat file exists
    std::ifstream datFile(cmdOpts.datFilePath, std::ios::binary);
    if (!datFile.is_open()) {
      bd::Err() << "Dat file was not opened: " << cmdOpts.datFilePath << std::endl;
      return 1;
    }
    datFile.close();

    bd::DatFileData dfd;
    bd::parseDat(cmdOpts.datFilePath, dfd);
    cmdOpts.vol_dims[0] = dfd.rX;
    cmdOpts.vol_dims[1] = dfd.rY;
    cmdOpts.vol_dims[2] = dfd.rZ;
    cmdOpts.dataType = dfd.dataType;

    switch (dfd.dataType) {
    case bd::DataType::UnsignedCharacter:
      go<uint8_t>(cmdOpts, &inFile, &outFile);
      break;
    case bd::DataType::UnsignedShort:
      go<uint16_t>(cmdOpts, &inFile, &outFile);
      break;
    default:
      std::cerr << "Not a data type I am willing to deal with yet." << std::endl;
      return 1;
    }

  }
  else {
    std::cerr << "Give a dat file." << std::endl;
  }

  return 0;
}