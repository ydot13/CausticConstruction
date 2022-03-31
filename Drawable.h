#pragma once
#include"Shader.h"

// Abstract class (base for mesh and model)
class Drawable {
public:
	virtual void Draw(Shader& shdr) const = 0;
};