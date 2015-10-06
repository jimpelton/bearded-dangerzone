#version 400 core

in vec3 vcol;
out vec4 frag_color;

void main () {
  frag_color = vec4(vcol, 1.0);
}
