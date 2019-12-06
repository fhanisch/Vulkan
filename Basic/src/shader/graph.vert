#version 450

layout (binding = 0) uniform UniformBufferObject
{
  mat4 mModel;
  mat4 mView;
  mat4 mProj;
} ubo;

layout (location = 0) in vec2 inPosition;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = ubo.mProj * ubo.mView * ubo.mModel * vec4(inPosition, 0.0, 1.0);
}