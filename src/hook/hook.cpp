#include "../pch.hpp"

void Hook::start_hooking(){
    Interfaces::find_interfaces();
    Interfaces::show_address();
    Hook_ImGui::hook_imgui();
}

void Hook::unhook(){
    Hook_ImGui::unhook_window();
    Hook_ImGui::unhook_poll_event();
}