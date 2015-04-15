#ifndef block_h__
#define block_h__

#include <bd/util/transformable.h>

#include <glm/glm.hpp>

#include <string>
#include <cstdlib>

#ifndef BLOCK_DATA_FILENAME
#define BLOCK_DATA_FILENAME "block_data.txt"
#endif

class Block : public bd::Transformable
{
public:

    Block();

    Block(size_t bidx,
        const glm::u64vec3 &voxloc,
        const glm::u64vec3 &numvox,
        const glm::vec3 &worldMinCoords,
        const glm::vec3 &color = glm::vec3(1, 1, 1));

    virtual ~Block();

public:
    size_t bidx() const;
    void bidx(size_t idx);

    glm::u64vec3 loc() const;
    void loc(glm::u64vec3 l);

    glm::vec3 min() const;
    void min(glm::vec3 m);

    float avg() const;
    void avg(float a);

    bool empty() const;
    void empty(bool e);

    unsigned int texid() const;
    void texid(unsigned int id);

    glm::u64vec3 numVox() const;

    virtual std::string to_string() const override;

private:
    // block voxel coordinates
    glm::u64vec3 m_loc;

    // block world coordinates
    glm::vec3 m_min;
    glm::vec3 m_max;

    // block size voxels
    glm::u64vec3 m_numvox;

    // avg value of this block
    float m_avg;

    // empty flag (true --> not sent to gpu)
    bool m_empty;

    // resource id of the texture for this block.
    unsigned int m_texid;

    // block linear index
    size_t m_bidx;
};

#endif // !block_h__
