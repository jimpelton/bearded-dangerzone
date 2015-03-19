
#include "shaderprogram.h"
#include "log.h"

#include <vector>
#include <assert.h>

namespace tst {

ShaderProgram::ShaderProgram()
    : ShaderProgram("no-name")
{ }


ShaderProgram::ShaderProgram(const std::string &desc)
    : m_name(0)
    , m_vertexShader(nullptr)
    , m_fragmentShader(nullptr)
    , m_desc(desc)
{ }

ShaderProgram::~ShaderProgram() { }

void ShaderProgram::addShader(ShaderStage *shader)
{
	assert(shader != nullptr && "Provided shader was a null pointer!");

    switch (shader->type()) {

    case ShaderType::Vertex:
        m_vertexShader = shader;
        break;

    case ShaderType::Fragment:
        m_fragmentShader = shader;
        break;

    default:
        gl_log_err("Unknown shader type.");
        throw AddShaderException(
			std::string("Unknown shader type provided: ") +
				(shader ? shader->type() == ShaderType::Vertex ? "Vertex" : "Fragment" : "")
			);

        break;
    }
}

GLuint ShaderProgram::build()
{

    // Link the program

    GLuint programId = glCreateProgram();
    gl_log("Created program id: %d", programId);

    glAttachShader(programId, m_vertexShader->name());
    glAttachShader(programId, m_fragmentShader->name());

    gl_log("Linking program");
    glLinkProgram(programId);

    // Check the program
    int InfoLogLength = 0;
    GLint result = GL_FALSE;

    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if ( InfoLogLength > 1 ) {
        std::vector<char> programErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(programId, InfoLogLength, NULL, &programErrorMessage[0]);
        gl_log_err("Shader Program info log: %s", &programErrorMessage[0]);
    }

    m_name = programId;

    checkForAndLogGlError(__func__, __LINE__);

    return programId;
}

void ShaderProgram::cleanup()
{
	throw std::runtime_error("Method ShaderProgram::cleanup() not implemented.");
}

GLint ShaderProgram::getUniformLocation(const char *uniform)
{
    assert(m_name != 0 && "m_name is 0, did the shader program build successfuly?");
    return glGetUniformLocation(m_name, uniform);
}

} /* namespace tst */
