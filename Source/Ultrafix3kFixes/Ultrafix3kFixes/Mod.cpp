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
auto sceneInfo = (SceneInfo**)0x142E70188;
GlobalVariables* globals = (GlobalVariables*)0x144000210;
const char** CurrentStateName = (const char**)0x142E8C1D0; // Not important
FunctionTable* RSDK;
ObjectPlayer* Player;
ObjectStarPost** StarPost = (ObjectStarPost**)0x143DB5C28;
ObjectDebugMode** DebugMode = (ObjectDebugMode**)0x143DB54C8;
ObjectShield**    Shield = (ObjectShield**)0x143DB5A70;
ObjectSlotHUD**   SlotHUD = (ObjectSlotHUD**)0x143db2058;
ObjectWater**     Water = (ObjectWater**)0x143db5258;
ObjectHUD**       HUD   = (ObjectHUD**)0x143db5688;
ObjectActClear**  ActClear = (ObjectActClear**)0x143db52b0;
ObjectFinalFireball** FinalFireBall = (ObjectFinalFireball**)0x143db6168;
ObjectS3K_BS_SlotSetup** S3K_BS_SlotSetup = (ObjectS3K_BS_SlotSetup**)0x143db2050;
ObjectBalloon** Balloon = (ObjectBalloon**)0x143db22c0;
ObjectCircleBumper** CircleBumper = (ObjectCircleBumper**)0x143db4ac8;
ObjectRing** Ring = (ObjectRing**)0x143db59b8;
ObjectRockPile** RockPile = (ObjectRockPile**)0x143db3fa0;
ObjectS3K_AIZBombing** S3K_AIZBombing = (ObjectS3K_AIZBombing**)0x143db89b8;
ObjectFlingRamp** FlingRamp = (ObjectFlingRamp**)0x143db4de0;
ObjectPuff** Puff = (ObjectPuff**)0x143db2cb8;
ObjectSignPost2** SignPost2 = (ObjectSignPost2**)0x1428bddd8;

// Custom struct, holds data when we need it. don't use unless ABSOLUTELY necessary.
static StockValues stockValues;

auto screenInfo = (ScreenInfo**)0x142E70190;
bool* usePathTracer = (bool*)(0x14285ab3c);

FUNCTION_PTR(void, __fastcall, Player_HandleAirFriction, 0x1401deb50, EntityPlayer* self);
FUNCTION_PTR(void, __fastcall, HUD_DrawNumbersBase10, 0x14010e170, EntityHUD *entity, Animator *animator, Vector2 *drawPos, int32 value, int32 digitCount);
FUNCTION_PTR(void, __fastcall, ScreenWrap_HandleHWrap, 0x1401b1230, void *state, bool32 noPlayer);
FUNCTION_PTR(void, __fastcall, StateMachineRun, 0x1400ad8f0, StateMachine *state, void *data);
FUNCTION_PTR(bool32, __fastcall, Player_CheckCollisionTouch, 0x1401dff50, EntityPlayer* player, void *e, Hitbox *entityHitbox);
FUNCTION_PTR(bool32, __fastcall, Player_CheckBadnikTouch, 0x1401dffe0, EntityPlayer* player, void *e, Hitbox *entityHitbox);
FUNCTION_PTR(void, __fastcall, Balloon_Create, 0x140110f30, void *data);
FUNCTION_PTR(void, __fastcall, Balloon_PlayerInteractionOG, 0x140111240, EntityBalloon *self);

HOOK(ObjectPlayer*, __fastcall, Player_StaticLoad, SigPlayer_StaticLoad(), ObjectPlayer* playerVars)
{
    Player = playerVars;
    return originalPlayer_StaticLoad(playerVars);
}

HOOK(void, __fastcall, Player_State_KnuxGlideLeft, SigPlayer_State_KnuxGlideLeft(), EntityPlayer* self)
{
    RSDK = *(FunctionTable**)0x142E70150;

    Hitbox* playerHitbox = RSDK->GetHitbox(&self->animator, 0);
    int32 offset = -TO_FIXED(1) * playerHitbox->left;

    if (self->position.x - offset <= (*Zone)->playerBoundsL[(*sceneInfo)->entitySlot]) {
        self->velocity.x = 0;
        self->abilitySpeed = 0;
    }

    originalPlayer_State_KnuxGlideLeft(self);

    if (self->position.x <= (*Zone)->playerBoundsL[(*sceneInfo)->entitySlot] + 0x100000 && !self->jumpHold)
    {
        self->direction = 0;
    }
}

HOOK(void, __fastcall, Player_State_GlideRight, SigPlayer_State_GlideRight(), EntityPlayer* self)
{
    RSDK = *(FunctionTable**)0x142E70150;

    Hitbox* playerHitbox = RSDK->GetHitbox(&self->animator, 0);
    int32 offset = playerHitbox->right << 16;

    if (self->position.x + offset >= (*Zone)->playerBoundsR[(*sceneInfo)->entitySlot]) {
        self->velocity.x = 0;
        self->abilitySpeed = 0;
    }

    originalPlayer_State_GlideRight(self);
}

