#pragma once

// GLEW
#include <GL/glew.h>

// Std
#include<vector>
#include<iostream>

// My includes
#include"Mesh.h"

// SOIL
#include <SOIL.h>

// Load texture from path
unsigned int loadTexture(char const* path, GLint mag_filter_param = GL_LINEAR, GLint min_filter_param = GL_LINEAR_MIPMAP_LINEAR);

// Create empty texture
GLuint CreateTexture(GLsizei width, GLsizei height, GLint internalformat, GLenum fromat, GLint param = GL_CLAMP_TO_EDGE);

// Load cubemap from paths in vector faces
GLuint loadCubemap(const std::vector<std::string>& faces);

// Generate mesh of square grid with size * size points
std::shared_ptr<Mesh> genGrid(int size);
