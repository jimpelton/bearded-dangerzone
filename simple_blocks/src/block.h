#ifndef block_h__
#define block_h__

#include <bd/scene/transformable.h>
#include <bd/graphics/drawable.h>

#include <glm/glm.hpp>

#include <string>

#ifndef BLOCK_DATA_FILENAME
#define BLOCK_DATA_FILENAME "block_data.txt"
#endif


class Block : public bd::Transformable
{
public:

    Block(const glm::u64vec3 &ijk, const glm::vec3 &dims, const glm::vec3 &origin);

    virtual ~Block();

    glm::u64vec3 ijk() const;
    void ijk(const glm::u64vec3 &ijk);

    bool empty() const;
    void empty(bool);

    virtual std::string to_string() const override;

private:
    glm::u64vec3 m_ijk;  ///< Block's location in block coordinates
    bool m_empty;        ///< True if this block was determined empty.

};

#endif // !block_h__
