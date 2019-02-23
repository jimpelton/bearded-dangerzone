#include "cmdline.h"

#include <iostream>
#include <string>

namespace subvol
{

int
parseThem(int argc, char *argv[], TCLAP::CmdLine &cmd, CommandLineOptions &opts)
try
{
//  if (argc == 1) {
//    return 0;
//  }


  // volume data file
  TCLAP::ValueArg<std::string>
      fileArg("f", "file", "Path to data file.", false, "", "string");
  cmd.add(fileArg);

  // transfer function file
  TCLAP::ValueArg<std::string>
      tfuncArg("u", "tfunc", "Path to transfer function file.", false, "", "string");
  cmd.add(tfuncArg);


  // opacity transfer function file
  TCLAP::ValueArg<std::string> opacityTFArg("",
                                            "otf",
                                            "Path to the opacity transfer function."
                                            "Takes precende over --tf"
                                            " and must be used with --ctf.",
                                            false,
                                            "",
                                            "string");
  cmd.add(opacityTFArg);


  // color transfer function file
  TCLAP::ValueArg<std::string> colorTFArg("",
                                          "ctf",
                                          "Path to the color transfer function. "
                                          "Takes precedencd over --tf"
                                          " and must be used with --otf.",
                                          false,
                                          "",
                                          "string");
  cmd.add(colorTFArg);

  // index file path
  TCLAP::ValueArg<std::string>
      indexFilePath("", "index-file", "Path to index file.", false, "", "string");
  cmd.add(indexFilePath);

  TCLAP::ValueArg<unsigned int>
      numSlicesArg("s", "num-slices", "Num slices per block", false, 1, "uint");
  cmd.add(numSlicesArg);





  // perf output file path
  TCLAP::ValueArg<std::string>
      perfOutPathArg("",
                     "perf-out-file",
                     "Print performance metrics to file.",
                     false,
                     "",
                     "string");
  cmd.add(perfOutPathArg);

  // perf mode (exit on when done with performance test)
  TCLAP::SwitchArg perfMode("p",
                            "perf-mode",
                            "Exit as soon as performance test is done.",
                            cmd, false);

  TCLAP::ValueArg<int>
      screenWidthArg("", "screen-width", "Width of the window", false, 1280,
                     "int");
  cmd.add(screenWidthArg);

  TCLAP::ValueArg<int>
      screenHeightArg("", "screen-height", "Height of the window", false, 720,
                      "int");
  cmd.add(screenHeightArg);

  TCLAP::ValueArg<std::string>
      gpuMemoryArg("", "gpu-mem", "Gpu memory to use", false, "512M", "string");
  cmd.add(gpuMemoryArg);

  TCLAP::ValueArg<std::string>
      mainMemoryArg("", "main-mem", "Cpu memory to use", false, "1G", "string");
  cmd.add(mainMemoryArg);

  TCLAP::ValueArg<float>
      samplingModifierXArg("", "smod-x", "Sampling modifier", false, 0, "float");
  cmd.add(samplingModifierXArg);

  TCLAP::ValueArg<float>
      samplingModifierYArg("", "smod-y", "Sampling modifier", false, 0, "float");
  cmd.add(samplingModifierYArg);

  TCLAP::ValueArg<float>
      samplingModifierZArg("", "smod-z", "Sampling modifier", false, 0, "float");
  cmd.add(samplingModifierZArg);

  cmd.parse(argc, argv);

  opts.rawFilePath = fileArg.getValue();
  opts.tfunc1dtPath = tfuncArg.getValue();
  opts.opacityTFuncPath = opacityTFArg.getValue();
  opts.colorTFuncPath = colorTFArg.getValue();
  opts.indexFilePath = indexFilePath.getValue();
  opts.num_slices = numSlicesArg.getValue();
  opts.perfOutPath = perfOutPathArg.getValue();
  opts.perfMode = perfMode.getValue();
  opts.windowWidth = screenWidthArg.getValue();
  opts.windowHeight = screenHeightArg.getValue();
  opts.gpuMemoryBytes = static_cast<int64_t>(convertToBytes(gpuMemoryArg.getValue()));
  opts.mainMemoryBytes = static_cast<int64_t>(convertToBytes(mainMemoryArg.getValue()));
  opts.smod_x = samplingModifierXArg.getValue();
  opts.smod_y = samplingModifierYArg.getValue();
  opts.smod_z = samplingModifierZArg.getValue();

  return static_cast<int>(cmd.getArgList().size());

} catch (TCLAP::ArgException &e) {
  std::cout << "Error parsing command line args: " << e.error() << " for argument "
            << e.argId() << std::endl;
  return 0;
}


size_t
convertToBytes(std::string s)
{
  size_t multiplier{ 1 };
  std::string last{ *( s.end()-1 ) };

  if (last=="K") {
    multiplier = 1024;
  } else if (last=="M") {
    multiplier = 1024*1024;
  } else if (last=="G") {
    multiplier = 1024*1024*1024;
  }

  std::string numPart(s.begin(), s.end()-1);
  auto num = stoull(numPart);

  return num*multiplier;
}


void
printThem(CommandLineOptions &opts)
{
  std::cout
      << "File path: " << opts.rawFilePath
      << "\nTransfer function: " << opts.tfunc1dtPath
      << "\nPerf out file: " << opts.perfOutPath
      << "\nPerf mode: " << opts.perfMode
      << "\nData Type: " << opts.dataType
      << "\nVol dims (w X h X d): " << opts.vol_w << " X " << opts.vol_h << " X "
      << opts.vol_d
      << "\nNum blocks (x X y X z): " << opts.numblk_x << " X " << opts.numblk_y << " X "
      << opts.numblk_z
      << "\nNum Slices: " << opts.num_slices
      << "\nWindow dims: " << opts.windowWidth << " X " << opts.windowHeight
      << "\nCpu memory: " << opts.mainMemoryBytes
      << "\nGpu memory: " << opts.gpuMemoryBytes
      << std::endl;
}

} // namespace subvol

