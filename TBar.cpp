#include "TBar.h"

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
	model = glm::translate(model, glm::vec3(posX + BarButton.posX, posY + BarButton.posY - (BarButton.Height - Height) / 2, 0.f));
	shdr.SetMat4("model", model);
	BarButton.btnMesh->Draw(shdr);
	glEnable(GL_DEPTH_TEST);
}