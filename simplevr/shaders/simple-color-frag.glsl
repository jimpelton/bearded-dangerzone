#version 400

uniform vec3 in_col; 
out vec4 col; 

void main () { 
    col = vec4(in_col, 1.0f); 
}