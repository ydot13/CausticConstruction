#pragma once

// My includes
#include "Mesh.h"
#include "Shader.h"
#include"Utilitis.h"

// Class of water grid with height-map
class Water {
public:
	Water(GLuint* width, GLuint* height, int resolution);
	// Update height-map
	void Update();
	// Add drop to height-map
	void AddDrop(float x, float y);

	// Draw mesh of water using height-map
	void Draw(Shader& shdr);
	
	~Water();

	std::shared_ptr<Mesh> waterGrid;
	GLuint current_frame;
private:
	// For height-map
	GLuint last_frame, FBO, VAO, VBO;
	GLenum fboStatus;

	// verticies of square of texture height-map
	const float vert_texture[5 * 6] = {
		-1.f, -1.f, 0.f, 0.f, 0.f,
		1.f, -1.f, 0.f, 1.f, 0.f,
		-1.f, 1.f, 0.f, 0.f, 1.f,
		1.f, -1.f, 0.f, 1.f, 0.f,
		1.f, 1.f, 0.f, 1.f, 1.f,
		-1.f, 1.f, 0.f, 0.f, 1.f
	};

	// sceenn height and width
	GLuint* height;
	GLuint* width;
	// resolution of height map
	int resolution;

	// Shaders of height-map update and adding drop 
	Shader update_shader = Shader("shader\\water\\water_update_vertex.glsl", "shader\\water\\water_update_fragment.glsl");
	Shader drop_shader = Shader("shader\\water\\water_update_vertex.glsl","shader\\water\\drop_fragment.glsl");
};
