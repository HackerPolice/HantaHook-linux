#include "../pch.hpp"

static bool set_up_done = false;

void Gui::set_up_color(){

    if (set_up_done)
        return;
    
    set_up_done = true;
    
    ImGuiStyle *style = &ImGui::GetStyle();
	ImGui::StyleColorsDark();

	style->Alpha = 1.f;
	style->WindowRounding = 1.f;
	style->FramePadding = ImVec2(4, 3);
	style->WindowPadding = ImVec2(8, 8);
	style->ItemInnerSpacing = ImVec2(4, 4);
	style->ItemSpacing = ImVec2(0.f, -1.f);
	style->FrameRounding = 5.f;
	style->ScrollbarSize = 2.f;
	style->ScrollbarRounding = 12.f;

	ImVec4 *colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_ChildBg] = ImColor(24, 29, 59, 0);
	colors[ImGuiCol_Border] = ImVec4(0.07f, 0.07f, 0.11f, 0.50f);
	colors[ImGuiCol_FrameBg] = ImColor(25, 25, 33, 255); // Check Box
	colors[ImGuiCol_FrameBgActive] = ImColor(25, 25, 33, 255);
	colors[ImGuiCol_FrameBgHovered] = ImColor(25, 25, 33, 255);
	colors[ImGuiCol_Header] = ImColor(25, 25, 152, 255);
	colors[ImGuiCol_HeaderActive] = ImColor(28, 28, 36, 255);
	colors[ImGuiCol_HeaderHovered] = ImColor(30, 30, 38, 255);
	colors[ImGuiCol_PopupBg] = ImColor(35, 35, 45, 255);
	colors[ImGuiCol_Button] = ImColor(160, 30, 30, 255);
	colors[ImGuiCol_ButtonHovered] = ImColor(190, 45, 35, 255);
	colors[ImGuiCol_ButtonActive] = ImColor(220, 60, 40, 255);
}