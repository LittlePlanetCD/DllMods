#include "Mod.hpp"
#include "Platform.hpp"
#include "ModAPI.hpp"
#include <windows.h>
#include "Helpers.h"

extern "C"
{
    HMODULE gameHandle = NULL;
    Platform::PlatformType platform = Platform::PlatformType::Steam;

    void BlankFunc() {}

    __declspec(dllexport) void LinkGameLogic()
    {
        // Reset
        for (auto& callbacks : ModAPI::ModCallbacks)
            callbacks.clear();

        Platform::ResetObjects(platform);

        if (!gameHandle)
        {
            MessageBoxA(NULL, "No Game Logic Loaded!", "ManiaGameLoader", MB_ICONERROR | MB_OK);
            return;
        }

        EngineInfo engine;
        Platform::CreateEngineInfo(&engine, platform);

        ModFunctionTable mod =
        {
            // Registration & Core
            BlankFunc, BlankFunc, BlankFunc, BlankFunc, BlankFunc, BlankFunc, BlankFunc,

            // Mod Info
            BlankFunc, BlankFunc, BlankFunc, BlankFunc, BlankFunc,
            
            // Mod Callbacks & Public Functions
            ModAPI::AddModCallback,
            BlankFunc, BlankFunc, BlankFunc,

            // Mod Settings
            BlankFunc, BlankFunc, BlankFunc, BlankFunc, BlankFunc, BlankFunc, BlankFunc, BlankFunc, BlankFunc,

            // Config
            BlankFunc, BlankFunc, BlankFunc, BlankFunc, BlankFunc, BlankFunc,

            // Achievements
            BlankFunc, BlankFunc, BlankFunc, BlankFunc,

            // Shaders
            BlankFunc,

            // StateMachine
            ModAPI::Mod_StateMachineRun,
            BlankFunc, BlankFunc, BlankFunc,
        };
        engine.modTable = &mod;

        // Setup game
        void (*linkGameLogicDLL)(void*) = (void(*)(void*))GetProcAddress(gameHandle, "LinkGameLogicDLL");

        if (linkGameLogicDLL)
            linkGameLogicDLL(&engine);
        else
            MessageBoxA(NULL, "Invalid game logic file!", "ManiaGameLoader", MB_ICONERROR | MB_OK);
    }

    __declspec(dllexport) void Init(const char* path)
    {
        platform = Platform::DetectPlatform();

        // Load Game.dll
        char currentDir[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, currentDir);
        while (gameHandle == nullptr)
        {
            SetCurrentDirectoryA(path);
            gameHandle = Platform::LoadGameLogicFile(platform);
            SetCurrentDirectoryA(currentDir);
            if (gameHandle)
            {
                // Check revision
                int* revision = (int*)GetProcAddress(gameHandle, "RSDKRevision");
                if (revision && *revision != 2)
                {
                    FreeLibrary(gameHandle);
                    gameHandle = nullptr;
                    if (MessageBoxA(NULL, "Game logic revision mismatch\n\nMake sure the game logic is compiled for rev2",
                        "ManiaGameLoader", MB_ICONERROR | MB_RETRYCANCEL) == IDCANCEL)
                        break;
                }
            }
            else
            {
                if (MessageBoxA(NULL, "Failed to load game logic file", "ManiaGameLoader",
                    MB_ICONERROR | MB_RETRYCANCEL) == IDCANCEL)
                    break;
            }

        }

        ModAPI::ApplyHooks(platform);
        Platform::ApplyPatches(platform);
    }
}