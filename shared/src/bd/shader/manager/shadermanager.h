
#ifndef shadermanager_h__
#define shadermanager_h__

#include "shaderprogram.h"

#include <vector>
#include <tuple>

namespace tst {




class ShaderManager {

struct ShaderGroup {
	unsigned int name;
	std::string desc;
	ShaderProgram prog;
};

public:
	ShaderManager();
	~ShaderManager();

public:
	/*! \brief Add the shader code to the library of stages. */
	void addShaderStage(ShaderType type, const std::string &desc, const std::string &code);

	void createProgramFromStages(const std::string &first, const std::string &second);

	/*! \brief Tells GL to release all of the shader resources used. */
	void cleanup();

	unsigned int nameOf(const std::string &desc);

	const ShaderProgram& program(unsigned int name) const;
	const ShaderProgram& program(const std::string desc) const;

private:
	void vertexStage(const std::string& desc, const std::string &code, ShaderStage &s);

private:
	std::vector<ShaderProgram> programs;
	std::vector<ShaderStage> stages;

};

}

#endif // shadermanager_h__
