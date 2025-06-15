#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform UniformBuffer
{
    mat4 model;
    mat4 view;
    mat4 projection;
}ubo;

void main()
{
    vec2 offset = gl_InstanceIndex * vec2(0.2, 0.2);
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(offset + inPos, 0.0, 1.0);
    fragColor = inColor + vec3(offset, 0.0);
}