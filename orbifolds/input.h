#pragma once

#include <string>
#include <GLFW/glfw3.h>

struct input_t {
	int dx;
	int dy;
	bool it;
	unsigned int play = 0;
};

namespace window {
	inline input_t input;

	inline void input_clear() {
		input.it = false;
	}

	void charCallback(GLFWwindow* glfwWindow, unsigned int codepoint);
	void onKeyAction(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);
}