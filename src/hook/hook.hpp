#pragma once

namespace Hook
{
    namespace Hook_ImGui{
        void hook_imgui();

        void hook_swap_window();
        void hook_poll_event();

        void unhook_window();
        void unhook_poll_event();
    };
    
    namespace Interfaces{
        void find_interfaces();

        void free_interfaces();
    };
    

    void start_hooking();
    void unhook();
};