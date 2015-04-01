#ifndef shader_h__
#define shader_h__

#include <GL/glew.h>

#include <vector>
#include <string>

namespace bd {
namespace util {
GLuint loadShader(GLenum type, std::string filepath);
GLuint compileShader(GLenum type, const char *shader);
GLuint linkProgram(const std::vector<GLuint> &shaderIds);
}
} // namespace

#endif
