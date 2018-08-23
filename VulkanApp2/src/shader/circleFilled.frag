#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 color;
layout (location = 1) in vec2 texCoords;

layout (location = 0) out vec4 outColor;

void main()
{
	vec4 farbe;
	float x = 2.0*texCoords.x-1.0;
	float y = 2.0*texCoords.y-1.0;
	float R = sqrt(pow(abs(x),2.0)+pow(abs(y),2.0));

	if (R<=1.0)
	{
		farbe=vec4(0.0,1.0,0.0,0.33);
	}
	else
	{
		farbe=vec4(0.0,0.0,0.0,0.0);
	}

	outColor = farbe;
}
