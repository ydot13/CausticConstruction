#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightView;
uniform mat4 lightProjection;


out float lightIntensity;
out vec3 lightPos;
out vec2 TexCoords;


void main() {
	
	lightIntensity = -dot(vec3(0.f, -1.f, 0.f), normalize(transpose(inverse(mat3(model))) * normal));
	
	vec4 worldPos = model * vec4(position, 1.f);

	vec4 lightProjection = lightProjection * lightView * worldPos;

	lightPos = 0.5 + lightProjection.xyz / lightProjection.w * 0.5;
	TexCoords = texCoords;
	gl_Position = projection * view * model * vec4(position, 1.f);

}