#include "pch.h"
#include <HiteModLoader.h>
#include <Helpers.h>
#include "SigScan.h"
#include "Game.h"

struct VectorInt
{
    int* start;
    int* end;
};

// Too lazy to sig scan these
ObjectZone** Zone = (ObjectZone**)0x143DB5CE0;
ControllerState* controllers = (ControllerState*)0x143729A90;
SceneInfo* sceneInfo = (SceneInfo*)0x143DB14E0;
GlobalVariables* globals = (GlobalVariables*)0x144000210;

HOOK(void, __fastcall, Player_State_KnuxGlideLeft, SigPlayer_State_KnuxGlideLeft(), EntityPlayer* _this)
{
	originalPlayer_State_KnuxGlideLeft(_this);

	if (_this->position.x <= (*Zone)->playerBoundsL[_this->playerID] + 0x100000 && !_this->jumpHold)
	{
		_this->direction = 0;
	}
}

HOOK(void, __fastcall, sub_1401EA5E0, Sigsub_1401EA5E0(), EntityPlayer* player)
{
    if (player->onGround && player->chargeTimer >= 19 && player->jumpHold)
    {
        auto Camera_ShakeScreen = (void(__fastcall*)(int32 screen, int32 shakeX, int32 shakeY, int32 a4, int32 a5))(SigCamera_ShakeScreen());
        if (player->superState == SUPERSTATE_SUPER && player->camera)
            Camera_ShakeScreen(player->camera->screenID, 6, 6, 1, 1);
    }

    originalsub_1401EA5E0(player);
}

HOOK(bool *, __fastcall, sub_1403A2550, Sigsub_1403A2550(), uint32 deviceID)
{
    auto inputs = originalsub_1403A2550(deviceID);
    bool mirror = ((bool(__fastcall*)())(SigIsMirrorMode()))();

    if (deviceID == 0)
    {
        controllers[0].keyUp.press    = inputs[0];
        controllers[0].keyDown.press  = inputs[1];
        controllers[0].keyLeft.press  = inputs[2];
        controllers[0].keyRight.press = inputs[3];
        controllers[0].keyA.press     = inputs[4];
        controllers[0].keyB.press     = inputs[5];
        controllers[0].keyC.press     = inputs[6];
        controllers[0].keyStart.press = inputs[20];
    
        controllers[1].keyUp.press     = inputs[10] | inputs[0];
        controllers[1].keyDown.press   = inputs[11] | inputs[1];
        controllers[1].keyLeft.press   = inputs[12] | inputs[2];
        controllers[1].keyRight.press  = inputs[13] | inputs[3];
        controllers[1].keyA.press      = inputs[14];
        controllers[1].keyB.press      = inputs[15];
        controllers[1].keyC.press      = inputs[16];
        controllers[1].keyX.press      = inputs[17];
        controllers[1].keyY.press      = inputs[18];
        controllers[1].keyZ.press      = inputs[19];
        controllers[1].keyStart.press  = inputs[20];
        controllers[1].keySelect.press = inputs[21];

        if (mirror)
        {
            bool buffer = controllers[1].keyLeft.press;
            controllers[1].keyLeft.press = controllers[1].keyRight.press;
            controllers[1].keyRight.press = buffer;
        }

        // TODO: Not sure what this does, but from what it looks,
        //  it may only affect v4 and v3
        //sub_140101BC0(21);
    }

    // Intentionally fail if id is 0
    if (deviceID == 0)
        return nullptr;
    return inputs;
}

HOOK(void, __fastcall, ActClear_Create, SigActClear_Create(), EntityActClear* self)
{
    originalActClear_Create(self);

    self->coolBonus = 0;
    self->timeBonus = 100;
    switch (sceneInfo->minutes)
    {
    case 0:
        self->timeBonus = 50000;
        break;
    case 1:
        self->timeBonus = sceneInfo->seconds < 30 ? 10000 : 5000;
        break;
    case 2:
        self->timeBonus = sceneInfo->seconds < 30 ? 4000 : 3000;
        break;
    case 3:
        self->timeBonus = sceneInfo->seconds < 30 ? 1000 : 100;
        break;
    case 9:
        if (!sceneInfo->debugMode &&
            globals->gameMode < MODE_TIMEATTACK &&
            sceneInfo->seconds == 59)
        {
            if (!(globals->medalMods & 0x20))
                self->timeBonus = 100000;
        }
        break;
    default:
        break;
    }
}

