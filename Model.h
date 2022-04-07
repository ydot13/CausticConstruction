#pragma once

// Std
#include<vector>
#include<unordered_map>

// My inccludes
#include"Utilitis.h"
#include"Drawable.h"
#include"Shader.h"
#include"Mesh.h"

//Assimp
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

// Other
#include<SOIL.h>


// Class of model wich consist of several Meshes
class Model : public Drawable {
public:
	Model(const std::string& path) {
		loadModel(path);
	}

	void Draw(Shader& shader) const override;

	

	void loadModel(const std::string& path);
private:
	std::vector<Mesh> meshes;
	std::string directory;
	// set of loaded textures
	std::unordered_map<std::string, Texture> loaded_textures;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType textureType);
};