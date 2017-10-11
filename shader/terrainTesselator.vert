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
  fragColor = vec3(1.0, 0.5, 0.0);
  texCoords = vec2(mesh.x, mesh.y);
  gl_Position = vec4(100.0*mesh.x, 0.0, 100.0*mesh.y, 1.0);
}
