#pragma once
#include<glm/glm.hpp>
#include"Shader.h"
#include<vector>

//Assimp
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

enum TextureType {
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

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	float shininess = 32.f;

	Mesh(const std::vector<Vertex> vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures) :
		vertices(vertices), indices(indices), textures(textures) {
		setupMesh();
	}

	void Draw(Shader& shader) const;

private:
	void setupMesh();
	unsigned int VAO = 0, VBO = 0, EBO = 0;
};

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader) const {
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	for (size_t i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		std::string name;
		switch (textures[i].type) {
			case DIFFUSE:
				name = "diffuse";
				name += std::to_string(diffuseNr++);
				break;
			case SPECULAR:
				name = "specular";
				name += std::to_string(specularNr++);
				break;
		}

		shader.SetFloat(("material." + name).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	//shader.SetFloat("material.shininess", shininess);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	//glActiveTexture(GL_TEXTURE0);	
}