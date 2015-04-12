/*
 * shader.cpp
 *
 *  Created on: Dec 14, 2014
 *      Author: jim
 */

#include "shaderstage.h"
#include "log.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>



namespace tst {

ShaderStage::ShaderStage()
	: ShaderStage(ShaderType::None)
{ }

ShaderStage::ShaderStage(ShaderType ty)
    : ShaderStage(ty, "no_description")
{ }

ShaderStage::ShaderStage(ShaderType t, const std::string &desc)
	: m_type(t)
    , m_desc(desc)
	, m_name(0)
    , m_isLoaded(false)
{ }

ShaderStage::~ShaderStage() {}

void ShaderStage::cleanup() {
	throw std::runtime_error("Not implemented");
}

GLuint ShaderStage::build(const std::string &code) {
	if (m_isLoaded) return m_name;

    GLenum type =
    	m_type == ShaderType::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;

    const char* ptrCode = &code[0];

	GLuint shaderId = 0;

	// Create shader and compile
	shaderId = glCreateShader(type);
    if (shaderId == 0) {
        gl_log_err("Unable to create shader!");
    } else {
        gl_log("Created shader, type: 0x%x04, id: %d", type, shaderId);
    }
	glShaderSource(shaderId, 1, &ptrCode, NULL);
	gl_log("Compiling shader: %s", m_desc.c_str());
	glCompileShader(shaderId);

	// Check for errors.
	GLint Result = GL_FALSE;
	int InfoLogLength = 0;

	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (!Result) {
        gl_log_err("Shader compile status for %d: %s.", shaderId,
        		Result ? "GL_TRUE" : "GL_FALSE");
    } else {
        gl_log("Shader compile status for %d: %s.", shaderId,
        		Result ? "GL_TRUE" : "GL_FALSE");
    }
	if ( InfoLogLength > 1 ){
		std::vector<char> msg(InfoLogLength+1);
		glGetShaderInfoLog(shaderId, InfoLogLength, NULL, &msg[0]);
		gl_log_err("Shader info log: %s", &msg[0]);
	}
	m_name = shaderId;
    m_isLoaded = true;

    checkForAndLogGlError(__func__, __LINE__);

	return shaderId;
}

} /* namespace tst */

