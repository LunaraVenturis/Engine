#version 450 core

layout(location = 0) out vec3 outData;
layout(location = 1) out vec3 outData2;
layout(location = 2) out flat float life;

layout(set = 0, binding = 0) uniform UniformBuffer
{
    mat4 model;
    mat4 view;
    mat4 projection;
    float zoom;
}

ubo;

layout(set = 0, binding = 1) readonly buffer positionBuffer { vec2 positions[]; };

layout(set = 0, binding = 2) readonly buffer lifeBuffer { float lifes[]; };

layout(set = 0, binding = 3) readonly buffer aliveParticles { uint aliveParticlesID[]; };

uint getVertexID() { return gl_VertexIndex % 6u; }

uint getCurrentQuadIndex() { return aliveParticlesID[gl_InstanceIndex]; }

vec2 getVertex(uint vertexID, uint quadIndex, float particleSize)
{
    vec2 pos = positions[quadIndex].xy;
    vec2 size = vec2(particleSize, particleSize);

    vec2 vertices[6] = {
            vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(1.0, 1.0), vec2(1.0, 1.0), vec2(-1.0, 1.0), vec2(-1.0, -1.0)};

    return pos + vertices[vertexID] * size;
}

vec2 getTextureCoord(uint vertexID)
{
    vec2 vertices[6] = {vec2(0, 0), vec2(1.0, 0), vec2(1.0, 1.0), vec2(1.0, 1.0), vec2(0, 1.0), vec2(0, 0)};

    return vertices[vertexID];
}

void main()
{
    float particleSize = 100;
    uint index = getCurrentQuadIndex();

    mat4 PVM = ubo.projection * ubo.view * ubo.model;
    vec4 outPosition = PVM * vec4(getVertex(getVertexID(), index, particleSize), 0.0, 1.0);
    gl_Position = outPosition;

    vec2 pos = positions[index].xy;
    vec4 clipPos = PVM * vec4(pos, 0.0, 1.0);

    vec2 resolution = vec2(1280, 720);

    vec2 screen = (clipPos.xy * 0.5 + 0.5) * resolution;

    outData = vec3(screen, particleSize / ubo.zoom);
    outData2.xy = getTextureCoord(getVertexID());
    life = lifes[index];
}