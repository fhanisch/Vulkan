#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform UniformBufferObject {
	mat4 mModel;
	mat4 mView;
	mat4 mProj;
} ubo;

layout( vertices=4 ) out;

layout (location = 0) in vec3 inColor[];
layout (location = 1) in vec2 inTexCoords[];

layout (location = 0) out vec3 fragColor[4];
layout (location = 1) out vec2 texCoords[4];

const float pi = 3.14159;

float getTessLevel(vec3 pos) {
	float R=1.0;
    vec3 f;

	float u = 2.0*pi*pos.x;
        float v = pi*pos.z;
        f.x = R*sin(v)*cos(u);
	    f.y = R*sin(v)*sin(u);
	    f.z = R*cos(v);
		vec4 p = ubo.mView * ubo.mModel * vec4(f,1.0);
		float distance = length(vec3(p.xyz));

		if (distance <= 200.0)
			return 100.0;
		else
			return 1.0;
}

void main() {

	float grid[4];

	if (gl_InvocationID == 0) { // wird nur beim ersten Durchlauf gesetzt
        
		grid[0] = getTessLevel(vec3(gl_in[0].gl_Position));
		grid[1] = getTessLevel(vec3(gl_in[1].gl_Position));
		grid[2] = getTessLevel(vec3(gl_in[2].gl_Position));
		grid[3] = getTessLevel(vec3(gl_in[3].gl_Position));

		gl_TessLevelOuter[0] = max(max(max(grid[0],grid[1]),grid[2]), grid[3]);
		gl_TessLevelOuter[1] = max(max(max(grid[0],grid[1]),grid[2]), grid[3]);
		gl_TessLevelOuter[2] = max(max(max(grid[0],grid[1]),grid[2]), grid[3]);
		gl_TessLevelOuter[3] = max(max(max(grid[0],grid[1]),grid[2]), grid[3]);
		gl_TessLevelInner[0] = max(max(max(grid[0],grid[1]),grid[2]), grid[3]);
		gl_TessLevelInner[1] = max(max(max(grid[0],grid[1]),grid[2]), grid[3]);
	}

	fragColor[gl_InvocationID] = inColor[gl_InvocationID];
	texCoords[gl_InvocationID] = inTexCoords[gl_InvocationID];
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}