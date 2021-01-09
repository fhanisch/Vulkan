#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 mModel;
    mat4 mView;
    mat4 mProj;
} ubo;
layout(binding = 1) uniform sampler2D texSampler;
layout (binding = 2) uniform UBO
{
	vec4 myColor;
} col;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 normalPosition;

layout (location = 0) out vec4 outColor;

const vec3 lightPosition = vec3(1000000.0,1000000.0,1000000.0);

vec3 calcADS(vec3 c, vec3 vertex, vec3 normal, vec3 light) {
	vec3 A = 0.1*c;
	vec3 D = c;
	vec3 S = vec3(0.0);

	vec3 s = normalize(light - vertex);
	vec3 n = normalize(normal);
	vec3 v = normalize(-vertex);
	vec3 r = reflect(-s,n);

	float diffuseIntensity = max(dot(s,n), 0.0);
	if( diffuseIntensity > 0.0 )
		S = vec3(1.0,1.0,0.6) * pow( max( dot(r,v), 0.0 ), 30.0 );

	return A + diffuseIntensity*D + S;
}

void main() {
	vec3 ADS = calcADS(col.myColor.rgb, vertexPosition, normalPosition, vec3(ubo.mView*vec4(lightPosition,1.0)));
	outColor = vec4(ADS, 1.0);
}