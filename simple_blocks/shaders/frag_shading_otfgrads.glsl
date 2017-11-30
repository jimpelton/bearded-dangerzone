#version 420 core

layout(location=0) smooth in vec3 vcol;
out vec4 color;

uniform sampler3D volume_sampler;
uniform sampler1D tf_sampler;
uniform float tfScalingVal;

uniform float n;   // n_shiney!
uniform vec3  L;    // light vector (expected normalized)
uniform vec3  V;    // viewing vector (expected normalized)
uniform vec3  mat;  //ambient=mat.x, diffuse=mat.y and specular=mat.z

const vec3 col_ambient = vec3(1, 1, 1);
const vec3 col_diffuse = vec3(1, 1, 1);
const vec3 col_spec = vec3(1, 1, 1);
const vec3 stepSize = vec3(0.01, 0.01, 0.01);

void main() {
    
	float volVal = texture(volume_sampler, vcol).x;
	
	// compute the gradient
	float Xp = texture(volume_sampler, vcol.xyz + vec3(+stepSize.x, 0, 0)).x;
	float Xm = texture(volume_sampler, vcol.xyz + vec3(-stepSize.x, 0, 0)).x;
	float Yp = texture(volume_sampler, vcol.xyz + vec3(0, -stepSize.y, 0)).x;
	float Ym = texture(volume_sampler, vcol.xyz + vec3(0, +stepSize.y, 0)).x;
	float Zp = texture(volume_sampler, vcol.xyz + vec3(0, 0, +stepSize.z)).x;
	float Zm = texture(volume_sampler, vcol.xyz + vec3(0, 0, -stepSize.z)).x;
	vec3 grad3 = normalize(vec3((Xm - Xp) * 0.5, (Yp - Ym) * 0.5, (Zm - Zp) * 0.5));

    // fetch color from transfer function
	vec4 tfCol = texture(tf_sampler, tfScalingVal*volVal);
    
	vec3 H = normalize(L + V);
    float HdotN = max(dot(H, grad3), 0.0);
	float LdotN = max(dot(L, grad3), 0.0);
	
	color =
        clamp(vec4(col_ambient * mat.x +
                   col_diffuse * mat.y * LdotN +
                   col_spec    * mat.z * pow(HdotN, n), 1.0 ),
              0.0,1.0 ) * tfCol;
//    color = vec4(volVal, 0, 0, 1.0) * tfCol;
}
