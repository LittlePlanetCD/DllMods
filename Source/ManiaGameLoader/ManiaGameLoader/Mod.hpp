#pragma once

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;

#define INT_TO_VOID(x) (void *)(size_t)(x)
#define VOID_TO_INT(x) (int32)(size_t)(x)

typedef struct {
    void *functionTable;
    void *APITable;

    void *gameInfo;
    void *currentSKU;
    void *sceneInfo;

    void *controllerInfo;
    void *stickInfoL;
    void *stickInfoR;
    void *triggerInfoL;
    void *triggerInfoR;
    void *touchInfo;
    void *unknownInfo;
    void *screenInfo;
    void *modTable;
} EngineInfo;

typedef struct {
    // Registration & Core
    void *RegisterGlobals;
    void *RegisterObject;
    void *RegisterObject_STD;
    void *RegisterObjectHook;
    void *FindObject;
    void *GetGlobals;
    void *Super;

    // Mod Info
    void *LoadModInfo;
    void *GetModPath;
    void *GetModCount;
    void *GetModIDByIndex;
    void *ForeachModID;

    // Mod Callbacks & Public Functions
    void *AddModCallback;
    void *AddModCallback_STD;
    void *AddPublicFunction;
    void *GetPublicFunction;

    // Mod Settings
    void * GetSettingsBool;
    void *GetSettingsInteger;
    void *GetSettingsFloat;
    void *GetSettingsString;
    void *SetSettingsBool;
    void *SetSettingsInteger;
    void *SetSettingsFloat;
    void *SetSettingsString;
    void *SaveSettings;

    // Config
    void *GetConfigBool;
    void *GetConfigInteger;
    void *GetConfigFloat;
    void *GetConfigString;
    void *ForeachConfig;
    void *ForeachConfigCategory;

    // Achievements
    void *RegisterAchievement;
    void *GetAchievementInfo;
    void *GetAchievementIndexByID;
    void *GetAchievementCount;

    // Shaders
    void *LoadShader;

    // StateMachine
    void *StateMachineRun;
    void *RegisterStateHook;
    // runs all high priority state hooks hooked to the address of 'state', returns if the main state should be skipped or not
    void *HandleRunState_HighPriority;
    // runs all low priority state hooks hooked to the address of 'state'
    void *HandleRunState_LowPriority;

    // Mod Settings (Part 2)
} ModFunctionTable;