#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform UniformBuffer { vec3 offset; }

ubo;

void main()
{
    vec2 offset = gl_InstanceIndex * vec2(0.2, 0.2) + ubo.offset.xy;
    gl_Position = vec4(offset + inPos, 0.0, 1.0);
    fragColor = inColor + vec3(offset, 0.0);
}