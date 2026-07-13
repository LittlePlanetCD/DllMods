#pragma once
#include "Mod.hpp"
#include <windows.h>

namespace Platform
{
    enum PlatformType
    {
        Steam,
        Epic
    };

    PlatformType DetectPlatform();
    void CreateEngineInfo(EngineInfo* engineInfo, PlatformType platform);
    void ApplyPatches(PlatformType platform);
    void ResetObjects(PlatformType platform);
    HMODULE LoadGameLogicFile(PlatformType platform);
}