#pragma once
#include"Mesh.h"
#include"Utilitis.h"

// Class of caustics on underwater surface
class Caustics {
public:
	Caustics(GLuint* w, GLuint* h, int r)
		: waterGrid(genGrid(512)), width(w), height(h), resolution(r) {
		Setup();
	}

	// Draw caustics texture
	void Draw(Shader& shader, GLuint water, GLuint env);

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	GLuint Frame;

	~Caustics() {
		glDeleteFramebuffers(1, &FBO);
		glDeleteTextures(1, &Frame);
	}

private:
	GLuint FBO;
	// Grid of water
	std::shared_ptr<Mesh> waterGrid;

	GLenum fboStatus;
	void Setup();

	// Width and height of screen
	GLuint* width;
	GLuint* height;
	// Resolution of texture
	int resolution;	
};

void Caustics::Draw(Shader& shader, GLuint water, GLuint env) {
	// bind frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Add blend for caustics addition
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
	glBlendEquation(GL_FUNC_ADD);

	// clear 
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set texture viewport
	glViewport(0, 0, resolution, resolution);
	
	// setup shader + draw
	shader.Use();
	shader.SetMat4("view", view);
	shader.SetMat4("model", model);
	shader.SetMat4("projection", projection);
	// water height-map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, water);
	shader.SetInt("water", 0);
	// environment map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, env);
	shader.SetInt("env", 1);
	// 1.f / end.resolution
	shader.SetFloat("deltaEnvTexture", 1.f / 1000);
	shader.SetVec3("light", glm::vec3(0.f, -1.f, 0.f));

	waterGrid->Draw(shader);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, Frame);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);

	glViewport(0, 0, *width, *height);
}

//create framebuffer
void Caustics::Setup() {
	Frame = CreateTexture(resolution, resolution, GL_RGBA16F, GL_FLOAT);
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Frame, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

	if ((fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "glCheckFramebufferStatus error 0x%X\n" << fboStatus;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
