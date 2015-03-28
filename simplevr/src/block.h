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

namespace bd = bearded::dangerzone;

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
        )
    {
        // compute number of blocks in the volume for each dimension.
        // (used to get linear block index later)
        int bs_x = vol_x / blk_x;  
        int bs_y = vol_y / blk_y;  
        int bs_z = vol_z / blk_z;  
        int numblocks = blocks.size();
        float dh = 360.0f / numblocks;
        float hue = 0.0f;
        
        // size of block in world coordinates
        // the blocks all cram into a 1x1x1 volume.
        glm::vec3 block_world_size = 
            glm::vec3(1.0f, 1.0f, 1.0f) / glm::vec3(bs_x, bs_y, bs_z);
        
        // Loop through block coordinates and populate block fields.
        for (int bz = 0; bz < bs_z; ++bz)
        for (int by = 0; by < bs_y; ++by)
        for (int bx = 0; bx < bs_x; ++bx) 
        {
            int bidx = bx + bs_x * (by + bz*bs_z);

            Block *blk = &( blocks[bidx] );
            blk->bidx = bidx;

            // block dimensions in voxels.
            blk->dims = { blk_x, blk_y, blk_z };
            // block location in voxel coordinates
            blk->loc = { bx*blk_x, by*blk_y, bz*blk_z };
            
            // convert block coordinates to world coordinates
            blk->min = { 
                bx * block_world_size.x,
                by * block_world_size.y,
                bz * block_world_size.z
            };

            // offset this block
            blk->min -= 0.5f;

            blk->m_quad.model(
                glm::translate(glm::mat4(1.0f), (blk->min + (blk->min + block_world_size)) / 2.0f) *
                    glm::scale(glm::mat4(1.0f), block_world_size)
                );

            bd::util::hsvToRgb(hue, 1.0f, 1.0f, blk->m_quad.color());
            hue += dh;
        }

        
    }

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
    )
    {

        // compute number of blocks in the volume for each dimension.
        // (used to get linear block index later)
        int bs_x = vol_x / blk_x;
        int bs_y = vol_y / blk_y;
        int bs_z = vol_z / blk_z;

        // Sum voxels within blocks
        // x,y,z are voxel coordinates.
        for (int z = 0; z < vol_z; ++z)
        for (int y = 0; y < vol_y; ++y)
        for (int x = 0; x < vol_x; ++x)
        {
            // voxel --> block coordinates
            int bx = x / blk_x;
            int by = y / blk_y;
            int bz = z / blk_z;
            // linear block index
            int bidx = bx + bs_x * (by + bz * bs_z);
            // linear voxel index
            size_t idx = x + vol_x * (y + z * vol_z);
            // accumulate voxel value
            Block *blk = &(blocks[bidx]);
            blk->avg += data[idx];
        }

        // compute average
        for (auto &b : blocks) {
            b.avg /= blk_vox;
            if (b.avg < 0.5f) {
                b.empty = true;
            }
        }
    }

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
 
    bd::geometry::Quad& quad() {
        return m_quad;
    }

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