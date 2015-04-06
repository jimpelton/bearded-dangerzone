#ifndef shader_h__
#define shader_h__

#include "texture.h"


#include <glm/glm.hpp>

#include <string>
#include <map>
#include <vector>

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

    bool isBuilt() const { return id() != 0; }

private:
    unsigned int compileShader(const char *shader);

private:
    ShaderType m_type;
    unsigned int m_id;
};

class ShaderProgram
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Constructors/Destructor
    ////////////////////////////////////////////////////////////////////////////////
    ShaderProgram();
    ShaderProgram(const Shader *vert, const Shader *frag);
    virtual ~ShaderProgram();

public:
    /** 
      * \brief Link frag and vert shaders if already provided (via constructor).
      * \return The non-zero gl identifier for the program, 0 on error.
      */
    unsigned int linkProgram();

    /** 
      * \brief Link provided frag and vertex shaders.
      * \return The non-zero gl identifier for the program, 0 on error.
      */
    unsigned int linkProgram(const Shader *vert, const Shader *frag);

    unsigned int addParam(const std::string &param);

    /** 
      * \brief Sets the shader uniform specified by \c param to \c val.
      * 
      * If \c param has not been added prior to calling setParam, chaos ensues.
      */
    void setParam(const std::string &param, glm::mat4 &val);
    void setParam(const std::string &param, glm::vec4 &val);
    void setParam(const std::string &param, glm::vec3 &val);

    /**  
      * \brief Get the id of the spec'd param.
      * \return The non-zero gl identifier, or 0 if param has never been set.
      */
    unsigned int getParamLocation(const std::string &param);

    void bind();
    void unbind();

private:
    
    bool checkBuilt();

    using ParamTable = std::map< std::string, unsigned int >;
    using TextureTable = std::map<unsigned int, Texture >;
    
    ////////////////////////////////////////////////////////////////////////////////
    // Member Data
    ////////////////////////////////////////////////////////////////////////////////
    const Shader *m_vert;
    const Shader *m_frag;
    unsigned int m_location;
    ParamTable m_params;
    TextureTable m_textures;

};

} // namespace bd

#endif /* shader_h__ */
