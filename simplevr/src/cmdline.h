#ifndef cmdline_h__
#define cmdline_h__

#include <string>

struct CommandLineOptions
{
    // raw file path
    std::string filePath;
    // volume data type
    std::string type;
    // voxels per side of block
    int block_side;
    // slices per block
    unsigned int num_slices;
    // vol width
    size_t w;
    // vol height
    size_t h;
    // vol depth
    size_t d;
};

int parseThem(int argc, const char *argv [], CommandLineOptions &opts);

#endif // cmdline_h__
