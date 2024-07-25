#include "pch.h"
#include <HiteModLoader.h>
#include <Helpers.h>
#include "SigScan.h"
#include "Data.hpp"
#include "Mod.hpp"
#include "Debug.hpp"

const char* drawGroupNames[16] = 
{
	"Draw Group 0", "Draw Group 1", "Draw Group 2", "Draw Group 3",
	"Draw Group 4", "Draw Group 5", "Draw Group 6", "Draw Group 7",
	"Draw Group 8", "Draw Group 9", "Draw Group 10", "Draw Group 11",
	"Draw Group 12", "Draw Group 13", "Draw Group 14", "Draw Group 15",
};

void** FunctionTable;
bool devButtonPressed = false;
bool fastButtonPressed = false;
bool functionButtonPressed[12]{};
int stepTimer = 0;

bool printWorkaround = false;
bool showPaletteOverlay = false;

uint32 GetPaletteEntry(uint8 bankID, uint8 index)
{
	switch (engineVersion)
	{
	default:
		return 0;
	case 5:
		return ((uint32(__fastcall*)(uint8 bankID, uint8 index))(FunctionTable[FunctionTable_GetPaletteEntry]))(bankID, index);
	case 4:
	case 3:
		auto legacyPalette = (uint16*)0x14292EF70;
		auto clr = legacyPalette[256 * bankID + index];

		int32 R = (clr & 0xF800) << 8;
		int32 G = (clr & 0x7E0) << 5;
		int32 B = (clr & 0x1F) << 3;
		return R | G | B;
	}
}

void DrawDebug()
{
	if (showPaletteOverlay)
	{
		// Use first screen
		*currentScreen_ptr = &screens[0];
		auto DrawRectangle = (void(__fastcall*)(int32 x, int32 y, int32 width, int32 height, uint32 color, int32 alpha, int32 inkEffect, bool32 screenRelative))(SigDrawRectangle());

		for (int32 p = 0; p < 8; ++p)
		{
			int32 x = screens[0].size.x - 128;
			int32 y = screens[0].size.y - 64;

			for (int32 i = 0; i < 16 * 16; ++i)
			{
				uint32 clr = GetPaletteEntry(p, i);

				DrawRectangle(x + (i & 0xF) * 2 + (p % 4) * 32,
					y + ((i >> 4) << 1) + ((p / 4) * 32), 2, 2, clr, 0xFF, INK_NONE, true);
			}
		}
	}
}

// The engine normally clears the variables,
//  but since it does not use the function table it got nuked
//  so we have to readd it. For now a hook will do.
HOOK(void, __fastcall, LoadSceneFolder, SigLoadSceneFolder())
{
	ClearViewableVariables();
	originalLoadSceneFolder();
}

HOOK(void, __fastcall, InitObjects, SigInitObjects())
{
	originalInitObjects();
	AddViewableVariable("Show Palettes", &showPaletteOverlay, VIEWVAR_BOOL, false, true);
	for (int i = 0; i < 16; ++i)
		AddViewableVariable(drawGroupNames[i], &drawGroupVisible[i], VIEWVAR_BOOL, false, true);
}


HOOK(void, __fastcall, ProcessObjectDrawLists, SigProcessObjectDrawLists())
{
	originalProcessObjectDrawLists();
	DrawDebug();
}

HOOK(void, __fastcall, ProcessStagev3, SigProcessStagev3())
{
	if (sceneInfo.state & ENGINESTATE_STEPOVER && !frameStep)
		return;
	else if (sceneInfo.state & ENGINESTATE_STEPOVER && frameStep)
		frameStep = false;

	originalProcessStagev3();
	for (int i = 1; i < gameSpeed; ++i)
		originalProcessStagev3();
	DrawDebug();
}

HOOK(void, __fastcall, ProcessStagev4, SigProcessStagev4())
{
	if (sceneInfo.state & ENGINESTATE_STEPOVER && !frameStep)
		return;
	else if (sceneInfo.state & ENGINESTATE_STEPOVER && frameStep)
		frameStep = false;

	originalProcessStagev4();
	for (int i = 1; i < gameSpeed; ++i)
		originalProcessStagev4();
	DrawDebug();
}

HOOK(void, __fastcall, StartGameObjects, SigStartGameObjects())
{
	originalStartGameObjects();

	FunctionTable = *(void***)0x142E70150;
	FunctionTable[FunctionTable_ClearDebugValues] = ClearViewableVariables;
	FunctionTable[FunctionTable_SetDebugValue] = AddViewableVariable;
}

// Workaround the mod loader
HOOK(void, __fastcall, PrintLog, ((intptr_t)SigPrintLog_() - 0x05), PrintModes printMode, const char* format, const char* p1, const char* p2, const char* p3)
{
	if (printMode == PRINT_SCRIPTERR)
	{
		sprintf_s(scriptErrorMessage, format, p1, p2, p3);
		
		// No need to report back to the mod loader
		return;
	}

	originalPrintLog(printMode, format, p1, p2, p3);
}

void RestartStage()
{
	switch (engineVersion)
	{
	case 5:
		sceneInfo.state = ENGINESTATE_LOAD;
		break;
	case 4:
	case 3:
		gameMode = ENGINE_MAINGAME;
		stageMode = STAGEMODE_LOAD;
		break;
	default:
		break;
	}
}

extern "C" __declspec(dllexport) void OnFrame()
{
	// Make sure to install the hook after the mod loader
	if (!printWorkaround)
	{
		INSTALL_HOOK(PrintLog);
		printWorkaround = true;
	}
}

