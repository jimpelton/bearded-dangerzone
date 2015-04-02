#version 400

in vec3 vert; 
in vec3 col;
uniform mat4 mvp;
out vec3 color;
void main() {
    gl_Position = mvp * vec4(vert, 1.0);
    color = col;
}



