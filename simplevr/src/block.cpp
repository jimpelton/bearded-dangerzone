

#include "block.h"


namespace bd = bearded::dangerzone;


void Block::initBlocks
(
    std::vector<Block> &blocks, 
    
    int vol_x, 
    int vol_y, 
    int vol_z, 

    int blk_x, 
    int blk_y, 
    int blk_z
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

        Block *blk = &(blocks[bidx]);
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

        blk->m_quad.translate(
            glm::translate(glm::mat4(1.0f),
            (blk->min + (blk->min + block_world_size)) / 2.0f));

        blk->m_quad.scale(glm::scale(glm::mat4(1.0f), block_world_size));

        bd::util::hsvToRgb(hue, 1.0f, 1.0f, blk->m_quad.color());
        hue += dh;
    }
}

void Block::avgblocks
(
    std::vector<Block> &blocks,
    const float* data,
    
    int vol_x, 
    int vol_y, 
    int vol_z,

    int blk_x, 
    int blk_y, 
    int blk_z,
    
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

void Block::printblocks(std::vector<Block> &blocks)
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
}

std::string Block::to_string() const
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
