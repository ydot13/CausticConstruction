#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

uniform mat4 model;

out vec2 TexCoords;


void main() {
	gl_Position = model * vec4(position, 1.f);
	TexCoords = texCoords;
}