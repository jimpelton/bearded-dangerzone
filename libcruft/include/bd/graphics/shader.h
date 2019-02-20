#ifndef shader_h__
#define shader_h__

#include <bd/graphics/texture.h>
#include <bd/util/bdobj.h>

#include <glm/fwd.hpp>

#include <string>
#include <map>
#include <vector>

namespace bd
{
enum class ShaderType
{
  Vertex,
  Fragment
};

//////////////////////////////////////////////////////////////////////////
/// \brief A static class with convenience method(s) to compile a shader
/// without having to use the Shader class.
//////////////////////////////////////////////////////////////////////////
class Shader;

class Compiler
{
private:
  Compiler()
  {
  }

public:
  ~Compiler()
  {
  }


  //////////////////////////////////////////////////////////////////////////
  /// \brief Create and compile a shader of \c ty type.
  //////////////////////////////////////////////////////////////////////////
  //TODO: Move compile(Shader&, const char*) to Shader class.
  static bool compile(Shader& shader, const char* code);


  //////////////////////////////////////////////////////////////////////////
  /// \brief Validate a shader program.
  /// \note Returns true if glGetProgramiv returns true when called with
  ///       GL_VALIDATE_STATUS.
  /// \return true if program will run with current OpenGL state.
  //////////////////////////////////////////////////////////////////////////
  //TODO: Move validateProgram(uint) to ShaderProgram class.
  //  static bool validateProgram(unsigned int id);
};

class Shader : public BDObj
{
public:
  Shader(ShaderType t, const std::string& name = "no-name");
  ~Shader();

  unsigned int create();

  //////////////////////////////////////////////////////////////////////////
  /// \brief Compile the shader in given io.
  /// \param A string containing the path to the io containing the shader.
  /// \return The non-zero gl identifier of the compiled shader, 0 on error.
  //////////////////////////////////////////////////////////////////////////
  void loadFromFile(const std::string& filepath);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Compile the shader in given string.
  ///  \param A string containing the path to the io containing the shader.
  ///  \return The non-zero gl identifier of the compiled shader, 0 on error.
  ///////////////////////////////////////////////////////////////////////////////
  void loadFromString(const std::string& shaderString);

  //////////////////////////////////////////////////////////////////////////
  /// \brief Get the type of this Shader.
  //////////////////////////////////////////////////////////////////////////
  ShaderType type() const;

  //////////////////////////////////////////////////////////////////////////
  /// \brief Get a string representation of this shader's type
  /// returns either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
  //////////////////////////////////////////////////////////////////////////
  const char* typeString() const;

  //////////////////////////////////////////////////////////////////////////
  /// \brief Get the GL id of this Shader.
  /// \note id() returns zero if this shader has not been compiled yet.
  //////////////////////////////////////////////////////////////////////////
  unsigned int
  id() const
  {
    return m_id;
  }

  //////////////////////////////////////////////////////////////////////////
  /// \brief True if shader has been compiled, false otherwise.
  //////////////////////////////////////////////////////////////////////////
  bool
  isBuilt() const
  {
    // if the shader does not successfully build, then m_id is kept at 0.
    return id() != 0;
  }

  virtual std::string to_string() const override;

private:
  //  unsigned int compileShader(const char *shader);

  ShaderType m_type; ///< bd type of this Shader
  unsigned int m_id; ///< OpenGL id of this shader.
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

  ShaderProgram(const ShaderProgram&) = default;

  ShaderProgram(Shader* vert, Shader* frag);

  virtual ~ShaderProgram();


  //////////////////////////////////////////////////////////////////////////
  /// \brief Add the Shader in the io at \c shaderPath as a stage in
  /// this program.
  /// \returns I can't remember.
  //////////////////////////////////////////////////////////////////////////
  //  unsigned int addStage(const std::string &shaderPath, bd::ShaderType);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Link frag and vert shaders if already provided (via constructor).
  /// \return The non-zero gl identifier for the program, 0 on error.
  ///////////////////////////////////////////////////////////////////////////////
  unsigned int linkProgram();

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Link provided frag and vertex shaders.
  /// \return The non-zero gl identifier for the program, 0 on error.
  ///////////////////////////////////////////////////////////////////////////////
  unsigned int linkProgram(Shader* vert, Shader* frag);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Open, build and link shaders in files at provided paths.
  /// \return The non-zero gl identifier for the program, 0 on error.
  ///////////////////////////////////////////////////////////////////////////////
  unsigned int linkProgram(const std::string& vertPath,
                           const std::string& fragPath);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Sets the shader uniform specified by \c param to \c val.
  ///////////////////////////////////////////////////////////////////////////////
  void setUniform(const char* param, const glm::mat4& val);

  void setUniform(const char* param, const glm::vec4& val);

  void setUniform(const char* param, const glm::vec3& val);

  void setUniform(const char* param, const glm::vec2& val);

  void setUniform(const char* param, glm::f32 val);
  //    void setUniform(const std::string &param, const Texture &tex);

  void setUniform(const char* param, glm::i32 val);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Get the id of the spec'd param.
  /// \return The non-zero gl identifier, or 0 if param has never been set.
  ///////////////////////////////////////////////////////////////////////////////
  unsigned int getUniformLocation(const char* param);

  void bind();

  void unbind();

  /// \brief Get the GL id of this ShaderProgram.
  unsigned int programId() const;

  bool validateProgram();

private:
  ///////////////////////////////////////////////////////////////////////////////
  // Private Members
  ///////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  /// \brief Add the given Shader object as a stage in this program.
  /// \returns I don't know.
  //////////////////////////////////////////////////////////////////////////
  unsigned int addStage(Shader*);


  /// \brief True if both shaders have been built, false otherwise.
  bool checkBuilt();


  /// \brief Creates a new program with the GL, attaches shader stages.
  unsigned int createNewProgram();


  /// \brief Map uniform name to location.
  using ParamTable = std::map<const char *, int>;

  //    using TextureTable = std::map<unsigned int, const bd::Texture* >;

  std::vector<Shader *> m_stages;
  unsigned int m_programId; ///< The opengl shader program id
  ParamTable m_params; ///< Uniform locations
};
} // namespace bd

#endif // !shader_h__


