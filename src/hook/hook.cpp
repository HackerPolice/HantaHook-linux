#include "../pch.hpp"

void Hook::start_hooking(){
    Hook_ImGui::hook_imgui();
    Interfaces::find_interfaces();
    Interfaces::show_address();
}

void Hook::unhook(){
    Hook_ImGui::unhook_window();
    Hook_ImGui::unhook_poll_event();
}