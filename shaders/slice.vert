#version 330 core
layout (location = 0) in vec3 vert;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(vert, 1.0);
}