HOOK(void, __fastcall, Player_State_ChargeHammerDash, SigPlayer_State_ChargeHammerDash(), EntityPlayer* player)
{
    auto Player_State_Air = (void(__fastcall*)(EntityPlayer* entity))SigPlayer_State_Air();
    if (player->onGround && player->chargeTimer >= 19 && player->jumpHold)
    {
        auto Camera_ShakeScreen = (void(__fastcall*)(int32 screen, int32 shakeX, int32 shakeY, int32 a4, int32 a5))(SigCamera_ShakeScreen());
        if (player->superState == SUPERSTATE_SUPER && player->camera)
            Camera_ShakeScreen(player->camera->screenID, 0, 6, 1, 1);
    }
    originalPlayer_State_ChargeHammerDash(player);
    if (player->animator.animationID == ANI_BREATHE) {
        player->jumpAbilityState = 0;
        player->chargeTimer = 0;
        player->state.state = (void(__fastcall*)())Player_State_Air;
    }
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

    // Hiya, Rosy here!
    // There's no Cool Bonus here, so let's just fix that up.
    self->coolBonus = 0;

    // And then we fix the score itself!
    if (!(*ActClear)->disableTimeBonus) {
        self->timeBonus = 100;
        switch ((*sceneInfo)->minutes)
        {
        case 0:
            self->timeBonus = 50000;
            break;
        case 1:
            self->timeBonus = (*sceneInfo)->seconds < 30 ? 10000 : 5000;
            break;
        case 2:
            self->timeBonus = (*sceneInfo)->seconds < 30 ? 4000 : 3000;
            break;
        case 3:
            self->timeBonus = (*sceneInfo)->seconds < 30 ? 1000 : 100;
            break;
        case 9:
            if (!(*sceneInfo)->debugMode &&
                globals->gameMode < MODE_TIMEATTACK &&
                (*sceneInfo)->seconds == 59)
            {
                if (!(globals->medalMods & MEDAL_NOTIMEOVER))
                    self->timeBonus = 100000;
            }
            break;
        default:
            break;
        }
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


HOOK(void, __fastcall, Player_State_HammerDash, SigPlayer_State_HammerDash(), EntityPlayer* self)
{
    originalPlayer_State_HammerDash(self);
    if (self->onGround)
    {
        ((void(__fastcall*)(EntityPlayer*))(SigPlayer_HandleGroundRotation()))(self);
    }
}

HOOK(void, __fastcall, Player_State_Hurt, SigPlayer_State_Hurt(), EntityPlayer *self)
{
	//auto globals = *(GlobalVariables**)0x144000210;
    //printf("globals secrets in struct mem is %llx\n", &globals->secrets);
    //printf("Zone ringFrame in struct mem is %llx\n", &(*Zone)->ringFrame);
    //printf("Zone cameraBoundsL in struct mem is %llx\n", &(*Zone)->cameraBoundsL);
    //printf("Player scale in struct mem is %llx\n", &self->scale);
    //printf("Player drawfx in struct mem is %llx\n", &self->drawFX);
    //printf("Player showHitbox in struct mem is %llx\n", &Player->showHitbox);
    //printf("Player rayDiveTimer in struct mem is %llx\n", &Player->rayDiveTimer);
    //printf("Player superPalette_Amy in struct mem is %llx\n", &Player->superPalette_Amy);
    //printf("Player amyFrames in struct mem is %llx\n", &Player->amyFrames);
    //printf("Player mightyFrames in struct mem is %llx\n", &Player->mightyFrames);
    //printf("Player sfxTransform2 in struct mem is %llx\n", &Player->sfxTransform2);
    //printf("Player sfxJump in struct mem is %llx\n", &Player->sfxJump);
    //printf("Player sfxEarthquake in struct mem is %llx\n", &Player->sfxEarthquake);

    if (self->onGround) {
        self->timer = 0;
    }
    originalPlayer_State_Hurt(self);
}

HOOK(void, __fastcall, Player_StageLoad, SigPlayer_StageLoad(), EntityPlayer* self)
{
    auto globals = *(GlobalVariables**)0x144000210;
    // Disable barrel hotline
    globals->secrets &= ~SECRET_BARRELHOTLINE;
    // Enable barrel hotline & enforce the S3k camera in Classic Mode
    if (!globals->playMode) {
        globals->secrets |= SECRET_BARRELHOTLINE;
        globals->secrets |= SECRET_CAMERATRACKING;
    }

    if (globals->playMode == BOOT_PLAYMODE_BOSSRUSH && globals->missionCondition != 0) { // if you are in Boss Rush stage and the condition is somehow NOT CLEARED (???)
        globals->missionCondition = 0; // clear that shit so no bugs can happen!!
    }

    originalPlayer_StageLoad(self);
}

HOOK(void, __fastcall, LinkGameLogicDLL, SigLinkGameLogicDLL(), void* info)
{
    originalLinkGameLogicDLL(info);

    RSDK = *(FunctionTable**)0x142E70150;
}

HOOK(void, __fastcall, NotifyCallback, SigNotifyCallback(), NotifyCallbackInfo* callbackInfo)
{
    auto &activePlayerCount = *((int8*)0x142FE7500);

    switch (callbackInfo->callback) {
    case NOTIFY_DEBUGPRINT:
    {
        printf("[RSDKv5U] DEBUG CALLBACK! VALUES ARE: %d , %d , %d .\n", callbackInfo->param1, callbackInfo->param2, callbackInfo->param3);
    }
    break;
    case CALLBACK_COPYPALETTE:
    {
        auto legacyPalette = (uint16*)0x14292EF70;
        memcpy(&legacyPalette[callbackInfo->param2], &legacyPalette[callbackInfo->param1], callbackInfo->param3 * sizeof(uint16));
    }
    break;
    case CALLBACK_SET1P:
    {
        activePlayerCount = 1;
    }
    break;
    case CALLBACK_SET2P:
    {
        activePlayerCount = 2;
    }
    break;
    case 2003:
    {
        // This sets the player count to 0, for testing.
        activePlayerCount = 0;
    }
    break;
    default:
        originalNotifyCallback(callbackInfo);
        break;
    }
}

HOOK(void, __fastcall, Starpost_BonusStars, SigStarPost_BonusStars(), void *self)
{
    globals = *(GlobalVariables**)0x144000210;
    unsigned short warpSfx = (*StarPost)->sfxWarp;
    if (!globals->playMode)
        (*StarPost)->sfxWarp = 0xFFFF;
    originalStarpost_BonusStars(self);
    (*StarPost)->sfxWarp = warpSfx;
}

FUNCTION_PTR(void, __fastcall, Music_FadeToStop, SigMusic_FadeToStop(), float speed);

HOOK(void, __fastcall, Starpost_EnterGachaponBonus, SigStarPost_EnterGachaponBonus(), void *self)
{
    originalStarpost_EnterGachaponBonus(self);
    Music_FadeToStop(0.025);
}

HOOK(void, __fastcall, Starpost_EnterGlowingSpheresBonus, SigStarPost_EnterGlowingSpheresBonus(), void *self)
{
    originalStarpost_EnterGlowingSpheresBonus(self);
    Music_FadeToStop(0.025);
}

HOOK(void, __fastcall, Starpost_EnterSlotBonus, SigStarPost_EnterSlotBonus(), void *self)
{
    originalStarpost_EnterSlotBonus(self);
    Music_FadeToStop(0.025);
}

HOOK(void, __fastcall, Player_State_Air, SigPlayer_State_Air(), EntityPlayer* self)
{
    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;
    auto Player_Gravity_False = (void(__fastcall*)(EntityPlayer* self))SigPlayer_Gravity_False();
    auto Player_Gravity_True = (void(__fastcall*)(EntityPlayer* self))0x1401e31e0;
    auto Player_State_Ground = (void(__fastcall*)(EntityPlayer* self))SigPlayer_State_Ground();
    auto Player_State_Spindash = (void(__fastcall*)(EntityPlayer* self))0x1401ebb40;
    //originalPlayer_State_Air(self);
    self->tileCollisions = globals->tileCollisionMode;
    Player_HandleAirFriction(self);

    if (self->onGround) {
        self->nextGroundState.state = (void(__fastcall*)())Player_State_Ground;
        self->stateGravity.state = (void(__fastcall*)())Player_Gravity_False;
    }
    else {
        self->stateGravity.state = (void(__fastcall*)())Player_Gravity_True;

        if (self->animator.animationID != ANI_SPINDASH)
            self->nextGroundState.state = (void(__fastcall*)())Player_State_Ground;

        if ((self->velocity.y > 0 && globals->gravityDir == CMODE_FLOOR) || (self->velocity.y < 0 && globals->gravityDir == CMODE_ROOF)) {
            if (self->animator.animationID >= ANI_SPRING_TWIRL) {
                if (self->animator.animationID <= ANI_SPRING_DIAGONAL) {
                    RSDK->SetSpriteAnimation(self->aniFrames, self->animationReserve, &self->animator, false, 0);
                }
                else if ((self->animator.animationID == ANI_SPRING_CS_REV || self->animator.animationID == ANI_SPRING_CS) && !self->animator.frameID
                    && self->animationReserve != ANI_SPRING_CS) {
                    RSDK->SetSpriteAnimation(self->aniFrames, ANI_WALK, &self->animator, false, 0);
                }
            }
        }

        int32 prevSpeed;
        switch (self->animator.animationID) {
            case ANI_IDLE:
            case ANI_WALK:
                RSDK->SetSpriteAnimation(self->aniFrames, ANI_WALK, &self->animator, false, self->animator.frameID);
                break;

            case ANI_LOOK_UP:
            case ANI_CROUCH:
            case ANI_SKID_TURN: RSDK->SetSpriteAnimation(self->aniFrames, ANI_WALK, &self->animator, false, self->animator.frameID); break;

            case ANI_JOG:
                prevSpeed = self->animator.speed;
                RSDK->SetSpriteAnimation(self->aniFrames, ANI_WALK, &self->animator, false, 0);
                self->animator.speed = prevSpeed;
                break;

            case ANI_JUMP:
                if (globals->gravityDir == CMODE_ROOF) {
                    if (self->velocity.y <= -self->jumpCap)
                        StateMachineRun(&self->stateAbility, self);
                }
                else if (globals->gravityDir == CMODE_FLOOR) {
                    if (self->velocity.y >= self->jumpCap)
                        StateMachineRun(&self->stateAbility, self);
                }
                break;

            case ANI_SKID:
                if (self->skidding <= 0)
                    RSDK->SetSpriteAnimation(self->aniFrames, ANI_WALK, &self->animator, false, self->animator.frameID);
                else
                    self->skidding--;
                break;

            case ANI_SPINDASH:
                if (self->nextGroundState.state != (void(__fastcall*)())Player_State_Ground)
                    self->nextGroundState.state = (void(__fastcall*)())Player_State_Spindash;
                break;

            default: break;
        }
    }
}

void Player_State_RollJump(EntityPlayer* self)
{
    *CurrentStateName = "State_RollJump";
    auto Player_State_Air = (void(__fastcall*)(EntityPlayer* self))SigPlayer_State_Air();

    self->left = false;
    self->right = false;

    // Run Air state
    Player_State_Air(self);

    if (self->animator.animationID == ANI_BREATHE) {
        self->state.state = (void(__fastcall*)())Player_State_Air;
        self->nextAirState.state = (void(__fastcall*)())Player_State_Air;
    }
}

HOOK(void, __fastcall, Player_Update, 0x1401d7f00, void)
{
    globals = *(GlobalVariables**)0x144000210;
    RSDK = *(FunctionTable**)0x142E70150;
    auto Player_State_Death = (void(__fastcall*)(EntityPlayer* self))0x1401e7ac0;
    auto Player_State_Drown = (void(__fastcall*)(EntityPlayer* self))0x1401e7f80;

    originalPlayer_Update();

    RSDK_THIS(Player);

    int32 YPosStorage = 0;

    if (globals->playMode == BOOT_PLAYMODE_BOSSRUSH && globals->missionCondition == MISSION_CONDITION_CLEAR) {                                  // If the boss is defeated in Boss Rush
        self->invincibleTimer = 80;                                                                                                             // Make the player invincible.
        self->drownTimer = 0;                                                                                                                   // Don't let them drown.
        if (!RSDK->CheckSceneFolder("BR3_3K_MGZ")) {                                                                                            // Is the player not in the Marble Garden Boss Rush level?
            if (self->state.state != (void(__fastcall*)())Player_State_Death && self->state.state != (void(__fastcall*)())Player_State_Drown) { // Is the player not drowning or dying? (there's still instances where this happens ig)
                YPosStorage = (*Zone)->deathBoundary[0];                                                                                        // if so, Store the Death Boundary of Player 1 (You're never Player 2).
                YPosStorage -= 0x100000;                                                                                                        // Have the storage a slight bit higher than the actual boundary.

                if (self->position.y >= YPosStorage) {                                                                                          // Is Player about to pass the stored coordinate?
                    self->position.y = YPosStorage;                                                                                             // If so, keep them at that coordinate to prevent them from falling out.
                    if (self->velocity.y > 0) {                                                                                                 // Does the player have velocity going down?
                        self->velocity.y = 0;                                                                                                   // If so, stop that.
                    }
                }
            }
        }
    }
}

HOOK(void, __fastcall, Player_State_Spindash, 0x1401ebb40, EntityPlayer* self)
{
    globals = *(GlobalVariables**)0x144000210;
    RSDK = *(FunctionTable**)0x142E70150;
    auto Player_State_Air = (void(__fastcall*)(EntityPlayer* self))SigPlayer_State_Air();
    auto Player_State_Spindash = (void(__fastcall*)(EntityPlayer* self))0x1401ebb40;

    originalPlayer_State_Spindash(self);

    if (self->characterID == ID_KNUCKLES && self->angle == 256 && !self->onGround && !self->groundedStore) { // what the fuck knuckles?
        self->state.state = (void(__fastcall*)())Player_State_Air;                                           // get out of the sky
        self->nextGroundState.state = (void(__fastcall*)())Player_State_Spindash;                            // but spindash later cuz you're supposed to (in most instances).
    }
}

HOOK(void, __fastcall, Player_Action_Jump, SigPlayer_Action_Jump(), EntityPlayer* entity)
{
    globals = *(GlobalVariables**)0x144000210;
    auto Player_State_Air = (void(__fastcall*)(EntityPlayer* entity))SigPlayer_State_Air();
    auto Player_State_Roll = (void(__fastcall*)(EntityPlayer* entity))SigPlayer_State_Roll();

    originalPlayer_Action_Jump(entity);

    if (!globals->playMode && entity->state.state == (void(__fastcall*)())Player_State_Roll) {
       entity->state.state = (void(__fastcall*)())Player_State_RollJump;
       entity->nextAirState.state = (void(__fastcall*)())Player_State_RollJump;
    }
    else {
        entity->state.state = (void(__fastcall*)())Player_State_Air;
    }

    entity->disableGravity = true;

}

HOOK(void, __fastcall, Player_State_Carried, SigPlayer_State_Carried(), EntityPlayer* self)
{
    auto Player_State_Carried = (void(__fastcall*)(EntityPlayer* entity))SigPlayer_State_Carried();

    if (self->state.state == (void(__fastcall*)())Player_State_Carried) {

        if (self->jumpPress && self->down) {
            self->jumpAbilityState = 1; // you can always use an ability when you jump out of a tails assist.
        }

    }

    originalPlayer_State_Carried(self);
}

HOOK(void, __fastcall, Player_Action_CarryPlayer, SigPlayer_Action_CarryPlayer(), EntityPlayer *self, EntityPlayer *leader)
{
    globals = *(GlobalVariables**)0x144000210;
    RSDK = *(FunctionTable**)0x142E70150;

    auto Player_State_Carried = (void(__fastcall*)(EntityPlayer* entity))SigPlayer_State_Carried();
    auto Player_State_ChargeHammerDash = (void(__fastcall*)(EntityPlayer* self))SigPlayer_State_ChargeHammerDash();

    int32 offset = self->position.y;

    switch (globals->gravityDir) {
        default: break;

        case CMODE_FLOOR:
            offset += 0x1C0000;
            if (self->animator.animationID == ANI_JUMP)
                offset += self->jumpOffset;
            break;

        case CMODE_ROOF:
            offset -= 0x1C0000;
            if (self->animator.animationID == ANI_JUMP)
                offset -= self->jumpOffset;
            break;
    }

    if ((leader->state.state != (void(__fastcall*)())Player_State_Carried) && (!leader->onGround || self->velocity.y < 0)) {

        if ((leader->state.state == (void(__fastcall*)())Player_State_RollJump || leader->state.state == (void(__fastcall*)())Player_State_ChargeHammerDash && leader->chargeTimer <= 19 ) && (leader->animator.animationID != ANI_FAN)) {
            if (abs(self->position.x - leader->position.x) < TO_FIXED(12) && abs(offset - leader->position.y) < TO_FIXED(12) && !self->flyCarryTimer
                && !leader->down && !leader->onGround) {
                RSDK->SetSpriteAnimation(leader->aniFrames, ANI_HANGPLAYER, &leader->animator, false, 0);
                leader->state.state           = (void(__fastcall*)())Player_State_Carried;
                leader->nextAirState.state    = nullptr;
                leader->nextGroundState.state = nullptr;
                RSDK->PlaySfx(Player->sfxGrab, false, 255);
            }
        }
    }

    originalPlayer_Action_CarryPlayer(self, leader);
}

HOOK(void, __fastcall, Player_DoHyperDash, SigPlayer_DoHyperDash(), EntityPlayer* self)
{
    StateMachineRun(&self->stateInput, self);

    originalPlayer_DoHyperDash(self);
}

HOOK(void, __fastcall, Player_Action_DblJumpSonic, SigPlayer_Action_DblJumpSonic(), EntityPlayer* self)
{
    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;
    auto Player_State_Air = (void(__fastcall*)(EntityPlayer* self))SigPlayer_State_Air();
    auto Player_Input_AI_Follow = (void(__fastcall*)(EntityPlayer* self))SigPlayer_Input_AI_Follow();
    int32 playerSlot = RSDK->GetEntitySlot(self);
    auto shield = (EntityShield*)RSDK->GetEntity(playerSlot + Player->maxPlayerCount);
    bool32 upStore = false;

    upStore = self->up;

    EntityPlayer *sidekick = (EntityPlayer *)RSDK->GetEntity(SLOT_PLAYER2);

    if (sidekick->classID == Player->classID && sidekick->sidekick) { // if Player 2 exists and is a sidekick (yes i know this looks stupid but that's the label do not question it).
        if (self->invincibleTimer > 0) { // allow shield abilities always.
            self->up = false;
        }
        else {
            switch (self->shield) { // do some fancy stuff for shield related abiltiies if P2 exists or not while not invincible.
            case SHIELD_NONE:
            case SHIELD_BLUE:
            case SHIELD_LIGHTNING:
                self->up = false;
                break;
            }
        }
    }
    else { // if P2 not exist, allow shield abilities while holding up.
        self->up = false;
    }

    if (self->shield != SHIELD_NONE && !self->sidekick && self->up && self->superState != SUPERSTATE_SUPER) {
        self->up = upStore;
        return;
    }

    if (self->jumpAbilityState == 1) {
        if (self->stateInput.state != (void(__fastcall*)())Player_Input_AI_Follow || (self->up)) {
            if (self->jumpPress) {
                if (self->shield > SHIELD_BLUE) {
                    if (self->state.state == (void(__fastcall*)())Player_State_RollJump){
                        self->state.state = (void(__fastcall*)())Player_State_Air;
                    }
                }
            }
        }
    }

    originalPlayer_Action_DblJumpSonic(self);

    self->up = upStore;

    if (self->jumpPress) {
        switch (self->shield) {
            case SHIELD_NONE:
                if (globals->medalMods & MEDAL_INSTASHIELD) {
                    if (shield->state.state == SigShield_State_Insta()) {
                        if (self->state.state == (void(__fastcall*)())Player_State_RollJump){
                            self->state.state = (void(__fastcall*)())Player_State_Air;
                        }
                    }
                    if (self->invincibleTimer >= 1) {
                        if (self->state.state == (void(__fastcall*)())Player_State_RollJump){
                            self->state.state = (void(__fastcall*)())Player_State_Air;
                        }
                    }
                }
            break;
        }
    }
    if (self->animator.animationID == ANI_BREATHE) {
        self->jumpAbilityState = 0;
        self->state.state = (void(__fastcall*)())Player_State_Air;
    }
}

HOOK(void, __fastcall, Player_Action_DblJumpAmy, 0x1401db9d0, EntityPlayer *self)
{
    bool32 upInputStore = false;

    upInputStore = self->up;

    self->up = false;

    originalPlayer_Action_DblJumpAmy(self);

    self->up = upInputStore;
}

HOOK(void, __fastcall, Player_State_Dropdash, SigPlayer_State_DropDash(), EntityPlayer* self)
{
    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;
    auto Player_State_Air = (void(__fastcall*)(EntityPlayer* entity))SigPlayer_State_Air();
    originalPlayer_State_Dropdash(self);
    if (self->animator.animationID == ANI_BREATHE) {
        self->jumpAbilityState = 0;
        self->state.state = (void(__fastcall*)())Player_State_Air;
    }
}

HOOK(void, __fastcall, Shield_Draw, 0x1401f30d0, EntityShield* self)
{
    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;

    auto Player_State_Crouch = (void(__fastcall*)(EntityPlayer* player))0x1401e74e0;
    auto Player_State_Spindash = (void(__fastcall*)(EntityPlayer* player))0x1401ebb40;
    auto Shield_Draw = (void(__fastcall*)(EntityShield* self))0x1401f30d0;
    if (self->type != SHIELD_UNK) {
        EntityPlayer *player = self->player;
        if (!player || player->classID != Player->classID || (player->superState == SUPERSTATE_NONE || player->superState == SUPERSTATE_FADEOUT)) {
            int32 dirStore = self->direction;
            if (globals->gravityDir == CMODE_ROOF)
                self->direction = dirStore ^ 3;

            if (player) {
                if (player->isChibi) {
                    self->drawFX |= FX_SCALE;
                    self->scale.x = 0x100;
                    self->scale.y = 0x100;
                }
                else {
                    if (player->drawFX & FX_SCALE)
                        self->drawFX |= FX_SCALE;
                    else
                        self->drawFX &= ~FX_SCALE;
                    self->scale.x = player->scale.x;
                    self->scale.y = player->scale.y;
                }

                self->position = player->position;

                Hitbox *playerHitbox = RSDK->GetHitbox(&player->animator, 0);
                if (playerHitbox) {
                    if (player->direction & FLIP_X)
                        self->position.x += (playerHitbox->left << 15) - (playerHitbox->right << 15) - (playerHitbox->left << 16);
                    else
                        self->position.x += ((playerHitbox->right + 2 * playerHitbox->left) << 15) - (playerHitbox->left << 15);
                    if (player->state.state != (void(__fastcall*)())Player_State_Crouch && player->state.state != (void(__fastcall*)())Player_State_Spindash && player->animator.animationID != ANI_SPINDASH) {
                        if ((player->direction & FLIP_Y) || player->invertGravity) {
                            self->position.y += (playerHitbox->top << 15) - (playerHitbox->bottom << 15) - (playerHitbox->top << 16);
                        }
                        else {
                            self->position.y += ((playerHitbox->bottom + 2 * playerHitbox->top) << 15) - (playerHitbox->top << 15);
                        }
                    }
                }
            }

            if (globals->useManiaBehavior && self->type == SHIELD_BUBBLE) {
                self->inkEffect = INK_ADD;
                RSDK->DrawSprite(&self->fxAnimator, NULL, false);
                self->inkEffect = INK_BLEND;
            }

            RSDK->DrawSprite(&self->shieldAnimator, NULL, false);
            self->direction = dirStore;
            ScreenWrap_HandleHWrap((void(__fastcall*)())Shield_Draw, false);
        }
    }
}

HOOK(void, __fastcall, Shield_Create, SigShield_Create(), EntityShield* self, void *data)
{
    self->active  = ACTIVE_NORMAL;
    self->visible = true;

    if (data) {
        EntityPlayer *player = (EntityPlayer *)data;
        if (player->classID == (*DebugMode)->classID)
            self->type = player->shield;
        self->player = player;
    }

    originalShield_Create(self, data);

}

HOOK(void, __fastcall, DebugMode_Update, 0x1401c6e00, EntityDebugMode* self)
{
    // originalDebugMode_Update(self);
    // if (self->classID == Player->classID) {
    //     RSDK_THIS(Player);
    //     //auto entityPlayer = (EntityPlayer*)self;
    //     RSDK->SetSpriteAnimation(self->aniFrames, ANI_WALK, &self->animator, false, 0);
    // }

}

HOOK(void, __fastcall, ScoreBonus_Draw, 0x14010f600, void)
{
    globals = *(GlobalVariables**)0x144000210;
    if (globals->playMode == BOOT_PLAYMODE_MISSION)
        return;
    originalScoreBonus_Draw();

}

int32 HUD_CharacterIndexFromID(int32 characterID)
{
    int32 id = -1;
    for (int32 i = characterID; i > 0; ++id, i >>= 1)
        ;
    return id;
}

void HUD_DrawNumbersBase16(EntityHUD* entity, Vector2 *drawPos, int32 value)
{
    RSDK = *(FunctionTable**)0x142E70150;
    int32 mult = 1;
    for (int32 i = 4; i; --i) {
        entity->numbersAnimator.frameID = value / mult & 0xF;
        RSDK->DrawSprite(&entity->numbersAnimator, drawPos, true);
        drawPos->x -= TO_FIXED(8);
        mult *= 16;
    }
}

HOOK(void, __fastcall, HUD_Draw, 0x1401ce730, EntityHUD* self)
{
    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;

    //originalHUD_Draw(self);

    if (!globals->showHUD)
        return;

    if (globals->playMode == BOOT_PLAYMODE_MISSION && globals->missionCondition == MISSION_CONDITION_FAIL){
        globals->hudEnable = HUDENABLE_OFF;
    }

    if ((globals->playMode == BOOT_PLAYMODE_MISSION && globals->missionCondition != MISSION_CONDITION_FAIL) || globals->playMode == BOOT_PLAYMODE_BOSSRUSH) {
        globals->hudEnable = HUDENABLE_ON;
    }

    EntityPlayer* player =  RSDK_GET_ENTITY((*sceneInfo)->currentScreenID, Player);

    Vector2 drawPos;
    Vector2 scorePos = self->scorePos;
    Vector2 timePos  = self->timePos;
    Vector2 ringPos  = self->ringsPos;
    Vector2 lifePos  = self->lifePos;

    self->ringFlashFrame = player->rings ? 0 : ((globals->persistentTimer >> 3) & 1);

    self->timeFlashFrame = 0;

    if (((*sceneInfo)->minutes >= 9 && isMainGameMode() && !(globals->medalMods & MEDAL_NOTIMEOVER)) || (*ActClear)->disableTimeBonus)
        self->timeFlashFrame = (globals->persistentTimer >> 3) & 1;

    // Draw "Score"
    self->hudElementsAnimator.frameID = 0;
    RSDK->DrawSprite(&self->hudElementsAnimator, &scorePos, true);

    // Draw Score
    drawPos.x = scorePos.x + 0x5B0000;
    drawPos.y = scorePos.y + 0xB0000;
    HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, player->score, 0);

    // Draw "Time" Text
    self->hudElementsAnimator.frameID = self->timeFlashFrame + 1;
    RSDK->DrawSprite(&self->hudElementsAnimator, &timePos, true);

    if (!self->enableTimeFlash || globals->persistentTimer & 8) {
        // Draw : or "
        drawPos.x                         = timePos.x - 0x080000;
        drawPos.y                         = timePos.y;
        self->hudElementsAnimator.frameID = self->extendedHUD ? 12 : 20;
        RSDK->DrawSprite(&self->hudElementsAnimator, &drawPos, true);

        self->numbersAnimator.frameID = 0;
        drawPos.y                     = timePos.y + TO_FIXED(self->numbersAnimator.frames[0].frame.height);
        if (self->extendedHUD) {
            // Draw Milliseconds
            drawPos.x = timePos.x + 0x5B0000;
            HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, (*sceneInfo)->milliseconds, 2);
            drawPos.x -= 0x80000;
        }
        else {
            drawPos.x = timePos.x + 0x430000;
        }

        if ((*sceneInfo)->minutes > 9) {
            // Draw Seconds
            HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, 59, 2);
            drawPos.x -= 0x79000;

            // Draw Minutes
            HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, 9, 1);
        }
        else {
            // Draw Seconds
            HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, (*sceneInfo)->seconds, 2);
            drawPos.x -= 0x79000;

            // Draw Minutes
            if ((*sceneInfo)->minutes > 9 && globals->medalMods & MEDAL_NOTIMEOVER)
                HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, (*sceneInfo)->minutes, 2);
            else
                HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, (*sceneInfo)->minutes, 1);
        }
    }

    // Draw "Rings" Text
    self->hudElementsAnimator.frameID = self->ringFlashFrame + 3;
    RSDK->DrawSprite(&self->hudElementsAnimator, &ringPos, true);

    // Draw Rings
    if (!self->enableRingFlash || globals->persistentTimer & 8) {
        self->numbersAnimator.frameID = 0;
        drawPos.x                     = self->ringsPos.x + 0x430000;
        drawPos.y                     = self->ringsPos.y + TO_FIXED(self->numbersAnimator.frames[self->numbersAnimator.frameID].frame.height);

        if (player->hyperRing) {
            HUD_DrawNumbersBase10(self, &self->hyperNumbersAnimator, &drawPos, player->rings, 0);
            drawPos.x -= 0x40000;
            self->hyperNumbersAnimator.frameID = 10;
            RSDK->DrawSprite(&self->hyperNumbersAnimator, &drawPos, true);
        }
        else
            HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, player->rings, 0);
    }

    if ((*sceneInfo)->debugMode) {
        if (player->camera) {
            if (globals->playMode != BOOT_PLAYMODE_MISSION) {
                // Draw Camera YPos
                drawPos.x = TO_FIXED((*screenInfo)[player->camera->screenID].size.x - 16);
                drawPos.y = 0x180000;
                HUD_DrawNumbersBase16(self, &drawPos, (*screenInfo)[player->camera->screenID].position.y);

                // Draw Camera XPos
                drawPos.x -= 0x90000;
                HUD_DrawNumbersBase16(self, &drawPos, (*screenInfo)[player->camera->screenID].position.x);

                // Draw Player YPos
                drawPos.x = TO_FIXED((*screenInfo)[player->camera->screenID].size.x - 16);
                drawPos.y += 0x100000;
                HUD_DrawNumbersBase16(self, &drawPos, FROM_FIXED(player->position.y));

                // Draw Player XPos
                drawPos.x -= 0x90000;
                HUD_DrawNumbersBase16(self, &drawPos, FROM_FIXED(player->position.x));
            }
        }
    }

    int32 lives = self->lives[player->playerID];

    if (globals->somethingRelatedToLives) {
        drawPos = self->lifePos;

        if (globals->medalMods & MEDAL_NOLIVES) {
            self->lifeIconAnimator.frameID  = self->lifeIconAnimator.frameCount - 1;
            self->lifeNamesAnimator.frameID = self->lifeIconAnimator.frameID;
            lives                           = globals->coinCount;
        }
        else {
            lives                           = self->lives[player->playerID];
            self->lifeIconAnimator.frameID  = HUD_CharacterIndexFromID(player->characterID);
            self->lifeNamesAnimator.frameID = HUD_CharacterIndexFromID(player->characterID);

            // "Miles" name
            if ((globals->playerID & 0xFF) == ID_TAILS && globals->secrets & SECRET_REGIONSWAP)
                self->lifeNamesAnimator.frameID = self->lifeNamesAnimator.frameCount - 1;
        }

        if (!*usePathTracer)
            self->lifeIconAnimator.frameID = self->lifeIconAnimator.frameCount - 2;

        if (self->lifeIconAnimator.frameID < 0) {
            self->lifeIconAnimator.frameID = self->lifeFrameIDs[player->playerID];
            lives--;
        }
        else {
            self->lifeFrameIDs[player->playerID] = self->lifeIconAnimator.frameID;
            self->lives[player->playerID]        = player->lives;
        }

        RSDK->DrawSprite(&self->lifeIconAnimator, &drawPos, true);

        switch (globals->gameSpriteStyle) {
            case GAME_S1:
            case GAME_CD:
            case GAME_S2:
            case GAME_SM:
                self->hudElementsAnimator.frameID = 14;
                RSDK->DrawSprite(&self->hudElementsAnimator, &drawPos, true);
                drawPos.x += 0x300000;
                if (player->lives < 10)
                    drawPos.x -= 0x80000;
                HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, lives, 0);
                break;

            case GAME_S3K:
            case GAME_S3:
            case GAME_SK:
                RSDK->DrawSprite(&self->lifeNamesAnimator, &drawPos, true);
                self->hudElementsAnimator.frameID = 14;

                if (globals->useCoins) {
                    drawPos.x -= 0x20000;
                    RSDK->DrawSprite(&self->hudElementsAnimator, &drawPos, true);
                    drawPos.x += 0x2C0000;
                    HUD_DrawNumbersBase10(self, &self->lifeNumbersAnimator, &drawPos, lives, 0);
                }
                else {
                    RSDK->DrawSprite(&self->hudElementsAnimator, &drawPos, true);
                    drawPos.x += 0x290000;
                    HUD_DrawNumbersBase10(self, &self->lifeNumbersAnimator, &drawPos, lives, 0);
                }
                break;
        }

    }
}

