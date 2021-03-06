#version 450
#extension GL_ARB_separate_shader_objects : enable

const float PI = 3.14159;

layout (binding = 0) uniform UniformBufferObject
{
  mat4 mModel;
  mat4 mView;
  mat4 mProj;
} ubo;

layout (push_constant) uniform PushConstants
{
	float seed_u;
	float seed_v;
} pushConsts;

layout (location = 0) in float u;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 texCoords;

out gl_PerVertex
{
    vec4 gl_Position;
};

float random2d(vec2 pos, float seed)
{
  return fract( sin( dot(pos, vec2(12.9898,78.233)) + seed) * 43758.5453123 );
}

float perlin_interp2(float u, float v)
{
	float SEED_U = pushConsts.seed_u;
	float SEED_V = pushConsts.seed_v;
	float x = fract(u);
	float y = fract(v);
	vec2 g_00 = vec2(2.0*random2d(vec2(floor(u),floor(v)), SEED_U)-1.0, 2.0*random2d(vec2(floor(u),floor(v)), SEED_V)-1.0);
	vec2 g_10 = vec2(2.0*random2d(vec2(floor(u),floor(v)) + vec2(1.0,0.0), SEED_U)-1.0, 2.0*random2d(vec2(floor(u),floor(v)) + vec2(1.0,0.0), SEED_V)-1.0);
	vec2 g_01 = vec2(2.0*random2d(vec2(floor(u),floor(v)) + vec2(0.0,1.0), SEED_U)-1.0, 2.0*random2d(vec2(floor(u),floor(v)) + vec2(0.0,1.0), SEED_V)-1.0);
	vec2 g_11 = vec2(2.0*random2d(vec2(floor(u),floor(v)) + vec2(1.0,1.0), SEED_U)-1.0, 2.0*random2d(vec2(floor(u),floor(v)) + vec2(1.0,1.0), SEED_V)-1.0);
	float w_00 = dot(g_00, vec2(x,y));
	float w_10 = dot(g_10, vec2(x-1.0,y));
	float w_01 = dot(g_01, vec2(x,y-1.0));
	float w_11 = dot(g_11, vec2(x-1.0,y-1.0));
	float s_x = 10.0*pow(x,3.0)-15.0*pow(x,4.0)+6.0*pow(x,5.0);
	float s_y = 10.0*pow(y,3.0)-15.0*pow(y,4.0)+6.0*pow(y,5.0);
	float w_0 = (1.0-s_x)*w_00 + s_x*w_10;
	float w_1 = (1.0-s_x)*w_01 + s_x*w_11;
	float w = (1.0-s_y)*w_0 + s_y*w_1;
	return w;
}

void main()
{
    float x,y;

    x = cos(u*2.0*PI);
    y = -sin(u*2.0*PI);

	float dr = 0.8*perlin_interp2(x, y) + 0.2*perlin_interp2(4.0*x, 4.0*y);

	x=(1+dr)*x;
	y=(1+dr)*y;

    fragColor = vec3(0.0, 1.0, 0.0);
    texCoords = vec2(0.0, 0.0);
    gl_Position = ubo.mProj * ubo.mView * ubo.mModel * vec4(x, y, 0.0, 1.0);
}