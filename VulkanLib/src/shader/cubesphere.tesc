#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform UniformBufferObject {
	mat4 mModel;
	mat4 mView;
	mat4 mProj;
} ubo;

layout( vertices=4 ) out;

layout (location = 0) in vec3 inNormal[];
layout (location = 1) in vec3 inFragColor[];
layout (location = 2) in vec2 inTexCoords[];

layout (location = 0) out vec3 outNormal[4];
layout (location = 1) out vec3 outFragColor[4];
layout (location = 2) out vec2 outTexCoords[4];

void main() {
	
	float grid = 20.0;

	if (gl_InvocationID == 0) { // wird nur beim ersten Durchlauf gesetzt
        
		gl_TessLevelOuter[0] = grid;
		gl_TessLevelOuter[1] = grid;
		gl_TessLevelOuter[2] = grid;
		gl_TessLevelOuter[3] = grid;
		gl_TessLevelInner[0] = grid;
		gl_TessLevelInner[1] = grid;
	}

	outNormal[gl_InvocationID] = inNormal[gl_InvocationID];
	outFragColor[gl_InvocationID] = inFragColor[gl_InvocationID];
	outTexCoords[gl_InvocationID] = inTexCoords[gl_InvocationID];
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}