#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <SOIL.h>
#include<vector>
#include<iostream>
#include"Mesh.h"
unsigned int loadTexture(char const* path, GLint mag_filter_param = GL_LINEAR, GLint min_filter_param = GL_LINEAR_MIPMAP_LINEAR);

GLuint CreateTexture(GLsizei width, GLsizei height, GLint internalformat, GLenum fromat, GLint param = GL_CLAMP_TO_EDGE);

GLuint loadCubemap(const std::vector<std::string>& faces);

std::shared_ptr<Mesh> genGrid(int size);
