#include "window.h"
#include "input.h"
#include <iostream>
#include <bitset>

typedef unsigned char byte;

namespace window {

	std::bitset<4> key_bs;
	
	void charCallback(GLFWwindow* glfwWindow, unsigned int codepoint) {
	}
	void onKeyAction(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
		switch (key)
		{
		case GLFW_KEY_RIGHT:
			key_bs.set(0, action != GLFW_RELEASE);
			break;
		case GLFW_KEY_LEFT:
			key_bs.set(1, action != GLFW_RELEASE);
			break;
		
		case GLFW_KEY_UP:
			key_bs.set(2, action != GLFW_RELEASE);
			break;
		case GLFW_KEY_DOWN:
			key_bs.set(3, action != GLFW_RELEASE);
			break;

		case GLFW_KEY_SLASH:
			input_it = action == GLFW_PRESS;
			break;
		case GLFW_KEY_PERIOD:
			input_fwd = action == GLFW_PRESS;
			break;
		case GLFW_KEY_COMMA:
			input_bck = action == GLFW_PRESS;
			break;

		default:
			return;
		}

		input_dx = key_bs.test(0) - key_bs.test(1);
		input_dy = key_bs.test(2) - key_bs.test(3);
	}
}