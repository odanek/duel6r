#version 430

uniform mat4 mvp;
layout(location = 0) in vec3 vp;

void main() {
    gl_Position = mvp * vec4(vp, 1.0);
}
