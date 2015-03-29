
#include "blockscollection.h"

namespace bd=bearded::dangerzone;

BlocksCollection::BlocksCollection() 
    : BlocksCollection(nullptr) 
{ }

BlocksCollection::BlocksCollection(Volume *vol)
    : m_blocks{ }
    , m_block_dims_voxels{vol->voxDims() / vol->blockDims()}
    , m_vol{ vol }
{ }

BlocksCollection::~BlocksCollection() { }

const glm::mat4 identity{1.0f};

void BlocksCollection::initBlocks()
{
    // compute number of blocks in the volume for each dimension.
    // (used to get linear block index later)
    glm::u64vec3 bs { m_vol->blockDims() };
    /*int bs_x {m_vol->blockDims().x};
    int bs_y {m_vol->blockDims().y};
    int bs_z {m_vol->blockDims().z};*/
    
    unsigned long long numblocks { bs.x * bs.y * bs.z };
    float dh { 360.0f / numblocks }; // hue delta
    float hue{ 0.0f };

    // size of block in world coordinates
    // the blocks all cram into a volume <= 1x1x1 in world coordinate units.
    glm::vec3 block_world_size { m_vol->worldDims() / glm::vec3{ bs } };

    // Loop through block coordinates and populate block fields.
    for (size_t bz = 0; bz < bs.z; ++bz)
    for (size_t by = 0; by < bs.y; ++by)
    for (size_t bx = 0; bx < bs.x; ++bx)
    {
        size_t bidx { bx + bs.x * (by + bz*bs.z) };

        Block blk;
        blk.bidx = bidx;

        glm::u64vec3 blkCoords { bx,by,bz };
        // block location in voxel coordinates
        blk.loc = m_block_dims_voxels * blkCoords; // { bx*blk_x, by*blk_y, bz*blk_z };

        // convert block coordinates to world coordinates
        blk.min = block_world_size * glm::vec3{ blkCoords } - 0.5f; 
        /*{bx * block_world_size.x, by * block_world_size.y, bz * block_world_size.z };*/

        // offset this block
        //blk->min -= 0.5f;

        blk.m_quad.translate(
            glm::translate(identity,
            (blk.min + (blk.min + block_world_size)) / 2.0f));

        blk.m_quad.scale(glm::scale(identity, block_world_size));

        bd::util::hsvToRgb(hue, 1.0f, 1.0f, blk.m_quad.color());
        hue += dh;

        m_blocks.push_back( blk );
    }
}


void BlocksCollection::avgblocks
(
    const float* data
)
{
    //number of blocks in the volume along each dimension.
    glm::u64vec3 bs { m_vol->blockDims() };
    
    glm::u64vec3 vol { m_vol->voxDims() };
    //number of voxels in the volume along each dimension.
    /*unsigned long long vol_x { m_vol->voxDims().x };
    unsigned long long vol_y { m_vol->voxDims().y };
    unsigned long long vol_z { m_vol->voxDims().z };*/
    
    // Sum voxels within blocks
    // x,y,z are voxel coordinates.
    for (size_t z = 0; z < vol.z; ++z)
    for (size_t y = 0; y < vol.y; ++y)
    for (size_t x = 0; x < vol.x; ++x)
    {
        // voxel --> block coordinates
        size_t bx { x / m_block_dims_voxels.x };
        size_t by { y / m_block_dims_voxels.y };
        size_t bz { z / m_block_dims_voxels.z };
        
        // linear block index
        size_t bidx { bx + bs.x * (by + bz * bs.z) };
        
        // linear voxel index
        size_t idx { x + vol.x * (y + z * vol.z) };
        
        // accumulate voxel value
        Block *blk { &(m_blocks[bidx]) };

        blk->avg += data[idx];
    }

    unsigned long long block_total_vox { 
        m_block_dims_voxels.x * m_block_dims_voxels.y * m_block_dims_voxels.z };

    // compute average
    for (auto &b : m_blocks) {
        b.avg /= block_total_vox;
        if (b.avg < 0.5f) {
            b.empty = true;
        }
    }
}

void BlocksCollection::printblocks()
{
    std::stringstream ss;

    for (auto &b : m_blocks) {
        ss << b.to_string() << '\n';
    }

    std::string peep { ss.str() };
    std::ofstream f  { BLOCK_DATA_FILENAME };

    if (f.is_open()) {
        f << peep;
        f.flush();
        f.close();
    }
}