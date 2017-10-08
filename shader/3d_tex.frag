#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 texCoords;

layout (location = 0) out vec4 outColor;

void main()
{
    float u = texCoords.x;
    float v = texCoords.y;
    vec4 texColor = texture(texSampler, vec2(u,v));
    outColor = texColor;
}
