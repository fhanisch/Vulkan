#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 mModel;
    mat4 mView;
    mat4 mProj;
} ubo;

layout( quads ) in;

layout (location = 0) in vec3 inNormal[];
layout (location = 1) in vec3 inFragColor[];
layout (location = 2) in vec2 inTexCoords[];

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 texCoords;
layout(location = 2) out vec3 vertexPosition;
layout(location = 3) out vec3 normalPosition;

const float pi = 3.14159;
const float SEED_U = 4289.0;
const float SEED_V = 987.0;
const float SEED_W = 173.0;

float random3d(vec3 pos, float seed)
{
  return fract( sin( dot(pos, vec3(12.9898,78.233,189.39581)) + seed) * 43758.5453123 );
}

float perlin_interp3(out vec3 n, vec3 r)
{
    float x = fract(r.x);
    float y = fract(r.y);
    float z = fract(r.z);

	float u = r.x;
    float v = r.y;
    float w = r.z;	

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

void main() {

	//vec3 P[4];
	//P[0]=gl_in[0].gl_Position.xyz;
	//P[1]=gl_in[1].gl_Position.xyz;
    //P[2]=gl_in[2].gl_Position.xyz;
    //P[3]=gl_in[3].gl_Position.xyz;

	float R=1.0;
	vec3 n1, n2, r, n, norm1, norm2, norm3;

	float phi = pi/4.0 - pi/2.0 * gl_TessCoord.x;
	float theta = -pi/4.0 + pi/2.0 * gl_TessCoord.y;

	mat3 Rx = mat3(	1.0, 0.0, 0.0,
					0.0, cos(phi), -sin(phi),
					0.0, sin(phi), cos(phi));

	mat3 Rx_ = mat3(1.0, 0.0, 0.0,
					0.0, cos(theta), -sin(theta),
					0.0, sin(theta), cos(theta));
	
	mat3 Ry = mat3(	cos(theta), 0.0, sin(theta),					
	 				0.0, 1.0, 0.0,
					-sin(theta), 0.0, cos(theta));

	mat3 Rz = mat3(	cos(phi), -sin(phi), 0.0,
					sin(phi), cos(phi), 0.0,
	 				0.0, 0.0, 1.0);	

	//n1 = vec3(-sin(phi), cos(phi), 0.0);
	//n2 = vec3(-sin(theta), 0.0, -cos(theta));
	if (inNormal[1].x == 0.0 && inNormal[2].x == 0.0) {
		n1 = transpose(Rz) * inNormal[0];
	}
	else {
		n1 = transpose(Rx) * inNormal[0];
	}
	if (inNormal[0].z == 0.0 && inNormal[2].x == 0.0) {
		n2 = transpose(Ry) * inNormal[1];
	}
	
	if (inNormal[2].x != 0.0) {
		n1 = transpose(Rz) * inNormal[2];
		n2 = transpose(Rx_) * inNormal[3];
	}

	r = -normalize(cross(n1,n2));
	n = r;

	r *= 2.0 + 1.0*perlin_interp3(norm1, 1.0*r) + 0.5*perlin_interp3(norm2, 2.0*r) + 0.25*perlin_interp3(norm3, 4.0*r);
	n += normalize(norm1 + norm2 + norm3);

	fragColor = vec3(0.0, 1.0, 0.0);
	texCoords = vec2(gl_TessCoord.x, gl_TessCoord.y);
	vertexPosition = vec3(ubo.mView * ubo.mModel * vec4(r, 1.0));
	normalPosition = vec3(transpose(inverse(ubo.mView * ubo.mModel)) * vec4(n, 1.0));
	gl_Position = ubo.mProj * vec4(vertexPosition, 1.0);
}