#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform UniformBufferObject
{
  mat4 mModel;
  mat4 mView;
  mat4 mProj;
} ubo;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexCoords;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outTexCoords;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    outColor = inColor;
    outTexCoords = inTexCoords;
    gl_Position = ubo.mProj * ubo.mView * ubo.mModel * vec4(inPosition, 1.0);
}
