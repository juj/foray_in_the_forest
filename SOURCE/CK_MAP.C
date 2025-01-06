/* "Foray in the Forest" Source Code
 * Copyright (C) 2022-2023 Nisaba
 * Copyright (C) 2019-2021 K1n9_Duk3
 *
 * The code in this file is based on:
 * Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
 * Copyright (C) 1993-2014 Flat Rock Software
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

// AR_MAP.C

#include "CK_DEF.H"
#pragma	hdrstop

/*
=============================================================================

LOCAL CONSTANTS

=============================================================================
*/


/*
=============================================================================

GLOBAL VARIABLES

=============================================================================
*/

CK_action CK_ACT_MapKeenStart = { 0x0, 0x0, AT_UnscaledFrame, 0x0, 0x0, 0x168, 0x0, 0x0, CK_MapKeenStill, NULL, CK_BasicDrawFunc1, &CK_ACT_MapKeenWave0};
CK_action CK_ACT_MapKeenWave0 = { WORLDKEENWAVE1SPR, WORLDKEENWAVE1SPR, AT_UnscaledFrame, 0x0, 0x0, 0x14, 0x0, 0x0, CK_MapKeenStill, NULL, CK_BasicDrawFunc1, &CK_ACT_MapKeenWave1};
CK_action CK_ACT_MapKeenWave1 = { WORLDKEENWAVE2SPR, WORLDKEENWAVE2SPR, AT_UnscaledFrame, 0x0, 0x0, 0x14, 0x0, 0x0, CK_MapKeenStill, NULL, CK_BasicDrawFunc1, &CK_ACT_MapKeenWave2};
CK_action CK_ACT_MapKeenWave2 = { WORLDKEENWAVE1SPR, WORLDKEENWAVE1SPR, AT_UnscaledFrame, 0x0, 0x0, 0x14, 0x0, 0x0, CK_MapKeenStill, NULL, CK_BasicDrawFunc1, &CK_ACT_MapKeenWave3};
CK_action CK_ACT_MapKeenWave3 = { WORLDKEENWAVE2SPR, WORLDKEENWAVE2SPR, AT_UnscaledFrame, 0x0, 0x0, 0x14, 0x0, 0x0, CK_MapKeenStill, NULL, CK_BasicDrawFunc1, &CK_ACT_MapKeenWave4};
CK_action CK_ACT_MapKeenWave4 = { WORLDKEENWAVE1SPR, WORLDKEENWAVE1SPR, AT_UnscaledFrame, 0x0, 0x0, 0x14, 0x0, 0x0, CK_MapKeenWalk, NULL, CK_BasicDrawFunc1, &CK_ACT_MapKeenStart};
CK_action CK_ACT_MapKeenWalk0 = { 0x0, 0x0, AT_ScaledOnce, 0x1, 0x0, 0x4, 0x18, 0x18, CK_MapKeenWalk, NULL, CK_BasicDrawFunc1, &CK_ACT_MapKeenWalk0};

void CK_MapShipThink(CK_object *ob);
CK_action CK_ACT_MapShip = {WORLDSHIPSPR, WORLDSHIPSPR, AT_Frame, false, false, 0, 0, 0, CK_MapShipThink, NULL, CK_BasicDrawFunc1, NULL};

//===========================================================================
//
// MAP KEEN
//
//===========================================================================


static int ck_mapKeenFrames[] ={ WORLDKEENU1SPR-1, WORLDKEENUR1SPR-1, WORLDKEENR1SPR-1, WORLDKEENDR1SPR-1, WORLDKEEND1SPR-1, WORLDKEENDL1SPR-1, WORLDKEENL1SPR-1, WORLDKEENUL1SPR-1 };
static int word_417BA[] ={ 2, 3, 1, 3}; 

