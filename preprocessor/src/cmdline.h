#ifndef preproc_cmdline_h__
#define preproc_cmdline_h__

#include <string>

enum class ActionType {
  READ, WRITE
};

enum class OutputType {
  Ascii, Binary
};

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
  OutputType outputFileType;
  // Action to perform (write/read index file)
  ActionType actionType;
  // true if block data should be dumped to file
  bool printBlocks;
  // number of blocks X
  unsigned long long numblk_x;
  // number of blocks Y
  unsigned long long numblk_y;
  // number of blocks Z
  unsigned long long numblk_z;
  // vol width
  unsigned long long vol_w;
  // vol height
  unsigned long long vol_h;
  // vol depth
  unsigned long long vol_d;
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
