#pragma once
#include "Mesh.h"
#include "Shader.h"
#include"Utilitis.h"

class Water {
public:
	Water(int width, int height, int resolution);
	void Update();
	void AddDrop(float x, float y);
	void Draw(Shader shdr);
	
	~Water();

	std::shared_ptr<Mesh> waterGrid;
	GLuint last_frame, current_frame, FBO, VAO, VBO;
	GLenum fboStatus;

	const float vert_texture[5 * 6] = {
		-1.f, -1.f, 0.f, 0.f, 0.f,
		1.f, -1.f, 0.f, 1.f, 0.f,
		-1.f, 1.f, 0.f, 0.f, 1.f,
		1.f, -1.f, 0.f, 1.f, 0.f,
		1.f, 1.f, 0.f, 1.f, 1.f,
		-1.f, 1.f, 0.f, 0.f, 1.f
	};
	int height, width, resolution;
	Shader update_shader = Shader("shader\\water\\water_update_vertex.glsl", "shader\\water\\water_update_fragment.glsl");
	Shader drop_shader = Shader("shader\\water\\water_update_vertex.glsl","shader\\water\\drop_fragment.glsl");
};
