#ifndef block_h__
#define block_h__

#include "texture.h"

#include <bd/scene/transformable.h>

#include <glm/fwd.hpp>

#include <string>
#include <vector>

class ShaderProgram;

class Block : public bd::Transformable
{
public:

    /// \brief Set/get this block's dimensions in voxels
    static void blockDims(const glm::u64vec3 &dims);
    static glm::u64vec3 blockDims();


    /// \brief Set/get the volume's dimensions in voxels
    static glm::u64vec3 volDims();
    static void volDims(const glm::u64vec3 &voldims);
    //TODO: move volDims() out of block class (yeah...probably make a volume abstr).


    /////////////////////////////////////////////////////////////////////////////////
    /// \brief Initializes \c nb blocks so that they fit within the extent of \c vd.
    /// \param nb[in]      Number of blocks in x,y,z directions.
    /// \param vd[in]      Volume dimensions
    /// \param blocks[out] Vector that new blocks are pushed onto.
    ///////////////////////////////////////////////////////////////////////////////
    static void initBlocks(glm::u64vec3 nb, glm::u64vec3 vd, std::vector<Block> &blocks);


    /////////////////////////////////////////////////////////////////////////////////
    /// \brief Marks blocks as empty and uploads GL textures if average is outside of [tmin..tmax].
    /// \param data[in] Volume data set
    /// \param blocks[in][out] Set of blocks to filter
    /// \param nonempty_blocks[out] Pointers to the non-empty blocks arye pushed onto this here vector.
    /// \param numBlks[in] Number of blocks in \c blocks parameter in x,y,z dirs.
    /// \param volsz[in] Dimensions of volume data set in \c data parameter.
    /// \param tmin[in] min average block value to filter against.
    /// \param tmax[in] max average block value to filter against.
    /// \param sampler[in] The sampler location of the block shader.
    ///////////////////////////////////////////////////////////////////////////////
    //TODO: filterblocks takes Functor.
    static void filterBlocks(float *data, 
        std::vector<Block> &blocks, std::vector<Block*> &nonempty_blocks,
        unsigned int sampler, float tmin = 0.0f, float tmax = 1.0f);


public:
    ///////////////////////////////////////////////////////////////////////////////
    //    Instance members
    ///////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Create block (i,j,k) with specified dims and world origin.
    /// \param[in] ijk The block ID (and position with in the grid of blocks).
    /// \param[in] dims The dimensions in data points of this block.
    /// \param[in] origin Origin of this block in world coords (or whatever coords you want...maybe...sigh...whatevs).
    ///////////////////////////////////////////////////////////////////////////////
    Block(const glm::u64vec3 &ijk, const glm::vec3 &dims, const glm::vec3 &origin);

    virtual ~Block();


    /// \brief Set/get the ijk location of this block.
    glm::u64vec3 ijk() const;
    void ijk(const glm::u64vec3 &ijk);


    /// \brief Set/get if this block is marked empty.
    void empty(bool);
    bool empty() const;


    /// \brief Set/get the average value of this here block.
    void avg(float);
    float avg() const;
    

    /// \brief Get the texture assoc'd with this block.
    Texture& texture();

//    void draw();


    /// \brief String rep. of this blockeroo.
    virtual std::string to_string() const override;

private:
    /////////////////////////////////////////////////////////////////////////////////
    /// \brief Fills \c out_blockData with part of \c in_data corresponding to block (i,j,k).
    /// \param ijk[in]     ijk coords of the block whos data to get.
    /// \param bsz[in]     The size of the block data.
    /// \param volsz[in]   The size of the volume data s.t. 
    ///                    volsz.x*volsz.y*volsz.z == length(in_data).
    /// \param in_data[in] Source data
    /// \param out_blockData[out] Destination space for data.
    ///////////////////////////////////////////////////////////////////////////////
    static void fillBlockData(glm::u64vec3 ijk, const float *in_data, float *out_blockData);

    static glm::u64vec3 m_blockDims; ///< Dimensions of a block in something.
    static glm::u64vec3 m_volDims;   ///< Volume dimensions (# data points).
    static glm::u64vec3 m_numBlocks; ///< Number of blocks volume is divided into.


    glm::u64vec3 m_ijk;  ///< Block's location in block coordinates.
    bool m_empty;        ///< True if this block was determined empty.
    float m_avg;         ///< Avg. val. of this block.
    Texture m_tex;       ///< Texture data assoc'd with this block.
};

std::ostream& operator<<(std::ostream &os, const Block &b);

#endif // !block_h__
