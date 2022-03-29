#version 330 core

struct Material {
    sampler2D diffuse1;
};

uniform Material material;

in vec2 TexCoords;
out vec4 color;

void main() {
    color = vec4(vec3(texture(material.diffuse1, TexCoords)) , 1.);
}
