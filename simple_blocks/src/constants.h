//
// Created by jim on 1/19/16.
//

#ifndef constants_h__
#define constants_h__

#include <glm/glm.hpp>

extern const int ELEMENTS_PER_QUAD; // { 5 }; //< 5 elements = 4 verts + 1 restart symbol

extern const glm::vec3 X_AXIS; //{ 1.0f, 0.0f, 0.0f };
extern const glm::vec3 Y_AXIS; //{ 0.0f, 1.0f, 0.0f };
extern const glm::vec3 Z_AXIS; //{ 0.0f, 0.0f, 1.0f };

extern const int VERTEX_COORD_ATTR; // = 0;
extern const int VERTEX_COLOR_ATTR; // = 1;

extern const int BLOCK_TEXTURE_UNIT; // = 0;
extern const int TRANSF_TEXTURE_UNIT; // = 1;

extern const char *VOLUME_SAMPLER_UNIFORM_STR;
extern const char *TRANSF_SAMPLER_UNIFORM_STR;

extern const char *LIGHTING_N_SHINEY_UNIFORM_STR; //= "n";
extern const char *LIGHTING_LIGHT_POS_UNIFORM_STR; // = "L";
extern const char *LIGHTING_VIEW_DIR_UNIFORM_STR; // = "V";
extern const char *LIGHTING_MAT_UNIFORM_STR; // = "mat";

//  const int BLOCK_TEXTURE_SAMPLER_UNIFORM = 0;
//  const int TRANSF_TEXTURE_SAMPLER_UNIFORM = 1;

extern const char *VOLUME_MVP_MATRIX_UNIFORM_STR; // = "mvp";
extern const char *VOLUME_TRANSF_SCALER_UNIFORM_STR; // = "tfScalingVal";

extern const char *WIREFRAME_MVP_MATRIX_UNIFORM_STR; // = "mvp";


#endif //! constants_h__
