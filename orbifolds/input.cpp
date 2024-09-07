#include "window.h"
#include "input.h"
#include <iostream>
#include <bitset>

typedef unsigned char byte;

namespace window {

	std::bitset<6> key_bs;
	
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

		case GLFW_KEY_RIGHT_SHIFT:
			key_bs.set(4, action != GLFW_RELEASE);
			break;
		case GLFW_KEY_RIGHT_CONTROL:
			key_bs.set(5, action != GLFW_RELEASE);
			break;

		case GLFW_KEY_SLASH:
			input.it = action == GLFW_PRESS;
			break;
		case GLFW_KEY_PERIOD:
			input.play = std::max<int>(1, input.play << (action == GLFW_PRESS));
			input.play = std::min<int>(32, input.play);
			break;
		case GLFW_KEY_COMMA:
			input.play = input.play >> (action == GLFW_PRESS);
			break;

		default:
			return;
		}

		input.dx = key_bs.test(0) - key_bs.test(1);
		input.dy = key_bs.test(2) - key_bs.test(3);
		input.dz = key_bs.test(4) - key_bs.test(5);
	}
}