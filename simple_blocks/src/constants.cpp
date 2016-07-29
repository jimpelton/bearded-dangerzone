//
// Created by Jim Pelton on 7/28/16.
//

#include "constants.h"

#include <glm/glm.hpp>

const int ELEMENTS_PER_QUAD{ 5 }; //< 5 elements = 4 verts + 1 restart symbol

const glm::vec3 X_AXIS{ 1.0f, 0.0f, 0.0f };
const glm::vec3 Y_AXIS{ 0.0f, 1.0f, 0.0f };
const glm::vec3 Z_AXIS{ 0.0f, 0.0f, 1.0f };

const int VERTEX_COORD_ATTR = 0;
const int VERTEX_COLOR_ATTR = 1;

const int BLOCK_TEXTURE_UNIT = 0;
const int TRANSF_TEXTURE_UNIT = 1;

const char *VOLUME_SAMPLER_UNIFORM_STR = "volume_sampler";
const char *TRANSF_SAMPLER_UNIFORM_STR = "tf_sampler";

const char *VOLUME_MVP_MATRIX_UNIFORM_STR = "mvp";
const char *VOLUME_TRANSF_UNIFORM_STR = "tfScalingVal";

const char *WIREFRAME_MVP_MATRIX_UNIFORM_STR = "mvp";
