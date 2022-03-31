#pragma once

//STD
#include<algorithm>
#include<vector>

// My includes
#include"Utilitis.h"
#include"Mesh.h"

// Class of bar with button
class TBar {
	// Button object
	struct TBarButton
	{
		TBarButton(float bX, float bY, float bW) : barX(bX), barY(bY), barWidth(bW) {
			Width = 0.02;
			Height = 0.05;
			posX = 0;
			posY = 0;

			Vertex ld, lu, rd, ru;
			ld.Position = glm::vec3(0, 0, 1.f);
			ld.TexCoords = glm::vec2(0.f, 0.f);

			lu.Position = glm::vec3(0, Height, 1.f);
			lu.TexCoords = glm::vec2(0.f, 1.f);

			rd.Position = glm::vec3(Width, 0, 1.f);
			rd.TexCoords = glm::vec2(1.f, 0.f);

			ru.Position = glm::vec3(Width, Height, 1.f);
			ru.TexCoords = glm::vec2(1.f, 1.f);

			std::vector<Vertex> v = { ld, lu, rd, ru };
			std::vector<unsigned int> i = { 0, 1, 2, 1, 3, 2 };

			Texture barTex;
			barTex.id = loadTexture("res\\interface\\btn.jpg", GL_NEAREST, GL_NEAREST_MIPMAP_LINEAR);

			barTex.path = "res\\interface\\btn.jpg";
			barTex.type = DIFFUSE;
			std::vector<Texture> t = { barTex };
			btnMesh = std::make_shared<Mesh>(v, i, t);
		}

		// Check intersection with cursor
		bool Hit(float x, float y) {
			x -= barX;
			y -= barY;
			return (x >= posX && x <= posX + Width && y >= posY && y <= posY + Height);
		}

		void SetPos(float x) {
			x -= barX;
			posX = std::min(barWidth, std::max(0.f, x));
		}

		float Width, Height;
		// Pos of btn in local system
		float posX, posY;
		// Pos of bar in sreen system
		float barX, barY;
		// Width of bar
		float barWidth;
		std::shared_ptr<Mesh> btnMesh;
	};
public:
	TBar(float bX, float bY, float W, float H) : BarButton(TBarButton(bX, bY, W)), posX(bX), posY(bY), Width(W), Height(H) {
		Vertex ld, lu, rd, ru;
		ld.Position = glm::vec3(0, 0,1.f);
		ld.TexCoords = glm::vec2(0.f, 0.f);

		lu.Position = glm::vec3(0,  Height, 1.f);
		lu.TexCoords = glm::vec2(0.f, 1.f);

		rd.Position = glm::vec3(Width, 0, 1.f);
		rd.TexCoords = glm::vec2(1.f, 0.f);

		ru.Position = glm::vec3(Width, Height, 1.f);
		ru.TexCoords = glm::vec2(1.f, 1.f);

		std::vector<Vertex> v = { ld, lu, rd, ru };
		std::vector<unsigned int> i = { 0, 1, 2, 1, 3, 2};

		Texture barTex;
		barTex.id = loadTexture("res\\interface\\bar.jpg", GL_NEAREST, GL_NEAREST_MIPMAP_LINEAR);

		barTex.path = "res\\interface\\bar.jpg";
		barTex.type = DIFFUSE;
		std::vector<Texture> t = { barTex };
		barMesh = std::make_shared<Mesh>(v, i, t);
	}
	// Draw Tbar
	void Show();

	// Events
	void OnMouseDown(float x, float y);
	void OnMove(float x, float y);
	void OnMouseUp(float x, float y);

	// get Val from 0.f to 1.f
	float getValue();

	// Check intersection with cursor
	bool Hit(float x, float y);

private:
	TBarButton BarButton;
	float Width, Height;
	float posX, posY;
	bool m_bButtonClick = false;

	std::shared_ptr<Mesh> barMesh;
	Shader shdr = Shader("shader\\interface\\interface_vertex.glsl", "shader\\interface\\interface_fragment.glsl");
};

bool TBar::Hit(float x, float y) {
	return (x >= posX && x <= posX + Width && y >= posY && y <= posY + Height);
}

void TBar::OnMouseDown(float x, float y) {
	if (BarButton.Hit(x, y)) {
		m_bButtonClick = true;
		BarButton.SetPos(x);
	}
	else if (Hit(x, y)) {
		BarButton.SetPos(x);
	}
}

void TBar::OnMove(float x, float y) {
	if (!m_bButtonClick)
		return;

	BarButton.SetPos(x);
}

float TBar::getValue() {
	return BarButton.posX / Width;
}

void TBar::OnMouseUp(float x, float y) {
	m_bButtonClick = false;
}

void TBar::Show() {
	//Draw bar
	glDisable(GL_DEPTH_TEST);
	glm::mat4 model(1.f);
	model = glm::translate(model, glm::vec3(posX, posY, 0.f));
	shdr.Use();
	shdr.SetMat4("model", model);
	barMesh->Draw(shdr);
	//Draw button
	model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(posX + BarButton.posX, posY + BarButton.posY - (BarButton.Height - Height)/2, 0.f));
	shdr.SetMat4("model", model);
	BarButton.btnMesh->Draw(shdr);
	glEnable(GL_DEPTH_TEST);
}