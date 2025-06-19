#version 450 core

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform UniformBuffer
{
    mat4 model;
    mat4 view;
    mat4 projection;
}ubo;

struct Quad {
    vec4 position;
    vec4 color;
};

layout(binding = 1) readonly buffer quadBuffer { Quad quads[]; };

uint getVertexID() { return gl_VertexIndex % 6u; }

uint getCurrentQuadIndex() { return gl_InstanceIndex; }

vec2 getVertex(uint vertexID, uint quadIndex)
{
    vec2 coords;
    vec2 pos = quads[quadIndex].position.xy;
    vec2 size = quads[quadIndex].position.zw;
    vec2 vertices[6] = {vec2(0, 0),
                        vec2(1, 0),
                        vec2(1, 1),
                        vec2(1, 1),
                        vec2(0, 1),
                        vec2(0,0)};
    vec2 worldPos = pos +  vertices[vertexID] * size;
    return worldPos;
}

void main()
{
    uint index = getCurrentQuadIndex();
    vec4 outPosition = ubo.projection * ubo.view * ubo.model * vec4(getVertex(getVertexID(), index), 0.0, 1.0);
    gl_Position = outPosition;
    fragColor = vec3(quads[index].color.rgb);
}