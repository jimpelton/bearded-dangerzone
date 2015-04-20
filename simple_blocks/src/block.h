#ifndef block_h__
#define block_h__

#include <bd/scene/transformable.h>

#include <glm/glm.hpp>

#include <string>
#include <cstdlib>


#ifndef BLOCK_DATA_FILENAME
#define BLOCK_DATA_FILENAME "block_data.txt"
#endif

// class bd::Transformable;

class Block : public bd::Transformable
{
public:

    Block();

    Block(const glm::u64vec3 &ijk, const glm::vec3 &origin);

    virtual ~Block();

    glm::vec3 origin() const;
    void origin(const glm::vec3 &);

    glm::u64vec3 ijk() const;
    void ijk(const glm::u64vec3 &ijk);

    virtual std::string to_string() const override;

private:
    glm::u64vec3 m_ijk;  ///< block's location in block coordinates
    glm::vec3 m_origin;  ///< block origin in world space

};

#endif // !block_h__
