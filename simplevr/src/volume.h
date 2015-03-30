#ifndef volume_h__
#define volume_h__

#include <glm/glm.hpp>

class Volume {
public:
    Volume(const glm::vec3 &dims_world, const glm::u64vec3 &dims_vox, const glm::uvec3 &dims_blocks) 
        : m_dims_world { dims_world }
        , m_dims_vox { dims_vox }
        , m_dims_blocks{ dims_blocks } 
    { }

    ~Volume() { }

    /* \brief dimsneions in world coords. */
    glm::vec3 dims() const { return m_dims_world; }
    
    /* \brief dimensions in voxels */
    glm::u64vec3 numVox() const { return m_dims_vox; }
    
    /* \brief dimensions in blocks */
    glm::u64vec3 numBlocks() const { return m_dims_blocks; }

private:
    /* \brief dimensions in world coords (floating point). */
    glm::vec3 m_dims_world;
    /* \brief dimensions in voxels */
    glm::u64vec3 m_dims_vox;
    /* \brief dimensions in blocks */
    glm::uvec3 m_dims_blocks;
};



#endif // volume_h__