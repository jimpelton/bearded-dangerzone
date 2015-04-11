/*
 * shadermanager.cpp
 *
 *  Created on: Feb 6, 2015
 *      Author: jim
 */

#include "shadermanager.h"
#include "shaderstage.h"

#include <algorithm>

namespace tst{

ShaderManager::ShaderManager() { }

ShaderManager::~ShaderManager() { }

void ShaderManager::addShaderStage
(
	ShaderType type,
	const std::string &desc,
	const std::string &code
)
{
	ShaderStage s(type, desc);
	s.build(code);
	stages.push_back(s);
}

void ShaderManager::createProgramFromStages
(
	const std::string &first,
	const std::string &second
)
{

}

void ShaderManager::cleanup() {
	throw std::runtime_error("ShaderManager::cleanup not implemented.");
}

unsigned int ShaderManager::nameOf(const std::string &desc) {
	std::vector<ShaderProgram>::iterator found =
		std::find_if(programs.begin(), programs.end(),
			[desc] (ShaderProgram &s) {
				return s.description() == desc;
		} );

	unsigned int rval = 0;
	if (found != programs.end()) {
		rval = (*found).name();
	}

	return rval;
}



} /* namespace tst */



