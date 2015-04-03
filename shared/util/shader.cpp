#include "shader.h"

#include "log/gl_log.h"

#include <sstream>
#include <fstream>

namespace bd {
GLuint loadShader(GLenum type, std::string filepath)
{
    GLuint shaderId = 0;
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

    gl_log("Compiling shader: %s", filepath.c_str());
    shaderId = compileShader(type, ptrCode);
    return shaderId;
}

GLuint compileShader(GLenum type, const char *shader)
{
    // Create shader and compile
    GLuint shaderId = glCreateShader(type);
    gl_log("Created shader, type: 0x%x04, id: %d", type, shaderId);
    glShaderSource(shaderId, 1, &shader, NULL);

    glCompileShader(shaderId);

    // Check for errors.
    GLint Result = GL_FALSE;
    int InfoLogLength;

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if (InfoLogLength > 1) {
        std::vector<char> msg(InfoLogLength + 1);
        glGetShaderInfoLog(shaderId, InfoLogLength, NULL, &msg[0]);
        gl_log("%s", &msg[0]);
    }

    return shaderId;
}

GLuint linkProgram(const std::vector<GLuint> &shaderIds)
{
    GLuint programId = glCreateProgram();
    gl_log("Created program id: %d", programId);

    for (auto &sh : shaderIds) {
        glAttachShader(programId, sh);
    }

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
} // namespace bd

