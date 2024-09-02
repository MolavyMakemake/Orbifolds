#include "gui.h"
#include "application.h"
#define CLOCK_GRAPH_UPDATE_FREQUENCY CLOCKS_PER_SEC * GRAPH_TIMESPAN_MS / (GRAPH_RESOLUTION * 1000)


#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

namespace window {

	namespace {
		clock_t lastFrame_clock = 0;
		clock_t graph_clock = 0;
	}

	void gui_drawRect(ImVec2 pos, ImVec2 size, ImU32 color) {
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImVec2 cursor_wp = ImGui::GetCursorPos();
		ImVec2 pos_sp = ImGui::GetCursorScreenPos() + pos;

		draw_list->AddRectFilled(pos_sp, pos_sp + size, color);
		ImGui::SetCursorPos(cursor_wp);
	}
}