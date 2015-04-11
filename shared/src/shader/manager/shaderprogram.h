/*
 * shaderprogram.h
 *
 *  Created on: Dec 14, 2014
 *      Author: jim
 */

#ifndef shaderprogram_h__
#define shaderprogram_h__

#include <GL/glew.h>
#include <stdexcept>
#include <string>

#include "shaderstage.h"

namespace tst {

class AddShaderException : public std::runtime_error
{
public:
    AddShaderException(const std::string &what) : runtime_error(what) {}
    AddShaderException(const char *what) : runtime_error(what) {}
};


class ShaderProgram
{
public:
    /*!
     * \brief Create a ShaderProgram with "no-desc" as the description.
     */
	ShaderProgram();

    /*!
     * \brief Create a shader program with readable description.
     */
	ShaderProgram(const std::string &desc);

	virtual ~ShaderProgram();

    /*!
     * \brief Add \c shader to this program
     *
     * If two shaders of the same type are added (say, two vertex shaders, for
     * example) then the second shader added is set to this program's vertex
     * shader. That is, the 2nd shader added takes precedence.
     *
     * Call build() after you add all your shaders.
     *
     * \returns This ShaderProgram.
     *
     */
	void addShader(ShaderStage *shader);

    /*!
     * \brief Read, load, compile, and link the shaders.
     *
     * Returns the program id as given by glCreateProgram().
     *
     * \return A GLuint that is the program id of this shader program, or 0 on
     * failure.
     */
	GLuint build();

	/*! \brief Notify GL to cleanup resources used by this ShaderProgram */
	void cleanup();

    //GLuint registerUniform(const std::string &uniform);

    /*!
     * \brief Get the id of the allocated shader program as returned by
     *        glCreateProgram().
     */
    GLuint name() const { return m_name; }

    GLint getUniformLocation(const char *uniform);

    const ShaderStage& cVertexShader() const { return *m_vertexShader; }

    const ShaderStage& cFragmentShader() const { return *m_fragmentShader; }

    ShaderStage& vertexShader() const { return *m_vertexShader; }

    ShaderStage& fragmentShader() const { return *m_fragmentShader; }

    const std::string& description() const { return m_desc; }

private:
	GLuint m_name;             /*!< opengl name */
	ShaderStage *m_vertexShader;    /*!< the vert shader for this prog */
	ShaderStage *m_fragmentShader;  /*!< the frag shader for this prog */
    std::string m_desc;        /*!< short human readable desc */
};

} /* namespace tst */

#endif /* SRC_SHADERPROGRAM_H_ */