void CK_SpawnMapKeen(int tileX, int tileY)
{
	unsigned info = CA_TileAtPos(tileX+1, tileY, 2);

	ck_keenObj->active = OBJ_ALWAYS_ACTIVE;
	if (ck_gameState.mapPosX == 0 && info != 0)
	{
		int px, py;

		ck_keenObj->clipped = CLIP_not;
		ck_keenObj->type = CT_Friendly;
		ck_keenObj->posX = RF_TileToUnit(tileX);
		ck_keenObj->posY = RF_TileToUnit(tileY);
		ck_keenObj->zLayer = FOREGROUNDPRIORITY;
		// get target coordinates:
		px = (info >> 8);
		py = (info & 0xFF);
		ck_keenObj->user1 = RF_TileToUnit(px);
		ck_keenObj->user2 = RF_TileToUnit(py) - 20*PIXGLOBAL;	// -4 pixels so the ship can land
		// erase BWB tiles at target position:
		px += mapbwidthtable[py] / 2;
		mapsegs[1][px++] = 0;
		mapsegs[1][px] = 0;
		CK_SetAction(ck_keenObj, &CK_ACT_MapShip);
		CA_MarkGrChunk(ck_keenObj->gfxChunk);
	}
	else
	{
		ck_keenObj->type = CT_Player;
		if (ck_gameState.mapPosX == 0)
		{
			ck_keenObj->posX = RF_TileToUnit(tileX);
			ck_keenObj->posY = RF_TileToUnit(tileY);
		}
		else
		{
			ck_keenObj->posX = ck_gameState.mapPosX;
			ck_keenObj->posY = ck_gameState.mapPosY;
			ck_gameState.levelsDone[MAP_BWBMEGAROCKET] = true;	// small hack
		}
		ck_keenObj->zLayer = 1;
		ck_keenObj->xDirection= ck_keenObj->yDirection = motion_None;
		ck_keenObj->user1 = dir_South;	// K1n9_Duk3 fix: using enum name
		ck_keenObj->user2 = 3;
		ck_keenObj->user3 = 0;
		ck_keenObj->gfxChunk = WORLDKEEND3SPR;
		CK_SetAction(ck_keenObj, &CK_ACT_MapKeenStart);
	}
}

void CK_MapShipThink(CK_object *ob)	// K1n9_Duk3 addition
{
	if (ob->type == CT_Player)
	{
		//
		// ship has reached the target position, now move it down
		// to let it land and then wait 1 second before entering
		// the next level
		//
		if (ob->posY < ob->user2 + 20*PIXGLOBAL)	// (4)
		{
			ck_nextY = tics * 6;	// move down (2)
		}
		else if ((ob->actionTimer += tics) > 1*TickBase)
		{
			ck_gameState.mapPosX = ob->posX;
			ck_gameState.mapPosY = ob->posY;
			ck_mapState.nextLevel = MAP_BWBMEGAROCKET;
			ck_levelState = LS_Landed;
			SD_PlaySound(ENTERLEVELSND);
		}
	}
	else if (ob->posX == ob->user1 && ob->posY == ob->user2)
	{
		ob->type = CT_Player;
	}
	else
	{
		int xdist, ydist, vx, vy;
		int dist, speed;

		// calculate distance from target:
		xdist = ob->user1 - ob->posX;
		ydist = ob->user2 - ob->posY;
		vx = abs(xdist);
		vy = abs(ydist);
		dist = max(vx, vy);

		// set base speed based on distance
		if (dist < 2*TILEGLOBAL)
		{
			speed = 8 * tics;
		}
		else if (dist < 4*TILEGLOBAL)
		{
			speed = 14 *tics;
		}
		else if (dist < 8*TILEGLOBAL)
		{
			speed = 30 * tics;
		}
		else
		{
			speed = 48 * tics;
		}

		// set movement speed for the current vector
		if (vx > vy)
		{
			long tmp = (long)ydist * (long)speed;
			ck_nextY = tmp / vx;
			ck_nextX = (xdist < 0) ? -speed : speed;
		}
		else
		{
			long tmp = (long)xdist * (long)speed;
			ck_nextX = tmp / vy;
			ck_nextY = (ydist < 0) ? -speed : speed;
		}

		// don't move past the target:
		if (ck_nextX > 0)
		{
			if (ck_nextX + ob->posX > ob->user1)
				ck_nextX = ob->user1 - ob->posX;
		}
		else
		{
			if (ck_nextX + ob->posX < ob->user1)
				ck_nextX = ob->user1 - ob->posX;
		}
		if (ck_nextY > 0)
		{
			if (ck_nextY + ob->posY > ob->user2)
				ck_nextY = ob->user2 - ob->posY;
		}
		else
		{
			if (ck_nextY + ob->posY < ob->user2)
				ck_nextY = ob->user2 - ob->posY;
		}
	}
}


//look for level entry