// HOOK(void, __fastcall, HUD_Draw, 0x1401ce730, EntityHUD* self) // This is the original code, recreated without the super button and Mania specific code.
// {
//     RSDK = *(FunctionTable**)0x142E70150;
//     globals = *(GlobalVariables**)0x144000210;

//     //originalHUD_Draw(self);

//     if (!globals->showHUD)
//         return;

//     if (globals->playMode == BOOT_PLAYMODE_MISSION || globals->playMode == BOOT_PLAYMODE_BOSSRUSH) {
//        globals->hudEnable = HUDENABLE_ON;
//        return;
//     }

//     EntityPlayer* player =  RSDK_GET_ENTITY((*sceneInfo)->currentScreenID, Player);

//     Vector2 drawPos;
//     Vector2 scorePos = self->scorePos;
//     Vector2 timePos  = self->timePos;
//     Vector2 ringPos  = self->ringsPos;
//     Vector2 lifePos  = self->lifePos;

//     self->ringFlashFrame = player->rings ? 0 : ((globals->persistentTimer >> 3) & 1);

//     self->timeFlashFrame = 0;

//     if (((*sceneInfo)->minutes >= 9 && isMainGameMode() && !(globals->medalMods & MEDAL_NOTIMEOVER)) || (*ActClear)->disableTimeBonus)
//         self->timeFlashFrame = (globals->persistentTimer >> 3) & 1;

