#pragma once

// RSDK Macros
#define RETRO_REV02 1
#define RETRO_REV0U 1
#define RETRO_USE_DEV_CODE (0) // Turns on debugging features that should not be in release.
// floating point variants
#define TO_FIXED_F(x)   ((x)*65536.0)
#define FROM_FIXED_F(x) ((x) / 65536.0)
#define isMainGameMode() (globals->gameMode == MODE_MANIA || globals->gameMode == MODE_ENCORE)

#define TO_FIXED(x)   ((int)TO_FIXED_F(x))
#define FROM_FIXED(x) ((int)FROM_FIXED_F(x))
#define RETRO_HASH_MD5(name) uint32 name[4]
#define RSDK_THIS(class)                  Entity##class *self = (Entity##class *)(*sceneInfo)->entity
#define RSDK_GET_ENTITY(slot, class)      ((Entity##class *)RSDK->GetEntity(slot))
#define CREATE_ENTITY(object, data, x, y) ((Entity##object *)RSDK->CreateEntity(object->classID, data, x, y))
#define StateMachine_None NULL

#define INT_TO_VOID(x)   (void *)(size_t)(x)
#define FLOAT_TO_VOID(x) INT_TO_VOID(*(int32 *)&(x))
#define VOID_TO_INT(x)   (int32)(size_t)(x)
#define VOID_TO_FLOAT(x) *(float *)&(x)

#define TABLE(var, ...)  var

#define foreach_active(type, entityOut)                                                                                                              \
    Entity##type *entityOut = NULL;                                                                                                                  \
    while (RSDK->GetActiveEntities(type->classID, (void **)&entityOut))
#define foreach_all(type, entityOut)                                                                                                                 \
    Entity##type *entityOut = NULL;                                                                                                                  \
    while (RSDK->GetAllEntities(type->classID, (void **)&entityOut))

#define foreach_active_type(type, entityOut)                                                                                                           \
    Entity *entityOut = NULL;                                                                                                                        \
    while (RSDK->GetActiveEntities(type, (void **)&entityOut))

#define foreach_all_type(type, entityOut)                                                                                                          \
    Entity *entityOut = NULL;                                                                                                                        \
    while (RSDK->GetAllEntities(type, (void **)&entityOut))

#define MIN(a, b)                      ((a) < (b) ? (a) : (b))
#define MAX(a, b)                      ((a) > (b) ? (a) : (b))
#define CLAMP(value, minimum, maximum) (((value) < (minimum)) ? (minimum) : (((value) > (maximum)) ? (maximum) : (value)))
#define FABS(a)                        ((a) > 0 ? (a) : -(a))

#define SET_BIT(value, set, pos) ((value) ^= (-(int32)(set) ^ (value)) & (1 << (pos)))
#define GET_BIT(b, pos)          ((b) >> (pos)&1)

typedef enum {
    TYPE_BLANK,
#if RETRO_REV02
    TYPE_DEVOUTPUT,
#endif
} DefaultObjTypes;

#define destroyEntity(entity)   RSDK->ResetEntity(entity, TYPE_BLANK, NULL)
#define destroyEntitySlot(slot) RSDK->ResetEntitySlot(slot, TYPE_BLANK, NULL)

// RSDK Primitive Types
typedef int int32;
typedef int uint32;
typedef short int16;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef int bool32;


typedef char int8;
typedef uint32 color;

enum ReservedEntities {
    SLOT_PLAYER1         = 0,
    SLOT_PLAYER2         = 1,
    SLOT_PLAYER3         = 2,
    SLOT_PLAYER4         = 3,
    SLOT_POWERUP1        = 4,
    SLOT_POWERUP2        = 5,
    SLOT_POWERUP3        = 6,
    SLOT_POWERUP4        = 7,
    SLOT_POWERUP1_2      = 8,
    SLOT_POWERUP2_2      = 9,
    SLOT_POWERUP3_2      = 10,
    SLOT_POWERUP4_2      = 11,
    SLOT_SPECIAL_SETUP   = 8,
    SLOT_MUSIC           = 9,
    SLOT_SPECIAL_HUD     = 10,
    SLOT_SPECIAL_MESSAGE = 11,
    SLOT_ZONE            = 12,
    // 13 = ???
    // 14 = ???
    SLOT_CUTSCENESEQ      = 15,
    SLOT_PAUSEMENU        = 16,
    SLOT_GAMEOVER         = 16,
    SLOT_ACTCLEAR         = 16,
    SLOT_MUSICSTACK_START = 40,
    //[41-47] are part of the music stack
    SLOT_MUSICSTACK_END = 48,
    SLOT_CAMERA1        = 60,
    SLOT_CAMERA2        = 61,
    SLOT_CAMERA3        = 62,
    SLOT_CAMERA4        = 63,
};

// MusicTracks
enum Tracks {
    TRACK_NONE       = -1,
    TRACK_STAGE      = 0,
    TRACK_INVINCIBLE = 1,
    TRACK_SNEAKERS   = 2,
    TRACK_MINIBOSS   = 3,
    TRACK_BOSS       = 4,
    TRACK_KNUCKLES   = 5,
    TRACK_ACTCLEAR   = 6,
    TRACK_DROWNING   = 7,
    TRACK_GAMEOVER   = 8,
    TRACK_SUPER      = 9,
    TRACK_SOUNDTEST  = 10,
    TRACK_1UP        = 11,
};

struct Hitbox
{
    int16 left;
    int16 top;
    int16 right;
    int16 bottom;
};

//struct SoundFX {
    //uint16 id;
//};

struct SoundInfo {
    uint16 playFlags;
    uint16 sfx;
    uint16 loopPoint;
};

typedef enum {
    CMODE_FLOOR,
    CMODE_LWALL,
    CMODE_ROOF,
    CMODE_RWALL,
} CModes;

enum PlayerAnimationIDs {
    ANI_IDLE,
    ANI_BORED_1,
    ANI_BORED_2,
    ANI_LOOK_UP,
    ANI_CROUCH,
    ANI_WALK,
    ANI_WALK_ANGLED,
    ANI_FALL,
    ANI_FALL_ANGLED,
    ANI_JOG,
    ANI_JOG_ANGLED,
    ANI_RUN,
    ANI_RUN_ANGLED,
    ANI_DASH,
    ANI_DASH_ANGLED,
    ANI_JUMP,
    ANI_SPRING_TWIRL,
    ANI_SPRING_DIAGONAL,
    ANI_SKID,
    ANI_SKID_TURN,
    ANI_SPINDASH,
    ANI_PUSH,
    ANI_HURT,
    ANI_DIE,
    ANI_DROWN,
    ANI_BALANCE_1,
    ANI_BALANCE_2,
    ANI_TREMBLE,
    ANI_SPRING_CS,
    ANI_STAND_CS,
    ANI_FAN,
    ANI_VICTORY,
    ANI_OUTTA_HERE,
    ANI_HANGGIMMICK,
    ANI_HANGBACKWARDS,
    ANI_HANGPLAYER,
    ANI_HANG_MOVE,
    ANI_POLE_SWING_V,
    ANI_POLE_SWING_H,
    ANI_SHAFT_SWING,
    ANI_TURNTABLE,
    ANI_TWISTER,
    ANI_SPIRAL_RUN,
    ANI_STICK,
    ANI_PULLEY_HOLD,
    ANI_SHIMMY_IDLE,
    ANI_SHIMMY_MOVE,
    ANI_BUBBLE,
    ANI_BREATHE,
    ANI_RIDE,
    ANI_CLING,
    ANI_BUNGEE,
    ANI_TWIST_RUN,
    ANI_FLUME,
    ANI_SLOPE_SLIDE,
    ANI_TRANSFORM,
    ANI_SPRING_CS_REV,
    ANI_HANG_TWIRL,
    ANI_CONTINUE,
    ANI_CONTINUE_UP,
    ANI_CYLINDERWALKOUTER,
    ANI_CYLINDERWALKINNER,
    ANI_RAPPELBARSWING,
    ANI_RAPPELDOWN,
    ANI_HORIZONTALBARHANG,
    ANI_BARRELUP,
    ANI_BARRELMIDUP,
    ANI_BARRELMID,
    ANI_BARRELMIDDOWN,
    ANI_BARRELDOWN,
    ANI_ABILITY_0,
    ANI_ABILITY_1,
    ANI_ABILITY_2,
    ANI_ABILITY_3,
    ANI_ABILITY_4,
    ANI_ABILITY_5,
    ANI_ABILITY_6,
    ANI_ABILITY_7,
    ANI_ABILITY_8,
    ANI_ABILITY_9,
    ANI_ABILITY_10,
    ANI_ABILITY_11,
    ANI_ABILITY_12,
    ANI_ABILITY_13,

    ANI_AIR_WALK, // leftovers(?)
    ANI_SPRING_DIAG,
    ANI_HANG,

    // Sonic Ability Anim Aliases
    ANI_DROPDASH          = ANI_ABILITY_0,
    ANI_WHISTLE           = ANI_ABILITY_1,
    ANI_HALFCYLINDERSKATE = ANI_ABILITY_2,
    ANI_SPINHANDLE        = ANI_ABILITY_3,
    ANI_CLIMBING          = ANI_ABILITY_4,
    ANI_PEELOUT           = ANI_ABILITY_5,
    ANI_PEELOUTANGLED     = ANI_ABILITY_6,

