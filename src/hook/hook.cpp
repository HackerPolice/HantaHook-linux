#include "../pch.hpp"

void Hook::start_hooking(){
    Hook_ImGui::hook_imgui();
    Interfaces::find_interfaces();
}

void Hook::unhook(){
    Hook_ImGui::unhook_window();
    Hook_ImGui::unhook_poll_event();
}