//     // Draw "Score"
//     self->hudElementsAnimator.frameID = 0;
//     RSDK->DrawSprite(&self->hudElementsAnimator, &scorePos, true);

//     // Draw Score
//     drawPos.x = scorePos.x + 0x630000;
//     drawPos.y = scorePos.y + 0xB0000;
//     HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, player->score, 0);

//     // Draw "Time" Text
//     self->hudElementsAnimator.frameID = self->timeFlashFrame + 1;
//     RSDK->DrawSprite(&self->hudElementsAnimator, &timePos, true);

//     if (!self->enableTimeFlash || globals->persistentTimer & 8) {
//         // Draw : or "
//         drawPos.x                         = timePos.x;
//         drawPos.y                         = timePos.y;
//         self->hudElementsAnimator.frameID = self->extendedHUD ? 12 : 20;
//         RSDK->DrawSprite(&self->hudElementsAnimator, &drawPos, true);

//         self->numbersAnimator.frameID = 0;
//         drawPos.y                     = timePos.y + TO_FIXED(self->numbersAnimator.frames[0].frame.height);
//         if (self->extendedHUD) {
//             // Draw Milliseconds
//             drawPos.x = timePos.x + 0x630000;
//             HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, (*sceneInfo)->milliseconds, 2);
//             drawPos.x -= 0x80000;
//         }
//         else {
//             drawPos.x = timePos.x + 0x4B0000;
//         }

//         if ((*sceneInfo)->minutes > 9) {
//             // Draw Seconds
//             HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, 59, 2);
//             drawPos.x -= 0x80000;

//             // Draw Minutes
//             HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, 9, 1);
//         }
//         else {
//             // Draw Seconds
//             HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, (*sceneInfo)->seconds, 2);
//             drawPos.x -= 0x80000;

//             // Draw Minutes
//             if ((*sceneInfo)->minutes > 9 && globals->medalMods & MEDAL_NOTIMEOVER)
//                 HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, (*sceneInfo)->minutes, 2);
//             else
//                 HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, (*sceneInfo)->minutes, 1);
//         }
//     }

//     // Draw "Rings" Text
//     self->hudElementsAnimator.frameID = self->ringFlashFrame + 3;
//     RSDK->DrawSprite(&self->hudElementsAnimator, &ringPos, true);

//     // Draw Rings
//     if (!self->enableRingFlash || globals->persistentTimer & 8) {
//         self->numbersAnimator.frameID = 0;
//         drawPos.x                     = self->ringsPos.x + (self->extendedHUD ? 0x630000 : 0x4B0000);
//         drawPos.y                     = self->ringsPos.y + TO_FIXED(self->numbersAnimator.frames[self->numbersAnimator.frameID].frame.height);

//         if (player->hyperRing) {
//             HUD_DrawNumbersBase10(self, &self->hyperNumbersAnimator, &drawPos, player->rings, 0);
//             drawPos.x -= 0x40000;
//             self->hyperNumbersAnimator.frameID = 10;
//             RSDK->DrawSprite(&self->hyperNumbersAnimator, &drawPos, true);
//         }
//         else
//             HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, player->rings, 0);
//     }

//     if ((*sceneInfo)->debugMode) {
//         if (player->camera) {
//             // Draw Camera YPos
//             drawPos.x = TO_FIXED((*screenInfo)[player->camera->screenID].size.x - 16);
//             drawPos.y = 0x180000;
//             HUD_DrawNumbersBase16(self, &drawPos, (*screenInfo)[player->camera->screenID].position.y);

//             // Draw Camera XPos
//             drawPos.x -= 0x90000;
//             HUD_DrawNumbersBase16(self, &drawPos, (*screenInfo)[player->camera->screenID].position.x);

//             // Draw Player YPos
//             drawPos.x = TO_FIXED((*screenInfo)[player->camera->screenID].size.x - 16);
//             drawPos.y += 0x100000;
//             HUD_DrawNumbersBase16(self, &drawPos, FROM_FIXED(player->position.y));

//             // Draw Player XPos
//             drawPos.x -= 0x90000;
//             HUD_DrawNumbersBase16(self, &drawPos, FROM_FIXED(player->position.x));
//         }
//     }

//     int32 lives = self->lives[player->playerID];

//     if (globals->somethingRelatedToLives) {
//         drawPos = self->lifePos;

