#ifndef volume_h__
#define volume_h__

#include <glm/glm.hpp>

class Volume {
public:
    Volume(const glm::vec3 &dims_world, const glm::u64vec3 &dims_vox,
           const glm::uvec3 &dims_blocks)
        : m_dims_world { dims_world }
        , m_numVox { dims_vox }
        , m_numBlocks{ dims_blocks } 
    {
    
    }

    ~Volume() { }

    /** \brief dimsneions in world coords. */
    glm::vec3 dims() const {
        return m_dims_world;
    }

    /** \brief dimensions in voxels */
    glm::u64vec3 numVox() const {
        return m_numVox;
    }

    /** \brief dimensions in blocks */
    glm::u64vec3 numBlocks() const {
        return m_numBlocks;
    }
    /** \brief total blocks in this volume */
    size_t totalBlocks() {
        return m_numBlocks.x * m_numBlocks.y * m_numBlocks.z;
    }

private:
    /* \brief dimensions of this volume in world coords (floating point). */
    glm::vec3 m_dims_world;
    /* \brief dimensions of this volume in voxels. */
    glm::u64vec3 m_numVox;
    /* \brief dimensions of this volume in blocks. */
    glm::uvec3 m_numBlocks;
};



#endif // volume_h__