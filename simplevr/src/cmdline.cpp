#include "cmdline.h"

#include <boost/program_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include <iostream>
#include <string>

int parseThem(int argc, char *argv [], CommandLineOptions &opts)
try {
    using std::string;
    namespace po = boost::program_options;
    boost::program_options::variables_map m_vm;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("raw-file", po::value<string>()->required(), "Volume file")
        ("help,h", "Show help message")
        ("dat,d", po::value<string>(), "(optional) Path to dat raw file description")
        ("type,t", po::value<string>()->required(), "Data type. C numeric primitives are accepted: char, float, \"unsigned int\", etc.")
        ("side,s", po::value<int>()->required(), "Length in voxels of a block side")
        ("xdim,x", po::value<int>()->required(), "X dimension of volume")
        ("ydim,y", po::value<int>()->required(), "Y dimension of volume")
        ("zdim,z", po::value<int>()->required(), "Z dimension of volume")
        ;

    po::positional_options_description p;
    p.add("raw-file", -1);
    po::command_line_parser parser{ argc, argv };
    parser.options(desc).positional(p); // .allow_unregistered();

    po::parsed_options options = parser.run();
    po::store(options, m_vm);

    if (m_vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    po::notify(m_vm);

    //     if (m_vm.count("d")) {
    //         readDatFile(m_vm["d"].as<string>());
    //     } else {
    opts.filePath = m_vm["raw-file"].as<string>();
    opts.type = m_vm["type"].as<string>();
    opts.block_side = m_vm["side"].as<int>();
    opts.w = m_vm["xdim"].as<int>();
    opts.h = m_vm["ydim"].as<int>();
    opts.d = m_vm["zdim"].as<int>();
    //  }

    return static_cast<int>(m_vm.size());

} catch (boost::program_options::error &e) {
    std::cerr << e.what() << std::endl;
    return 0;
}

