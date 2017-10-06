#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 mModel;
    mat4 mView;
    mat4 mProj;
} ubo;

layout(location = 0) in vec2 mesh;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 texCoords;

out gl_PerVertex
{
	vec4 gl_Position;
};

const float pi = 3.14159;

void main()
{
	float R=1.0;

	float u = 2.0*pi*mesh.x;
	float v = pi*mesh.y;
	vec3 f;

	f.x = R*sin(v)*cos(u);
	f.y = R*sin(v)*sin(u);
	f.z = R*cos(v);

  fragColor = vec3(0.0, 1.0, 0.0);
  texCoords = vec2(2.0*mesh.x, mesh.y);
  gl_Position = ubo.mProj * ubo.mView * ubo.mModel * vec4(f, 1.0);
}