    // Tails Ability Anim Aliases
    ANI_RAPPELDOWNLEGBENT = ANI_ABILITY_0,
    ANI_DASHANGLED1       = ANI_ABILITY_1,
    ANI_DASHANGLED2       = ANI_ABILITY_2,
    ANI_DASHVERTICAL      = ANI_ABILITY_3,
    ANI_CYLINDERSPIN      = ANI_ABILITY_4,
    ANI_FLY               = ANI_ABILITY_5,
    ANI_FLY_TIRED         = ANI_ABILITY_6,
    ANI_FLY_LIFT          = ANI_ABILITY_7,
    ANI_FLY_LIFT_DOWN     = ANI_ABILITY_8,
    ANI_FLY_LIFT_TIRED    = ANI_ABILITY_9,
    ANI_SWIM              = ANI_ABILITY_10,
    ANI_SWIM_TIRED        = ANI_ABILITY_11,
    ANI_SWIM_LIFT         = ANI_ABILITY_12,
    ANI_NEW_ANIMATION_83  = ANI_ABILITY_13, // lol

    // Knux Ability Anim Aliases
    ANI_LEDGE_PULL_UP = ANI_ABILITY_0,
    ANI_GLIDE         = ANI_ABILITY_1,
    ANI_GLIDE_DROP    = ANI_ABILITY_2,
    ANI_GLIDE_LAND    = ANI_ABILITY_3,
    ANI_GLIDE_SLIDE   = ANI_ABILITY_4,
    ANI_CLIMB_IDLE    = ANI_ABILITY_5,
    ANI_CLIMB_UP      = ANI_ABILITY_6,
    ANI_CLIMB_DOWN    = ANI_ABILITY_7,

    // Amy Ability Anim Aliases
    ANI_HAMMER_JUMP  = ANI_ABILITY_0,
    ANI_HAMMER_DASH  = ANI_ABILITY_1,
    ANI_HAMMER_THROW = ANI_ABILITY_2,
    ANI_HAMMER       = ANI_ABILITY_3,
};

enum ShieldAniIDs {
    SHIELDANI_BLUE = 0,
    SHIELDANI_FIRE = 1,
    SHIELDANI_FIREATTACK = 2,
    SHIELDANI_LIGHTNING = 3,
    SHIELDANI_LIGHTINGSPARK = 4,
    SHIELDANI_BUBBLEADD = 5,
    SHIELDANI_BUBBLE = 6,
    SHIELDANI_BUBBLEATTACKDADD = 7,
    SHIELDANI_BUBBLEATTACKD = 8,
    SHIELDANI_BUBBLEATTACKUPADD = 9,
    SHIELDANI_BUBBLEATTACKUP = 10,
    SHIELDANI_INSTA = 11,
};


// Others
typedef enum
{
    SUPERSTATE_NONE,
    SUPERSTATE_FADEIN,
    SUPERSTATE_SUPER,
    SUPERSTATE_FADEOUT,
    SUPERSTATE_DONE,
} SuperStates;

typedef enum {
    VIEWVAR_UINT8,
    VIEWVAR_UNKNOWN1,
    VIEWVAR_UNKNOWN2,
    VIEWVAR_UNKNOWN3,
    VIEWVAR_UNKNOWN4,
    VIEWVAR_UNKNOWN5,
    VIEWVAR_UNKNOWN6,
    VIEWVAR_BOOL,

    VIEWVAR_INVALID,
    VIEWVAR_UINT16,
    VIEWVAR_UINT32,
    VIEWVAR_INT8,
    VIEWVAR_INT16,
    VIEWVAR_INT32,
} ViewableVarTypes;

typedef enum {
    INK_NONE,
    INK_BLEND,
    INK_ALPHA,
    INK_ADD,
    INK_SUB,
    INK_TINT,
    INK_MASKED,
    INK_UNMASKED,
} InkEffects;

typedef enum { FX_NONE = 0, FX_FLIP = 1, FX_ROTATE = 2, FX_SCALE = 4 } DrawFX;

enum CharacterIDs
{
    ID_NONE     = 0 << 0,
    ID_SONIC    = 1 << 0,
    ID_TAILS    = 1 << 1,
    ID_KNUCKLES = 1 << 2,
    ID_AMY      = 1 << 3,

    ID_MIGHTY   = 1 << 4,  // ... Yes, these two are *still* here, somehow.
    ID_RAY      = 1 << 5,

    ID_TAILS_ASSIST    = ID_TAILS << 8,
    ID_KNUCKLES_ASSIST = ID_KNUCKLES << 8, // custom-added, can be used to check if "& knux" is active
    ID_DEFAULT_PLAYER  = ID_SONIC | ID_TAILS_ASSIST,
    ID_AMY_TAILS = ID_AMY | ID_TAILS_ASSIST,
};

enum GameModes
{
    MODE_MANIA       = 0x0,  // base game mode, used for both No Save and Saves.
    MODE_ENCORE      = 0x1,  // Encore Mode, completely unused, gameplay features half-functioning.
    MODE_TIMEATTACK  = 0x2,  // Time Attack mode, probably used? who knows.
    MODE_COMPETITION = 0x3,  // Competition mode. used for... well, S3's Competition Mode.
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

// Mission
enum MissionFunctions {
    MISSIONNO_NONE            = 0,
    MISSIONNO_MERCY           = 8,
    MISSIONNO_RINGCHALLENGE50 = 10,
    MISSIONNO_AERIALATTACK    = 18,
    MISSIONNO_RINGVACCUM      = 30,
    MISSIONNO_BALLOONBURST    = 38,
    MISSIONNO_FIREBALLDASH    = 39,
};

enum HUDEnableTypes {
    HUDENABLE_OFF,
    HUDENABLE_ON,
};

enum MissionConditions {
    MISSION_CONDITION_NONE,
    MISSION_CONDITION_CLEAR,
    MISSION_CONDITION_FAIL,
};


// RSDKv5 Structures

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

struct Animator
{
    SpriteFrame* frames;
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

struct Object {
    uint16 classID;
    uint8 active;
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


struct EntityBase : Entity {
    void *data[0x100];
    void *unknown;
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

struct ObjectPlayer : Object
{
    int32 sonicPhysicsTable[64];
    int32 sonicPhysicsTableCompetition[24];
    int32 tailsPhysicsTable[64];
    int32 tailsPhysicsTableCompetition[24];
    int32 knuxPhysicsTable[64];
    int32 knuxPhysicsTableCompetition[24];
    int32 mightyPhysicsTable[64];
    int32 rayPhysicsTable[64];
    int32 amyPhysicsTable[64];
    int32 amyPhysicsTableCompetition[24];
    int32 unknown0784[24];
    color superPalette_Sonic[30];
    color superPalette_Sonic_Water1[30];
    color superPalette_Sonic_Water2[30];
    color hyperPalette_Sonic[54];
    color hyperPalette_Sonic_Water1[54];
    color hyperPalette_Sonic_Water2[54];
    color *activeSuperPalette_Sonic;
    color *activeSuperPalette_Sonic_Water;
    color *activeHyperPalette_Sonic;
    color *activeHyperPalette_Sonic_Water;
    color superPalette_Tails[18];
    color superPalette_Tails_Water1[18];
    color superPalette_Tails_Water2[18];
    color *activeSuperPalette_Tails;
    color *activeSuperPalette_Tails_Water;
    color superPalette_Knux[33];
    color superPalette_Knux_Water1[33];
    color superPalette_Knux_Water2[33];
    color *activeSuperPalette_Knux;
    color *activeSuperPalette_Knux_Water;
    color superPalette_Amy[55];
    color superPalette_Amy_Water1[55];
    color superPalette_Amy_Water2[55];
    color *activeSuperPalette_Amy;
    color *activeSuperPalette_Amy_Water;

    float spindashChargeSpeeds[13];

    Hitbox instaShieldHitbox;
    Hitbox shieldHitbox;

    bool32 cantSwap;
    int32 playerCount;
    int32 maxPlayerCount;

    uint16 upState;
    uint16 downState;
    uint16 leftState;
    uint16 rightState;
    uint16 jumpPressState;
    uint16 jumpHoldState;
    int32 nextLeaderPosID;
    int32 lastLeaderPosID;
    Vector2 leaderPositionBuffer[16];
    Vector2 targetLeaderPosition;
    int32 autoJumpTimer;
    int32 respawnTimer;
    int32 aiInputSwapTimer;
    bool32 disableP2KeyCheck;
    int32 rings;
    int32 ringExtraLife;
    int32 powerups;
    int32 savedLives;
    int32 savedScore;
    int32 savedScore1UP;

    uint16 sonicFrames;
    uint16 superFrames;
    uint16 tailsFrames;
    uint16 tailSpriteFrames;
    uint16 knuxFrames;
    uint16 amyFrames;
    uint16 unkFrames;
    uint16 unkFrames2;

