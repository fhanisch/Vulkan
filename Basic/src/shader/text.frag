#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec2 inUV;

layout (binding = 2) uniform sampler2D samplerFont;

layout (location = 0) out vec4 outFragColor;

void main(void)
{
	vec4 color = vec4(0.0);
	float g = texture(samplerFont, inUV).r;

	if (g>0.5) color = vec4(0.0, 1.0, 0.0, 1.0);
	outFragColor = color;
}