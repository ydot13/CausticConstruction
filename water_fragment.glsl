#version 330 core
out vec4 FragColor;

in vec3 skyCoords;
uniform samplerCube skybox;

void main()
{

    FragColor = vec4(texture(skybox, skyCoords).rgb, 0.0)*0.9f + vec4(0.0f, 0.0f, 1.f, 0.f)*0.1f;
}