#pragma once
#include"Shader.h"
class Drawable {
public:
	virtual void Draw(Shader& shdr) const = 0;
};