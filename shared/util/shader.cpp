#include "shader.h"

#include "log/gl_log.h"

#include <GL/glew.h>

#include <sstream>
#include <fstream>
#include <array>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

namespace bd {


///////////////////////////////////////////////////////////////////////////////
Shader::Shader(ShaderType t)
    : m_type(t)
    , m_id(0)
{
}


///////////////////////////////////////////////////////////////////////////////
unsigned int Shader::loadFromFile(const std::string& filepath)
{
    //GLuint shaderId = 0;
    std::ifstream file(filepath.c_str());
    if (!file.is_open()) {
        gl_log_err("Couldn't open %s", filepath.c_str());
        return 0;
    }
    std::stringstream shaderCode;
    shaderCode << file.rdbuf();

    std::string code = shaderCode.str();
    const char *ptrCode = code.c_str();
    file.close();
    
    return compileShader(ptrCode);
}


///////////////////////////////////////////////////////////////////////////////
unsigned int Shader::loadFromString(const std::string& shaderString)
{
    return compileShader(shaderString.c_str());
}


///////////////////////////////////////////////////////////////////////////////
unsigned int Shader::compileShader(const char* shader)
{
    static const std::array<GLenum, 2> shTypes 
    {
        GL_VERTEX_SHADER, GL_FRAGMENT_SHADER 
    };

    GLenum gl_type = shTypes.at(static_cast<int>(m_type));
    GLuint shaderId = glCreateShader(gl_type);

    gl_log("Created shader, type: 0x%x04, id: %d", gl_type, shaderId);
    glShaderSource(shaderId, 1, &shader, nullptr);

    glCompileShader(shaderId);

    // Check for errors.
    GLint result = GL_FALSE;
    int infoLogLength;

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength > 1) {
        std::vector<char> msg(infoLogLength + 1);
        glGetShaderInfoLog(shaderId, infoLogLength, nullptr, &msg[0]);
        gl_log("%s", &msg[0]);
    }
    

    return m_id = shaderId;
}


///////////////////////////////////////////////////////////////////////////////
ShaderProgram::ShaderProgram() 
    : ShaderProgram(nullptr, nullptr)
{
}


///////////////////////////////////////////////////////////////////////////////
ShaderProgram::ShaderProgram(const Shader* vert, const Shader* frag) 
    : m_vert{ vert }
    , m_frag{ frag }
{
}


///////////////////////////////////////////////////////////////////////////////
ShaderProgram::~ShaderProgram()
{
}


///////////////////////////////////////////////////////////////////////////////
unsigned int ShaderProgram::linkProgram()
{
    if (!checkBuilt()) 
        return 0;
    
    GLuint programId = glCreateProgram();
    gl_log("Created program id: %d", programId);

    glAttachShader(programId, m_vert->id());
    glAttachShader(programId, m_frag->id());

    gl_log("Linking program");
    glLinkProgram(programId);

    // Check the program
    int InfoLogLength = 0;
    GLint result = GL_FALSE;

    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if (InfoLogLength > 1) {
        std::vector<char> programErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(programId, InfoLogLength, nullptr, &programErrorMessage[0]);
        gl_log("%s", &programErrorMessage[0]);
    }

    return m_programId = programId;
}


///////////////////////////////////////////////////////////////////////////////
unsigned int ShaderProgram::linkProgram(const Shader *vert, const Shader *frag)
{
    m_vert = vert;
    m_frag = frag;
    return linkProgram();
}


///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::setUniform(const std::string &param, glm::mat4 &val)
{
    unsigned int loc = getParamLocation(param);
    gl_check(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val)));
}


///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::setUniform(const std::string &param, glm::vec4 &val)
{
    unsigned int loc = getParamLocation(param);
    gl_check(glUniform4fv(loc, 1, glm::value_ptr(val)));
}


///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::setUniform(const std::string &param, glm::vec3 &val)
{
    unsigned int loc = getParamLocation(param);
    gl_check(glUniform3fv(loc, 1, glm::value_ptr(val)));
}


