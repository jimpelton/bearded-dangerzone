

#ifndef blockscollection_h__
#define blockscollection_h__

#include "block.h"
#include "volume.h"

#include <vector>

class BlocksCollection {


public:
    BlocksCollection();
    BlocksCollection(Volume *);
    ~BlocksCollection();

    /** \brief populate blocks with their data */
    void initBlocks();
    
    /** \brief Loop over blocks to determine emptyness
     * 
     *  \param blocks The blocks
     *  \param blk_vox The number of voxels in a block.
     */
    void avgblocks(const float* data);

    /** \brief Print the output of Block::to_string() for each block to a file.
     *
     *  The file name used is BLOCK_DATA_FILENAME, no checks for write permissions made :( .
     *
     *  \param blocks vector of blocks to print.
     */
    void printblocks();

    const std::vector<Block>& blocks() const { return m_blocks; }

private:
    std::vector<Block> m_blocks;
    
    // size of a single block in voxels
    glm::u64vec3 m_block_dims_voxels;

    Volume *m_vol;

};

#endif

