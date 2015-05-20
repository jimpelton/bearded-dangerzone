

#include "block.h"
//#include "texture.h"

#include <bd/log/gl_log.h>
#include <bd/scene/transformable.h>
#include <bd/util/util.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.inl>

#include <sstream>


glm::u64vec3 Block::m_blockDims{ 0, 0, 0 };
glm::u64vec3 Block::m_volDims{ 0, 0, 0 };
glm::u64vec3 Block::m_numBlocks{ 0, 0, 0 };



///////////////////////////////////////////////////////////////////////////////
glm::u64vec3 Block::blockDims()
{
    return m_blockDims;
}


///////////////////////////////////////////////////////////////////////////////
void Block::blockDims(const glm::u64vec3 &dims)
{
    m_blockDims = dims;
}


///////////////////////////////////////////////////////////////////////////////
glm::u64vec3 Block::volDims()
{
    return m_volDims;
}


///////////////////////////////////////////////////////////////////////////////
void Block::volDims(const glm::u64vec3 &voldims)
{
    m_volDims = voldims;
}


///////////////////////////////////////////////////////////////////////////////
// nb: number of blocks
// vd: volume voxel dimensions 
void Block::initBlocks(glm::u64vec3 nb, glm::u64vec3 vd, std::vector<Block> &blocks)
{
    m_blockDims = vd / nb;
    m_volDims = vd;
    m_numBlocks = nb;

    // block world dims
    glm::vec3 wld_dims{ 1.0f / glm::vec3(nb) };
    
    gl_log("Starting block init: Number of blocks: %dx%dx%d, "
        "Volume dimensions: %dx%dx%d Block dimensions: %.2f,%.2f,%.2f",
        nb.x, nb.y, nb.z,
        vd.x, vd.y, vd.z,
        wld_dims.x, wld_dims.y, wld_dims.z);

    // Loop through all our blockx (identified by <bx,by,bz>)and populate block fields.
    for (auto bz = 0ul; bz < nb.z; ++bz)
    for (auto by = 0ul; by < nb.y; ++by)
    for (auto bx = 0ul; bx < nb.x; ++bx) {

        // i,j,k block identifier
        glm::u64vec3 blkId{ bx, by, bz };
        // lower left corner in world coordinates
        glm::vec3 worldLoc{ (wld_dims * glm::vec3(blkId)) - 0.5f }; // - 0.5f;
        // origin in world coordiates
        glm::vec3 blk_origin{ (worldLoc + (worldLoc + wld_dims)) * 0.5f };

        Block blk{ glm::u64vec3(bx, by, bz), wld_dims, blk_origin };
        blocks.push_back(blk);
    }

    gl_log("Finished block init: total blocks is %d.", blocks.size());
}


///////////////////////////////////////////////////////////////////////////////
void Block::fillBlockData(glm::u64vec3 ijk, const float *in_data, float *out_blockData)
{
    size_t imageIdx = 0;
    glm::u64vec3 bst{ ijk * m_blockDims };  // block start = block index * block size
    glm::u64vec3 end{ bst + m_blockDims };
    for (auto k = bst.z; k < end.z; ++k)
    for (auto j = bst.y; j < end.y; ++j)
    for (auto i = bst.x; i < end.x; ++i) {
        size_t dataIdx{ bd::to1D(i, j, k, m_volDims.x, m_volDims.y) };
        float val = in_data[dataIdx];
        out_blockData[imageIdx++] = val;
    } 
}


///////////////////////////////////////////////////////////////////////////////
void Block::filterBlocks (float *data, 
    std::vector<Block> &blocks, std::vector<Block*> &nonempty_blocks, 
    unsigned int sampler, float tmin, float tmax)
{
    gl_log("Filtering blocks for empty space and creating GL textures.");
    size_t blkPoints = bd::vecCompMult(m_blockDims);

    float *image = new float[blkPoints];

    for (auto &b : blocks) {

        glm::vec3 ijk = b.ijk();
        fillBlockData(ijk, data, image);
        float avg{ 0.0f };
        for (size_t i = 0; i < blkPoints; ++i) {
            avg += image[i];
        }
        avg /= blkPoints;
        b.avg(avg);

        if (avg < tmin || avg > tmax) {
            b.empty(true);
        } else {
            b.texture().samplerLocation(sampler);
//            b.texture().textureUnit(0);
            b.texture().genGLTex3d(image, Texture::Format::RED, Texture::Format::RED,
                m_blockDims.x, m_blockDims.y, m_blockDims.z);

            if (b.texture().id() == 0) {
                gl_log_err("failed to allocate a gl texture, for block (%d,%d,%d).",
                    ijk.x, ijk.y, ijk.z);
            }

            nonempty_blocks.push_back(&b);
        }
    } // for auto

    // TODO: create list of pointers to non-empty blocks.
    delete [] image;
    gl_log("%d/%d blocks marked empty.", blocks.size()-nonempty_blocks.size(), blocks.size());

}


///////////////////////////////////////////////////////////////////////////////
Block::Block(const glm::u64vec3 &ijk, const glm::vec3 &dims, const glm::vec3 &origin)
    : m_ijk{ ijk }
    , m_empty{ false }
    , m_avg{ 0.0f }
    , m_tex{  }
{
    transform().scale(dims);
    transform().origin(origin);
    update();
}


///////////////////////////////////////////////////////////////////////////////
Block::~Block()
{
}

//void Block::draw()
//{
//
//}

///////////////////////////////////////////////////////////////////////////////
glm::u64vec3 Block::ijk() const
{
    return m_ijk;
}


///////////////////////////////////////////////////////////////////////////////
void Block::ijk(const glm::u64vec3 &ijk)
{
    m_ijk = ijk;
}


///////////////////////////////////////////////////////////////////////////////
bool Block::empty() const
{
    return m_empty;
}


///////////////////////////////////////////////////////////////////////////////
void Block::empty(bool b)
{
    m_empty = b;
}


///////////////////////////////////////////////////////////////////////////////
float Block::avg() const
{
    return m_avg;
}


///////////////////////////////////////////////////////////////////////////////
void Block::avg(float a)
{
    m_avg = a;
}

Texture& Block::texture()
{
    return m_tex;
}


///////////////////////////////////////////////////////////////////////////////
std::string Block::to_string() const
{
    std::stringstream ss;
    ss <<  "{ ijk: (" << m_ijk.x << ',' << m_ijk.y << ',' << m_ijk.z << ")\n"
        "Origin: " << m_transform.origin().x << ',' << m_transform.origin().y << ',' <<
        m_transform.origin().z <<
        "Empty: " << (m_empty ? "True\n" : "False\n") << "\n"
        "Texture: " << m_tex << " }";

    return ss.str();
}

std::ostream& operator<<(std::ostream &os, const Block &b)
{
    return os << b.to_string();
}