//         if (globals->medalMods & MEDAL_NOLIVES) {
//             self->lifeIconAnimator.frameID  = self->lifeIconAnimator.frameCount - 1;
//             self->lifeNamesAnimator.frameID = self->lifeIconAnimator.frameID;
//             lives                           = globals->coinCount;
//         }
//         else {
//             lives                           = self->lives[player->playerID];
//             self->lifeIconAnimator.frameID  = HUD_CharacterIndexFromID(player->characterID);
//             self->lifeNamesAnimator.frameID = HUD_CharacterIndexFromID(player->characterID);

//             // "Miles" name
//             if ((globals->playerID & 0xFF) == ID_TAILS && globals->secrets & SECRET_REGIONSWAP)
//                 self->lifeNamesAnimator.frameID = self->lifeNamesAnimator.frameCount - 1;
//         }

//         if (!*usePathTracer)
//             self->lifeIconAnimator.frameID = self->lifeIconAnimator.frameCount - 2;

//         if (self->lifeIconAnimator.frameID < 0) {
//             self->lifeIconAnimator.frameID = self->lifeFrameIDs[player->playerID];
//             lives--;
//         }
//         else {
//             self->lifeFrameIDs[player->playerID] = self->lifeIconAnimator.frameID;
//             self->lives[player->playerID]        = player->lives;
//         }

//         RSDK->DrawSprite(&self->lifeIconAnimator, &drawPos, true);

//         switch (globals->gameSpriteStyle) {
//             case GAME_S1:
//             case GAME_CD:
//             case GAME_S2:
//             case GAME_SM:
//                 self->hudElementsAnimator.frameID = 14;
//                 RSDK->DrawSprite(&self->hudElementsAnimator, &drawPos, true);
//                 drawPos.x += 0x300000;
//                 if (player->lives < 10)
//                     drawPos.x -= 0x80000;
//                 HUD_DrawNumbersBase10(self, &self->numbersAnimator, &drawPos, lives, 0);
//                 break;

//             case GAME_S3K:
//             case GAME_S3:
//             case GAME_SK:
//                 RSDK->DrawSprite(&self->lifeNamesAnimator, &drawPos, true);
//                 self->hudElementsAnimator.frameID = 14;

//                 if (globals->useCoins) {
//                     drawPos.x -= 0x20000;
//                     RSDK->DrawSprite(&self->hudElementsAnimator, &drawPos, true);
//                     drawPos.x += 0x2C0000;
//                     HUD_DrawNumbersBase10(self, &self->lifeNumbersAnimator, &drawPos, lives, 0);
//                 }
//                 else {
//                     RSDK->DrawSprite(&self->hudElementsAnimator, &drawPos, true);
//                     drawPos.x += 0x290000;
//                     HUD_DrawNumbersBase10(self, &self->lifeNumbersAnimator, &drawPos, lives, 0);
//                 }
//                 break;
//         }

//     }

//     if (globals->playMode == BOOT_PLAYMODE_MISSION || globals->playMode == BOOT_PLAYMODE_BOSSRUSH){
//         self->scorePos.y = TO_FIXED(-64);
//         self->timePos.y = TO_FIXED(13);
//         self->ringsPos.y = TO_FIXED(29);
//     }
// }

HOOK(void, __fastcall, HUD_Create, 0x1401ce050, void) {

    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;

    originalHUD_Create();

    RSDK_THIS(HUD);

    if (!(*sceneInfo)->inEditor) {
        if (globals->playMode == BOOT_PLAYMODE_MISSION || globals->playMode == BOOT_PLAYMODE_BOSSRUSH) { // If in Mission Mode or Boss Rush, shift UI elements to match HE2 UI
                                                                                                         // No longer on Draw function since it was a weird hack, this is more "proper".
            self->scorePos.y = TO_FIXED(-64);
            self->timePos.y = TO_FIXED(13);
            self->ringsPos.y = TO_FIXED(29);
        }

        RSDK->AddViewableVariable("Extended HUD", &self->extendedHUD, VIEWVAR_UINT8, false, true);

    }
}

// HOOK(void, __fastcall, HUD_Create, 0x1401ce050, void) { // original hook i made, recreated the code and cleaned up unused things from mania.

//     RSDK = *(FunctionTable**)0x142E70150;
//     globals = *(GlobalVariables**)0x144000210;

//     //originalHUD_Create(self);

//     RSDK_THIS(HUD);

//     if (!(*sceneInfo)->inEditor) {
//         (*ActClear)->disableTimeBonus = false;
//         self->active                      = ACTIVE_NORMAL;
//         self->visible                     = true;
//         self->drawGroup                   = 14;
//         self->scorePos.x                  = 0x100000;
//         self->scorePos.y                  = 0x90000;
//         self->timePos.x                   = 0x100000;
//         self->timePos.y                   = 0x190000;
//         self->ringsPos.x                  = 0x100000;
//         self->ringsPos.y                  = 0x290000;
//         self->lifePos.x                   = 0x100000;
//         self->lifePos.y                   = TO_FIXED((*screenInfo)->size.y - 8);

//         if (globals->playMode == BOOT_PLAYMODE_MISSION || globals->playMode == BOOT_PLAYMODE_BOSSRUSH) { // If in Mission Mode or Boss Rush, shift UI elements to match HE2 UI
//                                                                                                          // No longer on Draw function since it was a weird hack, this is more "proper".
//             self->scorePos.y = TO_FIXED(-64);
//             self->timePos.y = TO_FIXED(13);
//             self->ringsPos.y = TO_FIXED(29);
//         }

//         // this->hudElementsAnimator.SetAnimation(sVars->aniFrames, 0, true, 0);
//         // this->numbersAnimator.SetAnimation(sVars->aniFrames, 1, true, 0);
//         // this->hyperNumbersAnimator.SetAnimation(sVars->aniFrames, 9, true, 0);
//         // this->lifeIconAnimator.SetAnimation(sVars->aniFrames, 2, true, 0);
//         // this->lifeNamesAnimator.SetAnimation(sVars->aniFrames, 15, true, 0);
//         // this->lifeNumbersAnimator.SetAnimation(sVars->aniFrames, globals->gameSpriteStyle == GAME_S3K ? 14 : 1, true, 0);
//         // this->playerIDAnimator.SetAnimation(sVars->aniFrames, globals->gameMode == MODE_ENCORE ? 13 : 8, true, 0);
//         // this->superIconAnimator.SetAnimation(sVars->superButtonFrames, 0, true, 0);
//         // this->thumbsUpIconAnimator.SetAnimation(sVars->aniFrames, 10, true, 2);
//         // this->replayClapAnimator.SetAnimation(sVars->aniFrames, 10, true, 1);

//         RSDK->SetSpriteAnimation((*HUD)->aniFrames, 0, &self->hudElementsAnimator, true, 0);
//         RSDK->SetSpriteAnimation((*HUD)->aniFrames, 1, &self->numbersAnimator, true, 0);
//         RSDK->SetSpriteAnimation((*HUD)->aniFrames, 9, &self->hyperNumbersAnimator, true, 0);
//         RSDK->SetSpriteAnimation((*HUD)->aniFrames, 2, &self->lifeIconAnimator, true, 0);
//         RSDK->SetSpriteAnimation((*HUD)->aniFrames, globals->gameSpriteStyle == GAME_S3K ? 14 : 1, &self->lifeNumbersAnimator, true, 0);
//         RSDK->SetSpriteAnimation((*HUD)->aniFrames, 15, &self->lifeNamesAnimator, true, 0);
//         RSDK->SetSpriteAnimation((*HUD)->aniFrames, globals->gameMode == MODE_ENCORE ? 13 : 8, &self->playerIDAnimator, true, 0);

//         RSDK->AddViewableVariable("Show HUD", &self->visible, VIEWVAR_UINT8, false, true);
//         RSDK->AddViewableVariable("Extended HUD", &self->extendedHUD, VIEWVAR_UINT8, false, true);

//     }
// }

HOOK(void, __fastcall, Water_State_Bubbler, 0x1401b9c60, EntityWater *self)
{
    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;
    if (self->visible && RSDK->CheckOnScreen(self, &self->updateRange)) {
        if (!self->countdownID) {
            if (!self->bubbleFlags) {
                self->bubbleFlags = 1;
                int32 rand        = RSDK->Rand(0, 0x10000);
                self->bubbleType1 = rand % 6;
                self->bubbleType2 = rand & 12;

                if (!self->dudsRemaining--) {
                    self->bubbleFlags |= 2;
                    self->dudsRemaining = self->numDuds;
                }
            }
        }
    }
    originalWater_State_Bubbler(self);
}

HOOK(void, __fastcall, Player_State_Peelout, 0x1401eb510, EntityPlayer* self)
{
    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;
    auto Player_State_Ground = (void(__fastcall*)(EntityPlayer* self))SigPlayer_State_Ground();
    auto Camera_State_FollowY = (void(__fastcall*)(EntityCamera* self))0x1401c2560;

    if (self->gravityStrength == 0x1000) {
        if (self->superState == SUPERSTATE_SUPER) {
            self->abilityTimer += 0xA000;
            if (self->abilityTimer > 0x70000)
                self->abilityTimer = 0x70000;
        }
        else {
            self->abilityTimer += 0x6000;
            if (self->abilityTimer > 0x60000)
                self->abilityTimer = 0x60000;
        }
    }
    else if (self->superState == SUPERSTATE_SUPER) {
        self->abilityTimer += 0xA000;
        if (self->abilityTimer > 0xD0000)
            self->abilityTimer = 0xD0000;
    }
    else {
        self->abilityTimer += 0x6000;
        if (self->abilityTimer > 0xC0000)
            self->abilityTimer = 0xC0000;
    }

    if (self->abilityTimer < self->minJogVelocity) {

        RSDK->SetSpriteAnimation(self->aniFrames, ANI_WALK, &self->animator, false, 0);

        self->animator.speed = (self->abilityTimer >> 12) + 16;
        self->minJogVelocity = 0x5F5C2;
    }
    else if (self->abilityTimer < self->minDashVelocity) {
        if (self->animator.animationID == ANI_DASH || self->animator.animationID == ANI_RUN)
            RSDK->SetSpriteAnimation(self->aniFrames, ANI_RUN, &self->animator, false, 1);
        else
            RSDK->SetSpriteAnimation(self->aniFrames, ANI_RUN, &self->animator, false, 0);

        self->animator.speed = (self->abilityTimer >> 12) + 48;
        if (self->animator.speed > 0x100)
            self->animator.speed = 256;
        self->minDashVelocity = 0x9FFFF;
    }
    else {
        if (self->animator.animationID == ANI_DASH || self->animator.animationID == ANI_RUN)
            RSDK->SetSpriteAnimation(self->aniFrames, ANI_DASH, &self->animator, false, 1);
        else
            RSDK->SetSpriteAnimation(self->aniFrames, ANI_DASH, &self->animator, false, 0);

    }

    if (!self->up) {
        RSDK->StopSfx(Player->sfxPeelCharge);

        if (self->abilityTimer >= 0x60000) {
            if (self->camera && !(*Zone)->autoScrollSpeed) {
                self->scrollDelay   = 15;
                self->camera->state.state = (void(__fastcall*)())Camera_State_FollowY;
            }

            if (self->direction)
                self->groundVel = -self->abilityTimer;
            else
                self->groundVel = self->abilityTimer;

            RSDK->PlaySfx(Player->sfxPeelRelease, false, 0xFF);
        }

        self->state.state = (void(__fastcall*)())Player_State_Ground;
    }

}

