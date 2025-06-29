#version 450 core

layout(location = 0) out vec3 outData;
layout(location = 1) out flat float life;

layout(binding = 0) uniform UniformBuffer
{
    mat4 model;
    mat4 view;
    mat4 projection;
    float zoom;
}

ubo;

layout(binding = 1) readonly buffer positionBuffer { vec2 positions[]; };

layout(binding = 2) readonly buffer velocityBuffer { vec2 velocities[]; };

layout(binding = 3) readonly buffer lifeBuffer { float lifes[]; };

layout(binding = 4) readonly buffer aliveParticles { uint aliveParticlesID[]; };

uint getVertexID() { return gl_VertexIndex % 6u; }

uint getCurrentQuadIndex() { return aliveParticlesID[gl_InstanceIndex]; }

vec2 getVertex(uint vertexID, uint quadIndex, float particleSize)
{
    vec2 pos = positions[quadIndex].xy;
    vec2 size = vec2(particleSize, particleSize);

    vec2 vertices[6] = {vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(1.0, 1.0),
                        vec2(1.0, 1.0),   vec2(-1.0, 1.0), vec2(-1.0, -1.0)};

    return pos + vertices[vertexID] * size;
}

void main()
{
    float particleSize = 40;
    uint index = getCurrentQuadIndex();

    mat4 PVM = ubo.projection * ubo.view * ubo.model;
    vec4 outPosition = PVM * vec4(getVertex(getVertexID(), index, particleSize), 0.0, 1.0);
    gl_Position = outPosition;

    vec2 pos = positions[index].xy;
    vec4 clipPos = PVM * vec4(pos, 0.0, 1.0);

    vec2 resolution = vec2(1280, 720);

    vec2 screen = (clipPos.xy * 0.5 + 0.5) * resolution;

    outData = vec3(screen, particleSize / ubo.zoom);
    life = lifes[index];
}