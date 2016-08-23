//
// Created by jim on 8/23/16.
//

#include "cmdline.h"

#include <bd/io/datatypes.h>
#include <bd/log/logger.h>
#include <bd/io/buffer.h>
#include <bd/io/bufferedreader.h>

#include <iostream>
#include <fstream>

namespace rawhist
{
template<typename Ty>
void hist(std::istream &s, size_t szbuf)
{
  bd::BufferedReader<Ty> r(szbuf);

}

void
generate(CommandLineOptions &clo)
{
  // Decide what data type we have and call execute to kick off the processing.
  bd::DataType type{ bd::to_dataType(clo.dataType) };
  std::ifstream s;
  switch (type) {

    case bd::DataType::UnsignedCharacter:
      hist<unsigned char>(s, clo.bufferSize );
      break;

    case bd::DataType::UnsignedShort:
      hist<unsigned short>(s, clo.bufferSize);
      break;

    case bd::DataType::Float:
      hist<float>(s, clo.bufferSize);
      break;

    default:
      bd::Err() << "Unsupported/unknown datatype: " << clo.dataType << ".\n Exiting...";
      break;
  }

  //print histo to stdout.
}

} // namespace rawhist


int main(int argc, const char *argv[])
{
  rawhist::CommandLineOptions clo;
  if (rawhist::parseThem(argc, argv, clo) == 0) {
    std::cout << "No arguments provided.\nPlease use -h for usage info."
              << std::endl;
    return 1;
  }

  rawhist::generate(clo);

  return 0;
}


