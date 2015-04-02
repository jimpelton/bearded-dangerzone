#version 400
in vec3 vert; 
uniform mat4 vp;       // proj * view
uniform mat4 m;       // model matrix
uniform mat4 r;     // rotation matrix
uniform vec3 vdir;    // view dir
uniform float ds;     // slice distance

out vec3 uvw;

void main () {
  uvw = vert + 0.5f; 
  vec4 offset = vec4(vdir * ( ds * gl_InstanceID ), 1.0f); 
  vec4 vert_off = m * vec4(vert, 1.0f) + offset;
  
  //uvw = vert_off.xyz;

  gl_Position =  vp * r * vert_off;
  
  
  
  // gl_Position = m * ( vec4(vert, 1.0f) );
}
