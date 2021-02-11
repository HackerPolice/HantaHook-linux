#pragma once

/**
 * @file pch.hpp
 * @author Pritam Dutta 
 * @brief Pre Compiled Header
 * @version 0.1
 * @date 2021-02-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <cstdint>
#include <thread>
#include <dlfcn.h>
#include <vector>
#include <string>
#include <SDL2/SDL.h>

typedef void (*SDL_GL_SwapWindow_t)(SDL_Window *);
typedef int (*SDL_PollEvent_t)(SDL_Event *);

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/examples/libs/gl3w/GL/gl3w.h"
#include "imgui/examples/imgui_impl_opengles3.h"

#include "settings.hpp"
#include "gui/tabs/tabs.hpp"

#include "utils/utils.h"

#include "csgo_sdk/sdk.h"
#include "hook/hook.hpp"

#include "gui/gui.hpp"


namespace Global{
    
    // all the interfaces
    inline ICvar *cvar = nullptr;
};

using namespace Global;