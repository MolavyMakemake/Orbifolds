#include "gui.h"
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

	void gui_main(input_t* input, Mesh* mesh) {
		ImGui::Begin("gui");

		ImGui::Text("Iteration: %d", mesh->iteration);

		ImGui::Text("> %d", input->play);

		if (ImGui::Button("Reset")) {
			mesh->computeDomain();
			mesh->Update();
		}

		if (ImGui::BeginCombo("##combo", mesh_id[mesh->domain])) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < IM_ARRAYSIZE(mesh_id); n++)
			{
				bool is_selected = (mesh->domain == n); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(mesh_id[n], is_selected)) {
					mesh->domain = (MESH_)n;
					mesh->Reset();
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}

		ImGui::End();
	}
}