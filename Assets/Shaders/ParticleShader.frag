#version 450

layout(location = 0) in vec3 inData;
layout(location = 1) in flat float life;

layout(location = 0) out vec4 outColor;

void main()
{
    vec2 fragUV = gl_FragCoord.xy;
    vec2 circleCenter = inData.xy;

    float dist = length(fragUV - circleCenter);
    float mask = 1 - smoothstep(0, inData.z, dist);
    float normalizedLife = clamp(life / 2.01, 0.0, 1.0);

    const float power = 2;

    vec3 fireColor = mix(mix(vec3(1.0, 0.5, 0.0), vec3(1.0, 0.8, 0.0), normalizedLife),// orange to yellow
                         vec3(0.2, 0.1, 0.1),                                          // fade to dark red/brown
                         pow(1.0 - normalizedLife, power)                              // fade nonlinear
    );

    float alpha = mask * pow(normalizedLife, 1.5);
    outColor = vec4(fireColor, alpha);
}