void CK_ScanForLevelEntry(CK_object * obj)
{

	int tx, ty;

	for (ty = obj->clipRects.tileY1; ty <= obj->clipRects.tileY2; ty++)
	{
		for (tx = obj->clipRects.tileX1; tx <= obj->clipRects.tileX2; tx++)
		{
			/*
			unsigned tile = CA_TileAtPos(tx, ty, 1);
			if (tile == (0x00 + MAP_BONUSLEVEL) && (ck_gameState.keenScore <= 1000 || ck_gameState.cheated || ck_gameState.keenScore > 8000 ))
			{	
				tile = 0x91;
			}
			*/

			unsigned infotile = CA_TileAtPos(tx, ty, 2);
			
			if ((infotile == (0xC000 + MAP_BONUSLEVEL)) && (ck_gameState.cheated || ck_gameState.keenScore < 500000 || ck_gameState.keenScore >= 800000))
		//	if ((infotile == (0xC000 + MAP_BONUSLEVEL)) && (                        ck_gameState.keenScore <    800 || ck_gameState.keenScore >=  1300))  // just for testing reasons
		//	if (ck_gameState.keenScore >= 500000 && ck_gameState.keenScore < 800000 && !ck_gameState.cheated))
			{	
			
			}
			
			else if (infotile > 0xC000 && infotile <= 0xC020)
			{
				// Vanilla keen stores the current map loaded in the cache manager
				// and the "current_map" variable stored in the gamestate
				// would have been changed here.
				ck_gameState.mapPosX = obj->posX;
				ck_gameState.mapPosY = obj->posY;
				ck_mapState.nextLevel = infotile - 0xC000;
				ck_levelState = LS_LevelComplete;	// K1n9_Duk3 fix: use enum name instead of number
				SD_PlaySound(ENTERLEVELSND);
				return;
			}
			
		}
	}
}



void CK_MapKeenStill(CK_object * obj)
{

	if (ck_inputFrame.dir != dir_None)
	{
		obj->currentAction = &CK_ACT_MapKeenWalk0;
		obj->user2 = 0;
		CK_MapKeenWalk(obj);
	}

	if (ck_keenState.jumpIsPressed || ck_keenState.pogoIsPressed || ck_keenState.shootIsPressed)
	{
		CK_ScanForLevelEntry(obj);
	}
}

// user1 = last dir;
// user2 = walking frame ticker
// user3 = 
void CK_MapKeenWalk(CK_object * obj)
{

	if (obj->user3 == 0)
	{
		obj->xDirection = ck_inputFrame.xaxis;
		obj->yDirection = ck_inputFrame.yaxis;
		if (ck_keenState.pogoIsPressed || ck_keenState.jumpIsPressed || ck_keenState.shootIsPressed)
			CK_ScanForLevelEntry(obj);

		// Go back to standing if no arrows pressed
		if (ck_inputFrame.dir == dir_None)
		{
			obj->currentAction = &CK_ACT_MapKeenStart;
			obj->gfxChunk = ck_mapKeenFrames[obj->user1] + 3;
			return;
		}
		else
		{
			obj->user1 = ck_inputFrame.dir;
		}
	}
	else
	{
		if ((obj->user3 -= 4) < 0)
			obj->user3 = 0;
	}

	// Advance Walking Frame Animation
	if (++obj->user2 == 4)
		obj->user2 = 0;
	obj->gfxChunk = ck_mapKeenFrames[obj->user1] + word_417BA[obj->user2];

	//walk hi lo sound
	if (obj->user2 == 1)
	{
		SD_PlaySound(KEENWALK1SND);
	}
	else if (obj->user2 == 3)
	{
		SD_PlaySound(KEENWALK2SND);
	}
}



void CK_MapMiscFlagsCheck(CK_object *keen)
{

	int midTileX, midTileY;
	unsigned tile;
	byte info;

	if (keen->user3)
		return;

	midTileX = keen->clipRects.tileXmid;
	midTileY = ((keen->clipRects.unitY2 - keen->clipRects.unitY1) / 2 + keen->clipRects.unitY1) >> 8;

	tile = *((unsigned _seg *)mapsegs[1] + mapbwidthtable[midTileY]/2 + midTileX);
	info = tinf[INTILE+tile];

	switch (info)
	{
		// Check for special tile properties here
		default:
			break;
	}

}

//===========================================================================
//
// MAP FLAGS
// 
//===========================================================================

/*
CK_action CK_ACT_MapFlagFlips0 ;
CK_action CK_ACT_MapFlagFlips1 ;
CK_action CK_ACT_MapFlagFlips2 ;
CK_action CK_ACT_MapFlagFlips3 ;
CK_action CK_ACT_MapFlagFlips4 ;
CK_action CK_ACT_MapFlagFlips5 ;
CK_action CK_ACT_MapFlagFlips6 ;
*/


