#include <bd/log/logger.h>
#include <bd/log/gl_log.h>
#include <bd/graphics/shader.h>
#include <bd/util/gl_strings.h>
#include <bd/util/ordinal.h>

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <sstream>
#include <fstream>
#include <iostream>


namespace bd
{ namespace
{
const std::array<GLenum, 2> gl_target
{
  GL_VERTEX_SHADER,
  GL_FRAGMENT_SHADER
};
} // namespace


//////////////////////////////////////////////////////////////////////////
//     Compiler  Implementation
//////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
bool
Compiler::compile(Shader& shader, const char* code)
{
  unsigned int shaderId{ shader.id() };

  gl_check(glShaderSource(shaderId, 1, &code, nullptr));

  gl_check(glCompileShader(shaderId));

  // Check for errors.
  GLint result{ GL_FALSE };
  int infoLogLength;

  gl_check(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result));
  gl_check(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength));

  if (infoLogLength > 1) {
    std::vector<char> msg(infoLogLength + 1);
    gl_check(glGetShaderInfoLog(shaderId, infoLogLength, nullptr, &msg[0]));
    Dbg() << &msg[0];
  }

  return result == GL_TRUE;
}


//////////////////////////////////////////////////////////////////////////
//     Shader  Implementation
//////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
Shader::Shader(ShaderType t, const std::string& name)
  : BDObj(name)
    , m_type(t)
    , m_id(0)
{
}

Shader::~Shader()
{
  Dbg() << "Destructing shader: Type: " << typeString() << " GLid: " << m_id;
}


unsigned int
Shader::create()
{
  GLenum gl_type{ gl_target[ordinal<ShaderType>(m_type)] };
  GLuint shaderId = gl_check(glCreateShader(gl_type));

  Dbg() << "glCreateShader for type=" << bd::gl_to_string(gl_type) << " returned id=" << shaderId;

  return m_id = shaderId;
}

///////////////////////////////////////////////////////////////////////////////
void
Shader::loadFromFile(const std::string& filepath)
{
  //GLuint shaderId = 0;
  std::ifstream file(filepath.c_str());
  if (!file.is_open()) {
    Err() << "Couldn't open " << filepath;
    return;
  }
  std::stringstream shaderCode;
  shaderCode << file.rdbuf();

  std::string code{ shaderCode.str() };
  const char* ptrCode{ code.c_str() };
  file.close();

  bool result{ Compiler::compile(*this, ptrCode) }; //compileShader(ptrCode);
  if (! result) {
    Err() << "Shader " << this->to_string() << " failed to compile.";
  }
}


///////////////////////////////////////////////////////////////////////////////
void
Shader::loadFromString(const std::string& shaderString)
{
  bool result{ Compiler::compile(*this, shaderString.c_str()) };
  if (! result) {
    Err() << "Shader " << this->to_string() << " failed to compile.";
  }
  //return compileShader(shaderString.c_str());
}


///////////////////////////////////////////////////////////////////////////////
//unsigned int Shader::compileShader(const char *shader) {
//  return Compiler::compile(m_type, shader);
//}


///////////////////////////////////////////////////////////////////////////////
ShaderType
Shader::type() const
{
  return m_type;
}


///////////////////////////////////////////////////////////////////////////////
const char*
Shader::typeString() const
{
  return gl_to_string(gl_target[ordinal(m_type)]);
}

std::string
Shader::to_string() const
{
  return BDObj::to_string() + " " + typeString();
}


//////////////////////////////////////////////////////////////////////////
//   ShaderProgram Impl
//////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
ShaderProgram::ShaderProgram()
  : ShaderProgram(nullptr, nullptr)
{
}


///////////////////////////////////////////////////////////////////////////////
ShaderProgram::ShaderProgram(Shader* vert, Shader* frag)
  : m_stages{ }
    , m_programId{ 0 }
    , m_params{ }
{
  if (vert) addStage(vert);
  if (frag) addStage(frag);
}


///////////////////////////////////////////////////////////////////////////////
ShaderProgram::~ShaderProgram()
{
  //TODO: cleanup opengl shaders ... glDelete?
  Err() << "I might be a memory/resource leak (no cleanup in shader destructor).";
}


