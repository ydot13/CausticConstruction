#pragma once
#include"Mesh.h"
#include"Shader.h"
#include"Utilitis.h"

class EnvironmentMap {
public:
	EnvironmentMap(const std::vector<Mesh>& obj)
		: objects(obj) {
		Setup();
	}

	EnvironmentMap(std::vector<Mesh>&& obj)
		: objects(obj) {
		Setup();
	}

	void Draw(Shader shader);

	std::vector<glm::mat4> models;
	glm::mat4 view;
	glm::mat4 projection;

	GLuint Frame, FBO, RBO;

	~EnvironmentMap() {
		glDeleteRenderbuffers(1, &RBO);
		glDeleteFramebuffers(1, &FBO);
		glDeleteTextures(1, &Frame);
	}
private:
	std::vector<Mesh> objects;

	GLenum fboStatus;
	void Setup();
};

void EnvironmentMap::Draw(Shader shader) {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, 512, 512);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.Use();

	shader.SetMat4("view", view);
	shader.SetMat4("projection", projection);


	for (size_t i = 0; i < objects.size(); i++) {
		shader.SetMat4("model", models[i]);
		objects[i].Draw(shader);
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, Frame);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glViewport(0, 0, 800, 600);
}

void EnvironmentMap::Setup() {
	Frame = CreateTexture(512, 512, GL_RGBA16F, GL_FLOAT);
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Frame, 0);

	if ((fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "glCheckFramebufferStatus error 0x%X\n" << fboStatus;
	}

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
