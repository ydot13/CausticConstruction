#version 330 core

in vec4 worldPosition;
in float depth;

out vec4 color;

void main() {
	color = vec4(worldPosition.xyz, depth);
}