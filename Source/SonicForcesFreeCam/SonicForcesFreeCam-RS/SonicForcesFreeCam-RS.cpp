#include "stdafx.h"
#include <memory>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include "ForcesModLoader.h"
#define HEDGEEDIT

extern "C"
{
    size_t* ControllerAddress = 0;
    size_t* ControllerLSXAddress = 0;
    size_t* ControllerLSYAddress = 0;
    size_t* ControllerRSXAddress = 0;
    size_t* ControllerRSYAddress = 0;
    BYTE* TimerAddress = 0;
    void* Cam1PosAddy = 0;
    void* YVeloAddy = 0;
    void* YVeloAddy2 = 0;
    void* Whatever = 0;
    bool StillPressing = 0;
    bool StillPressing2 = 0;
    bool FreeCamMode = 0;
    short oldWidth = 0;

    float CameraRotationOffsetYaw = 0.0f;
    float CameraRotationOffsetPitch = 0.0f;
    float* CameraRot = 0;
    float prevX = 0.0f;
    float prevY = 0.0f;
    float prevZ = 0.0f;


    // 1406E6A75:
    // cmp[rcx + 78], rbx
    // jna 1406E69DA
    // mov r12, 0000000000000000
    // mov[r12], rcx
    // xor r12, r12
    // jmp 1406E69BD
    BYTE* GetWhateverHook = new BYTE[23]
    {
        0x48, 0x89, 0x0C, 0x25, 0x00, 0x00, 0x00, 0x00,
        0x48, 0x39, 0x59, 0x78,
        0x0F, 0x86, 0x63, 0xFE, 0xFF, 0xFF,
        0xE9, 0x41, 0xFE, 0xFF, 0xFF
    };

    // mov r12,0000000000000000
    // movsd [r12],xmm0
    // jmp 1405C3C42
    BYTE* GetRotationHook = new BYTE[21]
    {
        0x49, 0xBC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xF2, 0x41, 0x0F, 0x11, 0x04, 0x24,
        0xE9, 0x31, 0xFE, 0xFF, 0xFF
    };


    void WriteCamLock()
    {
        //if (!TimerAddress)
        {
            size_t* pointer = (size_t*)0x1420707B0;
            if (!*pointer)
                return;
            pointer = (size_t*)((*pointer) + 0x080);
            if (!*pointer)
                return;
            pointer = (size_t*)((*pointer) + 0x080);
            if (!*pointer)
                return;
            pointer = (size_t*)((*pointer) + 0x3F0);
            TimerAddress = (BYTE*)pointer;
        }


        if (!FreeCamMode)
        {
            WriteData((__int64*)0x1405C3C78, (__int64)0x8B80909090909090);     // nop
            WriteData((__int64*)0x1405C3C6A, (__int64)0x9090909090909090);     // nop
            WriteData((__int64*)0x1405C3C2E, (__int64)0xF290909090909090);     // nop
            WriteData((__int64*)0x1405C3C5C, (__int64)0x0FF2909090909090);     // nop
            //WriteData((__int64*)0x1405C3C4F, (__int64)0x8B90909090909090);     // nop                   Roll
            WriteData((__int64*)0x1405C3C45, (__int64)0x280F909090909090);     // nop
            WriteData((__int64*)0x1405C3C23, (__int64)0x0F90909090909090);     // nop
            WriteData((__int64*)0x1405C3C3A, (__int64)0x9090909090909090);     // nop
            WriteData((__int64*)0x140400B4F, (__int64)0x4890909090909090);     // nop
            WriteData((BYTE*)0x14067CB76, (BYTE)0x74);     // nop
            // Timer (Not the game update timer)
            WriteData((__int32*)0x14017EE5A, (__int32)0x90909090);     // nop
            WriteData((BYTE*)0x14017EE5E, (BYTE)0x90);     // nop

            FreeCamMode = true;
            if (TimerAddress)
            {
                *(BYTE*)(TimerAddress + 0x00) = 1;
                *(float*)(TimerAddress + 0x04) = 0.0f;
            }
        }
        else
        {
            WriteData((__int64*)0x1405C3C6A, (__int64)0x0000023083110FF2);     // movsd [rbx+00000230],xmm0
            WriteData((__int64*)0x140400B4F, (__int64)0x48000000B0A1290F);     //
            WriteData((__int64*)0x1405C3C23, (__int64)0x0F000001E083290F);     // movaps [rcx+000000B0],xmm4
            WriteData((__int64*)0x1405C3C2E, (__int64)0xF2000001F08B290F);     // movaps [rbx+000001F0],xmm1
            WriteData((__int64*)0x1405C3C3A, (__int64)0x0000020083110FF2);     // movsd [rbx+00000200],xmm0
            WriteData((__int64*)0x1405C3C45, (__int64)0x280F000002088B89);     // mov [rbx+00000208],ecx
            //WriteData((__int64*)0x1405C3C4F, (__int64)0x8B0000821083290F);     // movaps [rbx+00000210],xmm0    Roll
            WriteData((__int64*)0x1405C3C5C, (__int64)0x0FF2000002208389);     // mov [rbx+00000220],eax
            WriteData((__int64*)0x1405C3C78, (__int64)0x8B80000002388389);     // mov [rbx+00000238],eax
            WriteData((BYTE*)0x14067CB76, (BYTE)0x75);     // nop
            // Timer (Not the game update timer)
            WriteData((__int32*)0x14017EE5A, (__int32)0x49110FF3);
            WriteData((BYTE*)0x14017EE5E, (BYTE)0x2C);
            FreeCamMode = false;
            if (TimerAddress)
            {
                *(BYTE*)(TimerAddress + 0x00) = 0;
                *(float*)(TimerAddress + 0x04) = 1.0f;
            }
        }
    }


    __declspec(dllexport) void OnFrame()
    {
        if (!ControllerAddress)
        {
            size_t* pointer = (size_t*)0x142070060;
            if (!*pointer)
                return;
            pointer = (size_t*)((*pointer) + 0x770);
            if (!*pointer)
                return;
            pointer = (size_t*)((*pointer) + 0x038);
            if (!*pointer)
                return;
            pointer = (size_t*)((*pointer) + 0x018);
            if (!*pointer)
                return;
            pointer = (size_t*)((*pointer) + 0x728);
            if (!*pointer)
                return;
            pointer = (size_t*)((*pointer) + 0x3A0);
            //if (!*pointer)
            //    return;
            ControllerAddress = pointer;
            
            pointer = (size_t*)0x142070060;
            if (!*pointer)
                return;
            pointer = (size_t*)((*pointer) + 0x660);
            if (!*pointer)
                return;
            ControllerLSXAddress = (size_t*)((*pointer) + 0x208);
            ControllerLSYAddress = (size_t*)((*pointer) + 0x20C);
            ControllerRSXAddress = (size_t*)((*pointer) + 0x214);
            ControllerRSYAddress = (size_t*)((*pointer) + 0x218);
        }
        else
        {
            BYTE IsLBPressed = *((BYTE*)ControllerAddress + 9);
            BYTE IsRBPressed = *((BYTE*)ControllerAddress + 10);
            BYTE IsRTPressed = *((BYTE*)ControllerAddress + 2);
            BYTE IsLTPressed = *((BYTE*)ControllerAddress + 8);
            BYTE IsYPressed = *((BYTE*)ControllerAddress + 3);
            BYTE IsBPressed = *((BYTE*)ControllerAddress + 4);

            if (IsLBPressed && IsRBPressed && !StillPressing)
            {
                WriteCamLock();
                StillPressing = true;
            }
            if (!(IsLBPressed && IsRBPressed) && StillPressing)
                StillPressing = false;
            
            //0x0014F6F8
            if (*(*(BYTE**)Whatever + 0x78) != 6)
            {
                if (FreeCamMode)
                    WriteCamLock();
                return;
            }

            if (FreeCamMode)
            {
                if (IsBPressed && !StillPressing2)
                {
                    if (TimerAddress)
                    {
                        if (*(BYTE*)(TimerAddress + 0x00) == 0)
                            *(BYTE*)(TimerAddress + 0x00) = 1;
                        else
                            *(BYTE*)(TimerAddress + 0x00) = 0;
                    }
                    StillPressing2 = true;
                }

                if (!IsBPressed && StillPressing2)
                    StillPressing2 = false;

                size_t* pointer = (size_t*)0x142070060;
                pointer = (size_t*)((*pointer) + 0x630);
                pointer = (size_t*)((*pointer) + 0x148);
                pointer = (size_t*)((*pointer) + 0x108);
                pointer = (size_t*)((*pointer) + 0xAC);
                BYTE CharID = *(BYTE*)pointer;


                if ((*(size_t*)YVeloAddy) > 0x100000 && IsLTPressed && CharID == 2)
                {
                    *((float*)((*(size_t*)YVeloAddy) + 0xB8)) = *((float*)((*(size_t*)Cam1PosAddy) + 0x1E8));
                    *((float*)((*(size_t*)YVeloAddy) + 0xB4)) = *((float*)((*(size_t*)Cam1PosAddy) + 0x1E4));
                    *((float*)((*(size_t*)YVeloAddy) + 0xB0)) = *((float*)((*(size_t*)Cam1PosAddy) + 0x1E0));
                }
                if ((*(size_t*)YVeloAddy2) > 0x100000 && IsLTPressed)
                {
                    *((float*)((*(size_t*)YVeloAddy2) + 0xB8)) = *((float*)((*(size_t*)Cam1PosAddy) + 0x1E8));
                    *((float*)((*(size_t*)YVeloAddy2) + 0xB4)) = *((float*)((*(size_t*)Cam1PosAddy) + 0x1E4));
                    *((float*)((*(size_t*)YVeloAddy2) + 0xB0)) = *((float*)((*(size_t*)Cam1PosAddy) + 0x1E0));
                }

                if (*(float*)ControllerLSXAddress != 0.0f || *(float*)ControllerLSYAddress != 0.0f ||
                    *(float*)ControllerRSXAddress != 0.0f || *(float*)ControllerRSYAddress != 0.0f)
                {
                    if ((*(size_t*)Cam1PosAddy))
                    {

                        float speedMul = 5.0f;
                        float speedRotMul = 5.0f;
                        if (IsRTPressed)
                            speedMul = 20.0f;
                        if (IsRTPressed)
                            speedRotMul = 8.0f;

                        float speed = -*(float*)ControllerLSYAddress;
                        float yaw = *((float*)((*(size_t*)Cam1PosAddy) + 0x200));
                        float pitch = *((float*)((*(size_t*)Cam1PosAddy) + 0x204));
                        float roll = *((float*)((*(size_t*)Cam1PosAddy) + 0x208));
                        if (pitch > 1.56f)
                            pitch = 1.56f;
                        if (pitch < -1.56)
                            pitch = -1.56;
                        speed *= speedMul;
                        float xMove = speed * sinf(yaw) * cosf(pitch);
                        float yMove = speed * sinf(pitch);
                        float zMove = speed * cosf(yaw) * cosf(pitch);

                        xMove += (*(float*)ControllerLSXAddress) * (speedMul / 1.2f) * cos(yaw);
                        //yMove += (*(float*)ControllerLSXAddress) * (speedMul / 1.5f) * sinf(pitch);
                        zMove -= (*(float*)ControllerLSXAddress) * (speedMul / 1.2f) * sinf(yaw);


                        *((float*)((*(size_t*)Cam1PosAddy) + 0x1E0)) += xMove;
                        *((float*)((*(size_t*)Cam1PosAddy) + 0x1E4)) += yMove;
                        *((float*)((*(size_t*)Cam1PosAddy) + 0x1E8)) += zMove;
                        *((float*)((*(size_t*)Cam1PosAddy) + 0x200)) -= (*(float*)ControllerRSXAddress * 0.025f * (speedRotMul / 5.0f));
                        *((float*)((*(size_t*)Cam1PosAddy) + 0x204)) -= (*(float*)ControllerRSYAddress * 0.025f * (speedRotMul / 5.0f));
                        if (*((float*)((*(size_t*)Cam1PosAddy) + 0x204)) > 1.49f)
                            *((float*)((*(size_t*)Cam1PosAddy) + 0x204)) = 1.49f;
                        if (*((float*)((*(size_t*)Cam1PosAddy) + 0x204)) < -1.49)
                            *((float*)((*(size_t*)Cam1PosAddy) + 0x204)) = -1.49;
                        *((float*)((*(size_t*)Cam1PosAddy) + 0x208)) = 35.0f;
                        *((float*)((*(size_t*)Cam1PosAddy) + 0x234)) = 10000000.0f;
                        *((float*)((*(size_t*)Cam1PosAddy) + 0x220)) = 0.0f;
                        *((float*)((*(size_t*)Cam1PosAddy) + 0x1F0)) = 0.0f;
                        *((float*)((*(size_t*)Cam1PosAddy) + 0x1F4)) = 0.0f;
                        *((float*)((*(size_t*)Cam1PosAddy) + 0x1F8)) = 0.0f;
                        *((float*)((*(size_t*)Cam1PosAddy) + 0x238)) = 0.8f;
                    }
                }
            }
            else
            {
                if (!*(size_t*)Cam1PosAddy || !CameraRot)
                    return;

                size_t* pointer = (size_t*)0x142070060;
                pointer = (size_t*)((*pointer) + 0x630);
                pointer = (size_t*)((*pointer) + 0x148);
                pointer = (size_t*)((*pointer) + 0x108);
                pointer = (size_t*)((*pointer) + 0xAC);
                BYTE CharID = *(BYTE*)pointer;

                float x = 0.0f;
                float y = 0.0f;
                float z = 0.0f;


                // Player 2?
                if ((*(size_t*)YVeloAddy) > 0x100000 && CharID == 2)
                {
                    z = *((float*)((*(size_t*)YVeloAddy) + 0xB8));
                    y = *((float*)((*(size_t*)YVeloAddy) + 0xB4));
                    x = *((float*)((*(size_t*)YVeloAddy) + 0xB0));
                }
                // Player 1?
                if ((*(size_t*)YVeloAddy2) > 0x100000)
                {
                    z = *((float*)((*(size_t*)YVeloAddy2) + 0xB8));
                    y = *((float*)((*(size_t*)YVeloAddy2) + 0xB4));
                    x = *((float*)((*(size_t*)YVeloAddy2) + 0xB0));
                }

                // Speed
                float dx = fabsf(x - prevX);
                float dy = fabsf(y - prevY);
                float dz = fabsf(z - prevZ);
                float speed = fabsf(dx + dz);

                prevX = x;
                prevY = y;
                prevZ = z;


                float fullrot = (2.00f * M_PI);
                // Yaw
                CameraRotationOffsetYaw -= (*(float*)ControllerRSXAddress * 0.03f);
                // Pitch
                CameraRotationOffsetPitch -= (*(float*)ControllerRSYAddress * 0.03f);

                if (CameraRotationOffsetYaw > fullrot)
                    CameraRotationOffsetYaw -= fullrot;
                if (CameraRotationOffsetYaw < -fullrot)
                    CameraRotationOffsetYaw += fullrot;

                /*if ((*(CameraRot + 1) + CameraRotationOffsetPitch) > 1.00f)
                {
                    CameraRotationOffsetPitch += 1.00f - (*(CameraRot + 1) + CameraRotationOffsetPitch);
                    if (CameraRotationOffsetPitch < 0.0f)
                        CameraRotationOffsetPitch = 0.0f;
                }
                if ((*(CameraRot + 1) + CameraRotationOffsetPitch) < -0.20f)
                {
                    CameraRotationOffsetPitch -= 0.20f + (*(CameraRot + 1) + CameraRotationOffsetPitch);
                    if (CameraRotationOffsetPitch > 0.0f)
                        CameraRotationOffsetPitch = 0.0f;
                }*/

                *((float*)((*(size_t*)Cam1PosAddy) + 0x200)) = *(CameraRot + 0) + CameraRotationOffsetYaw;
                *((float*)((*(size_t*)Cam1PosAddy) + 0x204)) = *(CameraRot + 1) + CameraRotationOffsetPitch;
                if (*((float*)((*(size_t*)Cam1PosAddy) + 0x204)) > 1.49f)
                    *((float*)((*(size_t*)Cam1PosAddy) + 0x204)) = 1.49f;
                if (*((float*)((*(size_t*)Cam1PosAddy) + 0x204)) < -1.49)
                    *((float*)((*(size_t*)Cam1PosAddy) + 0x204)) = -1.49;
            }
        }
    }

    __declspec(dllexport) void Init(const char *path)
    {
        Cam1PosAddy = malloc(8);
        Whatever = malloc(8);
        memset(Cam1PosAddy, 0, 8);
        WriteJump((void*)0x1405C3C1C, (void*)0x1405C3E1C);
        WriteData((__int32*)0x1405C3E1C, (__int32)0x90D08B48);      // mov rdx,rax
        WriteData((__int16*)0x1405C3E1F, (__int16)0xB849);          // mov r8,ADDRESS
        WriteData((__int64*)0x1405C3E21, (__int64)Cam1PosAddy);     // mov r8,ADDRESS
        WriteData((__int32*)0x1405C3E29, (__int32)0x90188949);      // mov [r8],rbx
        WriteData((__int32*)0x1405C3E2C, (__int32)0x4040280F);      // movaps xmm0,[rax+40]
        WriteJump((void*)0x1405C3E30, (void*)0x1405C3C23);          // jmp 1405C3C23
        YVeloAddy = malloc(8);
        YVeloAddy2 = malloc(8);
        WriteJump((void*)0x1403F73E1, (void*)0x1405C3DC6);
        // cmp rax,20
        WriteData((__int32*)0x1405C3DC6, (__int32)0x20F88348);
        // jne 1405C3DDC
        WriteData((__int16*)0x1405C3DCA, (__int16)0x1075);
        // mov r12,ADDRESS
        WriteData((__int16*)0x1405C3DCC, (__int16)0xBC49);          // mov r12,
        WriteData((__int64*)0x1405C3DCE, (__int64)YVeloAddy);       // ADDRESS
        // mov [r12],rdx
        WriteData((__int32*)0x1405C3DD6, (__int32)0x24148949);
        // jmp 1405C3DF0
        WriteData((__int16*)0x1405C3DDA, (__int16)0x14EB);
        // cmp rax,40
        WriteData((__int32*)0x1405C3DDC, (__int32)0x40F88348);
        // jne 1405C3DF0
        WriteData((__int16*)0x1405C3DE0, (__int16)0x0E75);
        // mov r12,ADDRESS
        WriteData((__int16*)0x1405C3DE2, (__int16)0xBC49);          // mov r12,
        WriteData((__int64*)0x1405C3DE4, (__int64)YVeloAddy2);      // ADDRESS
        // mov [r12],rdx
        WriteData((__int32*)0x1405C3DEC, (__int32)0x24148949);
        // movaps xmm1,[rdx+000000E0] 
        WriteData((__int32*)0x1405C3DF0, (__int32)0x008A280F);      // movaps xmm1,[rdx+
        WriteData((__int32*)0x1405C3DF3, (__int32)0x000000E0);      // 000000E0]
        // jmp 1403F73E8
        WriteJump((void*)0x1405C3DF7, (void*)0x1403F73E8);


        WriteData((BYTE*)0x1406E6A75, GetWhateverHook, 23);
        WriteData((__int32*)0x1406E6A79, (__int32)Whatever);      // ADDRESS
        WriteJump((void*)0x1406E68C7, (void*)0x1406E6A75);

        //WriteJump((void*)0x1403F73E1, (void*)0x1405C3DC6);
        //WriteData((__int16*)0x1405C3DC6, (__int16)0xBC49);          // mov r12,ADDRESS
        //WriteData((__int64*)0x1405C3DC8, (__int64)YVeloAddy);       // mov r12,ADDRESS
        //WriteData((__int32*)0x1405C3DD0, (__int32)0x24148949);      // mov [r12],rdx
        //WriteData((__int32*)0x1405C3DD4, (__int32)0x008A280F);      // movaps xmm1,[rdx+000000E0]
        //WriteData((__int32*)0x1405C3DD7, (__int32)0x000000E0);      // movaps xmm1,[rdx+000000E0]
        //WriteJump((void*)0x1405C3DDB, (void*)0x1403F73E8);          // jmp 1403F73E8

        // GetRotation
        CameraRot = (float*)malloc(16);

        WriteData((__int64*)0x1405C3C3A, (__int64)0x9090909090909090);     // nop
        WriteJump((void*)0x1405C3C3A, (void*)0x1405C3DFC);
        WriteData((BYTE*)0x1405C3DFC, GetRotationHook, 21);
        WriteData((__int64*)0x1405C3DFE, (__int64)CameraRot);



    }
    __declspec(dllexport) ModInfo ForcesModInfo = { ModLoaderVer, GameVer };

}