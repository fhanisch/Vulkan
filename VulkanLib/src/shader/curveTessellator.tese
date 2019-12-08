#version 450
#extension GL_ARB_separate_shader_objects : enable

const float PI = 3.14159;

layout (binding = 0) uniform UniformBufferObject
{
	mat4 mModel;
	mat4 mView;
	mat4 mProj;
} ubo;

layout( isolines ) in;

layout (location = 0) in vec3 inColor[];
layout (location = 1) in vec2 inTexCoords[];

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 texCoords;

void main()
{
	vec2 P[2];

	P[0]=gl_in[0].gl_Position.xy;
	P[1]=gl_in[1].gl_Position.xy;

	float x = gl_TessCoord.x + P[0].x;
	float y = -0.5*sin(2.0*PI*x) + P[0].y;

	fragColor = mix(inColor[0], inColor[1], x);
	texCoords = mix(inTexCoords[0], inTexCoords[1], x);
	gl_Position = ubo.mProj * ubo.mView * ubo.mModel * vec4(x,y,0.0,1.0);
}