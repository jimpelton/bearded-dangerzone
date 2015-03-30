#ifndef block_h__
#define block_h__

#include <log/gl_log.h>
#include <util/util.h>
#include <geometry/quad.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#ifndef BLOCK_DATA_FILENAME
#define BLOCK_DATA_FILENAME "block_data.txt"
#endif

class Block
{
    friend class BlocksCollection;

public:

    Block()
        : bidx{ -1 }
        , loc{ 0, 0, 0 }
        , min{ 0.0f, 0.0f, 0.0f }
        , avg{ 0.0f }
        , empty{ false }
    { }

    const bd::geometry::Quad& quad() const { return m_quad; }

    std::string to_string() const;

    bool isEmpty() const { return empty; }
    unsigned int texid() const { return m_texid; }

private:
    // block linear index
    int bidx;

    // block voxel coordinates
    glm::u64vec3 loc;

    // block world coordinates
    glm::vec3 min;

    // avg value of this block
    float avg;

    // empty flag (true --> not sent to gpu)
    bool empty;

    // resource id of the texture for this block.
    unsigned int m_texid;

    // the instance quad geometry for this block
    bd::geometry::Quad m_quad;
};

#endif // block_h__