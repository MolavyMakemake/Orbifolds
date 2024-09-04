#include <string>
#include <GLFW/glfw3.h>

namespace window {
	inline int input_dx;
	inline int input_dy;

	inline bool input_it;
	inline unsigned int input_play = 0;

	inline void input_clear() {
		input_it = false;
	}

	void charCallback(GLFWwindow* glfwWindow, unsigned int codepoint);
	void onKeyAction(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);
}