#pragma once

#include "Diagnostics.h"
#include <string>
#include <queue>
#include <imgui.h>
#include <regex>
#include <chrono>
#include <list>

namespace application {
	inline bool shouldClose;

	void run();
	inline void terminate() {
		shouldClose = true;
	}
};