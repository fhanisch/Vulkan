#version 450
#extension GL_ARB_separate_shader_objects : enable

const float PI = 3.14159;
const float SEED_U = 5289.0;
const float SEED_V = 987.0;

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
layout(location = 2) out vec3 vertexPosition;
layout(location = 3) out vec3 normalPosition;

float random2d(vec2 pos, float seed)
{
  return fract( sin( dot(pos, vec2(12.9898,78.233)) + seed) * 43758.5453123 );
}

float perlin_interp2(out vec3 n, float u, float v)
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
    // Überblendungsfunktion: s(x) = 10x^3-15x^4+6x^5
    float s_x = pow(x,3.0)*(x*(x*6-15)+10);
    float s_y = pow(y,3.0)*(y*(y*6-15)+10);
    float derx_s_x = pow(x,2.0)*(x*(x*30-60)+30);
    float dery_s_y = pow(y,2.0)*(y*(y*30-60)+30);
    float w_0 = (1.0-s_x)*w_00 + s_x*w_10;
    float derx_w_0 = (1.0-s_x)*g_00.x - derx_s_x*w_00 + s_x*g_10.x + derx_s_x*w_10;
    //float derx_w_0 = g_00.x + derx_s_x*(w_10-w_00) + s_x*(g_10.x-g_00.x);
    float dery_w_0 = (1.0-s_x)*g_00.y + s_x*g_10.y;
    float w_1 = (1.0-s_x)*w_01 + s_x*w_11;
    float derx_w_1 = (1.0-s_x)*g_01.x - derx_s_x*w_01 + s_x*g_11.x + derx_s_x*w_11;
    //float derx_w_1 = g_01.x + derx_s_y*(w_11-w_01) + s_y*(g_11.x-g_01.x);
    float dery_w_1 = (1.0-s_x)*g_01.y + s_x*g_11.y;
    float w = (1.0-s_y)*w_0 + s_y*w_1;
    float derx_w = (1.0-s_y)*derx_w_0 + s_y*derx_w_1;
    float dery_w = (1.0-s_y)*dery_w_0 - dery_s_y*w_0 + s_y*dery_w_1 + dery_s_y*w_1;
    n = -cross(vec3(1.0,derx_w,0.0),vec3(0.0,dery_w,1.0));
		return w;
}

void main()
{
	vec3 P[4];
  vec3 n,n_1,n_2,n_3;

  P[0]=gl_in[0].gl_Position.xyz;
	P[1]=gl_in[1].gl_Position.xyz;
  P[2]=gl_in[2].gl_Position.xyz;
  P[3]=gl_in[3].gl_Position.xyz;

	float u = gl_TessCoord.x + P[0].x;
  float v = gl_TessCoord.y + P[0].z;
  float w = 50.0*perlin_interp2(n_1, u, v) + 10.0*perlin_interp2(n_2, 5.0*u, 5.0*v) + 2.0*perlin_interp2(n_3, 10.0*u, 10.0*v);
  vec3 vertex = vec3(u,w,v);
  n_1.x*=50.0;
  n_1.z*=50.0;
  n_2.x*=100.0;
  n_2.z*=100.0;
  n_3.x*=40.0;
  n_3.z*=40.0;
  n = n_1 + n_2 + n_3;
  fragColor = inColor[0];
  texCoords.x = 10.0*u;
  texCoords.y = 10.0*v;
  vertexPosition = vec3(ubo.mView * ubo.mModel * vec4(vertex, 1.0));
  normalPosition = vec3(transpose(inverse(ubo.mView * ubo.mModel)) * vec4(normalize(n), 1.0));
	gl_Position = ubo.mProj * vec4(vertexPosition, 1.0);
}
