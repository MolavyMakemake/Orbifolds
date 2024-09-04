#include <string>
#include <GLFW/glfw3.h>

namespace window {
	inline int input_dx;
	inline int input_dy;

	inline bool input_it;
	inline bool input_fwd;
	inline bool input_bck;

	inline void input_clear() {
		input_it = false;
		input_fwd = false;
		input_bck = false;
	}

	void charCallback(GLFWwindow* glfwWindow, unsigned int codepoint);
	void onKeyAction(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);
}