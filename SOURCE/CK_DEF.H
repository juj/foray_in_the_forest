/* "Foray in the Forest" Source Code
 * Copyright (C) 2022-2023 Nisaba
 * Copyright (C) 2019-2021 K1n9_Duk3
 *
 * The code in this file is based on:
 * Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// CK_DEF.H

#include "ID_HEADS.H"
#include <BIOS.H>

// Note to modders: Some interactions between certain "actors" in the game were
// programmed but ultimately never used in the official Foray in the Forest
// levels and therefore have been disabled to reduce the code size and make more
// memory available. You can re-enable that code by uncommenting the following
// line:

//#define EXTRA_INTERACTIONS


/*
=============================================================================

							MACROS

=============================================================================
*/


#define COLORBORDER(color)		asm{mov	dx,STATUS_REGISTER_1;in al,dx;\
	mov dx,ATR_INDEX;mov al,ATR_OVERSCAN;out dx,al;mov al,color;out	dx,al;\
	mov	al,32;out dx,al};

#define SIGN(x) 	((x)>0?1:-1)
#define ABS(x) 		((int)(x)>0?(x):-(x))
#define LABS(x) 	((long)(x)>0?(x):-(x)

// 	Redefine some functions from Omnispeak as Macros
#define TI_ForeTop(tile)	(tinf[NORTHWALL+tile])
#define TI_ForeRight(tile)	(tinf[EASTWALL+tile])
#define TI_ForeBottom(tile)	(tinf[SOUTHWALL+tile])
#define TI_ForeLeft(tile)	(tinf[WESTWALL+tile])
#define TI_ForeAnimTile(tile)   ((signed char)(tinf[MANIM+tile]))
#define TI_ForeMisc(tile)   	(tinf[INTILE+tile])

#define CA_UnmarkGrChunk(chunk)	grneeded[chunk] &= ~ca_levelbit
#define CA_UncacheGrChunk(chunk)	{CA_UnmarkGrChunk(chunk); if (grsegs[chunk]) MM_SetPurge(&grsegs[chunk], true);}

//#define CA_TileAtPos(x,y,plane) (*((unsigned far *)mapsegs[plane] + mapbwidthtable[y]/2+x))
// Nisaba patch:
#define CA_TileAtPos(x,y,plane) (*((word far *)((byte _seg *)mapsegs[plane] + mapbwidthtable[y])+x))	// K1n9_Duk3 mod: slightly faster version

#define CK_ObjectsCollide(currentObj, collideObj)	\
	(	(currentObj->clipRects.unitX2 > collideObj->clipRects.unitX1) &&	\
		(currentObj->clipRects.unitX1 < collideObj->clipRects.unitX2) &&	\
		(currentObj->clipRects.unitY1 < collideObj->clipRects.unitY2) &&	\
		(currentObj->clipRects.unitY2 > collideObj->clipRects.unitY1) )
/*
=============================================================================

						GLOBAL CONSTANTS

=============================================================================
*/

#define	MAXACTORS	MAXSPRITES
#define FOREGROUNDPRIORITY	MASKEDTILEPRIORITY
#define MAXPRIORITY	(PRIORITIES-1)

#define ACCGRAVITY	3
#define SPDMAXY		80

#define BLOCKSIZE	(8*PIXGLOBAL)		// for positioning starting actors

//
// movement scrolling edges
//
#define SCROLLEAST (TILEGLOBAL*11)
#define SCROLLWEST (TILEGLOBAL*9)
#define SCROLLSOUTH (TILEGLOBAL*8)
#define SCROLLNORTH (TILEGLOBAL*4)

#define CLIPMOVE	24			// speed to push out of a solid wall

#define GAMELEVELS	17

#define PLATFORMBLOCK	31			// InfoTile values
#define DIRARROWSTART	91
#define DIRARROWEND	99

// infoplane arrow directions (relative to DIRARROWSTART)
enum {
	arrow_North=0,
	arrow_East,
	arrow_South,
	arrow_West,
	arrow_NorthEast,
	arrow_SouthEast,
	arrow_SouthWest,
	arrow_NorthWest,
	NUMARROWDIRS
} arrowdir;

#define INFONUMBER_0 0x90
#define INFONUMBER_MAX (INFONUMBER_0 + 53)

#define DEFAULT_SHOTS 5
#define DEFAULT_LIVES 3

#define CHECKPOINT_INVINCIBILITY (TickBase)

//
// FITF-specific constants
//

#define MAXAMMO 25
// #define MAXAMMOEASY 30

// important map numbers
#define MAP_WORLDMAP		0
#define MAP_BWBMEGAROCKET   1
#define MAP_DOPEFISHLEVEL  11
#define MAP_MUSHROOMLEVEL1 16
#define MAP_TEMPLELEVEL    18
#define MAP_SLUGBUSTSLEVEL 19
#define MAP_BONUSLEVEL     20
#define MAP_HIGHSCORES     21
#define MAP_MUSHROOMLEVEL2 22
#define MAP_TARDISLEVEL    23
#define MAP_ACMELABLEVEL   24

// custom hitbox for Crab (used for tile collisions only):
#define CRAB_X1 0
#define CRAB_X2 (24*PIXGLOBAL)
#define CRAB_Y1 (1*PIXGLOBAL)
#define CRAB_Y2 (25*PIXGLOBAL)
#define CRAB_XMID ((CRAB_X1 + CRAB_X2)/2)
#define CRAB_YMID ((CRAB_Y1 + CRAB_Y2)/2)