HOOK(void, __fastcall, FinalFireBall_State_Generator, 0x140212280, EntityFinalFireball* self)
{
    originalFinalFireBall_State_Generator(self);
    if (!((*Zone)->timer & 1)){
        self->position.x += ((*Zone)->autoScrollSpeed * 2);
    }
}

HOOK(void, __fastcall, RockPile_Update, 0x140160bc0, void)
{
    RSDK = *(FunctionTable**)0x142E70150;

    RSDK_THIS(RockPile);

    EntityPlayer *player = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);

    if (player->characterID == ID_AMY && (player->animator.animationID == ANI_HAMMER_JUMP || player->animator.animationID == ANI_HAMMER_DASH)) { // listen, i know this is hacky as fuck, but we need a way to allow amy to break these walls.
        int32 prevSuperState = 0;            // if you are amy and are in either in the Hammer jump or Dash
        prevSuperState = player->superState; // preserve the superState (if there is one)
        Hitbox *hitboxStore;
        Hitbox tempHitbox;

        hitboxStore = player->outerbox;
        player->characterID = ID_SONIC;        // amy is now sonic.
        player->superState = SUPERSTATE_SUPER; // and now they are super.
        if ((self->type == TypePillarH || self->type == TypePillarV) && self->unbreakable == false && self->deactivated == false && self->onlyKnux == false) {
            if (player->animator.animationID == ANI_HAMMER_JUMP) {
                tempHitbox.left = -25;
                tempHitbox.top = -25;
                tempHitbox.right = 25;
                tempHitbox.bottom = 25;
                player->outerbox = &tempHitbox;
            }
            else if (player->animator.animationID == ANI_HAMMER_DASH) {
                switch (player->animator.frameID) {
                    case 0:
                    case 4:
                        tempHitbox.left = -10;
                        tempHitbox.top = -17;
                        tempHitbox.right = 23;
                        tempHitbox.bottom = 17;
                        break;
                    case 1:
                    case 5:
                        tempHitbox.left = -23;
                        tempHitbox.top = -17;
                        tempHitbox.right = 10;
                        tempHitbox.bottom = 17;
                        break;
                    case 2:
                    case 6:
                        tempHitbox.left = -18;
                        tempHitbox.top = -24;
                        tempHitbox.right = 10;
                        tempHitbox.bottom = 17;
                        break;
                    case 3:
                    case 7:
                        tempHitbox.left = -10;
                        tempHitbox.top = -26;
                        tempHitbox.right = 25;
                        tempHitbox.bottom = 17;
                        break;
                }

                // if (player->direction) {
                //     int16 rightHitboxPos = tempHitbox.left * -1;
                //     tempHitbox.left = tempHitbox.right * -1;
                //     tempHitbox.right = rightHitboxPos;
                // }
                player->outerbox = &tempHitbox;
            }
        }

        originalRockPile_Update();             // just so she can actually break the rocks.

        player->characterID = ID_AMY;          // and then she's back to normal.
        player->superState = prevSuperState;
        player->outerbox = hitboxStore;
    }
    else {
        originalRockPile_Update(); // otherwise, run the normal code.
    }
}

HOOK(void, __fastcall, EMZRockPile_Update, 0x140310e30, void)
{
    RSDK = *(FunctionTable**)0x142E70150;
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityPlayer *player2 = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);

    bool32 restoreP1 = false;
    bool32 restoreP2 = false;

    if (player1->characterID == ID_AMY && player1->animator.animationID == ANI_HAMMER_JUMP) {
        player1->animator.animationID = ANI_JUMP;
        restoreP1 = true;
    }

    if (player2->characterID == ID_AMY && player2->animator.animationID == ANI_HAMMER_JUMP) {
        player2->animator.animationID = ANI_JUMP;
        restoreP2 = true;
    }

    originalEMZRockPile_Update();

    if (restoreP1) {
        player1->animator.animationID = ANI_HAMMER_JUMP;
    }
    if (restoreP2) {
        player2->animator.animationID = ANI_HAMMER_JUMP;
    }
}

HOOK(void, __fastcall, Harisenbo_Update, 0x140125ce0, void)
{
    RSDK = *(FunctionTable**)0x142E70150;
    EntityPlayer *player = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);

    if (player->characterID == ID_AMY && (player->animator.animationID == ANI_HAMMER_JUMP || player->animator.animationID == ANI_HAMMER_DASH)) { // same with Harisenbo interactions, this is hacky but it WORKS.
        int32 prevInvincTimer = 0;
        prevInvincTimer = player->invincibleTimer;

        player->invincibleTimer = 80;

        originalHarisenbo_Update();

        player->invincibleTimer = prevInvincTimer;
    }
    else {
        originalHarisenbo_Update();
    }
}


HOOK(void, __fastcall, Puff_State_Move, 0x14012cd60, EntityPuff* self)
{
    RSDK = *(FunctionTable**)0x142E70150;

    EntityPlayer *player = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    
    if (player->characterID == ID_AMY && (player->animator.animationID == ANI_HAMMER_JUMP || player->animator.animationID == ANI_HAMMER_DASH)) {
        int32 prevInvincTimer = 0;
        prevInvincTimer = player->invincibleTimer;

        player->invincibleTimer = 80;

        originalPuff_State_Move(self);

        player->invincibleTimer = prevInvincTimer;
    }
    else {
        originalPuff_State_Move(self);
    }
}

HOOK(void, __fastcall, SignPost2_Create, 0x1400a1200, void* data)
{
    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;

    originalSignPost2_Create(data);

    RSDK_THIS(SignPost2);

    if (!(*sceneInfo)->inEditor) {
        if (globals->gameMode != MODE_TIMEATTACK) {
            uint16 animation = SignPostAnimationIDs::ANI_SPIN;

            if (globals->hasPlusDLC && globals->playMode >= 1) {
                animation = SignPostAnimationIDs::ANI_AMY_SPIN;
            }

            RSDK->SetSpriteAnimation((*SignPost2)->aniFrames, 9, &self->eggPlateAnimator, true, self->type == SignPostTypes::TypeDecoration ? 4 : 3);
        }
    }
}

HOOK(void, __fastcall, OriginsSetChannelAttributes, 0x1400DDDB0, uint32 channel, float volume, float pan, float speed)
{
    bool mirror = ((bool(__fastcall*)())(SigIsMirrorMode()))();
    if (mirror) {
        pan = -pan;
    }

    originalOriginsSetChannelAttributes(channel, volume, pan, speed);
}

HOOK(void, __fastcall, CircleBumper_PlayerInteraction, 0x140191d40, EntityCircleBumper *self)
{
    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;
    auto Player_State_Fly = (void(__fastcall*)(EntityPlayer* self))0x1401e80b0;
    auto Player_State_Dropdash = (void(__fastcall*)(EntityPlayer* self))SigPlayer_State_DropDash();
    auto Player_State_ChargeHammerDash = (void(__fastcall*)(EntityPlayer* self))SigPlayer_State_ChargeHammerDash();
    auto Player_State_Air = (void(__fastcall*)(EntityPlayer* entity))SigPlayer_State_Air();
    auto Player_State_Carried = (void(__fastcall*)(EntityPlayer* entity))SigPlayer_State_Carried();

    EntityPlayer *player = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    if (player->characterID == ID_AMY && player->animator.animationID == ANI_HAMMER_JUMP){
        int16 prevAniSpeed = 0;
        int16 prevAnitimer = 0;
        int32 prevFrame = 0;
        StateMachine prevState;
        int32 prevjumpAbilityState = 0;

        prevAniSpeed = player->animator.speed;
        prevAnitimer = player->animator.timer;
        prevFrame = player->animator.frameID;
        prevState = player->state;
        prevjumpAbilityState = player->jumpAbilityState;

        originalCircleBumper_PlayerInteraction(self);

        if (player->animator.animationID != ANI_HAMMER_JUMP) {
            player->animator.animationID = ANI_HAMMER_JUMP;
            player->animator.prevAnimationID = ANI_HAMMER_JUMP;

            player->animator.speed = prevAniSpeed;
            player->animator.timer = prevAnitimer;
            player->animator.frameID = prevFrame;
            player->state = prevState;
            player->jumpAbilityState = prevjumpAbilityState;
        }
    }
    else {
        originalCircleBumper_PlayerInteraction(self);
    }
}

HOOK(void, __fastcall, Balloon_PlayerInteraction, 0x140111240, EntityBalloon *self)
{
    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;
    auto Player_State_Fly = (void(__fastcall*)(EntityPlayer* self))0x1401e80b0;
    auto Player_State_Dropdash = (void(__fastcall*)(EntityPlayer* self))SigPlayer_State_DropDash();
    auto Player_State_ChargeHammerDash = (void(__fastcall*)(EntityPlayer* self))SigPlayer_State_ChargeHammerDash();
    auto Player_State_Air = (void(__fastcall*)(EntityPlayer* entity))SigPlayer_State_Air();


    originalBalloon_PlayerInteraction(self);
    foreach_active(Player, player)
    {
        if (!Player_CheckCollisionTouch(player, self, &(*Balloon)->hitbox))
             break;

        if (player->state.state != (void(__fastcall*)())Player_State_Fly && player->animator.animationID != ANI_ABILITY_1 && player->state.state != (void(__fastcall*)())Player_State_Dropdash
        && player->state.state != (void(__fastcall*)())Player_State_ChargeHammerDash) {
            player->state.state = (void(__fastcall*)())Player_State_Air;

            if (player->animator.animationID > ANI_JUMP || ((1 << (long)(player->animator.animationID & 0x1f) & 0x8820U) == 0)) {
                player->angle = 0;
                player->animator.animationID = ANI_WALK;
            }
        }
    }
}


HOOK(void, __fastcall, Balloon_Update, 0x140110f10, void)
{
    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;

    RSDK_THIS(Balloon);

    Vector2 range;

    range.x = 0x400000;
    range.y = 0x400000;

    if (self->popped) {
        if (!RSDK->CheckOnScreen(self, &range)) {
            self->popped = false;
            self->position.y = self->startY;
            Balloon_Create(NULL);
        }
        else {
            RSDK->ProcessAnimation(&self->animator);

            if (self->animator.frameID == self->animator.frameCount - 1) {
                if (globals->playMode == BOOT_PLAYMODE_MISSION && globals->missionFunctionNo == MISSIONNO_BALLOONBURST) {
                    destroyEntity(self);
                    RSDK->NotifyCallback(NOTIFY_KILL_ENEMY, 5, 0, 0);
                }
            }
        }
    }
    else {
        Balloon_PlayerInteractionOG(self);
        self->position.y = self->startY + (RSDK->Sin256(++self->angle) << 11);
        RSDK->ProcessAnimation(&self->animator);
    }
}

