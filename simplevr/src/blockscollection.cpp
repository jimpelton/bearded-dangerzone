#include "blockscollection.h"
#include "block.h"
#include "volume.h"

#include <util/texture.h>
#include <log/gl_log.h>

#include <GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <sstream>
#include <util/util.h>

const glm::mat4 identity {
    1.0f
};

BlocksCollection::BlocksCollection()
{
}

//BlocksCollection::BlocksCollection(std::unique_ptr<float[]> &data, glm::u64vec3 dataDim, 
//    glm::u64vec3 blockDim)
//    : m_blocks{ }
//    , m_block_dims_voxels{  }
//    , m_data{ std::move(data) }
//{
//}

BlocksCollection::~BlocksCollection()
{
}

// bs: number of blocks
// vol: volume voxel dimensions
void BlocksCollection::initBlocks(glm::u64vec3 bs, glm::u64vec3 vol)
{
    // number of blocks in the volume for each dimension.
    // (used to get linear block index later)
    //glm::u64vec3 bs{ m_vol->numBlocks() };

    unsigned long long numblocks{ bd::vecCompMult(bs) };
    float dh{ 360.0f / numblocks }; // hue delta
    float hue{ 0.0f };

    // size of block in world coordinates
    // the blocks all cram into a volume <= 1x1x1 in world coordinate units.
    glm::vec3 block_world_size{ glm::vec3{ vol } / glm::vec3{ bs } };

    // Loop through block coordinates and populate block fields.
    for (auto bz = 0; bz < bs.z; ++bz)
    for (auto by = 0; by < bs.y; ++by)
    for (auto bx = 0; bx < bs.x; ++bx) {
        size_t bidx{ bd::to1D(bx, by, bz, bs.x, bs.y) };

        glm::u64vec3 blkLoc{ bx, by, bz };
        glm::u64vec3 voxLoc{ m_block_dims_voxels * blkLoc };
        glm::vec3 worldLoc{ block_world_size *glm::vec3{ blkLoc } };
        glm::vec3 center{ (worldLoc + (worldLoc + block_world_size)) / 2.0f };

        glm::vec3 col;
        bd::hsvToRgb(hue, 1.0f, 1.0f, col);
        hue += dh;

        Block blk(bidx, voxLoc, m_block_dims_voxels, worldLoc, col);
        blk.transform().scale(block_world_size);
        blk.transform().position(center);
        m_blocks.push_back(blk);
    }

}


// bs: blocks per dimension in volume
// vol: data points in volume
void BlocksCollection::avgblocks(glm::u64vec3 bs, glm::u64vec3 vol)
{
    //number of blocks in the volume along each dimension.
    /*{ m_vol->numBlocks() };
    glm::u64vec3 vol{ m_vol->numVox() };*/

    // Sum voxels within blocks
    // x,y,z are voxel coordinates.
    for (auto z = 0; z < vol.z; ++z)
    for (auto y = 0; y < vol.y; ++y)
    for (auto x = 0; x < vol.x; ++x) {
        // voxel --> block coordinates
        unsigned long long bx{ x / m_block_dims_voxels.x };
        unsigned long long by{ y / m_block_dims_voxels.y };
        unsigned long long bz{ z / m_block_dims_voxels.z };

        // linear block index
        unsigned long long bidx{ bd::to1D(bx, by, bz, bs.x, bs.y) };

        // linear voxel index
        unsigned long long idx{ bd::to1D(x, y, z, vol.x, vol.y) };

        // accumulate voxel value
        Block *blk{ &(m_blocks[bidx]) };
        if (m_data[idx] > 0.5f)
            blk->avg(blk->avg() + 1);
    }

    unsigned long long block_total_vox {
        m_block_dims_voxels.x *m_block_dims_voxels.y *m_block_dims_voxels.z
    };

    unsigned long long emptyCount{ 0 };
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

// v: data dimensions of volume data
void BlocksCollection::createNonEmptyTextures(glm::u64vec3 v) 
{
    auto szvox =
        m_block_dims_voxels.z *
        m_block_dims_voxels.y *
        m_block_dims_voxels.z;

    //std::vector<float> image(szvox, 0.0f);
    float *image = new float[szvox];

    //glm::u64vec3 v{ m_vol->numVox() };
    for (Block &b : m_blocks) {
        if (b.empty()) continue;

        size_t imgIdx = 0;
        for (auto z = b.loc().z; z < b.loc().z + m_block_dims_voxels.z; ++z)
        for (auto y = b.loc().y; y < b.loc().y + m_block_dims_voxels.y; ++y)
        for (auto x = b.loc().x; x < b.loc().x + m_block_dims_voxels.x; ++x) {
            size_t didx{ bd::to1D(x, y, z, v.x, v.y) };
            image[imgIdx++] = m_data[didx];
        } // for for for

      /*  unsigned int t = bd::genGLTex3d(image,
            bd::TextureFormat::OneChannel, bd::TextureFormat::OneChannel,
            m_block_dims_voxels.x, m_block_dims_voxels.y, m_block_dims_voxels.z);*/

   /*     if (t == 0) {
            gl_log_err("genGLTex3d returned 0 for the texture id.");
        } else {
            b.texid(t);
        }*/

    } // for auto

    delete [] image;
}

void BlocksCollection::printblocks() 
{
    std::stringstream ss;

    for (auto &b : m_blocks) {
        ss << b.to_string() << '\n';
    }

    std::string peep{ ss.str() };
    std::ofstream f{ BLOCK_DATA_FILENAME };

    if (f.is_open()) {
        f << peep;
        f.flush();
        f.close();
    }
}

const Block& BlocksCollection::getBlock(size_t idx) const
{
    return m_blocks[idx];
}

//template
//unsigned long long
//BlocksCollection::findEmpties<NoEmpties>(NoEmpties func);
