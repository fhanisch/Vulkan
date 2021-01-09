#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 mModel;
    mat4 mView;
    mat4 mProj;
} ubo;

layout( quads ) in;

layout(location = 0) out vec3 vertexPosition;
layout(location = 1) out vec3 normalPosition;

void bezier_spline(out float B[4], out float dB[4], float t) {
	float omt=1.0-t;

	B[0]=omt*omt*omt;
	B[1]=3.0*omt*omt*t;
	B[2]=3.0*omt*t*t;
	B[3]=t*t*t;

	dB[0]=-3.0*omt*omt;
	dB[1]=-6.0*t*omt+3.0*omt*omt;
	dB[2]=-3.0*t*t+6.0*t*omt;
	dB[3]=3.0*t*t;
}

void main() {
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec3 vNormal;

	vec3 p00 = gl_in[0].gl_Position.xyz;
	vec3 p01 = gl_in[1].gl_Position.xyz;
	vec3 p02 = gl_in[2].gl_Position.xyz;
	vec3 p03 = gl_in[3].gl_Position.xyz;
	vec3 p10 = gl_in[4].gl_Position.xyz;
	vec3 p11 = gl_in[5].gl_Position.xyz;
	vec3 p12 = gl_in[6].gl_Position.xyz;
	vec3 p13 = gl_in[7].gl_Position.xyz;
	vec3 p20 = gl_in[8].gl_Position.xyz;
	vec3 p21 = gl_in[9].gl_Position.xyz;
	vec3 p22 = gl_in[10].gl_Position.xyz;
	vec3 p23 = gl_in[11].gl_Position.xyz;
	vec3 p30 = gl_in[12].gl_Position.xyz;
	vec3 p31 = gl_in[13].gl_Position.xyz;
	vec3 p32 = gl_in[14].gl_Position.xyz;
	vec3 p33 = gl_in[15].gl_Position.xyz;

	float bu[4], bv[4];
	float dbu[4], dbv[4];
	bezier_spline(bu,dbu,u);
	bezier_spline(bv,dbv,v);

	vec3 pos =	p00*bu[0]*bv[0] + p01*bu[0]*bv[1] + p02*bu[0]*bv[2] +
				p03*bu[0]*bv[3] +
				p10*bu[1]*bv[0] + p11*bu[1]*bv[1] + p12*bu[1]*bv[2] +
				p13*bu[1]*bv[3] +
				p20*bu[2]*bv[0] + p21*bu[2]*bv[1] + p22*bu[2]*bv[2] +
				p23*bu[2]*bv[3] +
				p30*bu[3]*bv[0] + p31*bu[3]*bv[1] + p32*bu[3]*bv[2] +
				p33*bu[3]*bv[3];

	vec3 du =	p00*dbu[0]*bv[0] + p01*dbu[0]*bv[1] + p02*dbu[0]*bv[2] +
				p03*dbu[0]*bv[3] +
				p10*dbu[1]*bv[0] + p11*dbu[1]*bv[1] + p12*dbu[1]*bv[2] +
				p13*dbu[1]*bv[3] +
				p20*dbu[2]*bv[0] + p21*dbu[2]*bv[1] + p22*dbu[2]*bv[2] +
				p23*dbu[2]*bv[3] +
				p30*dbu[3]*bv[0] + p31*dbu[3]*bv[1] + p32*dbu[3]*bv[2] +
				p33*dbu[3]*bv[3];

	vec3 dv =	p00*bu[0]*dbv[0] + p01*bu[0]*dbv[1] + p02*bu[0]*dbv[2] +
				p03*bu[0]*dbv[3] +
				p10*bu[1]*dbv[0] + p11*bu[1]*dbv[1] + p12*bu[1]*dbv[2] +
				p13*bu[1]*dbv[3] +
				p20*bu[2]*dbv[0] + p21*bu[2]*dbv[1] + p22*bu[2]*dbv[2] +
				p23*bu[2]*dbv[3] +
				p30*bu[3]*dbv[0] + p31*bu[3]*dbv[1] + p32*bu[3]*dbv[2] +
				p33*bu[3]*dbv[3];

	vNormal =	-cross(du,dv);
	
	vertexPosition = vec3(ubo.mView * ubo.mModel * vec4(pos, 1.0));	
	normalPosition = vec3(transpose(inverse(ubo.mView * ubo.mModel)) * vec4(vNormal, 1.0));
	gl_Position = ubo.mProj * vec4(vertexPosition, 1.0);
}