/*
=============================================================================

							 TYPES

=============================================================================
*/


struct CK_object;

typedef enum CK_Difficulty
{
	D_NotPlaying = 0,
	D_Easy = 1,
	D_Normal = 2,
	D_Hard = 3
} CK_Difficulty;

// see WL_DEF.H
typedef enum CK_Controldir {
	CD_north = 0,
	CD_east = 1,
	CD_south = 2,
	CD_west = 3
} CK_Controldir;

typedef enum CK_Dir {
	Dir_east = 0,
	Dir_northeast = 1,
	Dir_north = 2,
	Dir_northwest = 3,
	Dir_west = 4,
	Dir_southwest = 5,
	Dir_south = 6,
	Dir_southeast = 7,
	Dir_nodir = 8
} CK_Dir;

typedef enum CK_ClassType {
	CT_nothing = 0,
	CT_Friendly = 1,
	CT_Player = 2,
	CT_Stunner = 3,
	CT_EnemyShot = 4,
	CT_Item = 5,
	CT_Platform = 6,
	CT_StunnedCreature = 7,
	CT_MapFlag = 8,

	CT_Slug = 9,
	CT_Smirky,
	CT_Skypest,
	CT_Mimrock,
	CT_PatPat,
	CT_CouncilMember,
	CT_CouncilMemberDone,
	CT_Lindsey,
	CT_LindseyHidden,
	CT_LindseyVanish,
	CT_LindseySpecial,
	CT_XkyLick,
	CT_FungusAmongus,
	CT_Yorp,
	CT_DungBeetle,
	CT_Chemolain,
	CT_LogPlatform,
	CT_OccupiedLog,
	CT_SlipperyLog,
	CT_Salamander,
	CT_DreamBubble,
	CT_Wormouth,
	CT_Bucket,
	CT_Shrubbery,
	CT_Bounder,
	CT_Tentacle,
	CT_Poleknawel,
	CT_Kitten,
	CT_GellyGhost,
	CT_Collectible,
	CT_Roots,
	CT_Yeti,
	CT_TinyYeti,
	CT_TinyYetiMaster,
	CT_YetiCloud,
	CT_Cloud,
	CT_Bird,
	CT_BirdEgg,
	CT_Egg,
	CT_SneakyTree,
	CT_Crocodent,
	CT_TentacleNest,
	CT_Checkpoint,
	CT_Warble,
	CT_Efree,
	CT_Nisasi,
	CT_NisasiCooldown,
	CT_Page,
	CT_SlideDoor,
	CT_CloneMachineOff,
	CT_CloneMachineOn,
	CT_CloneAnimation,
	CT_Clone,
	CT_CloneRising,
	CT_CoronaPlant,
	CT_CoronaBall,
	CT_Nipnap,
	CT_Crab,
	CT_BrainFucl,
	CT_Fungus,
	CT_WorldNessie,
	CT_FallBlock,
	CT_Obelisk,
	CT_Mummy1,
	CT_Mummy2,
	CT_DonutCloud,
	CT_DonutCloudShadow,
	CT_JediBoot,

	NUMCLASSTYPES

} CK_ClassType;

typedef enum CK_MiscFlag
{
  MF_Nil,
  MF_Pole,
  MF_Door,
  MF_Deadly,
  MF_Centilife,
  MF_SwitchOff,
  MF_SwitchOn,
  MF_KeyholderA,
  MF_KeyholderB,
  MF_KeyholderC,
  MF_KeyholderD,
  MF_WaterN,
  MF_WaterE,
  MF_WaterS,
  MF_WaterW,
  MF_BridgeSwitch = 15,
  MF_Moon = 16,
  MF_PathArrow = 17,
  MF_Bridge = 18,
  MF_ZapperOn = 19,
  MF_Teleporter = 20,
  MF_Points100,
  MF_Points200,
  MF_Points500,
  MF_Points1000,
  MF_Points2000,
  MF_Points5000,
  MF_1UP,
  MF_Stunner,
  MF_ZapperOff = 30,
  MF_AmptonComputer = 31,
  MF_KeycardDoor = 32,
  MF_MapElevatorLeft = 33,
  MF_MapElevatorRight = 34,
  MF_DeadlyWater,    //35
  MF_Warp,           //36
  MF_FireHazard,     //37
  MF_MindMachine,    //38
  MF_CloneMachine,   //39
  MF_GemDoor,        //40
  MF_QuickDoor,      //41
  MF_AmmoStation,    //42
  MF_MusicTrigger,   //43
  MF_3UP,            //44
  MF_Water,          //45
  MF_DoubleDoor1,    //46
  MF_DoubleDoor2,    //47
  MF_Quicksand,      //48
  MF_HAL,			 //49
  MF_Snailas,        //50
  MF_Foobs,          //51
  MF_Foreground = 0x80
} CK_MiscFlag;

// Tile Top Values
#define TI_TOP_BREAKABLE	57

// Level Done Flags (bit flags!)
#define DF_DONE		1
#define DF_PERFECT	2
#define DF_CHEATED	4

typedef enum CK_LevelState
{
	LS_Playing = 0,             // In Level
	LS_Died = 1,                // Keen Died
	LS_LevelComplete = 2,       // Level Completed
	LS_CouncilRescued = 3,      // Rescued Council Member (Keen 4)
	LS_AboutToRecordDemo = 4,   // About to Record Demo
	LS_StartingNew = 5,	        // About to start a new game
	LS_StartingSaved = 6,       // Resuming a saved game
	LS_Aborted	   = 8,
	LS_DestroyedQED = 15,       // Destroyed QED (Keen 5)

	// Unauthentic level states
	LS_WarpToLevel,
	LS_ClonedJanitor,			// clone machine spawned 8 janitor clones
	LS_Landed,
	LS_BrainFucled,
	LS_BonusComplete,
	LS_MushroomTrigger,			
	LS_SecretTrigger,
} CK_LevelState;

