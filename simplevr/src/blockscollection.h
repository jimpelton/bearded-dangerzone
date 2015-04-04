

#ifndef blockscollection_h__
#define blockscollection_h__

#include "block.h"
#include "volume.h"

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

class BlocksCollection {
public:
    /** \brief Default constructor 
      * Create a blockscollection associted with given volume.
      */
    BlocksCollection();

    BlocksCollection(std::unique_ptr<float[]> &data, Volume *vol);
    ~BlocksCollection();

    ///////////////////////////////////////////////////////////////////////////
    // Member functions
    ///////////////////////////////////////////////////////////////////////////

    /** \brief Create the blocks for this BlocksCollection. 
     *  
     *  \c initBlocks() initializes blocks with block coords, voxel coords, etc.
     */
    void initBlocks();


    template< typename EmptyHeuristic >
    unsigned long long findEmpties(EmptyHeuristic isempty);


    /** \brief Loop over blocks to determine emptyness.
     * 
     *  \param data Pointer to raw volume data.
     */
    void avgblocks();

    /** \brief Generate gl textures for each block that is not empty.
     *  
     *  \param data Pointer to raw volume data.
     */
    void createNonEmptyTextures();

    /** \brief Print the output of Block::to_string() for each block to a file.
     *
     *  The file name used is BLOCK_DATA_FILENAME, no checks for write permissions made :( .
     *
     *  \param blocks vector of blocks to print.
     */
    void printblocks();

    ///////////////////////////////////////////////////////////////////////////
    // Accessors
    ///////////////////////////////////////////////////////////////////////////

    const std::vector<Block>& blocks() const { return m_blocks; }

    /** \brief Return the volume associated with this blockcollection. */
    const Volume& volume() const { return *m_vol; }

private:
    std::vector<Block> m_blocks;
    
    // size of a single block in voxels
    glm::u64vec3 m_block_dims_voxels;

    Volume *m_vol;

    std::unique_ptr<float[]> m_data;

};

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

