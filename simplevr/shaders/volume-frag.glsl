#version 400

in vec3 uvw;

uniform vec3 color; 
uniform sampler3D volume;
uniform sampler1D tf;

out vec4 out_col; 

void main () { 
    float val = texture(volume, uvw).r;
    out_col = vec4(val, val, val, 1.0); // * val; 
    //out_col = vec4(uvw, 1.0);
}