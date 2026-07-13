#include "Platform.hpp"
#include "Helpers.h"

namespace Platform
{
    PlatformType DetectPlatform()
    {
        return (strcmp((char*)ASLR(0x006179A0), "LinkGameLogicDLL") == 0) ? Platform::Epic : Platform::Steam;
    }

    void CreateEngineInfo(EngineInfo* engine, PlatformType platform)
    {
        switch (platform)
        {
        default:
        case PlatformType::Steam:
            engine->functionTable = (void*)ASLR(0x00E78C98);
            engine->APITable = (void*)ASLR(0x0066A280);
            engine->gameInfo = (void*)ASLR(0x0083C4C8);
            engine->currentSKU = (void*)ASLR(0x00E53584);
            engine->sceneInfo = (void*)ASLR(0x00E535AC);
            engine->controllerInfo = (void*)ASLR(0x008416D8);
            engine->stickInfoL = (void*)ASLR(0x0083D330);
            engine->stickInfoR = (void*)ASLR(0x00841570);
            engine->triggerInfoL = (void*)ASLR(0x0083D1A8);
            engine->triggerInfoR = (void*)ASLR(0x0083D248);
            engine->touchInfo = (void*)ASLR(0x0083D498);
            engine->unknownInfo = (void*)ASLR(0x0083D2F8);
            engine->screenInfo = (void*)ASLR(0x00AECA88);
            break;
        case PlatformType::Epic:
            engine->functionTable = (void*)ASLR(0x00E31770);
            engine->APITable = (void*)ASLR(0x00623200);
            engine->gameInfo = (void*)ASLR(0x006B57C8);
            engine->currentSKU = (void*)ASLR(0x00E0C0D0);
            engine->sceneInfo = (void*)ASLR(0x00E0C0E0);
            engine->controllerInfo = (void*)ASLR(0x007FA0E0);
            engine->stickInfoL = (void*)ASLR(0x007F5D38);
            engine->stickInfoR = (void*)ASLR(0x007F9F78);
            engine->triggerInfoL = (void*)ASLR(0x007FA3B0);
            engine->triggerInfoR = (void*)ASLR(0x007FA450);
            engine->touchInfo = (void*)ASLR(0x007F5EA0);
            engine->unknownInfo = (void*)ASLR(0x007F5D08);
            engine->screenInfo = (void*)ASLR(0x00AA76E8);
            break;
        }
    }

    void ApplyPatches(PlatformType platform)
    {
        switch (platform)
        {
        default:
        case PlatformType::Steam:
            // Prevent global allocation
            WRITE_MEMORY(ASLR(0x005A6E97), (char)0x90, (char)0x90, (char)0x90, (char)0x90, (char)0x90, (char)0x90);
            // Fix static loading
            WRITE_MEMORY(ASLR(0x005EF9ED), (char)0x04);
            break;
        case PlatformType::Epic:
            // Prevent global allocation
            WRITE_MEMORY(ASLR(0x005A4E97), (char)0x90, (char)0x90, (char)0x90, (char)0x90, (char)0x90, (char)0x90);
            // Fix static loading
            WRITE_MEMORY(ASLR(0x005EBB26), (char)0x04);
            break;
        }
    }

    void ResetObjects(PlatformType platform)
    {
        // Sets object count to 2
        // First two objects are from the engine
        switch (platform)
        {
        default:
        case PlatformType::Steam:
            *((uint16*)ASLR(0x0083D56C)) = 2;
            break;
        case PlatformType::Epic:
            *((uint16*)ASLR(0x00AA55C8)) = 2;
            break;
        }
    }

    HMODULE LoadGameLogicFile(PlatformType platform)
    {
        HMODULE handle = NULL;
        switch (platform)
        {
        default:
            handle = LoadLibraryA("Game.dll");
            break;
        case PlatformType::Steam:
            handle = LoadLibraryA("GameSteam.dll");
            break;
        case PlatformType::Epic:
            handle = LoadLibraryA("GameEpic.dll");
            break;
        }
        
        // Fallback to Game.dll
        if (!handle)
            handle = LoadLibraryA("Game.dll");

        return handle;
    }
}