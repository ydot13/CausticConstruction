#version 330 core
precision highp float;
out vec4 FragColor;
uniform sampler2D lastFrame;
in vec2 coord;

uniform float delta;

void main()
{

	vec2 delta = vec2(1.f / delta, 1.f / delta);
	vec2 coords = coord;
	vec4 info = texture(lastFrame, coords);

	vec2 dx = vec2(delta.x, 0.0);
	vec2 dy = vec2(0.0, delta.y);

	float average = (
		texture(lastFrame, coords - dx).r +
		texture(lastFrame, coords - dy).r +
		texture(lastFrame, coords + dx).r +
		texture(lastFrame, coords + dy).r
		) * 0.25;


	
	info.g += (average - info.r) * 2.0;
	info.g *= 0.995f;
	info.r += info.g;

		

	vec3 ddx = vec3(delta.x, texture(lastFrame, vec2(coords.x + delta.x, coords.y)).r - info.r, 0.f);
	vec3 ddy = vec3(0.f, texture(lastFrame, vec2(coords.x, coords.y + delta.y)).r - info.r, delta.y);

	info.ba = normalize(cross(ddy, ddx)).xz; 
	FragColor = info;
}