HOOK(void, __fastcall, S3K_BS_SlotSetup_StaticUpdate, 0x14010d530, void)
{
    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;

    if ((*S3K_BS_SlotSetup)->rotateDirection == FLIP_NONE) {
        switch ((*S3K_BS_SlotSetup)->rotateSpeed) {
            case RotateSpeeds::Speed0: stockValues.realRotation -= (*Zone)->timer & 1; break;
            case RotateSpeeds::Speed1: stockValues.realRotation--; break;
            case RotateSpeeds::SpeedMax: stockValues.realRotation -= 8; break;
        }
        if (stockValues.realRotation < 0)
            stockValues.realRotation += 512;
    }
    else {
        switch ((*S3K_BS_SlotSetup)->rotateSpeed) {
            case RotateSpeeds::Speed0: stockValues.realRotation += (*Zone)->timer & 1; break;
            case RotateSpeeds::Speed1: stockValues.realRotation++; break;
            case RotateSpeeds::SpeedMax: stockValues.realRotation += 8; break;
        }

        stockValues.realRotation &= 511;

    }
    if (globals->playMode == BOOT_PLAYMODE_CLASSIC) {
        (*S3K_BS_SlotSetup)->stageRotation = stockValues.realRotation & 0xFFFFFFF8;
    }
    else {
        (*S3K_BS_SlotSetup)->stageRotation = stockValues.realRotation;
    }

    originalS3K_BS_SlotSetup_StaticUpdate();
}

HOOK(void, __fastcall, S3K_BS_SlotSetup_StageLoad, 0x14010d160, void)
{
    originalS3K_BS_SlotSetup_StageLoad();

    stockValues.realRotation = 0; // set this to 0 upon load to ensure correct rotation start.
}

HOOK(void, __fastcall, S3K_AIZBombing_StageLoad, 0x1402aea60, void)
{
    RSDK = *(FunctionTable**)0x142E70150;
    
    originalS3K_AIZBombing_StageLoad();

    EntityPlayer *player = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);

    if (player->characterID == ID_KNUCKLES)
        (*S3K_AIZBombing)->aniFrames = RSDK->LoadSpriteAnimation("3K_AIZ/FlyingBatteryBombingK.bin", SCOPE_STAGE);
}

HOOK(void, __fastcall, FlingRamp_Update, 0x1401a21b0, void)
{
    RSDK = *(FunctionTable**)0x142E70150;
    auto Player_State_Air = (void(__fastcall*)(EntityPlayer* entity))SigPlayer_State_Air();

    RSDK_THIS(FlingRamp);

    foreach_active(Player, player)
    {
        if (player->onGround) {
            bool32 left  = self->direction == FLIP_NONE || self->direction == FLIP_X;
            bool32 right = self->direction == FLIP_NONE || self->direction == FLIP_Y;

            if (left && !(player->direction & FLIP_X) && player->velocity.x >= 0x40000) {
                if (Player_CheckCollisionTouch(player, self, &(*FlingRamp)->rampHitbox)) {
                    player->state.state = (void(__fastcall*)())Player_State_Air;
                }
            }
            else if (right && (player->direction & FLIP_X) && player->velocity.x <= -0x40000) {
                if (Player_CheckCollisionTouch(player, self, &(*FlingRamp)->rampHitbox)) {
                    player->state.state = (void(__fastcall*)())Player_State_Air;
                }
            }
        }
    }
    originalFlingRamp_Update();
}

HOOK(void, __fastcall, func_1402715a0, 0x1402715a0, void)
{
    RSDK = *(FunctionTable**)0x142E70150;

    //originalfunc_1402715a0();
    // I dont actually know what the hell this function is, but its called during the AIZ transition from Act 1 to 1.5.
    // I assume they didn't want to just stick all this in a different function.
    RSDK->SetPaletteEntry(0, 192, 0x002000); // Fix that one color because this game is silly.
    RSDK->SetPaletteEntry(0, 0xe4, 0x202020);
    RSDK->SetPaletteEntry(0, 0xee, 0x202060);
    RSDK->SetPaletteEntry(0, 0xed, 0x4020a0);
    RSDK->SetPaletteEntry(0, 0xea, 0x600000);
    RSDK->SetPaletteEntry(0, 0xe7, 0x800000);
    RSDK->SetPaletteEntry(0, 0xe9, 0xc04000);
}

HOOK(void, __fastcall, Shield_StageLoad, 0x1401f3550, void)
{
    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;

    char const *ShieldTypeLoad;
    switch (globals->gameSpriteStyle){
        case GAME_S1:
        case GAME_CD:
            ShieldTypeLoad = "S1_Global/Shields.bin";
            break;
        case GAME_S2:
            ShieldTypeLoad = "S2_Global/Shields.bin";
            break;
        case GAME_SM:
            ShieldTypeLoad = "Global/Shields.bin";
            break;
        case GAME_S3K:
        case GAME_S3:
           // if (!globals->playMode)
                ShieldTypeLoad = "3K_Global/Shields.bin";
            //else
                //ShieldTypeLoad = "3K_Global/Shields2.bin";
            break;
        default:
            ShieldTypeLoad = "3K_Global/Shields.bin";
            break;
    }

    (*Shield)->aniFrames = RSDK->LoadSpriteAnimation(ShieldTypeLoad, SCOPE_STAGE);

    (*Shield)->sfxBlueShield      = RSDK->GetSfx("Global/BlueShield.wav");
    (*Shield)->sfxBubbleShield    = RSDK->GetSfx("Global/BubbleShield.wav");
    (*Shield)->sfxFireShield      = RSDK->GetSfx("Global/FireShield.wav");
    (*Shield)->sfxLightningShield = RSDK->GetSfx("Global/LightningShield.wav");
    (*Shield)->sfxInstaShield     = RSDK->GetSfx("Global/InstaShield.wav");
    (*Shield)->sfxBubbleBounce    = RSDK->GetSfx("Global/BubbleBounce.wav");
    (*Shield)->sfxFireDash        = RSDK->GetSfx("Global/FireDash.wav");
    (*Shield)->sfxLightningJump   = RSDK->GetSfx("Global/LightningJump.wav");

    //originalShield_Create(self, data);

}

HOOK(void, __fastcall, SlotsHUD_StageLoad, 0x14010e310, void)
{
    RSDK = *(FunctionTable**)0x142E70150;
    globals = *(GlobalVariables**)0x144000210;

    char const *HUDTypeLoad;
    switch (globals->gameSpriteStyle){
        case GAME_S1:
        case GAME_CD:
            HUDTypeLoad = "S1_Global/HUD.bin";
            (*SlotHUD)->superButtonFrames = RSDK->LoadSpriteAnimation("S1_Global/SuperButtons.bin", SCOPE_STAGE);
            break;
        case GAME_S2:
            HUDTypeLoad = "S2_Global/HUD.bin";
            (*SlotHUD)->superButtonFrames = RSDK->LoadSpriteAnimation("S2_Global/SuperButtons.bin", SCOPE_STAGE);
            break;
        case GAME_SM:
            HUDTypeLoad = "Global/HUD.bin";
            (*SlotHUD)->superButtonFrames = RSDK->LoadSpriteAnimation("Global/SuperButtons.bin", SCOPE_STAGE);
            break;
        case GAME_S3K:
        case GAME_S3:
            HUDTypeLoad = "3K_Global/HUD.bin";
            (*SlotHUD)->superButtonFrames = RSDK->LoadSpriteAnimation("3K_Global/SuperButtons.bin", SCOPE_STAGE);
            break;
        default:
            HUDTypeLoad = "3K_Global/HUD.bin";
            (*SlotHUD)->superButtonFrames = RSDK->LoadSpriteAnimation("3K_Global/SuperButtons.bin", SCOPE_STAGE);
            break;
    }

    (*SlotHUD)->aniFrames = RSDK->LoadSpriteAnimation(HUDTypeLoad, SCOPE_STAGE);

    (*SlotHUD)->sfxClick     = RSDK->GetSfx("Stage/Click.wav");
    (*SlotHUD)->sfxStarpost  = RSDK->GetSfx("Global/StarPost.wav");
    (*SlotHUD)->showTAPrompt = false;

}

HOOK(void, __fastcall, Player_State_GlideDrop, 0x1401e8fc0, EntityPlayer *self)
{
    if (!self->onGround) {
        self->timer = 0;
        self->hurtAirborneTimer = 0; // this actually controls the glide timer... whar
    }
    originalPlayer_State_GlideDrop(self);
}

HOOK(void, __fastcall, Player_Create, 0x1401e0730, EntityPlayer *self)
{
    globals = *(GlobalVariables**)0x144000210;
    if (self->characterID == ID_NONE && Player->sonicFrames == (uint16)-1 && globals->medalMods & MEDAL_PEELOUT) {
        return; // If you somehow just... have no frames at all as "NONE", dont run the create code. 
    }
    originalPlayer_Create(self);
}

