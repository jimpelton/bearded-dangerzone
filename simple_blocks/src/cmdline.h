#ifndef cmdline_h__
#define cmdline_h__

#include <string>

enum class FileDataType
{
  Ascii, Binary
};

enum class


struct CommandLineOptions {
  // raw file path
  std::string filePath;
  // transfer function file path
  std::string tfuncPath;
  // for .dat descriptor file (currently unimplemented)
  std::string datFilePath;
  // volume data type
  std::string type;
  // true if block data should be dumped to file
  bool printBlocks;
  // number of blocks X
  size_t numblk_x;
  // number of blocks Y
  size_t numblk_y;
  // number of blocks Z
  size_t numblk_z;
  // slices per block
  unsigned int num_slices;
  // vol width
  size_t vol_w;
  // vol height
  size_t vol_h;
  // vol depth
  size_t vol_d;
  // threshold max
  float tmax;
  // threshold minimum
  float tmin;
  // initial camera position
  unsigned int cameraPos;
  // path to output file for performance counter values
  std::string perfOutPath;
  // quit program when perf tests are completed.
  bool perfMode;
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Parses command line args and populates \c opts.
///
/// If non-zero arg was returned, then the parse was successful, but it does 
/// not mean that valid or all of the required args were provided on the 
/// command line.
///
/// \returns 0 on parse failure, non-zero if the parse was successful.
///////////////////////////////////////////////////////////////////////////////
int parseThem(int argc, const char *argv[], CommandLineOptions &opts);

void printThem(CommandLineOptions &);

#endif // cmdline_h__
