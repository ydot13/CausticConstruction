#version 330 core

in vec3 lightPos;

uniform sampler2D caustics;

in vec2 TexCoords;
out vec4 color;

const float bias = 0.001;
const vec3 underwaterColor = vec3(0.4, 0.9, 1.0);
const vec2 resolution = vec2(512.);

float blur(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
    float intensity = 0.;
    vec2 off1 = vec2(1.3846153846) * direction;
    vec2 off2 = vec2(3.2307692308) * direction;
    intensity += texture2D(image, uv).x * 0.2270270270;
    intensity += texture2D(image, uv + (off1 / resolution)).x * 0.3162162162;
    intensity += texture2D(image, uv - (off1 / resolution)).x * 0.3162162162;
    intensity += texture2D(image, uv + (off2 / resolution)).x * 0.0702702703;
    intensity += texture2D(image, uv - (off2 / resolution)).x * 0.0702702703;
    return intensity;
}

void main() {

    float computedLightIntensity = 0.5;


	float causticsDepth = texture2D(caustics, lightPos.xy).w;

    if (causticsDepth > lightPos.z - bias) {
       
        float causticsIntensity = 0.5 * (
            blur(caustics, lightPos.xy, resolution, vec2(0., 0.5)) +
            blur(caustics, lightPos.xy, resolution, vec2(0.5, 0.))
            );

        computedLightIntensity += causticsIntensity * smoothstep(0., 1., 1.f);
    }

	color = vec4(underwaterColor * computedLightIntensity, 1.);
}
