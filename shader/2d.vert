#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject
{
  mat4 mModel;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 texCoords;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    texCoords = inTexCoords;
    gl_Position = ubo.mModel * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}
