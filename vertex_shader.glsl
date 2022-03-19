#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightView;
uniform mat4 lightProjection;

out vec3 lightPos;

out vec2 TexCoords;


void main() {
	vec4 worldPos = model * vec4(position, 1.f);

	vec4 lightProjection = lightProjection * lightView * worldPos;

	lightPos = vec3(lightProjection.xy*0.5f + 0.5f, 0.5f + lightProjection.z / lightProjection.w * 0.5);

	gl_Position = projection * view * model * vec4(position, 1.f);
	TexCoords = texCoords;
}