#version 430

const float waveHeight = 0.1;

layout(location = 0) in vec3 vp;
layout(location = 1) in vec2 uvIn;
layout(location = 2) in float texIndexIn;
layout(location = 3) in uint flagsIn;

uniform mat4 mvp;
uniform float globalTime;

out vec3 uv;

void main() {
    vec3 pos = vp;
    if (flagsIn == 1) {
        float displacement = sin(globalTime * 2.13 + 1.05 * vp.x) * waveHeight;
        pos = vec3(pos.x, pos.y - waveHeight + displacement, pos.z);
    }
    gl_Position = mvp * vec4(pos, 1.0);
    uv = vec3(uvIn, texIndexIn);
}