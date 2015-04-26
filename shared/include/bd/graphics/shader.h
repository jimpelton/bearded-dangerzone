#ifndef shader_h__
#define shader_h__

#include <bd/util/texture.h>

#include <glm/glm.hpp>

#include <string>
#include <map>
#include <vector>

namespace bd {

enum class ShaderType
{
    Vertex,
    Fragment
};

class Shader
{
public:
    Shader(ShaderType t);

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

    ShaderType m_type;
    unsigned int m_id;
};


///////////////////////////////////////////////////////////////////////////////
// Class ShaderProgram
///////////////////////////////////////////////////////////////////////////////
class ShaderProgram
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Constructors/Destructor
    ////////////////////////////////////////////////////////////////////////////////
    ShaderProgram();

    ShaderProgram(Shader *vert, Shader *frag);

    virtual ~ShaderProgram();

    /** 
      * \brief Link frag and vert shaders if already provided (via constructor).
      * \return The non-zero gl identifier for the program, 0 on error.
      */
    unsigned int linkProgram();

    /** 
      * \brief Link provided frag and vertex shaders.
      * \return The non-zero gl identifier for the program, 0 on error.
      */
    unsigned int linkProgram(Shader *vert, Shader *frag);

    /**
      * \brief Open, build and link shaders in files at provided paths.
      * \return The non-zero gl identifier for the program, 0 on error.
      */
    unsigned int linkProgram(const std::string &vertPath, const std::string &fragPath);

    /** 
      * \brief Sets the shader uniform specified by \c param to \c val.
      * 
      * If \c param has not been added prior to calling setUniform, chaos ensues.
      */
    void setUniform(const std::string &param, glm::mat4 &val);
    void setUniform(const std::string &param, glm::vec4 &val);
    void setUniform(const std::string &param, glm::vec3 &val);
//    void setUniform(const std::string &param, const Texture &tex);

    /**  
      * \brief Get the id of the spec'd param.
      * \return The non-zero gl identifier, or 0 if param has never been set.
      */
    unsigned int getParamLocation(const std::string &param);

    void bind();
    void unbind();

    /** \brief True if both shaders have been built, false otherwise. */
    bool checkBuilt();

private:
    ///////////////////////////////////////////////////////////////////////////////
    // Private Members
    ///////////////////////////////////////////////////////////////////////////////
    
    /** \brief Map uniform name to location. */
    using ParamTable = std::map< std::string, unsigned int >;
    
    /** \brief Maps sampler location, Texture */
//    using TextureTable = std::map<unsigned int, const Texture* >;
    
    ////////////////////////////////////////////////////////////////////////////////
    // Member Data
    ////////////////////////////////////////////////////////////////////////////////
    //TODO: use std::unique_ptr to manage Shader*'s.
    Shader *m_vert;
    Shader *m_frag;
    unsigned int m_programId; ///< The opengl shader program id
    ParamTable m_params;  ///< Uniform locations
//    TextureTable m_textures;  ///< Texture sampler locatons

};

} // namespace bd

#endif /* shader_h__ */