///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::setUniform(const std::string& param, const Texture& tex)
{
    unsigned int loc = getParamLocation(param);
    m_textures[loc] = &tex;
}


///////////////////////////////////////////////////////////////////////////////
unsigned int ShaderProgram::getParamLocation(const std::string& param)
{
    unsigned int rval = 0;
    ParamTable::iterator found = m_params.find(param);
    if (found != m_params.end()) {
        rval = (*found).second;
    } else {
        rval = gl_check(glGetUniformLocation(m_programId, param.c_str()));
        m_params[param] = rval;
    }

    return rval;
}


///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::bind()
{
    static const std::array<GLenum, 3> targets 
    { 
        GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D 
    };
    
    gl_check(glUseProgram(m_programId));
    
    int i = 0;
    for (auto &pair : m_textures) {
        gl_check(glActiveTexture(GL_TEXTURE0 + i));
        const Texture &tex = *(pair.second);
        GLenum type = targets.at(static_cast<int>(tex.type()));
        gl_check(glBindTexture(type, tex.id()));
        gl_check(glUniform1f(pair.first, tex.id()));

        i += 1;
    }

    gl_check(glActiveTexture(GL_TEXTURE0));
}


///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::unbind()
{
    gl_check(glUseProgram(0));
}


///////////////////////////////////////////////////////////////////////////////
bool ShaderProgram::checkBuilt()
{
    bool rval = true;
    
    if (!m_vert->isBuilt()) {
        gl_log_err("Vertex shader has not been built, cannot link program.");
        rval = false;
    }

    if (!m_frag->isBuilt()) {
        gl_log_err("Fragment shader has not been built, cannot link program.");
        rval = false;
    }

    return rval;
}

//GLuint loadShader(GLenum type, std::string filepath)
//{
//    GLuint shaderId = 0;
//    std::ifstream file(filepath.c_str());
//    if (!file.is_open()) {
//        gl_log_err("Couldn't open %s", filepath.c_str());
//        return 0;
//    }
//
//    std::stringstream shaderCode;
//    shaderCode << file.rdbuf();
//
//    std::string code = shaderCode.str();
//    const char *ptrCode = code.c_str();
//    file.close();
//
//    gl_log("Compiling shader: %s", filepath.c_str());
//    shaderId = compileShader(type, ptrCode);
//    return shaderId;
//}

//GLuint compileShader(GLenum type, const char *shader)
//{
//    // Create shader and compile
//    GLuint shaderId = glCreateShader(type);
//    gl_log("Created shader, type: 0x%x04, id: %d", type, shaderId);
//    glShaderSource(shaderId, 1, &shader, NULL);
//
//    glCompileShader(shaderId);
//
//    // Check for errors.
//    GLint Result = GL_FALSE;
//    int InfoLogLength;
//
//    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result);
//    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
//
//    if (InfoLogLength > 1) {
//        std::vector<char> msg(InfoLogLength + 1);
//        glGetShaderInfoLog(shaderId, InfoLogLength, NULL, &msg[0]);
//        gl_log("%s", &msg[0]);
//    }
//
//    return shaderId;
//}

//GLuint linkProgram(const std::vector<GLuint> &shaderIds)
//{
//    GLuint programId = glCreateProgram();
//    gl_log("Created program id: %d", programId);
//
//    for (auto &sh : shaderIds) {
//        glAttachShader(programId, sh);
//    }
//
//    gl_log("Linking program");
//    glLinkProgram(programId);
//
//    // Check the program
//    int InfoLogLength = 0;
//    GLint result = GL_FALSE;
//
//    glGetProgramiv(programId, GL_LINK_STATUS, &result);
//    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &InfoLogLength);
//
//    if (InfoLogLength > 1) {
//        std::vector<char> programErrorMessage(InfoLogLength + 1);
//        glGetProgramInfoLog(programId, InfoLogLength, NULL, &programErrorMessage[0]);
//        gl_log("%s", &programErrorMessage[0]);
//    }
//
//    return programId;
//}


} // namespace bd

