#version 400
in vec3 vert; 
uniform mat4 vp;       // proj * view
//uniform mat4 p;       // proj matrix
uniform mat4 m;       // model matrix
uniform mat4 r;     // rotation matrix
uniform vec3 vdir;    // view dir

uniform float ds;     // slice distance

void main () { 
  vec4 offset = vec4(vdir * ( ds * gl_InstanceID ), 1.0f); 
  gl_Position =  vp * r * (m * vec4(vert, 1.0f) + offset);
  // gl_Position = m * ( vec4(vert, 1.0f) );
}