extern CK_LevelState ck_levelState;

// This struct is 0x58 bytes big in Keen5
// It must be preserved if omnispeak savegames are to be compatible 
// with those of vanilla keen
typedef struct CK_GameState
{
	unsigned mapPosX;
	unsigned mapPosY;
	unsigned levelsDone[26];  // Number of levels complete
	long keenScore;			  // Keen's score. (This _is_ signed, by the looks of all the 'jl' instructions)
	long nextKeenAt;			// Score keen will get a new life at.
	long oldScore;				// K1n9_Duk3 addition for "perfect" scores
	int numShots;
	int numVitalin;
/*
	// stuff from Keen 5 that we don't need:
	int securityCard;
	int word_4729C;
	int fusesRemaining;
*/
	// FITF variables:
	unsigned collectiblesTotal;	// stuff that's already collected (bitfield!)
	unsigned janitorsSpawned, janitorsSkipped, janitorMessageShown;	//(bitfields)
	unsigned slugcans;

	int currentLevel;
	int numLives;			// Number of lives keen has.
	CK_Difficulty difficulty;		// Difficulty level of current game
	//struct CK_object *platform;  // This was a 16-bit pointer in DOS Keen5.exe

	//int generatorsDestroyed;	// from Atroxian Realm, not used in FITF

	// cheats have been moved into the gamestate for FITF
	boolean jumpCheat;				// Is the jump cheat enabled? (Not in Keen5 gamestate struct)
	boolean godMode;
	boolean slowMotionEnabled;
	boolean cheated;
	int brainfucled;
	unsigned continuesUsed, jokeNumber;
	byte musicplayed[LASTMUSIC];
} CK_GameState;

extern CK_GameState ck_gameState;

// K1n9_Duk3 addition: Map State

/*

The CK_MapState struct is supposed to hold all the map-specific parts of the
CK_GameState struct, like keys, number of fuses remaining (Keen 5), number of
items collected, number of items in the level and such things.

The main reason why I wanted to split the gamestate struct is that the LoadGame
function reads the gamestate struct and then loads the level as if the level was
started from the beginning, then it loads the map planes and objects to restore
the game to the state it was in when it was saved.

I thought it would make sense to adjust certain variables during CK_LoadLevel,
but since that function is also called when loading a game and is actually
called after the gamestate struct was loaded, it would mess up the elements of
the gamestate struct. That means you would have to create a temporary backup of
the affected elements, like Keen 5 does for the fuses remaining in the level.
Moving the affected elements into a different struct that can be loaded from
the savegame file AFTER CK_LoadLevel was executed would avoid this issue
entirely.

There's also the added benefit of being able to reset the second struct during
CK_LoadLevel to avoid redundancy. And since most if not all elements of this
struct need to be set to 0 initially, we can just use memset to set everything
to 0.

*/

typedef struct CK_MapState
{
	int keyGems[4];
	unsigned collectiblesNew;		// stuff that was collected in the current level (bitfield!)
	unsigned slugcans;
	int nextLevel;
	unsigned doneFlags;
	unsigned janitorsSpawned, janitorsSkipped, dreamMsg, warpInfo;
	boolean checkpointSaved;
	struct CK_object *checkpointTouched, *lastCheckpoint;
	struct CK_object *nisasi, *mindcontrolled, *quicksand, *mummy;
	boolean keenGetsPushed;
	int invincibilityTimer;
	int stunCounter, stunCooldown;
	int moontile, inwater;
	int brainfucl;
	int machinesActive;
	int music;
	int shaketics, shakecount, shakeX, shakeY;
	
	int screenYpx, wrapScroll;

	// for special bonus when collecting all point items:
	unsigned itemsTotal, itemsCollected;
	// we have decided against statistics in the form "(x) of (y) items collected",
	// so we don't need the following variables anymore
/*
	unsigned livesTotal, livesCollected;
	unsigned dropsTotal, dropsCollected;
	unsigned ammoTotal, ammoCollected;
	long pointsTotal, pointsCollected;
*/
} CK_MapState;

extern CK_MapState ck_mapState;
extern int brainfucled;

// end of K1n9_Duk3 addition

typedef enum CK_ActionType
{
	AT_UnscaledOnce,			// Unscaled Motion, Thinks once.
	AT_ScaledOnce,				// Scaled Motion, Thinks once.
	AT_Frame,					// No Motion, Thinks each frame (doesn't advance action)
	AT_UnscaledFrame,			// Unscaled Motion, Thinks each frame
	AT_ScaledFrame				// Scaled Motion, Thinks each frame
} CK_ActionType;

typedef enum CK_PushType
{
	push_none,
	push_down,
	push_up,
	push_left,
	push_right,
	push_side
} CK_PushType;

typedef struct CK_action
{
	int chunkLeft;
	int chunkRight;
	CK_ActionType type;
	int protectAnimation;
	CK_PushType stickToGround; 	// See KeenWiki: Galaxy Action Parameters (lemm/levelass)
	int timer;
	int velX, velY;
	void (*think)(struct CK_object *obj);
	void (*collide)(struct CK_object *obj, struct CK_object *other);
	void (*draw)(struct CK_object *obj);
	struct CK_action *next;
} CK_action;

