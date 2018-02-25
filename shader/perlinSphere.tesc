#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform UniformBufferObject
{
  mat4 mModel;
  mat4 mView;
  mat4 mProj;
} ubo;

layout( vertices=4 ) out;

layout (location = 0) in vec3 inColor[];
layout (location = 1) in vec2 inTexCoords[];

layout (location = 0) out vec3 fragColor[4];
layout (location = 1) out vec2 texCoords[4];

layout (push_constant) uniform PushConstants
{
	float grid;
	float maxDistance;
} pushConsts;

const float pi = 3.14159;

void main()
{
    float R=500.0;
	float grid;
     vec3 f;

	if (gl_InvocationID==0) // wird nur beim ersten Durchlauf gesetzt
	{
        float u = 2.0*pi*gl_in[0].gl_Position.x;
        float v = pi*gl_in[0].gl_Position.z;
        f.x = R*sin(v)*cos(u);
	    f.y = R*sin(v)*sin(u);
	    f.z = R*cos(v);
		vec4 p = ubo.mView * ubo.mModel * vec4(f,1.0);
		float distance = length(vec3(p.xyz));

		if (distance < pushConsts.maxDistance)
			grid = pushConsts.grid;
		else
			grid = 1.0;

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
