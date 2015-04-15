#include "blockscollection.h"
#include "block.h"
#include "volume.h"

#include <bd/util/util.h>
#include <bd/util/texture.h>
#include <bd/log/gl_log.h>

#include <GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <sstream>

const glm::mat4 identity { 1.0f };

BlocksCollection::BlocksCollection()
    : bd::BDObj()
{
}


///////////////////////////////////////////////////////////////////////////////
//BlocksCollection::BlocksCollection(std::unique_ptr<float[]> &data, glm::u64vec3 dataDim, 
//    glm::u64vec3 blockDim)
//    : m_blocks{ }
//    , m_block_dims_voxels{  }
//    , m_data{ std::move(data) }
//{
//}


///////////////////////////////////////////////////////////////////////////////
BlocksCollection::~BlocksCollection()
{
}


///////////////////////////////////////////////////////////////////////////////
// bs: number of blocks
// vol: volume voxel dimensions
void BlocksCollection::initBlocks(glm::u64vec3 bs, glm::u64vec3 vol)
{
    gl_log("Starting block init: Number of blocks: %dx%dx%d, "
        "Volume dimensions: %dx%dx%d", bs.x, bs.y, bs.z, vol.x, vol.y, vol.z);

    unsigned long long numblocks{ bd::vecCompMult(bs) };
    float dh{ 360.0f / numblocks }; // hue delta
    float hue{ 0.0f };

    // size of block in world coordinates
    // the blocks all cram into a volume <= 1x1x1 in world coordinate units.
    glm::vec3 block_world_size{ glm::vec3{ vol } / glm::vec3{ bs } };

    // Loop through block coordinates and populate block fields.
    for (auto bz = 0ul; bz < bs.z; ++bz)
    for (auto by = 0ul; by < bs.y; ++by)
    for (auto bx = 0ul; bx < bs.x; ++bx) {
        size_t bidx{ bd::to1D(bx, by, bz, bs.x, bs.y) };

        glm::u64vec3 blkLoc{ bx, by, bz };
        glm::u64vec3 voxLoc{ m_block_dims_voxels * blkLoc };
        glm::vec3 worldLoc{ block_world_size *glm::vec3{ blkLoc } };
        glm::vec3 center{ (worldLoc + (worldLoc + block_world_size)) / 2.0f };

        glm::vec3 col;
        bd::hsvToRgb(hue, 1.0f, 1.0f, col);
        hue += dh;

        Block blk(bidx, voxLoc, m_block_dims_voxels, worldLoc, col);

        blk.scale(block_world_size);
        blk.position(center);

        m_blocks.push_back(blk);
    }

    gl_log("Finished block init: total blocks is %d.", m_blocks.size());

}


///////////////////////////////////////////////////////////////////////////////
// bs: blocks per dimension in volume
// vol: dims of volume data
void BlocksCollection::avgblocks(glm::u64vec3 bs, glm::u64vec3 vol)
{
    //number of blocks in the volume along each dimension.
    /*{ m_vol->numBlocks() };
    glm::u64vec3 vol{ m_vol->numVox() };*/

    // Sum voxels within blocks
    // x,y,z are voxel coordinates.
    for (size_t z = 0; z < vol.z; ++z)
    for (size_t y = 0; y < vol.y; ++y)
    for (size_t x = 0; x < vol.x; ++x) {
        // voxel --> block coordinates
        size_t bx{ x / m_block_dims_voxels.x };
        size_t by{ y / m_block_dims_voxels.y };
        size_t bz{ z / m_block_dims_voxels.z };

        // linear block index
        size_t bidx{ bd::to1D(bx, by, bz, bs.x, bs.y) };

        // linear data index
        size_t idx{ bd::to1D(x, y, z, vol.x, vol.y) };

        // accumulate voxel value
        Block *blk{ &(m_blocks[bidx]) };
        if ((*m_data)[idx] > 0.5f)
            blk->avg(blk->avg() + 1);
    }

    size_t block_total_vox { bd::vecCompMult(m_block_dims_voxels)};
    size_t emptyCount{ 0 };

    // compute average
    for (auto &b : m_blocks) {
        float a{ b.avg() / block_total_vox };
        b.avg(a);
        if (a < 0.3f) {
            b.empty(true);
            emptyCount++;
        } else {
            b.empty(false);
        }
    }

    gl_log("Averaged %d blocks, decided %d are empty.", m_blocks.size(), emptyCount);
}


///////////////////////////////////////////////////////////////////////////////
// v: data dimensions of volume data
void BlocksCollection::createNonEmptyTextures(glm::u64vec3 v) 
{
    size_t szvox { bd::vecCompMult(m_block_dims_voxels) };
    float *image = new float[szvox];

    for (Block &b : m_blocks) {
        if (b.empty()) continue;

        size_t imgIdx = 0;
        for (auto z = b.loc().z; z < b.loc().z + m_block_dims_voxels.z; ++z)
        for (auto y = b.loc().y; y < b.loc().y + m_block_dims_voxels.y; ++y)
        for (auto x = b.loc().x; x < b.loc().x + m_block_dims_voxels.x; ++x) {
            size_t didx{ bd::to1D(x, y, z, v.x, v.y) };
            image[imgIdx++] = (*m_data)[didx];
        } // for for for

//        unsigned int t = bd::genGLTex3d(image,
//            bd::TextureFormat::OneChannel, bd::TextureFormat::OneChannel,
//            m_block_dims_voxels.x, m_block_dims_voxels.y, m_block_dims_voxels.z);

//        if (t == 0) {
//            gl_log_err("genGLTex3d returned 0 for the texture id.");
//        } else {
//            b.texid(t);
//        }

    } // for

    delete [] image;
};


///////////////////////////////////////////////////////////////////////////////
void BlocksCollection::data(std::shared_ptr<float[]> data)
{
    m_data = data;
}


///////////////////////////////////////////////////////////////////////////////
const std::vector<Block>& BlocksCollection::blocks() const
{
    return m_blocks;
}


///////////////////////////////////////////////////////////////////////////////
void BlocksCollection::printblocks() const
{
    std::stringstream ss;

    std::ofstream f{ BLOCK_DATA_FILENAME };
    if (f.is_open()) {
        for (auto &b : m_blocks) {
            ss << b.to_string() << '\n';
        }
        f << ss.str();
        f.flush();
        f.close();
    }
}


///////////////////////////////////////////////////////////////////////////////
std::string BlocksCollection::to_string() const
{

    return "{blocks: "+ std::to_string(m_blocks.size()) + "}" ;
}

//template
//unsigned long long
//BlocksCollection::findEmpties<NoEmpties>(NoEmpties func);
