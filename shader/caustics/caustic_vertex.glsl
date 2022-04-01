#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

uniform mat4 view;
uniform mat4 projection;
uniform sampler2D water;
uniform sampler2D env;
uniform vec3 light;
uniform float deltaEnvTexture;

out vec3 oldPosition;
out vec3 newPosition;
out float waterDepth;
out float depth;

const float eta = 0.7504;
const int MAX_ITERATIONS = 50;

void main() {
	vec4 waterInfo = texture(water, position.xz*0.5 + 0.5);

	// world water pos
	vec3 waterPosition = vec3(position.x, position.y + waterInfo.r, position.z);
	vec3 waterNormal = normalize(vec3(waterInfo.b, sqrt(1.f - dot(waterInfo.ba, waterInfo.ba)), waterInfo.a).xyz);

	// world pos of water surface
	oldPosition = waterPosition.xzy;

	// pos of water surface in screen space
	vec4 projectedWaterPosition = projection*view*vec4(waterPosition, 1.);
	vec2 currentPosition = projectedWaterPosition.xy;
	vec2 coords = currentPosition * 0.5f + 0.5f;

	// refracrted vec in screen space
	vec3 refracted = refract(light, waterNormal, eta);
	vec4 projectedRefractionVector = projection * view * vec4(refracted, 1.);
	vec3 refractedDirection = projectedRefractionVector.xyz;


	waterDepth = 0.5 + 0.5 * projectedWaterPosition.z / projectedWaterPosition.w;

	float currentDepth = projectedWaterPosition.z;
	vec4 environment = texture(env, coords);
	float factor;
	if (length(refractedDirection.xy) == 0.)
		factor = 0;
	else
		factor = deltaEnvTexture / length(refractedDirection.xy);

	vec2 deltaDirection = refractedDirection.xy * factor;
	float deltaDepth = refractedDirection.z* factor;

	// go through env map while curDepth < envDepth
	for (int i = 0; i < MAX_ITERATIONS; i++) {
		
		currentPosition += deltaDirection;
		currentDepth += deltaDepth;

		
		if (environment.w <= currentDepth) {
			break;
		}

		environment = texture(env, 0.5 + 0.5 * currentPosition);
	}

	// pos of refracted way on underwater surf
	newPosition = environment.xyz;
	vec4 projectedEnvPosition = projection * view * vec4(newPosition, 1.0);
	depth = 0.5 + 0.5 * projectedEnvPosition.z / projectedEnvPosition.w;

	newPosition = newPosition.xzy;

	gl_Position = projectedEnvPosition;
}