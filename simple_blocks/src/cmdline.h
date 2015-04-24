#ifndef cmdline_h__
#define cmdline_h__

#include <string>

struct CommandLineOptions
{
    // raw file path
    std::string filePath;
    // volume data type
    std::string type;
    // number of blocks X
    size_t numblk_x;
    // number of blocks Y
    size_t numblk_y;
    // number of blocks Z
    size_t numblk_z;
    // slices per block
    unsigned int num_slices;
    // vol width
    size_t w;
    // vol height
    size_t h;
    // vol depth
    size_t d;
    // threshold max
    float tmax;
    // threshold minimum
    float tmin;
};

int parseThem(int argc, const char *argv [], CommandLineOptions &opts);

void printThem(const CommandLineOptions &cmd);

#endif // cmdline_h__
