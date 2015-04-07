#ifndef volume_h__
#define volume_h__

#include "blockscollection.h"
#include <util/util.h>

#include <glm/glm.hpp>


class Volume 
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // Constructors/Desctructor
    ///////////////////////////////////////////////////////////////////////////////

    //Volume()
    //    : Volume({ 0.0f, 0.0f, 0.0f }, { 0, 0, 0 }, { 0, 0, 0 })
    //{
    //}


    //Volume(const glm::vec3 &dims_world, const glm::u64vec3 &dims_vox,
    //       const glm::uvec3 &dims_blocks)
    //    : m_dims_world { dims_world }
    //    , m_numVox { dims_vox }
    //    , m_numBlocks{ dims_blocks }
    //{
    //}

    /** \brief Constructs a volume with specified voxel dimensions. 
      *  The world dimensions of the volume are set as dims / max(dims).
      *  
      *  
      */
    Volume(const glm::u64vec3 &dims)
        : m_numVox{ dims }
    {
        auto largest = std::max({ dims.x, dims.y, dims.z });
        if (largest != 0) {
            m_dims_world = 
                glm::vec3{ dims } / static_cast<float>(largest);
        }
    }

    ~Volume()
    {
    }

    
    /** \brief dimsneions in world coords. */
    glm::vec3 worldDims() const { return m_dims_world; }


    /** \brief dimensions in voxels */
    glm::u64vec3 numVox() const { return m_numVox; }
    

    /** \brief dimensions in blocks */
    //glm::u64vec3 numBlocks() const { return m_numBlocks; }

    /** \brief total voxels in this volume */
    unsigned long long totalVox() { return bd::vecCompMult(m_numVox); }

    /** \brief total blocks in this volume */
    //size_t totalBlocks() { return compMult(m_numBlocks); }

private:
    ///////////////////////////////////////////////////////////////////////////////
    // Data members
    ///////////////////////////////////////////////////////////////////////////////

    /* \brief dimensions of this volume in world coords (floating point). */
    glm::vec3 m_dims_world;
    /* \brief dimensions of this volume in voxels. */
    glm::u64vec3 m_numVox;

    /* \brief dimensions of this volume in blocks. */
    //glm::uvec3 m_numBlocks;

    BlocksCollection m_collection;

};



#endif // volume_h__