#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in flat uint textureNumber;

layout(location = 0) out vec4 outColor;
layout(set = 0, binding = 4) uniform sampler2D particleTexture;

void main() { outColor = vec4(texture(particleTexture, texCoords.xy).rgb, 1.0); }