#include <bd/io/indexfile/indexfile.h>
#include <bd/filter/blockaveragefilter.h>
#include <bd/util/util.h>

#include <glm/glm.hpp>

#include <istream>
#include <ostream>


namespace bd
{
/*****************************************************************************
 * IndexFile                                                                 *
*****************************************************************************/


///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<IndexFile>
IndexFile::fromBinaryIndexFile(std::string const &path, bool &ok)
{
  std::unique_ptr<IndexFile> idxfile{ new IndexFile() };
  idxfile->m_fileName = path;

  if (path.empty()) {
    ok = false;
    return idxfile;
  }

  ok = idxfile->readBinaryIndexFile();
  if (! ok) {
    return idxfile;
  }

  ok = (idxfile->getHeader().version == VERSION);
  if (! ok) {
    Err() << "The index file provided is the wrong version! You should regenerate the "
      "index file.";
  }

  return idxfile;
}



///////////////////////////////////////////////////////////////////////////////
IndexFile::IndexFile()
  : m_header{ }
  , m_fileName{ }
  , m_fileBlocks()
{
}


///////////////////////////////////////////////////////////////////////////////
IndexFile::~IndexFile()
{
}


///////////////////////////////////////////////////////////////////////////////
const IndexFileHeader&
IndexFile::getHeader() const
{
  return m_header;
}


///////////////////////////////////////////////////////////////////////////////
void
IndexFile::writeBinaryIndexFile(std::ostream &os) const
{
  os.write(reinterpret_cast<const char *>(&m_header), sizeof(IndexFileHeader));
  os.write(reinterpret_cast<const char *>(&m_volume), sizeof(Volume));

  // read all the blocks
  for (auto &b : m_fileBlocks) {
    os.write(reinterpret_cast<const char *>(&b), sizeof(FileBlock));
  }
}


///////////////////////////////////////////////////////////////////////////////
void
IndexFile::writeBinaryIndexFile(std::string const &outpath) const
{
  std::ofstream os;
  os.open(outpath, std::ios::binary);
  if (!os.is_open()) {
    Err() << outpath << " could not be opened." << std::endl;
    return;
  }

  writeBinaryIndexFile(os);
  os.flush();
  os.close();
}


///////////////////////////////////////////////////////////////////////////////
void
IndexFile::writeAsciiIndexFile(std::ostream &os) const
{
  // os << "\"index\": {\n";
  //
  // Open outer JSON object
  os << "{\n";
  os << m_header << ",\n"
        << m_volume << ",\n";

  os << "\"blocks\": { \n";
  if (m_fileBlocks.size() > 0) {
    for (size_t i{ 0 }; i < m_fileBlocks.size() - 1; ++i) {
      os << m_fileBlocks[i] << ",\n";
    }
    os << m_fileBlocks[m_fileBlocks.size() - 1] << "\n";
  }

  os << "}}\n";
}


///////////////////////////////////////////////////////////////////////////////
void
IndexFile::writeAsciiIndexFile(std::string const &outpath) const
{
  std::ofstream os;
  os.open(outpath);
  if (!os.is_open()) {
    Err() << outpath << " could not be opened!";
    return;
  }

  writeAsciiIndexFile(os);

  os.flush();
  os.close();
}


///////////////////////////////////////////////////////////////////////////////
std::vector<FileBlock> const&
IndexFile::getFileBlocks() const
{
  return m_fileBlocks;
}


///////////////////////////////////////////////////////////////////////////////
std::vector<FileBlock>&
IndexFile::getFileBlocks()
{
  return m_fileBlocks;
}


///////////////////////////////////////////////////////////////////////////////
bd::Volume const&
IndexFile::getVolume() const
{
  return m_volume;
}


///////////////////////////////////////////////////////////////////////////////
bd::Volume&
IndexFile::getVolume()
{
  return m_volume;
}

void IndexFile::setVolume(bd::Volume const& v)
{
  m_volume = v;
}


