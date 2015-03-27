#ifndef block_h__
#define block_h__

#include <geometry/quad.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

namespace bd=bearded::dangerzone;

class Block
{
public:
    /*! \brief populate blocks with their data
     * 
     * Assums square blocks with just one parameter for block dimensions.
     * 
     *  \param blocks The collectin of blocks
     *  \param data raw volume data
     *  \param vol_w Volume width (in voxels)
     *  \param vol_h Volume height (in voxels)
     *  \param vol_d Volume depth (in voxels)
     *  \param blk_s Block side (in voxels)
     * 
     */
    static void sumblocks(std::vector<Block> &blocks, const float *data, 
        int vol_w, int vol_h, int vol_d, int blk_s)
    {

        int bs_x = vol_w / blk_s;
        int bs_y = vol_h / blk_s;
        int bs_z = vol_d / blk_s;

        for (int z = 0; z < vol_d; ++z)
        for (int y = 0; y < vol_h; ++y)
        for (int x = 0; x < vol_w; ++x) 
        {
            int bx = x / blk_s;
            int by = y / blk_s;
            int bz = z / blk_s;

            size_t idx = x + vol_w * (y + z * vol_d);
            int bidx = bx + bs_y * (by + bz*bs_y);

            Block *blk = &(blocks[bidx]);
            blk->bidx = bidx;
            blk->dims = { blk_s, blk_s, blk_s };
            blk->loc = glm::ivec3{ x, y, z } - 7;
            //blk->min = { x, y, z };
            blk->avg += data[idx];
        }
    }

    /** \brief Loop over blocks to determine emptyness
     * 
     *  \param blocks The blocks
     *  \param blk_vox The number of voxels in a block.
     */
    static void avgblocks(std::vector<Block> &blocks, size_t blk_vox)
    {
        for (auto &b : blocks) {
            b.avg /= blk_vox;
            if (b.avg < 0.5f) {
                b.empty = true;
            }
        }
    }

    /** \brief 
     *
     */
    static void printblocks(std::vector<Block> &blocks)
    {
        std::ofstream f("outfile.txt");
        for (auto &b : blocks) {
            std::string blockstr(b.to_string());
            std::cout << blockstr << '\n';
            f << blockstr << "\n------------\n";
        }

        f.flush();
        f.close();
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



    std::string to_string()
    {
        return "Idx: " + std::to_string(bidx) + "\n"
            "Loc: " + glm::to_string(loc) + "\n"
            "Min: " + glm::to_string(min) + "\n"
            "Dims: " + glm::to_string(dims) + "\n"
            "Avg: " + std::to_string(avg) + "\n"
            "Empty: " + std::to_string(empty);
    }


private:
    // block linear index
    int bidx;
    // block voxel coordinates
    glm::ivec3 loc;
    // block world coordinates
    glm::vec3 min;
    // size of this block
    glm::ivec3 dims;
    // avg value of this block
    float avg;
    // empty flag (true --> not sent to gpu)
    bool empty;
    // the instance quad for this block
    bd::geometry::Quad quad;
};



#endif // block_h__