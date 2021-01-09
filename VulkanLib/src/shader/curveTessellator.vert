#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 2) uniform UniformBufferObject
{
	vec4 myColor;
} c;

layout (location = 0) in float u;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 texCoords;

void main() {
    fragColor = vec3(c.myColor);
    texCoords = vec2(0.0, 0.0);
    gl_Position = vec4(u, 0.0, 0.0, 1.0);
}