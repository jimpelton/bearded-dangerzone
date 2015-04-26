#version 400 core

// Interpolated values from the vertex shaders
in vec3 vcol;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler3D volume_sampler;
uniform sampler1D tf_sampler;
// uniform float tfScalingVal;

void main() {
	float volVal = texture(volume_sampler, vcol).x;
	color = texture(tf_sampler, volVal);

//  	color = vec4(volVal, volVal, volVal, volVal) * 1.5f;
    //if(volVal == 0.0f) color.x = 1.0f;

//  color = vec4(vcol, 1.0f);
}