typedef enum CK_objActive
{
	OBJ_INACTIVE = 0,
	OBJ_ACTIVE = 1,
	OBJ_ALWAYS_ACTIVE = 2,
	OBJ_EXISTS_ONLY_ONSCREEN = 3
} CK_objActive;

typedef enum CK_clipped
{
	CLIP_not = 0,
	CLIP_normal,
	CLIP_simple,
	CLIP_full,
	CLIP_custom,	// K1n9_Duk3 addition
} CK_ClipType;

typedef struct CK_objPhysData
{
	// Unit cliping box
	int unitX1;
	int unitY1;
	int unitX2;
	int unitY2;
	int unitXmid;


	// Tile clipping box
	int tileX1;
	int tileY1;
	int tileX2;
	int tileY2;
	int tileXmid;
} CK_objPhysData;

// Because this struct was imported from 32 bit omnispeak to 16 bit dos
// need a second struct with unsigned members for holding absolute positions

typedef struct CK_objPhysDataU
{
	// Unit cliping box
	unsigned unitX1;
	unsigned unitY1;
	unsigned unitX2;
	unsigned unitY2;
	unsigned unitXmid;


	// Tile clipping box
	unsigned tileX1;
	unsigned tileY1;
	unsigned tileX2;
	unsigned tileY2;
	unsigned tileXmid;
} CK_objPhysDataU;

typedef struct CK_object
{
	int type;
	CK_objActive active;
	boolean visible;
	CK_ClipType clipped;
	int timeUntillThink;		// Number of frames to delay calling Think function
	unsigned posX;
	unsigned posY;
	int xDirection;
	int yDirection;
	int deltaPosX;
	int deltaPosY;
	int velX;
	int velY;
	int actionTimer;
	CK_action *currentAction;
	int gfxChunk;
	int zLayer;

	CK_objPhysDataU clipRects;

	// In real keen, these are shared, so we're moving them out.
#if 0
	CK_objPhysData oldRects;
	CK_objPhysData deltaRects;
	int nextX;
	int nextY;
#endif

	//TileInfo for surrounding tiles.
	int topTI, bottomTI, leftTI, rightTI;


	//struct RF_SpriteDrawEntry *sde;
	void *sde;

	int user1;
	int user2;
	int user3;
	int user4;

	struct CK_object *next;
	struct CK_object *prev;
} CK_object;

extern CK_objPhysData ck_oldrects;
extern CK_objPhysData ck_deltarects;
extern int ck_nextX;
extern int ck_nextY;

typedef struct CK_keenState
{
	int jumpTimer;
	long poleGrabTime;	// K1n9_Duk3 fix: this needs to use the same type as lasttimecount (see ID_RF.H)

	boolean jumpIsPressed;
	boolean jumpWasPressed;
	boolean pogoIsPressed;
	boolean pogoWasPressed;
	boolean shootIsPressed;
	boolean shootWasPressed;

	boolean keenSliding;
	boolean mustAdjustNextX;	// K1n9_Duk3 addition
	CK_object *platform;
} CK_keenState;

extern CK_keenState ck_keenState;
extern ControlInfo ck_inputFrame;

/*
=============================================================================

					 CK_MAIN.C DEFINITIONS

=============================================================================
*/

extern	char	str[80],str2[20];
extern	boolean	singlestep,jumpcheat,godmode,tedlevel;
extern	int extravbls; // Imported from Catacomb 3-D
extern	unsigned	tedlevelnum;

void CK_MeasureMultiline(const char *str, unsigned *w, unsigned *h);

void	DebugMemory (void);
void	TestSprites(void);
int		DebugKeys (void);
void	StartupId (void);
void	ShutdownId (void);
void	Quit (char *error);
void	InitGame (void);
void	DemoLoop (void);
void	main (void);

//TODO:		Variable lost and found... put these somwehere
extern		boolean		showscorebox;
extern		CK_Difficulty ck_startingDifficulty;
extern		int load_game_error, ck_startingSavedGame;
extern		boolean		ck_inHighScores;
extern		int		ck_nextMapNumber;
/*
=============================================================================

					  CK_GAME.C DEFINITIONS

=============================================================================
*/

extern		int			fadecount;
extern		CK_LevelState 		levelState;				// Level State (should probably be enum)


// Rename these to names from Catacomb 3-D (fix conflict with SaveGame struct)
boolean	LoadTheGame/*LoadGame*/ (int file);
boolean	SaveTheGame/*SaveGame*/ (int file);


void CK_NewGame(void);
void CK_GameOver(void);
void CK_ResetGame (void);
boolean SaveCheckpoint(void);
boolean LoadCheckpoint(void);
void CK_SetLoadingTransition(void);
void FadeAndUnhook (void);
void CK_BeginCacheBox (char *title, unsigned numChunks);
void CK_UpdateCacheBox(void);
void CK_FinishCacheBox(void);
void CK_TryAgainMenu(void);
void CK_MapLevelMarkAsDone(void);
void CK_LoadLevel(boolean doCache);
void CK_StartDemoRecord(void);
void CK_EndDemoRecord (void);
void CK_GameLoop(void);

void CK_SetPaletteVGA(void far *palette);
void CK_DefaultPalette(void);
void CK_LevelPalette(void);
void CK_FadeIn(void);
void CK_FadeOut(void);


/*
=============================================================================

					  CK_PLAY.C DEFINITIONS

=============================================================================
*/

