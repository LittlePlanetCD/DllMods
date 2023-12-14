#pragma once

// RSDK Macros
#define RETRO_REV02 1
#define RETRO_REV0U 1
#define TO_FIXED(x)   ((x) << 16)
#define RETRO_HASH_MD5(name) uint32 name[4]

// RSDK Primitive Types
typedef int int32;
typedef int uint32;
typedef short int16;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef int bool32;


typedef char int8;
typedef uint32 color;

// Others
typedef enum
{
    SUPERSTATE_NONE,
    SUPERSTATE_FADEIN,
    SUPERSTATE_SUPER,
    SUPERSTATE_FADEOUT,
    SUPERSTATE_DONE,
} SuperStates;

enum CharacterIDs
{
    ID_NONE = 0x0,
    ID_SONIC = 0x1,
    ID_TAILS = 0x2,
    ID_KNUCKLES = 0x4,
    ID_AMY = 0x8,
};

enum GameModes
{
    MODE_NOSAVE = 0x0,
    MODE_SAVEGAME = 0x1,
    MODE_TIMEATTACK = 0x2
};

enum VariableTypes : uint8
{
    VAR_UINT8 = 0x0,
    VAR_UINT16 = 0x1,
    VAR_UINT32 = 0x2,
    VAR_INT8 = 0x3,
    VAR_INT16 = 0x4,
    VAR_INT32 = 0x5,
    VAR_ENUM = 0x6,
    VAR_BOOL = 0x7,
    VAR_STRING = 0x8,
    VAR_VECTOR2 = 0x9,
    VAR_FLOAT = 0xA,
    VAR_COLOR = 0xB,
};


// RSDKv5 Structures
struct Animator
{
    void* frames;
    int32 frameID;
    int16 animationID;
    int16 prevAnimationID;
    int16 speed;
    int16 timer;
    int16 frameDuration;
    int16 frameCount;
    uint8 loopIndex;
    uint8 rotationStyle;
};

struct Vector2
{
    int32 x;
    int32 y;
};

struct __declspec(align(8)) StateMachine
{
    void(__fastcall* state)();
    int timer;
    char field_C;
    char field_D;
    char field_E;
    char priority;
};

struct Entity
{
    void* vfTable;
    Vector2 position;
    Vector2 scale;
    Vector2 velocity;
    Vector2 updateRange;
    int32 angle;
    int32 alpha;
    int32 rotation;
    int32 groundVel;
    int32 zdepth;
    uint16 group;
    uint16 classID;
    bool32 inRange;
    bool32 isPermanent;
    bool32 tileCollisions;
    bool32 interaction;
    bool32 onGround;
    uint8 active;
    uint8 filter;
    uint8 direction;
    uint8 drawGroup;
    uint8 collisionLayers;
    uint8 collisionPlane;
    uint8 collisionMode;
    uint8 drawFX;
    uint8 inkEffect;
    uint8 visible;
    uint8 onScreen;
};

struct EntityCamera : Entity
{
    StateMachine state;
    Entity* target;
    int32 screenID;
    Vector2 center;
    Vector2 targetMoveVel;
    Vector2 lastPos;
    Vector2 shakePos;
    Vector2 lookPos;
    Vector2 offset;
    bool32 disableYOffset;
    int32 centerY;
    int32 adjustY;
    int32 lerpPercent;
    int32 lerpSpeed;
    int32 lerpType;
    Vector2 endLerpPos;
    Vector2 startLerpPos;
    Vector2 boundsOffset;
    int32 boundsL;
    int32 boundsR;
    int32 boundsT;
    int32 boundsB;
};

struct EntityPlayer : Entity
{
    char padding0[144];
    StateMachine state;
    StateMachine nextAirState;
    StateMachine nextGroundState;
    char padding1[16];
    EntityCamera* camera;
    Animator animator;
    Animator tailAnimator;
    char padding2[30];
    uint16 playerID;
    char padding3[16];
    int32 characterID;
    int32 rings;
    int32 ringExtraLife;
    int32 shield;
    char padding4[60];
    int32 superState;
    char padding5[168];
    int32 controllerID;
    int32 controlLock;
    bool32 up;
    bool32 down;
    bool32 left;
    bool32 right;
    bool32 jumpPress;
    bool32 jumpHold;
    char padding6[4];
    int32 jumpAbilityState;
    char padding7[234];
    int32 chargeTimer;
};

struct ObjectZone
{
    char padding0[336];
    int32 cameraBoundsL[4];
    int32 cameraBoundsR[4];
    int32 cameraBoundsT[4];
    int32 cameraBoundsB[4];
    int32 playerBoundsL[4];
    int32 playerBoundsR[4];
    int32 playerBoundsT[4];
};

