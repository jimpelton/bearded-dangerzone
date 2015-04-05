#include "shader.h"

#include "log/gl_log.h"

#include <sstream>
#include <fstream>
#include <array>

namespace bd {
Shader::Shader(ShaderType t)
    : m_type(t)
{
}

unsigned Shader::loadFromFile(const std::string& filepath)
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

unsigned int Shader::loadFromString(const std::string& shaderString)
{
    return compileShader(shaderString.c_str());
}

unsigned int Shader::compileShader(const char* shader)
{
    static const std::array<GLenum, 2> shTypes 
    {
        GL_VERTEX_SHADER, GL_FRAGMENT_SHADER 
    };
    int ordinal = static_cast<int>(m_type);
    GLuint shaderId = glCreateShader(shTypes[ordinal]);

    gl_log("Created shader, type: 0x%x04, id: %d", ordinal, shaderId);
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

ShaderProgram::ShaderProgram() 
    : ShaderProgram(nullptr, nullptr)
{
}

ShaderProgram::ShaderProgram(const Shader* vert, const Shader* frag) 
    : m_vert{ vert }
    , m_frag{ frag }
{
}

unsigned ShaderProgram::linkProgram()
{
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
        glGetProgramInfoLog(programId, InfoLogLength, NULL, &programErrorMessage[0]);
        gl_log("%s", &programErrorMessage[0]);
    }

    return programId;
}

unsigned ShaderProgram::linkProgram(const Shader *vert, const Shader *frag)
{
    m_vert = vert;
    m_frag = frag;
    return linkProgram();
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

