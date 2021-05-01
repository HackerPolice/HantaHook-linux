#include "../pch.hpp"


void Hook::Interfaces::find_interfaces(){
    cvar = GetInterface<ICvar>(XORSTR("./bin/linux64/materialsystem_client.so"), XORSTR("VEngineCvar"));
}

void Hook::Interfaces::show_address(){
    if (!cvar)
        return;


    cvar->ConsoleColorPrintf(ColorRGBA(125, 85, 125), XORSTR("  [Cvar] | %p \n"), cvar);


}
void Hook::Interfaces::free_interfaces(){
    free(cvar);
}