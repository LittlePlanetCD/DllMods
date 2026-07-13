#include "ModAPI.hpp"
#include "Helpers.h"
#include "Platform.hpp"

// Dummy implementation of ModAPI
namespace ModAPI
{
    std::vector<void (*)(void*)> ModCallbacks[MODCB_MAX];

    void Mod_StateMachineRun(void (*state)(void))
    {
        if (state)
            state();
    }

    void AddModCallback(int32 callbackID, void (*callback)(void*))
    {
        if (callbackID >= 0 && callbackID < MODCB_MAX)
            ModCallbacks[callbackID].push_back(callback);
    }

    HOOK(void, __cdecl, ProcessObjects, ASLR(0x5F27A0))
    {
        originalProcessObjects();

        // None of these are at the right timing, but close enough?
        for (auto callback : ModCallbacks[MODCB_ONUPDATE])
            callback(INT_TO_VOID(1));
        for (auto callback : ModCallbacks[MODCB_ONLATEUPDATE])
            callback(INT_TO_VOID(1));
    }

    void ApplyHooks(Platform::PlatformType platform)
    {
        // Change address for Epic
        if (platform == Platform::Epic)
            originalProcessObjects = (ProcessObjects*)ASLR(0x005EE750);
        INSTALL_HOOK(ProcessObjects);
    }

}