//namespace {
//    boost::program_options::variables_map m_vm;
//}
//
//
//int parseThem(int argc, const char *argv[], CommandLineOptions &opts)
//try {
//    using std::string;
//    namespace po = boost::program_options;
//
//    po::options_description desc("Allowed options");
//    desc.add_options()
//        ("help,h", "Show help message")
//        ("file,f", po::value<std::string>()->required(), "Path to data file.")
//        ("tfunc,u",po::value<std::string>()->default_value(""), "Path to transfer function file.")
//        ("type,t", po::value<std::string>()->default_value("float"), "float, ushort, uchar")
//        ("xdim,x", po::value<size_t >()->default_value(32), "X dimension of volume")
//        ("ydim,y", po::value<size_t >()->default_value(32), "Y dimension of volume")
//        ("zdim,z", po::value<size_t >()->default_value(32), "Z dimension of volume")
//        ("nbx",    po::value<size_t >()->default_value(1),  "Num blocks X")
//        ("nby",    po::value<size_t >()->default_value(1),  "Num blocks Y")
//        ("nbz",    po::value<size_t >()->default_value(1),  "Num blocks Z")
//        ("tmin",   po::value<float>()->default_value(0.0f),  "Min threshold")
//        ("tmax",   po::value<float>()->default_value(1.0f),  "Max threshold")
//        ("print-blocks",  "Dump block info to blocks.txt.")
//        ;
//
//    po::command_line_parser parser{ argc, argv };
//    parser.options(desc);
//    po::parsed_options options = parser.run();
//    po::store(options, m_vm);
//
//    if (m_vm.count("help")) {
//        std::cout << desc << std::endl;
//        return 0;
//    }
//
//    po::notify(m_vm);
//
//    opts.filePath = m_vm["file"].as<std::string>();
//    opts.printBlocks = m_vm.count("print-blocks") ? true : false;
//    opts.tfunc1dtPath = m_vm["tfunc"].as<std::string>();
//    opts.type = m_vm["type"].as<std::string>();
//    opts.w = m_vm["xdim"].as<size_t>();
//    opts.h = m_vm["ydim"].as<size_t>();
//    opts.d = m_vm["zdim"].as<size_t>();
//    opts.numblk_x = m_vm["nbx"].as<size_t>();
//    opts.numblk_y = m_vm["nby"].as<size_t>();
//    opts.numblk_z = m_vm["nbz"].as<size_t>();
//    opts.tmin = m_vm["tmin"].as<float>();
//    opts.tmax = m_vm["tmax"].as<float>();
//
//    return static_cast<int>(m_vm.size());
//
//} catch (boost::program_options::error &e) {
//    std::cerr << e.what() << std::endl;
//    return 0;
//}
//
//void printThem()
//{
//
//    std::stringstream ss;
//
//    for (const auto& it : m_vm) {
//        ss << it.first.c_str() << " ";
//        auto& value = it.second.value();
//        if (auto v = boost::any_cast<int>(&value))
//            ss << *v << "\n";
//        else if (auto v = boost::any_cast<size_t >(&value))
//            ss << *v << "\n";
//        else if (auto v = boost::any_cast<std::string>(&value))
//            ss << *v << "\n";
//        else if (auto v = boost::any_cast<float>(&value))
//            ss << *v << "\n";
//        else
//            ss << "No value. \n";
//    }
//
//    gl_log("%s", ss.str().c_str());
//
//
////    std::cout <<
////    "File path: "  << opts.filePath   << "\n"
////    "Data Type: "  << opts.type       << "\n"
////    "Num blocks (x,y,z): " << opts.numblk_x << ", " << opts.numblk_y << ", "
////        << opts.numblk_z << "\n"
////    "Vol width: "  << opts.w          << "\n"
////    "Vol height: " << opts.h          << "\n"
////    "Vol depth: "  << opts.d          << "\n"
////    "Threshold (min-max): "  << opts.tmin << "-" << opts.tmax << std::endl;
//}