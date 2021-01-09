#version 450
#extension GL_ARB_separate_shader_objects : enable

layout( vertices=16 ) out;

const float grid = 20.0;

void main() {
	
	if (gl_InvocationID == 0) { // wird nur beim ersten Durchlauf gesetzt        		
		gl_TessLevelOuter[0] = grid;
		gl_TessLevelOuter[1] = grid;
		gl_TessLevelOuter[2] = grid;
		gl_TessLevelOuter[3] = grid;
		gl_TessLevelInner[0] = grid;
		gl_TessLevelInner[1] = grid;
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}