CK_action CK_ACT_MapFlag0 ;
CK_action CK_ACT_MapFlag1 ;
CK_action CK_ACT_MapFlag2 ;
CK_action CK_ACT_MapFlag3 ;

void CK_MapFlagSpawn(unsigned tileX, unsigned tileY, unsigned spriteoff);
/*
void CK_SpawnFlippingFlag(unsigned tileX, unsigned tileY, unsigned spriteoff);
void CK_KeenFlagThrown (CK_object *obj);
void CK_KeenFlagSearch (CK_object *obj);
void CK_KeenFlagLands(CK_object *obj);
*/
void CK_KeenFlagReact(CK_object *obj);

/*
CK_action CK_ACT_MapFlagFlips0 = { FLAGFLIP1SPR, FLAGFLIP1SPR, AT_Frame, 0, 0, 6, 0, 0, CK_KeenFlagThrown, NULL, CK_KeenFlagReact, &CK_ACT_MapFlagFlips1};
CK_action CK_ACT_MapFlagFlips1 = { FLAGFLIP1SPR, FLAGFLIP1SPR, AT_UnscaledFrame, 0, 0, 12, 0, 0, CK_KeenFlagSearch, NULL, CK_KeenFlagReact, &CK_ACT_MapFlagFlips2};
CK_action CK_ACT_MapFlagFlips2 = { FLAGFLIP2SPR, FLAGFLIP2SPR, AT_UnscaledFrame, 0, 0, 12, 0, 0, CK_KeenFlagSearch, NULL, CK_KeenFlagReact, &CK_ACT_MapFlagFlips3};
CK_action CK_ACT_MapFlagFlips3 = { FLAGFLIP3SPR, FLAGFLIP3SPR, AT_UnscaledFrame, 0, 0, 12, 0, 0, CK_KeenFlagSearch, NULL, CK_KeenFlagReact, &CK_ACT_MapFlagFlips4};
CK_action CK_ACT_MapFlagFlips4 = { FLAGFLIP4SPR, FLAGFLIP4SPR, AT_UnscaledFrame, 0, 0, 12, 0, 0, CK_KeenFlagSearch, NULL, CK_KeenFlagReact, &CK_ACT_MapFlagFlips5};
CK_action CK_ACT_MapFlagFlips5 = { FLAGFLIP6SPR, FLAGFLIP6SPR, AT_UnscaledFrame, 0, 0, 12, 0, 0, CK_KeenFlagSearch, NULL, CK_KeenFlagReact, &CK_ACT_MapFlagFlips6};
CK_action CK_ACT_MapFlagFlips6 = { FLAGFLIP6SPR, FLAGFLIP6SPR, AT_UnscaledFrame, true, 0, 1, 0, 0, CK_KeenFlagLands, NULL, CK_KeenFlagReact, &CK_ACT_MapFlag0};
*/

//void CK_SpawnRisingFlag(unsigned tileX, unsigned tileY, unsigned spriteoff);
void CK_KeenFlagRise(CK_object *ob);
CK_action CK_ACT_MapFlagRise = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CK_KeenFlagRise, NULL, CK_KeenFlagReact, NULL};

CK_action CK_ACT_MapFlag0 = { FLAGWAVE1SPR, FLAGWAVE1SPR, AT_UnscaledOnce, 0x0, 0x0, 0xa, 0x0, 0x0, NULL, NULL, CK_KeenFlagReact, &CK_ACT_MapFlag1};
CK_action CK_ACT_MapFlag1 = { FLAGWAVE2SPR, FLAGWAVE2SPR, AT_UnscaledOnce, 0x0, 0x0, 0xa, 0x0, 0x0, NULL, NULL, CK_KeenFlagReact, &CK_ACT_MapFlag2};
CK_action CK_ACT_MapFlag2 = { FLAGWAVE3SPR, FLAGWAVE3SPR, AT_UnscaledOnce, 0x0, 0x0, 0xa, 0x0, 0x0, NULL, NULL, CK_KeenFlagReact, &CK_ACT_MapFlag3};
CK_action CK_ACT_MapFlag3 = { FLAGWAVE4SPR, FLAGWAVE4SPR, AT_UnscaledOnce, 0x0, 0x0, 0xa, 0x0, 0x0, NULL, NULL, CK_KeenFlagReact, &CK_ACT_MapFlag0};

