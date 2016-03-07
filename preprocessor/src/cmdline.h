#ifndef preproc_cmdline_h__
#define preproc_cmdline_h__

#include <string>

struct CommandLineOptions {
  // raw file path
  std::string filePath;
  // output file path
  std::string outFilePath;
  // transfer function file path
  std::string tfuncPath;
  // for .dat descriptor file (currently unimplemented)
  std::string datFilePath;
  // volume data type
  std::string type;
  // output file type
  std::string outputFileType;
  // Action to perform (write/read index file)
  std::string actionType;
  // true if block data should be dumped to file
  bool printBlocks;
  // number of blocks X
  size_t numblk_x;
  // number of blocks Y
  size_t numblk_y;
  // number of blocks Z
  size_t numblk_z;
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
int 
parseThem(int argc, const char *argv[], CommandLineOptions &opts);

void 
printThem(const CommandLineOptions &);

std::ostream& 
operator<<(std::ostream&, const CommandLineOptions &);

#endif // preproc_cmdline_h__
