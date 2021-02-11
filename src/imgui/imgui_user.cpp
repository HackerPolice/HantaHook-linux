#include "imgui.h"
#include "imgui_internal.h"

bool ImGui::BeginTabsEx(const char* name, ImGuiID id, const ImVec2 & size_arg, bool border, int flags)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* parent_window = g.CurrentWindow;

	flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;
	flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);  // Inherit the NoMove flag

	// Size
	const ImVec2 content_avail = GetContentRegionAvail();
	ImVec2 size = ImFloor(size_arg);
	const int auto_fit_axises = ((size.x == 0.0f) ? (1 << ImGuiAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << ImGuiAxis_Y) : 0x00);
	if (size.x <= 0.0f)
		size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)
	if (size.y <= 0.0f)
		size.y = ImMax(content_avail.y + size.y, 4.0f);
	SetNextWindowSize(size);

	// Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(ImGuiID id) with a stable value.
	char title[256];
	if (name)
		ImFormatString(title, IM_ARRAYSIZE(title), "%s/%s_%08X", parent_window->Name, name, id);
	else
		ImFormatString(title, IM_ARRAYSIZE(title), "%s/%08X", parent_window->Name, id);

	const float backup_border_size = g.Style.ChildBorderSize;
	if (!border)
		g.Style.ChildBorderSize = 0.0f;
	bool ret = BeginTabsBackground(title, NULL, flags);
	g.Style.ChildBorderSize = backup_border_size;

	ImGuiWindow * child_window = g.CurrentWindow;
	child_window->ChildId = id;
	child_window->AutoFitChildAxises = auto_fit_axises;

	// Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
	// While this is not really documented/defined, it seems that the expected thing to do.
	if (child_window->BeginCount == 1)
		parent_window->DC.CursorPos = child_window->Pos;

	// Process navigation-in immediately so NavInit can run on first frame
	if (g.NavActivateId == id && !(flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayerActiveMask != 0 || child_window->DC.NavHasScroll))
	{
		FocusWindow(child_window);
		NavInitWindow(child_window, false);
		SetActiveID(id + 1, child_window); // Steal ActiveId with a dummy id so that key-press won't activate child item
		g.ActiveIdSource = ImGuiInputSource_Nav;
	}
	return ret;
}

bool ImGui::BeginTabs(const char* str_id, const ImVec2 & size_arg, bool border, int extra_flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	return BeginTabsEx(str_id, window->GetID(str_id), size_arg, border, extra_flags);
}

bool ImGui::ColorBar(const char* label, const ImVec2& size_arg, int flags)
{
	ImGuiWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;

	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat)
		flags |= ImGuiButtonFlags_Repeat;

	// Render
	auto black = ImGui::GetColorU32(ImVec4(0.0, 0.0, 0.0, 0.43));
	ImVec2 p = ImGui::GetCursorScreenPos();

	window->DrawList->AddRectFilledMultiColor(ImVec2(p.x, p.y - 6), ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y - 4), ImColor(12, 12, 12), ImColor(12, 12, 12), ImColor(12, 12, 12), ImColor(12, 12, 12));
	window->DrawList->AddRectFilledMultiColor(ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y - 6), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 4), ImColor(12, 12, 12), ImColor(12, 12, 12), ImColor(12, 12, 12), ImColor(12, 12, 12));

	window->DrawList->AddRectFilledMultiColor(ImVec2(p.x + 1, p.y - 5), ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y - 3), ImColor(55, 177, 218), ImColor(201, 84, 192), ImColor(201, 84, 192), ImColor(55, 177, 218));
	window->DrawList->AddRectFilledMultiColor(ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y - 5), ImVec2(p.x + ImGui::GetWindowWidth() - 1, p.y - 3), ImColor(201, 84, 192), ImColor(204, 227, 54), ImColor(204, 227, 54), ImColor(201, 84, 192));

	window->DrawList->AddRectFilledMultiColor(ImVec2(p.x + 1, p.y - 4), ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y - 2), black, black, black, black);
	window->DrawList->AddRectFilledMultiColor(ImVec2(p.x + ImGui::GetWindowWidth() / 2, p.y - 4), ImVec2(p.x + ImGui::GetWindowWidth() - 1, p.y - 2), black, black, black, black);

	return false;
}

bool ImGui::Tab(const char *label, bool selected)
{
	ImGuiWindow *window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	ImGuiContext &g = *GImGui;
	const ImGuiStyle &style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(ImVec2(120, 40), label_size.x + style.FramePadding.x * 2.0f,
	                                  label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id)) {
		return false;
	}

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

	if (hovered || held) {
		ImGui::SetMouseCursor(7);
	}

	if (selected) {
		window->DrawList->AddRectFilled(ImVec2(bb.Min.x - 3, bb.Min.y - 1), ImVec2(bb.Max.x + 3, bb.Max.y + 0),
		                                ImColor(220, 60, 40, 255), 8, 3);
	} else {
		if (!hovered) {
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x + 0, bb.Min.y + 0), ImVec2(bb.Max.x + 0, bb.Max.y + 0),
			                                ImColor(160, 30, 30, 255));
		} else {
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x + 0, bb.Min.y + 0), ImVec2(bb.Max.x + 0, bb.Max.y + 0),
			                                ImColor(190, 45, 35, 255));
		}
	}

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, 255 / 255.f));
	ImGui::RenderText(ImVec2(bb.Min.x + 0 + (105 / 2 - label_size.x / 3), bb.Min.y + style.FramePadding.y + 12), label);
	ImGui::PopStyleColor();

	return pressed;
}

