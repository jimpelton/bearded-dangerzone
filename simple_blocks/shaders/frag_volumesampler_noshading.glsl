#version 400 core

// Interpolated values from the vertex shaders
in vec3 UVW;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler3D volume_sampler;
// uniform sampler1D tf_sampler;
// uniform float tfScalingVal;

void main() {
	float volVal = texture(volume_sampler, UVW).x;
//	color = texture(tf_sampler, tfScalingVal*volVal);

  	color = vec4(volVal, volVal, volVal, volVal);
//  color = vec4(UVW, 1.0f);
}
