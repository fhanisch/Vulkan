#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform UniformBufferObject
{
  mat4 mModel;
  mat4 mView;
  mat4 mProj;
} ubo;

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

    x = cos(u*2.0*3.14159);
    y = -sin(u*2.0*3.14159);

    fragColor = vec3(0.0, 1.0, 0.0);
    texCoords = vec2(0.0, 0.0);
    gl_Position = ubo.mProj * ubo.mView * ubo.mModel * vec4(x, y, 0.0, 1.0);
}