///////////////////////////////////////////////////////////////////////////////
//unsigned int ShaderProgram::addStage(const std::string &path, ShaderType ty) {
//  Shader *sh{ new Shader(ty) };
//  sh->create();
//  sh->loadFromFile(path);
//  return addStage(sh);
//}


///////////////////////////////////////////////////////////////////////////////
unsigned int
ShaderProgram::linkProgram()
{
  if (!checkBuilt()) {
    // this ShaderProgram has already been built.
    return m_programId;
  }

  if (m_stages.empty()) {
    Err() << "When linking program, there were no shaders to link! Cannot "
      "create shader program.";
    return 0;
  }

  if (m_programId != 0) {
    Dbg() << "Relinking shader program id=" << m_programId;
  } else {
    // have GL make a program and bail out if no success.
    createNewProgram();

    if (m_programId == 0)
      return 0;

    Dbg() << "Linking shader program id=" <<  m_programId;
  }

  gl_check(glLinkProgram(m_programId));

  // Check the program
  int InfoLogLength{ 0 };
  GLint result{ GL_FALSE };

  gl_check(glGetProgramiv(m_programId, GL_LINK_STATUS, &result));
  gl_check(glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &InfoLogLength));
  Dbg() << "GL Link Status for shader program "
        << m_programId << ": "
        << (result == GL_FALSE ? "GL_FALSE" : "GL_TRUE")
        << ".";

  if (InfoLogLength > 1) {
    std::vector<char> programErrorMessage(InfoLogLength + 1);
    gl_check(glGetProgramInfoLog(m_programId,
      InfoLogLength,
      nullptr,
      &programErrorMessage[0]));
    Dbg() << &programErrorMessage[0];
  }

  if (! result)
    return 0;

  return m_programId;
}


///////////////////////////////////////////////////////////////////////////////
unsigned int
ShaderProgram::linkProgram(Shader* vert, Shader* frag)
{
  addStage(vert);
  addStage(frag);
  return linkProgram();
}


///////////////////////////////////////////////////////////////////////////////
unsigned int ShaderProgram::linkProgram(const std::string& vertFilePath,
                                        const std::string& fragFilePath)
{
  Shader* vert{ new Shader(ShaderType::Vertex, vertFilePath.c_str()) };
  vert->create();
  vert->loadFromFile(vertFilePath);

  Shader* frag{ new Shader(ShaderType::Fragment, fragFilePath.c_str()) };
  frag->create();
  frag->loadFromFile(fragFilePath);

  return linkProgram(vert, frag);
}


///////////////////////////////////////////////////////////////////////////////
void
ShaderProgram::setUniform(const char* param, const glm::mat4& val)
{
  unsigned int loc = getUniformLocation(param);
  //  gl_check(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val)));
  gl_check(glProgramUniformMatrix4fv(m_programId, loc, 1, GL_FALSE, glm::value_ptr(val)));
}


///////////////////////////////////////////////////////////////////////////////
void
ShaderProgram::setUniform(const char* param, const glm::vec4& val)
{
  unsigned int loc = getUniformLocation(param);
  //  gl_check(glUniform4fv(loc, 1, glm::value_ptr(val)));
  gl_check(glProgramUniform4fv(m_programId, loc, 1, glm::value_ptr(val)));
}


///////////////////////////////////////////////////////////////////////////////
void
ShaderProgram::setUniform(const char* param, const glm::vec3& val)
{
  unsigned int loc = getUniformLocation(param);
  //  gl_check(glUniform3fv(m_programId, loc, 1, glm::value_ptr(val)));
  gl_check(glProgramUniform3fv(m_programId, loc, 1, glm::value_ptr(val)));
}


///////////////////////////////////////////////////////////////////////////////
void
ShaderProgram::setUniform(const char* param, glm::f32 val)
{
  unsigned int loc = getUniformLocation(param);
  //  gl_check(glUniform1f(m_programId, loc, val));
  gl_check(glProgramUniform1f(m_programId, loc, val));
}