#define CK_MAX_OBJECTS 100

/*
typedef struct {
	CK_object objArray[CK_MAX_OBJECTS];
	CK_object *freeObject, *lastObject;
	int numObjects;
} CK_objectlist;
*/

// Switch to toggle Camera following keen.
extern	boolean ck_scrollDisabled;

extern	ControlInfo	ck_inputFrame;

extern	CK_object ck_objArray[CK_MAX_OBJECTS];

extern	CK_object *ck_freeObject;
extern	CK_object *ck_lastObject;
extern	CK_object tempObj;
extern	int ck_numObjects;

extern	CK_object *ck_keenObj;
extern	CK_object *ck_scoreBoxObj;

// The rectangle within which objects are active.
extern	int ck_activeX0Tile;
extern	int ck_activeY0Tile;
extern	int ck_activeX1Tile;
extern	int ck_activeY1Tile;

void	SpawnScore (void);
void CK_ForceScoreboxUpdate(void);
void CK_UpdateScoreBox (CK_object *ob);
void ScoreReact (CK_object *ob);

void CK_CountActiveObjects(void);
void DebugMemory (void);
void CK_ItemCheat(void);
void MergeTile16M(unsigned dest, unsigned source);
void CK_WallDebug(void);
void UndoWallDebug(void);
void TestSprites(void);
boolean CK_DebugKeys(void);
void CK_CheckKeys(void);
void CK_HandleInput(void);
void CK_SetupObjArray(void);
CK_object *CK_GetNewObj(boolean nonCritical);
void CK_RemoveObj(CK_object *obj);
int CK_ActionThink(CK_object *obj, int time);
void CK_RunAction(CK_object *obj);
void CK_CentreCamera(CK_object *obj);
void CK_MapCamera( CK_object *keen );
void CK_NormalCamera(CK_object *obj);
void StartMusic(unsigned level, boolean repeat);
void StopMusic(void);
void CK_PlayLoop (void);


/*
=============================================================================

					  CK_INTER.C DEFINITIONS

=============================================================================
*/

void CK_MainMenu(void);
void CK_HandleDemoKeys(void);
boolean FizzleDown(int speedmask);
void CK_DrawTerminator(void);
void CK_DrawStarWars(void);
void CK_ShowTitleScreen(void);
void CK_PlayDemo(int demoNumber);
void CK_DrawHighscores(void);
void CK_SubmitHighScore(long score, unsigned arg_4, int cans, int arg_5);
void CK_DoHighScores(void);

/*
=============================================================================

					  CK_TEXT.C DEFINITIONS

=============================================================================
*/

void HelpScreens (void);
void EndText(int num);

/*
=============================================================================

					  CK_PHYS.C DEFINITIONS

=============================================================================
*/

#define SLOPEMASK 7

void CK_CalcTileClipRects(CK_object *obj);
void CK_ResetClipRects(CK_object *obj);
void CK_SetOldClipRects(CK_object *obj);
void CK_SetDeltaClipRects(CK_object *obj);
void CK_PhysUpdateX(CK_object *obj, int deltaUnitX);
void CK_PhysUpdateY(CK_object *obj, int deltaUnitY);
void CK_PhysKeenClipDown(CK_object *obj);
void CK_PhysKeenClipUp(CK_object *obj);
boolean CK_NotStuckInWall(CK_object *obj);
void CK_PhysClipVert(CK_object *obj);
void CK_PhysClipHorz(CK_object *obj);
void CK_PhysUpdateNormalObj(CK_object *obj);
void CK_PhysFullClipToWalls(CK_object *obj);
void CK_PhysUpdateSimpleObj(CK_object *obj);
void CK_PhysPushX(CK_object *pushee, CK_object *pusher);
void CK_PhysPushY(CK_object *pushee, CK_object *pusher);
void CK_ClipToSprite(CK_object *push, CK_object *solid, boolean squish);
void CK_SetAction(CK_object *obj, CK_action *act);
void CK_SetAction2(CK_object *obj, CK_action *act);
boolean CK_ObjectVisible(CK_object *obj);
void CK_PhysGravityHigh(CK_object *obj);
void CK_PhysGravityMid(CK_object *obj);
void CK_PhysGravityLow(CK_object *obj);
void CK_PhysGravityEasyKeen(CK_object *obj);
void CK_PhysDampHorz(CK_object *obj);
void CK_PhysDampHorz2(CK_object *obj);	// K1n9_Duk3 addition
void CK_PhysAccelHorz(CK_object *obj, int accX, int velLimit);
void CK_PhysDampVert(CK_object *obj);
void CK_PhysAccelVert(CK_object *obj, int accX, int velLimit);

/*
=============================================================================

					  CK_KEEN.C DEFINITIONS

=============================================================================
*/

CK_action CK_ACT_itemNotify;

