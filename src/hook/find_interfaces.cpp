#include "../pch.hpp"


void Hook::Interfaces::find_interfaces(){
    cvar = GetInterface<ICvar>(XORSTR("./bin/linux64/materialsystem_client.so"), XORSTR("VEngineCvar"));
}

void Hook::Interfaces::free_interfaces(){
    free(cvar);
}