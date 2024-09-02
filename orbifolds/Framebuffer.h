#pragma once
#include <glad/glad.h>

class Framebuffer {

public:
	Framebuffer(int width, int height);
	void resize(int width, int height);
	void bind();
	void unbind();
	void activate();

	GLuint TBO, DBO, framebuffer;
};