extern "C" __declspec(dllexport) void PostInit()
{
    // Install hooks
    INSTALL_HOOK(Player_StaticLoad);
    INSTALL_HOOK(Player_State_KnuxGlideLeft);
    INSTALL_HOOK(Player_State_GlideRight);
    INSTALL_HOOK(Player_State_ChargeHammerDash);
    INSTALL_HOOK(sub_1403A2550);
    INSTALL_HOOK(ActClear_Create);
    INSTALL_HOOK(sub_140302180);
    INSTALL_HOOK(S3K_CompElement_Draw);
    INSTALL_HOOK(S3K_CompElement_State_Carousel);
    INSTALL_HOOK(S3K_CompElement_Create);
    INSTALL_HOOK(LevelSelect_State_Navigate);
    INSTALL_HOOK(Player_State_HammerDash);
    INSTALL_HOOK(NotifyCallback);
    INSTALL_HOOK(Player_StageLoad);
    INSTALL_HOOK(Player_State_Hurt);
    INSTALL_HOOK(Starpost_BonusStars);
    INSTALL_HOOK(Starpost_EnterGachaponBonus);
    INSTALL_HOOK(Starpost_EnterGlowingSpheresBonus);
    INSTALL_HOOK(Starpost_EnterSlotBonus);
    INSTALL_HOOK(Player_Action_Jump);
    INSTALL_HOOK(Player_Action_CarryPlayer);
    INSTALL_HOOK(Player_Action_DblJumpSonic);
    INSTALL_HOOK(Player_DoHyperDash);
    INSTALL_HOOK(Shield_Create);
    INSTALL_HOOK(SlotsHUD_StageLoad);
    INSTALL_HOOK(Shield_Draw);
    //INSTALL_HOOK(Player_State_Air);
    INSTALL_HOOK(Player_State_Peelout);
    INSTALL_HOOK(ScoreBonus_Draw);
    INSTALL_HOOK(Player_State_Dropdash);
    INSTALL_HOOK(Water_State_Bubbler);
    INSTALL_HOOK(func_1402715a0);
    INSTALL_HOOK(HUD_Draw);
    INSTALL_HOOK(HUD_Create);
    INSTALL_HOOK(FinalFireBall_State_Generator);
    INSTALL_HOOK(Player_Action_DblJumpAmy);
    INSTALL_HOOK(S3K_BS_SlotSetup_StaticUpdate);
    INSTALL_HOOK(S3K_BS_SlotSetup_StageLoad);
    INSTALL_HOOK(Player_State_Carried);
    INSTALL_HOOK(Balloon_PlayerInteraction);
    INSTALL_HOOK(Balloon_Update);
    INSTALL_HOOK(RockPile_Update);
    INSTALL_HOOK(Harisenbo_Update);
    INSTALL_HOOK(CircleBumper_PlayerInteraction);
    INSTALL_HOOK(OriginsSetChannelAttributes);
    INSTALL_HOOK(S3K_AIZBombing_StageLoad);
    INSTALL_HOOK(FlingRamp_Update);
    INSTALL_HOOK(Puff_State_Move);
    INSTALL_HOOK(SignPost2_Create);
    INSTALL_HOOK(Player_State_Spindash);
    INSTALL_HOOK(Player_Update);
    INSTALL_HOOK(EMZRockPile_Update);
    INSTALL_HOOK(Player_State_GlideDrop);
    INSTALL_HOOK(Player_Create);
    //INSTALL_HOOK(DebugMode_Update);
    //INSTALL_HOOK(LinkGameLogicDLL);

    WRITE_MEMORY(0x1400ADD3B, 0x90, 0x90);

    //WRITE_MEMORY(0x1401011C2, 0xC6, 0x05, 0xAF, 0x09, 0xCB, 0x03, 0x04, 0x90, 0x90); // Make CD load as a v4 Game / remove v3 legacy loading

    // fix green sphere collect bug. (mania moment.)
    WRITE_MEMORY(0x1402F2216, 0x8D);

    // write a ton of nops to fix bubbles.
    WRITE_MEMORY(0x1401b9d76, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90);
    WRITE_MEMORY(0x1401b9d94, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90);
    WRITE_MEMORY(0x1401b9da1, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90);
    WRITE_MEMORY(0x1401b9dae, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90);
    WRITE_MEMORY(0x1401b9dc4, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90);


    // Write a ton of nops to rewrite slots rotation
    for (int i = 0; i < 29; ++i)
        WRITE_MEMORY((0x14010d579 + i), 0x90);

    for (int i = 0; i < 17; ++i)
        WRITE_MEMORY((0x14010d59d + i), 0x90);

    for (int i = 0; i < 4; ++i)
        WRITE_MEMORY((0x14010d5B0 + i), 0x90);

    for (int i = 0; i < 13; ++i)
        WRITE_MEMORY((0x14010d5bd + i), 0x90);

    for (int i = 0; i < 19; ++i)
        WRITE_MEMORY((0x14010d5cc + i), 0x90);

    for (int i = 0; i < 4; ++i)
        WRITE_MEMORY((0x14010d5e1 + i), 0x90);

    for (int i = 0; i < 13; ++i)
        WRITE_MEMORY((0x14010d5ee + i), 0x90);

    for (int i = 0; i < 7; ++i)
        WRITE_MEMORY((0x14010d602 + i), 0x90);

    for (int i = 0; i < 5; ++i)
        WRITE_MEMORY((0x14010d610 + i), 0x90);


    // get rid of code in Balloon interaction
    for (int i = 0; i < 7; ++i)
        WRITE_MEMORY((0x1401112fb + i), 0x90);

    for (int i = 0; i < 6; ++i)
        WRITE_MEMORY((0x140111305 + i), 0x90);

    for (int i = 0; i < 8; ++i)
        WRITE_MEMORY((0x14011130d + i), 0x90);

    for (int i = 0; i < 5; ++i)
        WRITE_MEMORY((0x14011131E + i), 0x90);

    for (int i = 0; i < 5; ++i)
        WRITE_MEMORY((0x14011131E + i), 0x90);

    for (int i = 0; i < 12; ++i)
        WRITE_MEMORY((0x14011132D + i), 0x90);

    for (int i = 0; i < 2; ++i)
        WRITE_MEMORY((0x14011133D + i), 0x90);

    for (int i = 0; i < 5; ++i)
        WRITE_MEMORY((0x140111344 + i), 0x90);

    for (int i = 0; i < 10; ++i)
        WRITE_MEMORY((0x14011134E + i), 0x90);

    // Fix SignPost2 flying when in Run Past type.
    for (int i = 0; i < 2; ++i)
        WRITE_MEMORY((0x1400a1525 + i), 0x90);

    for (int i = 0; i < 7; ++i)
        WRITE_MEMORY((0x1400a1538 + i), 0x90);

    // nop code in SlotHUD draw
    WRITE_MEMORY(0x14010e0b8, 0x90, 0x90);

    // and fix the hud drawing in Rotating Slots Bonus.
    WRITE_MEMORY(0x14010E0D8, 0x81, 0x6D, 0x38, 0x00, 0x00, 0x03, 0x00, 0x48, 0x8D, 0x55, 0x38, 0x48, 0xA1, 0x50, 0x01, 0xE7, 0x42, 0x01, 0x00, 0x00, 0x00, 0x41, 0xB8, 0x01, 0x00, 0x00, 0x00, 0x49, 0x8B, 0xCF, 0xFF, 0x90, 0xE8, 0x02, 0x00, 0x00, 0x81, 0x45, 0x38, 0x00, 0x00, 0x2C, 0x00, 0x48, 0x8D, 0x97, 0x30, 0x01, 0x00, 0x00, 0xEB, 0x31);

    // remove movement code from Fireball_State_Generator
    WRITE_MEMORY(0x1402122fd, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90);
    WRITE_MEMORY(0x140212308, 0x90, 0x90, 0x90, 0x90);

    // Change Debug Mode lives monitor check to abide by coinMode instead of playMode
    WRITE_MEMORY(0x1401c71b3, 0x28);
    WRITE_MEMORY(0x1401c7278, 0x28);

    // Fix life numbers being big in SlotHUD_Create when in any other mode that's not S3K (this breaks Mania mode but whos playing in that gametype????)
    WRITE_MEMORY(0x14010dbb4, 0x0E);

    // Fix Saruder's coconut throw (??? it must've been using mania values)
    WRITE_MEMORY(0x140285d29, 0x20);

    // remove end of Player_Action_Jump for new hook
    WRITE_MEMORY(0x1401ddd85, 0x90, 0x90, 0x90, 0x90, 0x90);
    WRITE_MEMORY(0x1401ddd99, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90);

    // remove active and visibility from Shield Create
    WRITE_MEMORY(0x1401f2f36, 0x90, 0x90, 0x90, 0x90);
    WRITE_MEMORY(0x1401f2f3d, 0x90, 0x90, 0x90, 0x90);


    // Make S2 shield less transparent.
    WRITE_MEMORY(0x1401f2fe0, 0xD0); // set the shield alpha
    WRITE_MEMORY(0x1401f2fdc, 2);    // set ink effect to Alpha rather than Mania's Add.

    // make S2 shield always transparent, like S2 in the collection.
    WRITE_MEMORY(0x1401f2fd0, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90);

    // Fix one s3 1Up String
    WRITE_MEMORY(0x140b9375c, 0x55);

    // Change ItemBox debug type limit to max.
    // Note: This makes Mighty, Ray, and Coin monitors appear in Classic Mode, as well as Anniversary for Coin and Amy.
    // WRITE_MEMORY(0x1401d207C, 0x14);

    // Fix Knux Glide to be not like mania's when gliding from the top of a stage
    WRITE_MEMORY(((char*)SigPlayer_State_KnuxGlideLeft() + 0x282), 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90);
    WRITE_MEMORY(((char*)SigPlayer_State_GlideRight() + 0x255), 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90);


    // Fix SpecialClear
    for (int i = 0; i < 2; ++i)
        WRITE_MEMORY(((intptr_t)SigSpecialClear_State_FigureOutWhatToDoNext_D0() + 14 * i), 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90);

    // Fix Amy's jumpOffset
    WRITE_MEMORY(((char*)SigPlayer_Create_266() + 8), 0x04);

    // Change the screenshake to only work on Y for the Drop Dash
    WRITE_MEMORY(((char*)SigPlayer_State_DropDash() + 0xA6), 0x41, 0xB8, 0x06, 0x00, 0x00, 0x00, 0x31, 0xD2, 0xC7, 0x44, 0x24, 0x20, 0x01, 0x00, 0x00, 0x00, 0x45, 0x8D, 0x48, 0xFB);

    // Fix white fade for each Bonus Stage entry function.
    WRITE_MEMORY(((char*)SigStarPost_EnterGachaponBonus() + 0x3C), 0, 0, 0);
    WRITE_MEMORY(((char*)SigStarPost_EnterGlowingSpheresBonus() + 0x3C), 0, 0, 0);
    WRITE_MEMORY(((char*)SigStarPost_EnterSlotBonus() + 0x3C), 0, 0, 0);

    // Change Music_StopTrack for each bonus function to be a Return instead.
    WRITE_MEMORY(((char*)SigStarPost_EnterGachaponBonus() + 0x4E), 0xC3, 0x90, 0x90, 0x90, 0x90);
    WRITE_MEMORY(((char*)SigStarPost_EnterGlowingSpheresBonus() + 0x4E), 0xC3, 0x90, 0x90, 0x90, 0x90);
    WRITE_MEMORY(((char*)SigStarPost_EnterSlotBonus() + 0x4E), 0xC3, 0x90, 0x90, 0x90, 0x90);
}

extern "C" __declspec(dllexport) void Init(ModInfo* modInfo)
{
    // Scan signatures
    SigPlayer_State_KnuxGlideLeft();
    SigPlayer_State_ChargeHammerDash();
    SigCamera_ShakeScreen();
    Sigsub_1403A2550();
    SigIsMirrorMode();
    SigSpecialClear_State_FigureOutWhatToDoNext_D0();
    SigPlayer_Create_266();
    SigActClear_Create();
    Sigsub_140302180();
    SigS3K_CompElement_Draw();
    SigS3K_CompElement_State_Carousel();
    SigS3K_CompElement_Create();
    SigLevelSelect_State_Navigate();
    SigPlayer_State_HammerDash();
    SigPlayer_State_Roll();
    SigPlayer_State_Air();
    SigLinkGameLogicDLL();
    SigPlayer_StaticLoad();
    SigShield_State_Insta();
    SigPlayer_State_DropDash();
    SigPlayer_HandleGroundRotation();
    SigNotifyCallback();
    SigCopyPalette();
    SigPlayer_StageLoad();
    SigPlayer_State_Hurt();
    SigStarPost_EnterGachaponBonus();
    SigStarPost_EnterGlowingSpheresBonus();
    SigStarPost_EnterSlotBonus();
    SigMusic_FadeToStop();
    SigPlayer_State_GlideRight();
    SigPlayer_State_Ground();
    SigPlayer_Gravity_False();
    SigPlayer_Action_CarryPlayer();
    SigPlayer_State_Carried();
    SigPlayer_Action_DblJumpSonic();
    SigPlayer_Input_AI_Follow();
    SigPlayer_DoHyperDash();
    SigShield_Create();

    // Check signatures
    if (!SigValid)
    {
        MessageBoxA(nullptr, InvalidSig, "", NULL);
        MessageBoxW(nullptr, L"Signature Scan Failed!\n\nThis usually means there is a conflict or the mod is running on an incompatible game version.", L"Scan Error", MB_ICONERROR);
        return;
    }
}