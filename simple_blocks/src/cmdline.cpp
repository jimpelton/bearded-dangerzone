#include "cmdline.h"

#include <boost/program_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include <iostream>
#include <string>

int parseThem(int argc, const char *argv[], CommandLineOptions &opts)
try {
    using std::string;
    namespace po = boost::program_options;

    boost::program_options::variables_map m_vm;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help message")
        ("file,f", po::value<std::string>()->required(), "Path to data file.")
        ("type,t", po::value<std::string>()->default_value("float"), "float, ushort, uchar")
        ("xdim,x", po::value<size_t >()->default_value(32), "X dimension of volume")
        ("ydim,y", po::value<size_t >()->default_value(32), "Y dimension of volume")
        ("zdim,z", po::value<size_t >()->default_value(32), "Z dimension of volume")
        ("nbx",    po::value<size_t >()->default_value(1),  "Num blocks X")
        ("nby",    po::value<size_t >()->default_value(1),  "Num blocks Y")
        ("nbz",    po::value<size_t >()->default_value(1),  "Num blocks Z")
        ("threshold,r", po::value<float>()->default_value(0.1f),  "Threshold")
        ;

    po::command_line_parser parser{ argc, argv };
    parser.options(desc);
    po::parsed_options options = parser.run();
    po::store(options, m_vm);

    if (m_vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    po::notify(m_vm);

    opts.filePath = m_vm["file"].as<std::string>();
    opts.type = m_vm["type"].as<std::string>();
    opts.w = m_vm["xdim"].as<size_t>();
    opts.h = m_vm["ydim"].as<size_t>();
    opts.d = m_vm["zdim"].as<size_t>();
    opts.numblk_x = m_vm["nbx"].as<size_t>();
    opts.numblk_y = m_vm["nby"].as<size_t>();
    opts.numblk_z = m_vm["nbz"].as<size_t>();
    opts.threshold = m_vm["threshold"].as<float>();

    return static_cast<int>(m_vm.size());

} catch (boost::program_options::error &e) {
    std::cerr << e.what() << std::endl;
    return 0;
}

void printThem(const CommandLineOptions &opts)
{
    std::cout <<
    "File path: "  << opts.filePath   << "\n"
    "Data Type: "  << opts.type       << "\n"
    "Num blocks (x,y,z): " << opts.numblk_x << ", " << opts.numblk_y << ", "
        << opts.numblk_z << "\n"
    "Vol width: "  << opts.w          << "\n"
    "Vol height: " << opts.h          << "\n"
    "Vol depth: "  << opts.d          << "\n"
    "Threshold: "  << opts.threshold << std::endl;
}