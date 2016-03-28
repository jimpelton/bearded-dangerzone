#include "cmdline.h"

#include <tclap/CmdLine.h>

#include <iostream>
#include <string>

int 
parseThem(int argc, const char *argv[], CommandLineOptions &opts)
try {
  if (argc == 1) {
    return 0;
  }

  TCLAP::CmdLine cmd("Preprocessor.", ' ');


  // volume data file
  TCLAP::ValueArg<std::string> fileArg("f", "file", "Path to data file.", false, "", "string");
  cmd.add(fileArg);

  //output file
  TCLAP::ValueArg<std::string> outFileArg("o", "outfile", "Path to output file", false, "", "string");
  cmd.add(outFileArg);

  // transfer function file
  TCLAP::ValueArg<std::string> tfuncArg("u", "tfunc", "Path to transfer function file.", false, "", "string");
  cmd.add(tfuncArg);

  // .dat file
  TCLAP::ValueArg<std::string> datFileArg("d", "datfile", "Path to .dat file", false, "", "string");
  cmd.add(datFileArg);

  // volume data type
  std::vector<std::string> dataTypes{ "float", "ushort", "uchar" };
  TCLAP::ValuesConstraint<std::string> dataTypeAllowValues(dataTypes);
  TCLAP::ValueArg<std::string> dataTypeArg("t", "type", "Data type (float, ushort, uchar).", false, "", &dataTypeAllowValues);
  cmd.add(dataTypeArg);

  // ouput file type
  std::vector<std::string> outputTypes{ "ascii", "binary" };
  TCLAP::ValuesConstraint<std::string> outputTypesAllowValues(outputTypes);
  TCLAP::ValueArg<std::string> outputTypeArg("", "output", "Output file type (ascii, binary)", false, "", &outputTypesAllowValues);
  cmd.add(outputTypeArg);

  TCLAP::SwitchArg readArg("r", "read", "Read existing index file");
  cmd.add(readArg);

  TCLAP::SwitchArg writeArg("w", "write", "Write new index file from raw file");
  cmd.add(writeArg);

  // volume dims
  TCLAP::ValueArg<size_t> xdimArg("", "volx", "Volume x dim.", false, 1, "uint");
  cmd.add(xdimArg);

  TCLAP::ValueArg<size_t> ydimArg("", "voly", "Volume y dim.", false, 1, "uint");
  cmd.add(ydimArg);

  TCLAP::ValueArg<size_t> zdimArg("", "volz", "Volume z dim.", false, 1, "uint");
  cmd.add(zdimArg);


  // num blocks
  TCLAP::ValueArg<size_t> xBlocksArg("", "nbx", "Num blocks x dim", false, 1, "uint");
  cmd.add(xBlocksArg);

  TCLAP::ValueArg<size_t> yBlocksArg("", "nby", "Num blocks y dim", false, 1, "uint");
  cmd.add(yBlocksArg);

  TCLAP::ValueArg<size_t> zBlocksArg("", "nbz", "Num blocks z dim", false, 1, "uint");
  cmd.add(zBlocksArg);


  // threshold min/max
  TCLAP::ValueArg<float> tmin("", "tmin", "Thresh min", false, 0.0, "float");
  cmd.add(tmin);

  TCLAP::ValueArg<float> tmax("", "tmax", "Thresh max", false, 1.0, "float");
  cmd.add(tmax);


  // print blocks
  TCLAP::SwitchArg printBlocksArg("", "print-blocks", "Print blocks into to stdout.", cmd, false);


  cmd.parse(argc, argv);

  opts.actionType = readArg.getValue() ?
                    ActionType::READ : ActionType::WRITE;
  opts.filePath = fileArg.getValue();
  opts.outFilePath = outFileArg.getValue();
  opts.tfuncPath = tfuncArg.getValue();
  opts.datFilePath = datFileArg.getValue();
  opts.type = dataTypeArg.getValue();
  opts.outputFileType = outputTypeArg.getValue() == "ascii" ?
                        OutputType::Ascii : OutputType::Binary;
  opts.printBlocks = printBlocksArg.getValue();
  opts.vol_w = xdimArg.getValue();
  opts.vol_h = ydimArg.getValue();
  opts.vol_d = zdimArg.getValue();
  opts.numblk_x = xBlocksArg.getValue();
  opts.numblk_y = yBlocksArg.getValue();
  opts.numblk_z = zBlocksArg.getValue();
  opts.tmin = tmin.getValue();
  opts.tmax = tmax.getValue();

  return static_cast<int>(cmd.getArgList().size());

} catch (TCLAP::ArgException &e) {
  std::cerr << "Error parsing command line args: " << e.error() << " for argument "
      << e.argId() << std::endl;
  return 0;
}


void 
printThem(const CommandLineOptions &opts) 
{
  std::cout << opts << std::endl;
}

std::ostream &
operator<<(std::ostream & os, const CommandLineOptions &opts)
{
  os <<
    "Action type: " << (opts.actionType == ActionType::READ ? "Read" : "Write") << "\n"
    "File path: " << opts.filePath << "\n"
    "Output file path: " << opts.outFilePath << "\n"
    "Transfer function path: " << opts.tfuncPath << "\n"
    "Dat file: " << opts.datFilePath << "\n"
    "Data Type: " << opts.type << "\n"
    "Output file type: " << (opts.outputFileType == OutputType::Binary ? "Binary" : "Ascii") << "\n"
    "Vol dims (w X h X d): " << opts.vol_w << " X " << opts.vol_h << " X " << opts.vol_d << "\n"
    "Num blocks (x X y X z): " << opts.numblk_x << " X " << opts.numblk_y << " X " << opts.numblk_z << "\n"
    "Threshold (min-max): " << opts.tmin << " - " << opts.tmax << "\n"
    "Print blocks: " << (opts.printBlocks ? "True" : "False");

  return os;
}


