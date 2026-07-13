#pragma once
#include "Mod.hpp"
#include "Platform.hpp"
#include <vector>

namespace ModAPI
{
    enum ModCallbackEvents
    {
        MODCB_ONGAMESTARTUP,
        MODCB_ONSTATICLOAD,
        MODCB_ONSTAGELOAD,
        MODCB_ONUPDATE,
        MODCB_ONLATEUPDATE,
        MODCB_ONSTATICUPDATE,
        MODCB_ONDRAW,
        MODCB_ONSTAGEUNLOAD,
        MODCB_ONSHADERLOAD,
        MODCB_ONVIDEOSKIPCB,
        MODCB_ONSCANLINECB,
        MODCB_MAX,
    };

    extern std::vector<void (*)(void*)> ModCallbacks[MODCB_MAX];

    void Mod_StateMachineRun(void (*state)(void));
    void AddModCallback(int32 callbackID, void (*callback)(void*));

    void ApplyHooks(Platform::PlatformType platform);
}