bool ImGui::Sub(const char *label, bool selected)
{
	ImGuiWindow *window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}

	ImGuiContext &g = *GImGui;
	const ImGuiStyle &style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	auto draw = ImGui::GetWindowDrawList();

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(ImVec2(157, 40), label_size.x + style.FramePadding.x * 2.0f,
	                                  label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id)) {
		return false;
	}

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

	if (hovered || held) {
		ImGui::SetMouseCursor(7);
	}

	if (selected) {
		window->DrawList->AddRectFilled(ImVec2(bb.Min.x - 3, bb.Min.y - 1), ImVec2(bb.Max.x + 3, bb.Max.y + 0),
		                                ImColor(43, 43, 40, 255));
		draw->AddRectFilled(ImVec2(bb.Min.x - 0, bb.Min.y - 1), ImVec2(bb.Max.x - 147, bb.Max.y + 0),
		                    ImColor(200, 45, 35));
	} else {
		if (!hovered) {
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x + 0, bb.Min.y + 0), ImVec2(bb.Max.x + 0, bb.Max.y + 0),
			                                ImColor(32, 31, 32, 0));
		} else {
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x + 0, bb.Min.y + 0), ImVec2(bb.Max.x + 0, bb.Max.y + 0),
			                                ImColor(32, 31, 32, 255));
		}
	}

	if (selected) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(200 / 255.f, 45 / 255.f, 35 / 255.f, 255 / 255.f));
		ImGui::RenderText(ImVec2(bb.Min.x + 0 + (105 / 2 - label_size.x / 2), bb.Min.y + style.FramePadding.y + 10),
		                  label);
		ImGui::PopStyleColor();
	} else {
		if (!hovered) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 255 / 255.f));
			ImGui::RenderText(ImVec2(bb.Min.x + 0 + (105 / 2 - label_size.x / 2), bb.Min.y + style.FramePadding.y + 10),
			                  label);
			ImGui::PopStyleColor();
		} else {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 255 / 255.f));
			ImGui::RenderText(ImVec2(bb.Min.x + 0 + (105 / 2 - label_size.x / 2), bb.Min.y + style.FramePadding.y + 10),
			                  label);
			ImGui::PopStyleColor();
		}
	}
	return pressed;

}

void ImGui::EndTabs()
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;

	IM_ASSERT(window->Flags & ImGuiWindowFlags_ChildWindow);   // Mismatched BeginChild()/EndChild() callss
	if (window->BeginCount > 1)
	{
		End();
	}
	else
	{
		ImVec2 sz = window->Size;
		if (window->AutoFitChildAxises & (1 << ImGuiAxis_X)) // Arbitrary minimum zero-ish child size of 4.0f causes less trouble than a 0.0f
			sz.x = ImMax(4.0f, sz.x);
		if (window->AutoFitChildAxises & (1 << ImGuiAxis_Y))
			sz.y = ImMax(4.0f, sz.y);
		End();

		ImGuiWindow * parent_window = g.CurrentWindow;
		ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
		ItemSize(sz);
		if ((window->DC.NavLayerActiveMask != 0 || window->DC.NavHasScroll) && !(window->Flags & ImGuiWindowFlags_NavFlattened))
		{
			ItemAdd(bb, window->ChildId);
			RenderNavHighlight(bb, window->ChildId);

			// When browsing a window that has no activable items (scroll only) we keep a highlight on the child
			if (window->DC.NavLayerActiveMask == 0 && window == g.NavWindow)
				RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_TypeThin);
		}
		else
		{
			// Not navigable into
			ItemAdd(bb, 0);
		}
	}
}

void ImGui::InsertSliderWithoutText(const char *label, float *v, float v_min, float v_max, const char *format, float power){

	ImGui::Spacing(); 
	ImGui::Spacing(); 
	ImGui::NewLine(); 
	ImGui::SameLine(42.f); 
	ImGui::PushItemWidth(159.f); 
	ImGui::SliderFloat(label, v, v_min, v_max, format); 
	ImGui::PopItemWidth();
}

#include <string>

void ImGui::ColorPicker(const char* name, float* color, bool alpha) {

    ImGui::SameLine(219.f);
	auto alphaSliderFlag = alpha ? ImGuiColorEditFlags_AlphaBar : ImGuiColorEditFlags_NoAlpha;

	ImGui::SameLine(219.f);
	ImGui::ColorEdit4(std::string{ "##" }.append(name).append("Picker").c_str(), color, alphaSliderFlag | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
}
