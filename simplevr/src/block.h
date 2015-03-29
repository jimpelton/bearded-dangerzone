#ifndef block_h__
#define block_h__

#include <log/gl_log.h>
#include <util/util.h>
#include <geometry/quad.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>



#ifndef BLOCK_DATA_FILENAME
#define BLOCK_DATA_FILENAME "block_data.txt"
#endif



class Block
{
public:
    /*! \brief populate blocks with their data
     * 
     * Assums square blocks with just one parameter for block dimensions.
     * 
     *  \param blocks The collectin of blocks

     *  \param vol_w Volume width (in voxels)
     *  \param vol_h Volume height (in voxels)
     *  \param vol_d Volume depth (in voxels)
     *  \param blk_s Block side (in voxels)
     * 
     */
    static void initBlocks
        (
        std::vector<Block> &blocks, 
        int vol_x, int vol_y, int vol_z, 
        int blk_x, int blk_y, int blk_z
        );

    /** \brief Loop over blocks to determine emptyness
     * 
     *  \param blocks The blocks
     *  \param blk_vox The number of voxels in a block.
     */
    static void avgblocks
    (
        std::vector<Block> &blocks,
        const float* data,
        int vol_x, int vol_y, int vol_z,
        int blk_x, int blk_y, int blk_z,
        size_t blk_vox
    );

    /** \brief Print the output of Block::to_string() for each block to a file.
     *
     *  The file name used is BLOCK_DATA_FILENAME, no checks for write permissions made :( .
     *
     *  \param blocks vector of blocks to print.
     */
    static void printblocks(std::vector<Block> &blocks)
    {
        std::stringstream ss;

        for (auto &b : blocks) {
            ss << b.to_string() << '\n';
        }

        std::string peep(ss.str());
        std::ofstream f(BLOCK_DATA_FILENAME);
        
        if (f.is_open()) {
            f << peep;
            f.flush();
            f.close();
        }

        //std::cout << peep << std::endl;
    }

public:

    Block()
        : bidx(0)
        , loc(glm::ivec3(0))
        , min(glm::vec3(0.0f))
        , dims(glm::ivec3(0))
        , avg(0.0f)
        , empty(false)
    { }
 
    bd::geometry::Quad& quad() { return m_quad; }

    std::string to_string() const
    {
        std::stringstream ss;
        ss << "Idx: " << bidx << "\n"
            "Loc: " << glm::to_string(loc) << "\n"
            "Min: " << glm::to_string(min) << "\n"
            "Dims: " << glm::to_string(dims) << "\n"
            "Avg: " << avg << "\n"
            "Empty: " << empty << " (" << avg << ")";
        return ss.str();
    }

    bool isEmpty() const { return empty; }

private:
    // block linear index
    int bidx;
    // block voxel coordinates
    glm::vec3 loc;
    // block world coordinates
    glm::vec3 min;
    // size of this block
    glm::vec3 dims;
    // avg value of this block
    float avg;
    // empty flag (true --> not sent to gpu)
    bool empty;
    // the instance quad for this block
    bd::geometry::Quad m_quad;
};



#endif // block_h__