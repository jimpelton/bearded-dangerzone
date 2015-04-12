#ifndef blockscollection_h__
#define blockscollection_h__

// #include "block.h"

#include <bd/util/transformable.h>

#include <glm/glm.hpp>

#include <vector>
#include <memory>

//class NoEmpties
//{
//public:
//    bool operator()(const Block &b)
//    {
//        return false;
//    }
//};
//
//class AverageFill
//{
//public:
//    bool operator()(const Block &b)
//    {
//        //for(auto z=b.start)
//        
//    }
//};

class Volume;
class Block;

class BlocksCollection : public bd::Transformable {
public:

    BlocksCollection();

    BlocksCollection(std::unique_ptr<float []> &data, const Volume &v);

    ~BlocksCollection();


    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create the blocks for this BlocksCollection. 
    /// 
    /// \c initBlocks() initializes blocks with block coords, voxel coords, etc.
    ///////////////////////////////////////////////////////////////////////////
    void initBlocks(glm::u64vec3 bs, glm::u64vec3 vol);


    ///////////////////////////////////////////////////////////////////////////
    ///  \brief Loop over blocks to determine emptyness.
    /// 
    ///  \param data Pointer to raw volume data.
    ///////////////////////////////////////////////////////////////////////////
    void avgblocks(glm::u64vec3 bs, glm::u64vec3 vol);


    ///////////////////////////////////////////////////////////////////////////
    ///  \brief Generate gl textures for each block that is not empty.
    ///  
    ///  \param data Pointer to raw volume data.
    ///////////////////////////////////////////////////////////////////////////
    void createNonEmptyTextures(glm::u64vec3 v);


    ///////////////////////////////////////////////////////////////////////////
    ///  \brief Print the output of Block::to_string() for each block to a file.
    /// 
    ///  The file name used is BLOCK_DATA_FILENAME, no checks for write permissions made :( .
    /// 
    /// \param blocks vector of blocks to print.
    ///////////////////////////////////////////////////////////////////////////
    void printblocks();


    const Block& getBlock(size_t idx) const;

private:
    ///////////////////////////////////////////////////////////////////////////////
    // Data members
    ///////////////////////////////////////////////////////////////////////////////

    std::vector<Block> m_blocks;
    
    glm::u64vec3 m_block_dims_voxels;  ///< Size of a single block in voxels.

    std::unique_ptr<float[]> m_data;   ///< a pointer to the data used to generate block textures.

};

//TODO: EmptyHeuristic templates
//template <typename EmptyHeuristic>
//unsigned long long BlocksCollection::findEmpties(EmptyHeuristic isempty)
//{
//    unsigned long long empties = 0;
//    
//    for (const auto &b : m_blocks) {
//        const_cast<Block&>(b).empty(isempty(m_data, b));
//        empties++;
//    }
//
//    return empties;
//}

#endif

