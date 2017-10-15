#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform UniformBufferObject
{
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

void main()
{
  mat4 cam = inverse(ubo.mView);
  vec3 pos = vec3(-cam[3][0], -cam[3][1], -cam[3][2]);
  float u = 100.0*(mesh.x - 0.5) - floor(abs(pos.x)/2500.0)*25.0*sign(pos.x);
  float v = 100.0*(mesh.y - 0.5) - floor(abs(pos.z)/2500.0)*25.0*sign(pos.z);
  fragColor = vec3(1.0, 0.5, 0.0);
  texCoords = vec2(mesh.x, mesh.y);
  gl_Position = vec4(u, 0.0, v, 1.0);
}