CK_action CK_ACT_KeenShrug;
CK_action CK_ACT_keenStanding;
CK_action CK_ACT_keenRidePlatform;
CK_action CK_ACT_keenLookUp1;
CK_action CK_ACT_keenLookUp2;
CK_action CK_ACT_keenLookDown1;
CK_action CK_ACT_keenLookDown2;
CK_action CK_ACT_keenLookDown3;
CK_action CK_ACT_keenLookDown4;
CK_action CK_ACT_keenIdle;
CK_action CK_ACT_keenBored1;
CK_action CK_ACT_keenBored2;
CK_action CK_ACT_keenBored3;
CK_action CK_ACT_keenBored4;
CK_action CK_ACT_keenBored5;
CK_action CK_ACT_keenBored6;
CK_action CK_ACT_keenOpenBook1;
CK_action CK_ACT_keenOpenBook2;
CK_action CK_ACT_keenOpenBook3;
CK_action CK_ACT_keenOpenBook4;
CK_action CK_ACT_keenReadBook1;
CK_action CK_ACT_keenReadBook2;
CK_action CK_ACT_keenReadBook3;
CK_action CK_ACT_keenStowBook1;
CK_action CK_ACT_keenStowBook2;
CK_action CK_ACT_keenStowBook3;
CK_action CK_ACT_keenNot;
CK_action CK_ACT_keenDie0;
CK_action CK_ACT_keenDie1;
CK_action CK_ACT_keenRun1;
CK_action CK_ACT_keenRun2;
CK_action CK_ACT_keenRun3;
CK_action CK_ACT_keenRun4;
CK_action CK_ACT_keenShoot1;
CK_action CK_ACT_keenShoot2;
CK_action CK_ACT_keenShootUp1;
CK_action CK_ACT_keenShootUp2;
CK_action CK_ACT_keenJump1;
CK_action CK_ACT_keenJump2;
CK_action CK_ACT_keenFall1;
CK_action CK_ACT_keenFall2;
CK_action CK_ACT_keenJumpShoot1;
CK_action CK_ACT_keenJumpShoot2;
CK_action CK_ACT_keenJumpShoot3;
CK_action CK_ACT_keenJumpShootUp1;
CK_action CK_ACT_keenJumpShootUp2;
CK_action CK_ACT_keenJumpShootUp3;
CK_action CK_ACT_keenJumpShootDown1;
CK_action CK_ACT_keenJumpShootDown2;
CK_action CK_ACT_keenJumpShootDown3;
CK_action CK_ACT_keenPogo1;
CK_action CK_ACT_keenPogo2;
CK_action CK_ACT_keenPogo3;
CK_action CK_ACT_keenHang1;
CK_action CK_ACT_keenHang2;
CK_action CK_ACT_keenPull1;
CK_action CK_ACT_keenPull2;
CK_action CK_ACT_keenPull3;
CK_action CK_ACT_keenPull4;
CK_action CK_ACT_keenPull5;
CK_action CK_ACT_keenPoleSit;
CK_action CK_ACT_keenPoleUp1;
CK_action CK_ACT_keenPoleUp2;
CK_action CK_ACT_keenPoleUp3;
CK_action CK_ACT_keenPoleDown1;
CK_action CK_ACT_keenPoleDown2;
CK_action CK_ACT_keenPoleDown3;
CK_action CK_ACT_keenPoleDown4;
CK_action CK_ACT_keenPoleShoot1;
CK_action CK_ACT_keenPoleShoot2;
CK_action CK_ACT_keenPoleShootUp1;
CK_action CK_ACT_keenPoleShootUp2;
CK_action CK_ACT_keenPoleShootDown1;
CK_action CK_ACT_keenPoleShootDown2;

CK_action CK_ACT_keenShot1;
CK_action CK_ACT_keenShot2;
CK_action CK_ACT_keenShot3;
CK_action CK_ACT_keenShot4;

CK_action CK_ACT_keenShotHit1;
CK_action CK_ACT_keenShotHit2;

CK_action CK_ACT_keenSlide;

CK_action CK_ACT_keenPlaceGem;

CK_action CK_ACT_keenPressSwitch1;
CK_action CK_ACT_keenPressSwitch2;

CK_action CK_ACT_keenEnterDoor1;
CK_action CK_ACT_keenEnterDoor2;
CK_action CK_ACT_keenEnterDoor3;
CK_action CK_ACT_keenEnterDoor4;
CK_action CK_ACT_keenEnterDoor5;
CK_action CK_ACT_keenEnterDoor6;
CK_action CK_ACT_keenEnteredDoor;

// New Stuff:
CK_action CK_ACT_keenStunned1, CK_ACT_keenStunned2, CK_ACT_KeenNull;

