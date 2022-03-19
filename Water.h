#pragma once
#include "Mesh.h"
#include "Shader.h"

class Water {
public:
	int fl = 0;
	Water(int h, int w);
	void Update() {
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
			glViewport(0, 0, height, width);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, last_frame);
			update_shader.Use();
			
			update_shader.SetInt("lastFrame", 0);
			if (fl < 1) {
				fl++;
				update_shader.SetVec3("fl", glm::vec3(1.f, 0.f, 0.f));
			}
			else {
				update_shader.SetVec3("fl", glm::vec3(-1.f, 0.f, 0.f));
			}
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D,current_frame);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		std::swap(current_frame, last_frame);

		glViewport(0, 0, 800, 600);
	}
	void AddDrop(float x, float y);

	void Draw(Shader shdr) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, last_frame);
		shdr.SetInt("heightMap", 0);
		
		waterGrid->Draw(shdr);
	}

	~Water() {
		glDeleteFramebuffers(1, &FBO);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteTextures(1, &current_frame);
		glDeleteTextures(1, &last_frame);
	}

	std::shared_ptr<Mesh> waterGrid;
	GLuint last_frame, current_frame, FBO, VAO, VBO;
	GLenum fboStatus;

	float vert_texture[5 * 6] = {
		-1.f, -1.f, 0.f, 0.f, 0.f,
		1.f, -1.f, 0.f, 1.f, 0.f,
		-1.f, 1.f, 0.f, 0.f, 1.f,
		1.f, -1.f, 0.f, 1.f, 0.f,
		1.f, 1.f, 0.f, 1.f, 1.f,
		-1.f, 1.f, 0.f, 0.f, 1.f
	};
	int height, width;
	Shader update_shader = Shader("water_update_vertex.glsl", "water_update_fragment.glsl");
	Shader view_shader = Shader("vertex_shader.glsl", "fragment_shader.glsl");
};

Water::Water(int h, int w)  {
	height = h;
	width = w;
	std::vector<Vertex> verticies;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	float deltaW = 1.f / w;
	float deltaH = 1.f / h;
	for (long long i = 0; i < 1ll * (w + 1) * (h + 1); ++i) {
		glm::vec3 Position((i % (w + 1)) * deltaW , 0.f , (i / (w + 1)) * deltaH);
		Position.x = Position.x * 2 - 1;
		Position.z = Position.z * 2 - 1;
		Vertex vert;
		vert.Position = Position;

		vert.TexCoords = glm::vec2(Position.x, Position.z);
		verticies.push_back(std::move(vert));
		if (i / (w + 1) < h && i % (w + 1) < w) {
			indices.push_back(i);
			indices.push_back(i + 1);
			indices.push_back(i + (w + 1));

			indices.push_back(i + w + 1);
			indices.push_back(i + w + 2);
			indices.push_back(i + 1);
		}
	}

	waterGrid = std::make_shared<Mesh>(verticies, indices, textures);

	current_frame = CreateTexture(h, w, GL_RGBA16F, GL_FLOAT);
	last_frame = CreateTexture(h, w, GL_RGBA16F, GL_FLOAT);

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