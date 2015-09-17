#version 400

in vec3 v;
in vec3 in_col;
uniform mat4 mvp;

out vec3 vcol;

void main () {
  gl_Position = mvp * vec4(v, 1.0);
  vcol = in_col;
}

