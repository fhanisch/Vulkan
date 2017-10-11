#version 450
#extension GL_ARB_separate_shader_objects : enable

const float PI = 3.14159;
const float SEED_U = 9289.0;
const float SEED_V = 287.0;
const float scale = 10.0;

layout (binding = 0) uniform UniformBufferObject
{
  mat4 mModel;
  mat4 mView;
  mat4 mProj;
} ubo;

layout( quads ) in;

layout (location = 0) in vec3 inColor[];
layout (location = 1) in vec2 inTexCoords[];

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 texCoords;

float random2d(vec2 pos, float seed)
{
  return fract( sin( dot(pos, vec2(12.9898,78.233)) + seed) * 43758.5453123 );
}

float perlin_interp2(float u, float v)
{
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
	vec3 P[4];

  P[0]=gl_in[0].gl_Position.xyz;
	P[1]=gl_in[1].gl_Position.xyz;
  P[2]=gl_in[2].gl_Position.xyz;
  P[3]=gl_in[3].gl_Position.xyz;

	float u = gl_TessCoord.x + P[0].x;
  float v = gl_TessCoord.y + P[0].z;
  float w = 2.0*perlin_interp2(scale*u, scale*v) + 10.0*perlin_interp2(0.5*scale*u, 0.5*scale*v) +
          50.0*perlin_interp2(0.1*scale*u, 0.1*scale*v) + P[0].y;

  vec3 vertex = vec3(u,w,v);

  fragColor = inColor[0];
  texCoords.x = scale*u;
  texCoords.y = scale*v;
	gl_Position = ubo.mProj * ubo.mView * ubo.mModel * vec4(vertex,1.0);
}
