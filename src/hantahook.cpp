#include "pch.hpp"

// Thanks to fuzion project
/**
 * @file hantahook.cpp
 * @author Pritam Dutta (rohitdutta907@gmail.com)
 * @brief Main File for HantaHook a hack for CSGO
 * @version 0.1
 * @date 2021-02-10
 * 
 * @copyright Copyright (c) 2021
 */

static void Main_Thread();

/* Entrypoint to the Library. Called when loading */
int __attribute__((constructor)) Start()
{
	Main_Thread();
	// main.detach();

	// cvar->ConsoleColorPrintf(ColorRGBA(0, 225, 0), XORSTR("\nHantaHook Successfully loaded.\n"));
	return 0;
}

/* Called when un-injecting the library */
void __attribute__((destructor)) Shutdown()
{
	cvar->ConsoleColorPrintf(ColorRGBA(0, 225, 0), XORSTR("HantaHook Successfully unloaded.\n"));
	cvar->FindVar(XORSTR("cl_mouseenable"))->SetValue(1);
	Hook::unhook();
}

static void Main_Thread(){
    
	std::thread(Hook::start_hooking).detach();
	// Hook::start_hooking();
	
	// cvar->ConsoleColorPrintf(ColorRGBA(0, 225, 0), XORSTR("HantaHook Successfully loaded.\n"));
	// cvar->ConsoleDPrintf(XORSTR("Done"));
}