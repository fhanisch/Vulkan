#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 2) uniform UniformBufferObject {vec4 myColor;} c;

layout (location = 0) in vec3 color;
layout (location = 1) in vec2 texCoords;

layout (location = 0) out vec4 outColor;

const float s = 10.0;

void main() {
	vec4 farbe;
    vec3 farbe2;
	float x = texCoords.x;
	float y = texCoords.y;

    farbe2=color;
    if ( (mod(floor(2*y),2.0)==1 && mod(floor(2*x),2.0)==1) || (mod(floor(2*y),2.0)==0 && mod(floor(2*x),2.0)==0) )
        farbe2 = vec3(c.myColor);

	if ( (mod(floor(s*y),2.0)==1 && mod(floor(s*x),2.0)==1) || (mod(floor(s*y),2.0)==0 && mod(floor(s*x),2.0)==0) )
		farbe=vec4(farbe2,1.0);
	else
		farbe=vec4(0.5*farbe2,1.0);
	outColor = farbe;
}