HOOK(int, __fastcall, sub_140302180, Sigsub_140302180(), int index)
{
    originalsub_140302180(index);

    char* globals_ptr = *(char**)0x144000210;
    bool hasPlus = *(bool32*)(globals_ptr + 0x4C350C);
    bool isAnniversary = *(bool32*)(globals_ptr + 0x4C3510);
    auto vec = (VectorInt*)0x143FB9F88;
    
    if (hasPlus && isAnniversary)
    {
        vec->end = vec->start + 4;
        vec->start[0] = 0;
        vec->start[1] = 1;
        vec->start[2] = 3;
        vec->start[3] = 2;
    }
    else
    {
        vec->end = vec->start + 3;
        vec->start[0] = 0;
        vec->start[1] = 1;
        vec->start[2] = 2;
    }

    return vec->start[index];
}

HOOK(void, __fastcall, S3K_CompElement_Draw, SigS3K_CompElement_Draw(), __int64 a1)
{
    char* globals_ptr = *(char**)0x144000210;
    bool* hasPlus = (bool*)(globals_ptr + 0x4C350C);
    bool* isAnniversary = (bool*)(globals_ptr + 0x4C3510);
    bool temp = *hasPlus;

    // Reuse hasPlus
    *hasPlus = (temp && *isAnniversary);
    originalS3K_CompElement_Draw(a1);
    *hasPlus = temp;
}

HOOK(void, __fastcall, S3K_CompElement_State_Carousel, SigS3K_CompElement_State_Carousel(), __int64 a1)
{
    char* globals_ptr = *(char**)0x144000210;
    bool* hasPlus = (bool*)(globals_ptr + 0x4C350C);
    bool* isAnniversary = (bool*)(globals_ptr + 0x4C3510);
    bool temp = *hasPlus;

    // Reuse hasPlus
    *hasPlus = (temp && *isAnniversary);
    originalS3K_CompElement_State_Carousel(a1);
    *hasPlus = temp;
}

HOOK(void, __fastcall, S3K_CompElement_Create, SigS3K_CompElement_Create(), __int64 a1)
{
    char* globals_ptr = *(char**)0x144000210;
    bool* hasPlus = (bool*)(globals_ptr + 0x4C350C);
    bool* isAnniversary = (bool*)(globals_ptr + 0x4C3510);
    bool temp = *hasPlus;

    // Reuse hasPlus
    *hasPlus = (temp && *isAnniversary);
    originalS3K_CompElement_Create(a1);
    *hasPlus = temp;
}

HOOK(void, __fastcall, LevelSelect_State_Navigate, SigLevelSelect_State_Navigate(), __int64 a1)
{
    char* globals_ptr = *(char**)0x144000210;
    bool* hasPlus = (bool*)(globals_ptr + 0x4C350C);
    bool* isAnniversary = (bool*)(globals_ptr + 0x4C3510);
    bool temp = *hasPlus;

    // Reuse hasPlus
    *hasPlus = (temp && *isAnniversary);
    originalLevelSelect_State_Navigate(a1);
    *hasPlus = temp;
}

extern "C" __declspec(dllexport) void Init(ModInfo * modInfo)
{
    // Check signatures
    if (!SigValid)
    {
        MessageBoxW(nullptr, L"Signature Scan Failed!\n\nThis usually means there is a conflict or the mod is running on an incompatible game version.", L"Scan Error", MB_ICONERROR);
        return;
    }

    INSTALL_HOOK(Player_State_KnuxGlideLeft);
    INSTALL_HOOK(sub_1401EA5E0);
    INSTALL_HOOK(sub_1403A2550);
    INSTALL_HOOK(ActClear_Create);
    INSTALL_HOOK(sub_140302180);
    INSTALL_HOOK(S3K_CompElement_Draw);
    INSTALL_HOOK(S3K_CompElement_State_Carousel);
    INSTALL_HOOK(S3K_CompElement_Create);
    INSTALL_HOOK(LevelSelect_State_Navigate);

    // Fix SpecialClear
    for (int i = 0; i < 2; ++i)
        WRITE_MEMORY(((intptr_t)SigSpecialClear_State_FigureOutWhatToDoNext_D0() + 14 * i), 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90);
    // Fix Amy's jumpOffset
    WRITE_MEMORY(((char*)SigPlayer_Create_266() + 8), 0x04);
}