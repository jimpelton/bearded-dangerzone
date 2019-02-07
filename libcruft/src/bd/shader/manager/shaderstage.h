/*
 * shader.h
 *
 *  Created on: Dec 14, 2014
 *      Author: jim
 */

#ifndef shaderstage_h__
#define shaderstage_h__

#include <GL/glew.h>
#include <string>

namespace tst {

enum class ShaderType {
	Vertex, Fragment, None
};

/*!
 * \brief Encapsulates an opengl shader.
 *
 * Give references to instances of Shader to ShaderProgram to build and link an
 * entire shader. No need to call Shader::load() yourself, however calling
 * it manually should not harm anything.
 *
 */
class ShaderStage
{

public:
	ShaderStage();

	ShaderStage(ShaderType type);

    /*!
     * \brief create a shader with given \c type, located at \c path, and with readable
     * name \c desc.
     *
     * The string given by desc is useful for printing in log output or
     * to programatically identify which shader this is.
     *
     * \param shaderType One of GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
     * \param path The path to the io containing the shader code.
     * \param desc A human readable description of the shader.
     */
	ShaderStage(ShaderType type, const std::string &desc);

	virtual ~ShaderStage();

public:
    /*!
     * \brief Read the shader in \c code, allocate an OpenGL shader ID and compile.
     *
     * After the initial call, any subsequent calls to load have no effect
     * and return the non-zero OpenGL id obtained during the initial call. If
     * the initial call failed, then 0 is returned.
     *
     * \return The non-zero id returned from glCreateShader(), or 0 on failure.
     *
     */
	GLuint build(const std::string &code);

	/*!
	 * \brief Tell GL to release the resources used by this shader.
	 */
	void cleanup();

    /*!
     * \brief Get the opengl provided name for this shader.
     * \sa Shader::load()
     */
	GLuint name() const { return m_name; }

    /*!
     * \brief Get the opengl shader type.
     *
     * The hex values for fragment and vertex shaders are as follows:
     *  - GL_FRAGMENT_SHADER  0x8B30
     *  - GL_VERTEX_SHADER    0x8B31
     *
     * See \c https://www.opengl.org/registry/#specfiles for details on obtaining
     * these values.
     */
	ShaderType type() const { return m_type; }

    /*!
     * \brief Returns the readable description of this shader.
     */
    const std::string& description() const { return m_desc; }

private:
	ShaderType m_type;
	//std::string m_filePath; /*!< File this shader was loaded from. */
    std::string m_desc;
	GLuint m_name;
    bool m_isLoaded;
};

} /* namespace tst */

#endif /* shaderstage_h__ */
