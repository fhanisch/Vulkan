#version 450
#extension GL_ARB_separate_shader_objects : enable

const float PI = 3.14159;
const float SEED = 298.0;

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
	vec2 P[2];

  P[0]=gl_in[0].gl_Position.xy;
	P[1]=gl_in[1].gl_Position.xy;

	float x = gl_TessCoord.x + P[0].x;
  float y = 2.0*perlin_interp(2.0*x) + 1.0*perlin_interp(5.0*x) + 0.125*perlin_interp(20.0*x) + P[0].y;

  fragColor = mix(inColor[0], inColor[1], x);
  texCoords = mix(inTexCoords[0], inTexCoords[1], x);
	gl_Position = ubo.mProj * ubo.mView * ubo.mModel * vec4(x,y,0.0,1.0);
}
