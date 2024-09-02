#pragma once

#include <glm/glm.hpp>
#include <string>
#include <imgui.h>
#include "Camera.h"
#include "Shader.h"
#include "colors.h"
#include "Diagnostics.h"
#include "PostProccessing.h"

#define GUI_WINDOW_FLAGS	ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs
#define GUI_TABLE_FLAGS		ImGuiTableFlags_BordersInner | ImGuiTableFlags_NoPadInnerX
#define GUI_PLOT_FLAGS		ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame
#define GUI_AXIS_FLAGS_X	ImPlotAxisFlags_NoDecorations
#define GUI_AXIS_FLAGS_Y	ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_AutoFit

#define GRAPH_RESOLUTION	128
#define GRAPH_TIMESPAN_MS	20000

namespace window {
	void gui_drawRect(ImVec2 pos, ImVec2 size, ImU32 color);
};