#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

out vec2 coord;

void main() {
	coord = position.xy* 0.5 + 0.5;
	gl_Position = vec4(position, 1.f);
}