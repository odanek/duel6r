#version 430

layout(binding = 0) uniform sampler2DArray textureUnit;

uniform bool alphaTest;
uniform vec4 modulateColor;
uniform float globalTime;

in vec3 uv;
out vec4 result;

void main() {
    vec4 color = texture(textureUnit, uv);
    if (alphaTest && color.w < 1.0) {
        discard;
    }
    result = color * modulateColor;
}