  ///////////////////////////////////////////////////////////////////////////////
std::string const &
IndexFile::getPath()
{
  return m_fileName;
}


///////////////////////////////////////////////////////////////////////////////
std::string
IndexFile::getRawFileName()
{
  return m_header.raw_file;
}


///////////////////////////////////////////////////////////////////////////////
std::string
IndexFile::getTFFileName()
{
  return m_header.tf_file;
}


///////////////////////////////////////////////////////////////////////////////
void
IndexFile::setRawFileName(std::string const &f)
{
  size_t lenght = f.size() > 256 ? 256 : f.size();
  for (int i = 0; i < lenght; ++i){
    m_header.raw_file[i] = f[i];
  }
}


///////////////////////////////////////////////////////////////////////////////
void
IndexFile::setTFFileName(std::string const &f)
{
  int flen{ static_cast<int>(f.size()) };
  int lenght = flen > 256 ? 256 : flen;
  for (int i = 0; i < lenght; ++i){
    m_header.tf_file[i] = f[i];
  }
}

///////////////////////////////////////////////////////////////////////////////
bool
IndexFile::readBinaryIndexFile()
{
  // open index file (binary)
  std::ifstream is{ m_fileName, std::ios::binary };
  if (!is.is_open()) {
    Err() << "The file " << m_fileName << " could not be opened.";
    return false;
  }

  is.seekg(0, std::ios::beg);

  // read header
  IndexFileHeader ifh;
  is.read(reinterpret_cast<char *>(&ifh), sizeof(IndexFileHeader));
  m_header = ifh;

  // read volume
  Volume v;
  is.read(reinterpret_cast<char *>(&v), sizeof(Volume));
  m_volume = v;

  // Read all the things!
  FileBlock fb;
  for (size_t i = 0; i < m_volume.total_block_count(); ++i) {
    is.read(reinterpret_cast<char *>(&fb), sizeof(FileBlock));
    m_fileBlocks.push_back(fb);
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////
void
IndexFile::init(bd::DataType type)
{
  size_t tySize{ bd::to_sizeType(type) };

  // bc: number of blocks
  glm::u64vec3 bc{ m_volume.block_count() };

  // vd: volume voxel dimensions
  glm::u64vec3 vd{ m_volume.voxelDims() };

  // bd: block dimensions
  glm::u64vec3 bd{ m_volume.block_dims() };

  glm::vec3 wd{ m_volume.worldDims() / glm::vec3(bc) };

  Dbg() << "Starting FileBlock creation: "
    " # blocks: "
    << bc.x << ", " << bc.y << ", " << bc.z <<
    " Vol dim: "
    << vd.x << ", " << vd.y << ", " << vd.z <<
    " Block dim: "
    << wd.x << ", " << wd.y << ", " << wd.z;


  // Loop through all our blocks (identified by <bxi,byj,bzk>) and populate block fields.
  for (size_t bzk = 0; bzk < bc.z; ++bzk) {
    for (size_t byj = 0; byj < bc.y; ++byj) {
      for (size_t bxi = 0; bxi < bc.x; ++bxi) {
        // i,j,k block identifier
        glm::u64vec3 const blkId{ bxi, byj, bzk };

        glm::vec3 const worldLoc{ wd * glm::vec3(blkId) - 0.5f };

        // block center in world coordinates
        glm::vec3 const blkOrigin{ (worldLoc + (worldLoc + wd)) * 0.5f };

        // voxel start of block within volume 
        // (ijk index times the voxel dimensions of each block)
        glm::u64vec3 const startVoxel{ blkId * bd };

        FileBlock blk;
        blk.block_index = bd::to1D(bxi, byj, bzk, bc.x, bc.y);
        blk.ijk_index[0] = bxi;
        blk.ijk_index[1] = byj;
        blk.ijk_index[2] = bzk;
        blk.data_offset = tySize *
          bd::to1D(startVoxel.x, startVoxel.y, startVoxel.z, vd.x, vd.y);

        blk.data_bytes = tySize * bd.x * bd.y * bd.z;
          
        blk.voxel_dims[0] = static_cast<decltype(blk.voxel_dims[0])>(bd.x);
        blk.voxel_dims[1] = static_cast<decltype(blk.voxel_dims[1])>(bd.y);
        blk.voxel_dims[2] = static_cast<decltype(blk.voxel_dims[2])>(bd.z);

        blk.world_oigin[0] = blkOrigin.x;
        blk.world_oigin[1] = blkOrigin.y;
        blk.world_oigin[2] = blkOrigin.z;

        blk.world_dims[0] = wd.x;
        blk.world_dims[1] = wd.y;
        blk.world_dims[2] = wd.z;

        m_fileBlocks.push_back(blk);
      }
    }
  }
  
  m_fileBlocks.shrink_to_fit();

  initHeader(type);

  Dbg() << "Total blocks is: " << m_fileBlocks.size();
}


///////////////////////////////////////////////////////////////////////////////
void
IndexFile::initHeader(DataType dt)
{
  m_header.magic_number = MAGIC;
  m_header.version = VERSION;
  m_header.header_length = HEAD_LEN;
  m_header.dataType = IndexFileHeader::getTypeInt(dt);

  //  m_header.numblocks[0] = m_volume.block_count().x;
  //  m_header.numblocks[1] = m_volume.block_count().y;
  //  m_header.numblocks[2] = m_volume.block_count().z;
  //
  //  m_header.dataType = IndexFileHeader::getTypeInt(dt);
  //
  //  m_header.volume_extent[0] = m_volume.voxelDims().x;
  //  m_header.volume_extent[1] = m_volume.voxelDims().y;
  //  m_header.volume_extent[2] = m_volume.voxelDims().z;
  //
  //  m_header.volume_world_dims[0] = m_volume.worldDims().x;
  //  m_header.volume_world_dims[1] = m_volume.worldDims().y;
  //  m_header.volume_world_dims[2] = m_volume.worldDims().z;
  //
  //  glm::u64vec3 blkExt = m_volume.blocksExtent();
  //  m_header.blocks_extent[0] = blkExt.x;
  //  m_header.blocks_extent[1] = blkExt.y;
  //  m_header.blocks_extent[2] = blkExt.z;
  //
  //  m_header.vol_empty_voxels = m_volume.numEmptyVoxels();
  //  m_header.vol_avg = m_volume.avg();
  //  m_header.vol_max = m_volume.max();
  //  m_header.vol_min = m_volume.min();
  //  m_header.vol_total = m_volume.total();
}


///////////////////////////////////////////////////////////////////////////////
// FileBlock operator<<
std::ostream&
operator<<(std::ostream &os, bd::FileBlock const &block)
{
  return os << block.to_string();
}
} // namepsace bd
