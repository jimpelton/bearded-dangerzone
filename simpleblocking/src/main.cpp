

#include "log/gl_log.h"
#include "file/datareader.h"

#include <boost/program_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <string>
#include <map>


struct Block 
{
    // block coordinates
    glm::vec3 loc;
    // block voxel coordinate (voxel in volume)
    glm::vec3 min;
    // size of this block
    glm::vec3 dims;
    // avg value of this block
    float avg;
    // empty flag (true --> not sent to gpu)
    bool empty;

    Block()
        : min(glm::vec3(0.0f))
        , dims(glm::vec3(0.0f))
        , avg(0.0f)
    { }

    std::string to_string() 
    {
        return "Min: " + glm::to_string(min) + "\n"
            "Dims: " + glm::to_string(dims) + "\n"
            "Avg: " + std::to_string(avg) + "\n"
            "Empty: " + std::to_string(empty);
    }
};

struct CommandLineOptions 
{
    // raw file path
    std::string filePath;
    // volume data type
    std::string type;
    // voxels per side of block
    int side;
    // vol width
    size_t w;
    // vol height
    size_t h; 
    // vol depth
    size_t d;
} opts;

enum class DataType 
{ 
    Integer, 
    UnsignedInteger, 

    Character,
    UnsignedCharacter, 

    Short, 
    UnsignedShort, 

    Float 
};

std::map<std::string, DataType> g_dataTypesMap
{
    { "int", DataType::Integer },
    { "unsigned integer", DataType::UnsignedInteger },

    { "char", DataType::Character },
    { "uchar", DataType::UnsignedCharacter },
    { "unsigned char", DataType::UnsignedCharacter },


    { "short", DataType::Short },
    { "ushort", DataType::UnsignedShort },
    { "unsigned short", DataType::UnsignedShort },

    { "float", DataType::Float }
};

struct DatFileDescription
{
    // raw file name
    std::string objectFileName;
    
    // resolution x, y, z
    unsigned long long resx;
    unsigned long long resy;
    unsigned long long resz;
    
    // thickness x, y, z
    float thx;
    float thy;
    float thz;

    DataType format;
};

// #include <boost/algorithm/string.hpp>
// 
// void readDatFile(const std::string &fpath) 
// {
//     std::ifstream dat(fpath, std::ifstream::in);
//     
//     for (std::string line; std::getline(dat, line);) {
//         std::string::size_type colpos = line.find(':');
//         if (colpos == std::string::npos) continue;
// 
//         std::vector<std::string> values;
//         namespace ba = boost::algorithm;
//         ba::split(values, line.substr(colpos), [](char c){ return c == ','; });
//         if (values.size() != 0) {
// 
//         }
// 
//     }
// 
// }


int parseThem(int argc, char *argv[])
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
        opts.side = m_vm["side"].as<int>();
        opts.w = m_vm["xdim"].as<int>();
        opts.h = m_vm["ydim"].as<int>();
        opts.d = m_vm["zdim"].as<int>();
//  }

    return static_cast<int>(m_vm.size());

} catch (boost::program_options::error &e) {
    std::cerr << e.what() << std::endl;
    return 0;
}


float* readData(const std::string &dtype, const std::string &fname, size_t x, size_t y, size_t z) 
{
    DataType t = g_dataTypesMap[dtype];
    float *rval = nullptr;
    switch (t) {
    case DataType::Float:
    {
        bearded::dangerzone::file::DataReader<float, float> reader;
        reader.loadRaw3d(opts.filePath, opts.w, opts.h, opts.d);
        rval = reader.takeOwnership();
        break;
    }
    default:
        break;
    }
    
    return rval;
}

void sumblocks(std::vector<Block> &blocks, const float *data) 
{
    //Block *b = (Block*)blocks.data();
    
    int bs = opts.w / opts.side;  // blocks per side of volume
    
    for (int z = 0; z < opts.d; ++z)
    for (int y = 0; y < opts.h; ++y)
    for (int x = 0; x < opts.w; ++x) {
        int bx = x / opts.side;
        int by = y / opts.side;
        int bz = z / opts.side;
        size_t idx = x + opts.w * (y + z * opts.d);
        Block *blk = &(blocks[bx + bs * (by + bz*bs)]);
        blk->loc = { bx, by, bz };
        blk->min = { x, y, z };
        blk->avg += data[idx];
    }
}

void printblocks(std::vector<Block> &blocks) 
{
    std::ofstream f("outfile.txt");
    for (auto &b : blocks) {
        std::string blockstr(b.to_string());
        std::cout << blockstr << '\n';
        f << blockstr << "\n------------";
    }

    f.flush();
    f.close();
}

int main(int argc, char *argv[]) 
try
{
    gl_log_restart();
    gl_debug_log_restart();

    if (parseThem(argc, argv) == 0) {
        std::cerr << "Error parsing arguments, exiting...\n";
        exit(1);
    }
    
    float* data = readData(opts.type, opts.filePath, 
        opts.w, opts.h, opts.d);

    size_t voxels = opts.w * opts.h * opts.d;
    size_t voxPerBlock = opts.side*opts.side*opts.side;
    size_t numBlocks = voxels / voxPerBlock;
    
    std::vector<Block> blocks = std::vector<Block>(numBlocks);
    
    sumblocks(blocks, data);
    
    for (auto &b : blocks) {
        b.avg /= voxPerBlock;
        if (b.avg < 0.5f) {
            b.empty = true;
        }
    }

    printblocks(blocks);

    delete[] data;
    
    return 0;
} catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    exit(1);
}