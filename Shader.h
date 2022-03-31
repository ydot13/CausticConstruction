#pragma once

// Std
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>

// GLEW
#include<GL/glew.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Class of shader programm
class Shader {
public:
	GLuint Program;

	Shader(const GLchar* vertexPath, const char* fragmentPath);

	void SetVec3(const char* name, float x, float y, float z);
	void SetVec3(const char* name, glm::vec3 v) { SetVec3(name, v.x, v.y, v.z); }

	void SetMat4(const char* name, glm::mat4 matr);
	void SetMat3(const char* name, glm::mat3 matr);

	void SetFloat(const char* name, float val);
	void SetInt(const char* name, int val);

	void Use() const;
};