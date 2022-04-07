#pragma once
#include"Mesh.h"
#include"Shader.h"
#include"Utilitis.h"
#include "Drawable.h"

class EnvironmentMap {
public:
	EnvironmentMap(const std::vector<std::shared_ptr<Drawable>>& obj, GLuint* w, GLuint* h, int res)
		: objects(obj), width(w), height(h), resolution(res){
		Setup();
	}

	EnvironmentMap(std::vector<std::shared_ptr<Drawable>>&& obj, GLuint* w, GLuint* h, int res)
		: objects(obj), width(w), height(h), resolution(res) {
		Setup();
	}

	void Draw(Shader& shader);

	~EnvironmentMap();


	std::vector<glm::mat4> models;
	glm::mat4 view;
	glm::mat4 projection;

	GLuint Frame;
private:
	GLuint FBO, RBO;
	std::vector<std::shared_ptr<Drawable>> objects;
	GLuint* width;
	GLuint* height;
	int resolution;
	GLenum fboStatus;
	void Setup();
};