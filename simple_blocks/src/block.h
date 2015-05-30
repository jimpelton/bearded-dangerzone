#ifndef block_h__
#define block_h__

#include "texture.h"

#include <bd/scene/transformable.h>

#include <glm/fwd.hpp>

#include <string>
#include <vector>

#include <algorithm>

class ShaderProgram;


class Block : public bd::Transformable
{
public:


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


    /// \brief String rep. of this blockeroo.
    virtual std::string to_string() const override;

private:
    glm::u64vec3 m_ijk;  ///< Block's location in block coordinates.
    bool m_empty;        ///< True if this block was determined empty.
    float m_avg;         ///< Avg. val. of this block.
    Texture m_tex;       ///< Texture data assoc'd with this block.
};

std::ostream& operator<<(std::ostream &os, const Block &b);

#endif // !block_h__
