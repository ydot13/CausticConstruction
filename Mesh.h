#pragma once

// GLM
#include<glm/glm.hpp>

// My includes
#include"Shader.h"
#include"Drawable.h"

// Std
#include<vector>

//Assimp
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>


enum class TextureType {
	DIFFUSE,
	SPECULAR
};

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	TextureType type;
	aiString path;
};

// class of polygon mesh with textures(diffuse + specular)
class Mesh : public Drawable{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	float shininess = 32.f;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures) :
		vertices(vertices), indices(indices), textures(textures) {
		setupMesh();
	}

	void Draw(Shader& shader) const override;

private:
	void setupMesh();
	unsigned int VAO = 0, VBO = 0, EBO = 0;
};

