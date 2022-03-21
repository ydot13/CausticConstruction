#version 330 core
out vec4 FragColor;

in vec3 skyCoords;
uniform samplerCube skybox;
uniform sampler2D env;
in vec2 refractedPosition[3];
in float reflectionFactor;
void main()
{
    vec3 reflectedColor = texture(skybox, skyCoords).rgb;

    vec3 refractedColor = vec3(1.f);

    refractedColor.r = texture2D(env, refractedPosition[0] * 0.5 + 0.5).r;
    refractedColor.g = texture2D(env, refractedPosition[1] * 0.5 + 0.5).g;
    refractedColor.b = texture2D(env, refractedPosition[2] * 0.5 + 0.5).b;

    FragColor = vec4(mix(refractedColor, reflectedColor, clamp(reflectionFactor, 0., 1.)), 1.);
}