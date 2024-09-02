#include <string>
#include <GLFW/glfw3.h>

namespace application {
	void input_submitChar(unsigned int codepoint);
	void input_submitAction(int key, int scancode, int action, int mods);

	void input_poll();
}

namespace window {
	void charCallback(GLFWwindow* glfwWindow, unsigned int codepoint);
	void onKeyAction(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);
}