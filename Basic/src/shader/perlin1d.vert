#version 450
#extension GL_ARB_separate_shader_objects : enable

const float PI = 3.14159;
const float SEED = 2634.0;

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

float random(float p, float seed)
{
  return fract( sin(p + seed) * 43758.5453123 );
}

float perlin_interp(float u)
{
    float x = fract(u);
    float g_0 = 2.0 * random( floor(u), SEED ) - 1.0;
    float g_1 = 2.0 * random( floor(u) + 1.0, SEED ) - 1.0;
    float w_0 = g_0*x;
    float w_1 = g_1*(x-1.0);
    float s_x = 10.0*pow(x,3.0) - 15.0*pow(x,4.0) + 6.0*pow(x,5.0);
    float w = (1.0-s_x)*w_0 + s_x*w_1;
    return w;
}

void main()
{
    float x,y;

    x = u;
    y = 4.0*perlin_interp(2.0*x) + 2.0*perlin_interp(5.0*x) + 0.25*perlin_interp(20.0*x);

    fragColor = vec3(1.0, 0.0, 1.0);
    texCoords = vec2(0.0, 0.0);
    gl_Position = ubo.mProj * ubo.mView * ubo.mModel * vec4(x, y, 0.0, 1.0);
}
