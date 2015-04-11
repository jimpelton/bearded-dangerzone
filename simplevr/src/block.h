#ifndef block_h__
#define block_h__

#include <util/transformable.h>

#include <glm/glm.hpp>


#ifndef BLOCK_DATA_FILENAME
#define BLOCK_DATA_FILENAME "block_data.txt"
#endif

class Block : public bd::Transformable
{
public:
    // static void makeBlockSlices(unsigned int numslices);

public:

    Block()
        : m_bidx{ 0 }
        , m_loc{ 0, 0, 0 }
        , m_min{ 0.0f, 0.0f, 0.0f }
        , m_avg{ 0.0f }
        , m_empty{ false }
        , m_texid{ 0 }
    { }

    Block(size_t bidx, const glm::u64vec3 &voxloc, const glm::u64vec3 &numvox,
        const glm::vec3 &worldMinCoords, const glm::vec3 &color = glm::vec3(1, 1, 1)
        ) 
        : m_bidx{bidx}
        , m_loc{voxloc}
        , m_min{worldMinCoords}
        , m_numvox(numvox)
        , m_avg(0.0f)
        , m_empty(true)
        , m_texid(0)
    {
    }

public:
    int bidx() const{ return m_bidx;}
    void bidx(int idx){m_bidx = idx;}

    glm::u64vec3 loc() const{return m_loc;}
    void loc(glm::u64vec3 l){m_loc = l;}

    glm::vec3 min() const{return m_min;}
    void min(glm::vec3 m){m_min = m;}

    float avg() const{return m_avg;}
    void avg(float a){ m_avg = a; }

    bool empty() const{ return m_empty; }
    void empty(bool e){m_empty = e;}

    unsigned int texid() const{ return m_texid; }
    void texid(unsigned int id) { m_texid = id; }

    glm::u64vec3 numVox() const { return m_numvox; }

    std::string to_string() const;

    // block linear index
    size_t m_bidx;


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

};

#endif // !block_h__
