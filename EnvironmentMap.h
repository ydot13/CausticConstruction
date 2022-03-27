#pragma once
#include"Mesh.h"
#include"Shader.h"
#include"Utilitis.h"

class EnvironmentMap {
public:
	EnvironmentMap(const std::vector<Mesh>& obj, GLuint* w, GLuint* h, int res)
		: objects(obj), width(w), height(h), resolution(res){
		Setup();
	}

	EnvironmentMap(std::vector<Mesh>&& obj, GLuint* w, GLuint* h, int res)
		: objects(obj), width(w), height(h), resolution(res) {
		Setup();
	}

	void Resize();
	void Draw(Shader shader);

	~EnvironmentMap();


	std::vector<glm::mat4> models;
	glm::mat4 view;
	glm::mat4 projection;

	GLuint Frame, FBO, RBO;
private:
	std::vector<Mesh> objects;
	GLuint* width;
	GLuint* height;
	int resolution;
	GLenum fboStatus;
	void Setup();
};