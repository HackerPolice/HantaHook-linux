#include "tabs.hpp"


static bool test = false;

static void aim_bot(){
    
}

void LegitBotTab::Render(ImDrawList * draw, ImVec2 &pos){

    draw->AddRectFilled(ImVec2(pos.x + 180, pos.y + 65), ImVec2(pos.x + 960 - 15, pos.y + 95), ImColor(0, 0, 0, 150),10);
    ImGui::SetCursorPos(ImVec2(185, 70));
    ImGui::BeginGroup();
    {
        if (ImGui::CheckboxFill(XORSTR("##Enabled"), &settings::Legit_bot::is_enable));

    }ImGui::EndGroup();

    if (!settings::Legit_bot::is_enable)
        return; // return if legit bot not enable

    static ImColor c = ImColor(100,200,100,100);
    
    ImGui::SetCursorPos(ImVec2(180, 100));
    ImGui::BeginGroup();
    {
        ImGui::Columns(2, nullptr, false);
        {
            ImGui::CheckboxFill(XORSTR("##test"), &test);
        }
    }ImGui::EndGroup();
    

    
    
}