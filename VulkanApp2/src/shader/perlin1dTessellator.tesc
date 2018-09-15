#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform UniformBufferObject
{
  mat4 mModel;
  mat4 mView;
  mat4 mProj;
} ubo;

layout( vertices=2 ) out;

layout (location = 0) in vec3 inColor[];
layout (location = 1) in vec2 inTexCoords[];

layout (location = 0) out vec3 fragColor[2];
layout (location = 1) out vec2 texCoords[2];

void main()
{
	float d;

	if (gl_InvocationID==0) // wird nur beim ersten Durchlauf gesetzt
	{
		vec4 p = ubo.mView * ubo.mModel * (gl_in[0].gl_Position+gl_in[1].gl_Position)/2.0;
		float distance = length(vec2(p.xy));

		if (distance < 3.0)
			d = 100.0;
		else
			d = 1.0;
		gl_TessLevelOuter[0] = 1.0;
		gl_TessLevelOuter[1] = d;
	}

	fragColor[gl_InvocationID] = inColor[gl_InvocationID];
	texCoords[gl_InvocationID] = inTexCoords[gl_InvocationID];
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
