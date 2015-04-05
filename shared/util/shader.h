#ifndef shader_h__
#define shader_h__

#include <GL/glew.h>

#include <vector>
#include <string>

namespace bd {
; // <-- stop vs from indenting

enum class ShaderType
{
    Vertex,
    Fragment
};

class Shader
{
public:
    Shader(ShaderType t);

public:
    /** \brief Compile the shader in given file.
     *  \param A string containing the path to the file containing the shader.
     *  \return The non-zero gl identifier of the compiled shader, 0 on error.
     */
    unsigned int loadFromFile(const std::string &filepath);
    
    /** \brief Compile the shader in given string.
     *  \param A string containing the path to the file containing the shader.
     *  \return The non-zero gl identifier of the compiled shader, 0 on error.
     */
    unsigned int loadFromString(const std::string &shaderString);

    unsigned int id() const { return m_id; }

private:
    unsigned int compileShader(const char *shader);

private:
    ShaderType m_type;
    unsigned int m_id;
};

class ShaderProgram
{
public:
    ShaderProgram();
    ShaderProgram(const Shader *vert, const Shader *frag);

public:
    /** \brief Link provided frag and vertex shaders.
     *  \return The non-zero gl identifier for the program, 0 on error.
     */
    unsigned int linkProgram();
    unsigned int linkProgram(const Shader *vert, const Shader *frag);


private:
    const Shader *m_vert;
    const Shader *m_frag;
};


} // namespace bd

#endif /* shader_h__ */
