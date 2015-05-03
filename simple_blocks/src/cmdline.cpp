#include "cmdline.h"

#include <tclap/CmdLine.h>

//#include <boost/program_options.hpp>
//#include <boost/program_options/variables_map.hpp>

#include <iostream>
#include <string>

int parseThem(int argc, const char *argv[], CommandLineOptions &opts)
try
{
    if (argc == 1) {
        return 0;
    }

    TCLAP::CmdLine cmd("Simple Blocks blocking volume render experiment.", ' ');

    // volume data file
    TCLAP::ValueArg<std::string> fileArg("f", "file", "Path to data file.", false, "", "string");
    cmd.add(fileArg);

    // transfer function file
    TCLAP::ValueArg<std::string> tfuncArg("u", "tfunc", "Path to transfer function file.", false, "", "string");
    cmd.add(tfuncArg);


    // volume data type
    std::vector<std::string> dataTypes{ "float", "ushort", "uchar" };
    TCLAP::ValuesConstraint<std::string> dataTypeAllowValues(dataTypes);
    TCLAP::ValueArg<std::string> dataTypeArg("t", "type", "Data type (float, ushort, uchar).", false, "", &dataTypeAllowValues);;
    cmd.add(dataTypeArg);

    
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

	TCLAP::ValueArg<unsigned int> numSlicesArg("s", "num-slices", "Num slices per block", false, 1, "uint");
	cmd.add(numSlicesArg);

    // threshold min/max
    TCLAP::ValueArg<float> tmin("", "tmin", "Thresh min", false, 0.0, "float");
    cmd.add(tmin);
    
    TCLAP::ValueArg<float> tmax("", "tmax", "Thresh max", false, 1.0, "float");
    cmd.add(tmax);


    // print blocks
    TCLAP::SwitchArg printBlocksArg("", "print-blocks", "Print blocks into to stdout.", cmd, false);

    

    cmd.parse(argc, argv);

    opts.filePath = fileArg.getValue();
    opts.tfuncPath = tfuncArg.getValue();
    opts.type = dataTypeArg.getValue(); 
    opts.printBlocks = printBlocksArg.getValue();
    opts.w = xdimArg.getValue();
    opts.h = ydimArg.getValue();
    opts.d = zdimArg.getValue();
    opts.numblk_x = xBlocksArg.getValue();
    opts.numblk_y = yBlocksArg.getValue();
    opts.numblk_z = zBlocksArg.getValue();
    opts.num_slices = numSlicesArg.getValue();
    opts.tmin = tmin.getValue();
    opts.tmax = tmax.getValue();

    return cmd.getArgList().size();

} catch (TCLAP::ArgException &e) {
    std::cout << "Error parsing command line args: " << e.error() << " for argument " 
        << e.argId() << std::endl;
    return 0;
}


void printThem(CommandLineOptions &opts)
{
    std::cout <<
        "File path: " << opts.filePath << "\n"
        "Data Type: " << opts.type     << "\n"
        "Vol dims (w X h X d): "   << opts.w        << " X " << opts.h        << " X " << opts.d        << "\n"
        "Num blocks (x X y X z): " << opts.numblk_x << " X " << opts.numblk_y << " X " << opts.numblk_z << "\n"
        "Num Slices: " << opts.num_slices << "\n"
        "Threshold (min-max): "    << opts.tmin     << " - " << opts.tmax     << "\n"
        "Print blocks: "           << (opts.printBlocks ? "True" : "False")   << 
    std::endl;
}

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
//    opts.tfuncPath = m_vm["tfunc"].as<std::string>();
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