#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in flat uint textureNumber;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2DArray textureArray;

void main() { outColor = texture(textureArray, vec3(texCoords.xy, textureNumber)); }