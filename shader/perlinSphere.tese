#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
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

/*
out gl_PerVertex
{
	vec4 gl_Position;
};
*/

const float pi = 3.14159;
const float SEED_U = 4289.0;
const float SEED_V = 987.0;
const float SEED_W = 173.0;

float random3d(vec3 pos, float seed)
{
  return fract( sin( dot(pos, vec3(12.9898,78.233,189.39581)) + seed) * 43758.5453123 );
}

float perlin_interp3(out vec3 n, float u, float v, float w)
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

    // Überblendungsfunktion: s(x) = 10x^3-15x^4+6x^5
    float s_x = pow(x,3.0)*(x*(x*6-15)+10);
    float s_y = pow(y,3.0)*(y*(y*6-15)+10);
    float s_z = pow(z,3.0)*(z*(z*6-15)+10);
    float derx_s_x = pow(x,2.0)*(x*(x*30-60)+30);
    float derz_s_z = pow(z,2.0)*(z*(z*30-60)+30);

    float w_00 = (1.0-s_x)*w_000 + s_x*w_100;
    float derx_w_00 = (1.0-s_x)*g_000.x - derx_s_x*w_000 + s_x*g_100.x + derx_s_x*w_100;
    float derz_w_00 = (1.0-s_x)*g_000.z + s_x*g_100.z;
    float w_10 = (1.0-s_x)*w_010 + s_x*w_110;
    float derx_w_10 = (1.0-s_x)*g_010.x - derx_s_x*w_010 + s_x*g_110.x + derx_s_x*w_110;
    float derz_w_10 = (1.0-s_x)*g_010.z + s_x*g_110.z;
    float w_01 = (1.0-s_x)*w_001 + s_x*w_101;
    float derx_w_01 = (1.0-s_x)*g_001.x - derx_s_x*w_001 + s_x*g_101.x + derx_s_x*w_101;
    float derz_w_01 = (1.0-s_x)*g_001.z + s_x*g_101.z;
    float w_11 = (1.0-s_x)*w_011 + s_x*w_111;
    float derx_w_11 = (1.0-s_x)*g_011.x - derx_s_x*w_011 + s_x*g_111.x + derx_s_x*w_111;
    float derz_w_11 = (1.0-s_x)*g_011.z + s_x*g_111.z;

    float w_0 = (1.0-s_y)*w_00 + s_y*w_10;
    float derx_w_0 = (1.0-s_y)*derx_w_00 + s_y*derx_w_10;
    float derz_w_0 = (1.0-s_y)*derz_w_00 + s_y*derz_w_10;
    float w_1 = (1.0-s_y)*w_01 + s_y*w_11;
    float derx_w_1 = (1.0-s_y)*derx_w_01 + s_y*derx_w_11;
    float derz_w_1 = (1.0-s_y)*derz_w_01 + s_y*derz_w_11;

    float derx_w = (1.0-s_z)*derx_w_0 + s_z*derx_w_1;
    float derz_w = (1.0-s_z)*derz_w_0 - derz_s_z*w_0 + s_z*derz_w_1 + derz_s_z*w_1;
    n = -cross(vec3(1.0,derx_w,0.0),vec3(0.0,derz_w,1.0));
    return (1.0-s_z)*w_0 + s_z*w_1;
}

void main()
{
    vec3 P[4];
	float R=500.0;

    P[0]=gl_in[0].gl_Position.xyz;
	P[1]=gl_in[1].gl_Position.xyz;
    P[2]=gl_in[2].gl_Position.xyz;
    P[3]=gl_in[3].gl_Position.xyz;

    float pu = P[0].x + gl_TessCoord.x*(P[1].x-P[0].x);
    float pv = P[0].z + gl_TessCoord.y*(P[2].z-P[0].z);
	float u = 2.0*pi*pu;
	float v = pi*pv;
	vec3 f,n,n_1,n_2;

    f.x = R*sin(v)*cos(u);
	f.y = R*sin(v)*sin(u);
	f.z = R*cos(v);

	n = f;

    //float dr = 0.1*perlin_interp3(2.0*(f.x+1.0),2.0*(f.y+1.0),2.0*(f.z+1.0)) + 0.05*perlin_interp3(4.0*(f.x+1.0),4.0*(f.y+1.0),4.0*(f.z+1.0));
    float dr = 0.1*perlin_interp3(n_1,0.02*(f.x+1.0),0.02*(f.y+1.0),0.02*(f.z+1.0)) + 0.01*perlin_interp3(n_2,0.125*(f.x+1.0),0.125*(f.y+1.0),0.125*(f.z+1.0));

    n_1.x*=100;
    n_1.z*=100;
    n_2.x*=100;
    n_2.z*=100;
    n+=n_1+n_2;

    f*=1.0+dr;

    fragColor = vec3(1.0, 1.0, 0.0);
    texCoords = vec2(256.0*pu, 128.0*pv);
    vertexPosition = vec3(ubo.mView * ubo.mModel * vec4(f, 1.0));
    normalPosition = vec3(transpose(inverse(ubo.mView * ubo.mModel)) * vec4(normalize(n), 1.0));
	gl_Position = ubo.mProj * vec4(vertexPosition, 1.0);
}
