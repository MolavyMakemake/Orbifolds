#include "window.h"
#include "input.h"
#include "application.h"
#include <iostream>


namespace window {
	void charCallback(GLFWwindow* glfwWindow, unsigned int codepoint) {
		if (codepoint < 0x80)
			application::input_submitChar(codepoint);
	}
	void onKeyAction(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
		application::input_submitAction(key, scancode, action, mods);
	}
}

namespace application {

	namespace {
		bool isCharPending = false;
		bool isActionPending = false;

		unsigned int codepoint;
		int key, scancode, action, mods;

		void readChar() {
			
		}

		void readAction() {
			if (action == GLFW_RELEASE)
				return;
		}
	}

	void input_poll() {
		if (isCharPending) {
			readChar();
			isCharPending = false;
		}

		if (isActionPending) {
			readAction();
			isActionPending = false;
		}
	}

	void input_submitChar(unsigned int _codepoint) {
		while (isCharPending);

		codepoint	= _codepoint;
		isCharPending = true;
	}
	void input_submitAction(int _key, int _scancode, int _action, int _mods) {
		while (isActionPending);

		key			= _key;
		scancode	= _scancode;
		action		= _action;
		mods		= _mods;
		isActionPending = true;
	}
}