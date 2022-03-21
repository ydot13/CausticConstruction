#version 330 core
precision highp float;
out vec4 FragColor;
uniform sampler2D lastFrame;
in vec2 coord;
uniform vec3 fl;
void main()
{

	vec2 delta = vec2(1.f / 120, 1.f / 120);
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


	if ( fl.x < 0) {
		info.g += (average - info.r) * 2.0;
		info.g *= 0.995f;
		info.r += info.g;

		

		vec3 ddx = vec3(delta.x, texture(lastFrame, vec2(coords.x + delta.x, coords.y)).r - info.r, 0.f);
		vec3 ddy = vec3(0.f, texture(lastFrame, vec2(coords.x, coords.y + delta.y)).r - info.r, delta.y);

		info.ba = normalize(cross(ddy, ddx)).xz; 
		FragColor = info;
	}
	else {
		float drop = max(0.0, 1.0f - length(vec2(0.7f, 0.5f) - coords) / 0.08f);
		drop = 0.5 - cos(drop * 3.14159f) * 0.5f;
		info.r += drop * 0.2f;
		info.g = 0.0;
		FragColor = info;
	}
}