void CK_KeenColFunc(CK_object *a, CK_object *b);
void CK_IncreaseScore(int score);
void CK_SpawnKeen(int tileX, int tileY, int direction);
void CK_KeenGetTileItem(int tileX, int tileY, int itemNumber);
void CK_GetVitalin(int tileX, int tileY);
void CK_KeenGetTileVitalin(int tileX, int tileY);
void CK_KeenCheckSpecialTileInfo(CK_object *obj);
void CK_KeenPressSwitchThink(CK_object *obj);
boolean CK_KeenPressUp(CK_object *obj);
void CK_KeenSlide(CK_object *obj);
void CK_KeenEnterDoor0(CK_object *obj);
void CK_KeenEnterDoor1(CK_object *obj);
void CK_KeenTeleport(CK_object *obj, unsigned destination);
void CK_KeenEnterDoor(CK_object *obj);
void CK_KeenPlaceGem(CK_object *obj);
void CK_KeenRidePlatform(CK_object *obj);
boolean CK_KeenTryClimbPole(CK_object *obj);
void CK_KeenRunningThink(CK_object *obj);
void CK_HandleInputOnGround(CK_object *obj);
void CK_KeenStandingThink(CK_object *obj);
void CK_KeenLookUpThink(CK_object *obj);
void CK_KeenLookDownThink(CK_object *obj);
void CK_KeenDrawFunc(CK_object *obj);
void CK_KeenRunDrawFunc(CK_object *obj);
void CK_KeenReadThink(CK_object *obj);
void CK_KeenJumpThink(CK_object *obj);
void CK_KeenJumpDrawFunc(CK_object *obj);
void CK_KeenPogoBounceThink(CK_object *obj);
void CK_KeenPogoThink(CK_object *obj);
void CK_KeenBreakFuse(int x, int y);
void CK_KeenPogoDrawFunc(CK_object *obj);
void CK_KeenSpecialColFunc(CK_object *obj, CK_object *other);
void CK_KeenSpecialDrawFunc(CK_object *obj);
void CK_KeenHangThink(CK_object *obj);
void CK_KeenPullThink1(CK_object *obj);
void CK_KeenPullThink2(CK_object *obj);
void CK_KeenPullThink3(CK_object *obj);
void CK_KeenPullThink4(CK_object *obj);
void CK_KeenDeathThink(CK_object *obj);
void CK_KillKeen(void);
void CK_KeenPoleHandleInput(CK_object *obj);
void CK_KeenPoleSitThink(CK_object *obj);
void CK_KeenPoleUpThink(CK_object *obj);
void CK_KeenPoleDownThink(CK_object *obj);
void CK_KeenPoleDownDrawFunc(CK_object *obj);
void CK_SpawnShot(int x, int y, int direction);
void CK_ShotHit(CK_object *obj);
void CK_ShotThink(CK_object *shot);
void CK_ShotDrawFunc(CK_object *obj);
void CK_KeenSpawnShot(CK_object *obj);
void CK_KeenFall(CK_object *obj);

extern unsigned CK5_ItemPoints[];

// New Stuff:
void CK_ToggleSwitch(int switchTileX, int switchTileY);
void CK_HALTile(void);
void CK_StunKeen(int midx);
boolean CK_KeenIsDead(void);
void SpawnDreamBubble(unsigned x, unsigned y, unsigned type);
void RemoveDreamBubbles(void);

extern CK_action CK_ACT_keenDrown;
extern CK_action CK_ACT_keenBurn;
void CK_KillKeenEx(boolean force);
void CK_DrownKeen(int tileY);
void CK_BurnKeen(boolean force);

/*
=============================================================================

					  CK_OBJ.C DEFINITIONS

=============================================================================
*/

extern CK_action CK_ACT_DoorOpen1;
extern CK_action CK_ACT_DoorOpen2;
extern CK_action CK_ACT_DoorOpen3;

extern CK_action CK_act_item;
//extern CK_action CK_act_item2;

extern CK_action CK_ACT_VitalinNotify1;
extern CK_action CK_ACT_VitalinNotify2;
extern CK_action CK_ACT_VitalinNotify3;
extern CK_action CK_ACT_VitalinNotify4;

extern CK_action CK_ACT_FallPlat0;
extern CK_action CK_ACT_FallPlat1;
extern CK_action CK_ACT_FallPlat2;

void CK_Fall(CK_object *obj);
void CK_Fall2(CK_object *obj);
void CK_Glide(CK_object *obj);
void CK_DeadlyCol(CK_object *o1, CK_object *o2);
void CK_DeadlyCol2(CK_object *o1, CK_object *o2);
void CK_BasicDrawFunc1(CK_object *obj);
void CK_BasicDrawFunc2(CK_object *obj);
void CK_BasicDrawFunc3(CK_object *obj);
void CK_BasicDrawFunc4(CK_object *obj);
void CK_StunCreature(CK_object *creature, CK_object *stunner, CK_action *new_creature_act);
CK_object *CK_SpawnItem(int tileX, int tileY, int itemNumber);
void CK_PointItem(CK_object *obj);
void CK_SpawnAxisPlatform(int tileX, int tileY, int direction);
void CK_AxisPlatform(CK_object *obj);
void CK_PlatformDraw(CK_object *ob);
void CK_SpawnFallPlat(int tileX, int tileY);
void CK_FallPlatSit (CK_object *obj);
void CK_FallPlatFall (CK_object *obj);
void CK_FallPlatRise (CK_object *obj);

void CK_DoorOpen(CK_object *obj);
boolean CK_DoorsActive(void);
void CK_SecurityDoorOpen(CK_object *obj);

void CK_SpawnSlug(int tileX, int tileY);
//void CK_SpawnSmirky(int tileX, int tileY);
void CK_SpawnSkypest(int tileX, int tileY);
void CK_SpawnMimrock(int tileX, int tileY);
void CK_SpawnPatPat(int tileX, int tileY, int dir);
void CK_SpawnCouncilMember(int tileX, int tileY, int number/*, boolean forced*/);
void CK_SpawnLindsey(int tileX, int tileY, int num);
void CK_LindseyVanish(CK_object *ob);
void CK_SpawnXkyLick(int tileX, int tileY);
void CK_SpawnFungusAmongus(int tileX, int tileY);

