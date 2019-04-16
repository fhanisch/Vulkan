#version 450
#extension GL_ARB_separate_shader_objects : enable

const float PI = 3.14159;

layout (binding = 0) uniform UniformBufferObject
{
  mat4 mModel;
  mat4 mView;
  mat4 mProj;
} ubo;

layout (binding = 1) uniform UniformBufferObject2
{
	float time;
} T;

layout (location = 0) in float u;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 texCoords;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	float x,y;
	float t=T.time/2.0;

    x = u;
    y = 1.0/10.0*sin(2.0*PI*t)*sin(2.0*PI*(t-4.0*x));

    fragColor = vec3(1.0, 0.0, 0.0);
    texCoords = vec2(0.0, 0.0);
    gl_Position = ubo.mProj * ubo.mView * ubo.mModel * vec4(x, y, 0.0, 1.0);
}