#include <bd/graphics/shader.h>

#include <bd/log/gl_log.h>

#include <GL/glew.h>

#include <sstream>
#include <fstream>
#include <array>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

namespace bd {


///////////////////////////////////////////////////////////////////////////////
Shader::Shader(ShaderType t)
    : m_type(t), m_id(0)
{
}


///////////////////////////////////////////////////////////////////////////////
unsigned int Shader::loadFromFile(const std::string &filepath)
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
unsigned int Shader::loadFromString(const std::string &shaderString)
{
    return compileShader(shaderString.c_str());
}


///////////////////////////////////////////////////////////////////////////////
unsigned int Shader::compileShader(const char *shader)
{
    static const std::array<GLenum, 2> shTypes
        {
            GL_VERTEX_SHADER, GL_FRAGMENT_SHADER
        };

    GLenum gl_type = shTypes.at(static_cast<int>(m_type));
    GLuint shaderId = gl_check(glCreateShader(gl_type));

    gl_log("Created shader, type: 0x%x04, id: %d", gl_type, shaderId);
    gl_check(glShaderSource(shaderId, 1, &shader, nullptr));

    gl_check(glCompileShader(shaderId));

    // Check for errors.
    GLint result = GL_FALSE;
    int infoLogLength;

    gl_check(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result));
    gl_check(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength));

    if (infoLogLength > 1) {
        std::vector<char> msg(infoLogLength + 1);
        gl_check(glGetShaderInfoLog(shaderId, infoLogLength, nullptr, &msg[0]));
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
ShaderProgram::ShaderProgram(Shader *vert, Shader *frag)
    : m_vert{ vert }, m_frag{ frag }
{
}


///////////////////////////////////////////////////////////////////////////////
ShaderProgram::~ShaderProgram()
{
    //TODO: cleanup opengl shaders ... glDelete?
    gl_log_err("I might be a memory leak. Who cares! This be academia, foo!");
}


///////////////////////////////////////////////////////////////////////////////
unsigned int ShaderProgram::linkProgram()
{
    if (!checkBuilt())
        return 0;

    GLuint programId = gl_check(glCreateProgram());
    gl_log("Created program id: %d", programId);

    gl_check(glAttachShader(programId, m_vert->id()));
    gl_check(glAttachShader(programId, m_frag->id()));

    gl_log("Linking program");
    gl_check(glLinkProgram(programId));

    // Check the program
    int InfoLogLength = 0;
    GLint result = GL_FALSE;

    gl_check(glGetProgramiv(programId, GL_LINK_STATUS, &result));
    gl_check(glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &InfoLogLength));

    if (InfoLogLength > 1) {
        std::vector<char> programErrorMessage(InfoLogLength + 1);
        gl_check(glGetProgramInfoLog(programId, InfoLogLength, nullptr,
            &programErrorMessage[0]));
        gl_log("%s", &programErrorMessage[0]);
    }

    return m_programId = programId;
}


///////////////////////////////////////////////////////////////////////////////
unsigned int ShaderProgram::linkProgram(Shader *vert, Shader *frag)
{
    m_vert = vert;
    m_frag = frag;
    return linkProgram();
}

unsigned int ShaderProgram::linkProgram(const std::string &vertPath,
    const std::string &fragPath)
{
    Shader *vert = new Shader(ShaderType::Vertex);
    vert->loadFromFile(vertPath);

    Shader *frag = new Shader(ShaderType::Fragment);
    frag->loadFromFile(fragPath);

    return linkProgram(vert, frag);
}

///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::setUniform(const char* param, glm::mat4& val)
{
    unsigned int loc = getUniformLocation(param);
    gl_check(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val)));
}


///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::setUniform(const char *param, glm::vec4 &val)
{
    unsigned int loc = getUniformLocation(param);
    gl_check(glUniform4fv(loc, 1, glm::value_ptr(val)));
}


///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::setUniform(const char *param, glm::vec3 &val)
{
    unsigned int loc = getUniformLocation(param);
    gl_check(glUniform3fv(loc, 1, glm::value_ptr(val)));
}


///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::setUniform(const char *param, float val )
{
    unsigned int loc = getUniformLocation(param);
    gl_check(glUniform1f(loc, val));
}


///////////////////////////////////////////////////////////////////////////////
//void ShaderProgram::setUniform(const std::string &param, const Texture &tex)
//{
//    unsigned int loc = getUniformLocation(param);
//    m_textures[loc] = &tex;
//}


///////////////////////////////////////////////////////////////////////////////
unsigned int ShaderProgram::getUniformLocation(const char *param)
{
    unsigned int rval = 0;
    ParamTable::iterator found = m_params.find(param);
    if (found != m_params.end()) {
        rval = (*found).second;
    } else {
        rval = gl_check(glGetUniformLocation(m_programId, param));
        m_params[param] = rval;
    }

    return rval;
}


///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::bind()
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

} //namespace bd

