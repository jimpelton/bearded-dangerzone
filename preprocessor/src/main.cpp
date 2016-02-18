////////////////////////////////////////////////////////////////////////////////
// Preprocessor
// Generates index files for simple_blocks viewer.
// Index file format
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <bd/util/util.h>
#include <vector>

struct BlockCollection {





  static glm::u64vec3 g_numBlocks;  ///< Number of blocks in volume;
  static std::vector<glm::u64vec3> g_nonEmptyBlockIndexes; ///< Vector of the non-empty blocks.

};

template<typename Ty>
class BlockBuffer
{
public:
  BlockBuffer() : m_data{ nullptr }, m_blockId{ } { }

  /// \brief Convert 3D i,j,k coords into 1D index.
  size_t
  blockNumber()
  {
    return m_blockId.x +
        BlockCollection::g_numBlocks.x *
            (m_blockId.y + BlockCollection::g_numBlocks.y * m_blockId.z);
  }

  size_t
  numElements()
  {
    return m_dims.x * m_dims.y * m_dims.z;
  }


  Ty *m_data;              ///< Ptr to this buffer data.

  glm::u64vec3 m_dims;     ///< Size of this BlockBuffer in elements along each axis.
  glm::u64vec3 m_blockId;  ///< i,j,k index of this BlockBuffer.

};


////////////////////////////////////////////////////////////////////////////////
/// \brief Read volume data from \c data and copy into given \c BlockBuffer.

////////////////////////////////////////////////////////////////////////////////
template<typename Ty>
void
readBlock(const glm::u64vec3 & blockId, std::ifstream &inFile, BlockBuffer<Ty> & dest)
{
  const size_t voxels{ };
  const glm::u64vec3 dims{ dest.m_dims };
  const size_t rowSize{ dims.x };

  Ty *buf{ dest.m_data };

  inFile.seekg(dest.blockNumber() * voxels * sizeof(Ty));

  for(size_t z{ 0 }; z < dims.z; ++z) {
    for(size_t y{ 0 }; y<dims.y; ++y) {
      inFile.read(reinterpret_cast<char*>(buf), rowSize * sizeof(Ty));
      buf += rowSize;
    }
    inFile.seekg( ,inFile.cur);
  }



}


template<typename Ty>
void
saveBlock(std::ostream &out, BlockBuffer<Ty> &block)
{

  size_t voxels{ block.numElements() };
  Ty *buf{ block.m_data };

  out.write(reinterpret_cast<char*>(buf), voxels * sizeof(Ty));
  out.flush();

}


int main(int argc, char *argv[]) 
{
	return 0;
}