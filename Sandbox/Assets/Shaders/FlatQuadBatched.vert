#version 450 core

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 outTexCoords;
layout(location = 2) out flat uint textureNumber;

layout(binding = 0) uniform UniformBuffer
{
    mat4 model;
    mat4 view;
    mat4 projection;
    float zoom;
}

ubo;

layout(binding = 1) readonly buffer positionBuffer { vec3 positions[]; };

layout(binding = 2) readonly buffer sizeBuffer { vec2 sizes[]; };

layout(binding = 3) readonly buffer textureIndexBuffer { uint textureIndices[]; };

uint getVertexID() { return gl_VertexIndex % 6u; }

uint getCurrentQuadIndex() { return gl_InstanceIndex; }

vec2 getVertex(uint vertexID, uint quadIndex)
{
    vec2 coords;
    vec2 pos = positions[quadIndex].xy;
    vec2 size = sizes[quadIndex];

    vec2 vertices[6] = {vec2(pos.x, pos.y),
                        vec2(pos.x + size.x, pos.y),
                        vec2(pos.x + size.x, pos.y + size.y),
                        vec2(pos.x + size.x, pos.y + size.y),
                        vec2(pos.x, pos.y + size.y),
                        vec2(pos.x, pos.y)};

    return vec2(vertices[vertexID]);
}

vec2 getTextureCoord(uint vertexID)
{
    vec2 vertices[6] = {vec2(0, 0), vec2(1.0, 0), vec2(1.0, 1.0), vec2(1.0, 1.0), vec2(0, 1.0), vec2(0, 0)};

    return vertices[vertexID];
}

void main()
{
    uint index = getCurrentQuadIndex();
    vec4 outPosition = ubo.projection * ubo.view * ubo.model * vec4(getVertex(getVertexID(), index), 0.0, 1.0);
    gl_Position = outPosition;
    fragColor = vec3(0, 0, 0);
    outTexCoords = getTextureCoord(getVertexID());
    textureNumber = uint(textureIndices[index]);
}