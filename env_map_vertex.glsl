#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float depth;
out vec4 worldPosition;

void main() {
	worldPosition = model * vec4(position.x, position.y, position.z, 1.f);
	vec4 projectedPosition = projection * view * worldPosition;
	depth = projectedPosition.z;

	gl_Position = projectedPosition;
}