void CK_MapFlagSpawn(unsigned tileX, unsigned tileY, unsigned spriteoff)
{

	CK_object *flag = CK_GetNewObj(false);

	flag->clipped = CLIP_not;
	flag->zLayer = FOREGROUNDPRIORITY;
	flag->type = CT_MapFlag;
	//flag->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	flag->posX = (tileX << 8) + 0x60;
	flag->posY = (tileY << 8) - 0x1E0;
	flag->actionTimer = US_RndT() / 16;

	flag->user4 = spriteoff;

	CK_SetAction(flag, &CK_ACT_MapFlag0);
}

void CK_SpawnRisingFlag(unsigned tileX, unsigned tileY, unsigned spriteoff)
{
	CK_object *flag = CK_GetNewObj(false);

	flag->clipped = CLIP_not;
	flag->zLayer = FOREGROUNDPRIORITY;
	flag->type = CT_MapFlag;
	flag->posX = (tileX << 8) + 0x60;
	flag->posY = (tileY << 8) - 0x1E0;
	flag->user4 = spriteoff;
	flag->gfxChunk = FLAGFLIP1SPR;
	CK_SetAction(flag, &CK_ACT_MapFlagRise);
}

void CK_KeenFlagRise(CK_object *ob)
{
	if (!screenfaded)
	{
		if(!ob->user1)
		{
			ob->user1++;
			SD_PlaySound(FLAGSPINSND);
		}
		if ((ob->actionTimer += tics) >= 6)
		{
			ob->actionTimer -= 6;
			if (++ob->gfxChunk == FLAGWAVE1SPR)
			{
				ob->currentAction = &CK_ACT_MapFlag0;
				SD_PlaySound(FLAGLANDSND);
			}
		}
	}
}

/*
typedef struct {
	unsigned x, y;
} flagpointtype;

flagpointtype flagpoints[30];

void CK_SpawnFlippingFlag(unsigned tileX, unsigned tileY, unsigned spriteoff)
{

	CK_object *newobj;
	long deltaX, deltaY;
	int i;

	newobj = CK_GetNewObj(false);
	newobj->clipped = CLIP_not;
	newobj->zLayer = FOREGROUNDPRIORITY;
	newobj->type = CT_MapFlag;
	newobj->active = OBJ_ALWAYS_ACTIVE;
	newobj->posX = ck_gameState.mapPosX - 0x100;
	newobj->posY = ck_gameState.mapPosY - 0x100;

	newobj->user4 = spriteoff;

	// Destination coordinates
	newobj->user1 = (tileX<<8) + 0x60;
	newobj->user2 = (tileY<<8) - 0x260;
	
	deltaX = (int)newobj->user1 - (int)newobj->posX;
	deltaY = (int)newobj->user2 - (int)newobj->posY;

	// Make a table of coordinates for the flag's path
	for (i = 0; i < 30; i++)
	{

		// Draw points in a straight line between keen and the holster
		flagpoints[i].x = newobj->posX + deltaX*(i<24?i:24)/24;
		flagpoints[i].y = newobj->posY + deltaY*i/30;

		// Offset the Y points to mimic a parabolic trajectory
		if (i < 10)
			flagpoints[i].y -= i * 0x30;
		else if (i < 15) // starts to level
			flagpoints[i].y -= i * 16 + 0x140;
		else if (i < 20) // flag starts falling
			flagpoints[i].y -= (20 - i) * 16 + 0x1E0;
		else 		// starts falling faster
			flagpoints[i].y -= (29 - i) * 0x30;
	}

	CK_SetAction(newobj, &CK_ACT_MapFlagFlips0);
}



void CK_KeenFlagThrown (CK_object *obj)
{
	// Start the throw when screen becomes light
	if (!screenfaded)
	{
		SD_StopSound();
		SD_PlaySound(FLAGSPINSND);
		obj->currentAction = obj->currentAction->next;
	}
}


void CK_KeenFlagSearch (CK_object *obj)
{
	obj->user3 += tics;

	if (obj->user3 > 58)
		obj->user3 = 58;

	// Move the flag along the path
	obj->posX = flagpoints[obj->user3/2].x;
	obj->posY = flagpoints[obj->user3/2].y;

	obj->visible = true;
	if (!obj->user1)
		SD_PlaySound(FLAGSPINSND);
}

void CK_KeenFlagLands(CK_object *obj)
{

	// Put the flag in its holster
	obj->posX = obj->user1;
	obj->posY = obj->user2 + 0x80;

	SD_PlaySound(FLAGLANDSND);
}
*/

void CK_KeenFlagReact(CK_object *obj)
{
	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk+obj->user4, spritedraw, obj->zLayer);
}