struct InputState
{
    bool32 down;
    bool32 press;
    int32 keyMap;
};

struct ControllerState
{
    InputState keyUp;
    InputState keyDown;
    InputState keyLeft;
    InputState keyRight;
    InputState keyA;
    InputState keyB;
    InputState keyC;
    InputState keyX;
    InputState keyY;
    InputState keyZ;
    InputState keyStart;
    InputState keySelect;
};

struct SceneListInfo
{
    RETRO_HASH_MD5(hash);
    char name[0x20];
    uint16 sceneOffsetStart;
    uint16 sceneOffsetEnd;
    uint8 sceneCount;
};

struct SceneListEntry
{
    RETRO_HASH_MD5(hash);
    char name[0x20];
    char folder[0x10];
    char id[0x08];
};


struct SceneInfo
{
    Entity* entity;
    SceneListEntry* listData;
    SceneListInfo* listCategory;
    int32 timeCounter;
    int32 currentDrawGroup;
    int32 currentScreenID;
    uint16 listPos;
    uint16 entitySlot;
    uint16 createSlot;
    uint16 classCount;
    bool32 inEditor;
    bool32 effectGizmo;
    bool32 debugMode;
    bool32 useGlobalObjects;
    bool32 timeEnabled;
    uint8 activeCategory;
    uint8 categoryCount;
    uint8 state;
#if RETRO_REV02
    uint8 filter;
#endif
    uint8 milliseconds;
    uint8 seconds;
    uint8 minutes;
};

struct EntityActClear
{
    Entity entity;
    StateMachine(state);
    int32 timer;
    int32 stageFinishTimer;
    int32 timeBonus;
    int32 ringBonus;
    int32 coolBonus;
    int32 totalScore;
    int32 time;
    int32 newRecordTimer;
    bool32 achievedRank;
    bool32 isNewRecord;
    bool32 showCoolBonus;
    Vector2 playerNamePos;
    Vector2 gotThroughPos;
    Vector2 timeBonusPos;
    Vector2 ringBonusPos;
    Vector2 coolBonusPos;
    Vector2 totalScorePos;
    EntityPlayer* targetPlayer;
    // ...
};

struct EntityShield : Entity
{
    Entity entity;
    EntityPlayer* player;
    StateMachine state;
    int32 type;
    int32 timer;
    int32 frameFlags;
    bool32 forceVisible;
    Animator shieldAnimator;
    Animator fxAnimator;
};

typedef enum {
    ACTIVE_NEVER,
    ACTIVE_ALWAYS,
    ACTIVE_NORMAL,
    ACTIVE_PAUSED,
    ACTIVE_BOUNDS,
    ACTIVE_XBOUNDS,
    ACTIVE_YBOUNDS,
    ACTIVE_RBOUNDS,

    // Not really even a real active value, but some objects set their active states to this so here it is I suppose
    ACTIVE_DISABLED = 0xFF,
} ActiveFlags;

typedef enum {
    MEDAL_DEBUGMODE   = 1 << 0,
    MEDAL_ANDKNUCKLES = 1 << 1,
    MEDAL_PEELOUT     = 1 << 2,
    MEDAL_INSTASHIELD = 1 << 3,
    MEDAL_NODROPDASH  = 1 << 4,
    MEDAL_NOTIMEOVER = 1 << 5,
} MedalMods;

typedef enum {
    SHIELD_NONE,
    SHIELD_BLUE,
    SHIELD_BUBBLE,
    SHIELD_FIRE,
    SHIELD_LIGHTNING,
} ShieldTypes;

typedef enum {
    ENGINESTATE_LOAD,
    ENGINESTATE_REGULAR,
    ENGINESTATE_PAUSED,
    ENGINESTATE_FROZEN,
    ENGINESTATE_STEPOVER = 4,
    ENGINESTATE_DEVMENU  = 8,
    ENGINESTATE_VIDEOPLAYBACK,
    ENGINESTATE_SHOWIMAGE,
    ENGINESTATE_ERRORMSG,
    ENGINESTATE_ERRORMSG_FATAL,
    ENGINESTATE_NONE,
} EngineStates;

struct Matrix
{
  int32 values[4][4];
};

struct String
{
  uint16 *chars;
  uint16 length;
  uint16 size;
};

typedef enum {
    SCOPE_NONE,
    SCOPE_GLOBAL,
    SCOPE_STAGE,
} Scopes;

