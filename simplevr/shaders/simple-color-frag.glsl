#version 400

in vec3 color; 
out vec4 out_col; 

void main () { 
    out_col = vec4(color,1.0);
}
