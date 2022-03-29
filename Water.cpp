#include"Water.h"

Water::Water(GLuint* w, GLuint* h, int r) : width(w), height(h), resolution(r) {

	waterGrid = genGrid(resolution);

	current_frame = CreateTexture(resolution, resolution, GL_RGBA16F, GL_FLOAT);
	last_frame = CreateTexture(resolution, resolution, GL_RGBA16F, GL_FLOAT);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert_texture), vert_texture, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glGenFramebuffers(1, &FBO);
}

void Water::Update() {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, current_frame, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

	if ((fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "glCheckFramebufferStatus error 0x%X\n" << fboStatus;
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glViewport(0, 0, resolution, resolution);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, last_frame);
		update_shader.Use();
		update_shader.SetInt("lastFrame", 0);
		update_shader.SetFloat("delta", resolution - 5);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, current_frame);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	std::swap(current_frame, last_frame);

	glViewport(0, 0, *width, *height);
}

void Water::AddDrop(float x, float y) {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, current_frame, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

	if ((fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "glCheckFramebufferStatus error 0x%X\n" << fboStatus;
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glViewport(0, 0, resolution, resolution);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, last_frame);
		drop_shader.Use();
		drop_shader.SetInt("lastFrame", 0);
		drop_shader.SetFloat("x", x);
		drop_shader.SetFloat("y", y);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, current_frame);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	std::swap(current_frame, last_frame);

	glViewport(0, 0, *width, *height);
}

void Water::Draw(Shader shdr) {
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, last_frame);
	shdr.SetInt("heightMap", 3);

	waterGrid->Draw(shdr);
}

Water::~Water() {
	glDeleteFramebuffers(1, &FBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteTextures(1, &current_frame);
	glDeleteTextures(1, &last_frame);
}