typedef enum { FLIP_NONE, FLIP_X, FLIP_Y, FLIP_XY } FlipFlags;

struct GlobalVariables
{
    GameModes gameMode;
    CharacterIDs playerID;
    int specialCleared;
    int specialRingID;
    int blueSpheresID;
    int blueSpheresInit;
    int atlEnabled;
    int atlEntityCount;
    int atlEntitySlot[288];
    void* atlEntityData[288][276];
    int saveLoaded;
    int saveRAM[7168];
    int saveSlotID;
    int noSaveSlot[256];
    int menuParam[16384];
    int itemMode;
    int suppressTitlecard;
    int suppressAutoMusic;
    int competitionSession[16384];
    int medalMods;
    // ...
};

struct Hitbox
{
    int16 left;
    int16 top;
    int16 right;
    int16 bottom;
};

struct GameSpriteFrame
{
    int16 sprX;
    int16 sprY;
    int16 width;
    int16 height;
    int16 pivotX;
    int16 pivotY;
    uint16 duration;
    uint16 unicodeChar;
    uint8 sheetID;
};

struct SpriteFrame
{
    GameSpriteFrame frame;
    Hitbox hitboxes[8];
};

struct FunctionTable
{
    void(__fastcall* RegisterGlobalVariables)(void** globals, int32 size, void(__stdcall* initCB)(void* globals));
    void(__fastcall* RegisterObject)(void** staticVars, const char* name, uint32 entityClassSize, uint32 staticClassSize, void(__stdcall* update)(), void(__stdcall* lateUpdate)(), void(__stdcall* staticUpdate)(), void(__stdcall* draw)(), void(__stdcall* create)(void*), void(__stdcall* stageLoad)(), void(__stdcall* editorDraw)(), void(__stdcall* editorLoad)(), void(__stdcall* serialize)(), void(__stdcall* staticLoad)(void*));
    void(__fastcall* RegisterStaticVariables)(void** staticVars, const char* name, uint32 classSize);
    bool32(__fastcall* GetActiveEntities)(uint16 group, Entity** entity);
    bool32(__fastcall* GetAllEntities)(uint16 classID, Entity** entity);
    void(__fastcall* BreakForeachLoop)();
    void(__fastcall* SetEditableVar)(VariableTypes type, const char* name, uint8 classID, int32 offset);
    Entity* (__fastcall* GetEntity)(uint16 slot);
    int32(__fastcall* GetEntitySlot)(Entity* entity);
    int32(__fastcall* GetEntityCount)(uint16 classID, bool32 isActive);
    uint16(__fastcall* GetDrawListRefSlot)(uint8 drawGroup, uint16 listPos);
    Entity* (__fastcall* GetDrawListRef)(uint8 drawGroup, uint16 listPos);
    void(__fastcall* ResetEntity)(Entity* entity, uint16 classID, void* data);
    void(__fastcall* ResetEntitySlot)(uint16 slot, uint16 classID, void* data);
    Entity* (__fastcall* CreateEntity)(uint16 classID, void* data, int32 x, int32 y);
    void(__fastcall* CopyEntity)(void* destEntity, void* srcEntity, bool32 clearSrcEntity);
    bool32(__fastcall* CheckOnScreen)(Entity* entity, Vector2* range);
    bool32(__fastcall* CheckPosOnScreen)(Vector2* position, Vector2* range);
    void(__fastcall* AddDrawListRef)(uint8 drawGroup, uint16 entityID);
    void(__fastcall* SwapDrawListEntries)(uint8 drawGroup, uint16 slot1, uint16 slot2, uint16 count);
    void(__fastcall* SetDrawGroupProperties)(uint8 drawGroup, bool32 sorted, void(__stdcall* hookCB)());
    void(__fastcall* SetScene)(const char* categoryName, const char* sceneName);
    void(__fastcall* SetEngineState)(EngineStates state);
    void(__fastcall* ForceHardReset)(bool32 shouldHardReset);
    bool32(__fastcall* CheckValidScene)();
    bool32(__fastcall* CheckSceneFolder)(const char* folderName);
    void(__fastcall* LoadScene)();
    uint16(__fastcall* FindObject)(const char* name);
    void(__fastcall* ClearCameras)();
    void(__fastcall* AddCamera)(Vector2* targetPos, int32 offsetX, int32 offsetY, bool32 worldRelative);
    int32(__fastcall* GetVideoSetting)(int32 id);
    void(__fastcall* SetVideoSetting)(int32 id, int32 value);
    void(__fastcall* UpdateGameWindow)();
    int32(__fastcall* Sin1024)(int32 angle);
    int32(__fastcall* Cos1024)(int32 angle);
    int32(__fastcall* ATan1024)(int32 angle);
    int32(__fastcall* ASin1024)(int32 angle);
    int32(__fastcall* ACos1024)(int32 angle);
    int32(__fastcall* Sin512)(int32 angle);
    int32(__fastcall* Cos512)(int32 angle);
    int32(__fastcall* ATan512)(int32 angle);
    int32(__fastcall* ASin512)(int32 angle);
    int32(__fastcall* ACos512)(int32 angle);
    int32(__fastcall* Sin256)(int32 angle);
    int32(__fastcall* Cos256)(int32 angle);
    int32(__fastcall* ATan256)(int32 angle);
    int32(__fastcall* ASin256)(int32 angle);
    int32(__fastcall* ACos256)(int32 angle);
    int32(__fastcall* Rand)(int32 min, int32 max);
    int32(__fastcall* RandSeeded)(int32 min, int32 max, int32* randSeed);
    void(__fastcall* SetRandSeed)(int32 key);
    int8(__fastcall* ATan2)(int32 X, int32 Y);
    void* padding1[41];
    void(__fastcall* DrawSprite)(Animator* animator, Vector2* position, bool32 screenRelative);
    void(__fastcall* DrawDeformedSprite)(uint16 sheetID, int32 inkEffect, int32 alpha);
    void(__fastcall* DrawString)(Animator* animator, Vector2* position, String* string, int32 startFrame, int32 endFrame, int32 align, int32 spacing, void* unused, Vector2* charOffsets, bool32 screenRelative);
    void(__fastcall* DrawTile)(uint16* tileInfo, int32 countX, int32 countY, Vector2* position, Vector2* offset, bool32 screenRelative);
    void(__fastcall* CopyTile)(uint16 dest, uint16 src, uint16 count);
    void(__fastcall* DrawAniTile)(uint16 sheetID, uint16 tileIndex, uint16 srcX, uint16 srcY, uint16 width, uint16 height);
    void(__fastcall* DrawDynamicAniTile)(Animator* animator, uint16 tileIndex);
    void(__fastcall* FillScreen)(uint32 color, int32 alphaR, int32 alphaG, int32 alphaB);
    uint16(__fastcall* LoadMesh)(const char* filename, uint8 scope);
    uint16(__fastcall* Create3DScene)(const char* name, uint16 faceCnt, uint8 scope);
    void(__fastcall* Prepare3DScene)(uint16 sceneID);
    void(__fastcall* SetDiffuseColor)(uint16 sceneID, uint8 x, uint8 y, uint8 z);
    void(__fastcall* SetDiffuseIntensity)(uint16 sceneID, uint8 x, uint8 y, uint8 z);
    void(__fastcall* SetSpecularIntensity)(uint16 sceneID, uint8 x, uint8 y, uint8 z);
    void(__fastcall* AddModelToScene)(uint16 modelFrames, uint16 sceneIndex, uint8 drawMode, Matrix* matWorld, Matrix* matView, color color);
    void(__fastcall* SetModelAnimation)(uint16 model, Animator* animator, int16 speed, uint8 loopIndex, bool32 forceApply, int16 frameID);
    void(__fastcall* AddMeshFrameToScene)(uint16 modelFrames, uint16 sceneIndex, Animator* animator, uint8 drawMode, Matrix* matWorld, Matrix* matView, color color);
    void(__fastcall* Draw3DScene)(int32 spriteSheetID);
    uint16(__fastcall* LoadSpriteAnimation)(const char* filePath, Scopes scope);
    uint16(__fastcall* CreateSpriteAnimation)(const char* filename, uint32 frameCount, uint32 animCount, uint8 scope);
    void(__fastcall* SetSpriteAnimation)(uint16 aniFrames, uint16 animationID, Animator* animator, bool32 forceApply, int32 frameID);
    void(__fastcall* EditSpriteAnimation)(uint16 aniFrames, uint16 animID, const char* name, int32 frameOffset, uint16 frameCount, int16 animSpeed, uint8 loopIndex, uint8 rotationStyl);
    void(__fastcall* SetSpriteString)(uint16 aniFrames, uint16 animID, String* string);
    uint16(__fastcall* FindSpriteAnimation)(uint16 aniFrames, const char* name);
    SpriteFrame* (__fastcall* GetFrame)(uint16 aniFrames, uint16 anim, int32 frame);
    Hitbox* (__fastcall* GetHitbox)(Animator* animator, uint8 hitboxID);
    // ...
};

struct ObjectPlayer
{
    char padding0[0x1174];
    int32 playerCount;
};
