#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
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

const float pi = 3.14159;
const float SEED_U = 5289.0;
const float SEED_V = 987.0;
const float SEED_W = 23.0;

float random3d(vec3 pos, float seed)
{
  return fract( sin( dot(pos, vec3(12.9898,78.233,189.39581)) + seed) * 43758.5453123 );
}

float perlin_interp3(float u, float v, float w)
{
    float x = fract(u);
    float y = fract(v);
    float z = fract(w);

    vec3 g_000 = vec3(2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(0.0,0.0,0.0), SEED_U)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(0.0,0.0,0.0), SEED_V)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(0.0,0.0,0.0), SEED_W)-1.0);
    vec3 g_100 = vec3(2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(1.0,0.0,0.0), SEED_U)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(1.0,0.0,0.0), SEED_V)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(1.0,0.0,0.0), SEED_W)-1.0);
    vec3 g_010 = vec3(2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(0.0,1.0,0.0), SEED_U)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(0.0,1.0,0.0), SEED_V)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(0.0,1.0,0.0), SEED_W)-1.0);
    vec3 g_110 = vec3(2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(1.0,1.0,0.0), SEED_U)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(1.0,1.0,0.0), SEED_V)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(1.0,1.0,0.0), SEED_W)-1.0);
    vec3 g_001 = vec3(2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(0.0,0.0,1.0), SEED_U)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(0.0,0.0,1.0), SEED_V)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(0.0,0.0,1.0), SEED_W)-1.0);
    vec3 g_101 = vec3(2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(1.0,0.0,1.0), SEED_U)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(1.0,0.0,1.0), SEED_V)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(1.0,0.0,1.0), SEED_W)-1.0);
    vec3 g_011 = vec3(2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(0.0,1.0,1.0), SEED_U)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(0.0,1.0,1.0), SEED_V)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(0.0,1.0,1.0), SEED_W)-1.0);
    vec3 g_111 = vec3(2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(1.0,1.0,1.0), SEED_U)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(1.0,1.0,1.0), SEED_V)-1.0, 2.0*random3d(vec3(floor(u),floor(v),floor(w)) + vec3(1.0,1.0,1.0), SEED_W)-1.0);

    float w_000 = dot(g_000, vec3(x,y,z));
    float w_100 = dot(g_100, vec3(x-1.0,y,z));
    float w_010 = dot(g_010, vec3(x,y-1.0,z));
    float w_110 = dot(g_110, vec3(x-1.0,y-1.0,z));
    float w_001 = dot(g_001, vec3(x,y,z-1.0));
    float w_101 = dot(g_101, vec3(x-1.0,y,z-1.0));
    float w_011 = dot(g_011, vec3(x,y-1.0,z-1.0));
    float w_111 = dot(g_111, vec3(x-1.0,y-1.0,z-1.0));

    float s_x = 10.0*pow(x,3.0)-15.0*pow(x,4.0)+6.0*pow(x,5.0);
    float s_y = 10.0*pow(y,3.0)-15.0*pow(y,4.0)+6.0*pow(y,5.0);
    float s_z = 10.0*pow(z,3.0)-15.0*pow(z,4.0)+6.0*pow(z,5.0);

    float w_00 = (1.0-s_x)*w_000 + s_x*w_100;
    float w_10 = (1.0-s_x)*w_010 + s_x*w_110;
    float w_01 = (1.0-s_x)*w_001 + s_x*w_101;
    float w_11 = (1.0-s_x)*w_011 + s_x*w_111;

    float w_0 = (1.0-s_y)*w_00 + s_y*w_10;
    float w_1 = (1.0-s_y)*w_01 + s_y*w_11;

		return (1.0-s_z)*w_0 + s_z*w_1;
}

void main()
{
	float R=2.0;

	float u = 2.0*pi*mesh.x;
	float v = pi*mesh.y;
	vec3 f;

	f.x = R*sin(v)*cos(u);
	f.y = R*sin(v)*sin(u);
	f.z = R*cos(v);

  float dr = 0.1*perlin_interp3(2.0*(f.x+1.0),2.0*(f.y+1.0),2.0*(f.z+1.0)) + 0.05*perlin_interp3(4.0*(f.x+1.0),4.0*(f.y+1.0),4.0*(f.z+1.0));

  f.x*=(1.0+dr);
  f.y*=(1.0+dr);
  f.z*=(1.0+dr);

  fragColor = vec3(1.0, 1.0, 0.0);
  texCoords = vec2(16.0*mesh.x, 8.0*mesh.y);
  gl_Position = ubo.mProj * ubo.mView * ubo.mModel * vec4(f, 1.0);
}
