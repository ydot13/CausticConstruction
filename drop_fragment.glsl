#version 330 core
precision highp float;
out vec4 FragColor;
uniform sampler2D lastFrame;
in vec2 coord;
uniform float x;
uniform float y;

const float PI = 3.141592653589793;
const float strength = 0.1;
const float radius = 0.05f;
void main()
{
	vec2 coords = coord;
	vec4 info = texture(lastFrame, coords);

	float drop = max(0.0, 1.0f - length(vec2(x, y) - coords) / radius);
	drop = 0.5 - cos(drop * PI) * 0.5f;
	info.r += drop * strength;
	FragColor = info;
}


