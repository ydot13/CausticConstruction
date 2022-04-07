#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 cameraPos;
uniform sampler2D heightMap;
out vec2 refractedPosition[3];

out vec3 skyCoords;

const float eta = 0.7504f;
out float reflectionFactor;
uniform mat3 modelNorm;

void main() {
	vec3 Position = (vec4(position.x, position.y + texture(heightMap, position.xz*0.5 + 0.5).r, position.z, 1.f)).xyz;
	vec4 info = texture(heightMap, position.xz * 0.5 + 0.5);
	vec3 norm = vec3(info.b, sqrt(1.f - dot(info.ba, info.ba)), info.a).xyz;
	vec3 eye = normalize(Position - cameraPos);
	vec3 R = normalize(reflect(eye, normalize(norm)));
	skyCoords = R;

	reflectionFactor = 0.1 + pow(1. + dot(eye, norm), 2);

	vec3 refracted = normalize(refract(eye, normalize(norm), eta));

	mat4 proj = projection * view * model;

	vec4 projectedRefractedPosition = proj * vec4(Position + reflectionFactor *refracted, 1.f);

	refractedPosition[0] = projectedRefractedPosition.xy / projectedRefractedPosition.w;

	projectedRefractedPosition = proj * vec4(Position + reflectionFactor *normalize(refract(eye, norm, eta * 0.96)), 1.0);
	refractedPosition[1] = projectedRefractedPosition.xy / projectedRefractedPosition.w;

	projectedRefractedPosition = proj * vec4(Position + reflectionFactor *normalize(refract(eye, norm, eta * 0.92)), 1.0);
	refractedPosition[2] = projectedRefractedPosition.xy / projectedRefractedPosition.w;


	gl_Position = projection*view*vec4(Position, 1.f);
}