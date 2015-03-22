

#include "log/gl_log.h"
#include "file/datareader.h"



#include <boost/program_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <map>


struct CommandLineOptions {
    std::string filePath;
    std::string type;
    int side;
    size_t x, y, z;
} cl_options;

enum class DataType { Integer, Character, UnsignedInteger, UnsignedCharacter, Short, UnsignedShort, Float};

std::map<std::string, DataType> g_dataTypesMap {
    { "char", DataType::Character },
    { "int", DataType::Integer },
    { "unsigned char", DataType::UnsignedCharacter },
    { "unsigned integer", DataType::UnsignedInteger },
    { "short", DataType::Short },
    { "unsigned short", DataType::UnsignedShort },
    { "float", DataType::Float }
};


int parseThem(int argc, char *argv[])
try{
    using std::string;
    namespace po = boost::program_options;
    boost::program_options::variables_map m_vm;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help message")
        ("raw-file", po::value<string>()->required(), "Volume file")
        ("t", po::value<string>()->required(), "Data type. C numeric primitives are accepted: char, float, \"unsigned int\", etc.")
        ("s,side", po::value<string>()->required(), "Length in voxels of a block side")
        ("x", po::value<int>()->required(), "X dimension of volume")
        ("y", po::value<int>()->required(), "Y dimension of volume")
        ("z", po::value<int>()->required(), "Z dimension of volume")
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

    cl_options.filePath = m_vm["raw-file"].as<string>();
    cl_options.type = m_vm["t"].as<string>();
    cl_options.side = m_vm["side"].as<int>();
    cl_options.x = m_vm["x"].as<size_t>();
    cl_options.y = m_vm["y"].as<size_t>();
    cl_options.z = m_vm["z"].as<size_t>();
    

    return static_cast<int>(m_vm.size());
}
catch (boost::program_options::error &e) {
    std::cerr << e.what() << std::endl;
    return 0;
}

float* readData(const std::string &dtype, const std::string &fname, size_t x, size_t y, size_t z) {
    DataType t = g_dataTypesMap[dtype];
    float *rval = nullptr;
    switch (t) {
    case DataType::Float:
    {
        bearded::dangerzone::file::DataReader<float, float> reader;
        reader.loadRaw3d(cl_options.filePath, cl_options.x, cl_options.y, cl_options.z);
        rval = reader.takeOwnership();
        break;
    }
    default:
        break;
    }
    
    return rval;
}

struct Block {
    glm::vec3 min;
    glm::vec3 dims;
    float average;

    Block() 
        : min(glm::vec3(0.0f))
        , dims(glm::vec3(0.0f))
        , average(0.0f) 
    { }
};



int main(int argc, char *argv[]) {
    if (parseThem(argc, argv) == 0) {
        std::cerr << "Error parsing arguments, exiting...\n";
        exit(1);
    }
    
    float* data = readData(cl_options.type, cl_options.filePath, 
        cl_options.x, cl_options.y, cl_options.z);

    size_t voxels = cl_options.x * cl_options.y * cl_options.z;
    int numBlocks = voxels / cl_options.side;

    std::vector<Block> blockies = std::vector<Block>(numBlocks*numBlocks*numBlocks);
    Block ***blocks = (Block***)blockies.data();

    for (int z = 0; z < cl_options.z; ++z)
    for (int y = 0; y < cl_options.y; ++y)
    for (int x = 0; x < cl_options.x; ++x) {
        int bx = x / cl_options.side;
        int by = y / cl_options.side;
        int bz = z / cl_options.side;
        size_t idx = x + cl_options.x * (y + z * cl_options.z);
        blocks[bz][by][bx].average += data[idx];
    }
    
    for (auto &b : blockies) {
        b.average /= numBlocks;
    }


    delete[] data;
    
    return 0;
}