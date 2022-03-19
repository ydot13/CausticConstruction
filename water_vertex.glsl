#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 cameraPos;
uniform sampler2D heightMap;

out vec3 skyCoords;

//uniform mat3 modelNorm;

void main() {
	vec3 Position = (model*vec4(position.x, position.y + texture(heightMap, position.xz*0.5 + 0.5).r, position.z, 1.f)).xyz;
	vec4 info = texture(heightMap, position.xz * 0.5 + 0.5);
	vec3 norm = vec3(info.b, sqrt(1.f - dot(info.ba, info.ba)), info.a).xyz;
	vec3 I = normalize(Position - cameraPos);
	vec3 R = normalize(reflect(I, normalize(norm)));

	

	skyCoords = R;
	gl_Position = projection*view*model*vec4(Position, 1.f);
}