extern "C" __declspec(dllexport) void OnRsdkFrame()
{
	// Process script error
	if ((engineVersion == 3 || engineVersion == 4)
		&& sceneInfo.state != ENGINESTATE_DEVMENU
		&& gameMode == ENGINE_SCRIPTERROR)
	{
		DisplayScriptError();

		if (controller->keyStart.press || controller->keyA.press)
		{
			OpenDevMenu();
		}
		else if (controller->keyB.press)
		{
			sceneInfo.activeCategory = 0;
			gameMode = ENGINE_MAINGAME;
			stageMode = STAGEMODE_LOAD;
			sceneInfo.listPos = 0;
		}
		else if (controller->keyC.press)
		{
			gameMode = ENGINE_INITDEVMENU;
			stageMode = STAGEMODE_LOAD;
		}
	}

	// Toggle dev menu
	bool devButtonDown = GetAsyncKeyState(VK_OEM_3) != 0;
	if (devButtonDown && !devButtonPressed)
	{
		ToggleDevMenu();
		devButtonPressed = true;
	}
	else if (!devButtonDown)
		devButtonPressed = false;

	// Fast forward
	bool fastButtonDown = GetAsyncKeyState(VK_BACK) != 0;
	if (fastButtonDown && !fastButtonPressed)
	{
		gameSpeed = 8;
		fastButtonPressed = true;
	}
	else if (!fastButtonDown && fastButtonPressed)
	{
		gameSpeed = 1;
		fastButtonPressed = false;
	}
	else if (!fastButtonDown)
		fastButtonPressed = false;

	// Back scene
	bool f1ButtonDown = GetAsyncKeyState(VK_F1) != 0;
	if (f1ButtonDown && !functionButtonPressed[0])
	{
		sceneInfo.listPos--;
		if (sceneInfo.listPos < sceneInfo.listCategory[sceneInfo.activeCategory].sceneOffsetStart
			|| sceneInfo.listPos >= sceneInfo.listCategory[sceneInfo.activeCategory].sceneOffsetEnd)
		{
			sceneInfo.activeCategory--;
			if (sceneInfo.activeCategory >= sceneInfo.categoryCount)
				sceneInfo.activeCategory = sceneInfo.categoryCount - 1;
			sceneInfo.listPos = sceneInfo.listCategory[sceneInfo.activeCategory].sceneOffsetEnd - 1;
		}
		RestartStage();
		functionButtonPressed[0] = true;
	}
	else if (!f1ButtonDown)
		functionButtonPressed[0] = false;

	// Next scene
	bool f2ButtonDown = GetAsyncKeyState(VK_F2) != 0;
	if (f2ButtonDown && !functionButtonPressed[1])
	{
		sceneInfo.listPos++;
		if (sceneInfo.listPos >= sceneInfo.listCategory[sceneInfo.activeCategory].sceneOffsetEnd || sceneInfo.listPos == 0)
		{
			sceneInfo.activeCategory++;
			if (sceneInfo.activeCategory >= sceneInfo.categoryCount)
				sceneInfo.activeCategory = 0;
			sceneInfo.listPos = sceneInfo.listCategory[sceneInfo.activeCategory].sceneOffsetStart;
		}
		RestartStage();
		functionButtonPressed[1] = true;
	}
	else if (!f2ButtonDown)
		functionButtonPressed[1] = false;

	// Reload scene
	bool f5ButtonDown = GetAsyncKeyState(VK_F5) != 0;
	if (f5ButtonDown && !functionButtonPressed[4])
	{
		RestartStage();
		functionButtonPressed[4] = true;
	}
	else if (!f5ButtonDown)
		functionButtonPressed[4] = false;

	// Show palette overlay
	bool f10ButtonDown = GetAsyncKeyState(VK_F10) != 0;
	if (f10ButtonDown && !functionButtonPressed[9])
	{
		showPaletteOverlay = !showPaletteOverlay;
		functionButtonPressed[9] = true;
	}
	else if (!f10ButtonDown)
		functionButtonPressed[9] = false;

	// Frame step
	bool f11ButtonDown = GetAsyncKeyState(VK_F11) != 0;
	if (f11ButtonDown)
	{
		++stepTimer;
		if (!functionButtonPressed[10] || (stepTimer > 30 && (stepTimer & 1) == 0))
			frameStep = true;
		functionButtonPressed[10] = true;
	}
	else if (!f11ButtonDown)
	{
		stepTimer = 0;
		functionButtonPressed[10] = false;
	}

	// Pause
	bool f12ButtonDown = GetAsyncKeyState(VK_F12) != 0;
	if (f12ButtonDown && !functionButtonPressed[11])
	{
		if (sceneInfo.state != ENGINESTATE_NONE)
			sceneInfo.state ^= ENGINESTATE_STEPOVER;
		functionButtonPressed[11] = true;
	}
	else if (!f12ButtonDown)
		functionButtonPressed[11] = false;

}


extern "C" __declspec(dllexport) void Init(ModInfo* modInfo)
{
	// Check signatures
	if (!SigValid)
	{
		MessageBoxW(nullptr, L"Signature Scan Failed!\n\nThis usually means there is a conflict or the mod is running on an incompatible game version.", L"Scan Error", MB_ICONERROR);
		return;
	}

	INSTALL_HOOK(LoadSceneFolder);
	INSTALL_HOOK(InitObjects);
	INSTALL_HOOK(ProcessObjectDrawLists);
	INSTALL_HOOK(ProcessStagev3);
	INSTALL_HOOK(ProcessStagev4);
	INSTALL_HOOK(StartGameObjects);
}