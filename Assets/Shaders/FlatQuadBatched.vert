#version 450 core

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform UniformBuffer
{
    mat4 model;
    mat4 view;
    mat4 projection;
    float zoom;
}

ubo;

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

    vec2 vertices[6] = {vec2(pos.x, pos.y),
                        vec2(pos.x + size.x, pos.y),
                        vec2(pos.x + size.x, pos.y + size.y),
                        vec2(pos.x + size.x, pos.y + size.y),
                        vec2(pos.x, pos.y + size.y),
                        vec2(pos.x, pos.y)};

    return vec2(vertices[vertexID]);
}

void main()
{
    uint index = getCurrentQuadIndex();
    vec4 outPosition = ubo.projection * ubo.view * ubo.model * vec4(getVertex(getVertexID(), index), 0.0, 1.0);
    gl_Position = outPosition;
    fragColor = vec3(quads[index].color.rgb);
}