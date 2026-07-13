// dllmain.cpp : Defines the entry point for the DLL application.
#include "windows.h"
#include <ManiaModLoader.h>
#include "Platform.hpp"

extern "C" __declspec(dllexport) ModInfo ManiaModInfo = { ModLoaderVer, GameVer };

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
        // Big brain
        if (Platform::DetectPlatform() == Platform::PlatformType::Epic)
            ManiaModInfo.GameVersion = 6;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

