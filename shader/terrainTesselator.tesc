#version 450
#extension GL_ARB_separate_shader_objects : enable

layout( vertices=4 ) out;

layout (location = 0) in vec3 inColor[];
layout (location = 1) in vec2 inTexCoords[];

layout (location = 0) out vec3 fragColor[4];
layout (location = 1) out vec2 texCoords[4];

const float grid = 100.0;

void main()
{
	if (gl_InvocationID==0) // wird nur beim ersten Durchlauf gesetzt
	{
		gl_TessLevelOuter[0] = grid;
		gl_TessLevelOuter[1] = grid;
		gl_TessLevelOuter[2] = grid;
		gl_TessLevelOuter[3] = grid;
		gl_TessLevelInner[0] = grid;
		gl_TessLevelInner[1] = grid;
	}

	fragColor[gl_InvocationID] = inColor[gl_InvocationID];
	texCoords[gl_InvocationID] = inTexCoords[gl_InvocationID];
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
