#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 mModel;
    mat4 mView;
    mat4 mProj;
} ubo;

layout(location = 0) in vec2 mesh;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 texCoords;
layout(location = 2) out vec3 vertexPosition;
layout(location = 3) out vec3 normalPosition;

out gl_PerVertex {
	vec4 gl_Position;
};

const float pi = 3.14159;

void main() {
	float R=1.0;
	vec3 n1, n2, r, n;

	float phi = pi/4.0 - pi/2.0 * mesh.x;
	float theta = -pi/4.0 + pi/2.0 * mesh.y;

	n1 = vec3(-sin(phi), cos(phi), 0);
	n2 = vec3(-sin(theta), 0, -cos(theta));

	r = -normalize(cross(n1,n2));
	n = r;

	fragColor = vec3(0.0, 1.0, 0.0);
	texCoords = vec2(mesh.x, mesh.y);
	vertexPosition = vec3(ubo.mView * ubo.mModel * vec4(r, 1.0));
	normalPosition = vec3(transpose(inverse(ubo.mView * ubo.mModel)) * vec4(n, 1.0));
	gl_Position = ubo.mProj * vec4(vertexPosition, 1.0);
}