#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 color;
layout (location = 1) in vec2 texCoords;

layout (location = 0) out vec4 outColor;

const float s = 10.0;

void main()
{
	vec4 farbe;
	float x = texCoords.x;
	float y = texCoords.y;

	if ( (mod(floor(s*y),2.0)==1 && mod(floor(s*x),2.0)==1)
				|| (mod(floor(s*y),2.0)==0 && mod(floor(s*x),2.0)==0) )
		farbe=vec4(color,1.0);
	else
		farbe=vec4(0.5*color,1.0);
	outColor = farbe;
}
