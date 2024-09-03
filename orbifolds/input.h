#include <string>
#include <GLFW/glfw3.h>

namespace window {
	inline int input_dx;
	inline int input_dy;

	inline void input_clear() {
	}

	void charCallback(GLFWwindow* glfwWindow, unsigned int codepoint);
	void onKeyAction(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);
}