#ifndef blockscollection_h__
#define blockscollection_h__


#include <bd/util/transformable.h>
#include <bd/util/bdobj.h>

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

//class Volume;
class Block;

class BlocksCollection : public bd::BDObj
{
public:

    BlocksCollection();

    ~BlocksCollection();


    //////////////////////////////////////////////////////////////////////////
    /// \brief Get a const reference to the block at index \c idx.
    //////////////////////////////////////////////////////////////////////////
//    const Block& operator[](std::size_t idx) const;


    //////////////////////////////////////////////////////////////////////////
    /// \brief Add a block to this block collection.
    ///
    /// The block is added to this collection's list of transformable children
    //////////////////////////////////////////////////////////////////////////
//    void addBlock(Block &b);


    ///////////////////////////////////////////////////////////////////////////
    /// \brief Create the blocks for this BlocksCollection. 
    /// Given a number of blocks along x, y and z axis, creates Block objects
    /// with their dimensions and stuffs.
    ///
    /// \param bs Number of blocks in volume.
    /// \param vol Dimensions of volume data.
    ///////////////////////////////////////////////////////////////////////////
    void initBlocks(glm::u64vec3 bs, glm::u64vec3 vol);


    ///////////////////////////////////////////////////////////////////////////
    ///  \brief Loop over blocks to determine emptyness.
    /// 
    ///  \param bs Number of blocks in volume.
    ///  \param vol Dimensions of volume data.
    ///////////////////////////////////////////////////////////////////////////
    void avgblocks(glm::u64vec3 bs, glm::u64vec3 vol);


    ///////////////////////////////////////////////////////////////////////////
    ///  \brief Generate gl textures for each block that is not empty.
    ///  
    ///  \param data Pointer to raw volume data.
    ///////////////////////////////////////////////////////////////////////////
    void createNonEmptyTextures(glm::u64vec3 v);


    //////////////////////////////////////////////////////////////////////////
    /// \brief Set the pointer to data to be used in block processing.
    //////////////////////////////////////////////////////////////////////////
    void data(std::shared_ptr<float[]> data);


    const std::vector<Block>& blocks() const;


    ///////////////////////////////////////////////////////////////////////////
    ///  \brief Print the output of Block::to_string() for each block to a file.
    /// 
    ///  The file name used is BLOCK_DATA_FILENAME, no checks for write permissions made :( .
    /// 
    ///  \param blocks vector of blocks to print.
    ///////////////////////////////////////////////////////////////////////////
    void printblocks() const;

    virtual std::string to_string() const override;


private:
    ///////////////////////////////////////////////////////////////////////////////
    // Data members
    ///////////////////////////////////////////////////////////////////////////////

    std::vector<Block> m_blocks;
    
    glm::u64vec3 m_block_dims_voxels;  ///< Size of a single block in voxels.

    std::shared_ptr<float[]> m_data;   ///< a pointer to the data used to generate block textures.

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

