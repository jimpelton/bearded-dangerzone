#ifndef block_h__
#define block_h__

#include "texture.h"

#include <bd/scene/transformable.h>
#include <bd/graphics/drawable.h>

#include <glm/glm.hpp>

#include <string>
#include <fstream>

class Block : public bd::Transformable
{
public:

    Block(const glm::u64vec3 &ijk, const glm::vec3 &dims, const glm::vec3 &origin);

    virtual ~Block();

    glm::u64vec3 ijk() const;
    void ijk(const glm::u64vec3 &ijk);

    bool empty() const;
    void empty(bool);

    float avg() const;
    void avg(float);

    Texture& texture();

//    void draw();

    virtual std::string to_string() const override;


private:
    glm::u64vec3 m_ijk;  ///< Block's location in block coordinates
    bool m_empty;        ///< True if this block was determined empty.
    float m_avg;

    Texture m_tex;

};

std::ostream& operator<<(std::ostream &os, const Block &b);

#endif // !block_h__
