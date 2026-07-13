#include "pch.h"
#include <HiteModLoader.h>
#include <Helpers.h>
#include "SigScan.h"
#include "CriWare.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <thread>
#include <chrono>

// CRIWARE and HEDGE STUFF !!!!!
FUNCTION_PTR(int64_t, __fastcall, ResourceLoaderConstructor, 0x1515abd90, ResourceLoader* resLoader, int64_t allocator);
FUNCTION_PTR(void, __fastcall, ResourceLoaderLoadResource, 0x14077f4e0, ResourceLoader* resLoader, InplaceTempUri* uri, int64_t resType, int unk2, int unk3, Locale* locale);
FUNCTION_PTR(MemoryRouter*, __fastcall, MemoryRouterGetInstance, 0x14077c0a0);
FUNCTION_PTR(int64_t, __fastcall, ResAtomCueSheetGetTypeInfo, 0x1404c1d20);
FUNCTION_PTR(void*, __fastcall, AllocatorAlloc, 0x14077BF20, int64_t allocator, size_t size, size_t alignment);
FUNCTION_PTR(InplaceTempUri*, __fastcall, UriConstructor, 0x14077C210, InplaceTempUri* uri, const char* buffer, size_t bufferSize, const char* str, size_t strlen, int64_t allocator);

template<size_t len>
void loadAcb(const char (&str)[len]) {
    int64_t allocator = MemoryRouterGetInstance()->moduleAllocator; //0x14077C0A0, return type can be just int64_t
    ResourceLoader* resLoader = (ResourceLoader*)AllocatorAlloc(allocator, sizeof(ResourceLoader), 8);
    ResourceLoaderConstructor(resLoader, allocator); //0x1515ABD90
    InplaceTempUri* uri = (InplaceTempUri*)AllocatorAlloc(allocator, 0xA0, 8);
    uri = UriConstructor(uri, uri->bufferr, 0x80, str, len, allocator);
    Locale* locale = (Locale*)AllocatorAlloc(allocator, sizeof(Locale), 8);
    locale->localeId = 1;
    locale->localeName = "";
    ResourceLoaderLoadResource( //0x14077F4E0
        resLoader,
        uri,
        ResAtomCueSheetGetTypeInfo(), //0x1404C1D20, return type can be just int64_t
        0,
        1,
        locale);
};


HOOK(int64_t, __fastcall, FUNC_1403435B0, 0x1403435B0, int64_t a1)
{
    loadAcb("sound/[AudioFileNameHere].acb"); // Change this / add new ones to load custom ACB files!
    return originalFUNC_1403435B0(a1);
}

extern "C" __declspec(dllexport) void PostInit()
{
    // Install hooks
    INSTALL_HOOK(FUNC_1403435B0);
}

extern "C" __declspec(dllexport) void Init(ModInfo* modInfo)
{
    // Scan signatures

    // Check signatures
    if (!SigValid)
    {
        MessageBoxA(nullptr, InvalidSig, "", NULL);
        MessageBoxW(nullptr, L"Signature Scan Failed!\n\nThis usually means there is a conflict or the mod is running on an incompatible game version.", L"Scan Error", MB_ICONERROR);
        return;
    }
}