///////////////////////////////////////////////////////////////////////////////
//void ShaderProgram::setUniform(const std::string &param, const Texture &tex)
//{
//    unsigned int loc = getUniformLocation(param);
//    m_textures[loc] = &tex;
//}

void
ShaderProgram::setUniform(const char* param, glm::i32 val)
{
  unsigned int loc = getUniformLocation(param);
  //  gl_check(glUniform1f(m_programId, loc, val));
  gl_check(glProgramUniform1i(m_programId, loc, val));
}

///////////////////////////////////////////////////////////////////////////////
unsigned int
ShaderProgram::getUniformLocation(const char* param)
{
  int rval{ 0 };
  ParamTable::iterator found = m_params.find(param);
  if (found != m_params.end()) {
    rval = (*found).second;
  } else {
    rval = gl_check(glGetUniformLocation(m_programId, param));
    // put the uniform location in params table for faster lookup next time.
    m_params[param] = rval;
  }

  return static_cast<unsigned int>(rval);
}


///////////////////////////////////////////////////////////////////////////////
void
ShaderProgram::bind()
{
  //    static const std::array<GLenum, 3> targets
  //    {
  //        GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D
  //    };

  gl_check(glUseProgram(m_programId));

  //    int i = 0;
  // pair is (glsl sampler location, Texture)
  //    for (auto &pair : m_textures) {
  //        gl_check(glActiveTexture(GL_TEXTURE0 + i));
  //        const Texture &tex = *(pair.second);
  //        GLenum target = targets.at(static_cast<int>(tex.type()));
  //        gl_check(glBindTexture(target, tex.id()));
  //        gl_check(glUniform1f(pair.first, tex.id()));
  //
  //        i += 1;
  //    }

  //gl_check(glActiveTexture(GL_TEXTURE0));
}


///////////////////////////////////////////////////////////////////////////////
void
ShaderProgram::unbind()
{
  gl_check(glUseProgram(0));
}


///////////////////////////////////////////////////////////////////////////////
unsigned int
ShaderProgram::programId() const
{
  return m_programId;
}

//////////////////////////////////////////////////////////////////////////
bool
ShaderProgram::validateProgram()
{
  unsigned int id{ m_programId };
  GLint val{ GL_FALSE };
  gl_check(glValidateProgram(id));
  gl_check(glGetProgramiv(id, GL_VALIDATE_STATUS, &val));

  int logLength{ 0 };
  gl_check(glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength));
  if (logLength > 0) {
    std::vector<char> msg(logLength);
    gl_check(glGetProgramInfoLog(id, logLength, NULL, &msg[0]));
    Dbg() <<  &msg[0];
  }

  return val == GL_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//  ShaderProgram Private Members
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
unsigned int
ShaderProgram::addStage(Shader* stage)
{
  m_stages.push_back(stage);
  unsigned int id{ stage->id() };
  Dbg() << "Added shader " << stage->to_string();

  return id;
}


///////////////////////////////////////////////////////////////////////////////
unsigned int
ShaderProgram::createNewProgram()
{
  GLuint programId = gl_check(glCreateProgram());
  if (programId == 0) {

    Err() << "Unable to create shader program with glCreateProgram(). "
      "Returned id was 0.";

  } else {

    Dbg() << "Created program id: " << programId;
    for (const Shader* stage : m_stages) {
      gl_check(glAttachShader(programId, stage->id()));
    }

  }

  return m_programId = programId;
}

///////////////////////////////////////////////////////////////////////////////
bool
ShaderProgram::checkBuilt()
{
  bool rval = true;
  auto shader = m_stages.begin();

  while (rval && shader != m_stages.end()) {
    rval = (*shader)->isBuilt();
    Dbg() << "Shader " << (*shader)->to_string() << "is built: "
        << (rval ? "true" : "false");
    ++shader;
  }

  if (!rval && shader != m_stages.end()) {
    Err() << "Error Linking shader program " << m_programId
        << ": Shader " <<  (*shader)->to_string() << " was not built.";
  }

  return rval;
}
} //namespace bd