    uint16 sfxJump;
    uint16 sfxRoll;
    uint16 sfxCharge;
    uint16 sfxRelease;
    uint16 sfxPeelCharge;
    uint16 sfxPeelRelease;
    uint16 sfxDropdash;
    uint16 sfxLoseRings;
    uint16 sfxHurt;
    uint16 sfxPimPom;
    uint16 sfxSkidding;
    uint16 sfxGrab;
    uint16 sfxFlying;
    bool32 playingFlySfx;
    uint16 sfxTired;
    bool32 playingTiredSfx;
    uint16 sfxLand;
    uint16 sfxSlide;
    uint16 sfxOuttahere;
    uint16 sfxTransform2;
    uint16 sfxSwap;
    uint16 sfxSwapFail;
    uint16 sfxMightyDeflect;
    uint16 sfxMightyDrill;
    uint16 sfxMightyLand;
    uint16 sfxMightyUnspin;
    uint16 sfxEarthquake;
    uint16 sfxHammerJump;
    uint16 sfxHammerDash;
    uint16 sfxHammerHit;
    int32 raySwoopTimer;
    int32 rayDiveTimer;
    bool32 gotHit[4];
    StateMachine configureGhostCB;
    bool32 (*canSuperCB)(bool32 isHUD);
    int32 superDashCooldown;
    uint16 lookUpDelay;
    uint16 lookUpDistance;
    bool32 showHitbox;
    int32 finishedInput;
    // I would like to note that this struct doesnt actually get used much in Ultrafix, but its here for documentation.
};

struct EntityPlayer : Entity
{
    char PathTracerData[110];
    int32 rings;
    int32 lives;
    int32 score;
    int32 ringExtraLife;
    int32 score1UP;
    bool32 hyperRing;
    uint8 shield;
    StateMachine state;
    StateMachine nextAirState;
    StateMachine nextGroundState;
    StateMachine stateGravity;
    EntityCamera* camera;
    Animator animator;
    Animator tailAnimator;
    int32 minJogVelocity;
    int32 minRunVelocity;
    int32 minDashVelocity;
    int32 unused;
    int32 tailRotation;
    int32 tailDirection;
    uint16 aniFrames;           // animation frames.
    uint16 tailFrames;
    uint16 animationReserve;
    uint16 playerID;
    Hitbox *outerbox;
    Hitbox *innerbox;          // hitbox data.
    int32 characterID;
    int32 hurtAirborneTimer;    // I don't actually know what this is. it counts up when you are hit until you land, as well as hold up and down to move the camera.
    int32 timer;                // where player timer ACTUALLY is. this one is responsible for many things, such as: player idle animation.
    int32 abilityTimer;         // handles ability timers for certain things, spindash being one of them.
    int32 spindashCharge;       // handles the pitch of the spindash. note: it never goes down. why? i dunno.
    int32 abilityValue;
    int32 drownTimer;
    int32 invincibleTimer;
    int32 speedShoesTimer;
    int32 blinkTimer;
    int32 scrollDelay;
    int32 skidding;
    int32 pushing;
    int32 underwater;
    bool32 groundedStore;
    bool32 invertGravity;       // technically not used, but it still has effects on the player's control if forced to 1 on cheat engine. comes from Mania's FBZ invert gravity sections?
    bool32 isChibi;             // This is still in Sonic 3. Wild.
    bool32 isTransforming;
    bool32 disableGravity;
    int32 superState;
    int32 superRingLossTimer;
    int32 superBlendAmount;
    int32 superBlendState;
    int32 superBlendTimer;
    uint8 superColorIndex;
    uint8 superColorCount;
    uint8 superTableIndex;
    bool32 sidekick;
    int32 scoreBonus;
    int32 jumpOffset;
    int32 collisionFlagH;
    int32 collisionFlagV;
    int32 topSpeed;
    int32 acceleration;
    int32 deceleration;
    int32 airAcceleration;
    int32 airDeceleration;
    int32 skidSpeed;
    int32 rollingFriction;
    int32 rollingDeceleration;
    int32 gravityStrength;
    int32 abilitySpeed;
    int32 jumpStrength;
    int32 jumpCap;
    uint32 flailing;             // bitfield for flailing. 
    int32 sensorX[5];
    int32 sensorY;
    Vector2 moveLayerPosition;
    Vector2 lastMoveLayerPosition;
    StateMachine stateInputReplay;
    StateMachine stateInput;
    int32 controllerID;
    int32 controlLock;
    bool32 up;
    bool32 down;
    bool32 left;
    bool32 right;
    bool32 jumpPress;
    bool32 jumpHold;
    bool32 applyJumpCap;
    int32 jumpAbilityState;
    StateMachine stateAbility;
    StateMachine statePeelout;
    int32 flyCarryTimer;
    Vector2 flyCarrySidekickPos;
    Vector2 flyCarryLeaderPos;
    uint8 deathType;
    bool32 forceRespawn;            // Mania Leftover, encore respawn.
    bool32 isGhost;                 // ??? Mania time trial ghost LOL
    int32 abilityValues[8];
    void *abilityPtrs[8];
    int32 uncurlTimer;
    int32 spriteType;
    bool32 disableGroundAnims;
    StateMachine storedStateInput;
    Vector2 spikeDir;
    int32 totalKillCount;           // Kill counts are used for achievements
    int32 totalSpindashKillCount;
    int32 totalCycloneKillCount;    // In Origins this is likely referred as Rhinobot.
    int32 statsUnused0;
    int32 statsUnused1;
    int32 hyperAbilityState;
    bool32 isHyper;
    bool32 disableTileCollisions;
    char unknown3[4];               // there's something here, i dont know what it is.
    int32 chargeTimer;
    int32 enableAbilityCharge;
};

struct ObjectZone : Object
{
    int32 actID;
    StateMachine(stageFinishCallback);
    int32 shouldRecoverPlayers; // a little misleading, forces the player on-screen before an act transition if enabled
    char unknowndata[292]; // statemachine and vsSwap variables. we dont need those here, though.
    int32 listPos;
    int32 prevListPos;
    int32 ringFrame;
    int32 timer;
    int32 cameraBoundsL[4];
    int32 cameraBoundsR[4];
    int32 cameraBoundsT[4];
    int32 cameraBoundsB[4];
    int32 playerBoundsL[4];
    int32 playerBoundsR[4];
    int32 playerBoundsT[4];
    int32 playerBoundsB[4];
    int32 deathBoundary[4];
    int32 playerBoundActiveL[4];
    int32 playerBoundActiveR[4];
    int32 playerBoundActiveT[4];
    int32 playerBoundActiveB[4];
    int32 autoScrollSpeed;
};

struct ObjectStarPost : Object 
{
    Hitbox starpostHitbox;
    bool32 hasAchievement;
    Vector2 playerPositions[4];
    uint8 playerDirections[4];
    uint16 postIDs[4];
    uint8 storedMinutes;
    uint8 storedSeconds;
    uint8 storedMS;
    uint8 interactablePlayers;
    uint16 aniFrames;
    uint16 sfxStarPost;
    uint16 sfxWarp;
};

struct EntityStarPost : Entity
{
    StateMachine state;
    int32 id;
    bool32 vsRemove;
    int32 ballSpeed;
    int32 timer;
    int32 starTimer;
    int32 bonusStageID;
    int32 starAngleX;
    int32 starAngleY;
    int32 starRadius;
    Vector2 ballPos;
    Animator poleAnimator;
    Animator ballAnimator;
    Animator starAnimator;
    Hitbox hitboxStars;
    uint8 interactedPlayers;
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
    uint8 filter;
    uint8 milliseconds;
    uint8 seconds;
    uint8 minutes;
};

// Object Class
struct ObjectActClear : Object {
    uint16 aniFrames;
    uint16 sfxScoreAdd;
    uint16 sfxScoreTotal;
    uint16 sfxEvent;
    uint8 bufferMoveEnabled;
    uint8 isSavingGame;
    uint8 disableResultsInput;
    int32 displayedActID;
    uint8 finished;
    uint8 forceNoSave;
    StateMachine bufferMove_CB;
    StateMachine saveReplay_CB;
    uint8 hasSavedReplay;
    uint8 disableTimeBonus;
    uint8 actClearActive;
    bool32 disableJingle;
    bool32 disableVictory;
    int32 victoryTimer;
};

struct EntityActClear : Entity
{
    StateMachine state;
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

struct ObjectShield : Object 
{
    uint16 aniFrames;
    uint16 sfxBlueShield;
    uint16 sfxBubbleShield;
    uint16 sfxFireShield;
    uint16 sfxLightningShield;
    uint16 sfxInstaShield;
    uint16 sfxBubbleBounce;
    uint16 sfxFireDash;
    uint16 sfxLightningJump;
};


struct EntityShield : Entity
{
    EntityPlayer* player;
    StateMachine state;
    int32 type;
    int32 timer;
    int32 frameFlags;
    bool32 forceVisible;
    Animator shieldAnimator;
    Animator fxAnimator;
};

struct ObjectSpecialClear : Object 
{
    uint16 aniFrames;
    uint16 sfxScoreAdd;
    uint16 sfxScoreTotal;
    uint16 sfxEvent;
    uint16 sfxContinue;
    uint16 sfxTwinkle;
    uint16 sfxSuperEmerald;
    uint8 data_12[2];
    int32 continueStatus; // 0 - no continue, 1 - continue wait, 2 - continue complete
    uint8 data_18[8];
    int32 activationType;
};

struct EntitySpecialClear : Entity
{
    int8 type;
    uint8 gap60[7];
    StateMachine state;
    Animator animator;
    Vector2 position_98;
    int32 timer;
    int32 test;
};

struct ObjectSignPost2 : Object 
{
    uint16 aniFrames;
};

struct EntitySignPost2 : Entity
{
    StateMachine state;
    int32 type;
    Vector2 vsBoundsSize;
    Vector2 vsBoundsOffset;
    int32 vsExtendTop;
    int32 vsExtendBottom;
    int32 timer;
    int32 gravityStrength;
    int32 spinCount;
    int32 spinSpeed;
    int32 maxAngle;
    int32 sparkleType;
    int32 itemBounceCount;
    Vector2 playerPosStore[4];
    Animator eggPlateAnimator;
    Animator facePlateAnimator;
    Animator sidebarAnimator;
    Animator postTopAnimator;
    Animator standAnimator;
    uint8 activePlayers;
    bool32 spawnedByDebugMode;
    bool32 restorePlayerInteraction;
};

enum SignPostAnimationIDs {
    ANI_SONIC,
    ANI_TAILS,
    ANI_KNUX,
    ANI_MIGHTY,
    ANI_RAY,
    ANI_EGGMAN,
    ANI_POST,
    ANI_SPIN,
    ANI_AMY,
    ANI_AMY_SPIN,
};

enum SignPostTypes {
    TypeRunPast,
    TypeDrop,
    TypeCompetition,
    TypeDecoration,
};

struct ObjectDebugMode : Object 
{
    // Never used, only set, prolly leftover from S1/S2
    int16 classIDs[0x100];
    StateMachine(draw[0x100]);
    StateMachine(spawn[0x100]);
    Animator animator;
    int32 itemID;
    int32 itemCount;
    bool32 debugActive;
    uint8 itemType;
    uint8 itemTypeCount;
    int32 unused1; // no clue, though it could be "exitTimer" assuming this was based on v4's debugMode object?
};

struct EntityDebugMode : Entity
{
};

struct EntityHUD : Entity
{
    StateMachine state;
    Vector2 scorePos;
    Vector2 timePos;
    Vector2 ringsPos;
    Vector2 lifePos;
    int32 lifeFrameIDs[4];
    int32 lives[4];
    int32 targetPos;
    int32 actionPromptPos;
    StateMachine vsStates[4];
    Vector2 vsScorePos[4];
    Vector2 vsTimePos[4];
    Vector2 vsRingsPos[4];
    Vector2 vsLifePos[4];
    int32 vsTargetPos[4];
    int32 timeFlashFrame;
    int32 ringFlashFrame;
    bool32 enableTimeFlash;
    bool32 enableRingFlash;
    Animator hudElementsAnimator;
    Animator numbersAnimator;
    Animator hyperNumbersAnimator;
    Animator lifeIconAnimator;
    Animator lifeNumbersAnimator;
    Animator lifeNamesAnimator;
    Animator playerIDAnimator;
    Animator superIconAnimator;
    Animator superButtonAnimator;
    Animator thumbsUpIconAnimator;
    Animator thumbsUpButtonAnimator;
    Animator replayClapAnimator;
    Animator saveReplayButtonAnimator;
    uint8 extendedHUD;
};

enum HUDOffsetTypes {
    HUDOFF_SCORE,
    HUDOFF_TIME,
    HUDOFF_RINGS,
    HUDOFF_LIFE,
};

struct ObjectHUD : Object {
    uint16 aniFrames;
    uint16 superButtonFrames;
    uint16 sfxClick;
    uint16 sfxStarpost;
    bool32 showTAPrompt;
    bool32 replaySaveEnabled;
    int32 screenBorderType[4];
    int32 swapCooldown;
    int32 stockFlashTimers[4];
};

struct EntitySlotHUD : Entity
{
    StateMachine state;
    Vector2 ringsPos;
    Vector2 lifePos;
    int32 lifeFrameIDs[4];
    int32 lives[4];
    int32 field_a8;
    int32 field_ac;
    int32 ringFlashFrame;
    bool32 enableRingFlash;
    Animator hudElementsAnimator;
    Animator numbersAnimator;
    Animator hyperNumbersAnimator;
    Animator lifeIconAnimator;
    Animator lifeNumbersAnimator;
    Animator lifeNamesAnimator;
    Animator playerIDAnimator;
    Animator superIconAnimator;
    Animator superButtonAnimator;
    Animator thumbsUpIconAnimator;
    Animator thumbsUpButtonAnimator;
    Animator replayClapAnimator;
    Animator saveReplayButtonAnimator;
    uint8 extendedHUD;
};

struct ObjectSlotHUD : Object {
    uint16 aniFrames;
    uint16 superButtonFrames; // unused
    uint16 sfxClick;
    uint16 sfxStarpost;
    bool32 showTAPrompt;
    bool32 replaySaveEnabled;
    int32 screenBorderType[4];
    int32 swapCooldown;
    int32 stockFlashTimers[4];
};

struct EntityS1SS_Player : Entity {
    int32 rings;
    int32 lives;
    int32 score;
    int32 ringExtraLife;
    int32 score1UP;
    bool32 hyperRing;
    uint8 shield;
    uint16 playerID;
    int32 field_7c;
    StateMachine state;
    EntityCamera *camera;
    Animator animator;
    Animator tailAnimator;
    int32 tailRotation;
    int32 tailDirection;
    uint16 aniFrames;
    uint16 tailFrames;
    int32 characterID;
    uint8 gapE8[16];
    StateMachine stateInput;
    int32 controllerID;
    int32 controlLock;
    bool32 up;
    bool32 down;
    bool32 left;
    bool32 right;
    bool32 jumpPress;
    bool32 jumpHold;
    bool32 applyJumpCap;
    int32 gravity;
    Vector2 worldVelocity;
    Vector2 moveVelocity;
    int32 moveSpeed;
    bool32 isGhost;
    int32 abilityValues[8];
    void *abilityPtrs[8];
    int32 collisionFlags;
    uint8 padding2[12];
    int32 uncurlTimer;
    int32 spriteType;
    int32 rotateBlockTimer;
    int32 upDownBlockTimer;
    uint16 animationSpeed;
    int32 field_1cc;
};

struct ObjectS1SS_Player : Object 
{
    uint16 aniFrames;
    uint16 tailSpriteFrames;
    uint8 gap8[28];
    int32 rings;
    int32 ringExtraLife;
    uint8 gap2C[4];
    int32 savedLives;
    int32 savedScore;
    int32 savedScore1UP;
};

struct EntityScoreBonus : Entity
{
    int32 timer;
    Animator animator;
};

struct ObjectScoreBonus : Object 
{
    uint16 aniFrames;
};

typedef enum {
    WATER_WATERLEVEL,
    WATER_POOL,
    WATER_BUBBLER,
    WATER_HEIGHT_TRIGGER,
    WATER_BIG_BUBBLER,
    WATER_BTN_BIG_BUBBLE,
    WATER_SPLASH,
    WATER_BUBBLE,
    WATER_COUNTDOWN,
} WaterTypes;

// Object Class
struct ObjectWater : Object {
    int32 waterLevel;
    int32 newWaterLevel;
    int32 targetWaterLevel;
    int32 waterMoveSpeed;
    int32 constBubbleTimer[4];
    int32 randBubbleTimer[4];
    int32 unused1[4]; // unused but set to 0 on bubble timer sets. maybe an old bubble/drowning timer?
    TABLE(int32 bubbleSizes[18], { 2, 4, 2, 2, 2, 2, 4, 2, 4, 2, 2, 4, 2, 4, 2, 2, 4, 2 });
    uint16 aniFrames;
    uint16 bigBubbleFrames;
    uint16 wakeFrames;
    Hitbox hitboxPlayerBubble;
    Hitbox hitboxPoint;
    uint16 sfxSplash;
    uint16 sfxBreathe;
    uint16 sfxWarning;
    uint16 sfxDrownAlert;
    uint16 sfxDrown;
    uint16 sfxSkim;
    uint16 sfxDNAGrab;
    uint16 sfxDNABurst;
    uint16 sfxWaterLevelL;
    uint16 sfxWaterLevelR;
    int32 waterLevelChannelL;
    int32 waterLevelChannelR;
    int32 unused2;
    bool32 playingWaterLevelSfx;
    bool32 moveWaterLevel;
    int32 waterLevelVolume;
    int32 waterPalette;
    bool32 disableWaterSplash; // this is never set except for once and it's used in if ! statements to link the player or not
    int32 wakePosX[4];
    uint8 wakeDir[4];
    Animator wakeAnimator;
    int32 unused3;
    bool32 playingSkimSfx;
};

// Entity Class
struct EntityButton : Entity {
    int32 type;
    bool32 walkOnto;
    uint8 tag;
    bool32 down;
    bool32 toggled;
    bool32 currentlyActive;
    bool32 activated;
    bool32 wasActivated;
    int32 pressPos;
    Hitbox hitboxButton;
    Animator buttonAnimator;
    Animator baseAnimator;
};

struct EntityWater : Entity {
    StateMachine state;
    StateMachine stateDraw;
    int32 type;
    void *childPtr;
    int32 bubbleX;
    int8 bubbleType1;
    uint8 dudsRemaining;
    uint8 bubbleFlags;
    int8 bubbleType2;
    int32 numDuds;
    int32 countdownID;
    bool32 playerInBubble;
    Vector2 size;
    Vector2 height;
    int32 speed;
    int32 buttonTag;
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 priority;
    bool32 destroyOnTrigger;
    Hitbox hitbox;
    bool32 isBigBubble;
    int32 bigBubbleTimer;
    int32 timer;
    uint8 activePlayers;
    uint8 releasedPlayers;
    Vector2 bubbleOffset;
    Vector2 bubbleVelocity;
    EntityButton *taggedButton;
    Animator animator;
};

struct EntityFinalFireball : Entity
{
    StateMachine state;
    Animator animator;
    int32 field_98;
};

struct ObjectFinalFireball : Object
{
    uint16 aniFrames;
};

struct ObjectS3K_BS_SlotSetup : Object
{
    uint16 sfxContinue;
    int32 unknown_8;
    uint32 stageRotation;
    int32 rotateSpeed;
    uint8 rotateDirection;
    int32 layerTimer[9];
    int32 unknown_48;
    int32 unknown_52;
};

struct EntityBalloon : Entity
{
    Animator animator;
    int32 startY;
    uint8 color;
    bool32 bubbles;
    uint8 popped;
};

struct ObjectBalloon : Object 
{
    uint16 aniFrames;
    Hitbox hitbox;
    uint16 sfxPop;
    uint16 sfxBumper;
    uint8 unknown_0x12;
    uint8 unknown_0x13;
    uint8 unknown_0x14;
    uint8 unknown_0x15;
};

struct EntityCircleBumper : Entity
{
    StateMachine stateCollide;
    StateMachine stateMove;
    int32 hitCount;
    int32 type;
    Vector2 amplitude;
    int32 speed;
    Vector2 drawPos;
    Animator animator;
    int32 unused;
};

struct ObjectCircleBumper : Object 
{
    Hitbox hitboxBumper;
    uint16 aniFrames;
    uint16 sfxBumper;
};

struct ObjectRing : Object {
    Hitbox hitbox;
    int32 pan;
    uint16 aniFrames;
    uint16 sfxRing;
};

enum RockPileTypes {
    TypePillarV,
    TypePillarH,
    TypePile,
    TypeWall,
    TypeFloor,
};

struct EntityRockPile : Entity {
    uint8 type;
    uint16 delay;
    bool32 unbreakable;
    int32 drawGroupID;
    bool32 onlyKnux;
    bool32 onlyMighty;
    bool32 respawn;
    bool32 forceFall;
    bool32 deactivated;
    int32 timer;
    Vector2 originPos;
    char bytepadding2[4];
    Animator animator;
    int32 field_b0;
    Hitbox hitbox;
    char bytepadding3[4];
    StateMachine state;
    int8 debrisDrawGroup;
};

struct ObjectRockPile : Object {

};


struct ObjectS3K_AIZBombing : Object {
    uint16 aniFrames;
};

struct ObjectFlingRamp : Object {
    Hitbox rampHitbox;
};

struct EntityFlingRamp : Entity {
};

struct ObjectPuff : Object {
    Hitbox hitboxBadnik;
    uint16 aniFrames;
};

struct EntityPuff : Entity {
    StateMachine state;
    int32 timer;
    Vector2 startPos;
    uint8 startDir;
    Animator animator;
};


typedef struct {
    uint32 realRotation;
} StockValues;

enum RotateSpeeds {
        Speed0,
        Speed1,
        SpeedMax,
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
    BOOT_PLAYMODE_CLASSIC = 0,
    BOOT_PLAYMODE_ANNIVERSARY = 1,
    BOOT_PLAYMODE_BOSSRUSH = 2,
    BOOT_PLAYMODE_MIRRORING = 3,
    BOOT_PLAYMODE_MISSION = 4,
    BOOT_PLAYMODE_STORY = 5,
} PlayModes;

typedef enum {
    MEDAL_DEBUGMODE   = 1 << 0,
    MEDAL_ANDKNUCKLES = 1 << 1,
    MEDAL_PEELOUT     = 1 << 2,
    MEDAL_INSTASHIELD = 1 << 3,
    MEDAL_NODROPDASH  = 1 << 4,
    MEDAL_NOTIMEOVER  = 1 << 5,
    MEDAL_NOLIVES     = 1 << 6,
} MedalMods;

typedef enum {
    SECRET_RICKYMODE        = 1 << 0,
    SECRET_SUPERDASH        = 1 << 1,
    SECRET_BARRELHOTLINE    = 1 << 2, // unused in normal gameplay
    SECRET_NORMALPHYSICS_2P = 1 << 3,
    SECRET_REGIONSWAP       = 1 << 4,
    SECRET_CAMERATRACKING   = 1 << 5,
    SECRET_PENPENMODE       = 1 << 6,
    SECRET_RANDOMITEMS      = 1 << 7,
    SECRET_BLUESHIELDMODE   = 1 << 8,
    SECRET_UNKNOWN          = 1 << 9, // unused in normal gameplay
    SECRET_BANANAMODE       = 1 << 10,
    SECRET_NOITEMS          = 1 << 11,
    SECRET_HIDDENCREDITS    = 1 << 12,
} GameCheats;

typedef enum {
    SHIELD_NONE,
    SHIELD_BLUE,
    SHIELD_BUBBLE,
    SHIELD_FIRE,
    SHIELD_LIGHTNING,
    SHIELD_UNK,
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

typedef enum {
    PRINT_NORMAL,
    PRINT_POPUP,
    PRINT_ERROR,
    PRINT_FATAL,
} PrintModes;

enum RetroEngineCallbacks : int32 {
    CALLBACK_DISPLAYLOGOS = 0,
    CALLBACK_PRESS_START = 1,
    CALLBACK_TIMEATTACK_NOTIFY_ENTER = 2,
    CALLBACK_TIMEATTACK_NOTIFY_EXIT = 3,
    CALLBACK_FINISHGAME_NOTIFY = 4,
    CALLBACK_RETURNSTORE_SELECTED = 5,
    CALLBACK_RESTART_SELECTED = 6,
    CALLBACK_EXIT_SELECTED = 7,
    CALLBACK_BUY_FULL_GAME_SELECTED = 8,
    CALLBACK_TERMS_SELECTED = 9,
    CALLBACK_PRIVACY_SELECTED = 10,
    CALLBACK_TRIAL_ENDED = 11,
    CALLBACK_SETTINGS_SELECTED = 12,
    CALLBACK_PAUSE_REQUESTED = 13,
    CALLBACK_FULL_VERSION_ONLY = 14,
    CALLBACK_STAFF_CREDITS = 15,
    CALLBACK_MOREGAMES = 16,
    CALLBACK_SHOWREMOVEADS = 20,
    CALLBACK_AGEGATE = 100,

    // Sonic Origins Notify Callbacks
    NOTIFY_DEATH_EVENT = 128,
    NOTIFY_TOUCH_SIGNPOST = 129,
    NOTIFY_HUD_ENABLE = 130,
    NOTIFY_ADD_COIN = 131,
    NOTIFY_KILL_ENEMY = 132,
    NOTIFY_SAVESLOT_SELECT = 133,
    NOTIFY_FUTURE_PAST = 134,
    NOTIFY_GOTO_FUTURE_PAST = 135,
    NOTIFY_BOSS_END = 136,
    NOTIFY_SPECIAL_END = 137,
    NOTIFY_DEBUGPRINT = 138,
    NOTIFY_KILL_BOSS = 139,
    NOTIFY_TOUCH_EMERALD = 140,
    NOTIFY_STATS_ENEMY = 141,
    NOTIFY_STATS_CHARA_ACTION = 142,
    NOTIFY_STATS_RING = 143,
    NOTIFY_STATS_MOVIE = 144,
    NOTIFY_STATS_PARAM_1 = 145,
    NOTIFY_STATS_PARAM_2 = 146,
    NOTIFY_CHARACTER_SELECT = 147,
    NOTIFY_SPECIAL_RETRY = 148,
    NOTIFY_TOUCH_CHECKPOINT = 149,
    NOTIFY_ACT_FINISH = 150,
    NOTIFY_1P_VS_SELECT = 151,
    NOTIFY_CONTROLLER_SUPPORT = 152,
    NOTIFY_STAGE_RETRY = 153,
    NOTIFY_SOUND_TRACK = 154,
    NOTIFY_GOOD_ENDING = 155,
    NOTIFY_BACK_TO_MAINMENU = 156,
    NOTIFY_LEVEL_SELECT_MENU = 157,
    NOTIFY_PLAYER_SET = 158,
    NOTIFY_EXTRAS_MODE = 159,
    NOTIFY_SPIN_DASH_TYPE = 160,
    NOTIFY_TIME_OVER = 161,
    NOTIFY_TIMEATTACK_MODE = 162,
    NOTIFY_STATS_BREAK_OBJECT = 163,
    NOTIFY_STATS_SAVE_FUTURE = 164,
    NOTIFY_STATS_CHARA_ACTION2 = 165,

    // Sega Forever stuff
    CALLBACK_SHOWMENU_2 = 997,
    CALLBACK_SHOWHELPCENTER = 998,
    CALLBACK_CHANGEADSTYPE = 999,
    CALLBACK_NONE_1000 = 1000,
    CALLBACK_NONE_1001 = 1001,
    CALLBACK_NONE_1006 = 1002,
    CALLBACK_ONSHOWINTERSTITIAL = 1003,
    CALLBACK_ONSHOWBANNER = 1004,
    CALLBACK_ONSHOWBANNER_PAUSESTART = 1005,
    CALLBACK_ONHIDEBANNER = 1006,
    CALLBACK_REMOVEADSBUTTON_FADEOUT = 1007,
    CALLBACK_REMOVEADSBUTTON_FADEIN = 1008,
    CALLBACK_ONSHOWINTERSTITIAL_2 = 1009,
    CALLBACK_ONSHOWINTERSTITIAL_3 = 1010,
    CALLBACK_ONSHOWINTERSTITIAL_4 = 1011,
    CALLBACK_ONVISIBLEGRIDBTN_1 = 1012,
    CALLBACK_ONVISIBLEGRIDBTN_0 = 1013,
    CALLBACK_ONSHOWINTERSTITIAL_PAUSEDURATION = 1014,
    CALLBACK_SHOWCOUNTDOWNMENU = 1015,
    CALLBACK_ONVISIBLEMAINMENU_1 = 1016,
    CALLBACK_ONVISIBLEMAINMENU_0 = 1017,
    CALLBACK_ONSHOWREWARDADS = 1018,
    CALLBACK_ONSHOWBANNER_2 = 1019,
    CALLBACK_ONSHOWINTERSTITIAL_5 = 1020,

    // Custom
    CALLBACK_COPYPALETTE = 2000,
    CALLBACK_SET1P = 2001,
    CALLBACK_SET2P = 2002,
};

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

typedef enum {
    GAME_S1 = 0,
    GAME_CD = 1,
    GAME_S2 = 2,
    GAME_SM = 3,
    GAME_S3K = 4,
    GAME_S3 = 5,
    GAME_SK = 6,
} GameTypes;

typedef enum { FLIP_NONE, FLIP_X, FLIP_Y, FLIP_XY } FlipFlags;

struct GlobalVariables
{
    GameModes gameMode;
    CharacterIDs playerID;
    int32 specialCleared;
    int32 specialRingID;
    int32 blueSpheresID;
    int32 blueSpheresInit;
    int32 atlEnabled;
    int32 atlEntityCount;
    int32 atlEntitySlot[288];
    void* atlEntityData[288][276];
    int32 saveLoaded;
    int32 saveRAM[7168];
    int32 saveSlotID;
    int32 noSaveSlot[256];
    int32 menuParam[16384];
    int32 itemMode;
    int32 suppressTitlecard;
    int32 suppressAutoMusic;
    int32 competitionSession[16384];
    int32 medalMods;
    int32 parallaxOffset[0x100];
    int32 enableIntro;
    int32 optionsLoaded;
    int32 optionsRAM[0x80];
    int32 presenceID;
    int32 medallionDebug;
    int32 noSave;
    int32 notifiedAutosave;
    int32 recallEntities;
    int32 restartRings;
    int32 restart1UP;
    int32 restartPowerups;
    Vector2 restartPos[4];
    int32 restartSlot[4];
    int32 restartDir[4];
    int32 restartMinutes;
    int32 restartSeconds;
    int32 restartMilliseconds;
    int32 restartScore;
    int32 restartScore1UP;
    int32 restartLives[4];
    int32 restartMusicID;
    bool32 restartFlags;
    int32 field_47B4C;
    int32 showExtendedTimeHUD;
    int32 overrideRestart;
    int32 overrideUnknown;
    Vector2 overrideRestartPos[4];
    int32 overrideRestartSlot[4];
    int32 overrideRestartDir[4];
    int32 overrideRestartMinutes;
    int32 overrideRestartSeconds;
    int32 overrideRestartMilliseconds;
    int32 tempMinutes;
    int32 tempSeconds;
    int32 tempMilliseconds;
    int32 persistentTimer;
    int32 tempFlags;
    int32 continues;
    bool32 initCoolBonus;
    int32 coolBonus[4];
    int32 replayWriteBuffer[0x40000];
    int32 replayReadBuffer[0x40000];
    int32 replayTempWBuffer[0x40000];
    int32 replayTempRBuffer[0x40000];
    int32 replayTable;
    int32 replayTableLoaded;
    int32 taTable;
    int32 taTableLoaded;
    int32 stock;
    int32 characterFlags;
    bool32 vapeMode;
    int32 secrets;
    int32 titleIntroThanks;
    bool32 soundTestEnabled;
    bool32 superMusicEnabled;
    GameTypes playerSpriteStyle;
    GameTypes gameSpriteStyle;
    GameTypes ostStyle;
    GameTypes starpostStyle;
    bool32 stageFinished;
    int32 field_447C14;
    int32 storedOverrideUnknown;
    int32 atlCameraBoundsL[4];
    int32 atlCameraBoundsR[4];
    int32 atlCameraBoundsT[4];
    int32 atlCameraBoundsB[4];
    Vector2 atlCameraPos[4];
    Vector2 atlOffset;
    int32 unknownValues[4];
    int32 unknownValues2[4];
    int32 field_447CA4;
    bool32 tileCollisionMode;
    uint8 gravityDir;
    uint8 blueSpheresSeed[4];
    bool32 blueSpheresHasPerfect;
    int32 field_447CB8; // blueSpheresLevel?
    int32 field_447CBC;
    int32 field_447CC0;
    int32 field_447CC4;
    int32 field_447CC8;
    uint8 field_447CCC;
    int32 field_447CD0;    // thingy here?
    bool32 disableLives;
    bool32 mirrorMode;
    bool32 useManiaBehavior;
    int32 coinCount;
    bool32 showHUD;
    bool32 somethingRelatedToLives;
    uint8 gap4C34DC[0x20];
    bool32 hasPlusDLC;
    int32 playMode;
    int32 callbackParam0;
    int32 callbackParam1;
    int32 callbackParam2;
    int32 callbackParam3;
    bool32 hudEnable;
    bool32 useCoins;
	bool32 forceKillPlayer;
    int32 missionCondition;
    int32 missionFunctionNo;
    int32 missionValue;
    int32 missionEnd;
    int32 continueFlag;
    bool32 skipSaveSelect;
    int32 unkn_447D40;
    int32 unkn_447D44;
    int32 unkn_447D48;
    int32 callbackResult;
    bool32 skipTitleIntro;
    bool32 allowRetries;
    bool32 clearBlueSpheres;
    int32 unkn_4C3564;
    int32 unkn_4C3568;
    int32 unkn_4C356C;
    int32 unkn_4C3570;
    int32 unkn_4C3574;
    int32 statsParam7;
    int32 statsParam8;
    int32 statsParam2;
    int32 unkn_4C3584;
    int32 unkn_4C3588;
    bool32 waitSSRetry;
    int32 unknown_new_4C3590;
    int32 unknown_new_4C3594;
    int32 unknown_new_4C3598;
    int32 unknown_new_4C359C;
    int32 unknown_new_4C35A0;
    int32 unknown_new_4C35A4;
};


struct ScreenInfo
{
    uint16 frameBuffer[307200];
    Vector2 position;
    Vector2 size;
    Vector2 center;
    int32 pitch;
    int32 clipBound_X1;
    int32 clipBound_Y1;
    int32 clipBound_X2;
    int32 clipBound_Y2;
    int32 waterDrawPos;
};

struct FunctionTable
{
    // Registration
#if RETRO_REV0U
    void (*RegisterGlobalVariables)(void** globals, int32 size, void (*initCB)(void* globals));
    void (*RegisterObject)(void** staticVars, const char* name, uint32 entityClassSize, uint32 staticClassSize, void (*update)(void),
        void (*lateUpdate)(void), void (*staticUpdate)(void), void (*draw)(void), void (*create)(void*), void (*stageLoad)(void),
        void (*editorDraw)(void), void (*editorLoad)(void), void (*serialize)(void), void (*staticLoad)(void* staticVars));
#else
    void (*RegisterGlobalVariables)(void** globals, int32 size);
    void (*RegisterObject)(void** staticVars, const char* name, uint32 entityClassSize, uint32 staticClassSize, void (*update)(void),
        void (*lateUpdate)(void), void (*staticUpdate)(void), void (*draw)(void), void (*create)(void*), void (*stageLoad)(void),
        void (*editorDraw)(void), void (*editorLoad)(void), void (*serialize)(void));
#endif
#if RETRO_REV02
    void (*RegisterStaticVariables)(void** varClass, const char* name, uint32 classSize);
#endif

    // Entities & Objects
    bool32(*GetActiveEntities)(uint16 group, void** entity);
    bool32(*GetAllEntities)(uint16 classID, void** entity);
    void (*BreakForeachLoop)(void);
    void (*SetEditableVar)(uint8 type, const char* name, uint8 classID, int32 offset);
    void* (*GetEntity)(uint16 slot);
    int32(*GetEntitySlot)(void* entity);
    int32(*GetEntityCount)(uint16 classID, bool32 isActive);
    int32(*GetDrawListRefSlot)(uint8 drawGroup, uint16 listPos);
    void* (*GetDrawListRef)(uint8 drawGroup, uint16 listPos);
    void (*ResetEntity)(void* entity, uint16 classID, void* data);
    void (*ResetEntitySlot)(uint16 slot, uint16 classID, void* data);
    Entity* (*CreateEntity)(uint16 classID, void* data, int32 x, int32 y);
    void (*CopyEntity)(void* destEntity, void* srcEntity, bool32 clearSrcEntity);
    bool32(*CheckOnScreen)(void* entity, Vector2* range);
    bool32(*CheckPosOnScreen)(Vector2* position, Vector2* range);
    void (*AddDrawListRef)(uint8 drawGroup, uint16 entitySlot);
    void (*SwapDrawListEntries)(uint8 drawGroup, uint16 slot1, uint16 slot2, uint16 count);
    void (*SetDrawGroupProperties)(uint8 drawGroup, bool32 sorted, void (*hookCB)(void));

    // Scene Management
    void (*SetScene)(const char* categoryName, const char* sceneName);
    void (*SetEngineState)(uint8 state);
#if RETRO_REV02
    void (*ForceHardReset)(bool32 shouldHardReset);
#endif
    bool32(*CheckValidScene)(void);
    bool32(*CheckSceneFolder)(const char* folderName);
    void (*LoadScene)(void);
    int32(*FindObject)(const char* name);

    // Cameras
    void (*ClearCameras)(void);
    void (*AddCamera)(Vector2* targetPos, int32 offsetX, int32 offsetY, bool32 worldRelative);

    // API (Rev01 only)
#if !RETRO_REV02
    void* (*GetAPIFunction)(const char* funcName);
#endif

    // Window/Video Settings
    int32(*GetVideoSetting)(int32 id);
    void (*SetVideoSetting)(int32 id, int32 value);
    void (*UpdateWindow)(void);

    // Math
    int32(*Sin1024)(int32 angle);
    int32(*Cos1024)(int32 angle);
    int32(*Tan1024)(int32 angle);
    int32(*ASin1024)(int32 angle);
    int32(*ACos1024)(int32 angle);
    int32(*Sin512)(int32 angle);
    int32(*Cos512)(int32 angle);
    int32(*Tan512)(int32 angle);
    int32(*ASin512)(int32 angle);
    int32(*ACos512)(int32 angle);
    int32(*Sin256)(int32 angle);
    int32(*Cos256)(int32 angle);
    int32(*Tan256)(int32 angle);
    int32(*ASin256)(int32 angle);
    int32(*ACos256)(int32 angle);
    int32(*Rand)(int32 min, int32 max);
    int32(*RandSeeded)(int32 min, int32 max, int32* seed);
    void (*SetRandSeed)(int32 seed);
    uint8(*ATan2)(int32 x, int32 y);

    // Matrices
    void (*SetIdentityMatrix)(Matrix* matrix);
    void (*MatrixMultiply)(Matrix* dest, Matrix* matrixA, Matrix* matrixB);
    void (*MatrixTranslateXYZ)(Matrix* matrix, int32 x, int32 y, int32 z, bool32 setIdentity);
    void (*MatrixScaleXYZ)(Matrix* matrix, int32 x, int32 y, int32 z);
    void (*MatrixRotateX)(Matrix* matrix, int16 angle);
    void (*MatrixRotateY)(Matrix* matrix, int16 angle);
    void (*MatrixRotateZ)(Matrix* matrix, int16 angle);
    void (*MatrixRotateXYZ)(Matrix* matrix, int16 x, int16 y, int16 z);
    void (*MatrixInverse)(Matrix* dest, Matrix* matrix);
    void (*MatrixCopy)(Matrix* matDest, Matrix* matSrc);

    // Strings
    void (*InitString)(String* string, const char* text, uint32 textLength);
    void (*CopyString)(String* dst, String* src);
    void (*SetString)(String* string, const char* text);
    void (*AppendString)(String* string, String* appendString);
    void (*AppendText)(String* string, const char* appendText);
    void (*LoadStringList)(String* stringList, const char* filePath, uint32 charSize);
    bool32(*SplitStringList)(String* splitStrings, String* stringList, int32 startStringID, int32 stringCount);
    void (*GetCString)(char* destChars, String* string);
    bool32(*CompareStrings)(String* string1, String* string2, bool32 exactMatch);

    // Screens & Displays
    void (*GetDisplayInfo)(int32* displayID, int32* width, int32* height, int32* refreshRate, char* text);
    void (*GetWindowSize)(int32* width, int32* height);
    int32(*SetScreenSize)(uint8 screenID, uint16 width, uint16 height);
    void (*SetClipBounds)(uint8 screenID, int32 x1, int32 y1, int32 x2, int32 y2);
#if RETRO_REV02
    void (*SetScreenVertices)(uint8 startVert2P_S1, uint8 startVert2P_S2, uint8 startVert3P_S1, uint8 startVert3P_S2, uint8 startVert3P_S3);
#endif

    // Spritesheets
    uint16(*LoadSpriteSheet)(const char* filePath, uint8 scope);

    // Palettes & Colors
#if RETRO_REV02
    void (*SetTintLookupTable)(uint16* lookupTable);
#else
    uint16* (*GetTintLookupTable)(void);
#endif
    void (*SetPaletteMask)(color maskColor);
    void (*SetPaletteEntry)(uint8 bankID, uint8 index, uint32 color);
    color(*GetPaletteEntry)(uint8 bankID, uint8 index);
    void (*SetActivePalette)(uint8 newActiveBank, int32 startLine, int32 endLine);
    void (*CopyPalette)(uint8 sourceBank, uint8 srcBankStart, uint8 destinationBank, uint8 destBankStart, uint8 count);
#if RETRO_REV02
    void (*LoadPalette)(uint8 bankID, const char* path, uint16 disabledRows);
#endif
    void (*RotatePalette)(uint8 bankID, uint8 startIndex, uint8 endIndex, bool32 right);
    void (*SetLimitedFade)(uint8 destBankID, uint8 srcBankA, uint8 srcBankB, int16 blendAmount, int32 startIndex, int32 endIndex);
#if RETRO_REV02
    void (*BlendColors)(uint8 destBankID, color* srcColorsA, color* srcColorsB, int32 blendAmount, int32 startIndex, int32 count);
#endif

    // Drawing
    void (*DrawRect)(int32 x, int32 y, int32 width, int32 height, uint32 color, int32 alpha, int32 inkEffect, bool32 screenRelative);
    void (*DrawLine)(int32 x1, int32 y1, int32 x2, int32 y2, uint32 color, int32 alpha, int32 inkEffect, bool32 screenRelative);
    void (*DrawCircle)(int32 x, int32 y, int32 radius, uint32 color, int32 alpha, int32 inkEffect, bool32 screenRelative);
    void (*DrawCircleOutline)(int32 x, int32 y, int32 innerRadius, int32 outerRadius, uint32 color, int32 alpha, int32 inkEffect,
        bool32 screenRelative);
    void (*DrawFace)(Vector2* vertices, int32 vertCount, int32 r, int32 g, int32 b, int32 alpha, int32 inkEffect);
    void (*DrawBlendedFace)(Vector2* vertices, color* vertColors, int32 vertCount, int32 alpha, int32 inkEffect);
    void (*DrawSprite)(Animator* animator, Vector2* position, bool32 screenRelative);
    void (*DrawDeformedSprite)(uint16 sheetID, int32 inkEffect, bool32 screenRelative);
    void (*DrawText)(Animator* animator, Vector2* position, String* string, int32 endFrame, int32 textLength, int32 align, int32 spacing,
        void* unused, Vector2* charOffsets, bool32 screenRelative);
    void (*DrawTile)(uint16* tiles, int32 countX, int32 countY, Vector2* position, Vector2* offset, bool32 screenRelative);
    void (*CopyTile)(uint16 dest, uint16 src, uint16 count);
    void (*DrawAniTiles)(uint16 sheetID, uint16 tileIndex, uint16 srcX, uint16 srcY, uint16 width, uint16 height);
#if RETRO_REV0U
    void (*DrawDynamicAniTiles)(Animator* animator, uint16 tileIndex);
#endif
    void (*FillScreen)(uint32 color, int32 alphaR, int32 alphaG, int32 alphaB);

    // Meshes & 3D Scenes
    uint16(*LoadMesh)(const char* filename, uint8 scope);
    uint16(*Create3DScene)(const char* identifier, uint16 faceCount, uint8 scope);
    void (*Prepare3DScene)(uint16 sceneIndex);
    void (*SetDiffuseColor)(uint16 sceneIndex, uint8 x, uint8 y, uint8 z);
    void (*SetDiffuseIntensity)(uint16 sceneIndex, uint8 x, uint8 y, uint8 z);
    void (*SetSpecularIntensity)(uint16 sceneIndex, uint8 x, uint8 y, uint8 z);
    void (*AddModelTo3DScene)(uint16 modelFrames, uint16 sceneIndex, uint8 drawMode, Matrix* matWorld, Matrix* matView, color color);
    void (*SetModelAnimation)(uint16 modelFrames, Animator* animator, int16 speed, uint8 loopIndex, bool32 forceApply, int16 frameID);
    void (*AddMeshFrameTo3DScene)(uint16 modelFrames, uint16 sceneIndex, Animator* animator, uint8 drawMode, Matrix* matWorld, Matrix* matView,
        color color);
    void (*Draw3DScene)(uint16 sceneIndex);

    // Sprite Animations & Frames
    uint16(*LoadSpriteAnimation)(const char* filePath, uint8 scope);
    uint16(*CreateSpriteAnimation)(const char* filePath, uint32 frameCount, uint32 listCount, uint8 scope);
    void (*SetSpriteAnimation)(uint16 aniFrames, uint16 listID, Animator* animator, bool32 forceApply, int32 frameID);
    void (*EditSpriteAnimation)(uint16 aniFrames, uint16 listID, const char* name, int32 frameOffset, uint16 frameCount, int16 speed, uint8 loopIndex,
        uint8 rotationStyle);
    void (*SetSpriteString)(uint16 aniFrames, uint16 listID, String* string);
    uint16(*FindSpriteAnimation)(uint16 aniFrames, const char* name);
    SpriteFrame* (*GetFrame)(uint16 aniFrames, uint16 listID, int32 frameID);
    Hitbox* (*GetHitbox)(Animator* animator, uint8 hitboxID);
    int16(*GetFrameID)(Animator* animator);
    int32(*GetStringWidth)(uint16 aniFrames, uint16 listID, String* string, int32 startIndex, int32 length, int32 spacing);
    void (*ProcessAnimation)(Animator* animator);

    // Tile Layers
    uint16(*GetTileLayerID)(const char* name);
    void* (*GetTileLayer)(uint16 layerID);
    void (*GetLayerSize)(uint16 layer, Vector2* size, bool32 usePixelUnits);
    uint16(*GetTile)(uint16 layer, int32 x, int32 y);
    void (*SetTile)(uint16 layer, int32 x, int32 y, uint16 tile);
    void (*CopyTileLayer)(uint16 dstLayerID, int32 dstStartX, int32 dstStartY, uint16 srcLayerID, int32 srcStartX, int32 srcStartY, int32 countX,
        int32 countY);
    void (*ProcessParallax)(void* tileLayer);
    void* (*GetScanlines)(void);

    // Object & Tile Collisions
    bool32(*CheckObjectCollisionTouchBox)(void* thisEntity, Hitbox* thisHitbox, void* otherEntity, Hitbox* otherHitbox);
    bool32(*CheckObjectCollisionTouchCircle)(void* thisEntity, int32 thisRadius, void* otherEntity, int32 otherRadius);
    uint8(*CheckObjectCollisionBox)(void* thisEntity, Hitbox* thisHitbox, void* otherEntity, Hitbox* otherHitbox, bool32 setPos);
    bool32(*CheckObjectCollisionPlatform)(void* thisEntity, Hitbox* thisHitbox, void* otherEntity, Hitbox* otherHitbox, bool32 setPos);
    bool32(*ObjectTileCollision)(void* entity, uint16 collisionLayers, uint8 collisionMode, uint8 collisionPlane, int32 xOffset, int32 yOffset,
        bool32 setPos);
    bool32(*ObjectTileGrip)(void* entity, uint16 collisionLayers, uint8 collisionMode, uint8 collisionPlane, int32 xOffset, int32 yOffset,
        int32 tolerance);
    void (*ProcessObjectMovement)(void* entity, Hitbox* outer, Hitbox* inner);
#if RETRO_REV0U
    void (*SetupCollisionConfig)(int32 minDistance, uint8 lowTolerance, uint8 highTolerance, uint8 floorAngleTolerance, uint8 wallAngleTolerance,
        uint8 roofAngleTolerance);
    void (*SetPathGripSensors)(void* sensors); // expects 5 sensors
    void (*FindFloorPosition)(void* sensor);
    void (*FindLWallPosition)(void* sensor);
    void (*FindRoofPosition)(void* sensor);
    void (*FindRWallPosition)(void* sensor);
    void (*FloorCollision)(void* sensor);
    void (*LWallCollision)(void* sensor);
    void (*RoofCollision)(void* sensor);
    void (*RWallCollision)(void* sensor);
#endif
    int32(*GetTileAngle)(uint16 tile, uint8 cPlane, uint8 cMode);
    void (*SetTileAngle)(uint16 tile, uint8 cPlane, uint8 cMode, uint8 angle);
    uint8(*GetTileFlags)(uint16 tile, uint8 cPlane);
    void (*SetTileFlags)(uint16 tile, uint8 cPlane, uint8 flag);
#if RETRO_REV0U
    void (*CopyCollisionMask)(uint16 dst, uint16 src, uint8 cPlane, uint8 cMode);
    void (*GetCollisionInfo)(void** masks, void** tileInfo);
#endif

    // Audio
    uint16(*GetSfx)(const char* path);
    int32(*PlaySfx)(uint16 sfx, int32 loopPoint, int32 priority);
    void (*StopSfx)(uint16 sfx);
    int32(*PlayStream)(const char* filename, uint32 channel, uint32 startPos, uint32 loopPoint, bool32 loadASync);
    void (*SetChannelAttributes)(uint32 channel, float volume, float pan, float speed);
    void (*StopChannel)(uint32 channel);
    void (*PauseChannel)(uint32 channel);
    void (*ResumeChannel)(uint32 channel);
    bool32(*IsSfxPlaying)(uint16 sfx);
    bool32(*ChannelActive)(uint32 channel);
    uint32(*GetChannelPos)(uint32 channel);

    // Videos & "HD Images"
    bool32(*LoadVideo)(const char* filename, double startDelay, bool32(*skipCallback)(void));
    bool32(*LoadImage)(const char* filename, double displayLength, double fadeSpeed, bool32(*skipCallback)(void));

    // Input
#if RETRO_REV02
    uint32(*GetInputDeviceID)(uint8 inputSlot);
    uint32(*GetFilteredInputDeviceID)(bool32 confirmOnly, bool32 unassignedOnly, uint32 maxInactiveTimer);
    int32(*GetInputDeviceType)(uint32 deviceID);
    bool32(*IsInputDeviceAssigned)(uint32 deviceID);
    int32(*GetInputDeviceUnknown)(uint32 deviceID);
    int32(*InputDeviceUnknown1)(uint32 deviceID, int32 unknown1, int32 unknown2);
    int32(*InputDeviceUnknown2)(uint32 deviceID, int32 unknown1, int32 unknown2);
    int32(*GetInputSlotUnknown)(uint8 inputSlot);
    int32(*InputSlotUnknown1)(uint8 inputSlot, int32 unknown1, int32 unknown2);
    int32(*InputSlotUnknown2)(uint8 inputSlot, int32 unknown1, int32 unknown2);
    void (*AssignInputSlotToDevice)(uint8 inputSlot, uint32 deviceID);
    bool32(*IsInputSlotAssigned)(uint8 inputSlot);
    void (*ResetInputSlotAssignments)(void);
#endif
#if !RETRO_REV02
    void (*GetUnknownInputValue)(int32 inputSlot, int32 type, int32* value);
#endif

    // User File Management
    bool32(*LoadUserFile)(const char* fileName, void* buffer, uint32 size); // load user file from exe dir
    bool32(*SaveUserFile)(const char* fileName, void* buffer, uint32 size); // save user file to exe dir

    // Printing (Rev02)
#if RETRO_REV02
    void (*PrintLog)(int32 mode, const char* message, ...);
    void (*PrintText)(int32 mode, const char* message);
    void (*PrintString)(int32 mode, String* message);
    void (*PrintUInt32)(int32 mode, const char* message, uint32 i);
    void (*PrintInt32)(int32 mode, const char* message, int32 i);
    void (*PrintFloat)(int32 mode, const char* message, float f);
    void (*PrintVector2)(int32 mode, const char* message, Vector2 vec);
    void (*PrintHitbox)(int32 mode, const char* message, Hitbox hitbox);
#endif

    // Editor
    void (*SetActiveVariable)(int32 classID, const char* name);
    void (*AddVarEnumValue)(const char* name);

    // Printing (Rev01)
#if !RETRO_REV02
    void (*PrintMessage)(void* message, uint8 type);
#endif

    // Debugging
#if RETRO_REV02
    void (*ClearViewableVariables)(void);
    void (*AddViewableVariable)(const char* name, void* value, int32 type, int32 min, int32 max);
#endif

#if RETRO_REV0U
    // Origins Extras
    void (*NotifyCallback)(int32 callbackID, int32 param1, int32 param2, int32 param3);
    void (*SetGameFinished)(void);
    void (*StopAllSfx)(void);
#endif

};

struct ObjectSoundboard
{
    int32 sfxCount;
    uint16 sfxList[32];
    int32 sfxLoopPoint[32];
    SoundInfo (*sfxCheckCallback[32])();
    void (*sfxUpdateCallback[32])(int32 sfxID);
    bool32 sfxIsPlaying[32];
    int32 sfxChannel[32];
    int32 sfxPlayingTimer[32];
    int32 sfxFadeOutTimer[32];
    int32 sfxFadeOutDuration[32];
};

struct ObjectFXAudioPan
{
    Vector2 size;
    Vector2 sfxPos;
    bool32 sfxActive;
    Animator animator;
    uint16 sfxID;
    uint32 loopPos;
    String soundName;
};

struct NotifyCallbackInfo
{
    RetroEngineCallbacks callback;
    int32 param1;
    int32 param2;
    int32 param3;
    int32 param4;
    int32 param5;
};