void CK_SpawnDungBeetle(int tileX, int tileY);
void CK_SpawnChemolain(int tileX, int tileY);
void CK_SpawnLogPlatform(int tileX, int tileY, int xdir);
void CK_SpawnSalamander(int tileX, int tileY);
void CK_SpawnWormouth(int tileX, int tileY);
void CK_SpawnShrubbery(int tileX, int tileY);
void CK_SpawnBounder(int tileX, int tileY);
void CK_SpawnTentacle(int tileX, int tileY);
extern CK_action CK_ACT_Swallowed;
void CK_SpawnPoleknawel(int tileX, int tileY);
void CK_SpawnKitten(int tileX, int tileY);
void CK_SpawnGellyGhost(int tileX, int tileY);
void CK_SpawnCollectible(int tileX, int tileY, int number);
void CK_SpawnRoots(int tileX, int tileY);
void CK_SpawnYeti(int tileX, int tileY);
void CK_SpawnCloud(int tileX, int tileY);
void CK_SpawnInvisibleCloud(unsigned unitX, unsigned unitY, unsigned w, unsigned h);
void CK_SpawnMolet(int tileX, int tileY);
void CK_SpawnSneakyTree(int tileX, int tileY);
void CK_SpawnBird(int tileX, int tileY);
void CK_SpawnCroc(int tileX, int tileY);
void CK_SpawnTentacleNest(int tileX, int tileY);

extern CK_action CK_ACT_Smoke;
void CK_SpawnCheckpoint(int tileX, int tileY);
void CheckpointSave(CK_object *ob);
boolean CheckpointLoad(void);
CK_object *SpawnSmoke(int x, int y);
CK_object *SpawnSmokes(int x, int y);
void CK_SpawnLeaveaTrip(int tileX, int tileY);
void CK_SpawnWarble(int tileX, int tileY);
CK_object *CK_SpawnStaticSprite(unsigned tileX, unsigned tileY, graphicnums chunk, int zLayer);
void CK_SpawnEfree(unsigned x, unsigned y);
void CK_EfreeGrabKeen(CK_object *ob);
void CK_SpawnNisasi(int tileX, int tileY);
void CK_DropNisasi(void);		// for Keen
void CK_ObjDropNisasi(CK_object *ob);	// for other objects
void CK_SpawnCouncilPage(int tileX, int tileY, int type);
CK_object *CK_SpawnSlideDoor(int tileX, int tileY, int type);
CK_object *CK_FindPageObj(unsigned infoval);
void CK_ControlPage(CK_object *ob);
extern CK_action CK_ACT_keenEnterMachine;
extern CK_action CK_ACT_keenMindcontrol;

void CK_SpawnCloneMachine(int tileX, int tileY, int type, int dir);
void CK_ToggleCloneMachine(int tileX, int tileY);
void CK_SpawnCoronaPlant(int tileX, int tileY, int dir);
void CK_SpawnNipnap(int tileX, int tileY);
void CK_SpawnCrab(int tileX, int tileY, boolean onCeiling);
void CK_SpawnBrainFucl(int tileX, int tileY);
void CK_SpawnDonut(int tileX, int tileY);
void CK_SpawnBalloonSmirky(int tileX, int tileY);
void CK_SpawnFungusAmongus(int tileX, int tileY);
void CK_SpawnWorldNessie(int tileX, int tileY);
void CK_SpawnFrog(int tileX, int tileY);
void CK_SpawnQuicksand(unsigned posX, unsigned posY);
void CK_SpawnFallBlock(int tileX, int tileY);
void CK_SpawnObelisk(int tileX, int tileY);
void CK_SpawnMummySpawner(int tileX, int tileY);
void CK_SpawnLemonShark(int tileX, int tileY);
void CK_SpawnDonutCloud(int tileX, int tileY);
void CK_SpawnDonutCloudShadow(int tileX, int tileY);
void CK_SpawnJediBoot(int tileX, int tileY);
void CK_SpawnSlugCans(int tileX, int tileY);
void CK_SpawnBigShroom(int tileX, int tileY);

/*
=============================================================================

					  CK_MAP.C DEFINITIONS

=============================================================================
*/

extern CK_action CK_ACT_MapKeenStart;
extern CK_action CK_ACT_MapKeenWave0;
extern CK_action CK_ACT_MapKeenWave1;
extern CK_action CK_ACT_MapKeenWave2;
extern CK_action CK_ACT_MapKeenWave3;
extern CK_action CK_ACT_MapKeenWave4;
extern CK_action CK_ACT_MapKeenWalk0;
extern CK_action CK_ACT_MapFlag0;
extern CK_action CK_ACT_MapFlag1;
extern CK_action CK_ACT_MapFlag2;
extern CK_action CK_ACT_MapFlag3;

void CK_SpawnMapKeen(int tileX, int tileY);
void CK_ScanForLevelEntry(CK_object * obj);
void CK_ScanForWorldmapTile(CK_object * obj);
void CK_MapKeenStill(CK_object * obj);
void CK_MapKeenWalk(CK_object * obj);
void CK_MapMiscFlagsCheck(CK_object *keen);
void CK_MapFlagSpawn(unsigned tileX, unsigned tileY, unsigned spriteoff);
void CK_SpawnRisingFlag(unsigned tileX, unsigned tileY, unsigned spriteoff);
/*
void CK_KeenFlagThrown (CK_object *obj);
void CK_KeenFlagSearch (CK_object *obj);
void CK_KeenFlagLands(CK_object *obj);
*/
/*
=============================================================================

					  CK_MISC.C DEFINITIONS

=============================================================================
*/
extern const char *ck_levelNames[];
void CK_ScanInfoLayer(void);
void CK_JanitorDialog(int number);
void CK_JanitorSleepMonolog(void);
void CK_JanitorStunnedMonolog(void);
void CK_JanitorNisasiDialog(int number);
void CK_LindseyDialog(int number);
void CK_HALDialog(void);
void CK_BonusLevelDialog(void);
void RunSnailas(void);
void RunFoobs(void);
void CK_MiscDialog(int number);
void CK_Jukebox(void);
void RunFoobsInSpace(void);
