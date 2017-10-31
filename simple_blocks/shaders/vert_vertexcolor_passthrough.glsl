#version 400 core

// vertices
layout(location = 0) in vec4 v;

// This is used for either vertex color (without opacity channel) or texture coordinates
// and is passed through to the fragment shader un changed.
layout(location = 1) in vec3 in_col;

uniform mat4 mvp;

out vec3 vcol;

void main () {
  gl_Position = mvp * v;
  vcol = in_col.xyz;
}

