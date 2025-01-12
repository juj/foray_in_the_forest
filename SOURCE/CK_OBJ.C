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

// CK_OBJ.C

#include "CK_DEF.H"
#pragma	hdrstop

// This file contains some object functions (think, etc) which are common to
// several episodes.

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

CK_action CK_act_item;
//CK_action CK_act_item2;	// K1n9_Duk3 mod: we don't actually need this

CK_action CK_ACT_VitalinNotify1;
CK_action CK_ACT_VitalinNotify2;
CK_action CK_ACT_VitalinNotify3;
CK_action CK_ACT_StandPlatform;
CK_action CK_act_AxisPlatform;
CK_action CK_ACT_FallPlat0;
CK_action CK_ACT_FallPlat1;
CK_action CK_ACT_FallPlat2;


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
void CK_DoorOpen(CK_object *obj);
void CK_SecurityDoorOpen(CK_object *obj);
void CK_PointItem(CK_object *obj);
void CK_SpawnStandPlatform(int tileX, int tileY);
void CK_SpawnAxisPlatform(int tileX, int tileY, int direction);
void CK_AxisPlatform(CK_object *obj);
void CK_SpawnFallPlat(int tileX, int tileY);
void CK_FallPlatSit (CK_object *obj);
void CK_FallPlatFall (CK_object *obj);
void CK_FallPlatRise (CK_object *obj);
/*
=============================================================================

LOCAL VARIABLES

=============================================================================
*/


//===========================================================================
//
// GENERIC ACTOR BEHAVIOURS
// 
//===========================================================================

void near TurnX(CK_object *ob)
{
	ob->xDirection = -ob->xDirection;
	ob->posX -= ob->deltaPosX;
	ob->posY -= ob->deltaPosY;
	CK_SetAction2(ob, ob->currentAction);
}

void near TurnY(CK_object *ob)
{
	ob->yDirection = -ob->yDirection;
	ob->posX -= ob->deltaPosX;
	ob->posY -= ob->deltaPosY;
	CK_SetAction2(ob, ob->currentAction);
}

// Think function for adding gravity

void CK_Fall(CK_object *obj)
{
	// If we have a movement speed, the think function was already executed
	// for this frame and the animation (state change) caused the game to
	// execute it again. In that case, don't do anything.
	if (ck_nextX || ck_nextY)
		return;

	CK_PhysGravityHigh(obj);
	ck_nextX = obj->velX * tics;
}

// Think function for adding a slightly lower amount of gravity

void CK_Fall2(CK_object *obj)
{
	// If we have a movement speed, the think function was already executed
	// for this frame and the animation (state change) caused the game to
	// execute it again. In that case, don't do anything.
	if (ck_nextX || ck_nextY)
		return;

	CK_PhysGravityMid(obj);
	ck_nextX = obj->velX * tics;
}

void CK_Glide(CK_object *obj)
{
	// If we have a movement speed, the think function was already executed
	// for this frame and the animation (state change) caused the game to
	// execute it again. In that case, don't do anything.
	if (ck_nextX || ck_nextY)
		return;

	ck_nextX = obj->velX * tics;
	ck_nextY = obj->velY * tics;
}

void CK_ObjBadState (void) {
	//Quit("Object with bad state!");
	SM_Quit(S_BADSTATE);
}

void CK_DeadlyCol(CK_object *o1, CK_object *o2)
{
	if (o2->type == CT_Stunner)
	{
		CK_ShotHit(o2);
	}
	else if (o2->type == CT_Player)
	{
		CK_KillKeen();
	}
}

void CK_DeadlyCol2(CK_object *o1, CK_object *o2)
{
	if (o2->type == CT_Player)
	{
		CK_KillKeen();
	}
}


void CK_BasicDrawFunc1(CK_object *obj)
{
	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}

//
// For walking and turning around at edges 
//
void CK_BasicDrawFunc2(CK_object *obj)
{
	// K1n9_Duk3 mod: merged all checks into one if-statement
	if ( (obj->xDirection == motion_Right && obj->leftTI != 0)
	  || (obj->xDirection == motion_Left && obj->rightTI != 0)
	  || (obj->topTI == 0) )
	{
		obj->posX -= obj->deltaPosX;
		obj->posY -= obj->deltaPosY;
		obj->xDirection = -obj->xDirection;
		obj->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(obj, obj->currentAction);
	}

	RF_PlaceSprite(&(obj->sde), obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}

//
// For slugs  (and maybe others???)
//
void CK_BasicDrawFunc3(CK_object *obj)
{
	// K1n9_Duk3 mod: merged all checks into one if-statement
	if ( (obj->xDirection == motion_Right && obj->leftTI != 0)
	  || (obj->xDirection == motion_Left && obj->rightTI != 0)
	  || (obj->topTI == 0 || (obj->topTI & ~SLOPEMASK)) )
	{
		obj->posX -= obj->deltaPosX;
		obj->posY -= obj->deltaPosY;
		obj->xDirection = -obj->xDirection;
		obj->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(obj, obj->currentAction);
	}

	RF_PlaceSprite(&(obj->sde), obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}

void CK_FallReact(CK_object *obj)
{
	// Handle physics
	if (obj->leftTI || obj->rightTI)
	{
		obj->velX = 0;
	}

	if (obj->bottomTI)
	{
		obj->velY = 0;
	}

	if (obj->topTI)
	{
		obj->velX = obj->velY = 0;
		if (obj->currentAction->next)
		{
			CK_SetAction2(obj, obj->currentAction->next);
		}
	}
	CK_BasicDrawFunc1(obj);
}

//
// Think function for stunned creatures
//
void CK_BasicDrawFunc4(CK_object *obj)
{
	int starsX, starsY;

	CK_FallReact(obj);
/*
	// Handle physics
	if (obj->leftTI || obj->rightTI)
	{
		obj->velX = 0;
	}

	if (obj->bottomTI)
	{
		obj->velY = 0;
	}

	if (obj->topTI)
	{
		obj->velX = obj->velY = 0;
		if (obj->currentAction->next)
		{
			CK_SetAction2(obj, obj->currentAction->next);
		}
	}

	// Draw the primary chunk
	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
*/


	// Draw the stunner stars, offset based on the initial type of the stunned
	// critter

	starsX = starsY = 0;

	switch (obj->user4)
	{
		// Enemy specific star-offsets
		case CT_Slug:
			starsY -= 8*PIXGLOBAL;
			break;
		case CT_Wormouth:
			starsX = 4*PIXGLOBAL;
			starsY = -350;	// a bit weird, but this is the value used in KEEN 4
			break;
		case CT_XkyLick:
			starsY = -16*PIXGLOBAL;
			break;
		case CT_Bird:
			starsX = 12*PIXGLOBAL;
			starsY = -8*PIXGLOBAL;
			break;
	}

	// Tick the star 3-frame animation forward
	if ((obj->user1 += tics) > 10)
	{
		obj->user1 -= 10;
		if (++obj->user2 >= 3)
			obj->user2 = 0;
	}

	// FIXME: Will cause problems on 64-bit systems
	RF_PlaceSprite((void **)&obj->user3, obj->posX + starsX, obj->posY + starsY, obj->user2 + STUNNEDSTARS1SPR, spritedraw, 3);
}

void CK_StunCreature(CK_object *creature, CK_object *stunner, CK_action *new_creature_act)
{
	// Kill the stunner shot	(K1n9_Duk3 fix: only when it's not a NULL pointer)
	if (stunner)
		CK_ShotHit(stunner);

	CK_ObjDropNisasi(creature);

	// Set stunned creature action
	creature->user1 = creature->user2 = creature->user3 = 0;
	creature->user4 = creature->type;
	CK_SetAction2(creature, new_creature_act);
	creature->type = CT_StunnedCreature;

	// Make the creature jump up a bit
	if ((creature->velY -= 0x18) < -0x30)
		creature->velY = -0x30;

	// K1n9_Duk3 fix:
	creature->timeUntillThink = 0;	// allow it to jump without delay
}

//===========================================================================
//
// DOORS
// 
//===========================================================================

CK_action CK_ACT_DoorOpen1 = {0, 0, AT_UnscaledOnce, 0, 0, 10, 0, 0, CK_DoorOpen, NULL, NULL, &CK_ACT_DoorOpen2};
CK_action CK_ACT_DoorOpen2 = {0, 0, AT_UnscaledOnce, 0, 0, 10, 0, 0, CK_DoorOpen, NULL, NULL, &CK_ACT_DoorOpen3};
CK_action CK_ACT_DoorOpen3 = {0, 0, AT_UnscaledOnce, 0, 0, 10, 0, 0, CK_DoorOpen, NULL, NULL, NULL};

void CK_DoorOpen(CK_object *obj)
{
	int i;
	unsigned tilesToReplace[0x30];

	if (obj->user1 > 0x30)
	{
		//Quit("Door too tall!");
		SM_Quit(S_DOORTOOTALL);
	}

	for (i = 0; i < obj->user1; ++i)
	{
		tilesToReplace[i] = CA_TileAtPos(obj->posX, obj->posY+i, 1) + obj->user2;	// K1n9_Duk3 mod: + user2 instead of + 1 (to support closing doors)
	}

	RF_MemToMap(tilesToReplace, 1, obj->posX, obj->posY, 1, obj->user1);
}
// patch: wait for doors to be completle opened/close before releasing the door switch
boolean CK_DoorsActive(void)
{
	CK_object *ob;
	
	for (ob=ck_keenObj; ob; ob=ob->next)
	{
		if (ob->currentAction && ob->currentAction->think == CK_DoorOpen)
			return true;
	}
	return false;
}

void CK_SecurityDoorOpen(CK_object *obj)
{
	int x, y;
	unsigned tilesToReplace[0x30];
	for (y = 0; y < 4; ++y)
	{
		for (x = 0; x < 4; ++x)
		{
			tilesToReplace[y*4+x] = CA_TileAtPos(obj->posX+x, obj->posY+y, 1) - 4;
		}
	}

	RF_MemToMap(tilesToReplace, 1, obj->posX, obj->posY, 4, 4);
	obj->user1++;
	if (obj->user1 == 3)
	{
		obj->currentAction = 0;
	}
}

//===========================================================================
//
// ITEMS 
// 
//===========================================================================

CK_action CK_act_item = {0, 0, AT_UnscaledOnce, 0, 0, 20, 0, 0, CK_PointItem, NULL, CK_BasicDrawFunc1, &CK_act_item};
//CK_action CK_act_item2 = {0, 0, AT_UnscaledOnce, 0, 0, 20, 0, 0, CK_PointItem, NULL, CK_BasicDrawFunc1, &CK_act_item};	// K1n9_Duk3 mod: we don't actually need this

CK_action CK_ACT_VitalinNotify1 = {VITALINBURST1SPR, VITALINBURST1SPR, AT_UnscaledOnce, 0, 0, 8, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_VitalinNotify2};
CK_action CK_ACT_VitalinNotify2 = {VITALINBURST2SPR, VITALINBURST2SPR, AT_UnscaledOnce, 0, 0, 8, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_VitalinNotify3};
CK_action CK_ACT_VitalinNotify3 = {VITALINBURST3SPR, VITALINBURST3SPR, AT_UnscaledOnce, 0, 0, 8, 0, 0, NULL, NULL, CK_BasicDrawFunc1, NULL};

//StartSprites + 
unsigned CK_ItemSpriteChunks[] ={
	REDGEM1SPR, YELLOWGEM1SPR, BLUEGEM1SPR, GREENGEM1SPR,
	POINTS100ITEM1SPR, POINTS200ITEM1SPR, POINTS500ITEM1SPR, POINTS1000ITEM1SPR, POINTS2000ITEM1SPR, POINTS5000ITEM1SPR,
	ITEM3UP1SPR, ITEMSTUNNER1SPR
};

CK_object *CK_SpawnItem(int tileX, int tileY, int itemNumber)
{

	CK_object *obj = CK_GetNewObj(false);

	obj->clipped = CLIP_not;
	//obj->active = OBJ_ACTIVE;
	obj->zLayer = 2;
	obj->type = CT_Item;	
	obj->posX = tileX << 8;
	obj->posY = tileY << 8;
	obj->yDirection = -1;
	obj->user1 = itemNumber;
	obj->gfxChunk = CK_ItemSpriteChunks[itemNumber];
	obj->user2 = obj->gfxChunk;
	obj->user3 = obj->gfxChunk + 2;
	CK_SetAction(obj, &CK_act_item);

	// FITF hack: item number 6 (500 points) doesn't animate
	if (itemNumber == 6)
		obj->user3--;

	/*
	if (itemNumber == 10)
	{
		ck_mapState.livesTotal++;
	}
	else if (itemNumber == 11)
	{
		ck_mapState.ammoTotal++;
	}
	else*/ if (CK5_ItemPoints[itemNumber])
	{
		//ck_mapState.pointsTotal += CK5_ItemPoints[itemNumber];
		ck_mapState.itemsTotal++;
	}

	// TODO: Wrong place to cache?
	/*
	CA_CacheGrChunk(obj->gfxChunk);
	CA_CacheGrChunk(obj->gfxChunk + 1);
	*/

	return obj;
}
void CK_PointItem(CK_object *obj)
{
	//	obj->timeUntillThink = 20;
	obj->visible = true;
	if (++obj->gfxChunk == obj->user3)
		obj->gfxChunk = obj->user2;
}

//===========================================================================
//
// PLATFORMS
// 
//===========================================================================

CK_action CK_ACT_StandPlatform = {PLATFORMSPR, PLATFORMSPR, AT_UnscaledOnce, 0, 0, 32000, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_StandPlatform};


void CK_SpawnStandPlatform(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);

	obj->type = CT_Platform;
	//obj->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	obj->zLayer = 0;
	obj->posX = tileX << 8;
	obj->posY = obj->user1 = tileY << 8;
	obj->xDirection = 0;
	obj->yDirection = 1;
	obj->clipped = CLIP_not;
	CK_SetAction(obj, &CK_ACT_StandPlatform);
	//obj->gfxChunk = obj->currentAction->chunkLeft;
	CK_ResetClipRects(obj);
}

CK_action CK_act_AxisPlatform = {PLATFORMSPR, PLATFORMSPR, AT_Frame, 0, 0, 0, 0, 0, CK_AxisPlatform, NULL, CK_PlatformDraw, NULL };

void CK_SpawnAxisPlatform(int tileX, int tileY, int direction)
{
	CK_object *obj = CK_GetNewObj(false);

	obj->type = CT_Platform;
	obj->active = OBJ_ALWAYS_ACTIVE;
	obj->zLayer = 0;
	obj->posX = tileX << 8;
	obj->posY = tileY << 8;

	switch (direction)
	{
	case 0:
		obj->xDirection = 0;
		obj->yDirection = -1;
		break;
	case 1:
		obj->xDirection = 1;
		obj->yDirection = 0;
		break;
	case 2:
		obj->xDirection = 0;
		obj->yDirection = 1;
		break;
	case 3:
		obj->xDirection = -1;
		obj->yDirection = 0;
		break;
	}

	CK_SetAction(obj, &CK_act_AxisPlatform);
	//obj->gfxChunk = obj->currentAction->chunkLeft;

	CK_ResetClipRects(obj);
}



void CK_AxisPlatform(CK_object *obj)
{
	unsigned nextPosUnit, nextPosTile;

	if (ck_nextX || ck_nextY)
	{
		return;
	}
	//TODO: Implement properly.
	ck_nextX = obj->xDirection * 12 * tics;
	ck_nextY = obj->yDirection * 12 * tics;

	if (obj->xDirection == 1)
	{
		nextPosUnit = obj->clipRects.unitX2 + ck_nextX;
		nextPosTile = nextPosUnit >> 8;
		if (obj->clipRects.tileX2 != nextPosTile && 
			*(mapsegs[2] + (mapwidth * obj->clipRects.tileY1 + nextPosTile)) == PLATFORMBLOCK)
		{
			obj->xDirection = -1;
			//TODO: Change DeltaVelocity
			ck_nextX -= (nextPosUnit & 255);
		}
	}
	else if (obj->xDirection == -1)
	{
		nextPosUnit = obj->clipRects.unitX1 + ck_nextX;
		nextPosTile = nextPosUnit >> 8;
		if (obj->clipRects.tileX1 != nextPosTile && 
			*(mapsegs[2] + (mapwidth * obj->clipRects.tileY1 + nextPosTile)) == PLATFORMBLOCK)
		{
			obj->xDirection = 1;
			//TODO: Change DeltaVelocity
			//CK_PhysUpdateX(obj, 256 - nextPosUnit&255);
			ck_nextX += (256 - nextPosUnit) & 255;
		}
	}
	else if (obj->yDirection == 1)
	{
		nextPosUnit = obj->clipRects.unitY2 + ck_nextY;
		nextPosTile = nextPosUnit >> 8;
		if (obj->clipRects.tileY2 != nextPosTile && 
			*(mapsegs[2] + (mapwidth * nextPosTile + obj->clipRects.tileX1)) == PLATFORMBLOCK)
		{
			if (CA_TileAtPos(obj->clipRects.tileX1, nextPosTile - 2, 2) == PLATFORMBLOCK)
			{
				//Stop the platform.
				obj->visible = true;
				ck_nextY = 0;
			}
			else
			{
				obj->yDirection = -1;
				//TODO: Change DeltaVelocity
				ck_nextY -= ( nextPosUnit & 255);
			}
		}
	}
	else if (obj->yDirection == -1)
	{
		nextPosUnit = obj->clipRects.unitY1 + ck_nextY;
		nextPosTile = nextPosUnit >> 8;
		if (obj->clipRects.tileY1 != nextPosTile && 
			*(mapsegs[2] + (mapwidth * nextPosTile + obj->clipRects.tileX1)) == PLATFORMBLOCK)
		{
			if (CA_TileAtPos(obj->clipRects.tileX1, nextPosTile + 2, 2) == PLATFORMBLOCK)
			{
				// Stop the platform.
				obj->visible = true;
				ck_nextY = 0;
			}
			else
			{
				obj->yDirection = 1;
				//TODO: Change DeltaVelocity
				ck_nextY += 256 - (nextPosUnit & 255);
			}
		}
	}
}

void CK_PlatformDraw(CK_object *ob)
{
	unsigned frame;

	//place platform sprite:
	RF_PlaceSprite(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, spritedraw, ob->zLayer);
	
	//place (or remove) thruster sprites:
	frame = (((unsigned)lasttimecount) >> 2) & 1;
	if (ob->xDirection == 1)
	{
		RF_PlaceSprite((void**)&ob->user2, ob->posX-1*PIXGLOBAL, ob->posY+3*PIXGLOBAL, frame+PLATSIDETHRUST1SPR, spritedraw, 0);
		if (ob->user3)
			RF_RemoveSprite((void**)&ob->user3);
	}
	else if (ob->xDirection == -1)
	{
		if (ob->user2)
			RF_RemoveSprite((void**)&ob->user2);
		RF_PlaceSprite((void**)&ob->user3, ob->posX+48*PIXGLOBAL, ob->posY+5*PIXGLOBAL, frame+PLATSIDETHRUST1SPR, spritedraw, 1);
	}
	else if (ob->yDirection == -1)
	{
		RF_PlaceSprite((void**)&ob->user2, ob->posX+2*PIXGLOBAL, ob->posY+9*PIXGLOBAL, frame+PLATLTHRUST1SPR, spritedraw, 0);
		RF_PlaceSprite((void**)&ob->user3, ob->posX+46*PIXGLOBAL, ob->posY+8*PIXGLOBAL, frame+PLATRTHRUST1SPR, spritedraw, 0);
	}
	else if (ob->yDirection == 1)
	{
		if (frame)
		{
			RF_PlaceSprite((void**)&ob->user2, ob->posX+2*PIXGLOBAL, ob->posY+9*PIXGLOBAL, frame+PLATLTHRUST1SPR, spritedraw, 0);
			RF_PlaceSprite((void**)&ob->user3, ob->posX+46*PIXGLOBAL, ob->posY+8*PIXGLOBAL, frame+PLATRTHRUST1SPR, spritedraw, 0);
		}
		else
		{
			if (ob->user2)
				RF_RemoveSprite((void**)&ob->user2);
			if (ob->user3)
				RF_RemoveSprite((void**)&ob->user3);
		}
	}
}

CK_action CK_ACT_FallPlat0 = {PLATFORMSPR, PLATFORMSPR, AT_Frame, 0x0, 0x0, 0x0, 0x0, 0x0, CK_FallPlatSit, NULL, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_FallPlat1 = {PLATFORMSPR, PLATFORMSPR, AT_Frame, 0x0, 0x0, 0x0, 0x0, 0x0, CK_FallPlatFall, NULL, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_FallPlat2 = {PLATFORMSPR, PLATFORMSPR, AT_ScaledFrame, 0x0, 0x0, 0x0, 0x0, -32, CK_FallPlatRise, NULL, CK_BasicDrawFunc1, NULL};


void CK_SpawnFallPlat(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = CT_Platform;
	new_object->active = OBJ_ALWAYS_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->user1 = new_object->posY = tileY << 8;
	new_object->xDirection = motion_None;
	new_object->yDirection = motion_Down;
	new_object->clipped = CLIP_not;
	CK_SetAction(new_object, &CK_ACT_FallPlat0);
}

void CK_FallPlatSit (CK_object *obj)
{

	if (obj == ck_keenState.platform)
	{
		ck_nextY = tics * 16;
		obj->velY = 0;
		if ((unsigned)(obj->posY + ck_nextY - obj->user1) >= 0x80)
			obj->currentAction = &CK_ACT_FallPlat1;
	}
}

void CK_FallPlatFall (CK_object *obj)
{
	unsigned newY, newYT;

	CK_PhysGravityHigh(obj);

	// K1n9_Duk3 fix: never move more than 15 pixels in one step (avoid moving through blocking tiles)
	if (ck_nextY >= 15*PIXGLOBAL)
		ck_nextY = 15*PIXGLOBAL;

	newY = obj->clipRects.unitY2 + ck_nextY;
	newYT = newY >> 8;

	// Stop falling if platform hits a block
	if ((obj->clipRects.tileY2 != newYT) && (CA_TileAtPos(obj->clipRects.tileX1, newYT, 2) == PLATFORMBLOCK))
	{
		ck_nextY = 0xFF - (obj->clipRects.unitY2 & 0xFF);
		if (ck_keenState.platform != obj)
			obj->currentAction = &CK_ACT_FallPlat2;
	}
}

void CK_FallPlatRise (CK_object *obj)
{
	if (ck_keenState.platform == obj)
	{
		obj->velY = 0;
		obj->currentAction = &CK_ACT_FallPlat1;
	}
	else if ((unsigned) obj->posY <= (unsigned) obj->user1)
	{
		ck_nextY = obj->user1 - obj->posY;
		obj->currentAction = &CK_ACT_FallPlat0;
	}
}

//===========================================================================

/*
##############################################################################

	FITF enemies

##############################################################################
*/

/*
==============================================================================
	SLUGS

	user1 - 0 for regular slugs, non-zero for donut-carrying slug
	user2 - 
	user3 - sprite pointer for donut sprite
	user4 - Nisasi pointer
==============================================================================
*/

CK_action CK_ACT_SlugMove0;
CK_action CK_ACT_SlugMove1;
CK_action CK_ACT_SlugSliming0;
CK_action CK_ACT_SlugStunned0;
CK_action CK_ACT_SlugStunned1;
CK_action CK_ACT_SlugSlime0;
CK_action CK_ACT_SlugSlimeEvap0;

void CK_SpawnSlug(int tileX, int tileY);
void CK_SlugMove(CK_object *obj);
void CK_SlugSlime(CK_object *obj);
void CK_SlugCol(CK_object *a, CK_object *b);
void CK_SlugSlimeCol(CK_object *a, CK_object *b);
void CK_SlugWalkReact(CK_object *ob);

CK_action CK_ACT_SlugMove0 = {SLUGWALKL1SPR, SLUGWALKR1SPR, AT_UnscaledOnce, 0, 1, 8, 0x60, 0, NULL, CK_SlugCol, CK_SlugWalkReact, &CK_ACT_SlugMove1};
CK_action CK_ACT_SlugMove1 = {SLUGWALKL2SPR, SLUGWALKR2SPR, AT_UnscaledOnce, 0, 1, 8, 0x20, 0, CK_SlugMove, CK_SlugCol, CK_SlugWalkReact, &CK_ACT_SlugMove0};
CK_action CK_ACT_SlugSliming0 = {SLUGSLIMELSPR, SLUGSLIMERSPR, AT_UnscaledOnce, 0, 1, 60, 0x40, 0, CK_SlugSlime, CK_SlugCol, CK_SlugWalkReact, &CK_ACT_SlugMove0};
CK_action CK_ACT_SlugStunned0 = {SLUGSTUNNED1SPR, SLUGSTUNNED1SPR, AT_Frame, 0, 0, 0, 0, 0, CK_Fall, NULL, CK_BasicDrawFunc4, NULL};
CK_action CK_ACT_SlugStunned1 = {SLUGSTUNNED2SPR, SLUGSTUNNED2SPR, AT_Frame, 0, 0, 0, 0, 0, CK_Fall, NULL, CK_BasicDrawFunc4, NULL};
CK_action CK_ACT_SlugSlime0 = {SLIME1SPR, SLIME1SPR, AT_UnscaledOnce, 0, 0, 300, 0, 0, NULL, CK_SlugSlimeCol, CK_BasicDrawFunc1, &CK_ACT_SlugSlimeEvap0};
CK_action CK_ACT_SlugSlimeEvap0 = {SLIME2SPR, SLIME2SPR, AT_UnscaledOnce, 0, 0, 60, 0, 0, NULL, CK_SlugSlimeCol, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_SlugFall = {SLUGWALKL1SPR, SLUGWALKR1SPR, AT_Frame, false, push_none, 0, 0, 0, CK_Fall, CK_SlugCol, CK_FallReact, &CK_ACT_SlugMove0};


void CK_SpawnSlug(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_Slug;
	//obj->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	obj->zLayer = 2;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY) - 0x71;
	obj->xDirection = US_RndT() < 0x80 ? motion_Right : motion_Left;
	obj->yDirection = motion_Down;
	CK_SetAction(obj, &CK_ACT_SlugMove0);
}

void CK_SlugMove(CK_object *obj)
{
	if (!obj->user4)	// K1n9_Duk3 mod: don't slime with Nisasi
	{
		register int rnd;	// cannot use unsigned here!

		rnd = US_RndT();
		if (ck_gameState.difficulty == D_Hard)
			rnd -= 0x10;
		
		if (rnd < 0x10)
		{
			// start sliming:
			ck_nextX = 0;	// K1n9_Duk3 mod: don't move!
			obj->xDirection = obj->posX < ck_keenObj->posX ? motion_Right : motion_Left;
			obj->currentAction = &CK_ACT_SlugSliming0;
			SD_PlaySound(SLUGSLIMESND);
		}
	}
}

void CK_SlugSlime(CK_object *obj)
{
	CK_object *slime = CK_GetNewObj(true);
	slime->type = CT_Friendly;
	slime->active = OBJ_EXISTS_ONLY_ONSCREEN;
	slime->zLayer = 0;
	slime->posX = obj->posX;
	slime->posY = obj->clipRects.unitY2 - 0x80;
	CK_SetAction(slime, &CK_ACT_SlugSlime0);
}

void CK_SlugCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
	{
		CK_KillKeen();
	}
	else if (b->type == CT_Stunner)
	{
		// Nisaba patch: Donut-Slug: Slug carries donut item like a snail shell. 
		// If shot, slug gets stunned and the donut can be collected as a 
		// regular point item.
		if (a->user1)	// if slug is carrying donut
		{
			RF_RemoveSprite((void**)&a->user3);
			
			if (ck_freeObject)	// because CK_SpawnItem() will cause a crash when this is NULL
			{
				CK_object *donut;
			
				donut = CK_SpawnItem(a->clipRects.tileXmid, a->clipRects.tileY1, 9);
				ck_mapState.itemsTotal--;	// because CK_SpawnItem increased it
				donut->posX = a->posX + 4*PIXGLOBAL;	// TODO: adjust relative position
				donut->posY = a->posY + 4*PIXGLOBAL;	// here as well
				CK_SetAction(donut, donut->currentAction);	// set action again because position changed
			}
		}
		CK_StunCreature(a, b, US_RndT() < 0x80 ? &CK_ACT_SlugStunned0 : &CK_ACT_SlugStunned1);
		a->velY = -24;
		a->velX = a->xDirection * 8;
	}
}

void CK_SlugSlimeCol(CK_object *a, CK_object *b)
{
	
	switch (b->type)
	{
	case CT_Player:
		if (a->currentAction->next)
			CK_KillKeen();
		break;
#ifdef EXTRA_INTERACTIONS
	case CT_CouncilMember:
	case CT_CouncilMemberDone:
		CK_RemoveObj(a);
		break;
#endif
	}
	
}

void CK_SlugWalkReact(CK_object *ob)
{
	CK_BasicDrawFunc3(ob);
	// Nisaba patch: Donut-Slug: Slug carries donut item like a snail shell. 
	// If shot, slug gets stunned and the donut can be collected as a point item.
	if (ob->user1)
	{
		int xoff, yoff;
		
		switch (ob->gfxChunk)
		{
		case SLUGWALKL1SPR:
			xoff = 4*PIXGLOBAL;
			yoff = 4*PIXGLOBAL;
			break;
		case SLUGWALKL2SPR:
			xoff = 7*PIXGLOBAL;
			yoff = 5*PIXGLOBAL;
			break;
		case SLUGSLIMELSPR:
			xoff = 7*PIXGLOBAL;
			yoff = 4*PIXGLOBAL;
			break;
		case SLUGWALKR1SPR:
			xoff = -4*PIXGLOBAL;
			yoff = 4*PIXGLOBAL;
			break;
		case SLUGWALKR2SPR:
			xoff = -7*PIXGLOBAL;
			yoff = 5*PIXGLOBAL;
			break;
		default:	// just to make sure xoff and yoff are initialized...
			// no break here!
		case SLUGSLIMERSPR:
			xoff = -7*PIXGLOBAL;
			yoff = 4*PIXGLOBAL;
			break;
		}
#if 1
		// draw donut behind slug sprite:
		RF_PlaceSprite((void**)&ob->user3, ob->posX+xoff, ob->posY+yoff, POINTS5000ITEM1SPR, spritedraw, ob->zLayer);
		RF_PlaceSpriteAfter(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, spritedraw, ob->zLayer, (void**)&ob->user3);
#else
		// draw donut on top of slug sprite:
		RF_PlaceSpriteAfter((void**)&ob->user3, ob->posX+xoff, ob->posY+yoff, POINTS5000ITEM1SPR, spritedraw, ob->zLayer, &ob->sde);
#endif
	}
	if (!ob->topTI)
	{
		ob->timeUntillThink = 0;
		ob->xDirection = -ob->xDirection;
		CK_SetAction2(ob, &CK_ACT_SlugFall);
	}
}



/*
==============================================================================
	SMIRKY / TREASURE EATER
==============================================================================
*/

/*
CK_action CK_ACT_Smirky0;
CK_action CK_ACT_Smirky1;
CK_action CK_ACT_SmirkyTele0;
CK_action CK_ACT_SmirkyTele1;
CK_action CK_ACT_SmirkyTele2;
CK_action CK_ACT_SmirkyTele3;
CK_action CK_ACT_SmirkyTele4;
CK_action CK_ACT_SmirkyTele5;
CK_action CK_ACT_SmirkyTele6;
CK_action CK_ACT_SmirkyTele7;
CK_action CK_ACT_SmirkyJump0;
CK_action CK_ACT_SmirkyJump1;
CK_action CK_ACT_SmirkyJump2;
CK_action CK_ACT_SmirkyJump3;
CK_action CK_ACT_SmirkyStunned0;
CK_action CK_ACT_SmirkyStunned1;
*/

/*
CK_action CK_ACT_StolenItem0;
CK_action CK_ACT_StolenItem1;
CK_action CK_ACT_StolenItem2;
CK_action CK_ACT_StolenItem3;
*/

/*
void CK_SpawnSmirky(int tileX, int tileY);
void CK_SmirkySearch(CK_object *obj);
void CK_SmirkyTeleport(CK_object *obj);
void CK_SmirkyCol(CK_object *a, CK_object *b);
void CK_SmirkyCheckTiles(CK_object *obj);
void CK_SmirkyDraw(CK_object *obj);
*/

/*
CK_action CK_ACT_Smirky0 = {SMIRKYLOOK1SPR, SMIRKYLOOK1SPR, AT_UnscaledOnce, 0, 0, 20, 0, 0, NULL, CK_SmirkyCol, CK_BasicDrawFunc1, &CK_ACT_Smirky1};
CK_action CK_ACT_Smirky1 = {SMIRKYLOOK2SPR, SMIRKYLOOK2SPR, AT_UnscaledOnce, 0, 0, 20, 0, 0, CK_SmirkySearch, CK_SmirkyCol, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_SmirkyTele0 = {SMIRKYPOOF1SPR, SMIRKYPOOF1SPR, AT_UnscaledOnce, 0, 0, 20, 0, 0, NULL, CK_SmirkyCol, CK_BasicDrawFunc1, &CK_ACT_SmirkyTele1};
CK_action CK_ACT_SmirkyTele1 = {SMIRKYPOOF2SPR, SMIRKYPOOF2SPR, AT_UnscaledOnce, 0, 0, 20, 0, 0, NULL, CK_SmirkyCol, CK_BasicDrawFunc1, &CK_ACT_SmirkyTele2};
CK_action CK_ACT_SmirkyTele2 = {SMIRKYPOOF3SPR, SMIRKYPOOF3SPR, AT_UnscaledOnce, 0, 0, 20, 0, 0, NULL, CK_SmirkyCol, CK_BasicDrawFunc1, &CK_ACT_SmirkyTele3};
CK_action CK_ACT_SmirkyTele3 = {SMIRKYPOOF4SPR, SMIRKYPOOF4SPR, AT_UnscaledOnce, 0, 0, 20, 0, 0, CK_SmirkyTeleport, CK_SmirkyCol, CK_BasicDrawFunc1, &CK_ACT_SmirkyTele4};
CK_action CK_ACT_SmirkyTele4 = {SMIRKYPOOF4SPR, SMIRKYPOOF4SPR, AT_UnscaledOnce, 0, 0, 20, 0, 0, NULL, CK_SmirkyCol, CK_BasicDrawFunc1, &CK_ACT_SmirkyTele5};
CK_action CK_ACT_SmirkyTele5 = {SMIRKYPOOF3SPR, SMIRKYPOOF3SPR, AT_UnscaledOnce, 0, 0, 20, 0, 0, NULL, CK_SmirkyCol, CK_BasicDrawFunc1, &CK_ACT_SmirkyTele6};
CK_action CK_ACT_SmirkyTele6 = {SMIRKYPOOF2SPR, SMIRKYPOOF2SPR, AT_UnscaledOnce, 0, 0, 20, 0, 0, NULL, CK_SmirkyCol, CK_BasicDrawFunc1, &CK_ACT_SmirkyTele7};
CK_action CK_ACT_SmirkyTele7 = {SMIRKYPOOF1SPR, SMIRKYPOOF1SPR, AT_UnscaledOnce, 0, 0, 20, 0, 0, NULL, CK_SmirkyCol, CK_BasicDrawFunc1, &CK_ACT_SmirkyJump0};
CK_action CK_ACT_SmirkyJump0 = {SMIRKYHOPL1SPR, SMIRKYHOPR1SPR, AT_UnscaledFrame, 0, 0, 6, 0, 0, CK_Fall2, CK_SmirkyCol, CK_SmirkyDraw, &CK_ACT_SmirkyJump1};
CK_action CK_ACT_SmirkyJump1 = {SMIRKYHOPL2SPR, SMIRKYHOPR2SPR, AT_UnscaledFrame, 0, 0, 6, 0, 0, CK_Fall2, CK_SmirkyCol, CK_SmirkyDraw, &CK_ACT_SmirkyJump2};
CK_action CK_ACT_SmirkyJump2 = {SMIRKYHOPL3SPR, SMIRKYHOPR3SPR, AT_UnscaledFrame, 0, 0, 6, 0, 0, CK_Fall2, CK_SmirkyCol, CK_SmirkyDraw, &CK_ACT_SmirkyJump3};
CK_action CK_ACT_SmirkyJump3 = {SMIRKYHOPL2SPR, SMIRKYHOPR2SPR, AT_UnscaledFrame, 0, 0, 6, 0, 0, CK_Fall2, CK_SmirkyCol, CK_SmirkyDraw, &CK_ACT_SmirkyJump0};
CK_action CK_ACT_SmirkyStunned0 = {SMIRKYHOPL1SPR, SMIRKYHOPL1SPR, AT_Frame, 0, 0, 0, 0, 0, CK_Fall, NULL, CK_BasicDrawFunc4, &CK_ACT_SmirkyStunned1};
CK_action CK_ACT_SmirkyStunned1 = {SMIRKYSTUNNEDSPR, SMIRKYSTUNNEDSPR, AT_Frame, 0, 0, 0, 0, 0, CK_Fall, NULL, CK_BasicDrawFunc4, NULL};
*/

/*
CK_action CK_ACT_StolenItem0 = {TREASUREPOOF1SPR, TREASUREPOOF1SPR, AT_ScaledOnce, 0, 0, 10, 0, 8, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_StolenItem1};
CK_action CK_ACT_StolenItem1 = {TREASUREPOOF2SPR, TREASUREPOOF2SPR, AT_ScaledOnce, 0, 0, 10, 0, 8, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_StolenItem2};
CK_action CK_ACT_StolenItem2 = {TREASUREPOOF3SPR, TREASUREPOOF3SPR, AT_ScaledOnce, 0, 0, 10, 0, 8, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_StolenItem3};
CK_action CK_ACT_StolenItem3 = {TREASUREPOOF4SPR, TREASUREPOOF4SPR, AT_ScaledOnce, 0, 0, 10, 0, 8, NULL, NULL, CK_BasicDrawFunc1, NULL};
*/

/*
void CK_SpawnSmirky(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_Smirky;
	//obj->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	obj->zLayer = FOREGROUNDPRIORITY;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY) - 0x180;
	obj->xDirection = US_RndT() < 0x80 ? motion_Right : motion_Left;
	obj->yDirection = motion_Down;
	CK_SetAction(obj, &CK_ACT_Smirky0);
}

void CK_SmirkySearch(CK_object *obj)
{
	int x, y, i;
	CK_object *o;
	obj->currentAction = &CK_ACT_SmirkyJump0;

	// Jump for item goodies
	for (o = ck_keenObj; o; o = o->next)
	{
		if (o->type == CT_Item)
		{
			if (o->active == OBJ_ACTIVE &&
					o->clipRects.unitX2 > obj->clipRects.unitX1 &&
					o->clipRects.unitX1 < obj->clipRects.unitX2 &&
					o->clipRects.unitY2 < obj->clipRects.unitY1 &&
					o->clipRects.unitY2 > obj->clipRects.unitY1 + 0x300)
			{
				obj->velX = 0;
				obj->velY = -48;
				return;
			}
		}
	}

	// Jump for tile goodies
	// DOS Keen increments a far pointer to the foreground plane, but it is
	// semantically equivalent to calculate the offset on every iteration
	for (y = obj->clipRects.tileY1 - 3; y < obj->clipRects.tileY1; y++)
	{
		for (x = obj->clipRects.tileX1; x < obj->clipRects.tileX2 + 1; x++)
		{
			int mf = TI_ForeMisc(CA_TileAtPos(x, y, 1)) & 0x7F;
			if (mf == MF_Centilife || mf >= MF_Points100 && mf <= MF_Stunner)
			{
				obj->velX = 0;
				obj->velY = -48;
				return;
			}
		}
	}

	// Decide to teleport for some reason
	if (obj->user1 >= 2)
	{
		obj->currentAction = &CK_ACT_SmirkyTele0;
		return;
	}

	// Decide to hop to the side if there's something to land on
	for (i = 0; i < 4; i++)
	{
		int y = obj->clipRects.tileY2 - 2 + i;
		int x = obj->clipRects.tileXmid + obj->xDirection * 4;
		int tf = TI_ForeTop(CA_TileAtPos(x, y, 1));
		if (tf)
		{
			obj->velX = obj->xDirection * 20;
			obj->velY = -24;
			return;
		}
	}

	// Decide to teleport if counter hits threshold
	if (++obj->user1 == 2)
	{
		SD_PlaySound(SMIRKYTELESND);
		obj->currentAction = &CK_ACT_SmirkyTele0;
		return;
	}

	// Finally, just turn around and hop back where we came from
	obj->velX = -obj->xDirection * 20;
	obj->velY = -24;
}

void CK_SmirkyTeleport(CK_object *obj)
{
	CK_object *o;
	obj->user1 = 0;	// reset tleport timer
	for (o = ck_keenObj; o; o = o->next)
	{
		if (o->type == CT_Item && o->user1 >= 4)	// K1n9_Duk3 fix: skip key gems!
		{
			obj->posX = o->posX - 0x80;
			obj->posY = o->posY;
			CK_SetAction(obj, &CK_ACT_SmirkyTele0);
		}
	}

	// Remove smirky if all the treasure items are gone
	if (!o)
	{
		CK_RemoveObj(obj);
		return;
	}
}

void CK_SmirkyCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Item && b->user1 >= 4)	// K1n9_Duk3 fix: skip key gems!
	{
		b->type = CT_Friendly;
		b->zLayer = FOREGROUNDPRIORITY;
		CK_SetAction2(b, &CK_ACT_StolenItem0);
		SD_PlaySound(SMIRKYSTEALSND);
	}
	else if (b->type == CT_Stunner)
	{
		a->user1 = a->user2 = a->user3 = 0;
		a->user4 = a->type;
		a->type = CT_StunnedCreature;
		CK_ShotHit(b);
		CK_SetAction2(a, &CK_ACT_SmirkyStunned0);
		a->velY -= 16;
	}
}
*/

void CK_SmirkyCheckTiles(CK_object *obj)
{
	int x, y;
	for (y = obj->clipRects.tileY1; y <= obj->clipRects.tileY2; y++)
	{
		for (x = obj->clipRects.tileX1; x <= obj->clipRects.tileX2; x++)
		{
			int mf = TI_ForeMisc(CA_TileAtPos(x, y, 1)) & 0x7F;
			if (mf == MF_Centilife || mf >= MF_Points100 && mf <= MF_Stunner)
			{
				CK_object *o;
				static unsigned emptyTile = 0;	// actually this is the same address in memory as the
				// lifewater drop emptytile... suggesting that it's a global var
				RF_MemToMap(&emptyTile, 1, x, y, 1, 1);

				o = CK_GetNewObj(true);
				o->type = CT_Friendly;
				o->zLayer = FOREGROUNDPRIORITY;
				o->clipped = CLIP_not;
				o->posX = RF_TileToUnit(x);
				o->posY = RF_TileToUnit(y);
				o->active = OBJ_EXISTS_ONLY_ONSCREEN;
				/*
				CK_SetAction2(o, &CK_ACT_StolenItem0);
				*/
				SD_PlaySound(SMIRKYSTEALSND);	// K1n9_Duk3 addition (for consistency)
			}
		}
	}
}

/*
void CK_SmirkyDraw(CK_object *obj)
{
	CK_SmirkyCheckTiles(obj);

	if (obj->topTI)
		CK_SetAction2(obj, &CK_ACT_Smirky0);

	if (obj->leftTI || obj->rightTI)
	{
		obj->user1++;
		obj->xDirection = -obj->xDirection;
		obj->velX = 0;
	}

	if (obj->topTI)
		obj->velY = 0;

	RF_PlaceSprite(&(obj->sde), obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}
*/


/*
==============================================================================
	SKYPEST

	user1 - upside-down flag for landing on ceilings
	user2 - flying time (in tics)
==============================================================================
*/

#define SKYPEST_GROUNDDELAY		(3*TickBase)
#define SKYPEST_CEILINGDELAY	(ck_gameState.difficulty == D_Easy? 2*TickBase : 4*TickBase)
#define SKYPEST_MAXDELAY		(4*TickBase)
//#define SKYPEST_MAXDELAY	(max(SKYPEST_GROUNDDELAY, SKYPEST_CEILINGDELAY))

CK_action CK_ACT_SkypestFly0;
CK_action CK_ACT_SkypestFly1;
CK_action CK_ACT_SkypestSquish0;
CK_action CK_ACT_SkypestPreen0;
CK_action CK_ACT_SkypestPreen1;
CK_action CK_ACT_SkypestPreen2;
CK_action CK_ACT_SkypestPreen3;
CK_action CK_ACT_SkypestPreen4;
CK_action CK_ACT_SkypestPreen5;
CK_action CK_ACT_SkypestPreen6;
CK_action CK_ACT_SkypestPreen7;
CK_action CK_ACT_SkypestPreen8;
CK_action CK_ACT_SkypestPreen9;
CK_action CK_ACT_SkypestPreen10;
CK_action CK_ACT_SkypestPreen11;
CK_action CK_ACT_SkypestPreen12;
CK_action CK_ACT_SkypestPreen13;
CK_action CK_ACT_SkypestPreen14;
CK_action CK_ACT_SkypestPreen15;
CK_action CK_ACT_SkypestPreen16;

void CK_SpawnSkypest(int tileX, int tileY);
void CK_SkypestFly(CK_object *obj);
void CK_SkypestAirCol(CK_object *a, CK_object *b);
void CK_SkypestGroundCol(CK_object *a, CK_object *b);
void CK_SkypestTakeoff(CK_object *obj);
void CK_SkypestDraw(CK_object *obj);

void CK_SkypestGroundDraw(CK_object *obj)
{
	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, obj->user1|spritedraw, obj->zLayer);
}

CK_action CK_ACT_SkypestFly0 = {SKYPESTFLYL1SPR, SKYPESTFLYR1SPR, AT_UnscaledFrame, 1, 0, 5, 0, 0, CK_SkypestFly, CK_SkypestAirCol, CK_SkypestDraw, &CK_ACT_SkypestFly1};
CK_action CK_ACT_SkypestFly1 = {SKYPESTFLYL2SPR, SKYPESTFLYR2SPR, AT_UnscaledFrame, 1, 0, 5, 0, 0, CK_SkypestFly, CK_SkypestAirCol, CK_SkypestDraw, &CK_ACT_SkypestFly0};
CK_action CK_ACT_SkypestSquish0 = {SKYPESTSQUISHSPR, SKYPESTSQUISHSPR, AT_Frame, 0, 0, 0, 0, 0, NULL, NULL, CK_SkypestGroundDraw, &CK_ACT_SkypestSquish0};
CK_action CK_ACT_SkypestPreen0 = {SKYPESTSITSPR, SKYPESTSITSPR, AT_UnscaledOnce, 0, 0, 100, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen1};
CK_action CK_ACT_SkypestPreen1 = {SKYPESTPREEN1SPR, SKYPESTPREEN1SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen2};
CK_action CK_ACT_SkypestPreen2 = {SKYPESTPREEN2SPR, SKYPESTPREEN2SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen3};
CK_action CK_ACT_SkypestPreen3 = {SKYPESTPREEN3SPR, SKYPESTPREEN3SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen4};
CK_action CK_ACT_SkypestPreen4 = {SKYPESTPREEN4SPR, SKYPESTPREEN4SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen5};
CK_action CK_ACT_SkypestPreen5 = {SKYPESTPREEN3SPR, SKYPESTPREEN3SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen6};
CK_action CK_ACT_SkypestPreen6 = {SKYPESTPREEN2SPR, SKYPESTPREEN2SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen7};
CK_action CK_ACT_SkypestPreen7 = {SKYPESTPREEN1SPR, SKYPESTPREEN1SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen8};
CK_action CK_ACT_SkypestPreen8 = {SKYPESTSITSPR, SKYPESTSITSPR, AT_UnscaledOnce, 0, 0, 60, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen9};
CK_action CK_ACT_SkypestPreen9 = {SKYPESTPREEN5SPR, SKYPESTPREEN5SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen10};
CK_action CK_ACT_SkypestPreen10 = {SKYPESTPREEN6SPR, SKYPESTPREEN6SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen11};
CK_action CK_ACT_SkypestPreen11 = {SKYPESTPREEN7SPR, SKYPESTPREEN7SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen12};
CK_action CK_ACT_SkypestPreen12 = {SKYPESTPREEN8SPR, SKYPESTPREEN8SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen13};
CK_action CK_ACT_SkypestPreen13 = {SKYPESTPREEN7SPR, SKYPESTPREEN7SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen14};
CK_action CK_ACT_SkypestPreen14 = {SKYPESTPREEN6SPR, SKYPESTPREEN6SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen15};
CK_action CK_ACT_SkypestPreen15 = {SKYPESTPREEN5SPR, SKYPESTPREEN5SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, NULL, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestPreen16};
CK_action CK_ACT_SkypestPreen16 = {SKYPESTSITSPR, SKYPESTSITSPR, AT_UnscaledOnce, 0, 0, 100, 0, 0, CK_SkypestTakeoff, CK_SkypestGroundCol, CK_SkypestGroundDraw, &CK_ACT_SkypestFly0};

void CK_SpawnSkypest(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_Skypest;
	//obj->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	obj->zLayer = 0;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY);
	obj->xDirection = US_RndT() < 0x80 ? motion_Right : motion_Left;
	obj->yDirection = US_RndT() < 0x80 ? motion_Down : motion_Up;
	CK_SetAction(obj, &CK_ACT_SkypestFly0);
}

void CK_SkypestFly(CK_object *obj)
{
	if (ck_nextX || ck_nextY)	// if we have movement, this think function has already been executed for the current frame
		return;

	if (obj->user2 < SKYPEST_MAXDELAY)	// safety measure to avoid overflow issues
		obj->user2 += tics;

	if (US_RndT() < tics)
		obj->xDirection = -obj->xDirection;

	if (obj->yDirection == motion_Up && US_RndT() < tics)
		obj->yDirection = motion_Down;

	if (obj->yDirection == motion_Down && US_RndT() < tics * 2)
		obj->yDirection = -obj->yDirection;

	CK_PhysAccelHorz(obj, obj->xDirection, 20);
	CK_PhysAccelVert(obj, obj->yDirection, 20);
}

void CK_SkypestAirCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
	{
		CK_KillKeen();
	}
	// There's no "else" in the original Keen 4 EXE so, so let's omit it for
	// the sake of consistency, even if there's no change to behaviors
	if (b->type == CT_Stunner)
	{
		if (b->xDirection == motion_Right)
			a->velX = 20;
		else if (b->xDirection == motion_Left)
			a->velX = -20;
		else if (b->yDirection == motion_Down)
			a->velY = 20;
		else if (b->yDirection == motion_Up)
			a->velY = -20;

		CK_ShotHit(b);
	}
}

void CK_SkypestGroundCol(CK_object *a, CK_object *b)
{
	if (b->currentAction == &CK_ACT_keenPogo1 ||
			b->currentAction == &CK_ACT_keenPogo2 ||
			b->currentAction == &CK_ACT_keenPogo3)
	{
		CK_SetAction2(a, &CK_ACT_SkypestSquish0);
		SD_PlaySound(SQUISHSND);
		a->type = CT_Friendly;
	}
}

void CK_SkypestTakeoff(CK_object *obj)
{
	if (obj->user1)
	{
		obj->yDirection = motion_Down;
		obj->velY = 16;
		ck_nextY = 144;
	}
	else
	{
		obj->yDirection = motion_Up;
		obj->velY = -16;
		ck_nextY = -144;
	}
	obj->user1 = obj->user2 = 0;	// reset flying time and upside-down flag
}

void CK_SkypestDraw(CK_object *obj)
{
	if (obj->bottomTI && !obj->rightTI && !obj->leftTI)
	{
		if ((obj->bottomTI & SLOPEMASK) == 1 && obj->user2 >= SKYPEST_CEILINGDELAY	// only land on flat ceilings
			&& ck_gameState.currentLevel != 11)	// but never land on ceilings in level 11
		{
			obj->user1 = drawflipped;	//landed on ceiling
			CK_SetAction2(obj, &CK_ACT_SkypestPreen0);
			CK_SkypestGroundDraw(obj);
			return;
		}
		obj->velY = 8;
		obj->yDirection = motion_Down;
	}

	if (obj->topTI && !obj->rightTI && !obj->leftTI && (obj->topTI != TI_TOP_BREAKABLE))	// Nisaba patch: but never fuse tiles
	{
		if (obj->user2 >= SKYPEST_GROUNDDELAY)
		{
			obj->posY += 0x80;
			obj->user1 = 0;	//landed on floor
			CK_SetAction2(obj, &CK_ACT_SkypestPreen0);
		}
		else
		{
			obj->velY = -8;
			obj->yDirection = motion_Up;
		}
	}

	if (obj->leftTI)
	{
		obj->velX = 0;
		obj->xDirection = motion_Left;
	}

	if (obj->rightTI)
	{
		obj->velX = 0;
		obj->xDirection = motion_Right;
	}

	RF_PlaceSprite(&(obj->sde), obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}

#undef SKYPEST_GROUNDDELAY
#undef SKYPEST_CEILINGDELAY
#undef SKYPEST_MAXDELAY

/*
==============================================================================
	MIMROCK
==============================================================================
*/

/*
CK_action CK_ACT_Mimrock0;
CK_action CK_ACT_MimrockWalk0;
CK_action CK_ACT_MimrockWalk1;
CK_action CK_ACT_MimrockWalk2;
CK_action CK_ACT_MimrockWalk3;
CK_action CK_ACT_MimrockWalk4;
CK_action CK_ACT_MimrockWalk5;
CK_action CK_ACT_MimrockJump0;
CK_action CK_ACT_MimrockJump1;
CK_action CK_ACT_MimrockJump2;
CK_action CK_ACT_MimrockBounce0;
CK_action CK_ACT_MimrockStunned0;
CK_action CK_ACT_MimrockStunned1;

void CK_SpawnMimrock(int tileX, int tileY);
void CK_MimrockWait(CK_object *obj);
void CK_MimrockCheckJump(CK_object *obj);
void CK_MimrockCol(CK_object *a, CK_object *b);
void CK_MimrockAirCol(CK_object *a, CK_object *b);
void CK_MimrockJumpDraw(CK_object *obj);
void CK_MimrockBounceDraw(CK_object *obj);

CK_action CK_ACT_Mimrock0 = {MIMROCKSITSPR, MIMROCKSITSPR, AT_UnscaledOnce, 0, 1, 20, 0, 0, CK_MimrockWait, NULL, CK_BasicDrawFunc2, &CK_ACT_Mimrock0};
CK_action CK_ACT_MimrockWalk0 = {MIMROCKWALKL1SPR, MIMROCKWALKR1SPR, AT_UnscaledOnce, 0, 1, 6, 0x40, 0, CK_MimrockCheckJump, CK_MimrockCol, CK_BasicDrawFunc2, &CK_ACT_MimrockWalk1};
CK_action CK_ACT_MimrockWalk1 = {MIMROCKWALKL2SPR, MIMROCKWALKR2SPR, AT_UnscaledOnce, 0, 1, 6, 0x40, 0, CK_MimrockCheckJump, CK_MimrockCol, CK_BasicDrawFunc2, &CK_ACT_MimrockWalk2};
CK_action CK_ACT_MimrockWalk2 = {MIMROCKWALKL3SPR, MIMROCKWALKR3SPR, AT_UnscaledOnce, 0, 1, 6, 0x40, 0, CK_MimrockCheckJump, CK_MimrockCol, CK_BasicDrawFunc2, &CK_ACT_MimrockWalk3};
CK_action CK_ACT_MimrockWalk3 = {MIMROCKWALKL4SPR, MIMROCKWALKR4SPR, AT_UnscaledOnce, 0, 1, 6, 0x40, 0, CK_MimrockCheckJump, CK_MimrockCol, CK_BasicDrawFunc2, &CK_ACT_MimrockWalk4};
CK_action CK_ACT_MimrockWalk4 = {MIMROCKWALKL1SPR, MIMROCKWALKR1SPR, AT_UnscaledOnce, 0, 1, 6, 0x40, 0, CK_MimrockCheckJump, CK_MimrockCol, CK_BasicDrawFunc2, &CK_ACT_MimrockWalk5};
CK_action CK_ACT_MimrockWalk5 = {MIMROCKWALKL2SPR, MIMROCKWALKR2SPR, AT_UnscaledOnce, 0, 1, 6, 0x40, 0, CK_MimrockCheckJump, CK_MimrockCol, CK_BasicDrawFunc2, &CK_ACT_Mimrock0};
CK_action CK_ACT_MimrockJump0 = {MIMROCKJUMPL1SPR, MIMROCKJUMPL1SPR, AT_UnscaledFrame, 0, 0, 24, 0, 0, CK_Fall2, CK_MimrockAirCol, CK_MimrockJumpDraw, &CK_ACT_MimrockJump1};
CK_action CK_ACT_MimrockJump1 = {MIMROCKJUMPL2SPR, MIMROCKJUMPR2SPR, AT_UnscaledFrame, 0, 0, 10, 0, 0, CK_Fall2, CK_MimrockAirCol, CK_MimrockJumpDraw, &CK_ACT_MimrockJump2};
CK_action CK_ACT_MimrockJump2 = {MIMROCKJUMPL3SPR, MIMROCKJUMPR3SPR, AT_Frame, 0, 0, 10, 0, 0, CK_Fall2, CK_MimrockAirCol, CK_MimrockJumpDraw, &CK_ACT_MimrockJump1};
CK_action CK_ACT_MimrockBounce0 = {MIMROCKJUMPL3SPR, MIMROCKJUMPR3SPR, AT_Frame, 0, 0, 10, 0, 0, CK_Fall, CK_MimrockCol, CK_MimrockBounceDraw, NULL};
CK_action CK_ACT_MimrockStunned0 = {MIMROCKJUMPL3SPR, MIMROCKJUMPR3SPR, AT_Frame, 0, 0, 12, 0, 0, CK_Fall, NULL, CK_BasicDrawFunc4, &CK_ACT_MimrockStunned1};
CK_action CK_ACT_MimrockStunned1 = {MIMROCKSTUNNEDSPR, MIMROCKSTUNNEDSPR, AT_Frame, 0, 0, 0, 0, 0, CK_Fall, NULL, CK_BasicDrawFunc4, NULL}; 

void CK_SpawnMimrock(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_Mimrock;
	//obj->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	obj->zLayer = FOREGROUNDPRIORITY;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY) - 0xD0;
	obj->xDirection = motion_Right;
	obj->yDirection = motion_Down;
	CK_SetAction(obj, &CK_ACT_Mimrock0);
}

void CK_MimrockWait(CK_object *obj)
{
	if (ABS((unsigned)(obj->clipRects.unitY2 - ck_keenObj->clipRects.unitY2)) <= 0x500)
	{
		if (ABS((unsigned)(obj->posX - ck_keenObj->posX)) > 0x300)
		{
			if (ck_keenObj->posX < obj->posX)
			{
				if (ck_keenObj->xDirection == motion_Left)
				{
					obj->xDirection = motion_Left;
					obj->currentAction = &CK_ACT_MimrockWalk0;
				}
			}
			else
			{
				if (ck_keenObj->xDirection == motion_Right)
				{
					obj->xDirection = motion_Right;
					obj->currentAction = &CK_ACT_MimrockWalk0;
				}
			}
		}
	}
}

void CK_MimrockCheckJump(CK_object *obj)
{
	if ((ABS((unsigned)(obj->clipRects.unitY2 - ck_keenObj->clipRects.unitY2)) <= 0x500) &&
			(obj->xDirection == ck_keenObj->xDirection))
	{
		if (ABS((unsigned)(obj->posX - ck_keenObj->posX)) < 0x400)
		{
			obj->velX = obj->xDirection * 20;
			obj->velY = -40;
			ck_nextY = obj->velY * tics;
			obj->currentAction = &CK_ACT_MimrockJump0;
		}
	}
	else
	{
		obj->currentAction = &CK_ACT_Mimrock0;
	}
}

void CK_MimrockCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Stunner)
	{
		a->user1 = a->user2 = a->user3 = 0;
		a->user4 = a->type;
		a->type = CT_StunnedCreature;
		CK_ShotHit(b);
		CK_SetAction2(a, &CK_ACT_MimrockStunned0);
		a->velY -= 16;
	}
}

void CK_MimrockAirCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
	{
		CK_KillKeen();
	}
	else
	{
		CK_MimrockCol(a,b);
	}
}

void CK_MimrockJumpDraw(CK_object *obj)
{
	if (obj->topTI)
	{
		SD_PlaySound(SOUND_MIMROCKJUMP);
		obj->velY = -20;
		CK_SetAction2(obj, &CK_ACT_MimrockBounce0);
	}

	if (obj->leftTI || obj->topTI)
		obj->velX = 0;

	if (obj->topTI || obj->bottomTI)
		obj->velY = 0;

	RF_PlaceSprite(&(obj->sde), obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}

void CK_MimrockBounceDraw(CK_object *obj)
{
	if (obj->topTI)
	{
		SD_PlaySound(SOUND_MIMROCKJUMP);
		CK_SetAction2(obj, &CK_ACT_Mimrock0);
	}

	if (obj->leftTI || obj->topTI)
		obj->velX = 0;

	if (obj->topTI)
		obj->velY = 0;

	RF_PlaceSprite(&(obj->sde), obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}
*/

/*
==============================================================================
	PatPats
==============================================================================
*/

CK_action CK_ACT_PatPatD0;
CK_action CK_ACT_PatPatD1;
CK_action CK_ACT_PatPatD2;
CK_action CK_ACT_PatPatU0;
CK_action CK_ACT_PatPatU1;
CK_action CK_ACT_PatPatU2;

void CK_SpawnPatPat(int tileX, int tileY, int dir);
void CK_PatPatThink(CK_object *obj); 

CK_action CK_ACT_PatPatD0 = {PATPATD1SPR, PATPATD1SPR, AT_Frame, 0, 0, 30, 0, 0, CK_PatPatThink, CK_DeadlyCol, CK_BasicDrawFunc1, &CK_ACT_PatPatD1};
CK_action CK_ACT_PatPatD1 = {PATPATD2SPR, PATPATD2SPR, AT_Frame, 0, 0, 30, 0, 0, CK_PatPatThink, CK_DeadlyCol, CK_BasicDrawFunc1, &CK_ACT_PatPatD2};
CK_action CK_ACT_PatPatD2 = {PATPATD3SPR, PATPATD3SPR, AT_Frame, 0, 0, 30, 0, 0, CK_PatPatThink, CK_DeadlyCol, CK_BasicDrawFunc1, &CK_ACT_PatPatD0};
CK_action CK_ACT_PatPatU0 = {PATPATU1SPR, PATPATU1SPR, AT_Frame, 0, 0, 30, 0, 0, CK_PatPatThink, CK_DeadlyCol, CK_BasicDrawFunc1, &CK_ACT_PatPatU1};
CK_action CK_ACT_PatPatU1 = {PATPATU2SPR, PATPATU2SPR, AT_Frame, 0, 0, 30, 0, 0, CK_PatPatThink, CK_DeadlyCol, CK_BasicDrawFunc1, &CK_ACT_PatPatU2};
CK_action CK_ACT_PatPatU2 = {PATPATU3SPR, PATPATU3SPR, AT_Frame, 0, 0, 30, 0, 0, CK_PatPatThink, CK_DeadlyCol, CK_BasicDrawFunc1, &CK_ACT_PatPatU0};

void CK_SpawnPatPat(int tileX, int tileY, int dir)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_PatPat;
	//obj->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	obj->clipped = CLIP_not;
	obj->zLayer = FOREGROUNDPRIORITY;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY);
	obj->xDirection = motion_Right;
	obj->yDirection = motion_Down;
	if (dir == 0) 
	{
		CK_SetAction(obj, &CK_ACT_PatPatD0);
		obj->posY -= 0xC0;
	} else {
		CK_SetAction(obj, &CK_ACT_PatPatU0);
		obj->posY -= 0x40;
	}

	obj->user1 = obj->currentAction->timer;
	obj->user2 = dir;
}

void CK_PatPatThink(CK_object *obj) 
{
	int delta;
	int xdist;
	unsigned ydist;
	boolean gofast = false;

	if (ABS(obj->posX - ck_keenObj->posX) < 0x300) {
		if (obj->user2 == 0) {
			ydist = obj->clipRects.unitY2 - ck_keenObj->clipRects.unitY1;
			if (ydist < 0x800) 
				gofast = true;
		} else {
			ydist = ck_keenObj->clipRects.unitY2 - obj->clipRects.unitY1;
			if (ydist < 0x800) 
				gofast = true;
		}
	}

	if (gofast)
	{
		delta = 4 * tics;
	} else {
		delta = tics;
	}

	if ((obj->user1 -= delta) <= 0)
	{
		int extra = -obj->user1;
		CK_SetAction2(obj, obj->currentAction->next);
		obj->user1 = obj->currentAction->timer - extra;
	}
}

/*
==============================================================================
	COUNCIL JANITOR

	The bucket is spawned directly after spawning the janitor, so the janitor
	can always access the bucket object via its 'next' pointer. That means
	we don't need to store a pointer in the user variables, so there are no
	pointer issues when loading a saved game.

	user1 - used for steps countdown and animation count
	user2 - janitor number
	user3 - cooldown after touching a sleeping Keen
	user4 - Nisasi pointer

	BUCKET

	user1 - address of janitor's sprite pointer (sde)
	user2
	user3
	user4
==============================================================================
*/

CK_action CK_ACT_CouncilWalk0;
CK_action CK_ACT_CouncilWalk1;
//CK_action CK_ACT_CouncilPause;

CK_action CK_ACT_Bucket;
CK_action CK_ACT_CouncilMop0;
CK_action CK_ACT_CouncilMop1;
CK_action CK_ACT_CouncilTurn0;
CK_action CK_ACT_CouncilTurn1;

void CK_SpawnCouncilMember(int tileX, int tileY, int number)
{	
	int addr;
	
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_CouncilMember;
	obj->zLayer = 0;
	obj->user2 = number;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY) - 369;
	obj->xDirection = US_RndT() < 0x80 ? motion_Right : motion_Left;
	obj->yDirection = motion_Down;
	addr = FP_OFF(&obj->sde);
	CK_SetAction(obj, &CK_ACT_CouncilWalk0);

	// also spawn the bucket along with the council janitor:
	obj = CK_GetNewObj(false);
	obj->type = CT_Bucket;
	obj->zLayer = 0;	// must have the same priority as the janitor for RF_PlaceSpriteAfter
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY);
	obj->gfxChunk = COUNCILBUCKETSPR;
	obj->user1 = addr;
	CK_SetAction(obj, &CK_ACT_Bucket);
}

void CK_CouncilWalk(CK_object *obj)
{
	// handle steps countdown:
	if (obj->user1)
		obj->user1--;

	// handle cooldown:
	if (obj->user3)
		obj->user3--;

	// random pause
/*
	if ((tics << 3) > US_RndT())
		obj->currentAction = &CK_ACT_CouncilPause;
*/
}

void CK_CouncilMop(CK_object *obj)
{
	if (--obj->user1 <= 0)
	{
		obj->user1 = 12;	// this is how many steps the janitor must move before he can use the bucket again
		CK_SetAction2(obj, &CK_ACT_CouncilWalk0);

		// make the bucket visible again:
		obj = obj->next;	// bucket is always spawned directly after the janitor, so obj->next is the bucket object
		obj->gfxChunk = COUNCILBUCKETSPR;
		obj->visible = true;	// force redraw of the bucket!
	}
}

void CK_CouncilContact(CK_object *obj, CK_object *hit)
{
	if (hit->type == CT_Bucket && hit == obj->next && !obj->user1)
	{
		if ((obj->xDirection == motion_Left && obj->posX == hit->posX)
			|| (obj->xDirection == motion_Right && obj->posX + 8*PIXGLOBAL == hit->posX))
		{
			hit->gfxChunk = 0;	// make bucket invisible
			hit->visible = true;	// force redraw

			obj->user1 = 3;	// this is how many times the the bucket animation plays
			CK_SetAction2(obj, &CK_ACT_CouncilMop0);
		}
	}
	// Note: touching Keen is handled in CK_KeenColFunc() in CK_KEEN.C
}

void CK_CouncilReact(CK_object *obj)
{
	int xdir = obj->xDirection;

	CK_BasicDrawFunc2(obj);

	if (xdir != obj->xDirection)
	{
		CK_SetAction2(obj, &CK_ACT_CouncilTurn0);
		obj->timeUntillThink = 0;
		CK_BasicDrawFunc1(obj);	// to replace the sprite placed by CK_BasicDrawFunc2()
	}
}

void BucketReact(CK_object *obj)
{
	RF_PlaceSpriteAfter(&(obj->sde), obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer, (void**)obj->user1);
}

CK_action CK_ACT_CouncilWalk0 = {COUNCILWALKL1SPR, COUNCILWALKR1SPR, AT_UnscaledOnce, false, push_down,  19, 0x20, 0, CK_CouncilWalk, CK_CouncilContact, CK_CouncilReact, &CK_ACT_CouncilWalk1};
CK_action CK_ACT_CouncilWalk1 = {COUNCILWALKL2SPR, COUNCILWALKR2SPR, AT_UnscaledOnce, false, push_down,  19, 0x20, 0, CK_CouncilWalk, CK_CouncilContact, CK_CouncilReact, &CK_ACT_CouncilWalk0};
//CK_action CK_ACT_CouncilPause = {COUNCILSTANDLSPR, COUNCILSTANDRSPR, AT_UnscaledOnce, false, push_down, 120, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_CouncilWalk0};

CK_action CK_ACT_Bucket = {0, 0, AT_UnscaledOnce, false, push_none, 32000, 0, 0, NULL, NULL, BucketReact, &CK_ACT_Bucket};
CK_action CK_ACT_CouncilMop0 = {COUNCILMOPL1SPR, COUNCILMOPR1SPR, AT_UnscaledOnce, false, push_down, 20, 0, 0, CK_CouncilMop, NULL, CK_BasicDrawFunc1, &CK_ACT_CouncilMop1};
CK_action CK_ACT_CouncilMop1 = {COUNCILMOPL2SPR, COUNCILMOPR2SPR, AT_UnscaledOnce, false, push_down, 20, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_CouncilMop0};

// HACK:
// Left and right sprites are swapped here, since these actions are used AFTER
// the janitor turns around, but we want to pretend he's still facing the other
// direction.
CK_action CK_ACT_CouncilTurn0 = {COUNCILSTANDRSPR, COUNCILSTANDLSPR, AT_UnscaledOnce, false, push_down, 120, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_CouncilTurn1};
CK_action CK_ACT_CouncilTurn1 = {COUNCILWALKR1SPR, COUNCILWALKL1SPR, AT_UnscaledOnce, false, push_down,  10, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_CouncilWalk0};

/*
==============================================================================
	Lindsey

	user1 - initial y position
	user4 - associated message number
==============================================================================
*/

CK_action CK_ACT_Lindsey0;
CK_action CK_ACT_Lindsey1;
CK_action CK_ACT_Lindsey2;
CK_action CK_ACT_Lindsey3;

void CK_SpawnLindsey(int tileX, int tileY, int num);
void CK_LindseyFloat(CK_object *obj);
void CK_LindseyReact(CK_object *obj);

CK_action CK_ACT_Lindsey0 = {LINDSEY1SPR, LINDSEY1SPR, AT_UnscaledFrame, 0, 0, 20, 0, 0, CK_LindseyFloat, NULL, CK_LindseyReact, &CK_ACT_Lindsey1};
CK_action CK_ACT_Lindsey1 = {LINDSEY2SPR, LINDSEY2SPR, AT_UnscaledFrame, 0, 0, 20, 0, 0, CK_LindseyFloat, NULL, CK_LindseyReact, &CK_ACT_Lindsey2};
CK_action CK_ACT_Lindsey2 = {LINDSEY3SPR, LINDSEY3SPR, AT_UnscaledFrame, 0, 0, 20, 0, 0, CK_LindseyFloat, NULL, CK_LindseyReact, &CK_ACT_Lindsey3};
CK_action CK_ACT_Lindsey3 = {LINDSEY4SPR, LINDSEY4SPR, AT_UnscaledFrame, 0, 0, 20, 0, 0, CK_LindseyFloat, NULL, CK_LindseyReact, &CK_ACT_Lindsey0};

void CK_SpawnLindsey(int tileX, int tileY, int num)
{
	CK_object *obj = CK_GetNewObj(false);
	//obj->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	obj->zLayer = 0;
	obj->clipped = CLIP_not;
	obj->posX = RF_TileToUnit(tileX);
	obj->user1 = obj->posY = RF_TileToUnit(tileY) - 0x100;
	obj->user4 = num;
	obj->yDirection = motion_Down;
	if (num == 0)
	{
		obj->type = CT_Lindsey;
	}
	else if (num == -1)
	{
		obj->type = CT_LindseySpecial;
	}
	else
	{
		obj->type = CT_LindseyHidden;
	}
	CK_SetAction(obj, &CK_ACT_Lindsey0);
}

void CK_LindseyFloat(CK_object *obj)
{
	// special Lindsey shows a message and ends the level after the smoke clears
	if (obj->type == CT_LindseySpecial)
	{
		if ((obj->user4 += tics) > 33)	// need at least 32 tics for the smoke to clear
		{
			ck_levelState = LS_BonusComplete;
		}
	}

	CK_PhysAccelVert(obj, obj->yDirection, 8);
	if ((int)(obj->user1 - obj->posY) > 0x20)
		obj->yDirection = motion_Down;

	if ((int)(obj->posY - obj->user1) > 0x20)
		obj->yDirection = motion_Up;
}

void CK_LindseyReact(CK_object *obj)
{
	if (obj->type == CT_LindseyHidden)
		RF_RemoveSprite(&obj->sde);
	else
		CK_BasicDrawFunc1(obj);
}

void CK_LindseyVanish(CK_object *ob)
{
	ob->type = CT_LindseyVanish;	// no further interaction with this object
	ob->gfxChunk = SMIRKYPOOF1SPR;
	ob->user1 = SMIRKYPOOF4SPR;
	ob->posX += -8*PIXGLOBAL;
	CK_SetAction2(ob, &CK_ACT_Smoke);
}

/*
==============================================================================
	XkyLicks / Optiquat

	user1 = shuffletimer
	user2 = chase mode flag
	user3 = mushroom tics
	user4 = Nisasi pointer
==============================================================================
*/

CK_action CK_ACT_XkyLickHop0;
CK_action CK_ACT_XkyLickHop1;
CK_action CK_ACT_XkyLickHop2;
CK_action CK_ACT_XkyLickHop3;
CK_action CK_ACT_XkyLickFlame0;
CK_action CK_ACT_XkyLickFlame1;
CK_action CK_ACT_XkyLickFlame2;
CK_action CK_ACT_XkyLickFlame3;
CK_action CK_ACT_XkyLickFlame4;
CK_action CK_ACT_XkyLickFlame5;
CK_action CK_ACT_XkyLickFlame6;
CK_action CK_ACT_XkyLickFlame7;
CK_action CK_ACT_XkyLickStunned0;
CK_action CK_ACT_XkyLickStunned1;

CK_action CK_ACT_XkyLickShuffle0;
CK_action CK_ACT_XkyLickShuffle1;
CK_action CK_ACT_XkyLickShuffle2;
CK_action CK_ACT_XkyLickShuffle3;

CK_action CK_ACT_XkyLickSearch0;
CK_action CK_ACT_XkyLickSearch1;

CK_action CK_ACT_XkyLickShroomed;

void CK_SpawnXkyLick(int tileX, int tileY);
void CK_XkyLickBounce(CK_object *obj);
void CK_XkyLickShuffle(CK_object *obj);
void CK_XkyLickCol(CK_object *a, CK_object *b);
void CK_XkyLickAttackCol(CK_object *a, CK_object *b);
void CK_XkyLickDraw(CK_object *obj);
void CK_XkyLickReact(CK_object *obj);

CK_action CK_ACT_XkyLickHop0 = {XKYLICKL1SPR, XKYLICKR1SPR, AT_UnscaledOnce, false, push_none, 10, 0, 0, CK_XkyLickBounce, CK_XkyLickCol, CK_BasicDrawFunc1, &CK_ACT_XkyLickHop1};
CK_action CK_ACT_XkyLickHop1 = {XKYLICKL2SPR, XKYLICKR2SPR, AT_Frame, false, push_none, 0, 0, 0, CK_Fall, CK_XkyLickCol, CK_XkyLickDraw, &CK_ACT_XkyLickHop2};
CK_action CK_ACT_XkyLickHop2 = {XKYLICKL3SPR, XKYLICKR3SPR, AT_Frame, false, push_none, 0, 0, 0, CK_Fall, CK_XkyLickCol, CK_XkyLickDraw, NULL};
CK_action CK_ACT_XkyLickHop3 = {XKYLICKL4SPR, XKYLICKR4SPR, AT_UnscaledOnce, false, push_none, 10, 0, 0, NULL, CK_XkyLickCol, CK_BasicDrawFunc1, &CK_ACT_XkyLickHop0};
CK_action CK_ACT_XkyLickFlame0 = {XKYLICKATTACKL1SPR, XKYLICKATTACKR1SPR, AT_UnscaledOnce, true, push_none, 4, 0, 0, NULL, CK_XkyLickCol, CK_BasicDrawFunc1, &CK_ACT_XkyLickFlame1};
CK_action CK_ACT_XkyLickFlame1 = {XKYLICKATTACKL2SPR, XKYLICKATTACKR2SPR, AT_UnscaledOnce, true, push_none, 4, 0, 0, NULL, CK_XkyLickCol, CK_BasicDrawFunc1, &CK_ACT_XkyLickFlame2};
CK_action CK_ACT_XkyLickFlame2 = {XKYLICKATTACKL3SPR, XKYLICKATTACKR3SPR, AT_UnscaledOnce, true, push_none, 4, 0, 0, NULL, CK_XkyLickAttackCol, CK_BasicDrawFunc1, &CK_ACT_XkyLickFlame3};
CK_action CK_ACT_XkyLickFlame3 = {XKYLICKATTACKL2SPR, XKYLICKATTACKR2SPR, AT_UnscaledOnce, true, push_none, 4, 0, 0, NULL, CK_XkyLickAttackCol, CK_BasicDrawFunc1, &CK_ACT_XkyLickFlame4};
CK_action CK_ACT_XkyLickFlame4 = {XKYLICKATTACKL1SPR, XKYLICKATTACKR1SPR, AT_UnscaledOnce, true, push_none, 4, 0, 0, NULL, CK_XkyLickAttackCol, CK_BasicDrawFunc1, &CK_ACT_XkyLickFlame5};
CK_action CK_ACT_XkyLickFlame5 = {XKYLICKATTACKL2SPR, XKYLICKATTACKR2SPR, AT_UnscaledOnce, true, push_none, 4, 0, 0, NULL, CK_XkyLickAttackCol, CK_BasicDrawFunc1, &CK_ACT_XkyLickFlame6};
CK_action CK_ACT_XkyLickFlame6 = {XKYLICKATTACKL3SPR, XKYLICKATTACKR3SPR, AT_UnscaledOnce, true, push_none, 4, 0, 0, NULL, CK_XkyLickAttackCol, CK_BasicDrawFunc1, &CK_ACT_XkyLickFlame7};
CK_action CK_ACT_XkyLickFlame7 = {XKYLICKATTACKL2SPR, XKYLICKATTACKR2SPR, AT_UnscaledOnce, true, push_none, 4, 0, 0, NULL, CK_XkyLickAttackCol, CK_BasicDrawFunc1, &CK_ACT_XkyLickHop2};
CK_action CK_ACT_XkyLickStunned0 = {XKYLICKSTUNNED1SPR, XKYLICKSTUNNED1SPR, AT_Frame, false, push_none, 0, 0, 0, CK_Fall, NULL, CK_BasicDrawFunc4, &CK_ACT_XkyLickStunned1};
CK_action CK_ACT_XkyLickStunned1 = {XKYLICKSTUNNEDSPR, XKYLICKSTUNNEDSPR, AT_Frame, false, push_none, 0, 0, 0, CK_Fall, NULL, CK_BasicDrawFunc4, NULL};

CK_action CK_ACT_XkyLickShuffle0 = {XKYLICKL1SPR, XKYLICKR1SPR, AT_ScaledOnce, false, push_down, 8, 2, 0, CK_XkyLickShuffle, CK_XkyLickCol, CK_XkyLickReact, &CK_ACT_XkyLickShuffle1};
CK_action CK_ACT_XkyLickShuffle1 = {XKYLICKL2SPR, XKYLICKR2SPR, AT_ScaledOnce, false, push_down, 8, 2, 0, CK_XkyLickShuffle, CK_XkyLickCol, CK_XkyLickReact, &CK_ACT_XkyLickShuffle2};
CK_action CK_ACT_XkyLickShuffle2 = {XKYLICKL3SPR, XKYLICKR3SPR, AT_ScaledOnce, false, push_down, 8, 2, 0, CK_XkyLickShuffle, CK_XkyLickCol, CK_XkyLickReact, &CK_ACT_XkyLickShuffle3};
CK_action CK_ACT_XkyLickShuffle3 = {XKYLICKL4SPR, XKYLICKR4SPR, AT_ScaledOnce, false, push_down, 8, 2, 0, CK_XkyLickShuffle, CK_XkyLickCol, CK_XkyLickReact, &CK_ACT_XkyLickShuffle0};

CK_action CK_ACT_XkyLickSearch0 = {XKYLICKL1SPR, XKYLICKL1SPR, AT_UnscaledOnce, false, push_down, 20, 0, 0, NULL, CK_XkyLickCol, CK_XkyLickReact, &CK_ACT_XkyLickSearch1};
CK_action CK_ACT_XkyLickSearch1 = {XKYLICKR2SPR, XKYLICKR2SPR, AT_UnscaledOnce, false, push_down, 20, 0, 0, NULL, CK_XkyLickCol, CK_XkyLickReact, &CK_ACT_XkyLickHop0};

CK_action CK_ACT_XkyLickShroomed = {XKYLICKSTUNNED1SPR, XKYLICKSTUNNED1SPR, AT_Frame, false, push_none, 0, 0, 0, CK_Fall, CK_XkyLickCol, CK_XkyLickDraw, NULL};

#define MUSHROOMDELAY (-42)	// counted in bounces (must be negative!)

#if ((signed int)(MUSHROOMDELAY) > 0)
#error MUSHROOMDELAY must NOT be a positive value!
#endif

void CK_SpawnXkyLick(int tileX, int tileY)
{

	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_XkyLick;
	//obj->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	obj->zLayer = 2;
	obj->posX = RF_TileToUnit(tileX);
	obj->user1 = obj->posY = RF_TileToUnit(tileY);
	obj->user3 = MUSHROOMDELAY;
	obj->xDirection = US_RndT() < 0x80? motion_Right : motion_Left;
	obj->yDirection = motion_Down;
	CK_SetAction(obj, &CK_ACT_XkyLickHop2);	// K1n9_Duk3 mod: start in falling state
}

void CK_XkyLickBounce(CK_object *obj) 
{
	CK_object *target = ck_keenObj;
	int dx, dy=0;

	if (obj->user3 > 0)
	{
		if (--obj->user3 <= 0)
		{
			obj->user3 = MUSHROOMDELAY;
		}
		else
		{
			target = ck_mapState.lastCheckpoint;
			dy = 30*PIXGLOBAL;	// because of different sprite offsets
		}
	}
	else if (obj->user3 < 0)
	{
		obj->user3++;
	}

	dx = target->posX - obj->posX;
	dy += target->posY - obj->posY;

	if (((obj->user2 && ABS(dx) < 0xA00) || 
			(!obj->user2 && ABS(dx) < 0x700)) && 
			dy < 0x300 && dy > -0x700 &&
			!obj->user4)
	{
		// Chase
		obj->user2 = 1;
		obj->xDirection = obj->posX > target->posX ? motion_Left : motion_Right;

		// Start attack animation when TARGET is really close:

		if (dy >= -0x100 && dy <= 0x100 && !obj->user4)
		{
			if (obj->xDirection == motion_Right && dx > -0x20 && dx < 0x180 ||
					obj->xDirection == motion_Left && dx < 0x20 && dx > -0x200)
			{
				SD_PlaySound(OPTIQUATLICKSND);
				CK_SetAction2(obj, &CK_ACT_XkyLickFlame0);
				return;
			}
		}

		if (ABS(dx) < 0x300) 
		{
			obj->velX = obj->xDirection * 16;
			obj->velY = -16;
		} 
		else 
		{
			obj->velX = obj->xDirection * 32;
			obj->velY = -24;
		}
	}
	else	// TARGET is too far away
	{
		obj->user2 = 0;

		// Jump around 
		if (US_RndT() < 0x30) 
		{
			obj->xDirection = -obj->xDirection;
			obj->velX = obj->xDirection * 4;
			obj->velY = -32 - (US_RndT() >> 4);
		} 
		else if (US_RndT() < 0x40) 
		{
			CK_SetAction2(obj, &CK_ACT_XkyLickShuffle0);
			obj->user1 = 8 + (US_RndT() >> 4);
		} 
		else 
		{
			obj->velX = obj->xDirection * 16;
			obj->velY = -16 - (US_RndT() >> 5);
		}

		// Randomly start chasing a checkpoint mushroom
		if (!obj->user3)
		{
			//if (US_RndT() == 255)	// extremely rare, but not impossible
			if (US_RndT() < 0x80)	// pretty common
			{
				obj->user3 = 4;	// four bounces should be enough, otherwise the checkpoint would be too far away anyway
				obj->user2 = 1;	// start in chase mode
			}
		}
		else if (obj->user3 > 0)
		{
			obj->user3 = MUSHROOMDELAY;
		}
	}
}

void CK_XkyLickShuffle(CK_object *obj)
{
	if (obj->user3 < 0)
	{
		obj->user3++;
	}

	if (--obj->user1 == 0)
	{
		CK_SetAction2(obj, &CK_ACT_XkyLickSearch0);
	}
}

void CK_XkyLickCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Stunner)
	{
	//	CK_StunCreature(a, b, US_RndT() < 0x80 ? &CK_ACT_XkyLickStunned0 : &CK_ACT_XkyLickStunned1);
		CK_StunCreature(a, b, &CK_ACT_XkyLickStunned0);
		a->velY = -24;
		a->velX = a->xDirection * 8;
		a->zLayer = 2;
	}
}

void CK_XkyLickAttackCol(CK_object *a, CK_object *b)
{
	if (b->gfxChunk == CPBIGSHROOMSPR
	//	&& a->user3 > 0		// add the user3 check if accidently licking a mushroom shouldn't count
		&& a->currentAction == &CK_ACT_XkyLickFlame7)
	{
		if (  (a->xDirection < 0 && a->clipRects.unitX1 > b->clipRects.unitX1)
			|| (a->xDirection > 0 && a->clipRects.unitX2 < b->clipRects.unitX2))
		{
			CK_SetAction2(a, &CK_ACT_XkyLickShroomed);
			a->velY = -24;
			a->velX = -a->xDirection * 8;
		}
		a->user3 = MUSHROOMDELAY;
	}
	else if (b->type == CT_Player)
	{
		CK_KillKeen();
	}
	else
	{
		CK_XkyLickCol(a, b);
	}
}

void CK_XkyLickDraw(CK_object *obj) 
{
#if 0
	if (obj->leftTI || obj->rightTI) {

	}
#endif

	if (obj->topTI) {
		CK_SetAction2(obj, &CK_ACT_XkyLickHop3);
	}

	CK_BasicDrawFunc1(obj);
}

void CK_XkyLickReact(CK_object *obj)
{
	// turn around when hitting a wall or walking off a ledge:
	if ( (obj->xDirection == motion_Right && obj->leftTI != 0)
	  || (obj->xDirection == motion_Left && obj->rightTI != 0)
	  || (obj->topTI == 0) )
	{
		obj->posX -= obj->deltaPosX;
		obj->posY -= obj->deltaPosY;
		obj->xDirection = -obj->xDirection;
		obj->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(obj, obj->currentAction);
		
		// just start falling if still not touching the ground:
		if (!obj->topTI)
		{
			CK_SetAction2(obj, &CK_ACT_XkyLickHop2);
			obj->timeUntillThink = obj->velX = obj->velY = 0;
		}
	}

	CK_BasicDrawFunc1(obj);
}


#if 0
/*
==============================================================================
	Yorp
==============================================================================
*/

void CK_SpawnYorp(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_Yorp;
//	obj->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	obj->zLayer = 2;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY) - 0x100;
	obj->xDirection = US_RndT() < 0x80 ? motion_Right : motion_Left;
	obj->yDirection = motion_Down;
	CK_SetAction(obj, &CK_ACT_YorpWalk0);
}

void CK_YorpWalk(CK_object *obj)
{
	int r = US_RndT();
	if (r < 0x20) 
	{
		obj->user1 = 5 + (r >> 3);
		CK_SetAction(obj, &CK_ACT_YorpSearch0);
		return;
	}

	r = US_RndT();
	if (r < 0x20) 
	{
		CK_SetAction(obj, &CK_ACT_YorpJump0);
		obj->velX = 10;
		obj->velY = -10 - (r >> 2);
		return;
	}
}

void CK_YorpSearch(CK_object *obj)
{
	if (--obj->user1 == 0) 
	{
		obj->xDirection = ck_keenObj->clipRects.unitXmid > obj->clipRects.unitXmid?
			motion_Right : motion_Left;

		CK_SetAction(obj, &CK_ACT_YorpWalk0);
	}
}

void CK_YorpCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Stunner)
	{
		CK_StunCreature(a, b, US_RndT() < 0x80 ? &CK_ACT_XkyLickStunned0 : &CK_ACT_XkyLickStunned1);
		a->velY = -24;
		a->velX = a->xDirection * 8;
	} 
	else if (b->type == CT_Player)
	{
		if (b->velY > 0 && 
				(b->clipRects.unitX2 > a->clipRects.unitX1 && b->clipRects.unitX1 < a->clipRects.unitX1) || 
				(b->clipRects.unitX1 < a->clipRects.unitX2 && b->clipRects.unitX2 > a->clipRects.unitX2))
		{
			CK_SetAction(a, &CK_ACT_YorpFazed0);
		}
		else
		{
			CK_PhysPushX(b, a);
		}
	}
}
#endif

/*
==============================================================================
	EARL FISH
==============================================================================

CK_action CK_ACT_EarlFishSwim0;
CK_action CK_ACT_EarlFishSwim1;
CK_action CK_ACT_EarlFishSwim2;
CK_action CK_ACT_EarlFishRecover0;
CK_action CK_ACT_EarlFishRecover1;

void CK_SpawnEarlFish(int tileX, int tileY);
void CK_EarlFishRecover(CK_object *obj);
void CK_EarlFishGlide(CK_object *obj);
void CK_EarlFishDraw(CK_object *obj);

CK_action CK_ACT_EarlFishSwim0 = {EARLFISHL1SPR, EARLFISHR1SPR, AT_UnscaledFrame, 0, 0, 10, 0, 0, CK_Glide, NULL, CK_EarlFishDraw, &CK_ACT_EarlFishSwim1};
CK_action CK_ACT_EarlFishSwim1 = {EARLFISHL2SPR, EARLFISHR2SPR, AT_UnscaledFrame, 0, 0, 10, 0, 0, CK_Glide, NULL, CK_EarlFishDraw, &CK_ACT_EarlFishSwim2};
CK_action CK_ACT_EarlFishSwim2 = {EARLFISHL3SPR, EARLFISHR3SPR, AT_Frame, 0, 0, 10, 0, 0, CK_EarlFishGlide, NULL, CK_EarlFishDraw, &CK_ACT_EarlFishSwim2};
CK_action CK_ACT_EarlFishRecover0 = {EARLFISHL2SPR, EARLFISHR2SPR, AT_ScaledOnce, 0, 0, 20, 3, 0, NULL, NULL, CK_EarlFishDraw, &CK_ACT_EarlFishRecover1};
CK_action CK_ACT_EarlFishRecover1 = {EARLFISHL1SPR, EARLFISHR1SPR, AT_ScaledOnce, 0, 0, 20, 2, 0, CK_EarlFishRecover, NULL, CK_EarlFishDraw, &CK_ACT_EarlFishSwim0};

// user1: vertical movement timer
// user2: glide decay timer

void CK_SpawnEarlFish(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_EarlFish;
	//obj->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	// obj->clipped = CLIP_simple;
	obj->zLayer = 0;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY);
	obj->xDirection = US_RndT() < 0x80 ? motion_Right : motion_Left;
	obj->yDirection = motion_None;
	CK_SetAction(obj, &CK_ACT_EarlFishSwim0);
}

#define EARLFISH_MIN_GLIDE 9
#define EARLFISH_MIN_GLIDE_Y 2
void CK_EarlFishRecover(CK_object *obj) {
			obj->user1 = 0;
			obj->velX = obj->xDirection * 24;
			obj->velY = ((int)US_RndT() - 128) >> 4;
			obj->yDirection = obj->velY > 0? motion_Down : motion_Up;
			if (ABS(obj->velY) < EARLFISH_MIN_GLIDE_Y) {
				obj->velY = obj->yDirection * EARLFISH_MIN_GLIDE_Y;
			}
}

void CK_EarlFishGlide(CK_object *obj) {

	CK_PhysDampVert(obj);
	// slow down a bit
	if (ABS(obj->velY) < EARLFISH_MIN_GLIDE_Y) {
		obj->velY = obj->yDirection * (EARLFISH_MIN_GLIDE_Y);
	}

	CK_PhysDampHorz(obj);
	if (ABS(obj->velX) < EARLFISH_MIN_GLIDE) {
		obj->velX = obj->xDirection * EARLFISH_MIN_GLIDE;
		if (++obj->user1 > TickBase) {
			obj->velX >>= 1; 
			if (US_RndT() < 0x08) {
				obj->currentAction = &CK_ACT_EarlFishRecover0;
			}
		}
	}
}


void CK_EarlFishDraw(CK_object *obj)
{
	if (obj->rightTI || obj->leftTI)
	{
		obj->velX = 0;
		obj->xDirection = -obj->xDirection;
	}

	if (obj->topTI || obj->bottomTI) {
		obj->velY = -obj->velY;
		obj->velY >>= 1;
	}

	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}
*/

/*
==============================================================================
	DUNG BEETLE

	walks back & forth; just eye candy
==============================================================================
*/

CK_action CK_ACT_DungBeetle1;
CK_action CK_ACT_DungBeetle2;
CK_action CK_ACT_DungBeetle3;
CK_action CK_ACT_DungBeetle4;

CK_action CK_ACT_DungBeetle1 = {DUNGBEETLEL1SPR, DUNGBEETLER1SPR, AT_UnscaledFrame, false, push_down, 8, 16, 0, NULL, NULL, CK_BasicDrawFunc3, &CK_ACT_DungBeetle2};
CK_action CK_ACT_DungBeetle2 = {DUNGBEETLEL2SPR, DUNGBEETLER2SPR, AT_UnscaledFrame, false, push_down, 8, 16, 0, NULL, NULL, CK_BasicDrawFunc3, &CK_ACT_DungBeetle3};
CK_action CK_ACT_DungBeetle3 = {DUNGBEETLEL3SPR, DUNGBEETLER3SPR, AT_UnscaledFrame, false, push_down, 8, 16, 0, NULL, NULL, CK_BasicDrawFunc3, &CK_ACT_DungBeetle4};
CK_action CK_ACT_DungBeetle4 = {DUNGBEETLEL4SPR, DUNGBEETLER4SPR, AT_UnscaledFrame, false, push_down, 8, 16, 0, NULL, NULL, CK_BasicDrawFunc3, &CK_ACT_DungBeetle1};

void CK_SpawnDungBeetle(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_DungBeetle;
//	obj->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
//	obj->clipped = CLIP_simple;
	obj->zLayer = 0;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY) + 5*PIXGLOBAL;
	obj->xDirection = US_RndT() < 0x80 ? motion_Right : motion_Left;
	obj->yDirection = motion_None;
	CK_SetAction(obj, &CK_ACT_DungBeetle1);
}

/*
==============================================================================
	CHEMOLAIN

	teleports from log to log, stuns Keen on contact and can't be stunned

	user1 - countdown until next teleport
	user2 - color cycle counter
	user3 - log pointer
	user4 - Nisasi pointer
==============================================================================
*/

CK_action CK_ACT_ChemStand;
CK_action CK_ACT_ChemLook;
CK_action CK_ACT_ChemLick1;
CK_action CK_ACT_ChemLick2;
CK_action CK_ACT_ChemLick3;

CK_action CK_ACT_ChemCycle;

CK_action CK_ACT_ChemWarpOut1;
CK_action CK_ACT_ChemWarpOut2;
CK_action CK_ACT_ChemWarpOut3;
CK_action CK_ACT_ChemWarpOut4;

CK_action CK_ACT_ChemHidden;

CK_action CK_ACT_ChemWarpIn1;
CK_action CK_ACT_ChemWarpIn2;
CK_action CK_ACT_ChemWarpIn3;
CK_action CK_ACT_ChemWarpIn4;

void CK_SpawnChemolain(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_Chemolain;
	obj->active = OBJ_ALWAYS_ACTIVE;
	obj->clipped = CLIP_not;
	obj->zLayer = 1;	// logs need to use a lower zLayer value than this one!
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY);
	obj->xDirection = motion_None;
	obj->yDirection = motion_None;
	CK_SetAction(obj, &CK_ACT_ChemStand);
	obj->user1 = 100;
}

void CK_ChemCountdown(CK_object *obj)
{
	int temp = 1;

	// count down at 4x the speed if Keen is near:
	if (abs(ck_keenObj->posX - obj->posX) < 10*TILEGLOBAL)
		temp <<= 2;

	if (obj->user1 > temp)
	{
		obj->user1 -= temp;
	}
	else if (!obj->user4)	// don't teleport with Nisasi
	{
		obj->user1 = 100;	// reset countdown
		CK_SetAction2(obj, &CK_ACT_ChemWarpOut1);
		if (obj->user3)
		{
			register CK_object *log = (CK_object *)obj->user3;
			// old log is no longer occupied:
			log->type = CT_LogPlatform;
		}
	}
}

void CK_ChemStandThink(CK_object *obj)
{
	int temp = US_RndT();

	if (temp < 2)
	{
		CK_SetAction2(obj, &CK_ACT_ChemLick1);
	}
	else if (temp < 16)
	{
		CK_SetAction2(obj, &CK_ACT_ChemLook);
	}
	else if (CK_ObjectVisible(obj))	// only count down when visible (for ALWAYS ACTIVE)
	{
		CK_ChemCountdown(obj);
	}
}

void CK_ChemCycleThink(CK_object *obj)
{
	if (obj->user2-- > 0)
	{
		if (obj->gfxChunk >= CHEMCYCLE6SPR || obj->gfxChunk < CHEMCYCLE1SPR)
		{
			obj->gfxChunk = CHEMCYCLE1SPR;
		}
		else
		{
			obj->gfxChunk++;
		}
		obj->visible = true;	// force redraw
	}
	else
	{
		CK_SetAction2(obj, &CK_ACT_ChemStand);
	}
	// allow Chemolain to warp out during the color cycle animation
	// (to make sure Keen and the Chemolain won't get stun-locked)
	CK_ChemCountdown(obj);
}

void CK_ChemHiddenThink(CK_object *obj)
{
	CK_object *o, *best=NULL;
//	boolean visible = false;

	// find a (random) log platform:
	for (o=ck_keenObj; o; o=o->next)
	{
#if 1	// (new version)
		// only log platforms and not the one it's currently sitting on!
		if (o != (CK_object*)obj->user3 && o->type == CT_LogPlatform
			// log platform must not be farther than 16 tiles away from obj
			&& abs(o->clipRects.tileXmid - obj->clipRects.tileXmid) <= 16
			&& abs(o->clipRects.tileY1 - obj->clipRects.tileY2) <= 16)
		{
			// make sure to remember the first log platform we find,
			// in case we don't find any others.
			if (!best)
			{
				best = o;
			}
			else
			{
				// use a 50% chance to prefer the new one over the old.
				if (US_RndT() < 128)
					best = o;
			}
		}
#else	// (old version)
		// only log platforms and not the one it's currently sitting on!
		if (o != (CK_object*)obj->user3 && o->type == CT_LogPlatform)
		{
			// make sure to remember the first log platform we find,
			// in case we don't find any others.
			if (!best)
			{
				best = o;
				visible = CK_ObjectVisible(o);
			}
			else if (CK_ObjectVisible(o))
			{
				if (!visible)
				{
					// "best" log is not visible, but current log is, so of course we
					// want to use the current one as the "best". But let's say there
					// is a very small chance that we keep the old one.
					if (US_RndT() < 250)
					{
						best = o;
						visible = true;
					}
				}
				else
				{
					// both logs are visible, so we'll use a 50% chance to prefer the
					// new one over the old.
					if (US_RndT() < 128)
						best = o;
				}
			}
			else if (!visible)
			{
				// neither the "best" nor the current log are visible, so we'll use
				// a 50% chance to prefer the new one over the old.
				if (US_RndT() < 128)
					best = o;
			}
		}
#endif
	}

	// scan complete, now let's see if we have found a new log
	if (best)
	{
		// okay, move the chemolain to the new log and make it appear:
		obj->user3 = (int)best;
		obj->posX = best->posX;	// Note: contact function sets the correct position
		obj->posY = best->posY;
		CK_SetAction2(obj, &CK_ACT_ChemWarpIn1);
		best->type = CT_OccupiedLog;
	}
	// if no log is found, the actor waits and keeps searching until it finds one
}

void CK_ChemRide(CK_object *a, CK_object *b)
{
	// HACK:
	// We abuse this contact function to let the chemolain
	// "ride" the log it's currently occupying. For this to
	// work, the chemolain needs to have its hitbox extended
	// down a bit, so that it always touches the log even if
	// the log moves down.

	if (!a->user3)
	{
		switch (b->type)
		{
		case CT_LogPlatform:
			b->type = CT_OccupiedLog;
			a->user3 = (int)b;
			break;
		default:
			return;
		}
	}

	if (a->user3 == (int)b)
	{
		// adjust x & y position correctly and make sure hitboxes still overlap
		ck_nextX = b->posX+4*PIXGLOBAL - a->posX;
		ck_nextY = b->clipRects.unitY1 - 34*PIXGLOBAL - a->posY;
		CK_PhysUpdateNormalObj(a);
	}
}

void CK_ChemContact(CK_object *a, CK_object *b)
{
	switch (b->type)
	{
	case CT_Player:
		CK_StunKeen(a->clipRects.unitXmid);
		CK_SetAction2(a, &CK_ACT_ChemCycle);
		a->user2 = 12;	// cycle through all 6 frames twice
		break;
	case CT_Stunner:
		CK_ShotHit(b);
		CK_SetAction2(a, &CK_ACT_ChemCycle);
		a->user2 = 12;	// cycle through all 6 frames twice
		break;
	default:
		CK_ChemRide(a, b);
		break;
	}
}

CK_action CK_ACT_ChemStand = {CHEMLOOK1SPR, CHEMLOOK1SPR, AT_UnscaledOnce, false, 0, 7, 0, 0, CK_ChemStandThink, CK_ChemContact, CK_BasicDrawFunc1, &CK_ACT_ChemStand};
CK_action CK_ACT_ChemLook  = {CHEMLOOK2SPR, CHEMLOOK2SPR, AT_UnscaledOnce, false, 0, 70, 0, 0, NULL, CK_ChemContact, CK_BasicDrawFunc1, &CK_ACT_ChemStand};
CK_action CK_ACT_ChemLick1 = {CHEMLICK1SPR, CHEMLICK1SPR, AT_UnscaledOnce, false, 0, 14, 0, 0, NULL, CK_ChemContact, CK_BasicDrawFunc1, &CK_ACT_ChemLick2};
CK_action CK_ACT_ChemLick2 = {CHEMLICK2SPR, CHEMLICK2SPR, AT_UnscaledOnce, false, 0, 14, 0, 0, NULL, CK_ChemContact, CK_BasicDrawFunc1, &CK_ACT_ChemLick3};
CK_action CK_ACT_ChemLick3 = {CHEMLICK1SPR, CHEMLICK1SPR, AT_UnscaledOnce, false, 0, 14, 0, 0, NULL, CK_ChemContact, CK_BasicDrawFunc1, &CK_ACT_ChemStand};

CK_action CK_ACT_ChemCycle = {0, 0, AT_UnscaledOnce, false, 0, 8, 0, 0, CK_ChemCycleThink, CK_ChemContact, CK_BasicDrawFunc1, &CK_ACT_ChemCycle};

CK_action CK_ACT_ChemWarpOut1 = {CHEMWARP1SPR, CHEMWARP1SPR, AT_UnscaledOnce, false, 0, 13, 0, 0, NULL, CK_ChemRide, CK_BasicDrawFunc1, &CK_ACT_ChemWarpOut2};
CK_action CK_ACT_ChemWarpOut2 = {CHEMWARP2SPR, CHEMWARP2SPR, AT_UnscaledOnce, false, 0, 12, 0, 0, NULL, CK_ChemRide, CK_BasicDrawFunc1, &CK_ACT_ChemWarpOut3};
CK_action CK_ACT_ChemWarpOut3 = {CHEMWARP3SPR, CHEMWARP3SPR, AT_UnscaledOnce, false, 0, 11, 0, 0, NULL, CK_ChemRide, CK_BasicDrawFunc1, &CK_ACT_ChemWarpOut4};
CK_action CK_ACT_ChemWarpOut4 = {CHEMWARP4SPR, CHEMWARP4SPR, AT_UnscaledOnce, false, 0, 10, 0, 0, NULL, CK_ChemRide, CK_BasicDrawFunc1, &CK_ACT_ChemHidden};

CK_action CK_ACT_ChemHidden = {-1, -1, AT_UnscaledOnce, false, 0, 22, 0, 0, CK_ChemHiddenThink, NULL, CK_BasicDrawFunc1, &CK_ACT_ChemHidden};

CK_action CK_ACT_ChemWarpIn1 = {CHEMWARP4SPR, CHEMWARP4SPR, AT_UnscaledOnce, false, 0, 10, 0, 0, NULL, CK_ChemRide, CK_BasicDrawFunc1, &CK_ACT_ChemWarpIn2};
CK_action CK_ACT_ChemWarpIn2 = {CHEMWARP3SPR, CHEMWARP3SPR, AT_UnscaledOnce, false, 0, 9, 0, 0, NULL, CK_ChemRide, CK_BasicDrawFunc1, &CK_ACT_ChemWarpIn3};
CK_action CK_ACT_ChemWarpIn3 = {CHEMWARP2SPR, CHEMWARP2SPR, AT_UnscaledOnce, false, 0, 9, 0, 0, NULL, CK_ChemRide, CK_BasicDrawFunc1, &CK_ACT_ChemWarpIn4};
CK_action CK_ACT_ChemWarpIn4 = {CHEMWARP1SPR, CHEMWARP1SPR, AT_UnscaledOnce, false, 0, 8, 0, 0, NULL, CK_ChemRide, CK_BasicDrawFunc1, &CK_ACT_ChemStand};

/*
==============================================================================
	LOG

	- a (swimming) wooden platform that Keen can land on
	- sinks when Keen is standing on it (but not as fast as falling platforms)
	- can also move left/right
	- is blocked by tiles, but can also be blocked by block icons
	- stops moving left/right when hitting the ground (while sinking)
	- continues moving in the old direction when it rises back up again

	user1 - initial y position (for sinking and rising back up)
	user2 - previous x direction
==============================================================================
*/

CK_action CK_ACT_Log;

void CK_SpawnLogPlatform(int tileX, int tileY, int xdir)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_LogPlatform;
	obj->active = OBJ_ALWAYS_ACTIVE;
//	obj->clipped = CLIP_full;	// Tile-Hitbox differs from Object-Hitbox!
	// Note: when using full clipping, you need to add a hard-coded hitbox
	// definition in CK_PhysFullClipToWalls in CK_PHYS.C
	obj->zLayer = 0;	// logs are drawn behind any other actors
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = obj->user1 = RF_TileToUnit(tileY) + 3*PIXGLOBAL;
	obj->xDirection = xdir;
	obj->yDirection = motion_None;
	CK_SetAction(obj, &CK_ACT_Log);
}

void CK_LogThink(CK_object *obj)
{
	if (obj == ck_keenState.platform)
	{
		// Keen is standing on this log, so make it sink slowly:
		ck_nextY = tics;

		if ((int)(obj->posY - obj->user1) >= 16*PIXGLOBAL)
		{
			// make Keen fall off:
			obj->type = CT_SlipperyLog;
			ck_keenState.platform = NULL;
		}
	}
	else if (obj->posY > obj->user1)
	{
		// Keen is no longer standing on this log and it hasn't reached its
		// initial y position again, so make it rise (faster than it sinks):
		register int speed = -(tics*4);
		if ((ck_nextY = obj->user1 - obj->posY) < speed)
			ck_nextY = speed;
	}
	else
	{
		if (obj->type == CT_SlipperyLog)
			obj->type = CT_LogPlatform;
		ck_nextY = obj->user1 - obj->posY;	// move back to initial position
	}

	// Note: engine handles left/right movement if we have an x direction

	// if log is moving, check for [B] icons 
	// (2 tiles below the log's initial y position!):
	if (obj->xDirection == motion_Left)
	{
		if (CA_TileAtPos(obj->clipRects.tileX1, RF_UnitToTile(obj->user1) + 2, 2) == PLATFORMBLOCK)
		{
			obj->xDirection = motion_Right;
		}
	}
	else if (obj->xDirection == motion_Right)
	{
		if (CA_TileAtPos(obj->clipRects.tileX2, RF_UnitToTile(obj->user1) + 2, 2) == PLATFORMBLOCK)
		{
			obj->xDirection = motion_Left;
		}
	}
}

void CK_LogReact(CK_object *obj)
{
	if (obj->leftTI || obj->rightTI)
	{
		// hit a left/right wall, so turn around:
		obj->xDirection = -obj->xDirection;
	}
	if (obj->topTI)
	{
		// log has hit the ground
		if (obj == ck_keenState.platform)
		{
			// log is sinking, so stop moving left/right:
			if (obj->xDirection != motion_None)
			{
				obj->user2 = obj->xDirection;
				obj->xDirection = motion_None;
			}
		}
		else
		{
			// log is not sinking, so it must have hit a slope.
			// just turn around when that happens
			obj->xDirection = -obj->xDirection;
		}
	}
	else
	{
		// not touching the ground anymore, so start moving left/right again:
		if (obj->xDirection == motion_None)
			obj->xDirection = obj->user2;
	}
	CK_BasicDrawFunc1(obj);
}

CK_action CK_ACT_Log = {LOGSPR, LOGSPR, AT_ScaledFrame, false, 0, 8, 8, 0, CK_LogThink, NULL, CK_LogReact, &CK_ACT_Log};

/*
==============================================================================
	SALAMANDER

	stays in place, vanishes forever when Keen comes near, just eye candy
==============================================================================
*/

CK_action CK_ACT_SalamanderStand;
CK_action CK_ACT_SalamanderLick1;

CK_action CK_ACT_SalamanderVanish1;
CK_action CK_ACT_SalamanderVanish2;
CK_action CK_ACT_SalamanderVanish3;
CK_action CK_ACT_SalamanderVanish4;
CK_action CK_ACT_SalamanderVanish5;
CK_action CK_ACT_SalamanderVanish6;
CK_action CK_ACT_SalamanderVanish7;
CK_action CK_ACT_SalamanderVanish8;
CK_action CK_ACT_SalamanderVanish9;
CK_action CK_ACT_SalamanderVanish10;

void CK_SpawnSalamander(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_Salamander;
//	obj->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
//	obj->clipped = CLIP_simple;
	obj->zLayer = 0;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY);
	obj->xDirection = motion_Left;
	obj->yDirection = motion_None;
	CK_SetAction(obj, &CK_ACT_SalamanderStand);
}

void CK_SalamanderThink(CK_object *obj)
{
	if (ABS(obj->clipRects.unitXmid - ck_keenObj->clipRects.unitXmid) < 6*TILEGLOBAL
		&& (int)(obj->clipRects.unitY1 - ck_keenObj->clipRects.unitY2) < 3*TILEGLOBAL
		&& (int)(ck_keenObj->clipRects.unitY1 - obj->clipRects.unitY2) < 2*TILEGLOBAL)
	{
		CK_SetAction2(obj, &CK_ACT_SalamanderVanish1);
	}
	else if (obj->currentAction == &CK_ACT_SalamanderStand)
	{
		register int rnd = US_RndT();
		if (rnd < 16)
		{
			CK_SetAction2(obj, &CK_ACT_SalamanderLick1);
		}
	}
}

CK_action CK_ACT_SalamanderStand = {SALAMANDERSTANDSPR, SALAMANDERSTANDSPR, AT_UnscaledOnce, false, 0, 8, 0, 0, CK_SalamanderThink, NULL, CK_BasicDrawFunc1, &CK_ACT_SalamanderStand};
CK_action CK_ACT_SalamanderLick1 = {SALAMANDERLICK1SPR, SALAMANDERLICK1SPR, AT_UnscaledOnce, false, 0, 8, 0, 0, CK_SalamanderThink, NULL, CK_BasicDrawFunc1, &CK_ACT_SalamanderStand};

CK_action CK_ACT_SalamanderVanish1  = {SALAMANDERVANISH1SPR,  SALAMANDERVANISH1SPR,  AT_UnscaledOnce, false, 0, 5, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_SalamanderVanish2};
CK_action CK_ACT_SalamanderVanish2  = {SALAMANDERVANISH2SPR,  SALAMANDERVANISH2SPR,  AT_UnscaledOnce, false, 0, 5, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_SalamanderVanish3};
CK_action CK_ACT_SalamanderVanish3  = {SALAMANDERVANISH3SPR,  SALAMANDERVANISH3SPR,  AT_UnscaledOnce, false, 0, 5, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_SalamanderVanish4};
CK_action CK_ACT_SalamanderVanish4  = {SALAMANDERVANISH4SPR,  SALAMANDERVANISH4SPR,  AT_UnscaledOnce, false, 0, 5, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_SalamanderVanish5};
CK_action CK_ACT_SalamanderVanish5  = {SALAMANDERVANISH5SPR,  SALAMANDERVANISH5SPR,  AT_UnscaledOnce, false, 0, 5, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_SalamanderVanish6};
CK_action CK_ACT_SalamanderVanish6  = {SALAMANDERVANISH6SPR,  SALAMANDERVANISH6SPR,  AT_UnscaledOnce, false, 0, 7, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_SalamanderVanish7};
CK_action CK_ACT_SalamanderVanish7  = {SALAMANDERVANISH7SPR,  SALAMANDERVANISH7SPR,  AT_UnscaledOnce, false, 0, 7, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_SalamanderVanish8};
CK_action CK_ACT_SalamanderVanish8  = {SALAMANDERVANISH8SPR,  SALAMANDERVANISH8SPR,  AT_UnscaledOnce, false, 0, 7, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_SalamanderVanish9};
CK_action CK_ACT_SalamanderVanish9  = {SALAMANDERVANISH9SPR,  SALAMANDERVANISH9SPR,  AT_UnscaledOnce, false, 0, 5, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_SalamanderVanish10};
CK_action CK_ACT_SalamanderVanish10 = {SALAMANDERVANISH10SPR, SALAMANDERVANISH10SPR, AT_UnscaledOnce, false, 0, 5, 0, 0, NULL, NULL, CK_BasicDrawFunc1, NULL};

/*
==============================================================================
	WORMOUTH

	implementation taken straight from KEEN 4
==============================================================================
*/

extern CK_action CK_ACT_Wormouth;
extern CK_action CK_ACT_WormouthLook1;
extern CK_action CK_ACT_WormouthLook2;
extern CK_action CK_ACT_WormouthLook3;
extern CK_action CK_ACT_WormouthLook4;
extern CK_action CK_ACT_WormouthLook5;
extern CK_action CK_ACT_WormouthLook6;
extern CK_action CK_ACT_WormouthLook7;
extern CK_action CK_ACT_WormouthLook8;
extern CK_action CK_ACT_WormouthBite1;
extern CK_action CK_ACT_WormouthBite2;
extern CK_action CK_ACT_WormouthBite3;
extern CK_action CK_ACT_WormouthBite4;
extern CK_action CK_ACT_WormouthBite5;
extern CK_action CK_ACT_WormouthStunned;
extern CK_action CK_ACT_WormouthFall;

void CK_SpawnWormouth(int tileX, int tileY)
{
	CK_object *ob;

	ob = CK_GetNewObj(false);
	ob->type = CT_Wormouth;
//	ob->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	ob->zLayer = 0;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY) + 0x8F;
	if (US_RndT() < 0x80)
	{
		ob->xDirection = motion_Right;
	}
	else
	{
		ob->xDirection = motion_Left;
	}
	ob->yDirection = motion_Down;
	CK_SetAction(ob, &CK_ACT_Wormouth);
	
		// HACK: modify speed setting based on difficulty:
	if (ck_gameState.difficulty == D_Hard)
		CK_ACT_Wormouth.velX = 21;
	else
		CK_ACT_Wormouth.velX = 16;
}

void WormouthLookRightThink(CK_object *ob)
{
	if (ck_keenObj->posX > ob->posX)
	{
		ob->xDirection = motion_Right;
		ob->currentAction = &CK_ACT_Wormouth;
	}
}

void WormouthLookThink(CK_object *ob)
{
	if (ck_keenObj->posX > ob->posX)
	{
		ob->xDirection = motion_Right;
	}
	else
	{
		ob->xDirection = motion_Left;
	}
}

void WormouthLookLeftThink(CK_object *ob)
{
	if (ck_keenObj->posX < ob->posX)
	{
		ob->xDirection = motion_Left;
		ob->currentAction = &CK_ACT_Wormouth;
	}
}

void WormouthThink(CK_object *ob)
{
	int xdist, ydist;

	xdist = ck_keenObj->posX - ob->posX;
	ydist = ck_keenObj->clipRects.unitY2 - ob->clipRects.unitY2;
	if ((xdist < -3*TILEGLOBAL || xdist > 3*TILEGLOBAL) && US_RndT() < 6)
	{
		ck_nextX = 0;	// no movement on state transition!
		ob->currentAction = &CK_ACT_WormouthLook1;
	}
	else if (ydist >= -TILEGLOBAL && ydist <= TILEGLOBAL)
	{
		if (ob->xDirection == motion_Right && xdist > 8*PIXGLOBAL && xdist < 24*PIXGLOBAL
			|| ob->xDirection == motion_Left && xdist < -8*PIXGLOBAL && xdist > -32*PIXGLOBAL)
		{
			SD_PlaySound(WORMMOUTHBITESND);
			ck_nextX = 0;	// no movement on state transition!
			ob->currentAction = &CK_ACT_WormouthBite1;
		}
	}
}

void WormouthLookContact(CK_object *ob, CK_object *hit)
{
	if (hit->type == CT_Stunner)
	{
		CK_StunCreature(ob, hit, &CK_ACT_WormouthStunned);
		// Note: our CK_StunCreature() always makes the creature jump,
		// which wasn't the case in KEEN 4!
		ob->velY = 0;	//no jump for you!
	}
}

void WormouthBiteContact(CK_object *ob, CK_object *hit)
{
	if (hit->type == CT_Player)
	{
		if (ob->xDirection == motion_Right && ob->posX > hit->posX
			|| ob->xDirection == motion_Left && ob->posX < hit->posX)
			return;
		CK_KillKeen();
	}
	else
	{
		WormouthLookContact(ob, hit);
	}
}

void WormouthWalkReact(CK_object *ob)
{
	CK_BasicDrawFunc2(ob);
	if (!ob->topTI)
	{
		ob->timeUntillThink = 0;
		ob->xDirection = -ob->xDirection;
		CK_SetAction2(ob, &CK_ACT_WormouthFall);
	}
}

CK_action CK_ACT_Wormouth        = {WORMOUTHHIDESPR,    WORMOUTHHIDESPR,    AT_ScaledOnce,    true,  push_down,  4, 16, 0, WormouthThink,          NULL,                WormouthWalkReact, &CK_ACT_Wormouth};
CK_action CK_ACT_WormouthLook1   = {WORMOUTHLOOKL1SPR,  WORMOUTHLOOKL1SPR,  AT_UnscaledOnce,  false, push_none, 20,  0, 0, NULL,                   WormouthLookContact, CK_BasicDrawFunc1, &CK_ACT_WormouthLook2};
CK_action CK_ACT_WormouthLook2   = {WORMOUTHLOOKL2SPR,  WORMOUTHLOOKL2SPR,  AT_UnscaledOnce,  false, push_none,  8,  0, 0, NULL,                   WormouthLookContact, CK_BasicDrawFunc1, &CK_ACT_WormouthLook3};
CK_action CK_ACT_WormouthLook3   = {WORMOUTHLOOKL1SPR,  WORMOUTHLOOKL1SPR,  AT_UnscaledOnce,  false, push_none, 20,  0, 0, WormouthLookLeftThink,  WormouthLookContact, CK_BasicDrawFunc1, &CK_ACT_WormouthLook4};
CK_action CK_ACT_WormouthLook4   = {WORMOUTHHIDESPR,    WORMOUTHHIDESPR,    AT_UnscaledOnce,  false, push_none,  8,  0, 0, NULL,                   WormouthLookContact, CK_BasicDrawFunc1, &CK_ACT_WormouthLook5};
CK_action CK_ACT_WormouthLook5   = {WORMOUTHLOOKR1SPR,  WORMOUTHLOOKR1SPR,  AT_UnscaledOnce,  false, push_none, 20,  0, 0, NULL,                   WormouthLookContact, CK_BasicDrawFunc1, &CK_ACT_WormouthLook6};
CK_action CK_ACT_WormouthLook6   = {WORMOUTHLOOKR2SPR,  WORMOUTHLOOKR2SPR,  AT_UnscaledOnce,  false, push_none,  8,  0, 0, NULL,                   WormouthLookContact, CK_BasicDrawFunc1, &CK_ACT_WormouthLook7};
CK_action CK_ACT_WormouthLook7   = {WORMOUTHLOOKR1SPR,  WORMOUTHLOOKR1SPR,  AT_UnscaledOnce,  false, push_none, 20,  0, 0, WormouthLookRightThink, WormouthLookContact, CK_BasicDrawFunc1, &CK_ACT_WormouthLook8};
CK_action CK_ACT_WormouthLook8   = {WORMOUTHHIDESPR,    WORMOUTHHIDESPR,    AT_UnscaledOnce,  false, push_none,  8,  0, 0, WormouthLookThink,      NULL,                CK_BasicDrawFunc1, &CK_ACT_Wormouth};
CK_action CK_ACT_WormouthBite1   = {WORMOUTHBITEL1SPR,  WORMOUTHBITER1SPR,  AT_UnscaledOnce,  false, push_none,  8,  0, 0, NULL,                   WormouthBiteContact, CK_BasicDrawFunc1, &CK_ACT_WormouthBite2};
CK_action CK_ACT_WormouthBite2   = {WORMOUTHBITEL2SPR,  WORMOUTHBITER2SPR,  AT_UnscaledOnce,  false, push_none,  8,  0, 0, NULL,                   WormouthBiteContact, CK_BasicDrawFunc1, &CK_ACT_WormouthBite3};
CK_action CK_ACT_WormouthBite3   = {WORMOUTHBITEL3SPR,  WORMOUTHBITER3SPR,  AT_UnscaledOnce,  false, push_none, 16,  0, 0, NULL,                   WormouthBiteContact, CK_BasicDrawFunc1, &CK_ACT_WormouthBite4};
CK_action CK_ACT_WormouthBite4   = {WORMOUTHBITEL2SPR,  WORMOUTHBITER2SPR,  AT_UnscaledOnce,  false, push_none,  8,  0, 0, NULL,                   WormouthBiteContact, CK_BasicDrawFunc1, &CK_ACT_WormouthBite5};
CK_action CK_ACT_WormouthBite5   = {WORMOUTHBITEL1SPR,  WORMOUTHBITER1SPR,  AT_UnscaledOnce,  false, push_none,  8,  0, 0, NULL,                   WormouthBiteContact, CK_BasicDrawFunc1, &CK_ACT_Wormouth};
CK_action CK_ACT_WormouthStunned = {WORMOUTHSTUNNEDSPR, WORMOUTHSTUNNEDSPR, AT_Frame,         false, push_none, 10,  0, 0, CK_Fall,                NULL,                CK_BasicDrawFunc4, NULL};
CK_action CK_ACT_WormouthFall    = {WORMOUTHHIDESPR,    WORMOUTHHIDESPR,    AT_Frame,         false, push_none,  0,  0, 0, CK_Fall,                NULL,                CK_FallReact,      &CK_ACT_Wormouth};

/*
==============================================================================
	SHRUBBERY

	Randomly takes a peek while standing still. Stands up if Keen is nearby
	and walks towards Keen. Temporarily stuns Keen when touching him.

	user1 - number of repeats for the long looking animation
==============================================================================
*/

CK_action CK_ACT_Shrubbery;
CK_action CK_ACT_ShrubberyLook1;
CK_action CK_ACT_ShrubberyLook2;
//CK_action CK_ACT_ShrubberyLook3;
//CK_action CK_ACT_ShrubberyLook4;
CK_action CK_ACT_ShrubberyLook5;
CK_action CK_ACT_ShrubberyStandUp;
CK_action CK_ACT_ShrubberyWalk1;
CK_action CK_ACT_ShrubberyWalk2;
CK_action CK_ACT_ShrubberySitDown;
CK_action CK_ACT_ShrubberyFall;

void CK_SpawnShrubbery(int tileX, int tileY)
{
	CK_object *ob;

	ob = CK_GetNewObj(false);
	ob->type = CT_Shrubbery;
//	ob->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	ob->zLayer = 0;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY) - 11*PIXGLOBAL;
	ob->xDirection = motion_Left;
	ob->yDirection = motion_Down;
	CK_SetAction(ob, &CK_ACT_Shrubbery);
}

void ShrubberyThink(CK_object *ob)
{
	// stand up if Keen is near:
	if (ABS((unsigned)(ob->clipRects.unitY2 - ck_keenObj->clipRects.unitY2)) <= 3*TILEGLOBAL 
		&& ABS((unsigned)(ob->posX - ck_keenObj->posX)) <= 5*TILEGLOBAL)
	{
		CK_SetAction2(ob, &CK_ACT_ShrubberyStandUp);
		if (ck_keenObj->clipRects.unitXmid > ob->clipRects.unitXmid)
			ob->xDirection = motion_Right;
		else
			ob->xDirection = motion_Left;
	}
	else if (ob->gfxChunk == SHRUBBERYSPR)	// only when not already looking around
	{
		// randomly start looking around:
		int rnd = US_RndT();

		if (rnd < 0x08)
		{
			ob->user1 = 4;	// play the looking animation twice
			ob->gfxChunk = SHRUBBERYLOOK1SPR;
			CK_SetAction2(ob, &CK_ACT_ShrubberyLook2);
		}
		else if (rnd < 0x10)
		{
			CK_SetAction2(ob, &CK_ACT_ShrubberyLook1);
		}
	}
}

void ShrubberyLookThink(CK_object *ob)
{
	// Note: this is used in an AT_Frame action, so it's safe to re-use actionTimer in here
	if ((ob->actionTimer += tics) >= 10)
	{
		ob->actionTimer -= 10;
		ob->gfxChunk++;
		if (ob->gfxChunk > SHRUBBERYLOOK4SPR)
		{
			if (--ob->user1 <= 0)
			{
				CK_SetAction2(ob, &CK_ACT_ShrubberyLook5);
			}
			else
			{
				ob->gfxChunk = SHRUBBERYLOOK2SPR;
			}
		}
	}
	ShrubberyThink(ob);
}

void ShrubberyWalkThink(CK_object *ob)
{
	// turn around to follow Keen
	if (ck_keenObj->clipRects.unitXmid > ob->clipRects.unitXmid)
		ob->xDirection = motion_Right;
	else
		ob->xDirection = motion_Left;

	// sit down when Keen is too far away
	if (!CK_ObjectVisible(ob)
		|| ABS((unsigned)(ob->clipRects.unitY2 - ck_keenObj->clipRects.unitY2)) > 3*TILEGLOBAL 
		|| ABS((unsigned)(ob->posX - ck_keenObj->posX)) > 5*TILEGLOBAL)
	{
		CK_SetAction2(ob, &CK_ACT_ShrubberySitDown);
	}
}

void ShrubberyContact(CK_object *ob, CK_object *hit)
{
	int ymove;

	switch (hit->type)
	{
	case CT_Player:
	//	if (ob->gfxChunk != SHRUBBERYSPR)
		{
			CK_StunKeen(ob->clipRects.unitXmid);
			// change shrubbery state to avoid stun-locking Keen:
			if (ob->currentAction != &CK_ACT_ShrubberyFall)
			{
				CK_SetAction2(ob, &CK_ACT_ShrubberySitDown);
				ob->timeUntillThink = (2*TickBase)/10;	//also wait 2 seconds before looking or standing up again
			}
		}
		break;
	case CT_Stunner:
		CK_ShotHit(hit);
		if (ob->gfxChunk < SHRUBBERYGETUPSPR)
		{
			CK_SetAction2(ob, &CK_ACT_Shrubbery);
			ob->timeUntillThink = (4*TickBase)/10;	// don't look or stand up for 4 seconds (CK_ACT_Shrubbery thinks every 10 tics)
			break;
		}
		ymove = ob->clipRects.unitY1;
		CK_SetAction2(ob, &CK_ACT_ShrubberyFall);

		// make sure the top of the bush/hitbox stays in place:
		if ((ymove -= ob->clipRects.unitY1) < 0)	// only move up, don't move down!
			CK_PhysUpdateY(ob, ymove);
		// Note: we can't move the bush down because then it might be pushed partially
		// into the ground, causing the engine to move it to the left or to the right
		// in an attempt to fix that. And we don't want that to happen.

		// jump up a bit:
		ob->timeUntillThink = 0;
		if ((ob->velY -= 0x18) < -0x30)
			ob->velY = -0x30;
		break;
	}
}

void ShrubberyFallReact(CK_object *ob)
{
	if (ob->topTI)
	{
		// object has hit the ground, back to waiting state
		CK_SetAction2(ob, &CK_ACT_Shrubbery);
		ob->velY = 0;
		ob->velX = 0;
		ob->timeUntillThink = (4*TickBase)/10;	// don't look or stand up for 4 seconds (CK_ACT_Shrubbery thinks every 10 tics)
	}
	CK_BasicDrawFunc1(ob);
}

CK_action CK_ACT_Shrubbery        = {SHRUBBERYSPR,        SHRUBBERYSPR,        AT_UnscaledOnce, false, push_down, 10,  0, 0, ShrubberyThink,     ShrubberyContact, CK_BasicDrawFunc1, &CK_ACT_Shrubbery};
CK_action CK_ACT_ShrubberyLook1   = {SHRUBBERYLOOK1SPR,   SHRUBBERYLOOK1SPR,   AT_UnscaledOnce, false, push_down, 10,  0, 0, ShrubberyThink,     ShrubberyContact, CK_BasicDrawFunc1, &CK_ACT_ShrubberyLook5};
CK_action CK_ACT_ShrubberyLook2   = {0,                   0,                   AT_Frame,        false, push_down, 10,  0, 0, ShrubberyLookThink, ShrubberyContact, CK_BasicDrawFunc1, NULL};
//CK_action CK_ACT_ShrubberyLook3   = {SHRUBBERYLOOK3SPR,   SHRUBBERYLOOK3SPR,   AT_UnscaledOnce, false, push_none, 10,  0, 0, ShrubberyThink,     ShrubberyContact, CK_BasicDrawFunc1, &CK_ACT_ShrubberyLook4};
//CK_action CK_ACT_ShrubberyLook4   = {SHRUBBERYLOOK4SPR,   SHRUBBERYLOOK4SPR,   AT_UnscaledOnce, false, push_none, 10,  0, 0, ShrubberyThink,     ShrubberyContact, CK_BasicDrawFunc1, &CK_ACT_ShrubberyLook5};
CK_action CK_ACT_ShrubberyLook5   = {SHRUBBERYLOOK1SPR,   SHRUBBERYLOOK1SPR,   AT_UnscaledOnce, false, push_down, 10,  0, 0, ShrubberyThink,     ShrubberyContact, CK_BasicDrawFunc1, &CK_ACT_Shrubbery};
CK_action CK_ACT_ShrubberyStandUp = {SHRUBBERYGETUPSPR,   SHRUBBERYGETUPSPR,   AT_UnscaledOnce, false, push_down, 10,  0, 0, NULL,               ShrubberyContact, CK_BasicDrawFunc1, &CK_ACT_ShrubberyWalk1};
CK_action CK_ACT_ShrubberyWalk1   = {SHRUBBERYSNEAKY1SPR, SHRUBBERYSNEAKY1SPR, AT_ScaledOnce,   false, push_down, 10,  8, 0, ShrubberyWalkThink, ShrubberyContact, CK_BasicDrawFunc2, &CK_ACT_ShrubberyWalk2};
CK_action CK_ACT_ShrubberyWalk2   = {SHRUBBERYSNEAKY2SPR, SHRUBBERYSNEAKY2SPR, AT_ScaledOnce,   false, push_down, 10,  8, 0, ShrubberyWalkThink, ShrubberyContact, CK_BasicDrawFunc2, &CK_ACT_ShrubberyWalk1};
CK_action CK_ACT_ShrubberySitDown = {SHRUBBERYGETUPSPR,   SHRUBBERYGETUPSPR,   AT_UnscaledOnce, false, push_down, 10,  0, 0, NULL,               ShrubberyContact, CK_BasicDrawFunc1, &CK_ACT_Shrubbery};
CK_action CK_ACT_ShrubberyFall    = {SHRUBBERYSHOTSPR,    SHRUBBERYSHOTSPR,    AT_Frame,        false, push_none, 10,  0, 0, CK_Fall,            ShrubberyContact, ShrubberyFallReact, &CK_ACT_ShrubberyFall};

/*
==============================================================================
	BOUNDER

	Hovers in place, randomly floats left or right, follows Keen after getting
	shot.

	user1 - initial y position (for hovering)
	user2 - remaining chase ticks
	user3 - amplitude
	user4 - slow acceleration counter
==============================================================================
*/

#define BOUNDER_FLOAT_VELX 4
#define BOUNDER_CHASE_VELX 24
#define BOUNDER_FLOAT_VELY 16
#define BOUNDER_BOUNCE_VELY 14
#define BOUNDER_ACCELERATES true
#define BOUNDER_MIN_AMPLITUDE 24
#define BOUNDER_ACCELERATION_DELAY 20

CK_action CK_ACT_BounderFloat1;
CK_action CK_ACT_BounderFloat2;
CK_action CK_ACT_BounderFloat1c;
CK_action CK_ACT_BounderFloat2c;
CK_action CK_ACT_BounderStunned;

void CK_SpawnBounder(int tileX, int tileY)
{
	CK_object *ob;

	ob = CK_GetNewObj(false);
	ob->type = CT_Bounder;
	//ob->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	ob->zLayer = 1;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY) - 9*PIXGLOBAL;
	ob->user1 = ob->posY - 2*TILEGLOBAL;
	ob->user3 = BOUNDER_MIN_AMPLITUDE;
	ob->xDirection = motion_None;
	ob->yDirection = motion_Up;
	CK_SetAction(ob, &CK_ACT_BounderFloat1c);
}

void BounderFloatThink(CK_object *ob)
{
	register int temp;

	// If we have a movement speed, the think function was already executed
	// for this frame and the animation (state change) caused the game to
	// execute it again. In that case, don't do anything.
	if (ck_nextX || ck_nextY)
		return;

	if (ob->gfxChunk != BOUNDERFLOATSHOTSPR)
	{
		if (ob->user2 > tics)
		{
			ob->user2 -= tics;
		}
		else
		{
			ob->user2 = 0;
		}
	}

#if BOUNDER_ACCELERATES
	temp = ob->user2? BOUNDER_CHASE_VELX : BOUNDER_FLOAT_VELX;
	if (ob->xDirection && ob->gfxChunk != BOUNDERFLOATSHOTSPR && abs(ob->velX) <= temp)
	{
		if (temp == BOUNDER_FLOAT_VELX)
		{
			if ((ob->user4 += tics) > 0)
			{
				ob->user4 -= BOUNDER_ACCELERATION_DELAY;
				ob->velX += ob->xDirection;
			}
			ck_nextX = ob->velX * tics;
		}
		else
			CK_PhysAccelHorz(ob, ob->xDirection, temp);
	}
#else
	if (ob->xDirection && ob->gfxChunk != BOUNDERFLOATSHOTSPR)
	{
		CK_PhysAccelHorz(ob, ob->xDirection, ob->user2? BOUNDER_CHASE_VELX : BOUNDER_FLOAT_VELX);
	}
#endif
	else
	{
		CK_PhysDampHorz(ob);
	}

	if (ob->yDirection == motion_Up && ob->posY < ob->user1-ob->user3)
	{
		ob->yDirection = motion_Down;
	}
	else if (ob->yDirection == motion_Down && ob->posY > ob->user1+ob->user3)
	{
		ob->yDirection = motion_Up;
	}

	temp = ob->velY;
	CK_PhysAccelVert(ob, ob->yDirection, BOUNDER_FLOAT_VELY);

	//if (((temp ^ ob->velY) & 0x8000) && temp >= 0 && ob->gfxChunk != BOUNDERFLOATSHOTSPR)
	if ((((temp ^ ob->velY) & 0x8000) && temp >= 0 || ob->topTI) && ob->gfxChunk != BOUNDERFLOATSHOTSPR)
	{
		int xdist;
		xdist = ck_keenObj->clipRects.unitXmid - ob->clipRects.unitXmid;
		if (ob->user2)
		{
			// chasing Keen
			temp = xdist;

			if (abs(xdist) > 15*TILEGLOBAL)
				ob->user2 = false;	// stop chasing Keen

			// we don't have a SIGN macro that leaves 0 as 0, so only use the macro on non-0 values
			if (temp)
				temp = SIGN(temp);
			ob->xDirection = temp;
		}
		else
		{
			// random movement
			temp = US_RndT();
			if (temp < 100)
			{
				temp = motion_Left;
			}
			else if (temp < 200)
			{
				temp = motion_Right;
			}
			else
			{
				temp = motion_None;
			}
			ob->xDirection = temp;
		}

#if (!BOUNDER_ACCELERATES)
		// I don't remember why I set the the velX value here. It prevents the
		// acceleration code above from working correctly and it uses different
		// speed values than the acceleration code.
		if (ob->user2)
			ob->velX = temp * 40;
		else
			ob->velX = temp * 20;
#endif

		// adjust amplitude based on xdist
		ob->user3 = 1*TILEGLOBAL - (abs(xdist) >> 2);	//TODO: fine-tune this!
		if (ob->user3 < BOUNDER_MIN_AMPLITUDE)
			ob->user3 = BOUNDER_MIN_AMPLITUDE;
	}

	if (ob->gfxChunk != BOUNDERFLOATSHOTSPR)
	{
		// change state according to new direction
		if (ob->xDirection != motion_None)
		{
			if (ob->velY < 0)
			{
				ob->currentAction = &CK_ACT_BounderFloat1;
			}
			else
			{
				ob->currentAction = &CK_ACT_BounderFloat2;
			}
		}
		else
		{
			if (ob->velY < 0)
			{
				ob->currentAction = &CK_ACT_BounderFloat1c;
			}
			else
			{
				ob->currentAction = &CK_ACT_BounderFloat2c;
			}
		}
	}
}

void BounderContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		CK_KillKeen();
		break;
	case CT_Stunner:
		CK_ShotHit(hit);
		CK_SetAction2(ob, &CK_ACT_BounderStunned);
		/*
		for (ob=ck_keenObj; ob; ob=ob->next)
		{
			if (ob->type == CT_Bounder)
			{
				*/
		ob->user2 = 9*TickBase;	// start chasing Keen for 9 seconds
		ob->user3 = BOUNDER_MIN_AMPLITUDE;/*
			}
		}*/
		break;
	}
}

void BounderReact(CK_object *ob)
{
#if BOUNDER_ACCELERATES
	if (ob->topTI && ob->velY > 0 || ob->bottomTI && ob->velY < 0)
	{
		if ((ob->velY = -ob->velY) < 0)
			ob->yDirection = motion_Up;
		else
			ob->yDirection = motion_Down;
		
		// bounce off the floor/ceiling:
		if (((ob->user1 - ob->posY) ^ (ob->yDirection)) & 0x8000)
			ob->velY = ob->yDirection * BOUNDER_BOUNCE_VELY;
	}
	if (ob->leftTI && ob->velX > 0 || ob->rightTI && ob->velX < 0)
	{
		ob->velX = -ob->velX;
		if (ob->xDirection != motion_None)
		{
			if (ob->velX < 0)
				ob->xDirection = motion_Left;
			else
				ob->xDirection = motion_Right;
		}
	}
#else
	if (ob->topTI || ob->bottomTI)
	{
		ob->yDirection = -ob->yDirection;
		ob->velY = -ob->velY;
	}
	else if (ob->leftTI || ob->rightTI)
	{
		ob->xDirection = -ob->xDirection;
		ob->velX = -ob->velX;
	}
#endif
	CK_BasicDrawFunc1(ob);
}

CK_action CK_ACT_BounderFloat1 =  {BOUNDERFLOATL1SPR,   BOUNDERFLOATR1SPR,   AT_Frame, false, push_none, 10, 0, 0, BounderFloatThink, BounderContact, BounderReact, &CK_ACT_BounderFloat1};
CK_action CK_ACT_BounderFloat2 =  {BOUNDERFLOATL2SPR,   BOUNDERFLOATR2SPR,   AT_Frame, false, push_none, 10, 0, 0, BounderFloatThink, BounderContact, BounderReact, &CK_ACT_BounderFloat2};
CK_action CK_ACT_BounderFloat1c = {BOUNDERFLOAT1SPR,    BOUNDERFLOAT1SPR,    AT_Frame, false, push_none, 10, 0, 0, BounderFloatThink, BounderContact, BounderReact, &CK_ACT_BounderFloat1c};
CK_action CK_ACT_BounderFloat2c = {BOUNDERFLOAT2SPR,    BOUNDERFLOAT2SPR,    AT_Frame, false, push_none, 10, 0, 0, BounderFloatThink, BounderContact, BounderReact, &CK_ACT_BounderFloat2c};
CK_action CK_ACT_BounderStunned = {BOUNDERFLOATSHOTSPR, BOUNDERFLOATSHOTSPR, AT_ScaledFrame, false, push_none, 100, 0, 0, BounderFloatThink, BounderContact, BounderReact, &CK_ACT_BounderFloat1c};


/*
==============================================================================
	TENTACLE

	Stands in place until Keen comes near, then it moves out of its hole and
	slams the ground in an attempt to hurt Keen. Has a 3 second delay after
	each attack.
==============================================================================
*/

CK_action CK_ACT_TentacleWait;
CK_action CK_ACT_TentacleAttack1;
CK_action CK_ACT_TentacleAttack2;
CK_action CK_ACT_TentacleAttack3;
CK_action CK_ACT_TentacleAttack4;
CK_action CK_ACT_TentacleAttack5;
CK_action CK_ACT_TentacleAttack6;
CK_action CK_ACT_TentacleAttack7;
CK_action CK_ACT_TentacleAttack8;
CK_action CK_ACT_TentacleAttack9;

void CK_SpawnTentacle(int tileX, int tileY)
{
	CK_object *ob;

	ob = CK_GetNewObj(false);
	ob->type = CT_Tentacle;
//	ob->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	ob->zLayer = 1;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	ob->clipped = CLIP_not;
	CK_SetAction(ob, &CK_ACT_TentacleWait);
}

void TentacleThink(CK_object *ob)
{
	int xdist =  ck_keenObj->clipRects.unitXmid - ob->clipRects.unitXmid;
	if (ABS(ob->posY - ck_keenObj->posY) < 6*TILEGLOBAL)
	{
		if (xdist < 0 && xdist >= (int)(-3.5*TILEGLOBAL))
		{
			ob->xDirection = motion_Left;
			ob->currentAction = &CK_ACT_TentacleAttack1;
		}
		else if (xdist >= 0 && xdist <= (int)(3.5*TILEGLOBAL))
		{
			ob->xDirection = motion_Right;
			ob->currentAction = &CK_ACT_TentacleAttack1;
		}
	}
}

#pragma argsused
void TentacleAttackThink(CK_object *ob)
{
	SD_PlaySound(TENTACLESWOOSHSND);
}

void TentacleContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		CK_KillKeen();
		break;
	case CT_Stunner:
		CK_ShotHit(hit);	// shot explodes but doesn't do anything else
		break;
	}
}

CK_action CK_ACT_TentacleWait    = {TENTACLESPR,   TENTACLESPR,   AT_Frame,        false, push_none, 0, 0, 0, TentacleThink, NULL, CK_BasicDrawFunc1, &CK_ACT_TentacleWait};
CK_action CK_ACT_TentacleAttack1 = {TENTACLEL1SPR, TENTACLER1SPR, AT_UnscaledOnce, false, push_none, 10, 0, 0, NULL, TentacleContact, CK_BasicDrawFunc1, &CK_ACT_TentacleAttack2};
CK_action CK_ACT_TentacleAttack2 = {TENTACLEL2SPR, TENTACLER2SPR, AT_UnscaledOnce, false, push_none, 8, 0, 0, NULL, TentacleContact, CK_BasicDrawFunc1, &CK_ACT_TentacleAttack3};
CK_action CK_ACT_TentacleAttack3 = {TENTACLEL3SPR, TENTACLER3SPR, AT_UnscaledOnce, false, push_none, 4, 0, 0, TentacleAttackThink, TentacleContact, CK_BasicDrawFunc1, &CK_ACT_TentacleAttack4};
CK_action CK_ACT_TentacleAttack4 = {TENTACLEL4SPR, TENTACLER4SPR, AT_UnscaledOnce, false, push_none, 4, 0, 0, NULL, TentacleContact, CK_BasicDrawFunc1, &CK_ACT_TentacleAttack5};
CK_action CK_ACT_TentacleAttack5 = {TENTACLEL5SPR, TENTACLER5SPR, AT_UnscaledOnce, false, push_none, 8, 0, 0, NULL, TentacleContact, CK_BasicDrawFunc1, &CK_ACT_TentacleAttack6};
CK_action CK_ACT_TentacleAttack6 = {TENTACLEL6SPR, TENTACLER6SPR, AT_UnscaledOnce, false, push_none, 8, 0, 0, NULL, TentacleContact, CK_BasicDrawFunc1, &CK_ACT_TentacleAttack7};
CK_action CK_ACT_TentacleAttack7 = {TENTACLEL7SPR, TENTACLER7SPR, AT_UnscaledOnce, false, push_none, 8, 0, 0, NULL, TentacleContact, CK_BasicDrawFunc1, &CK_ACT_TentacleAttack8};
CK_action CK_ACT_TentacleAttack8 = {TENTACLEL1SPR, TENTACLER1SPR, AT_UnscaledOnce, false, push_none, 14, 0, 0, NULL, TentacleContact, CK_BasicDrawFunc1, &CK_ACT_TentacleAttack9};
CK_action CK_ACT_TentacleAttack9 = {TENTACLESPR,   TENTACLESPR,   AT_UnscaledOnce, false, push_none, 210, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_TentacleWait};

/*
==============================================================================
	POLEKNAWEL

	Slides down to the bottom of a pole and randomly throws projectiles. Falls
	to the ground when shot. Walks on the ground and randomly throws
	projectiles. Can also get on poles again.
	
	user4 - Nisasi pointer
==============================================================================
*/

CK_action CK_ACT_PoleknawelPole1;
CK_action CK_ACT_PoleknawelPole2;
CK_action CK_ACT_PoleknawelThrow;
CK_action CK_ACT_PoleknawelFall;
CK_action CK_ACT_PoleknawelLand;
CK_action CK_ACT_PoleknawelLook;
CK_action CK_ACT_PoleknawelWalk1;
CK_action CK_ACT_PoleknawelWalk2;
CK_action CK_ACT_PoleknawelPoop;
CK_action CK_ACT_PoleknawelShot1;
CK_action CK_ACT_PoleknawelShot2;
CK_action CK_ACT_PoleknawelPool1;
CK_action CK_ACT_PoleknawelPool2;


void CK_SpawnPoleknawel(int tileX, int tileY)
{
	CK_object *ob;

	ob = CK_GetNewObj(false);
	ob->type = CT_Poleknawel;
//	ob->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	ob->zLayer = 1;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	ob->xDirection = US_RndT() < 0x80? motion_Left : motion_Right;
	if ((TI_ForeMisc(CA_TileAtPos(tileX, tileY, 1)) & 0x7F) == MF_Pole)
	{
		ob->posX -= 0x78;
		ob->clipped = CLIP_not;
		CK_SetAction(ob, &CK_ACT_PoleknawelPole1);
	}
	else
	{
	//	ob->clipped = CLIP_normal;	//redundant - GetNewObj always sets clipped to CLIP_normal
		CK_SetAction(ob, &CK_ACT_PoleknawelLook);
	}
}

void PoleknawelLand(CK_object *ob)
{
#ifdef EXTRA_INTERACTIONS 
	// only start looking when NOT grabbed by a Nisasi:
	if (!ob->user4)
	{
		ob->currentAction = &CK_ACT_PoleknawelLook;
	}
#endif
	ob->currentAction = &CK_ACT_PoleknawelLook;
}

void PoleknawelWalk(CK_object *ob)
{
	// Note: the distance from the middle of the tile in the following check should match the walking speed of the Poleknawel.
	if (ABS((ob->clipRects.unitXmid & 0xFF) - TILEGLOBAL/2) <= 0x40	//middle of the hitbox is near the middle of the tile
		&& (TI_ForeMisc(CA_TileAtPos(ob->clipRects.tileXmid, ob->clipRects.tileY2, 1)) & 0x7F) == MF_Pole)
	{
		ob->currentAction = &CK_ACT_PoleknawelPole1;
		ck_nextX = TILEGLOBAL/2 - (ob->clipRects.unitXmid & 0xFF);	//move to the middle of the tile
		ob->clipped = CLIP_not;
	}
	else
	{
		register int rnd;	// cannot use unsigned here!
		
		rnd = US_RndT();
		if (!(ck_gameState.difficulty == D_Easy))
			rnd -= 0x10;
		
		if (rnd < 0x06)
		{
		ck_nextX = 0;	// no movement on state transition!
		ob->currentAction = &CK_ACT_PoleknawelPoop;
		ob->xDirection = ob->posX < ck_keenObj->posX ? motion_Right : motion_Left;
		SD_PlaySound(EGGCRACKSND);
		}
	}
}

void PoleknawelSlide(CK_object *ob)
{
	if ((TI_ForeMisc(CA_TileAtPos(ob->clipRects.tileXmid, RF_UnitToTile(ob->clipRects.unitY2+0x20), 1)) & 0x7F) == MF_Pole)
	{
		ck_nextY = 0x20;
	}
	else
	{
		register int rnd;	// cannot use unsigned here!
		
		rnd = US_RndT();
		if (!(ck_gameState.difficulty == D_Easy))
			rnd -= 0x10;
		
		if (rnd < 0x06)
		// throw a projectile
		ob->currentAction = &CK_ACT_PoleknawelThrow;
		//TODO: play a sound here?
	}
}

void PoleknawelThrow(CK_object *ob)
{
	CK_object *knawl = CK_GetNewObj(true);
	knawl->type = CT_Friendly;
	knawl->active = OBJ_EXISTS_ONLY_ONSCREEN;
	knawl->zLayer = 0;
	knawl->posX = ob->clipRects.unitXmid - 7*PIXGLOBAL;
	knawl->yDirection = motion_Down;

	if (ob->currentAction == &CK_ACT_PoleknawelPoop)
	{
		knawl->posY = ob->clipRects.unitY1;

		knawl->velY = -32;
		knawl->velX = 32*ob->xDirection;
	}
	else
	{
		knawl->posY = ob->clipRects.unitY2;

		// shoot in a random direction (and with random speed):
		knawl->velX = (US_RndT()-0x80)/4;	// random speed between -32 and 31
	}

	/*
	// shoot in Keen's direction:
	if (ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid - TILEGLOBAL)
	{
		knawl->xDirection = motion_Left;
		knawl->velX = -0x18;
	}
	else if (ck_keenObj->clipRects.unitXmid > ob->clipRects.unitXmid + TILEGLOBAL)
	{
		knawl->xDirection = motion_Right;
		knawl->velX = 0x18;
	}
	// otherwise the shot flies straight down (values for this are already set by CK_GetNewObj)
	*/
	CK_SetAction(knawl, &CK_ACT_PoleknawelShot1);
	SD_PlaySound(EGGCRACKSND);
}
/*
void PoleknawelPoop(CK_object *ob)
{
	CK_object *slime = CK_GetNewObj(true);
	slime->type = CT_Friendly;
	slime->active = OBJ_EXISTS_ONLY_ONSCREEN;
	slime->zLayer = 0;
	slime->posX = ob->clipRects.unitXmid - 8*PIXGLOBAL;
	slime->posY = ob->clipRects.unitY2 - 7*PIXGLOBAL;
	CK_SetAction(slime, &CK_ACT_PoleknawelPool1);
}
*/
void PoleknawelContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		CK_KillKeen();
		break;
	case CT_Stunner:
		CK_ShotHit(hit);
		CK_ObjDropNisasi(ob);
		ob->clipped = CLIP_normal;
		CK_SetAction2(ob, &CK_ACT_PoleknawelFall);

		// Make the creature jump up a bit
		if ((ob->velY -= 0x18) < -0x30)
			ob->velY = -0x30;
		ob->timeUntillThink = 0;	// allow it to jump without delay
		break;
	}
}

void PoleknawelWalkReact(CK_object *ob)
{
	CK_BasicDrawFunc2(ob);
	if (!ob->topTI)
	{
		ob->timeUntillThink = 0;
		CK_SetAction2(ob, &CK_ACT_PoleknawelFall);
	}
}

void PoleknawelFallReact(CK_object *ob)
{
	if (ob->topTI)
	{
		ob->velY = 0;
		CK_SetAction2(ob, &CK_ACT_PoleknawelLand);
	}
	else if (ob->bottomTI)
	{
		ob->velY = 0;
	}
	CK_BasicDrawFunc1(ob);
}

void KnawlReact(CK_object *ob)
{
	if (ob->topTI)	// hit the ground ?
	{
		// turn into pool when hitting the ground:
		CK_SetAction2(ob, &CK_ACT_PoleknawelPool1);

		/*
		// bounce off the ground and remove after x bounces:
		ob->velY = -ob->velY / 2;	// bounce back at half the speed
		if (++ob->user1 > 2)	//remove when it hits the ground for the 2nd time
		{
			CK_RemoveObj(ob);
			return;
		}
		*/
	}
	if (ob->bottomTI)	// hit the ceiling?
	{
		ob->velY = -ob->velY / 2;	// bounce back at half the speed
	}
	if (ob->leftTI || ob->rightTI)	// hit a wall?
	{
		ob->velX = -ob->velX / 2;	// bounce back at half the speed
	}
	CK_BasicDrawFunc1(ob);
}

CK_action CK_ACT_PoleknawelPole1 = {POLEKNAWEL1SPR, POLEKNAWEL1SPR, AT_UnscaledOnce, false, push_none, 8, 0, 0, NULL, PoleknawelContact, CK_BasicDrawFunc1, &CK_ACT_PoleknawelPole2};
CK_action CK_ACT_PoleknawelPole2 = {POLEKNAWEL2SPR, POLEKNAWEL2SPR, AT_UnscaledOnce, false, push_none, 8, 0, 0, PoleknawelSlide, PoleknawelContact, CK_BasicDrawFunc1, &CK_ACT_PoleknawelPole1};
CK_action CK_ACT_PoleknawelThrow = {POLEKNAWELSHOOTSPR, POLEKNAWELSHOOTSPR, AT_UnscaledOnce, false, push_none, 60, 0, 0, PoleknawelThrow, PoleknawelContact, CK_BasicDrawFunc1, &CK_ACT_PoleknawelPole1};
CK_action CK_ACT_PoleknawelFall  = {POLEKNAWELFALLSPR, POLEKNAWELFALLSPR, AT_Frame, false, push_none, 0, 0, 0, CK_Fall, PoleknawelContact, PoleknawelFallReact, NULL};
CK_action CK_ACT_PoleknawelLand  = {POLEKNAWELLANDSPR, POLEKNAWELLANDSPR, AT_UnscaledOnce, false, push_none, 140, 0, 0, PoleknawelLand, PoleknawelContact, CK_BasicDrawFunc1, &CK_ACT_PoleknawelLand};
CK_action CK_ACT_PoleknawelLook  = {POLEKNAWELLOOKSPR, POLEKNAWELLOOKSPR, AT_UnscaledOnce, false, push_none, 20, 0, 0, NULL, PoleknawelContact, CK_BasicDrawFunc1, &CK_ACT_PoleknawelWalk1};
CK_action CK_ACT_PoleknawelWalk1 = {POLEKNAWELWALK1SPR, POLEKNAWELWALK1SPR, AT_UnscaledOnce, false, push_down, 8, 0x40, 0, NULL, PoleknawelContact, PoleknawelWalkReact, &CK_ACT_PoleknawelWalk2};
CK_action CK_ACT_PoleknawelWalk2 = {POLEKNAWELWALK2SPR, POLEKNAWELWALK2SPR, AT_UnscaledOnce, false, push_down, 8, 0x40, 0, PoleknawelWalk, PoleknawelContact, PoleknawelWalkReact, &CK_ACT_PoleknawelWalk1};
CK_action CK_ACT_PoleknawelPoop  = {POLEKNAWELPOOPSPR, POLEKNAWELPOOPSPR, AT_UnscaledOnce, false, push_down, 60, 0x40, 0, PoleknawelThrow, PoleknawelContact, PoleknawelWalkReact, &CK_ACT_PoleknawelWalk1};
CK_action CK_ACT_PoleknawelShot1 = {KNAWL1SPR, KNAWL1SPR, AT_ScaledFrame, false, push_none, 8, 0, 0, CK_Fall, CK_DeadlyCol2, KnawlReact, &CK_ACT_PoleknawelShot2};
CK_action CK_ACT_PoleknawelShot2 = {KNAWL2SPR, KNAWL2SPR, AT_ScaledFrame, false, push_none, 8, 0, 0, CK_Fall, CK_DeadlyCol2, KnawlReact, &CK_ACT_PoleknawelShot1};
CK_action CK_ACT_PoleknawelPool1 = {KNAWL3SPR, KNAWL3SPR, AT_UnscaledOnce, false, push_none, 300, 0, 0, NULL, CK_SlugSlimeCol, CK_BasicDrawFunc1, &CK_ACT_PoleknawelPool2};
CK_action CK_ACT_PoleknawelPool2 = {KNAWL4SPR, KNAWL4SPR, AT_UnscaledOnce, false, push_none, 60, 0, 0, NULL, CK_SlugSlimeCol, CK_BasicDrawFunc1, NULL};

/*
==============================================================================
	KITTEN

	Sits on its fat butt and meows at random. Starts rolling towards Keen
	when getting shot or when Keen is close. Explodes when touching Keen.

	user4 - Nisasi pointer
==============================================================================
*/

CK_action CK_ACT_KittenIdle1;
CK_action CK_ACT_KittenMeow1;
CK_action CK_ACT_KittenMeow2;
CK_action CK_ACT_KittenMeow3;
CK_action CK_ACT_KittenRoll1;
CK_action CK_ACT_KittenRoll2;
CK_action CK_ACT_KittenRoll3;
CK_action CK_ACT_KittenRoll4;
CK_action CK_ACT_KittenRoll5;
CK_action CK_ACT_KittenBlow1;
CK_action CK_ACT_KittenBlow2;
CK_action CK_ACT_KittenBlow3;
CK_action CK_ACT_KittenSmoke1;
CK_action CK_ACT_KittenSmoke2;
CK_action CK_ACT_KittenSmoke3;
CK_action CK_ACT_KittenSmoke4;
CK_action CK_ACT_KittenSmoke5;
CK_action CK_ACT_KittenSmoke6;

void CK_SpawnKitten(int tileX, int tileY)
{
	CK_object *ob;

	ob = CK_GetNewObj(false);
	ob->type = CT_Kitten;
//	ob->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	ob->zLayer = 1;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY) - 10*PIXGLOBAL;
	ob->xDirection = US_RndT() < 0x80? motion_Left : motion_Right;
	ob->yDirection = motion_Down;
//	ob->clipped = CLIP_normal;	//redundant - GetNewObj always sets clipped to CLIP_normal
	CK_SetAction(ob, &CK_ACT_KittenIdle1);
}

void KittenIdleThink(CK_object *ob)
{
	// start rolling when Keen is close:
	if (ABS(ck_keenObj->clipRects.unitXmid - ob->clipRects.unitXmid) < 8*TILEGLOBAL
		&& ABS(ck_keenObj->posY - ob->posY) < 3*TILEGLOBAL)
	{
		CK_ObjDropNisasi(ob);
		CK_SetAction2(ob, &CK_ACT_KittenRoll1);
	}
	else if (ob->currentAction == &CK_ACT_KittenIdle1 && CK_ObjectVisible(ob))
	{
		if (US_RndT() == 4)
		{
			SD_PlaySound(KITTENMEOWSND);
			ob->currentAction = &CK_ACT_KittenMeow1;
		}
	}
}

void KittenRollThink(CK_object *ob)
{
	// stop rolling when too far away from Keen:
	if (ABS(ck_keenObj->clipRects.unitXmid - ob->clipRects.unitXmid) > 9*TILEGLOBAL
		|| ABS(ck_keenObj->posY - ob->posY) > 5*TILEGLOBAL)
	{
		CK_SetAction2(ob, &CK_ACT_KittenIdle1);
	}
	else
	{
		if (ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid)
			ob->xDirection = motion_Left;
		else
			ob->xDirection = motion_Right;
	}
}

#pragma argsused
void KittenExplodeThink(CK_object *ob)
{
	SD_PlaySound(KITTENEXPLODESND);
}

void KittenContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		CK_SetAction2(ob, &CK_ACT_KittenBlow1);
		ob->clipped = CLIP_not;	// no clipping for the smoke cloud
		ob->timeUntillThink = 0;	// allow Kitten to think, otherwise the explosion sound might not play
		CK_ObjDropNisasi(ob);
		break;
	case CT_Stunner:
		CK_ShotHit(hit);
		CK_SetAction2(ob, &CK_ACT_KittenRoll1);
		ob->timeUntillThink = 20;
		CK_ObjDropNisasi(ob);
		break;
	}
}

CK_action CK_ACT_KittenIdle1 = {KITTENSTANDSPR,  KITTENSTANDSPR,  AT_Frame, false, push_down, 0, 0, 0, KittenIdleThink, KittenContact, CK_BasicDrawFunc2, NULL};
CK_action CK_ACT_KittenMeow1 = {KITTENMEOW1SPR,  KITTENMEOW1SPR,  AT_UnscaledOnce, false, push_down, 8, 0, 0, KittenIdleThink, KittenContact, CK_BasicDrawFunc2, &CK_ACT_KittenMeow2};
CK_action CK_ACT_KittenMeow2 = {KITTENMEOW2SPR,  KITTENMEOW2SPR,  AT_UnscaledOnce, false, push_down, 8, 0, 0, KittenIdleThink, KittenContact, CK_BasicDrawFunc2, &CK_ACT_KittenMeow3};
CK_action CK_ACT_KittenMeow3 = {KITTENMEOW3SPR,  KITTENMEOW3SPR,  AT_UnscaledOnce, false, push_down, 8, 0, 0, KittenIdleThink, KittenContact, CK_BasicDrawFunc2, &CK_ACT_KittenIdle1};
CK_action CK_ACT_KittenRoll1 = {KITTENWALKL1SPR, KITTENWALKR1SPR, AT_UnscaledOnce, false, push_down, 8, 0x80, 0, KittenRollThink, KittenContact, CK_BasicDrawFunc2, &CK_ACT_KittenRoll2};
CK_action CK_ACT_KittenRoll2 = {KITTENWALKL2SPR, KITTENWALKR2SPR, AT_UnscaledOnce, false, push_down, 8, 0x80, 0, NULL, KittenContact, CK_BasicDrawFunc2, &CK_ACT_KittenRoll3};
CK_action CK_ACT_KittenRoll3 = {KITTENWALKL3SPR, KITTENWALKR3SPR, AT_UnscaledOnce, false, push_down, 8, 0x80, 0, NULL, KittenContact, CK_BasicDrawFunc2, &CK_ACT_KittenRoll4};
CK_action CK_ACT_KittenRoll4 = {KITTENWALKL4SPR, KITTENWALKR4SPR, AT_UnscaledOnce, false, push_down, 8, 0x80, 0, NULL, KittenContact, CK_BasicDrawFunc2, &CK_ACT_KittenRoll5};
CK_action CK_ACT_KittenRoll5 = {KITTENWALKL5SPR, KITTENWALKR5SPR, AT_UnscaledOnce, false, push_down, 8, 0x80, 0, NULL, KittenContact, CK_BasicDrawFunc2, &CK_ACT_KittenRoll1};
CK_action CK_ACT_KittenBlow1 = {KITTENBLOWL1SPR, KITTENBLOWR1SPR, AT_UnscaledOnce, false, push_none, 9, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_KittenBlow2};
CK_action CK_ACT_KittenBlow2 = {KITTENBLOWL2SPR, KITTENBLOWR2SPR, AT_UnscaledOnce, false, push_none, 9, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_KittenBlow3};
CK_action CK_ACT_KittenBlow3 = {KITTENBLOWL3SPR, KITTENBLOWR3SPR, AT_UnscaledOnce, false, push_none, 9, 0, 0, KittenExplodeThink, NULL, CK_BasicDrawFunc1, &CK_ACT_KittenSmoke1};
CK_action CK_ACT_KittenSmoke1 = {SURGE1SPR, SURGE1SPR, AT_UnscaledOnce, false, push_none, 8, 0, 0, NULL, CK_DeadlyCol, CK_BasicDrawFunc1, &CK_ACT_KittenSmoke2};
CK_action CK_ACT_KittenSmoke2 = {SURGE2SPR, SURGE2SPR, AT_UnscaledOnce, false, push_none, 8, 0, 0, NULL, CK_DeadlyCol, CK_BasicDrawFunc1, &CK_ACT_KittenSmoke3};
CK_action CK_ACT_KittenSmoke3 = {SURGE3SPR, SURGE3SPR, AT_UnscaledOnce, false, push_none, 8, 0, 0, NULL, CK_DeadlyCol, CK_BasicDrawFunc1, &CK_ACT_KittenSmoke4};
CK_action CK_ACT_KittenSmoke4 = {SURGE4SPR, SURGE4SPR, AT_UnscaledOnce, false, push_none, 8, 0, 0, NULL, CK_DeadlyCol, CK_BasicDrawFunc1, &CK_ACT_KittenSmoke5};
CK_action CK_ACT_KittenSmoke5 = {SURGE5SPR, SURGE5SPR, AT_UnscaledOnce, false, push_none, 8, 0, 0, NULL, CK_DeadlyCol, CK_BasicDrawFunc1, &CK_ACT_KittenSmoke6};
CK_action CK_ACT_KittenSmoke6 = {SURGE6SPR, SURGE6SPR, AT_UnscaledOnce, false, push_none, 8, 0, 0, NULL, CK_DeadlyCol, CK_BasicDrawFunc1, NULL};

/*
==============================================================================
	GELLYGHOST

	Floats up and down. Stuns Keen on touch. Can be pushed by shooting it.

	user1 - bottom y
	user2 - visibility state (see enum)
==============================================================================
*/

#define GG_UP_SPEED 18

enum {GG_NORMAL, GG_VANISH, GG_APPEAR, GG_HIDDEN};

CK_action CK_ACT_GGhostIdle;

void CK_SpawnGellyGhost(int tileX, int tileY)
{
	CK_object *ob;

	ob = CK_GetNewObj(false);
	ob->type = CT_GellyGhost;
//	ob->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	ob->zLayer = 1;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = ob->user1 = RF_TileToUnit(tileY);
	ob->xDirection = US_RndT() < 0x80? motion_Left : motion_Right;
	ob->yDirection = motion_Down;
	ob->velY = 4;
//	ob->clipped = CLIP_normal;	//redundant - GetNewObj always sets clipped to CLIP_normal
	ob->gfxChunk = GELLYGHOSTFADE4SPR;
	ob->user2 = GG_HIDDEN;
	CK_SetAction(ob, &CK_ACT_GGhostIdle);
}

void GellyGhostThink(CK_object *ob)
{
	int dx, dy;

	if (ob->velY == 0)
	{
		ob->velY = 6;
		ob->yDirection = motion_Down;
		if (ob->velX == 0)
		{
			ob->xDirection = -ob->xDirection;
		}
	}
	else if (ob->posY >= ob->user1)
	{
		ob->velY = -GG_UP_SPEED;
		ob->yDirection = motion_Up;
	}

	if (ob->velY < 0)
	{
		CK_PhysDampVert(ob);
		CK_PhysDampHorz(ob);
	}
	else
	{
		ck_nextY = ob->velY * tics;
		if (ob->velX < -4 || ob->velX > 4)
			CK_PhysDampHorz(ob);
		else
			CK_PhysAccelHorz(ob, ob->xDirection, 4);
	}

	//
	// change frame to match current state
	//

	dx = ob->clipRects.unitXmid - ck_keenObj->clipRects.unitXmid;
	dy = ob->clipRects.unitY2 - ck_keenObj->clipRects.unitY2;
	if (dx < 0)
		dx = -dx;
	if (dy < 0)
		dy = -dy;

	if (dx > 5*TILEGLOBAL || dy > 5*TILEGLOBAL)
	{
		if (ob->user2 != GG_VANISH && ob->user2 != GG_HIDDEN)
		{
			ob->user2 = GG_VANISH;
			if (ob->gfxChunk < GELLYGHOSTFADE1SPR)
				ob->gfxChunk = GELLYGHOSTFADE1SPR;
			ob->actionTimer = 0;
		}
	}
	else if (dx < 4*TILEGLOBAL && dy < 4*TILEGLOBAL)
	{
		if (ob->user2 == GG_VANISH || ob->user2 == GG_HIDDEN)
		{
			ob->user2 = GG_APPEAR;
			if (ob->gfxChunk < GELLYGHOSTFADE1SPR)
				ob->gfxChunk = GELLYGHOSTFADE4SPR;
			ob->actionTimer = 0;
		}
	}

	if (ob->user2 == GG_VANISH)
	{
		if ((ob->actionTimer += tics) >= 10)
		{
			ob->actionTimer -= 10;
			if (ob->gfxChunk == GELLYGHOSTFADE4SPR)
			{
				ob->user2 = GG_HIDDEN;
			}
			else
			{
				ob->gfxChunk++;
			}
		}
	}
	else if (ob->user2 == GG_APPEAR)
	{
		if ((ob->actionTimer += tics) >= 10)
		{
			ob->actionTimer -= 10;
			if (ob->gfxChunk-- == GELLYGHOSTFADE1SPR)
				ob->user2 = GG_NORMAL;
		}
	}

	if (ob->user2 != GG_NORMAL)
		return;

	if (ob->velX <= -16)
		ob->gfxChunk = GELLYGHOSTSHOTL1SPR;
	else if (ob->velX >= 16)
		ob->gfxChunk = GELLYGHOSTSHOTR1SPR;
	else if (ob->velX < -8)
		ob->gfxChunk = GELLYGHOSTSHOTL2SPR;
	else if (ob->velX > 8)
		ob->gfxChunk = GELLYGHOSTSHOTR2SPR;
	else
	{
		if (ob->xDirection == motion_Left)
		{
			if (ob->posY >= ob->user1-2*PIXGLOBAL)
			{
				ob->gfxChunk = GELLYGHOSTFLOATL2SPR;
			}
			else if (ob->velY < -8)
			{
				ob->gfxChunk = GELLYGHOSTFLOATL3SPR;
			}
			else if (ob->velY < 0)
			{
				ob->gfxChunk = GELLYGHOSTFLOATL4SPR;
			}
			else
			{
				ob->gfxChunk = GELLYGHOSTFLOATL1SPR;
			}
		}
		else
		{
			if (ob->posY >= ob->user1-2*PIXGLOBAL)
			{
				ob->gfxChunk = GELLYGHOSTFLOATR2SPR;
			}
			else if (ob->velY < -8)
			{
				ob->gfxChunk = GELLYGHOSTFLOATR3SPR;
			}
			else if (ob->velY < 0)
			{
				ob->gfxChunk = GELLYGHOSTFLOATR4SPR;
			}
			else
			{
				ob->gfxChunk = GELLYGHOSTFLOATR1SPR;
			}
		}
	}
}

void GellyGhostContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		CK_StunKeen(ob->clipRects.unitXmid);
		break;
	case CT_Stunner:
		// push ghost in shot direction and adjust user1 (bottom y)
		if (hit->xDirection == motion_Left)
		{
			ob->xDirection = motion_Right;
			ob->velX -= 24;
			if (ob->velX < -32)
				ob->velX = -32;
		}
		else if (hit->xDirection == motion_Right)
		{
			ob->xDirection = motion_Left;
			ob->velX += 24;
			if (ob->velX > 32)
				ob->velX = 32;
		}
		else if (hit->yDirection == motion_Up)
		{
			ob->yDirection = motion_Up;
			ob->velY -= 24;
			ob->user1 -= 3*TILEGLOBAL;
		}
		else if (hit->yDirection == motion_Down)
		{
			ob->yDirection = motion_Down;
			ob->velY += 24;
			ob->user1 += 3*TILEGLOBAL;
		}
		CK_ShotHit(hit);
		break;
	}
}

void GellyGhostReact(CK_object *ob)
{	
	if (ob->topTI)
	{
		ob->user1 = ob->posY - 4*PIXGLOBAL;
	}
	if (ob->bottomTI)
	{
		ob->velY = 0;
		ob->user1 = ob->posY + GG_UP_SPEED*GG_UP_SPEED;
	}
	if (ob->leftTI || ob->rightTI)
	{
		ob->velX = 0;
	}

	if (ob->user2 == GG_HIDDEN)
		RF_RemoveSprite(&ob->sde);
	else
		CK_BasicDrawFunc1(ob);
}

CK_action CK_ACT_GGhostIdle = {0, 0, AT_Frame, false, push_none, 0, 0, 0, GellyGhostThink, GellyGhostContact, GellyGhostReact, NULL};

#undef GG_UP_SPEED

/*
==============================================================================
	COLLECTIBLES

	Special GOODIES that Keen can collect. They can be spawned as regular items
	in the level or as non-interactive items in the BWB level (if collected).

	user1 - unique bit for the item (1 << itemnumber)
	user2 - first sprite (same as for regular bonus items)
	user3 - last sprite + 1 (same as for regular bonus items)
==============================================================================
*/

// Note: Keen touching the item is handled in CK_KeenColFunc() in CK_KEEN.C !

void CK_SpawnCollectible(int tileX, int tileY, int number)
{
	int bit;
	CK_object *ob;

	bit = 1 << number;

	if (ck_gameState.currentLevel == MAP_BWBMEGAROCKET)
	{
		// we're in the BWB level here

		// offsets to allow pixel-perfect positions in BWB cockpit:
		// (these are in pixel units, relative to the tile position)
		// Yorp, Bear, Keen, Vort, Garg, Mort, 
		static far int xoffs[6] = {-3, +3, -6, +1, +0, +3};
		static far int yoffs[6] = {-2, -3, +6, +0, -5, +1};

		// don't spawn the collectible if the player hasn't collected it:
		if (!(ck_gameState.collectiblesTotal & bit))
			return;

		ob = CK_GetNewObj(false);
		ob->clipped = CLIP_not;
		ob->type = CT_Friendly;
		ob->posX = RF_TileToUnit(tileX) + RF_PixelToUnit(xoffs[number]);
		ob->posY = RF_TileToUnit(tileY) + RF_PixelToUnit(yoffs[number]);
		ob->gfxChunk = COLLECTIBLES_LUMP_START + number*2;
		ob->user2 = ob->gfxChunk;
		ob->user3 = ob->gfxChunk + 1;
		CK_SetAction(ob, &CK_act_item);

		CA_MarkGrChunk(ob->gfxChunk);
	}
	else	// not in BWB level
	{
		// don't spawn the collectible if the player already collected it:
		if ((ck_gameState.collectiblesTotal & bit))
			return;

		// check if there already is an identical collectible in the level
		for (ob=ck_keenObj; ob; ob=ob->next)
		{
			if (ob->type == CT_Collectible && ob->user1 == bit)
			{
				// randomly decide to keep the old one or remove it and spawn a new one
				if (US_RndT() < 0x80)
				{
					return;	//keep the old one
				}
				CK_RemoveObj(ob);	// remove the old one and spawn a new one
				break;
			}
		}

		ob = CK_GetNewObj(false);
		ob->type = CT_Collectible;
		ob->posX = RF_TileToUnit(tileX);
		ob->posY = RF_TileToUnit(tileY);
		if (number == 0)	// if it's the Yorp
		{
			ob->posX += 0*PIXGLOBAL;
			ob->posY += -10*PIXGLOBAL;
		}
		ob->yDirection = -1;
		ob->gfxChunk = COLLECTIBLES_LUMP_START + number*2;
		ob->user2 = ob->gfxChunk;
		ob->user3 = ob->gfxChunk + 2;
		ob->user1 = bit;
		CK_SetAction(ob, &CK_act_item);

		CA_MarkGrChunk(ob->gfxChunk);
		CA_MarkGrChunk(ob->gfxChunk+1);
	}
}

/*
==============================================================================
	ROOTS

	Intended to be placed at the base of a pole. Kill Keen on touch and need
	to be shot 2 times to remove them, each shot changing the frame.
	Regenerates after 4 seconds if not fully destroyed.

	user1 - regeneration timer
==============================================================================
*/

CK_action CK_ACT_Roots;

void CK_SpawnRoots(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->clipped = CLIP_not;
	ob->type = CT_Roots;
	ob->posX = RF_TileToUnit(tileX) - 4*PIXGLOBAL;
	ob->posY = RF_TileToUnit(tileY) - 21*PIXGLOBAL;
	ob->gfxChunk = ROOTS1SPR;
	CK_SetAction(ob, &CK_ACT_Roots);
}

void RootsThink(CK_object *ob)
{
	if (ob->gfxChunk > ROOTS2SPR)
	{
		if ((ob->user1 -= tics) < 0)
		{
			if (ob->gfxChunk++ >= ROOTS5SPR)
			{
				CK_RemoveObj(ob);
				return;
			}
			ob->user1 += 10;	// delay for vanishing animation
		}
	}
	else if (ob->gfxChunk > ROOTS1SPR)
	{
		if ((ob->user1 -= tics) < 0)
		{
			ob->gfxChunk--;
			ob->user1 += 10;	// delay for growing animation
		}
	}
}

void RootsContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		CK_KillKeen();
		break;
	case CT_Stunner:
		if (ob->gfxChunk > ROOTS2SPR)
			break;
		CK_ShotHit(hit);
		if (ob->gfxChunk == ROOTS2SPR)
		{
			ob->user1 = 10;	// delay for vanishing animation
			ob->active = OBJ_EXISTS_ONLY_ONSCREEN;
			SD_PlaySound(ROOTSSND);
		}
		else
		{
			ob->user1 = 4*TickBase;
		}
		ob->gfxChunk++;
		CK_SetAction2(ob, ob->currentAction);	// force game to update the sprite
		break;
	}
}

CK_action CK_ACT_Roots = {0, 0, AT_Frame, false, push_none, 0, 0, 0, RootsThink, RootsContact, CK_BasicDrawFunc1, NULL};

/*
==============================================================================
	YETI

	Stands at tunnel entrances and blows a cloud at Keen when Keen comes near.
	The cloud pushes Keen back. Shooting the Yeti 5 times causes it to explode
	into several small yetis that roll and jump around, pushing Keen on touch.

	user1 - health
	user2 - cooldown/delay between clouds
	user3 - animation counter
	user4 - cooldown for hurt animation
------------------------------------------------------------------------------
	YETI CLOUD

	user1 - animation counter
------------------------------------------------------------------------------
	SMALL YETI

	user1 - cooldown (can't merge into big yeti when this is non-zero)
	user2 - touch counter
	user4 - big yeti's posX
==============================================================================
*/

#define TINY_YETI_COUNT 4	// remember: count is 1 smaller than the total because the object never touches itself
// Also remember that this code needs TINY_YETI_COUNT-1 growing sprites!

CK_action CK_ACT_YetiStand;
CK_action CK_ACT_YetiCloud;
CK_action CK_ACT_TinyYetiRoll;
CK_action CK_ACT_TinyYetiJump;

void CK_SpawnYeti(int tileX, int tileY)
{
	CK_object *ob;

	ob = CK_GetNewObj(false);
	ob->type = CT_Yeti;
//	ob->active = OBJ_ACTIVE;	//redundant - GetNewObj always sets active to OBJ_ACTIVE
	ob->zLayer = 1;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY) - 27*PIXGLOBAL;
	ob->xDirection = motion_Left;
	ob->yDirection = motion_Down;
//	ob->clipped = CLIP_normal;	//redundant - GetNewObj always sets clipped to CLIP_normal
	ob->gfxChunk = YETIL1SPR;
	CK_SetAction(ob, &CK_ACT_YetiStand);
}

void near PushKeenX(CK_object *hit, CK_object *ob)
{
	// push Keen, but only if Keen is in regular collision mode:
	if (hit->currentAction->collide == &CK_KeenColFunc)
	{
		// remember topTI, so Keen can still land while being pushed
		int oldTop = hit->topTI;
		int oldBottom = hit->bottomTI;
		int oldY = hit->posY;
		int oldStick = hit->currentAction->stickToGround;
		if (ck_keenState.platform)
			hit->currentAction->stickToGround = false;	// don't push Keen down when he's on a platform object
		ck_mapState.keenGetsPushed = true;
		CK_PhysPushX(hit, ob);
		ck_mapState.keenGetsPushed = false;
		hit->currentAction->stickToGround = oldStick;	// restore old value
		if (hit->leftTI || hit->rightTI)
			hit->user1 = hit->user2 = 0;	// disable Keen's idle animations while being pushed
		if (hit->posY == oldY)
		{
			if (!hit->topTI)
				hit->topTI = oldTop;
			if (!hit->bottomTI)
				hit->bottomTI = oldBottom;
		}
	}
}

void YetiThink(CK_object *ob)
{
	if (ob->clipRects.unitXmid > ck_keenObj->clipRects.unitXmid)
	{
		ob->xDirection = motion_Left;
	}
	else
	{
		ob->xDirection = motion_Right;
	}

	if (ob->user2 > tics)
	{
		ob->user2 -= tics;
	}
	else if (ob->user2)
	{
		ob->user2 = 0;
	}
	else if (ABS(ob->clipRects.unitXmid - ck_keenObj->clipRects.unitXmid) <= 4*TILEGLOBAL
		&& ABS(ob->posY - ck_keenObj->posY) <= 2*TILEGLOBAL)
	{
		CK_object *cloud = CK_GetNewObj(true);
		cloud->zLayer = 2;
		cloud->posX = ob->posX + 9*PIXGLOBAL;
		cloud->posY = ob->posY + 10*PIXGLOBAL;
		cloud->type = CT_YetiCloud;
		cloud->gfxChunk = YETICLOUD1SPR;
		cloud->xDirection = ob->xDirection;
		cloud->velX = 29*ob->xDirection;
		CK_SetAction(cloud, &CK_ACT_YetiCloud);
		SD_PlaySound(YETICLOUDSND);

		ob->user2 = 35;	// don't spawn another cloud in the next 35 tics
	}

	if ((ob->user4 -= tics) < 0)
	{
		ob->user4 = 0;
		ck_nextX = (ob->posX & PIXGLOBAL);
	}
	else
	{
		ck_nextX = (ob->posX & PIXGLOBAL)? PIXGLOBAL : -PIXGLOBAL;	// shake it, yeti!
		ob->user3 += tics << 1;	// make animation go at triple speed
	}

	// update frame:
	ob->user3 += tics;
	if (ob->user3 >= 40)
	{
		ob->user3 -= 40;
	}
	if (ob->user3 < 25)
	{
		ob->gfxChunk = YETIL1SPR+1 + ob->xDirection;
	}
	else
	{
		ob->gfxChunk = YETIL2SPR+1 + ob->xDirection;
	}
}

void YetiContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		PushKeenX(hit, ob);
		break;
	case CT_Stunner:
		CK_ShotHit(hit);
		ob->user4 = 35;	// half a second of shaking
		if (++ob->user1 >= 5)
		{
			register int i;

			// turn big yeti into tiny yeti:
			ob->user1 = 10*TickBase;
			ob->user2 = ob->user3 = 0;
			ob->user4 = ob->posX + TILEGLOBAL;
			ob->type = CT_TinyYetiMaster;
			ob->gfxChunk = TINYYETIROLL1SPR;
			CK_SetAction2(ob, &CK_ACT_TinyYetiRoll);

			// spawn some more tiny yetis:
			for (i=0; i < TINY_YETI_COUNT; i++)
			{
				hit = CK_GetNewObj(true);

				// Note: if CK_GetNewObj() returns the dummy object (because there
				// were no free spots in the objarray), the tiny yeti object will
				// not exists in the level and the tiny yetis won't be able to merge
				// into a big one again.

				hit->type = CT_TinyYeti;
				hit->zLayer = 1;
				hit->gfxChunk = TINYYETIROLL1SPR;
				hit->posX = ob->posX + (i & 1)*TILEGLOBAL;
				hit->posY = ob->posY + (i & 2)*TILEGLOBAL/2;
				hit->xDirection = (US_RndT() < 0x80)? motion_Left : motion_Right;
				hit->user1 = 12*TickBase;
				hit->user4 = ob->posX + TILEGLOBAL;
				CK_SetAction(hit, &CK_ACT_TinyYetiRoll);
			}
		}
		break;
	}
}

void YetiCloudThink(CK_object *ob)
{
	if ((ob->user1 += tics) >= 8)
	{
		ob->user1 -= 8;
		if (++ob->gfxChunk > YETICLOUD7SPR)
		{
			CK_RemoveObj(ob);
			return;
		}
	}
	ck_nextX = ob->velX * tics;
}

void YetiCloudContact(CK_object *ob, CK_object *hit)
{
	if (hit->type == CT_Player && ob->type == CT_YetiCloud)
	{
		PushKeenX(hit, ob);
	}
}

void TinyYetiRollThink(CK_object *ob)
{
	int xmove = tics * 28;
	int xdist = ob->posX - ob->user4;

	if ((ob->user1 -= tics) < 0)
		ob->user1 = 0;

	if (ob->user1 && !ob->user2)
	{
		// don't move too far away from the big yeti's position:
		if (xdist > 2*TILEGLOBAL)
		{
			ob->xDirection = motion_Left;
		}
		else if (xdist < -2*TILEGLOBAL)
		{
			ob->xDirection = motion_Right;
		}

		// jump at random:
		if (US_RndT() < 10)
		{
			if (ob->xDirection == motion_Left)
			{
				ob->velX = -28;
				ob->gfxChunk = TINYYETILSPR;
			}
			else
			{
				ob->velX = 28;
				ob->gfxChunk = TINYYETIRSPR;
			}
			ob->velY = -48;
			CK_SetAction2(ob, &CK_ACT_TinyYetiJump);
			return;
		}
	}
	else
	{
		// return to big yeti's position:
		if (ob->posX > ob->user4)
		{
			ob->xDirection = motion_Left;
		}
		else
		{
			ob->xDirection = motion_Right;
		}

		// don't let any tiny yeti move past the target position:
		if (xmove > abs(xdist))
		{
			xmove = abs(xdist);
		}

		// stop moving when close to desired position:
		if (ABS(xdist) < 2*PIXGLOBAL)	// distance must be smaller than half the hitbox width of the tiny yeti!
		{
			if (ob->user2)
			{
				// tiny yeti has already grown a bit (merged with others):
				ob->gfxChunk = ob->user2+YETIGROW1SPR-1;
			}
			else
			{
				// tiny yeti is just standing:
				ob->gfxChunk = TINYYETIROLL1SPR;

				// allow the (non-growing) yeti to jump a little while waiting to get merged:
				if (!ob->user2)
				{
					ob->velX = 0;
					ob->velY = -24;
					CK_SetAction2(ob, &CK_ACT_TinyYetiJump);
				}
			}
			// master can only absorb the others when it is at
			// the target position, so move it there
			if (ob->type == CT_TinyYetiMaster)
				ck_nextX = ob->user4-ob->posX;
			return;
		}
	}

	ck_nextX = ob->xDirection * xmove;

	// rolling animation:
	if ((ob->actionTimer += tics) >= 4)
	{
		ob->actionTimer -= 4;
		ob->gfxChunk += ob->xDirection;
	}
	// Note: actionTimer is ignored for actions using AT_Frame, so it's safe to use it here.

	if (ob->gfxChunk < TINYYETIROLL1SPR)
	{
		ob->gfxChunk = TINYYETIROLL4SPR;
	}
	else if (ob->gfxChunk > TINYYETIROLL4SPR)
	{
		ob->gfxChunk = TINYYETIROLL1SPR;
	}
}

void TinyYetiJumpThink(CK_object *ob)
{
	int xdist = ob->posX - ob->user4;

	if ((ob->user1 -= tics) < 0)
		ob->user1 = 0;

	// don't move too far away from the big yeti's position:
	if ((ob->xDirection == motion_Right && xdist > + 2*TILEGLOBAL)
		|| (ob->xDirection == motion_Left && xdist < - 2*TILEGLOBAL))
	{
		CK_PhysDampHorz(ob);
	}
	else
	{
		ck_nextX = ob->velX * tics;
	}
	CK_PhysGravityHigh(ob);
}

void TinyYetiContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		PushKeenX(hit, ob);
		break;
	case CT_TinyYeti:
		// only tiny yeti master can grow, and only when touching tiny yetis that have the same target position!
		if (ob->user1 || ob->type != CT_TinyYetiMaster || hit->user4 != ob->user4 || ob->posX != ob->user4)
			break;

		// absorb the other yeti:
#if 1
		CK_RemoveObj(hit);
#else
		// turn hit object into smoke cloud instead of just removing it
		hit->velX = 0;
		hit->gfxChunk = YETICLOUD1SPR;
		hit->type = CT_YetiCloud;
		hit->user1 = 0;
		hit->clipped = CLIP_not;
		hit->zLayer = FOREGROUNDPRIORITY;
		hit->posX -= 10*PIXGLOBAL;
		hit->posY -= 10*PIXGLOBAL;
		CK_SetAction2(hit, &CK_ACT_YetiCloud);
#endif

		if (++ob->user2 == TINY_YETI_COUNT)
		{
			// turn ob into big yeti:
			ob->type = CT_Yeti;
			ob->gfxChunk = YETIL1SPR;
			ob->posY -= 29*PIXGLOBAL;
			ob->posX = ob->user4 - TILEGLOBAL;
			ob->user1 = ob->user2 = ob->user3 = ob->user4 = ob->actionTimer = 0;
			CK_SetAction(ob, &CK_ACT_YetiStand);
		}
		else
		{
			if (ob->velY < 0)
				ob->velY = 0;
			ob->user1 = 7;	// force a pause of 0.1 seconds after merging
			// assign correct sprite to tiny yeti master:
			ob->gfxChunk = ob->user2 + YETIGROW1SPR-1;
			CK_SetAction(ob, ob->currentAction);	// update hitbox (DON'T use CK_SetAction2 here - that's not safe after manipulating posX)
		}
		break;
	}
}

void TinyYetiJumpReact(CK_object *ob)
{
	if (ob->leftTI || ob->rightTI)
	{
		ob->velX = -ob->velX/2;
		ob->xDirection = -ob->xDirection;
	}
	if (ob->bottomTI || ob->topTI)
	{
		ob->velY = 0;
		if (ob->topTI)
		{
			CK_SetAction2(ob, &CK_ACT_TinyYetiRoll);
		}
	}
	CK_BasicDrawFunc1(ob);
}

void TinyYetiRollReact(CK_object *ob)
{
	if (!ob->topTI)
	{
		if (US_RndT() < 0x80)
		{
			if (ob->xDirection == motion_Left)
			{
				ob->velX = -28;
				ob->gfxChunk = TINYYETILSPR;
			}
			else
			{
				ob->velX = 28;
				ob->gfxChunk = TINYYETIRSPR;
			}
			ob->velY = -48;
			CK_SetAction2(ob, &CK_ACT_TinyYetiJump);
		}
		else
		{
			ob->velX = -ob->velX;
			ob->xDirection = -ob->xDirection;
			ob->posX -= ob->deltaPosX;
		}
	}
	else if (ob->leftTI || ob->rightTI)
	{
		ob->velX = -ob->velX;
		ob->xDirection = -ob->xDirection;
	}
	CK_BasicDrawFunc1(ob);
}

CK_action CK_ACT_YetiStand = {0, 0, AT_Frame, false, push_none, 0, 0, 0, YetiThink, YetiContact, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_YetiCloud = {0, 0, AT_Frame, false, push_none, 0, 0, 0, YetiCloudThink, YetiCloudContact, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_TinyYetiRoll = {0, 0, AT_Frame, false, push_down, 0, 0, 0, TinyYetiRollThink, TinyYetiContact, TinyYetiRollReact, NULL};
CK_action CK_ACT_TinyYetiJump = {0, 0, AT_Frame, false, push_none, 0, 0, 0, TinyYetiJumpThink, TinyYetiContact, TinyYetiJumpReact, NULL};

#undef TINY_YETI_COUNT

/*
==============================================================================
	CLOUD

	A more or less static platform that Keen can stand on. Keen slowly sinks
	into it until he eventually falls out at the bottom end.

	user1 - initial y position
	user2 - initial hitbox height
==============================================================================
*/

CK_action CK_ACT_Cloud;

void CK_SpawnCloud(int tileX, int tileY)
{
	CK_object *ob;

	ob = CK_GetNewObj(false);
	ob->type = CT_Cloud;
	ob->active = OBJ_ALWAYS_ACTIVE;
	ob->zLayer = 2;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY /*= ob->user1*/ = RF_TileToUnit(tileY);
	ob->clipped = CLIP_not;
	ob->gfxChunk = BIGCLOUDSPR;
	CK_SetAction(ob, &CK_ACT_Cloud);

	ob->user2 = ob->clipRects.unitY2-ob->clipRects.unitY1;
}

void CK_SpawnInvisibleCloud(unsigned unitX, unsigned unitY, unsigned w, unsigned h)
{
	CK_object *ob;

	ob = CK_GetNewObj(false);
	ob->type = CT_Cloud;
	ob->active = OBJ_ALWAYS_ACTIVE;
	ob->posX=ob->clipRects.unitX1 = unitX;
	ob->posY=ob->clipRects.unitY1 /*= ob->user1*/ = unitY;
	ob->clipped = CLIP_not;
	ob->currentAction = &CK_ACT_Cloud;
	ob->clipRects.unitX2 = unitX + w;
	ob->clipRects.unitY2 = unitY + h;
	ob->user2 = h;
	CK_CalcTileClipRects(ob);
}

void CloudThink(CK_object *ob)
{
#if 0
	//
	// version 1: make the cloud sink when Keen stands on it
	//
	if (ob == ck_keenState.platform)
	{
		// Keen is standing on the cloud, so make it move down:
		ck_nextY = tics;
		if (ob->posY - ob->user1 >= 3*TickBase)
		{
			// make Keen fall off
			ck_keenState.platform = NULL;
			ob->type = CT_Friendly;
		}
	}
	else
	{
		// Keen is NOT standing on the cloud, so let it rise back up:
		ck_nextY = -tics*2;
		if (ck_nextY+ob->posY < ob->user1)
			ck_nextY = ob->user1 - ob->posY;
		ob->type = CT_Cloud;
	}
#else
	//
	// version 2: make Keen sink into the cloud
	//
	if (ob == ck_keenState.platform)
	{
		// Keen is standing on the cloud, so make Keen sink into it by hacking the cloud's hitbox:
		ob->clipRects.unitY1 += ob->gfxChunk? tics*2 : tics;
		if (ob->clipRects.unitY1 >= ob->clipRects.unitY2)
		{
			// make Keen fall off
			ck_keenState.platform = NULL;
			ob->type = CT_Friendly;
		}
	}
	else
	{
		// Keen is NOT standing on the cloud, so restore the hitbox:
		ob->clipRects.unitY1 = ob->clipRects.unitY2 - ob->user2;
		ob->type = CT_Cloud;
	}
#endif
}

CK_action CK_ACT_Cloud = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CloudThink, NULL, CK_BasicDrawFunc1, NULL};

/*
==============================================================================
	MOLET

	Walks back and forth on normal ground until it hits a wall that has the
	left or right side set to 2 instead of 1. This causes it to change into
	noclip mode, digging through the wall. Touching a block icon causes it
	to change back into normal movement mode.

	user1 - animation counter
	user2 - animation frame

	WARNING: Some older versions of Abiathar don't seem to like tiles that have
	the left/right side set to 2 and change them to 1 instead when saving the
	levels. Be sure to upgrade to the most recent version before editing any
	FITF levels.
==============================================================================
*/

CK_action CK_ACT_MoletDirt;

void CK_SpawnMoletDirt(unsigned unitX, unsigned unitY)
{
	CK_object *ob;

	ob = CK_GetNewObj(true);
	ob->type = CT_Friendly;
	ob->active = OBJ_EXISTS_ONLY_ONSCREEN;
	ob->clipped = CLIP_not;
	ob->zLayer = 1;	// must use a greater value than Molet!
	ob->posX = unitX;
	ob->posY = unitY;
	ob->gfxChunk = MOLETDIRT1SPR;
	ob->user1 = 50;
	CK_SetAction(ob, &CK_ACT_MoletDirt);
}

void MoletDirtThink(CK_object *ob)
{
	if (++ob->gfxChunk > MOLETDIRT6SPR)
		ob->gfxChunk = MOLETDIRT1SPR;

	if (--ob->user1 <= 0)
		CK_RemoveObj(ob);
}

CK_action CK_ACT_MoletDirt = {0, 0, AT_UnscaledOnce, false, push_none, 4, 0, 0, MoletDirtThink, NULL, CK_BasicDrawFunc1, &CK_ACT_MoletDirt};

CK_action CK_ACT_MoletWalk;
CK_action CK_ACT_MoletDig;
CK_action CK_ACT_MoletFall;

void CK_SpawnMolet(int tileX, int tileY)
{
	CK_object *ob;

	ob = CK_GetNewObj(false);
	ob->type = CT_Friendly;
	ob->active = OBJ_ALWAYS_ACTIVE;
	ob->zLayer = 0;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY) - 4*PIXGLOBAL;
	ob->xDirection = US_RndT() < 0x80? motion_Left : motion_Right;
	if (ob->xDirection == motion_Left)
		ob->gfxChunk = MOLETWALKL1SPR;
	else
		ob->gfxChunk = MOLETWALKR1SPR;
	CK_SetAction(ob, &CK_ACT_MoletWalk);
}

void MoletAnimate(CK_object *ob)
{
	// a 4-frame animation that changes the frame every 6 tics:
	if ((ob->user1 += tics) >= 6)
	{
		ob->user1 -= 6;
		if (++ob->user2 == 4)
			ob->user2 = 0;
	}
	if (ob->xDirection == motion_Left)
	{
		ob->gfxChunk = MOLETWALKL1SPR + ob->user2;
	}
	else
	{
		ob->gfxChunk = MOLETWALKR1SPR + ob->user2;
	}
#ifdef EXTRA_INTERACTIONS
	CK_SmirkyCheckTiles(ob);
#endif
}

void MoletDig(CK_object *ob)
{
	int oldchunk = ob->gfxChunk;
	MoletAnimate(ob);
	if (ob->gfxChunk != oldchunk)
	{
		// TODO? check for direction arrows in the info plane?
		if (ob->xDirection == motion_Left)
		{
			if (CA_TileAtPos(ob->clipRects.tileX1, ob->clipRects.tileY2, 2) == PLATFORMBLOCK)
			{
				ob->currentAction = &CK_ACT_MoletWalk;
				ob->clipped = CLIP_normal;
			}
			oldchunk = ob->clipRects.unitX1-3*PIXGLOBAL;
			if ((oldchunk & 0xFF) < 8)
			{
				oldchunk = RF_UnitToTile(oldchunk)-1;
				if (!TI_ForeRight(CA_TileAtPos(oldchunk, ob->clipRects.tileY2, 1))
					&& TI_ForeRight(CA_TileAtPos(oldchunk+1, ob->clipRects.tileY2, 1)))
				{
					// Molet is coming out of the wall, so add dirt
					CK_SpawnMoletDirt(RF_TileToUnit(oldchunk)-2*PIXGLOBAL, ob->posY-8*PIXGLOBAL);
				}
			}
			ck_nextX = -8;
		}
		else
		{
			if (CA_TileAtPos(ob->clipRects.tileX2, ob->clipRects.tileY2, 2) == PLATFORMBLOCK)
			{
				ob->currentAction = &CK_ACT_MoletWalk;
				ob->clipped = CLIP_normal;
			}
			oldchunk = ob->clipRects.unitX2+3*PIXGLOBAL;
			if ((oldchunk & 0xFF) > TILEGLOBAL-8)
			{
				oldchunk = RF_UnitToTile(oldchunk)+1;
				if (!TI_ForeLeft(CA_TileAtPos(oldchunk, ob->clipRects.tileY2, 1))
					&& TI_ForeLeft(CA_TileAtPos(oldchunk-1, ob->clipRects.tileY2, 1)))
				{
					// Molet is coming out of the wall, so add dirt
					CK_SpawnMoletDirt(RF_TileToUnit(oldchunk)-16*PIXGLOBAL, ob->posY-8*PIXGLOBAL);
				}
			}
			ck_nextX = 8;
		}
	}
}

void MoletFall(CK_object *ob)
{
	CK_Fall(ob);
	CK_SmirkyCheckTiles(ob);
}

void MoletContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		if (ob->xDirection < 0)
		{
			// molet is facing left
			// only the tip of the drill (leftmost side of the molet's hitbox) is deadly
			if (hit->clipRects.unitX1 <= ob->clipRects.unitX1 && hit->clipRects.unitX2 >= ob->clipRects.unitX1)
				CK_KillKeen();
		}
		else
		{
			// molet is facing right
			// only the tip of the drill (rightmost side of the molet's hitbox) is deadly
			if (hit->clipRects.unitX1 <= ob->clipRects.unitX2 && hit->clipRects.unitX2 >= ob->clipRects.unitX2)
				CK_KillKeen();
		}
		break;
	case CT_Stunner:
		CK_ShotHit(hit);
		break;
		
#ifdef EXTRA_INTERACTIONS
	case CT_Item:
		if (hit->user1 >= 4)	// don't steal key gems!
		{
			hit->type = CT_Friendly;
			hit->zLayer = FOREGROUNDPRIORITY;
			hit->yDirection = motion_None;
			CK_SetAction2(hit, &CK_ACT_StolenItem0);
			SD_PlaySound(SMIRKYSTEALSND);
		}
		break;
#endif
	}
}

void MoletWalkReact(CK_object *ob)
{
	if (ob->leftTI && ob->xDirection == motion_Right)
	{
		if (ob->leftTI == 2)
		{
			ob->clipped = CLIP_not;
			CK_SetAction2(ob, &CK_ACT_MoletDig);
			CK_SpawnMoletDirt(RF_TileToUnit(ob->clipRects.tileX2)+0*PIXGLOBAL, ob->posY-8*PIXGLOBAL);
		}
		else
			goto turn;
	}
	else if (ob->rightTI && ob->xDirection == motion_Left)
	{
		if (ob->rightTI == 2)
		{
			ob->clipped = CLIP_not;
			CK_SetAction2(ob, &CK_ACT_MoletDig);
			CK_SpawnMoletDirt(RF_TileToUnit(ob->clipRects.tileX1)-16*PIXGLOBAL, ob->posY-8*PIXGLOBAL);
		}
		else
		{
turn:
			ob->xDirection = -ob->xDirection;
			ob->posX -= ob->deltaPosX;
			ob->gfxChunk -= 4*ob->xDirection;
		}
	}
	else if (!ob->topTI)
	{
		CK_SetAction2(ob, &CK_ACT_MoletFall);
	}
	CK_BasicDrawFunc1(ob);
}

void MoletFallReact(CK_object *ob)
{
	if (ob->topTI)
	{
		ob->velY = 0;
		CK_SetAction2(ob, &CK_ACT_MoletWalk);
	}
	CK_BasicDrawFunc1(ob);
}

CK_action CK_ACT_MoletWalk = {0, 0, AT_UnscaledFrame, false, push_down, 6, 24, 0, MoletAnimate, MoletContact, MoletWalkReact, &CK_ACT_MoletWalk};
CK_action CK_ACT_MoletDig = {0, 0, AT_Frame, false, push_none, 0, 0, 0, MoletDig, MoletContact, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_MoletFall = {0, 0, AT_Frame, false, push_none, 0, 0, 0, MoletFall, MoletContact, MoletFallReact, NULL};

/*
==============================================================================
	BIRD

	Flies around above Keen's head. Starts a dive attack when Keen gets too
	close and/or drops an egg when shot. Also has a chance to drop an egg at
	random.

	user1 - counter for the shaking animation when laying an egg
	      - movement duration when flying normally
	      - countdown before attack sequence
	user2 - defines if the bird can reproduce (0 = sterile, 1 = fertile)
	user3 - hits taken so far
------------------------------------------------------------------------------
	EGG

	Falls to the ground and either lands safely or cracks open and spills its
	contents. If it landed safely, it will break when touched by Keen or hit by
	a shot. If the egg breaks more than 4 seconds after it was spawned, it
	releases a new bird.

	user1 - counts for how long the egg has existed
	user4 - Nisasi pointer
==============================================================================
*/

#define BIRD_SPEED 10
#define BIRD_SHAKETIME (TickBase/2)

CK_action CK_ACT_BirdFly1;
CK_action CK_ACT_BirdFly2;
CK_action CK_ACT_BirdFly3;
CK_action CK_ACT_BirdFly4;
CK_action CK_ACT_BirdFly5;
CK_action CK_ACT_BirdAttack;
CK_action CK_ACT_BirdLayEgg;
CK_action CK_ACT_BirdFall;
CK_action CK_ACT_BirdStunned;

CK_action CK_ACT_BirdEgg;
CK_action CK_ACT_EggPiece;

void CK_SpawnBird(int tileX, int tileY)
{
	CK_object *ob;

	ob = CK_GetNewObj(false);
	ob->type = CT_Bird;
	ob->zLayer = 1;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	ob->user2 = true;	// bird is fertile
	CK_SetAction(ob, &CK_ACT_BirdFly2);
}

int near CountBirds(void)
{
	int count = 0;
	CK_object *ob;

	for (ob = ck_keenObj->next; ob; ob = ob->next)
	{
		if (ob->type == CT_Bird || ob->type == CT_BirdEgg
			|| (ob->type == CT_StunnedCreature && ob->user4 == CT_Bird))
		{
			count++;
		}
	}
	return count;
}

int near CountEggs(void)
{
	int count = 0;
	CK_object *ob;

	for (ob = ck_keenObj->next; ob; ob = ob->next)
	{
		if (ob->type == CT_Egg || ob->type == CT_BirdEgg)
		{
			count++;
		}
	}
	return count;
}

void BirdFlyThink(CK_object *ob)
{
	int ydist = ob->clipRects.unitY2 + 55*PIXGLOBAL - ck_keenObj->clipRects.unitY2;
	int xdist = ck_keenObj->clipRects.unitXmid - ob->clipRects.unitXmid;

	// If we have a movement speed, the think function was already executed
	// for this frame and the animation (state change) caused the game to
	// execute it again. In that case, don't do anything.
	if (ck_nextX || ck_nextY)
		return;

	// move in a random direction while Keen is not a player type (dying or grabbed by Efree)
	if (ck_keenObj->type != CT_Player)
	{
		if ((ob->user1 -= tics) < 0)
		{
			ob->user1 = TickBase/2;
			ob->xDirection = (US_RndT() % 3)-1;
			ob->yDirection = (US_RndT() % 3)-1;
		}
		CK_PhysAccelVert(ob, ob->yDirection, BIRD_SPEED);
		CK_PhysAccelHorz(ob, ob->xDirection, BIRD_SPEED);
		return;
	}

	if (ydist > 0)
	{
		CK_PhysAccelVert(ob, motion_Up, BIRD_SPEED);
	}
	else
	{
		CK_PhysAccelVert(ob, motion_Down, BIRD_SPEED);
	}

	if (xdist > 6*TILEGLOBAL)
	{
		ob->xDirection = motion_Right;
		CK_PhysAccelHorz(ob, motion_Right, BIRD_SPEED);
		return;
	}
	else if (xdist < -6*TILEGLOBAL)
	{
		ob->xDirection = motion_Left;
		CK_PhysAccelHorz(ob, motion_Left, BIRD_SPEED);
		return;
	}
	else
	{
		if ((ob->user1 -= tics) < 0)
			ob->user1 = 0;

		if (xdist > 5*TILEGLOBAL || xdist < -5*TILEGLOBAL)
		{
			// Bird is more than 5 tiles, but less than 6 tiles away from Keen.
			// This would be a good place to (randomly) start the dive attack.
			if (ydist < PIXGLOBAL && ydist > -PIXGLOBAL && US_RndT() < 128
				&& ck_keenObj->type == CT_Player)
			{
				ob->currentAction = &CK_ACT_BirdAttack;
				return;
			}
		}

		if (ob->user1)
		{
			CK_PhysAccelHorz(ob, ob->xDirection, BIRD_SPEED);
		}
		else
		{
			CK_PhysDampHorz(ob);

			// start laying an egg when Keen is directly below the bird and looking up:
			if (ck_keenObj->gfxChunk == KEENLOOKUPSPR
				&& ob->velY == 0
				&& ck_keenObj->clipRects.unitY1 > ob->clipRects.unitY2
				&& ABS(ob->clipRects.unitXmid - ck_keenObj->clipRects.unitXmid) < TILEGLOBAL)
			{
				SD_PlaySound(BIRDLAYSEGGSND);
				ob->currentAction = &CK_ACT_BirdLayEgg;
			}
			else if (US_RndT() < 0x10)
			{
				ob->user1 = 2*TickBase;
			}
		}
	}
}

void BirdAttackThink(CK_object *ob)
{
	if (ob->gfxChunk < BIRDATTACKR1SPR || ob->gfxChunk > BIRDATTACKL3SPR)
	{
		if (ob->posX < ck_keenObj->posX)
		{
			ob->xDirection = motion_Right;
			ob->gfxChunk = BIRDATTACKR1SPR;
		}
		else
		{
			ob->xDirection = motion_Left;
			ob->gfxChunk = BIRDATTACKL1SPR;
		}
		ob->velX = 0;
		ob->velY = 0;
		ob->user1 = 0;
		SD_PlaySound(BIRDSND);
	}
	else if (ob->user1 < 2)
	{
		ob->user1 += tics;
	}
	else if (ob->xDirection < 0)
	{
		if (ob->clipRects.unitX2 < ck_keenObj->clipRects.unitX1)
		{
			// bird has moved past Keen, so make it move up again:
			CK_PhysDampHorz2(ob);
			if (ob->clipRects.unitY2 + 2*TILEGLOBAL > ck_keenObj->clipRects.unitY1)
			{
				CK_PhysAccelVert(ob, -2, BIRD_SPEED);
			}
			else
			{
				CK_PhysDampVert(ob);
			}
			if (ob->velX > -BIRD_SPEED)
			{
				ob->currentAction = &CK_ACT_BirdFly4;
			}
			return;
		}
		// bird is still moving towards Keen here
		CK_PhysAccelHorz(ob, -4, 48);
		if (ob->velX > -48)
		{
			// bird is still starting the attack (hasn't reached max velX yet)
			CK_PhysAccelVert(ob, 2, 48);
			if (ob->gfxChunk == BIRDATTACKL1SPR)
			{
				if ((ob->actionTimer += tics) > 10)
					ob->gfxChunk++;
			}
		}
		else
		{
			CK_PhysAccelVert(ob, -2, 0);
			ob->gfxChunk = BIRDATTACKL3SPR;
		}
	}
	else
	{
		if (ob->clipRects.unitX1 > ck_keenObj->clipRects.unitX2)
		{
			// bird has moved past Keen, so make it move up again:
			CK_PhysDampHorz2(ob);
			if (ob->clipRects.unitY2 + 2*TILEGLOBAL > ck_keenObj->clipRects.unitY1)
			{
				CK_PhysAccelVert(ob, -2, BIRD_SPEED);
			}
			else
			{
				CK_PhysDampVert(ob);
			}
			if (ob->velX < BIRD_SPEED)
			{
				ob->currentAction = &CK_ACT_BirdFly4;
			}
			return;
		}
		// bird is still moving towards Keen here
		CK_PhysAccelHorz(ob, 4, 48);
		if (ob->velX < 48)
		{
			// bird is still starting the attack (hasn't reached max velX yet)
			CK_PhysAccelVert(ob, 2, 48);
			if (ob->gfxChunk == BIRDATTACKR1SPR)
			{
				if ((ob->actionTimer += tics) > 10)
					ob->gfxChunk++;
			}
		}
		else
		{
			// bird has reached full velX, now reduce velY
			CK_PhysAccelVert(ob, -2, 0);
			ob->gfxChunk = BIRDATTACKR3SPR;
		}
	}

	if (ck_nextY < 0)
		ob->yDirection = motion_Up;
	else
		ob->yDirection = motion_Down;
}

void BirdLayEggThink(CK_object *ob)
{
	// shake before laying the egg
	if ((ob->actionTimer += tics) < BIRD_SHAKETIME)
	{
		if (++ob->user1 & 1)
		{
			ck_nextX = PIXGLOBAL;
		}
		else
		{
			ck_nextX = -PIXGLOBAL;
		}
	}
	else
	{
		if (CountEggs() < 5)	// each egg needs 4 sprites (egg + 3 small pieces when breaking), so 5 eggs = 20 sprites!
		{
			CK_object *egg;

			// spawn egg:
			egg = CK_GetNewObj(true);
			egg->posX = ob->posX;
			egg->posY = ob->posY + TILEGLOBAL;
			egg->gfxChunk = EGG3SPR;

			// check if bird is fertile and can lay a fertilized egg:
			if (ob->user2 && US_RndT() < 85 && CK_ObjectVisible(ob) && CountBirds() < 7)
			{
				egg->type = CT_BirdEgg;
				ob->user2 = false;
			}
			else
			{
				egg->type = CT_Egg;
				egg->active = OBJ_EXISTS_ONLY_ONSCREEN;
			}
			CK_SetAction(egg, &CK_ACT_BirdEgg);
		}

		CK_SetAction2(ob, &CK_ACT_BirdFly1);
		ob->velY = -BIRD_SPEED;	// bird flies up
	}
}

void BirdContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		CK_KillKeen();
		break;
	case CT_Stunner:
		if (++ob->user3 == 2)
		{
			CK_StunCreature(ob, hit, &CK_ACT_BirdFall);
			ob->zLayer = 0;
		}
		else
		{
			CK_ShotHit(hit);
			if (ob->currentAction != &CK_ACT_BirdLayEgg)	// don't restart the shaking animation if it's playing already
			{
				SD_PlaySound(BIRDLAYSEGGSND);
				CK_SetAction2(ob, &CK_ACT_BirdLayEgg);
				ob->velX = 0;
				ob->velY = 0;
			}

		}

		// make the other birds start the attack animation:
		for (hit = ck_keenObj->next; hit; hit = hit->next)
		{
			if (hit != ob && hit->type == CT_Bird && hit->active && hit->currentAction != &CK_ACT_BirdLayEgg)
			{
				CK_SetAction2(hit, &CK_ACT_BirdAttack);
			}
		}
		break;
	}
}

void BirdReact(CK_object *ob)
{
	if ((ob->leftTI && ob->xDirection > 0) || (ob->rightTI && ob->xDirection < 0))
	{
		ob->velX = -ob->velX;
		ob->xDirection = -ob->xDirection;
		if (ob->currentAction == &CK_ACT_BirdAttack)
			CK_SetAction2(ob, &CK_ACT_BirdFly1);
	}
	if ((ob->topTI && ob->yDirection > 0) || (ob->bottomTI && ob->yDirection < 0))
	{
		ob->velY = -ob->velY;
		ob->xDirection = -ob->xDirection;
		if (ob->currentAction == &CK_ACT_BirdAttack)
			CK_SetAction2(ob, &CK_ACT_BirdFly1);
	}
	CK_BasicDrawFunc1(ob);
}

void BirdEggThink(CK_object *ob)
{
	CK_Fall(ob);
	if (ob->gfxChunk == EGG3SPR)
	{
		if (ob->user1 < 4*TickBase)
		{
			ob->user1 += tics;
		}
		// if grabbed by Nisasi AND landed on ground, then start breaking the egg:
		if (ob->user4 && ob->topTI)
		{
			ob->gfxChunk--;
		}
	}
	else if (ob->gfxChunk < EGG3SPR)
	{
		if ((ob->actionTimer += tics) >= 10)
		{
			ob->actionTimer -= 10;
			if (ob->gfxChunk == EGG1SPR)
			{
				CK_ObjDropNisasi(ob);

				// if there's a bird in the egg, let it hatch, otherwise just break the egg
				if (ob->type == CT_BirdEgg && ob->user1 >= 4*TickBase)
				{
					SD_PlaySound(BIRDLAYSEGGSND);
					ob->gfxChunk = EGGBIRD1SPR;
				}
				else
				{
					
					SD_PlaySound(EGGCRACKSND);
					ob->gfxChunk = EGGCRACK1SPR;
					ob->active = OBJ_EXISTS_ONLY_ONSCREEN;
				}

				// spawn some flying shell pieces
				{
					CK_object *o;
					int i;

					for (i=2; i>0; i--)			// Nisaba patch: we only need two eggshell splitter
					{
						o = CK_GetNewObj(true);
						o->posX = ob->posX + TILEGLOBAL;
						o->posY = ob->posY + TILEGLOBAL;
						if (i == 1)
						{
							o->velX = 48 - (US_RndT() & 15);
						}
						else if (i == 2)
						{
							o->velX = -48 + (US_RndT() & 15);
						}
						else
						{
							o->velX = 8 - (US_RndT() & 15);
						}
						o->velY = -48 + (US_RndT() & 15);
						o->gfxChunk = i + EGGSHELL1SPR - 1;
						o->active = OBJ_EXISTS_ONLY_ONSCREEN;
						o->zLayer = 1;
						CK_SetAction(o, &CK_ACT_EggPiece);
					}
				}
			}
			else
			{
				ob->gfxChunk--;
			}
		}
	}
	else if (ob->gfxChunk >= EGGBIRD1SPR && ob->gfxChunk <= EGGBIRD5SPR)
	{
		if ((ob->actionTimer += tics) >= 10)
		{
			ob->actionTimer -= 10;
			ob->gfxChunk++;

			if (ob->gfxChunk == EGGBIRD2SPR)
			{
				// spawn a new bird
				CK_object *bird;

				bird = CK_GetNewObj(true);
				bird->zLayer = 1;
				bird->type = CT_Bird;
				bird->posX = ob->posX;
				bird->posY = ob->posY - TILEGLOBAL;
				bird->user2 = true;	// new bird is fertile, too
				bird->xDirection = (ck_keenObj->clipRects.unitXmid < bird->clipRects.unitXmid) ? motion_Left : motion_Right;
				CK_SetAction(bird, &CK_ACT_BirdFly1);

				ob->active = OBJ_EXISTS_ONLY_ONSCREEN;
			}

			if (ob->gfxChunk > EGGBIRD5SPR)
				CK_RemoveObj(ob);
		}
	}
	else if (ob->gfxChunk >= EGGCRACK1SPR && ob->gfxChunk <= EGGCRACK4SPR)
	{
		if ((ob->actionTimer += tics) >= 10)
		{
			ob->actionTimer -= 10;
			ob->gfxChunk++;
		}
	}
	else if (ob->gfxChunk == EGGCRACK5SPR)
	{
		if ((ob->actionTimer += tics) >= 5*TickBase)
		{
			ob->actionTimer -= 5*TickBase;
			ob->gfxChunk++;
		}
	}
	else if (ob->gfxChunk == EGGCRACK6SPR)
	{
		if ((ob->actionTimer += tics) >= 14)
			CK_RemoveObj(ob);
	}
	else
	{
		// this should be unreachable, but just in case...
		CK_RemoveObj(ob);
	}
}

void BirdEggContact(CK_object *ob, CK_object *hit)
{
	if (ob->gfxChunk == EGG3SPR)
	{
		switch (hit->type)
		{
		case CT_Stunner:
			CK_ShotHit(hit);
			// no break here!
		case CT_Player:
			// break the egg:
			SD_PlaySound(EGGCRACKSND);
			ob->gfxChunk--;
			break;
		}
	}
	else if (ob->gfxChunk >= EGGCRACK1SPR && ob->gfxChunk <= EGGCRACK6SPR)
	{
		if (hit->type == CT_Player)
			CK_KillKeen();
	}
}

void BirdEggReact(CK_object *ob)
{
	if (ob->topTI)
	{
		ob->velY = 0;
		// make the egg break if there's no bird in it:
		if (ob->gfxChunk == EGG3SPR && ob->type != CT_BirdEgg)
			ob->gfxChunk--;
	}
	CK_BasicDrawFunc1(ob);
}

void EggPieceReact(CK_object *ob)
{
	if (ob->leftTI || ob->rightTI || ob->topTI || ob->bottomTI)
	{
		CK_RemoveObj(ob);
	}
	else
	{
		CK_BasicDrawFunc1(ob);
	}
}

CK_action CK_ACT_BirdFly1 = {BIRDFLYL1SPR, BIRDFLYR1SPR, AT_UnscaledFrame, false, push_none, 8, 0, 0, BirdFlyThink, BirdContact, BirdReact, &CK_ACT_BirdFly2};
CK_action CK_ACT_BirdFly2 = {BIRDFLYL2SPR, BIRDFLYR2SPR, AT_UnscaledFrame, false, push_none, 8, 0, 0, BirdFlyThink, BirdContact, BirdReact, &CK_ACT_BirdFly3};
CK_action CK_ACT_BirdFly3 = {BIRDFLYL3SPR, BIRDFLYR3SPR, AT_UnscaledFrame, false, push_none, 8, 0, 0, BirdFlyThink, BirdContact, BirdReact, &CK_ACT_BirdFly4};
CK_action CK_ACT_BirdFly4 = {BIRDFLYL4SPR, BIRDFLYR4SPR, AT_UnscaledFrame, false, push_none, 8, 0, 0, BirdFlyThink, BirdContact, BirdReact, &CK_ACT_BirdFly5};
CK_action CK_ACT_BirdFly5 = {BIRDFLYL5SPR, BIRDFLYR5SPR, AT_UnscaledFrame, false, push_none, 8, 0, 0, BirdFlyThink, BirdContact, BirdReact, &CK_ACT_BirdFly1};
CK_action CK_ACT_BirdAttack = {0, 0, AT_Frame, false, push_none, 0, 0, 0, BirdAttackThink, BirdContact, BirdReact, NULL};
CK_action CK_ACT_BirdLayEgg = {BIRDFLAYEGGL1SPR, BIRDFLAYEGGR1SPR, AT_Frame, false, push_none, 0, 0, 0, BirdLayEggThink, BirdContact, BirdReact, NULL};
CK_action CK_ACT_BirdFall = {BIRDSHOTSPR, BIRDSHOTSPR, AT_Frame, false, push_none, 0, 0, 0, CK_Fall, NULL, CK_BasicDrawFunc4, &CK_ACT_BirdStunned};
CK_action CK_ACT_BirdStunned = {BIRDSTUNNEDSPR, BIRDSTUNNEDSPR, AT_Frame, false, push_none, 0, 0, 0, CK_Fall, NULL, CK_BasicDrawFunc4, NULL};

CK_action CK_ACT_BirdEgg = {0, 0, AT_Frame, false, push_none, 0, 0, 0, BirdEggThink, BirdEggContact, BirdEggReact, NULL};
CK_action CK_ACT_EggPiece = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CK_Fall, 0, EggPieceReact, NULL};


/*
==============================================================================
	SNEAKY TREE

	A tree stump that Keen can stand on. Looks around at random while Keen is
	NOT standing on it. Moves away from Keen when Keen is not looking at it.

	user1 - state (see enum)
	user2 - look counter
	user3 - blocked direction (1 = can't go right; -1 = can't go left)
	user4 - Nisasi ptr
==============================================================================
*/

enum {ST_LOOK, ST_EXTEND, ST_RETRACT, ST_SLIDE};

//#define SNEAKYTREE_LOOKDELAY (ck_gameState.difficulty == D_Easy? (TickBase/3) : (TickBase/2)) // half a second
#define SNEAKYTREE_LOOKDELAY (TickBase/2) // half a second
#define SNEAKYTREE_EXTENDDELAY (10) 
#define SNEAKYTREE_RETRACTDELAY (5) 
#define SNEAKYTREE_SLIDERETRACTDELAY (5)
#define SNEAKYTREE_SLIDESPEED 48
#define SNEAKYTREE_SLIDESPEED2 32

CK_action CK_ACT_SneakyTree;

void CK_SpawnSneakyTree(int tileX, int tileY)
{
	CK_object *ob;

	ob = CK_GetNewObj(false);
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY) - 24*PIXGLOBAL;
	ob->zLayer = 0;
	ob->type = CT_SneakyTree;
	ob->clipped = CLIP_custom;
	ob->gfxChunk = SNEAKYHIDESPR;
	CK_SetAction(ob, &CK_ACT_SneakyTree);
}

void SneakyTreeThink(CK_object *ob)
{
	if (ob->velX || ob->user1 == ST_SLIDE)
	{
		CK_PhysDampHorz2(ob);	// DampHorz2 means higher friction

		if (ob->user1 == ST_SLIDE)
		{
			if ((ob->actionTimer += tics) >= SNEAKYTREE_SLIDERETRACTDELAY)
			{
				ob->actionTimer -= SNEAKYTREE_SLIDERETRACTDELAY;

				if (ob->gfxChunk == SNEAKYSLIDEL1SPR)
				{
					ob->gfxChunk = SNEAKYLOOKLSPR;
					ob->user1 = ST_LOOK;
				}
				else if (ob->gfxChunk == SNEAKYSLIDER1SPR)
				{
					ob->gfxChunk = SNEAKYLOOKRSPR;
					ob->user1 = ST_LOOK;
				}
				else
				{
					ob->gfxChunk--;
				}
			}
		}
		else
		{
			ob->gfxChunk = ob->xDirection < 0 ? SNEAKYLOOKLSPR : SNEAKYLOOKRSPR;
		}
	}
	else if (ob->user1 == ST_EXTEND)
	{
		if (ob->xDirection > 0)
		{
			if (ck_keenObj->xDirection < 0 && ck_keenObj->clipRects.unitX2 < ob->clipRects.unitX1)
			{
				if ((ob->actionTimer += tics) >= SNEAKYTREE_EXTENDDELAY)
				{
					ob->actionTimer -= SNEAKYTREE_EXTENDDELAY;

					if (ob->gfxChunk == SNEAKYSLIDER4SPR)
					{
						ob->gfxChunk--;// = SNEAKYLOOKRSPR;
						ob->velX = ob->user3 ? SNEAKYTREE_SLIDESPEED2 : SNEAKYTREE_SLIDESPEED;
						ob->user1 = ST_SLIDE;
						ob->user3 = 0;
						ob->actionTimer = 0;
					}
					else
					{
						ob->gfxChunk++;
					}
				}
			}
			else
			{
				ob->user1 = ST_RETRACT;
				ob->actionTimer = 0;
			}
		}
		else
		{
			if (ck_keenObj->xDirection > 0 && (ck_keenObj->clipRects.unitX1 > ob->clipRects.unitX2
				|| (ob->user3 == motion_Right && ck_keenObj->clipRects.unitX1 >= ob->posX + 22*PIXGLOBAL)))
			{
				if ((ob->actionTimer += tics) >= SNEAKYTREE_EXTENDDELAY)
				{
					ob->actionTimer -= SNEAKYTREE_EXTENDDELAY;

					if (ob->gfxChunk == SNEAKYSLIDEL4SPR)
					{
						ob->gfxChunk--;// = SNEAKYLOOKLSPR;
						ob->velX = ob->user3 ? -SNEAKYTREE_SLIDESPEED2 : -SNEAKYTREE_SLIDESPEED;
						ob->user1 = ST_SLIDE;
						ob->user3 = 0;
						ob->actionTimer = 0;
					}
					else
					{
						ob->gfxChunk++;
					}
				}
			}
			else
			{
				ob->user1 = ST_RETRACT;
				ob->actionTimer = 0;
			}
		}
	}
	else if (ob->user1 == ST_RETRACT)
	{
		if ((ob->actionTimer += tics) >= SNEAKYTREE_RETRACTDELAY)
		{
			ob->actionTimer -= SNEAKYTREE_RETRACTDELAY;

			if (ob->gfxChunk == SNEAKYSLIDEL1SPR || ob->gfxChunk == SNEAKYSLIDER1SPR)
			{
				ob->gfxChunk = SNEAKYLOOKUSPR;
				ob->user1 = ST_LOOK;
			}
			else
			{
				ob->gfxChunk--;
			}
		}
	}
	else if (ob == ck_keenState.platform || ob->user4)
	{
		if ((ob->actionTimer += tics) >= SNEAKYTREE_LOOKDELAY)
		{
			ob->actionTimer -= SNEAKYTREE_LOOKDELAY;

			ob->gfxChunk = US_RndT() < 0x10 ? SNEAKYHIDESPR : SNEAKYLOOKUSPR;
		}
		ob->user1 = ST_LOOK;
		ob->user2 = 0;
	}
	else //if (ob->user1 == ST_LOOK)
	{
		int xdist = ABS(ob->clipRects.unitXmid - ck_keenObj->clipRects.unitXmid);

		if (ck_keenObj->xDirection < 0 && ck_keenObj->clipRects.unitX2 < ob->clipRects.unitX1)
		{
			ob->xDirection = motion_Right;
		}
		else if (ck_keenObj->xDirection > 0 && (ck_keenObj->clipRects.unitX1 > ob->clipRects.unitX2
			|| (ob->user3 == motion_Right && ck_keenObj->clipRects.unitX1 >= ob->posX + 22*PIXGLOBAL)))
		{
			ob->xDirection = motion_Left;
		}
		else
		{
			ob->xDirection = motion_None;
		}

		if (ob->xDirection == motion_None || xdist > 6*TILEGLOBAL)
		{
			ob->gfxChunk = SNEAKYHIDESPR;
			ob->actionTimer = 0;
			ob->user2 = 0;
		}
		else if ((ob->actionTimer += tics) >= SNEAKYTREE_LOOKDELAY)
		{
			ob->actionTimer -= SNEAKYTREE_LOOKDELAY;

			ob->gfxChunk = US_RndT() < 0x80 ? SNEAKYLOOKLSPR : SNEAKYLOOKRSPR;
			ob->user2++;

			if (ob->user2 > (ck_gameState.difficulty == D_Easy? 6 : 10))
			{
				if (ob->user3 != ob->xDirection)
				{
					ob->gfxChunk = ob->xDirection > 0 ? SNEAKYSLIDER1SPR : SNEAKYSLIDEL1SPR;
					ob->user1 = ST_EXTEND;
				}
				ob->user2 = 0;
			}
		}
	}
}

void SneakyTreeReact(CK_object *ob)
{
	if ((ob->xDirection > 0 && ob->leftTI) || (ob->xDirection < 0 && ob->rightTI) || !ob->topTI)
	{
		ob->user3 = ob->xDirection;
		ob->velX = 0;
		if (!ob->topTI)
		{
			ob->posX -= ob->deltaPosX;
			ob->posY -= ob->deltaPosY;
		}
	}
	CK_BasicDrawFunc1(ob);
}

CK_action CK_ACT_SneakyTree = {0, 0, AT_Frame, false, push_down, 0, 0, 0, SneakyTreeThink, NULL, SneakyTreeReact, NULL};

#undef SNEAKYTREE_LOOKDELAY
#undef SNEAKYTREE_EXTENDDELAY
#undef SNEAKYTREE_RETRACTDELAY
#undef SNEAKYTREE_SLIDERETRACTDELAY
#undef SNEAKYTREE_SLIDESPEED
#undef SNEAKYTREE_SLIDESPEED2

/*
==============================================================================
	CROCODENT

	Walks around and has a random chance of jumping. Turns into a trap when
	Keen is near, trap is activated when Keen touches it or tries to jump over
	it. Trap also activates automatically after 8 seconds.

	user1 - counter for leaving the trap state
	user2 - random jump allowed (boolean)
	user3 - old tileXmid position
	user4 - Nisasi pointer
==============================================================================
*/

#define CROC_HIGHJUMP -52
#define CROC_LOWJUMP  -32
#define CROC_XSPEED    26

CK_action CK_ACT_CrocWalk;
CK_action CK_ACT_CrocJump;
CK_action CK_ACT_CrocTrap;
CK_action CK_ACT_CrocSnap;

void CK_SpawnCroc(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);

	ob->type = CT_Crocodent;
	ob->zLayer = 1;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY) + 2*PIXGLOBAL;
	if (US_RndT() < 0x80)
	{
		ob->xDirection = motion_Left;
		ob->gfxChunk = CROCOWALKL1SPR;
	}
	else
	{
		ob->xDirection = motion_Right;
		ob->gfxChunk = CROCOWALKR1SPR;
	}
	CK_SetAction(ob, &CK_ACT_CrocWalk);
}

void CrocCheckTiles(CK_object *ob)
{
	int w,h,i,skip,off;

	h = ob->clipRects.tileY2 - ob->clipRects.tileY1 + 1;
	w = ob->clipRects.tileX2 - ob->clipRects.tileX1 + 1;
	skip = mapwidth - w;
	off = ob->clipRects.tileX1 + mapbwidthtable[ob->clipRects.tileY1]/2;
	while (h--)
	{
		i = w;
		while (i--)
		{
			register int info = mapsegs[2][off++];

			switch (info-DIRARROWSTART)
			{
			case arrow_East:
				if (ob->xDirection > 0)
					ob->user2 = true;
				else
					ob->user2 = false;
				break;

			case arrow_West:
				if (ob->xDirection < 0)
					ob->user2 = true;
				else
					ob->user2 = false;
				break;
			}
		}
		off += skip;
	}
}

void CrocWalkThink(CK_object *ob)
{
	int xdist, ydist;

	CrocCheckTiles(ob);

	xdist = ob->clipRects.unitXmid - ck_keenObj->clipRects.unitXmid;
	ydist = ob->clipRects.unitY2 - ck_keenObj->clipRects.unitY2;

	if (!ob->user4
		&& ABS(xdist) < 4*TILEGLOBAL
	//	&& ck_keenObj->currentAction->stickToGround
		&& ABS(ydist) < 3*TILEGLOBAL)
	{
		ck_nextX = 0;	// no movement!
		ob->gfxChunk = ob->xDirection < 0 ? CROCOTRAPOPENL1SPR : CROCOTRAPOPENR1SPR;
		ob->currentAction = &CK_ACT_CrocTrap;
	}
	else if (ob->xDirection < 0)
	{
		if (++ob->gfxChunk > CROCOWALKL4SPR)
			ob->gfxChunk = CROCOWALKL1SPR;

		if (!ob->user4)	// not grabbed by Nisasi?
		{
			// start jumping when standing in a jump arrow spot:
			if (ob->user3 != ob->clipRects.tileXmid // only do this check once per tile!
				&& CA_TileAtPos(ob->clipRects.tileXmid,ob->clipRects.tileY2,2) == DIRARROWSTART+arrow_NorthWest
				&& US_RndT() < 100)
			{
				ob->velY = CROC_HIGHJUMP;
				ob->velX = -CROC_XSPEED;
				ob->gfxChunk = CROCOJUMPL1SPR;
				ob->currentAction = &CK_ACT_CrocJump;
			}
			// otherwise try to do a random jump, if allowed:
			else if (ob->user2 && US_RndT() < 8)
			{
				ob->velY = CROC_LOWJUMP;
				ob->velX = -CROC_XSPEED;
				ob->gfxChunk = CROCOJUMPL1SPR;
				ob->currentAction = &CK_ACT_CrocJump;
			}
		}
	}
	else
	{
		if (++ob->gfxChunk > CROCOWALKR4SPR)
			ob->gfxChunk = CROCOWALKR1SPR;

		if (!ob->user4)	// not grabbed by Nisasi?
		{
			// start jumping when standing in a jump arrow spot:
			if (ob->user3 != ob->clipRects.tileXmid // only do this check once per tile!
				&& CA_TileAtPos(ob->clipRects.tileXmid,ob->clipRects.tileY2,2) == DIRARROWSTART+arrow_NorthEast
				&& US_RndT() < 100)
			{
				ob->velY = CROC_HIGHJUMP;
				ob->velX = CROC_XSPEED;
				ob->gfxChunk = CROCOJUMPR1SPR;
				ob->currentAction = &CK_ACT_CrocJump;
			}
			// otherwise try to do a random jump, if allowed:
			else if (ob->user2 && US_RndT() < 8)
			{
				ob->velY = CROC_LOWJUMP;
				ob->velX = CROC_XSPEED;
				ob->gfxChunk = CROCOJUMPR1SPR;
				ob->currentAction = &CK_ACT_CrocJump;
			}
		}
	}

	ob->user3 = ob->clipRects.tileXmid;	// remember old tile position for the jump checks
}

void CrocJumpThink(CK_object *ob)
{
	CK_Fall(ob);	// medium gravity
	CrocCheckTiles(ob);

	if (ck_nextY > 0)
		ob->gfxChunk = ob->xDirection < 0 ? CROCOJUMPL2SPR : CROCOJUMPR2SPR;
}

void CrocTrapThink(CK_object *ob)
{
	switch (ob->gfxChunk)
	{
	case CROCOTRAPOPENR1SPR:
	case CROCOTRAPOPENL1SPR:
		if ((ob->actionTimer += tics) >= 10)
		{
			ob->actionTimer -= 10;
			ob->gfxChunk++;
		}
		break;
	case CROCOTRAPOPENR2SPR:
	case CROCOTRAPOPENL2SPR:
		if ((ob->actionTimer += tics) >= 10)
		{
			ob->actionTimer -= 10;
			ob->gfxChunk = CROCOTRAPOPEN3SPR;
		}
		break;
	case CROCOTRAPOPEN3SPR:
		if ((ob->actionTimer += tics) < 10)
			break;
		ob->actionTimer = 0;
		ob->user1 = 0;
	case CROCOTRAP1SPR:
	case CROCOTRAP2SPR:
		if (ob->actionTimer == 0)
		{
			if (ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid)
			{
				ob->gfxChunk = CROCOTRAP1SPR;
			}
			else
			{
				ob->gfxChunk = CROCOTRAP2SPR;
			}
			// randomly look in the other direction:
			if (US_RndT() < 50)
			{
				ob->gfxChunk ^= (CROCOTRAP1SPR ^ CROCOTRAP2SPR);
			}
			ob->actionTimer = 0.5*TickBase;
		}
		else
		{
			if ((ob->actionTimer -= tics) < 0)
				ob->actionTimer = 0;
		}

		if (ck_keenObj->clipRects.unitY2 <= ob->clipRects.unitY2
			&& ck_keenObj->clipRects.unitX2 > ob->clipRects.unitX1
			&& ck_keenObj->clipRects.unitX1 < ob->clipRects.unitX2)
		{
			SD_PlaySound(TENTACLESWOOSHSND);
			ob->gfxChunk = CROCOTRAPSNAP1SPR;
			ob->velX = 0;
			ob->velY = -48;
			ob->currentAction = &CK_ACT_CrocSnap;
		}

		// cancel trap mode after 5 seconds
		if ((ob->user1 += tics) >= 5*TickBase)
		{
			ob->user1 = 0;
			ob->gfxChunk = CROCOTRAPSNAP1SPR;
			ob->velX = 0;
			ob->velY = -24;
			ob->currentAction = &CK_ACT_CrocSnap;
		}

		break;
	}
}

void CrocSnapThink(CK_object *ob)
{
	CK_Fall(ob);

/*	
	if (ck_nextY >= 0 && ob->topTI)
	{
		if ((ob->actionTimer += tics) >= 40)
		{
			ob->gfxChunk = ob->xDirection < 0 ? CROCOWALKL1SPR : CROCOWALKR1SPR;
			ob->currentAction = &CK_ACT_CrocWalk;
		}
	}
	else
*/
	{
		if (ck_nextY > 0)
			ob->gfxChunk = CROCOTRAPSNAP2SPR;
	}
}

void CrocContact(CK_object *ob, CK_object *hit)
{
	if (hit->type == CT_Player
		&& ob->gfxChunk >= CROCOTRAPOPENR1SPR && ob->gfxChunk <= CROCOTRAPSNAP1SPR)
	{
		CK_KillKeen();
		if (ob->currentAction == &CK_ACT_CrocTrap)
		{
			ob->gfxChunk = CROCOTRAPSNAP1SPR;
			ob->velX = 0;
			ob->velY = -48;
			CK_SetAction2(ob, &CK_ACT_CrocSnap);
		}
	}
	
#ifdef EXTRA_INTERACTIONS
	else if (hit->type == CT_Skypest && ob->gfxChunk == CROCOTRAPSNAP1SPR)
	{
		//TODO: make sure the skypest is in the crocodent's mouth
		CK_RemoveObj(hit);
		ob->gfxChunk++;
	}
#endif
}

void CrocWalkReact(CK_object *ob)
{
	if (ob->xDirection > 0 && ob->leftTI)
	{
		ob->xDirection = motion_Left;
		ob->gfxChunk += CROCOWALKL1SPR - CROCOWALKR1SPR;
	}
	else if (ob->xDirection < 0 && ob->rightTI)
	{
		ob->xDirection = motion_Right;
		ob->gfxChunk += CROCOWALKR1SPR - CROCOWALKL1SPR;
	}
	if (!ob->topTI)	// not on solid ground anymore?
	{
#if 0
		// old code, not needed anymore:

		// TODO: just turn around if it's not "safe" to jump
		if (ob->user4)
		{
			ob->xDirection = -ob->xDirection;
			ob->posX -= ob->deltaPosX;
			ob->posY -= ob->deltaPosY;
			ob->gfxChunk += (ob->xDirection < 0)? (CROCOWALKL1SPR - CROCOWALKR1SPR) : (CROCOWALKR1SPR - CROCOWALKL1SPR);
		}
		else
		{
			if (ob->xDirection < 0)
			{
				ob->gfxChunk = CROCOJUMPL1SPR;
				ob->velX = -CROC_XSPEED;
			}
			else
			{
				ob->gfxChunk = CROCOJUMPR1SPR;
				ob->velX = CROC_XSPEED;
			}
			ob->velY = CROC_LOWJUMP;
			CK_SetAction2(ob, &CK_ACT_CrocJump);
		}
#else
		// new code: always turn around (jump would have been initiated by CrocWalk)

		// update sprite number, so TurnX uses the correct hitbox:
		ob->gfxChunk += (ob->xDirection > 0)? (CROCOWALKL1SPR - CROCOWALKR1SPR) : (CROCOWALKR1SPR - CROCOWALKL1SPR);
		TurnX(ob);
		if (!ob->topTI)	// still not on solid ground?
		{
			// just start falling:
			ob->xDirection = -ob->xDirection;	// back to old direction
			ob->gfxChunk = (ob->xDirection < 0)? CROCOJUMPL1SPR : CROCOJUMPR1SPR;
			ob->velX = ob->velY = 0;
			CK_SetAction2(ob, &CK_ACT_CrocJump);
		}
#endif
	}
	CK_BasicDrawFunc1(ob);
}

void CrocJumpReact(CK_object *ob)
{
	if (ob->bottomTI)
		ob->velY = 0;
	if (ob->leftTI && ob->velX > 0 || ob->rightTI && ob->velX < 0)
		ob->velX = 0;
	if (ob->topTI)
	{
		ob->velY = 0;
		ob->gfxChunk = ob->xDirection < 0 ? CROCOWALKL1SPR : CROCOWALKR1SPR;
		CK_SetAction2(ob, &CK_ACT_CrocWalk);
	}
	CK_BasicDrawFunc1(ob);
}

void CrocSnapReact(CK_object *ob)
{
	if (ob->bottomTI)
		ob->velY = 0;
	if (ob->topTI && ob->velY >= 0)
	{
		ob->velY = 0;
		// turn to face Keen:
		ob->xDirection = (ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid)? motion_Left : motion_Right;
		if (US_RndT() < 77)	// croc has a 30% chance to walk away from Keen after landing
			ob->xDirection = -ob->xDirection;
		ob->gfxChunk = ob->xDirection < 0 ? CROCOWALKL1SPR : CROCOWALKR1SPR;
		CK_SetAction2(ob, &CK_ACT_CrocWalk);
	//	ob->gfxChunk = CROCOTRAPSNAP3SPR;
	}
	CK_BasicDrawFunc1(ob);
}

CK_action CK_ACT_CrocWalk = {0, 0, AT_UnscaledOnce, false, push_down, 10, 50, 0, CrocWalkThink, NULL, CrocWalkReact, &CK_ACT_CrocWalk};
CK_action CK_ACT_CrocJump = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CrocJumpThink, NULL, CrocJumpReact, NULL};
CK_action CK_ACT_CrocTrap = {0, 0, AT_Frame, false, push_down, 0, 0, 0, CrocTrapThink, CrocContact, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_CrocSnap = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CrocSnapThink, CrocContact, CrocSnapReact, NULL};

#undef CROC_HIGHJUMP
#undef CROC_LOWJUMP
#undef CROC_XSPEED

/*
==============================================================================
	TENTACLE NEST

	Stationary object that pulls Keen or Skypests into its mouth to kill them.

------------------------------------------------------------------------------
	SWALLOWED OBJECT

	user1 - pointer to the tentacle nest object
	user2 - animation delay
	user3 - first animation sprite
	user4 - last animation sprite
==============================================================================
*/

#define NEST_ANIMDELAY 10

CK_action CK_ACT_TentacleNest;

void CK_SpawnTentacleNest(int tileX, int tileY)
{
	CK_object *ob;

	ob = CK_GetNewObj(false);
	ob->type = CT_TentacleNest;
	ob->clipped = CLIP_not;
	ob->zLayer = 2;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	ob->gfxChunk = GRETENTA1SPR;
	CK_SetAction(ob, &CK_ACT_TentacleNest);
}

void TentacleNestThink(CK_object *ob)
{
	if ((ob->actionTimer += tics) >= NEST_ANIMDELAY)
	{
		ob->actionTimer -= NEST_ANIMDELAY;
		ob->gfxChunk++;
		if (ob->gfxChunk > GRETENTA8SPR)
			ob->gfxChunk = GRETENTA1SPR;
	}

	//TODO: spawn mindcontrol creature?
}

void TentacleNestContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		if (ck_gameState.godMode || ck_mapState.invincibilityTimer || CK_KeenIsDead())
			return;
		ck_scrollDisabled = true;
		SD_PlaySound(DEATHTENTACLENESTSND);
		hit->user3 = hit->gfxChunk = EFREEKEEN1SPR;
		hit->user4 = EFREEKEEN2SPR;
		hit->user2 = 30;
		CK_DropNisasi();
		break;
	case CT_Skypest:
		hit->user3 = hit->gfxChunk = SKYPESTFLYL1SPR;
		hit->user4 = SKYPESTFLYL2SPR;
		hit->user2 = 5;
		break;
	default:
		return;
	}
	CK_SetAction(hit, &CK_ACT_Swallowed);
	hit->clipped = CLIP_not;
	hit->type = CT_Friendly;
	hit->user1 = (int)ob;
	hit->zLayer = ob->zLayer-1;
	if (hit->zLayer < 0)
		hit->zLayer = 0;
	hit->yDirection = (hit->clipRects.unitY1 < ob->posY+24*PIXGLOBAL)? motion_Down : motion_Up;
}

void SwallowedThink(CK_object *ob)
{
#define XSPEED 8
#define YSPEED 4

	CK_object *nest = (CK_object*)ob->user1;

	ck_nextX = nest->posX+28*PIXGLOBAL - ob->clipRects.unitXmid;
	if (ob->yDirection > 0)
	{
		ck_nextY = PIXGLOBAL*tics;
		if (ob->clipRects.unitY1 >= nest->posY+24*PIXGLOBAL)
			ob->yDirection = motion_Up;
	}
	else// if (ob->yDirection <= 0)
	{
		ck_nextY = -YSPEED*tics;
	}
	if (ck_nextX < 0 && ck_nextX < -XSPEED*tics)
	{
		ck_nextX = -XSPEED*tics;
	}
	else if (ck_nextX > 0 && ck_nextX > XSPEED*tics)
	{
		ck_nextX = XSPEED*tics;
	}

	if ((ob->actionTimer += tics) > ob->user2)
	{
		ob->actionTimer -= ob->user2;
		if (++ob->gfxChunk > ob->user4)
			ob->gfxChunk = ob->user3;
	}

	if (ob->yDirection <= 0 && ob->clipRects.unitY2 < nest->posY+24*PIXGLOBAL)
	{
		if (ob == ck_keenObj)
		{
			ob->gfxChunk = 0;
			ck_levelState = LS_Died;
		}
		else
		{
			CK_RemoveObj(ob);
		}
	}

#undef XSPEED
#undef YSPEED
}

CK_action CK_ACT_TentacleNest = {0, 0, AT_Frame, false, push_none, 0, 0, 0, TentacleNestThink, TentacleNestContact, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_Swallowed = {0, 0, AT_Frame, false, push_none, 0, 0, 0, SwallowedThink, NULL, CK_BasicDrawFunc1, NULL};

/*
==============================================================================
	CHECKPOINT MUSHROOMS

	user1 - 0 if not touched, 1 if touched
==============================================================================
*/

CK_action CK_ACT_Checkpoint;
CK_action CK_ACT_CheckpointCap;
CK_action CK_ACT_Smoke;

void CK_SpawnCheckpoint(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->type = CT_Checkpoint;
	ob->clipped = CLIP_not;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY) - 44*PIXGLOBAL;
	ob->gfxChunk = CHECKPOINT1SPR;
	CK_SetAction(ob, &CK_ACT_Checkpoint);
}

void CheckpointThink(CK_object *ob)
{
	if (ob->user1)
	{
		ob->gfxChunk = CPBIGSHROOMSPR;
	}
	else
	{
		if ((ob->actionTimer+=tics) >= 12)
		{
			ob->actionTimer -= 12;
			if (ob->gfxChunk++ == CHECKPOINT3SPR)
				ob->gfxChunk = CHECKPOINT1SPR;
		}
	}
}

void CheckpointCapThink(CK_object *ob)
{
	// remove the cap when Keen lands on something solid or has fallen out of the cap:
	if (ck_keenObj->topTI || ob->clipRects.unitY2 < ck_keenObj->clipRects.unitY1)
		CK_RemoveObj(ob);
}

CK_object * SpawnSmoke(int x, int y)
{
	CK_object *ob = CK_GetNewObj(true);
	SD_PlaySound(CHECKPOINTSND);
	ob->posX = x;
	ob->posY = y;
	ob->clipped = CLIP_not;
	ob->zLayer = FOREGROUNDPRIORITY-1;
	ob->gfxChunk = SMIRKYPOOF1SPR;
	ob->user1 = SMIRKYPOOF4SPR;
	ob->actionTimer = US_RndT() & 7;
	CK_SetAction(ob, &CK_ACT_Smoke);
	return ob;
}

void SmokeThink(CK_object *ob)
{
	register int maxtics = ob->type == CT_LindseyVanish? 8 : 20;

	if ((ob->actionTimer += tics) >= maxtics)
	{
		ob->actionTimer -= maxtics;
		if (ob->gfxChunk++ == ob->user1)
			CK_RemoveObj(ob);
	}
}

// Note:
// Keen touching the checkpoint is handled in CK_KEEN.C
// CK_PlayLoop() calls CheckpointSave() 

void CheckpointSave(CK_object *ob)
{
	CK_object *o;

	// turn the old "used" checkpoint mushrooms into smoke:
#if 0
	for (o=ck_keenObj; o; o=o->next)
	{
		if (o->type == CT_Checkpoint && o->user1)
		{
			o->posX -= 8*PIXGLOBAL;
			o->posY += TILEGLOBAL;
			o->type = CT_Friendly;
			o->gfxChunk = SMIRKYPOOF1SPR;
			o->user1 = SMIRKYPOOF4SPR;
			o->active = OBJ_EXISTS_ONLY_ONSCREEN;
			CK_SetAction(o, &CK_ACT_Smoke);
		}
	}
#else
	if (ck_mapState.lastCheckpoint)
	{
		o = ck_mapState.lastCheckpoint;
		o->posX -= 8*PIXGLOBAL;
		o->posY += TILEGLOBAL;
		o->type = CT_Friendly;
		o->gfxChunk = SMIRKYPOOF1SPR;
		o->user1 = SMIRKYPOOF4SPR;
		o->active = OBJ_EXISTS_ONLY_ONSCREEN;
		CK_SetAction(o, &CK_ACT_Smoke);
	}
	ck_mapState.lastCheckpoint = ob;
#endif
	ck_mapState.checkpointTouched = NULL;

	// save checkpoint and change object states:
	ob->user1 = true;
//	ob->gfxChunk = CPBIGSHROOMSPR;
//	CK_SetAction(ob, ob->currentAction);

	if (SaveCheckpoint())
	{
		// spawn "CHECKPOINT" sprite:
		o = CK_GetNewObj(true);
		o->posX = ob->posX-24*PIXGLOBAL;
		o->posY = ob->posY+42*PIXGLOBAL;
		o->zLayer = FOREGROUNDPRIORITY;
		o->clipped = CLIP_not;
		o->type = CT_Friendly;
		o->gfxChunk = CHECKPOINTRISESPR;
		o->yDirection = motion_Up;
		CK_SetAction(o, &CK_ACT_itemNotify);
		o->actionTimer = -40;	// stay visible a little longer than the score numbers
		
		// spawn some smoke clouds:
		o = SpawnSmoke(ob->posX-4*PIXGLOBAL, ob->posY-6*PIXGLOBAL);
		o = SpawnSmoke(ob->posX-20*PIXGLOBAL, ob->posY-8*PIXGLOBAL);
		o->gfxChunk++;
		o = SpawnSmoke(ob->posX-4*PIXGLOBAL, ob->posY+20*PIXGLOBAL);
		o->gfxChunk++;
		o = SpawnSmoke(ob->posX-20*PIXGLOBAL, ob->posY+20*PIXGLOBAL);

		// play checkpoint sound effect:
		// (down here to avoid conflicts with SpawnSmoke and its sound effect)
		SD_PlayTwoSounds(CHECKPOINTSND, CHECKPOINT2SND); 
	}
	else
	{
		ck_mapState.lastCheckpoint = NULL;
		CK_RemoveObj(ob);
	}
}

boolean CheckpointLoad(void)
{
	CK_object *o;

	if (!LoadCheckpoint())
		return false;

	// find the checkpoint object:
#if 0
	for (o=ck_keenObj; o; o=o->next)
	{
		if (o->type == CT_Checkpoint && o->user1)
		{
			// move Keen into the cap of the checkpoint mushroom:
			ck_keenObj->posX = o->posX;
			ck_keenObj->posY = o->posY;

			// spawn a second cap on top of Keen (so Keen falls out of the cap):
			o = CK_GetNewObj(true);
			o->clipped = CLIP_not;
			o->posX = ck_keenObj->posX;
			o->posY = ck_keenObj->posY;
			o->zLayer = FOREGROUNDPRIORITY-1;
			o->gfxChunk = CPSHROOMCAPSPR;
			CK_SetAction(o, &CK_ACT_CheckpointCap);

			// make Keen fall:
			ck_keenObj->clipped = CLIP_normal;	// just in case Keen was in noclip mode
			ck_keenObj->velX = 0;
			ck_keenObj->velY = 0;
			ck_keenObj->yDirection = motion_Down;
			CK_SetAction(ck_keenObj, &CK_ACT_keenFall1);
			break;
		}
	}
#else
	if (ck_mapState.lastCheckpoint)
	{
		o = ck_mapState.lastCheckpoint;
		// move Keen into the cap of the checkpoint mushroom:
		ck_keenObj->posX = o->posX;
		ck_keenObj->posY = o->posY;

		// spawn a second cap on top of Keen (so Keen falls out of the cap):
		o = CK_GetNewObj(true);
		o->clipped = CLIP_not;
		o->posX = ck_keenObj->posX;
		o->posY = ck_keenObj->posY;
		o->zLayer = FOREGROUNDPRIORITY-1;
		o->gfxChunk = CPSHROOMCAPSPR;
		CK_SetAction(o, &CK_ACT_CheckpointCap);

		// make Keen fall:
		ck_keenObj->clipped = CLIP_normal;	// just in case Keen was in noclip mode
		ck_keenObj->velX = 0;
		ck_keenObj->velY = 0;
		ck_keenObj->yDirection = motion_Down;
		CK_SetAction(ck_keenObj, &CK_ACT_keenFall1);

	}
#endif
	RemoveDreamBubbles();

	// Note: If no checkpoint object was found, Keen's position is left unchanged
	// and loading the checkpoint works more like loading a saved game.

	if (ck_mapState.invincibilityTimer < CHECKPOINT_INVINCIBILITY)
		ck_mapState.invincibilityTimer = CHECKPOINT_INVINCIBILITY;

	// remove IF-Stunner objects from the level:
	for (o=ck_keenObj; o; o=o->next)
	{
		if (o->type == CT_Item && o->user1 == 11 && o->user4 == -1)
			CK_RemoveObj(o);
	}

	return true;
}

CK_action CK_ACT_Checkpoint = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CheckpointThink, NULL, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_CheckpointCap = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CheckpointCapThink, NULL, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_Smoke = {0, 0, AT_Frame, false, push_none, 8, 0, 0, SmokeThink, NULL, CK_BasicDrawFunc1, NULL};


/*
==============================================================================
	LEAVEA TRIP

	Tree:
	user1: state (see enum)

	Leaf:
	user1 - frame for walking animation ( & 3) or vanish counter (squished leaf)
	user2 - pointer to parent tree object
==============================================================================
*/

CK_action CK_ACT_LeaveaTree;
CK_action CK_ACT_LeaveaFall;
CK_action CK_ACT_LeaveaWalk;

enum {LT_IDLE, LT_BLANK, LT_GROW};

#define LEAVEA_VELX 16

void near RemoveLeaf(CK_object *ob)
{
	((CK_object*)ob->user2)->user1 = LT_GROW;	// allow "mother" to grow another leaf
	CK_RemoveObj(ob);
}

void CK_SpawnLeaveaTrip(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	ob->gfxChunk = LEAVEATREE1SPR;
	ob->type = CT_Friendly;
	CK_SetAction(ob, &CK_ACT_LeaveaTree);
}

void LeaveaTreeThink(CK_object *ob)
{
	int xdist;

	switch (ob->user1)
	{
	case LT_IDLE:
#ifdef EXTRA_INTERACTIONS 	// Nisaba patch: we don't need LeaveaTree animations
		if ((ob->actionTimer += tics) >= 12)		
		{
			ob->actionTimer -= 12;
			if (++ob->gfxChunk > LEAVEATREE3SPR)
				ob->gfxChunk = LEAVEATREE1SPR;
		}
#endif

		xdist = ob->clipRects.unitXmid-ck_keenObj->clipRects.unitXmid;
		if (xdist > -6*TILEGLOBAL && xdist < 6*TILEGLOBAL)
		{
			CK_object *leaf = CK_GetNewObj(true);
			leaf->posX = ob->posX + 6*PIXGLOBAL;
			leaf->posY = ob->clipRects.unitY2;
			leaf->gfxChunk = LEAVEAFALL2SPR;
			leaf->user2 = (int)ob;
			leaf->xDirection = motion_Left;
			leaf->velX = LEAVEA_VELX;
			CK_SetAction(leaf, &CK_ACT_LeaveaFall);
			ob->user1 = LT_BLANK;
			ob->gfxChunk = LEAVEATREE1SPR;
		}
		break;
	case LT_BLANK:
		ob->gfxChunk = LEAVEATREE1SPR;
		break;
	case LT_GROW:
		ob->actionTimer += tics;

#ifdef EXTRA_INTERACTIONS	// Nisaba patch: we don't need LeaveaTree animations
		if (ob->gfxChunk != LEAVEATREE5SPR)
		{											
			if (ob->actionTimer >= 70)
			{
				ob->gfxChunk = LEAVEATREE5SPR;
				ob->actionTimer = 0;
			}
		}
		else if (ob->actionTimer >= 12)
#endif

		{
			ob->actionTimer = 0;
			ob->user1 = LT_IDLE;
			ob->gfxChunk = LEAVEATREE1SPR;
		}
		break;
	}
}

void LeaveaFallThink(CK_object *ob)
{
	if (!CK_ObjectVisible(ob))
	{
		RemoveLeaf(ob);
		return;
	}
#if 0
	if ((ob->actionTimer += tics) >= 8)
	{
		ob->actionTimer -= 8;
		if (++ob->gfxChunk > LEAVEAFALL3SPR)
			ob->gfxChunk = LEAVEAFALL1SPR;
	}
#else
	if (ob->xDirection > 0 && ob->velX >= LEAVEA_VELX
		|| ob->xDirection < 0 && ob->velX <= -LEAVEA_VELX)
	{
		ob->xDirection = -ob->xDirection;
	}
	if (ob->velX >= 5)
	{
		ob->gfxChunk = LEAVEAFALL3SPR;
	}
	else if (ob->velX <= -5)
	{
		ob->gfxChunk = LEAVEAFALL1SPR;
	}
	else
	{
		ob->gfxChunk = LEAVEAFALL2SPR;
	}
	CK_PhysAccelVert(ob, motion_Down, 10);
	CK_PhysAccelHorz(ob, ob->xDirection, LEAVEA_VELX);
#endif
}

void LeaveaFallReact(CK_object *ob)
{
	if (ob->topTI)
	{
		ck_nextY = 3*PIXGLOBAL;
		CK_SetAction2(ob, &CK_ACT_LeaveaWalk);
		if (ck_keenObj->clipRects.unitXmid > ob->clipRects.unitXmid)
		{
			ob->gfxChunk = LEAVEAWALKR1SPR;
			ob->xDirection = motion_Right;
		}
		else
		{
			ob->gfxChunk = LEAVEAWALKL1SPR;
			ob->xDirection = motion_Left;
		}
	}
	CK_BasicDrawFunc1(ob);
}

void LeaveaWalkThink(CK_object *ob)
{
	int xdist;
	ob->zLayer = 2;

	if (!CK_ObjectVisible(ob))
	{
		RemoveLeaf(ob);
		return;
	}

	ob->user1++;
	if (ob->gfxChunk == LEAVEASQUISHEDSPR)
	{
		ck_nextX = 0;	// squished leaves shouldn't move

		// squished leaves disappear after a while, even when they're still visible
		if (ob->user1 > 30)
			RemoveLeaf(ob);
	}
	else
	{
		ob->gfxChunk = LEAVEAWALKR1SPR + (ob->user1 & 3);
		if (ob->xDirection < 0)
			ob->gfxChunk += LEAVEAWALKL1SPR-LEAVEAWALKR1SPR;
	}
}

void LeaveaWalkContact(CK_object *ob, CK_object *hit)
{
	if (hit != ck_keenObj || ob->gfxChunk == LEAVEASQUISHEDSPR)
		return;

	if (hit->currentAction == &CK_ACT_keenPogo1 ||
		hit->currentAction == &CK_ACT_keenPogo2 ||
		hit->currentAction == &CK_ACT_keenPogo3)
	{
		ob->gfxChunk = LEAVEASQUISHEDSPR;
		CK_SetAction2(ob, ob->currentAction);
		SD_PlaySound(SQUISHSND);
		ob->user1 = 0;
		
	}
	else
	{
		PushKeenX(hit, ob);
	}
}

void LeaveaWalkReact(CK_object *ob)
{
	if (!ob->topTI)
	{
		ob->velY = 0;
		ob->velX = ob->xDirection*8;
		CK_SetAction2(ob, &CK_ACT_LeaveaFall);
		CK_BasicDrawFunc1(ob);
	}
	else
	{
		CK_BasicDrawFunc2(ob);
	}
	ob->timeUntillThink = 0;
}

CK_action CK_ACT_LeaveaTree = {0, 0, AT_Frame, false, push_none, 0, 0, 0, LeaveaTreeThink, NULL, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_LeaveaFall = {0, 0, AT_Frame, false, push_none, 0, 0, 0, LeaveaFallThink, NULL, LeaveaFallReact, NULL};
CK_action CK_ACT_LeaveaWalk = {0, 0, AT_UnscaledOnce, false, push_down, 8, 24, 0, LeaveaWalkThink, LeaveaWalkContact, LeaveaWalkReact, &CK_ACT_LeaveaWalk};

/*
==============================================================================
	WARBLE

	user1 - current state of the object
	user2 - hide countdown after getting shot twice/blink count after first hit
	user3 - hits taken so far
	user4 - Nisasi pointer
==============================================================================
*/

enum {WARBLE_RISE, WARBLE_IDLE, WARBLE_SINK, WARBLE_HIDE};

#define WARBLE_RISE_DELAY 8
#define WARBLE_SINK_DELAY 8
#define WARBLE_BLINK_DELAY 10
#define WARBLE_ANIM_DELAY 18

CK_action CK_ACT_Warble;

void CK_SpawnWarble(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);

	ob->type = CT_Warble;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY) - 37*PIXGLOBAL;
	ob->clipped = CLIP_not;
	ob->gfxChunk = WARBLEGROW1SPR;
	ob->user1 = WARBLE_HIDE;
	CK_SetAction(ob, &CK_ACT_Warble);
}

void WarbleThink(CK_object *ob)
{
	int xdist = ob->clipRects.unitXmid - ck_keenObj->clipRects.unitXmid;
	xdist = ABS(xdist);
	switch (ob->user1)
	{
	case WARBLE_HIDE:
		ob->gfxChunk = WARBLEGROW1SPR;
		if (ob->user2)
		{
			if (ob->user2 > tics)
				ob->user2 -= tics;
			else
				ob->user2 = 0;
		}
		else if (xdist < 3*TILEGLOBAL && ob->clipRects.unitY2 >= ck_keenObj->clipRects.unitY1)
		{
			ob->user1 = WARBLE_RISE;
		}
		break;
	case WARBLE_RISE:
		if ((ob->actionTimer += tics) >= WARBLE_RISE_DELAY)
		{
			ob->actionTimer -= WARBLE_RISE_DELAY;
			ob->gfxChunk++;
			if (ob->gfxChunk == WARBLEGROW6SPR)
				ob->user1 = WARBLE_IDLE;
		}
		break;
	case WARBLE_SINK:
		if ((ob->actionTimer += tics) >= WARBLE_SINK_DELAY)
		{
			ob->actionTimer -= WARBLE_SINK_DELAY;
			ob->gfxChunk--;
			if (ob->gfxChunk == WARBLEGROW1SPR)
			{
				ob->user1 = WARBLE_HIDE;
				ob->user3 = 0;	// hiding "heals" the warble
			}
		}
		break;
	case WARBLE_IDLE:
		if (xdist > 5*TILEGLOBAL)
		{
			CK_ObjDropNisasi(ob);
			ob->user1 = WARBLE_SINK;
			ob->gfxChunk = WARBLEGROW5SPR;
		}
		else
		{
			if (ob->user2)
			{
				if ((ob->actionTimer += tics) >= WARBLE_BLINK_DELAY)
				{
					ob->actionTimer -= WARBLE_BLINK_DELAY;
					ob->gfxChunk = (ob->user2 & 1)? WARBLEGROW13SPR : WARBLEGROW10SPR;
					ob->user2--;
				}
			}
			else
			{
				if ((ob->actionTimer += tics) >= WARBLE_ANIM_DELAY)
				{
					ob->actionTimer -= WARBLE_ANIM_DELAY - (US_RndT() & 7);	// animation delay is sligtly random
					xdist = (ob->gfxChunk - WARBLEGROW6SPR) & 3;	// we're just re-using the variable here
					ob->gfxChunk = (US_RndT() >> 3) & 3;
					if (ob->gfxChunk == xdist)	// same frame as before?
						ob->gfxChunk ^= 3;	// can't allow that!
					ob->gfxChunk += ob->user3? WARBLEGROW10SPR : WARBLEGROW6SPR;
				}
			}
		}
		break;
	}
}

void WarbleContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		CK_KillKeen();
		break;
	case CT_Stunner:
		CK_ShotHit(hit);
		if (ob->user3++)
		{
			ob->user2 = 3*TickBase;	// stay hidden for 3 seconds
			switch (ob->user1)
			{
			case WARBLE_IDLE:
				ob->gfxChunk = WARBLEGROW5SPR;
				// no break here!
			case WARBLE_RISE:
				ob->actionTimer = 0;
				ob->user1 = WARBLE_SINK;
				CK_ObjDropNisasi(ob);
				break;
			}
		}
		else
		{
			ob->user2 = 8;
		}
		break;
	}
}

CK_action CK_ACT_Warble = {0, 0, AT_Frame, false, push_none, 0, 0, 0, WarbleThink, WarbleContact, CK_BasicDrawFunc1, NULL};

/*
==============================================================================
	STATIC SPRITES

	Just set decoration, these objects don't actually do anything.
==============================================================================
*/

CK_object *CK_SpawnStaticSprite(unsigned tileX, unsigned tileY, graphicnums chunk, int zLayer)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	ob->clipped = CLIP_not;
	ob->zLayer = zLayer;
	ob->gfxChunk = chunk;
	CK_SetAction(ob, &CK_ACT_Bucket);
	CA_MarkGrChunk(chunk);
	return ob;
}

/*
==============================================================================
	EFREE

	Picks Keen up and transports him to otherwise inaccessible locations.

	user1 - state
	user2 - base sprite + animation offset (highest bit is animation offset)
	user3 - blink duration left (relative to animation speed!)
==============================================================================
*/

enum {EFREE_SEARCH, EFREE_DOCK, EFREE_TRANSPORT, EFREE_VANISH};

CK_action CK_ACT_Efree;

void CK_SpawnEfree(unsigned x, unsigned y)
{
	CK_object *ob;

	// don't spawn it if the sprites aren't cached:
	if (!(grneeded[EFREE1SPR] & ca_levelbit))
		return;

	ob = CK_GetNewObj(true);
	ob->posX = x-4*PIXGLOBAL;
	ob->posY = y;
	ob->clipped = CLIP_not;
	ob->user2=ob->gfxChunk = EFREE1SPR;
	ob->type = CT_Efree;
	CK_SetAction(ob, &CK_ACT_Efree);
}

void EfreeThink(CK_object *ob)
{
	switch (ob->user1)
	{
	case EFREE_SEARCH:
		if (ob->clipRects.unitY2 >= ck_keenObj->clipRects.unitY1+4*PIXGLOBAL
			|| ob->posX+4*PIXGLOBAL != ck_keenObj->posX
			|| ck_mapState.nisasi
			|| CK_KeenIsDead())
		{
			ob->user1 = EFREE_VANISH;
		}
		else
		{
			ck_nextY = 8*tics;
		}
		goto animate;
	case EFREE_DOCK:
		ck_nextY = ck_keenObj->posY + 5*PIXGLOBAL - ob->clipRects.unitY2;
		ob->user1++;
		goto animate;
/*
		if ((ob->actionTimer += tics) >= 6)
		{
			ob->actionTimer -= 6;
			if (ob->user2 < 5)
			{
				static unsigned anim[] = {EFREE3SPR, EFREE1SPR, EFREE4SPR, EFREE1SPR, EFREE5SPR};
				ob->gfxChunk = anim[ob->user2++];
			}
			else
			{
				ob->gfxChunk = EFREE1SPR;
				ob->user1++;
			}
		}
		break;
*/
	case EFREE_TRANSPORT:
		// drop Keen when passing through a bock icon:
		{
			unsigned off, w;
			off = mapbwidthtable[ob->clipRects.tileY2]/2 + ob->clipRects.tileX1;
			w = 1 + ob->clipRects.tileX2 - ob->clipRects.tileX1;
			while (w--)
			{
				if (mapsegs[2][off++] == PLATFORMBLOCK)
					goto dropKeen;
			}
		}

		if	(ob->clipRects.tileY1 == 0)	// always drop Keen at the top of the level!
		{
dropKeen:
			// make Keen fall:
			CK_SetAction2(ck_keenObj, &CK_ACT_keenFall1);
			ck_keenObj->clipped = CLIP_normal;
			ck_keenObj->type = CT_Player;
			ck_keenObj->velX=ck_keenObj->velY=0;
			// Efree vanishes:
			ob->user1++;
		}
		else
		{
			// move Keen up:
			CK_PhysUpdateY(ck_keenObj, -8*tics);
			ck_keenObj->visible = true;
		}
		// no break here!
	case EFREE_VANISH:
		ck_nextY = -8*tics;
animate:
		if ((ob->actionTimer += tics) >= 6)
		{
			ob->actionTimer -= 6;
			if (ob->user3)
			{
				if ((ob->user3 -= tics) <= 0)
				{
					if ((ob->user2 & 0x7FFF) == EFREE1SPR)
					{
						ob->user3 = 0;
					}
					else
					{
						//ob->user3 = TickBase/2 + US_RndT()/8;	//wait 0.5 to 1 second before next blink
						ob->user3 = 3;	//don't start the next blink immediately
					}
					ob->user2 &= 0x8000;
					ob->user2 |= EFREE1SPR;
				}
			}
			else
			{
				//random blink animation:
				int rnd = US_RndT();
				if (rnd < 24)
				{
					ob->user2 &= 0x8000;
					ob->user2 |= EFREE3SPR + ((rnd >> 2) & 6);
					ob->user3 = 3;	//blink for 0.3888 seconds
				}
			}
			ob->user2 ^= 0x8000;
			ob->gfxChunk = (ob->user2 & 0x7FFF) + ((ob->user2 & 0x8000)? 1 : 0);
		}
		break;
	}
	if (ob->user1 == EFREE_VANISH && !CK_ObjectVisible(ob))
		CK_RemoveObj(ob);
}

void CK_EfreeGrabKeen(CK_object *ob)
{
	if (ob->user1 == EFREE_SEARCH)
	{
		ck_keenObj->type = CT_Friendly;
		ck_keenObj->clipped = CLIP_not;
		ck_keenObj->gfxChunk = ck_keenObj->user2 = EFREEKEEN1SPR;
		ck_keenObj->user3 = EFREEKEEN2SPR+1;
		CK_SetAction2(ck_keenObj, &CK_act_item);
		ob->user1 = EFREE_DOCK;
	}
}

CK_action CK_ACT_Efree = {0, 0, AT_Frame, false, push_none, 0, 0, 0, EfreeThink, NULL, CK_BasicDrawFunc1, NULL};

/*
==============================================================================
	NISASI/HUGGUG

	Jumps around and slides down on walls. Grabs onto Keen's head when touching
	it, and has to be shaken off. Keen can only walk left/right while Nisasi
	hangs on his head (handled elsewhere).

	user1 - pointer to grabbed object
	user2 - old player direction (for shaking off) / last grabbed object
	user3 - player shake counter / cooldown
	user4 - shake cooldown
==============================================================================
*/

CK_action CK_ACT_Nisasi;

void CK_SpawnNisasi(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY) - 11*PIXGLOBAL;
	ob->type = CT_Nisasi;
	ob->xDirection = (US_RndT() < 0x80)? -1 : 1;
	ob->velY = 16;
	ob->gfxChunk = NISASIJUMPL1SPR + 2 + ob->xDirection*2;
	CK_SetAction(ob, &CK_ACT_Nisasi);
}

void CK_NisasiLetGo(CK_object *ob)
{
	ob->velX = 48*ob->xDirection;
	ob->velY = -24;
	((CK_object*)ob->user1)->user4 = 0;	// remove Nisasi ptr from grabbed object
	ob->user2 = ob->user1;	// last grabbed object = currently grabbed object
	ob->user3 = 20*TickBase;	// don't grab it again for 20 seconds
	ob->actionTimer = ob->user1 = ob->user4 = 0;	// reset other variables, too
	ob->type = CT_NisasiCooldown;	// don't grab anything until Nisasi lands
//	ob->clipped = CLIP_normal;	// we don't use noclip anymore
}

void NisasiThink(CK_object *ob)
{
	if (ob->user1)
	{
		CK_object *grabbed = (CK_object *)ob->user1;
		ob->visible = true;	// always redraw the Nisasi when it grabs onto something
		if (grabbed == ck_keenObj)
		{
			// handle shaking off:
			if (ob->user2 != grabbed->xDirection)
			{
				ob->user2 = grabbed->xDirection;
				if (++ob->user3 > 3)
				{
					// Nisasi is shaken off
				//	ob->clipped = CLIP_normal;	// we don't use noclip anymore
					ob->xDirection = ob->user2;
					ob->velX = 48*ob->xDirection;
					ob->velY = -24;
					ob->actionTimer=ob->user1=ob->user2=ob->user3=ob->user4 = 0;
					ob->type = CT_NisasiCooldown;
					ck_mapState.nisasi = NULL;	// for input stuff
					SD_SetMusicRate(DEFAULT_MUSIC_RATE);
					goto normal;
				}
				else
				{
					ob->user4 = 10;	// restart cooldown
				}
			}
			else if (ob->user3)
			{
				if (ob->user4 < tics)
				{
					ob->user3 = ob->user4 = 0;	// reset shake count and cooldown
				}
				else
				{
					ob->user4 -= tics;
				}
			}
		}
		else
		{
			if ((ob->user3 -= tics) <= 0)
			{
				CK_NisasiLetGo(ob);
				goto normal;
			}
		}
		ck_nextX = grabbed->posX - ob->posX;
		ck_nextY = grabbed->posY - ob->posY;
		return;
	}
	else if (ob->user3)
	{
		if ((ob->user3 -= tics) <= 0)
		{
			ob->user2 = 0;	// stop ignoring the last grabbed object
			ob->user3 = 0;
		}
	}
normal:
	ob->actionTimer += tics;
	if (ob->leftTI || ob->rightTI || ob->topTI)
	{
		ob->gfxChunk = NISASIHANGLSPR + 2 + ob->xDirection*2;
		if (!ob->topTI)
		{
			int wall;
			if (ob->leftTI)
				wall = TI_ForeLeft(CA_TileAtPos(ob->clipRects.tileX2+1, ob->clipRects.tileY2, 1));
			else
				wall = TI_ForeRight(CA_TileAtPos(ob->clipRects.tileX1-1, ob->clipRects.tileY2, 1));

			if (wall)
				wall = !(TI_ForeTop(CA_TileAtPos(ob->clipRects.tileXmid, ob->clipRects.tileY2+1, 1)));

			if (wall && ob->actionTimer < 4*TickBase)
			{
				ck_nextY = tics;	// slide down on the wall
				ck_nextX = -ob->xDirection;	// push against wall, so leftTI/rightTI will be updated correctly
				return;
			}
		}
		// start jumping
		ob->velX = ob->xDirection * (US_RndT()/8 +16);	// random speed of 16 to 48
		ob->velY = -48;
	}

	CK_Fall(ob);
	ck_nextX = ob->velX * tics;

	ob->gfxChunk = NISASIJUMPL1SPR + 2 + ob->xDirection*2;
	if (ob->actionTimer & 0x8)
		ob->gfxChunk++;
}

void NisasiContact(CK_object *ob, CK_object *hit)
{
	if (ob->user1 || (ob->user2 == (int)hit) || ob->type != CT_Nisasi || (hit->user4 && hit->type != CT_Player))
		return;

	switch (hit->type)
	{
	case CT_Player:
		ob->visible = true;	// force redraw
		//TODO: check if Nisasi hit Keen's head
		if (!ck_mapState.nisasi && !ck_mapState.invincibilityTimer && !CK_KeenIsDead())
		{
			if (hit->gfxChunk == KEENSLEEP3SPR || hit->gfxChunk == KEENSLEEP4SPR)
			{
				//CK_RemoveObj(ob);
				ob->gfxChunk = YETICLOUD3SPR;
				ob->clipped = CLIP_not;
				ob->type = CT_Friendly;
				ob->user1=ob->velX=0;
				CK_SetAction2(ob, &CK_ACT_YetiCloud);

				// find the current dream bubble
				for (ob=ck_keenObj->next; ob; ob=ob->next)
				{
					if (ob->type == CT_DreamBubble)
					{
						ob->user4 = -1;	// force bubble to show the Nisasi sprite next
						ob->user1=ob->user2=0;	// force bubble to update the sprite
						break;
					}
				}
			}
			else
			{
				ob->user1 = (int)hit;
				ob->user2 = hit->xDirection;
				ob->user3 = ob->user4 = 0;	// reset shake count and cooldown
			//	ob->clipped = CLIP_not;	// we don't actually need to noclip
				ob->timeUntillThink = 0;
				ck_mapState.nisasi = ob;	// for input stuff
				hit->clipped = CLIP_normal;	// just in case
				// Keen might have been climbing or pogoing or whatever, so change the state
				if (hit->topTI)
				{
					CK_SetAction2(hit, &CK_ACT_keenStanding);
				}
				else
				{
					CK_SetAction2(hit, &CK_ACT_keenFall1);
				}
			}
		}
		return;
	case CT_CouncilMember:
	case CT_CouncilMemberDone:
		break;
		
#ifdef EXTRA_INTERACTIONS
	case CT_Crocodent:
		if (hit->currentAction != &CK_ACT_CrocWalk)
			return;
		break;
	case CT_Chemolain:
		if (hit->gfxChunk > CHEMCYCLE6SPR || hit->gfxChunk < CHEMLOOK1SPR)
			return;
		hit->user2 = 12;
		CK_SetAction2(hit, &CK_ACT_ChemCycle);
		break;
	case CT_Egg:
	case CT_BirdEgg:
		if (hit->gfxChunk < EGG1SPR || hit->gfxChunk > EGG3SPR)
			return;
		break;
	case CT_SneakyTree:
		if (hit->user1 != ST_LOOK)
		{
			hit->velX = hit->actionTimer = 0;	// stop moving
			hit->user1 = ST_RETRACT;
		}
		if (hit == ck_keenState.platform)
			ck_keenState.platform = NULL;
		break;
	case CT_Kitten:
		if (hit->currentAction->think != &KittenIdleThink)
			return;
		break;
	case CT_Poleknawel:
		if (hit->gfxChunk < POLEKNAWELFALLSPR || hit->gfxChunk > POLEKNAWELPOOPSPR)
		{
#if 1
			// Nisasi CAN grab Poleknawels while they're on a pole, making them fall
			hit->clipped = CLIP_normal;
			hit->timeUntillThink = 0;
			CK_SetAction2(hit, &CK_ACT_PoleknawelFall);
#else
			// Nisasi can NOT grab Poleknawels while they're on a pole
			return;
#endif
		}
		if (hit->gfxChunk != POLEKNAWELFALLSPR)
			CK_SetAction2(hit, &CK_ACT_PoleknawelLand);	// make polwknawel stop moving/throwing
		break;
#endif 

	case CT_Slug:
		if (hit->currentAction == &CK_ACT_SlugSliming0)
			CK_SetAction2(hit, &CK_ACT_SlugMove0);	// abort sliming when hit by Nisasi
		break;
	case CT_XkyLick:
		if ((hit->gfxChunk >= XKYLICKATTACKR1SPR && hit->gfxChunk <= XKYLICKATTACKL3SPR))
			CK_SetAction2(hit, &CK_ACT_XkyLickHop2);	// abort licking when hit by Nisasi
		break;
	case CT_Warble:
		if (hit->user1 != WARBLE_IDLE)
			return;
		hit->user2 += 8;	// make it blink 8 times
		hit->user3 = 1;		// second phase (next stunner hit makes it hide)
		break;
	case CT_CoronaPlant:
		hit->actionTimer = 0;
		if (hit->user1)
		{
			// close plant's mouth:
			hit->user1 = 0;
			hit->gfxChunk--;
			hit->visible = true;
		}
		break;
	default:
		return;
	}
	// common code to make Nisasi grab onto the hit object:
	hit->user4 = (int)ob;
	ob->visible = true;
	ob->user1 = (int)hit;
	ob->user3 = 4*TickBase;	// let go after 4 seconds
//	ob->clipped = CLIP_not;	// we don't actually need to noclip
	ob->timeUntillThink = 0;
}

void NisasiReact(CK_object *ob)
{
	if (ob->user1)
	{
		unsigned x, y, g;
		void *spriteptr = &ob->sde;

		ob = (CK_object *)ob->user1;
		x = ob->posX;
		y = ob->posY;
		g = ob->xDirection < 0? NISASIGRABLSPR : NISASIGRABRSPR;

		switch (ob->type)
		{
#ifdef EXTRA_INTERACTIONS
		case CT_Chemolain:
			x += 12*PIXGLOBAL;
			y += 1*PIXGLOBAL;
			g = NISASIGRABRSPR;
			break;
		case CT_Crocodent:
			x += ob->xDirection < 0? -11*PIXGLOBAL : 11*PIXGLOBAL;
			y += -15*PIXGLOBAL;
			break;
		case CT_SneakyTree:
			x += 14*PIXGLOBAL;
			y += -15*PIXGLOBAL;
			g = NISASIGRABRSPR;
			break;
		case CT_Egg:
		case CT_BirdEgg:
			x += 8*PIXGLOBAL;
			y += (-6-(ob->gfxChunk - EGG1SPR))*PIXGLOBAL;
			g = NISASIGRABRSPR;
			break;
		case CT_Poleknawel:
			x += 2*PIXGLOBAL;
			y += -4*PIXGLOBAL;
			g = NISASIGRABRSPR;
			break;
		case CT_Kitten:
			switch (ob->gfxChunk)
			{
			case KITTENSTANDSPR:
				x += -27*PIXGLOBAL;
				y += -14*PIXGLOBAL;
				g = NISASIGRABRSPR;
				break;
			case KITTENMEOW1SPR:
				x += -34*PIXGLOBAL;
				y += -7*PIXGLOBAL;
				g = NISASIGRABRSPR;
				break;
			case KITTENMEOW2SPR:
				x += -42*PIXGLOBAL;
				y += -18*PIXGLOBAL;
				g = NISASIGRABLSPR;
				break;
			case KITTENMEOW3SPR:
				x += -36*PIXGLOBAL;
				y += -16*PIXGLOBAL;
				g = NISASIGRABLSPR;
				break;
			}
			break;
#endif
		case CT_CouncilMember:
		case CT_CouncilMemberDone:
			switch (ob->gfxChunk)
			{
			case COUNCILWALKR1SPR:
				y += -12*PIXGLOBAL;
				x += -1*PIXGLOBAL;
				break;
			case COUNCILWALKR2SPR:
				y += -12*PIXGLOBAL;
				x += 1*PIXGLOBAL;
				break;
			case COUNCILSTANDRSPR:
				y += -11*PIXGLOBAL;
			//	x += 0*PIXGLOBAL;
				g = NISASIGRABRSPR;	// because xDirection is swapped while turning around
				break;
			case COUNCILMOPR1SPR:
				y += -11*PIXGLOBAL;
				x += 1*PIXGLOBAL;
				break;
			case COUNCILMOPR2SPR:
				y += -12*PIXGLOBAL;
				x += 1*PIXGLOBAL;
				break;
			case COUNCILWALKL1SPR:
				y += -12*PIXGLOBAL;
				x += -8*PIXGLOBAL;
				break;
			case COUNCILWALKL2SPR:
				y += -12*PIXGLOBAL;
				x += -9*PIXGLOBAL;
				break;
			case COUNCILSTANDLSPR:
				y += -11*PIXGLOBAL;
				x += -8*PIXGLOBAL;
				g = NISASIGRABLSPR;	// because xDirection is swapped while turning around
				break;
			case COUNCILMOPL1SPR:
				y += -11*PIXGLOBAL;
				x += -7*PIXGLOBAL;
				break;
			case COUNCILMOPL2SPR:
				y += -12*PIXGLOBAL;
				x += -8*PIXGLOBAL;
				break;
			}
			break;
		case CT_Slug:
			switch (ob->gfxChunk)
			{
			case SLUGWALKR1SPR:
				x += -4*PIXGLOBAL;
				y += -9*PIXGLOBAL;
				break;
			case SLUGWALKR2SPR:
				x += -6*PIXGLOBAL;
				y += -8*PIXGLOBAL;
				break;
			case SLUGWALKL1SPR:
				x += -12*PIXGLOBAL;
				y += -9*PIXGLOBAL;
				break;
			case SLUGWALKL2SPR:
				x += -10*PIXGLOBAL;
				y += -8*PIXGLOBAL;
				break;
			}
			break;
		case CT_XkyLick:
			g = ob->xDirection < 0? NISASIGRABRSPR : NISASIGRABLSPR;	// opposite of regular mapping!
			switch (ob->gfxChunk)
			{
			case XKYLICKR1SPR:
				x += -9*PIXGLOBAL;
				y += -24*PIXGLOBAL;
				break;
			case XKYLICKR2SPR:
				x += -9*PIXGLOBAL;
				y += -26*PIXGLOBAL;
				g = NISASIGRABLSPR;	// because xDirection is ignored in search animation
				break;
			case XKYLICKR3SPR:
				x += -6*PIXGLOBAL;
				y += -25*PIXGLOBAL;
				break;
			case XKYLICKR4SPR:
				x += -6*PIXGLOBAL;
				y += -22*PIXGLOBAL;
				break;
			case XKYLICKL1SPR:
			//	x += 0*PIXGLOBAL;
				y += -23*PIXGLOBAL;
				g = NISASIGRABRSPR;	// because xDirection is ignored in search animation
				break;
			case XKYLICKL2SPR:
			//	x += 0*PIXGLOBAL;
				y += -25*PIXGLOBAL;
				break;
			case XKYLICKL3SPR:
				x += -1*PIXGLOBAL;
				y += -24*PIXGLOBAL;
				break;
			case XKYLICKL4SPR:
				x += -1*PIXGLOBAL;
				y += -21*PIXGLOBAL;
				break;
			}
			break;
		case CT_Player:
			switch (ob->gfxChunk)
			{
			case KEENSTANDRSPR:
				y -= 15*PIXGLOBAL;
			//	x += 0*PIXGLOBAL;
				break;
			case KEENRUNR1SPR:
				y -= 14*PIXGLOBAL;
				x += -1*PIXGLOBAL;
				break;
			case KEENRUNR2SPR:
				y -= 12*PIXGLOBAL;
				x += 0*PIXGLOBAL;
				break;
			case KEENRUNR3SPR:
				y -= 14*PIXGLOBAL;
				x -= 1*PIXGLOBAL;
				break;
			case KEENRUNR4SPR:
				y -= 13*PIXGLOBAL;
			//	x += 0*PIXGLOBAL;
				break;
			case KEENJUMPR2SPR:
				y -= 12*PIXGLOBAL;
				x -= 1*PIXGLOBAL;
				break;
			case KEENJUMPR3SPR:
				y -= 12*PIXGLOBAL;
			//	x += 0*PIXGLOBAL;
				break;
			case KEENSTANDLSPR:
				y -= 15*PIXGLOBAL;
				x -= 7*PIXGLOBAL;
				break;
			case KEENRUNL1SPR:
				y -= 14*PIXGLOBAL;
				x -= 8*PIXGLOBAL;
				break;
			case KEENRUNL2SPR:
				y -= 12*PIXGLOBAL;
				x -= 8*PIXGLOBAL;
				break;
			case KEENRUNL3SPR:
				y -= 14*PIXGLOBAL;
				x -= 7*PIXGLOBAL;
				break;
			case KEENRUNL4SPR:
				y -= 13*PIXGLOBAL;
				x -= 8*PIXGLOBAL;
				break;
			case KEENJUMPL2SPR:
				y -= 12*PIXGLOBAL;
				x -= 8*PIXGLOBAL;
				break;
			case KEENJUMPL3SPR:
				y -= 12*PIXGLOBAL;
				x -= 9*PIXGLOBAL;
				break;
			}
			break;
		case CT_Warble:
			g = NISASIGRABLSPR;
			switch (ob->gfxChunk)
			{
			case WARBLEGROW6SPR:
			case WARBLEGROW7SPR:
			case WARBLEGROW8SPR:
			case WARBLEGROW9SPR:
				x += -8*PIXGLOBAL;
				y += -11*PIXGLOBAL;
				break;
			case WARBLEGROW10SPR:
			case WARBLEGROW11SPR:
			case WARBLEGROW12SPR:
			case WARBLEGROW13SPR:
				x += -9*PIXGLOBAL;
				y += -13*PIXGLOBAL;
				break;
			}
			break;
		case CT_CoronaPlant:
			switch (ob->gfxChunk)
			{
			case CORONAL1SPR:
				x += -11*PIXGLOBAL;
				y += -12*PIXGLOBAL;
				g = NISASIGRABLSPR;
				break;
			case CORONAU1SPR:
				x += -2*PIXGLOBAL;
				y += -16*PIXGLOBAL;
				g = NISASIGRABRSPR;
				break;
			case CORONAR1SPR:
				x += 1*PIXGLOBAL;
				y += -8*PIXGLOBAL;
				g = NISASIGRABRSPR;
				break;
			}
		}
		RF_PlaceSpriteAfter(spriteptr, x, y, g, spritedraw, ob->zLayer, &ob->sde);
		return;
	}
	if (ob->bottomTI)
		ob->velY = 0;
	if (ob->velX && (ob->leftTI || ob->rightTI || ob->topTI))
	{
		ob->type = CT_Nisasi;
		ob->velX = 0;
		ob->actionTimer = US_RndT();	// sliding down on the walls is also random
		ob->timeUntillThink = 21;
		if (ob->topTI)
		{
			ob->xDirection = (ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid)? -1 : 1;
			ob->gfxChunk = NISASIFLOORSPR;
		}
		else
		{
			ob->xDirection = -ob->xDirection;
			ob->gfxChunk = NISASIHANGLSPR + 2 + ob->xDirection*2;
		}
	}
	CK_BasicDrawFunc1(ob);
}

void CK_DropNisasi(void)	// for Keen
{
	register CK_object *ob;
	if (ck_mapState.nisasi)
	{
		ob = ck_mapState.nisasi;
		ck_mapState.nisasi = NULL;
	//	ob->clipped = CLIP_normal;	// we don't use noclip anymore
		ob->xDirection = ob->user2;
		ob->velX = ob->velY = 1;
		ob->actionTimer=ob->user1=ob->user2=ob->user3=ob->user4 = 0;
		ob->type = CT_NisasiCooldown;
		SD_SetMusicRate(DEFAULT_MUSIC_RATE);
	}
}

void CK_ObjDropNisasi(CK_object *ob)	// for other objects
{
	// Make Nisasi let go (if creature is grabbed by a Nisasi
	if (ob->user4 && ob->user4 != -1)
	{
		CK_object *n = (CK_object *)ob->user4;
		if (n->type == CT_Nisasi && n->user1 == (int)ob)
		{
			CK_NisasiLetGo(n);
		}
	}
}

CK_action CK_ACT_Nisasi = {0, 0, AT_Frame, false, push_none, 0, 0, 0, NisasiThink, NisasiContact, NisasiReact, NULL};

/*
==============================================================================
	COUNCIL PAGE

	Stands in place, waiting to get mind-controlled by the player. Can only
	walk left/right and use switches. No jumping/falling or shooting.

	user1 - idle counter (when controlled by player), destX for sliding
	user2 - internal state (see enum)
	user3 - tileX for switches
	user4 - tileY for switches
==============================================================================
*/

enum {
	PG_INACTIVE, PG_NORMAL, PG_DREAM, PG_STOPDREAM,
	PG_USE, PG_AFTERUSE, PG_ACTIVATE, PG_INACTIVATE,
	PG_ENTERMACHINE, PG_LEAVEMACHINE, PG_LEFTMACHINE
};

#define PAGE_DREAMDELAY (10*TickBase)
#define PAGE_WALKSPEED	(12)	// half of Keen's walking speed
#define PAGE_SLIDESPEED	(6)

extern CK_action CK_ACT_PageWalk;

CK_action CK_ACT_Page;

void CK_SpawnCouncilPage(int tileX, int tileY, int type)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->posX = RF_TileToUnit(tileX) + 8*PIXGLOBAL;
	ob->posY = RF_TileToUnit(tileY) - 15*PIXGLOBAL;
	ob->type = CT_Page;
	ob->xDirection = motion_Right;
	ob->gfxChunk = PAGESLEEPRSPR;
//	ob->user2 = PG_INACTIVE;	// redundant (PG_INACTIVE is 0 and user2 is already set to 0)
	CK_SetAction(ob, type? &CK_ACT_PageWalk : &CK_ACT_Page);
}

void PageThink(CK_object *ob)
{
	switch (ob->user2)
	{
	case PG_ACTIVATE:
		if (ob->next->gfxChunk == 0)	// is door open?
			ob->user2 = PG_LEAVEMACHINE;
		// no break here!
	case PG_INACTIVE:
inactive:
		if ((ob->actionTimer += tics) >= 8)
		{
			ob->actionTimer -= 8;
			ob->gfxChunk++;
		}
		if (ob->gfxChunk > PAGESTAND3SPR || ob->gfxChunk < PAGESTAND1SPR)
			ob->gfxChunk = PAGESTAND1SPR;
		break;
	case PG_INACTIVATE:
		if (ob->next->gfxChunk == SLIDEDOORRED8SPR)	// door is closed?
		{
			// wait 1/4 second before switching to Keen
			if ((ob->user1 += tics) >= TickBase/4)
			{
				// make Keen controllable again:
				ck_keenObj->user1 = 1;	// make Keen wait until door is open
				ck_keenObj->gfxChunk = KEENSTANDSPR;
			//	if (!CK_ObjectVisible(ck_keenObj))
				//	CK_CentreCamera(ck_keenObj);
				
				// make Page inactive:
				ob->user2 = PG_INACTIVE;
				ck_mapState.mindcontrolled = NULL;

				// open Keen's door:
				for (ob=ck_keenObj; ob; ob=ob->next)
				{
					if (ob->type == CT_SlideDoor && ob->user2 == SLIDEDOORBLUE1SPR)
					{
						ob->user1 = 0;
						ob->actionTimer = -TickBase/4;	// wait 1/4 second before opening the door
					}
				}
				return;	// because ob is no longer valid
			}
		}
		goto inactive;
	case PG_ENTERMACHINE:
		ck_nextY = -(tics*4);
		if ((ob->actionTimer += tics*4) >= (TILEGLOBAL/4))
		{
			ob->actionTimer -= (TILEGLOBAL/4);
			if (++ob->user1 == 4)
			{
				// change Page state:
				ck_nextY += ob->actionTimer;
				ob->user1 = ob->user3 = 0;
				ob->user2 = PG_INACTIVATE;

				// close Page's door:
				ob = ob->next;
				ob->user1 = 1;
				return;	// because ob is no longer valid
			}
		}
		ob->gfxChunk = (ob->user1 & 1)? PAGESWITCH1SPR : PAGESWITCH2SPR;
		break;
	case PG_LEAVEMACHINE:
		CK_PhysGravityMid(ob);
		ob->gfxChunk = PAGEDREAM1SPR;
		if (ob->topTI)
		{
			// change Page state:
			ob->velY = 0;
			ob->user2 = PG_LEFTMACHINE;
			ob->gfxChunk = PAGESTAND1SPR;
		}
		break;
	case PG_LEFTMACHINE:
		ob->user1 += tics;
		if (ck_inputFrame.dir == dir_None && ob->user1 < TickBase)
		{
			ck_nextY = 16;	// push down
			goto inactive;
		}
		goto benormal;
	case PG_USE:
		ob->gfxChunk = PAGESWITCH1SPR;
		// Is the door still aktive?
		if (ob->clipRects.unitX1 == ob->user1 && !CK_DoorsActive())
		{
			if ((TI_ForeMisc(CA_TileAtPos(ob->user3, ob->user4, 1)) & 0x7F) == MF_MindMachine)
			{
				// change Page state:
				ob->user2 = PG_ENTERMACHINE;
				ob->user1 = ob->user3 = ob->user4 = ob->actionTimer = 0;	// reset counters
			}
			else if ((ob->actionTimer += tics) >= 8)	// short delay before actually using the switch
			{
				CK_ToggleSwitch(ob->user3, ob->user4);
				ob->actionTimer=ob->user1=ob->user3=ob->user4 = 0;
				ob->user2 = PG_AFTERUSE;
			}
		}
		else
		{
			// slide to the middle of the switch tile:
			register int maxspeed = tics*PAGE_SLIDESPEED;
			ck_nextX = ob->user1-ob->clipRects.unitX1;
			if (ck_nextX < -maxspeed)
			{
				ck_nextX = -maxspeed;
			}
			else if (ck_nextX > maxspeed)
			{
				ck_nextX = maxspeed;
			}
			ck_nextY = abs(ck_nextX) + 16;	// manually push to floor
		}
		break;
	case PG_AFTERUSE:
		ob->gfxChunk = PAGESWITCH2SPR;
		if ((ob->actionTimer += tics) >= 8)
		{
			ob->actionTimer = 0;
			ob->user2 = PG_NORMAL;
		}
		break;
	case PG_DREAM:
		if ((ob->actionTimer += tics) >= 15)
		{
			ob->actionTimer -= 15;
			if (ob->gfxChunk == PAGEDREAM1SPR)
				ob->gfxChunk++;
			else
				ob->gfxChunk = PAGEDREAM1SPR;
		}
		ob->user1 += tics;
		if (ck_inputFrame.dir != dir_None || ob->user1 > 15*TickBase)
		{
			ob->actionTimer = 0;
			ob->user2 = PG_STOPDREAM;
			ob->gfxChunk = PAGESTAND1SPR;
			RemoveDreamBubbles();
		}
		break;
	case PG_STOPDREAM:
		if ((ob->actionTimer += tics) < 15)
			break;
benormal:
		ob->actionTimer=ob->user1 = 0;
		ob->user2 = PG_NORMAL;
		// no break here!
	default:
		if (ck_inputFrame.yaxis == -1 && !ck_keenState.keenSliding)
		{
			unsigned h, x, y, i;
			x = ob->clipRects.tileXmid;
			y = ob->clipRects.tileY1;
			i = x + mapbwidthtable[y]/2;
			h = ob->clipRects.tileY2 - y+1;
			while (h--)
			{
				switch (TI_ForeMisc(mapsegs[1][i]) & 0x7F)
				{
				case MF_SwitchOn:
				case MF_SwitchOff:
				case MF_BridgeSwitch:
					// set target x position:
					ob->user1 = RF_TileToUnit(x);
startuse:
					// save x/y coordinates of the switch tile:
					ob->user3 = x;
					ob->user4 = y;
					// start using the switch/button/whatever:
					ob->user2 = PG_USE;
					ob->actionTimer = 0;
					ob->gfxChunk = PAGESWITCH1SPR;
					ck_keenState.keenSliding = true;
					return;
				case MF_MindMachine:
					ob->user1 = RF_TileToUnit(x) - 8*PIXGLOBAL;
					if ((TI_ForeMisc(mapsegs[1][i+1]) & 0x7F) == MF_MindMachine)
						ob->user1 += TILEGLOBAL;
					goto startuse;
				}
				i += mapwidth;
				y++;
			}
		}
		if (ck_inputFrame.xaxis)
		{
			ob->xDirection = ck_inputFrame.xaxis;
			ck_nextX = ob->xDirection*tics*PAGE_WALKSPEED;
			ob->gfxChunk = ob->xDirection +1+PAGEWALKLSPR;
			ob->user1 = 0;	// reset idle counter
		}
		else
		{
			ob->gfxChunk = ob->xDirection +1+ PAGESLEEPLSPR;
			if ((ob->user1 += tics) >= PAGE_DREAMDELAY)
			{
				// start idle/dream animation
				ob->user2 = PG_DREAM;	
				ob->gfxChunk = PAGESTAND1SPR;
				SpawnDreamBubble(ob->posX+4*PIXGLOBAL, ob->posY-30*PIXGLOBAL, 1);
				ob->user1 = 0;
			}
		}
		ck_nextY = abs(ck_nextX) + 16;	// manually push to floor
	}
}

void PageReact(CK_object *ob)
{
	unsigned y;

	// don't fall:
	if (!ob->topTI && (ob->user2 == PG_NORMAL || ob->user2 == PG_USE || ob->user2 == PG_LEFTMACHINE))
	{
		CK_PhysUpdateX(ob, -ob->deltaPosX);
		CK_PhysUpdateY(ob, -ob->deltaPosY);
	}

	y = ob->posY;
	if (ob->user2 == PG_NORMAL)
	{
		y -= ((ob->posX-1) & (PIXGLOBAL << 3)) >> 3;	// adds up & down movement while walking
	}
	RF_PlaceSprite(&ob->sde, ob->posX, y, ob->gfxChunk, spritedraw, ob->zLayer);
}

CK_action CK_ACT_Page = {0, 0, AT_Frame, false, push_none, 0, 0, 0, PageThink, NULL, PageReact, NULL};

CK_object *CK_FindPageObj(unsigned infoval)
{
	CK_object *ob;
	unsigned x, y;
	
	y = infoval & 0xFF;
	x = infoval >> 8;

	for (ob=ck_keenObj; ob; ob=ob->next)
	{
		if (ob->type == CT_Page && ob->clipRects.tileX1 == x && ob->clipRects.tileY2 == y)
		{
			return ob;
		}
	}
	return NULL;
}

void CK_ControlPage(CK_object *ob)
{
	// enable Page:
	ob->user2 = PG_ACTIVATE;
	ob->user1 = 0;
	ck_mapState.mindcontrolled = ob;
//	if (!CK_ObjectVisible(ob))
	//	CK_CentreCamera(ob);

	// open Page's door:
	ob = ob->next;	// door object ALWAYS follows directly after Page object
	ob->user1 = 0;
	ob->actionTimer = -TickBase/4;	// wait 1/4 second before opening
}

void KeenEnterMachineThink(CK_object *ob)
{
	if (++ob->user1 == 5)
	{
		ob->user2 = (int)CK_SpawnSlideDoor(ob->clipRects.tileX1, ob->clipRects.tileY2, 1);
		ob->user1 = 0;
		ob->gfxChunk = KEENSTANDLSPR;
		ob->timeUntillThink = 8;
		ob->currentAction = &CK_ACT_keenMindcontrol;
		ob->type = CT_Friendly;	// enemies won't interact with Keen
	}
	else
	{
		ck_nextY = -4*PIXGLOBAL;
		ob->gfxChunk = (ob->user1 & 1)? KEENWALKAWAY1SPR : KEENWALKAWAY2SPR;
		SD_PlaySound(KEENWALK1SND + (ob->user1 & 1));
	}
}

CK_action CK_ACT_keenEnterMachine = {0, 0, AT_UnscaledOnce, false, push_none, 9, 0, 0, KeenEnterMachineThink, NULL, CK_BasicDrawFunc1, &CK_ACT_keenEnterMachine};

void MindcontrolThink(CK_object *ob)
{
	if (ob->user1)
	{
		ob->gfxChunk = KEENSTANDSPR;
		if (((CK_object *)ob->user2)->gfxChunk == 0)	// is door open?
		{
			CK_RemoveObj((CK_object *)ob->user2);	// remove Keen's door object
			ob->user1=ob->user2=ob->user3=ob->user4 = 0;
			ob->currentAction = &CK_ACT_keenStanding;
			ob->type = CT_Player;	// enemies will interact with Keen again
		}
	}
	else if (ob->user3)
	{
		ob->gfxChunk = KEENSTANDSPR;
		if ( ((CK_object *)ob->user2)->gfxChunk == SLIDEDOORBLUE8SPR )
		{
			// Keen's door is closed, control the Page after 1/4 second!
			if ((ob->actionTimer += tics) >= TickBase/4)
			{
				CK_ControlPage((CK_object *)ob->user3);
				ob->user3 = 0;
			}
		}
		else
		{
			ob->actionTimer = 0;
		}
	}
	else
	{
		switch (ck_inputFrame.xaxis)
		{
		case motion_Left:
			ob->gfxChunk = KEENSTANDLSPR;
			break;
		case motion_Right:
			ob->gfxChunk = KEENSTANDRSPR;
			break;
		default:
			ob->gfxChunk = KEENSTANDSPR;
		}
	}
}

CK_action CK_ACT_keenMindcontrol = {0, 0, AT_Frame, false, push_down, 0, 0, 0, MindcontrolThink, NULL, CK_BasicDrawFunc1, NULL};

/*
==============================================================================
	SLIDING DOOR

	Just eye candy. Opens or closes when a mindcontrol machine is used.

	user1 - direction (0 = opening, 1 = closing)
	user2 - base sprite number
==============================================================================
*/

#define DOOR_ANIM_DELAY 4

extern CK_action CK_ACT_SlideDoor;

CK_object *CK_SpawnSlideDoor(int tileX, int tileY, int type)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->type = CT_SlideDoor;
	ob->clipped = CLIP_not;
	ob->zLayer = 3;
	ob->active = OBJ_ALWAYS_ACTIVE;
	ob->posX = RF_TileToUnit(tileX) + 8*PIXGLOBAL;;
	ob->posY = RF_TileToUnit(tileY) - 18*PIXGLOBAL;
	switch (type)
	{
	case 0:
		ob->user2 = SLIDEDOORRED1SPR;
		ob->gfxChunk = SLIDEDOORRED8SPR;
		break;
	case 1:
		ob->user2 = SLIDEDOORBLUE1SPR;
		ob->gfxChunk = 0;
		break;
	}
	ob->user1 = 1;
	CK_SetAction(ob, &CK_ACT_SlideDoor);
	return ob;
}

void SlideDoorThink(CK_object *ob)
{
	if (ob->user1)
	{
		// door is closing
		if (ob->gfxChunk != ob->user2+7)
		{
			// closing animation is not done yet
			if ((ob->actionTimer += tics) >= DOOR_ANIM_DELAY)
			{
				ob->actionTimer -= DOOR_ANIM_DELAY;
				if (!ob->gfxChunk)
				{
					ob->gfxChunk = ob->user2;
				}
				else
				{
					ob->gfxChunk++;
				}
			}
		}
		else
		{
			ob->actionTimer = 0;
		}
	}
	else
	{
		// door is opening
		if (ob->gfxChunk && CK_ObjectVisible(ob))
		{
			// opening animation is not done yet
			if ((ob->actionTimer += tics) >= DOOR_ANIM_DELAY)
			{
				ob->actionTimer -= DOOR_ANIM_DELAY;
				if (ob->gfxChunk == ob->user2)
				{
					ob->gfxChunk = 0;
				}
				else
				{
					ob->gfxChunk--;
				}
			}
		}
		else
		{
			ob->actionTimer = 0;
		}
	}
}

CK_action CK_ACT_SlideDoor = {0, 0, AT_Frame, false, push_none, 0, 0, 0, SlideDoorThink, NULL, CK_BasicDrawFunc1, NULL};

/*
==============================================================================
	CLONE MACHINE

	Creates dummy clone objects

	user1 - machine type (janitor, poleknawel ...)
	user2 - janitor spawn counter
	user3 - spawn countdown
	user4 - combined x/y position
------------------------------------------------------------------------------
	CLONE OBJECT

	user1 - clone type; no animation or movement while high bit is set
	user2 - clipping value
------------------------------------------------------------------------------
	SCANNING/CLONING ANIMATION

	user1 - pointer to scanned/cloned object
	user2 - movement counter
	user3 - animation counter
==============================================================================
*/

enum {CLONE_POLEKNAWEL, CLONE_MOLET, CLONE_CHEMOLAIN, CLONE_NISASI, CLONE_JANITOR, NUMCLONETYPES};

CK_action CK_ACT_CloneMachine;
CK_action CK_ACT_CloneAnimation;
CK_action CK_ACT_CloneObj;

#define SCAN_HEIGHT 42	// how many pixels the scan animation moves up or down
#define SCAN_WIDTH 32	// width of the scan animation sprite, in pixels
#define SCAN_YOFF -7
#define SCAN_DIR motion_Down	// should be either motion_Up or motion_Down
#define SCAN_ANIM_DELAY 4	// delay for sprite change in tics (lower numbers -> faster animation)
#define SCAN_MOVE_DELAY 2	// delay for movement (up/down)
#define SCAN_INTERVAL (SCAN_HEIGHT*SCAN_MOVE_DELAY + 2*SCAN_WIDTH)	// time between the start of a scan and the start of the next scan

// please don't modify this:
#if (SCAN_DIR == motion_Up)
#define SCAN_START_Y (SCAN_YOFF*PIXGLOBAL)
#elif (SCAN_DIR == motion_Down)
#define SCAN_START_Y ((SCAN_YOFF-SCAN_HEIGHT)*PIXGLOBAL)
#else
#error bad SCAN_DIR
#endif

static graphicnums cloneBaseSprite[NUMCLONETYPES] = {POLEKNAWELWALK1SPR, MOLETWALKR1SPR, CHEMLOOK1SPR, NISASIFLOORSPR, COUNCILSTANDRSPR};

void CK_SpawnCloneMachine(int tileX, int tileY, int type, int dir)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->type = CT_CloneMachineOff;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY+1);
	ob->user1 = type;
	ob->user4 = tileX | (tileY << 8);
	ob->active = OBJ_ALWAYS_ACTIVE;
	ob->clipped = CLIP_not;
	ob->gfxChunk = cloneBaseSprite[type];
	ob->posY -= spritetable[ob->gfxChunk - STARTSPRITES].yh + 1;	// + 1 so it won't clip into the floor
	ob->xDirection = dir;
	CK_SetAction(ob, &CK_ACT_CloneMachine);

	if (type == CLONE_JANITOR)
		ob->gfxChunk = 0;
}

void CloneObjAnimate(CK_object *ob)
{
	ob->actionTimer += tics;
	switch (ob->user1)
	{
	case CLONE_POLEKNAWEL:
		if (ob->type == CT_CloneRising)
		{
			ob->gfxChunk = POLEKNAWELPOOPSPR;
			break;
		}
		while (ob->actionTimer >= 10)
		{
			ob->actionTimer -= 10;
			if (++ob->gfxChunk > POLEKNAWELWALK2SPR)
				ob->gfxChunk = POLEKNAWELWALK1SPR;
		}
		break;
	case CLONE_MOLET:
		while (ob->actionTimer >= 10)
		{
			ob->actionTimer -= 10;
			if (++ob->gfxChunk > MOLETWALKR4SPR)
				ob->gfxChunk = MOLETWALKR1SPR;
		}
		break;
	case CLONE_CHEMOLAIN:
		if (ob->gfxChunk == CHEMLOOK1SPR)
		{
			if (ob->actionTimer > 180)
			{
				ob->actionTimer = 0;
				ob->gfxChunk = CHEMLICK1SPR;
			}
		}
		else
		{
			while (ob->actionTimer >= 10)
			{
				ob->actionTimer -= 10;
				if (++ob->gfxChunk > CHEMLICK2SPR)
				{
					ob->actionTimer = (US_RndT() >> 2);
					ob->gfxChunk = CHEMLOOK1SPR;
					break;
				}
			}
		}
		break;
	case CLONE_NISASI:
		while (ob->actionTimer >= 30)
		{
			ob->actionTimer -= 54;
			ob->gfxChunk ^= (NISASIFLOORSPR^NISASIFLOOR2SPR);
		}
		break;
	case CLONE_JANITOR:
		if (!ob->gfxChunk)
		{
			ob->actionTimer = 0;
			break;
		}
		if (ob->actionTimer >= 4*TickBase)
		{
			ob->actionTimer = US_RndT();
			if (ob->gfxChunk == COUNCILSTANDRSPR)
			{
				ob->gfxChunk = COUNCILWALKR1SPR;
			}
			else
			{
				ob->gfxChunk = COUNCILSTANDRSPR;
			}
		}
		break;
	}
}

void CloneMachineThink(CK_object *ob)
{
	CK_object *scan, *clone;

	if (!(ob->user1 & 0x8000))
	{
		CloneObjAnimate(ob);
	}

	if (ob->type == CT_CloneMachineOn)
	{
		if ((ob->user3 += tics) >= SCAN_INTERVAL && !(ob->user1 & 0x8000))
		{
			ob->user3 = 0;	// restart countdown

			//
			// start the scanning animation
			//

			clone = CK_GetNewObj(true);
			clone->zLayer = FOREGROUNDPRIORITY-1;
			clone->type = CT_Friendly;	// don't clip the sprite of the scanned object
			clone->active = OBJ_ALWAYS_ACTIVE;
			clone->user1 = (int)ob;	// scanner animation gets a pointer to the scanned object
			clone->posX = ob->posX;
			clone->posY = ob->clipRects.unitY2 + SCAN_START_Y;
			clone->clipped = CLIP_not;
			clone->gfxChunk = SCANNER1SPR;	//TODO!
			CK_SetAction(clone, &CK_ACT_CloneAnimation);

			//
			// spawn the clone (based on clone machine type)
			//
			clone = CK_GetNewObj(true);
			clone->active = OBJ_ALWAYS_ACTIVE;
			clone->posY = ob->posY;
			clone->posX = ob->posX + (4*TILEGLOBAL)*ob->xDirection;
			clone->xDirection = ob->xDirection;
			clone->yDirection = motion_Down;
			clone->user1 = ob->user1;
			if (ob->user1 == CLONE_POLEKNAWEL)		// Nisaba: spawning position patch
			{
				clone->posX = ob->posX + (-10*TILEGLOBAL)*ob->xDirection;
	
			}
			if (ob->user1 == CLONE_MOLET)
			{
				clone->posX = ob->posX + (0*TILEGLOBAL)*ob->xDirection;
				clone->posY = ob->posY + (-4*TILEGLOBAL)*ob->xDirection;
			}
			if (ob->user1 == CLONE_NISASI)
			{
				clone->posX = ob->posX + (-4*TILEGLOBAL)*ob->xDirection;
				clone->posY = ob->posY + (4*TILEGLOBAL)*ob->xDirection;
			}
			if (ob->user1 == CLONE_JANITOR)
			{
				if ((++ob->user2) == 8)
					ob->type = CT_Friendly;
			}
			clone->type = CT_Clone;
			clone->gfxChunk = ob->gfxChunk;
			clone->actionTimer = ob->actionTimer - tics;
			CK_SetAction(clone, &CK_ACT_CloneObj);
#if (SCAN_DIR == motion_Up)
			clone->user2 = (spritetable[clone->gfxChunk - STARTSPRITES].yh - spritetable[clone->gfxChunk - STARTSPRITES].orgy) >> G_P_SHIFT;
#elif (SCAN_DIR == motion_Down)
			clone->user2 = SCAN_HEIGHT - (((spritetable[clone->gfxChunk - STARTSPRITES].yh - spritetable[clone->gfxChunk - STARTSPRITES].orgy) >> G_P_SHIFT) - spritetable[clone->gfxChunk - STARTSPRITES].height);
#endif
			clone->user1 |= 0x8000;	// hack to prevent clone object from animating
			ob->user1 |= 0x8000;	// also prevent master object from animating

			//
			// we're re-using the scan variable here to spawn the clone animation
			//
			scan = CK_GetNewObj(true);
			scan->zLayer = FOREGROUNDPRIORITY-1;
			scan->active = OBJ_ALWAYS_ACTIVE;
			scan->type = CT_CloneAnimation;
			scan->user1 = (int)clone;	// animation gets a pointer to the clone object
			scan->posX = clone->posX;
			scan->posY = clone->clipRects.unitY2 + SCAN_START_Y;
			scan->clipped = CLIP_not;
			scan->gfxChunk = SCANNER1SPR;	//TODO!
			CK_SetAction(scan, &CK_ACT_CloneAnimation);
		}
	}
	else if (ob->type == CT_Friendly)
	{
		if ((ob->user3 += tics) >= 2*TickBase)
		{
			ck_levelState = LS_ClonedJanitor;
		}
	}
	else
	{
		ob->user3 = 0;
	}
}

//Old:
/* 
void CloneMachineContact(CK_object *ob, CK_object *hit)
{
	int xdist;

	if (ob->user1 == CLONE_JANITOR)
	{
		switch (hit->type)
		{
		case CT_CouncilMember:
		case CT_CouncilMemberDone:
			xdist = hit->posX - ob->posX;
			if (xdist > -2*PIXGLOBAL && xdist < 2*PIXGLOBAL)
			{
				// remove the janitor object
				CK_RemoveObj(hit);
				// make the clone machine display the janitor sprite
				ob->gfxChunk = COUNCILSTANDRSPR;
				ob->visible = true;	// force redraw
				// and deactivate the clone machine by default
				ob->type = CT_CloneMachineOff;
			}
		}
	}
}
*/ 
//New:
void CloneMachineContact(CK_object *ob, CK_object *hit)
{
	int xdist;
	unsigned tile;

	if (ob->user1 == CLONE_JANITOR)
	{
		switch (hit->type)
		{
		case CT_CouncilMember:
		case CT_CouncilMemberDone:
			xdist = hit->posX - ob->posX;
			if (xdist > -2*PIXGLOBAL && xdist < 2*PIXGLOBAL)
			{
				// remove the janitor object
				CK_RemoveObj(hit);
				// make the clone machine display the janitor sprite
				ob->gfxChunk = COUNCILSTANDRSPR;
				ob->visible = true;	// force redraw
				
				// enable first button of activation sequence (at x=4, y=92):
				tile = 3195;	// tile number to place
				RF_MemToMap(&tile, 1, 4, 92, 1, 1);
			}
			break;
			
		case CT_Player:
			// show janitor message 10 if not done already:
			if (ob->gfxChunk && !(ck_gameState.janitorMessageShown & (1 << 10)))
			{
				CK_JanitorDialog(10);
			}
			break;
		}
	}
}

void CloneAnimationThink(CK_object *ob)
{
	ob->actionTimer += tics;
	while (ob->actionTimer >= SCAN_ANIM_DELAY)
	{
		ob->actionTimer -= SCAN_ANIM_DELAY;

		if (++ob->gfxChunk > SCANNER3SPR)
			ob->gfxChunk = SCANNER1SPR;
	}

	ob->user2 += tics;
	while (ob->user2 >= SCAN_MOVE_DELAY)
	{
		ob->user2 -= SCAN_MOVE_DELAY;

		if (++ob->user3 > SCAN_HEIGHT)
		{
			//
			// allow clone/base object to animate again
			//
			((CK_object *)ob->user1)->user1 &= 0x7FFF;
			if (ob->type == CT_CloneAnimation)
			{
				((CK_object *)ob->user1)->user2 = 0;	// nothing clipped off anymore
				((CK_object *)ob->user1)->visible = true;	// force redraw
			}

			//
			// kill the animation object
			//
			CK_RemoveObj(ob);
			return;
		}

		ck_nextY += SCAN_DIR*PIXGLOBAL;
		if (ob->type == CT_CloneAnimation)
		{
			CK_object *clone = (CK_object *)ob->user1;

			if (clone->user2)
			{
				clone->user2--;	// make one more line of the sprite visible
				if (!clone->user2)
					clone->user1 &= 0x7FFF;	// allow object to move and animate
				clone->visible = true;	// force redraw
			}
		}

	}
}

void CloneObjThink(CK_object *ob)
{
	//
	// do nothing while cloning animation is still active
	//
	if (ob->user1 & 0x8000)
		return;

	//
	// animate the clone
	//
	CloneObjAnimate(ob);

	if (ob->type == CT_CloneRising)
	{
		if (ob->bottomTI)	// hit the ceiling?
		{
			CK_RemoveObj(ob);
		}
		else
		{
			CK_PhysAccelVert(ob, motion_Up, 70);
		}
	}
	else
	{
		//
		// clone either falls down or slides to the right
		//
		if (ob->topTI)
		{
			ck_nextX = (PIXGLOBAL/2) * tics * ob->xDirection;
			ck_nextY = abs(ck_nextX)+1;	// also push down (stick to ground)
			ob->velY = 0;
		}
		else
		{
			CK_PhysGravityMid(ob);
		}

		//
		// change the clone's state when it hits a platform blocker icon
		//
		if (CA_TileAtPos(ob->clipRects.tileXmid, ob->clipRects.tileY2, 2) == PLATFORMBLOCK)
		{
			if (ob->user1 == CLONE_JANITOR)
			{
				// janitor clones turn into real janitors:
				ob->currentAction = &CK_ACT_CouncilWalk0;
				ob->type = CT_CouncilMemberDone;
			}
			else
			{
				// other clones start floating upwards:
				ob->type = CT_CloneRising;
				ob->velY = -PIXGLOBAL;
			} 
		}
	}
}

void CloneObjReact(CK_object *ob)
{
	if (ob->user2 < 0)
	{
		CK_BasicDrawFunc1(ob);
	}
	else
	{
#if (SCAN_DIR == motion_Up)
		RF_PlaceSpriteClipped(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, spritedraw, ob->zLayer, ob->user2);
#elif (SCAN_DIR == motion_Down)
		RF_PlaceSpriteClipped(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, spritedraw, ob->zLayer, -ob->user2);
#endif
	}
}

CK_action CK_ACT_CloneMachine = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CloneMachineThink, CloneMachineContact, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_CloneAnimation = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CloneAnimationThink, NULL, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_CloneObj = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CloneObjThink, NULL, CloneObjReact, NULL};

void CK_ToggleCloneMachine(int tileX, int tileY)
{
	CK_object *o;
	unsigned position = tileX | (tileY << 8);

	for (o=ck_keenObj->next; o; o = o->next)
	{
		if (o->user4 == position
			&& (o->type == CT_CloneMachineOn || o->type == CT_CloneMachineOff))
		{
			o->type ^= (CT_CloneMachineOn ^ CT_CloneMachineOff);
			if (o->type == CT_CloneMachineOn)
				ck_mapState.machinesActive++;
			else
				ck_mapState.machinesActive--;
			break;
		}
	}
}

/*
==============================================================================
	CORONA PLANT

	Basically a turret, except with its own sprite. Spits sticky balls.

	user1 - frame offset (for spitting frame)
	user2
	user3
	user4 - Nisasi pointer
------------------------------------------------------------------------------
	CORONA BALL

	A projectile that flies in a straight line and sticks to walls.

	user1 - state (0 = flying, 1 = sticking to wall)
==============================================================================
*/

CK_action CK_ACT_CoronaPlant;
CK_action CK_ACT_CoronaBall;

void CK_SpawnCoronaPlant(int tileX, int tileY, int dir)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->type = CT_CoronaPlant;
	ob->clipped = CLIP_not;
	ob->zLayer = 0;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	ob->xDirection = dir;
	ob->yDirection = (dir == 0)? motion_Up : motion_None;
	ob->velX = ob->xDirection * 64;
	ob->velY = ob->yDirection * 64;
	ob->gfxChunk = CORONAU1SPR + (dir << 1);
	CK_SetAction(ob, &CK_ACT_CoronaPlant);
}

void CoronaPlantThink(CK_object *ob)
{
	CK_object *ball;

	if (ob->user4)	// do nothing while grabbed by Nisasi
		return;

	ob->actionTimer += tics;
	if (ob->user1)
	{
		//
		// plant's mouth is open 
		//
		if (ob->actionTimer >= 34)
		{
			ob->user1 = 0;	// close mouth
			ob->gfxChunk--;
			ob->actionTimer = 0;
		}
	}
	else
	{
		//
		// plant's mouth is closed
		//
		if (ob->actionTimer >= (ck_gameState.difficulty == D_Easy? 3*TickBase : 2*TickBase))
		{
			// start spitting
			ob->user1++;	// open mouth
			ob->gfxChunk++;
			ob->actionTimer = 0;

			// spawn the ball
			ball = CK_GetNewObj(true);
			ball->type = CT_CoronaBall;
			ball->active = OBJ_EXISTS_ONLY_ONSCREEN;
			ball->zLayer = 1;
			ball->posX = ob->posX;
			ball->posY = ob->posY;
			switch (ob->xDirection)
			{
			case motion_Left:
				ball->posX -= 1*PIXGLOBAL;
				ball->posY += 4*PIXGLOBAL;
				break;
			case motion_None:
				ball->posX += 4*PIXGLOBAL;
				ball->posY -= 4*PIXGLOBAL;
				break;
			case motion_Right:
				ball->posX += 6*PIXGLOBAL;
				ball->posY += 4*PIXGLOBAL;
				break;
			}
			ball->velX = ob->velX;
			ball->velY = ob->velY;
			ball->gfxChunk = CORONAVIRUS1SPR;
			CK_SetAction2(ball, &CK_ACT_CoronaBall);
			SD_PlaySound(BERKELOIDTHROWSND);
		}
	}
}

void CoronaBallThink(CK_object *ob)
{
	ob->actionTimer += tics;

	if (ob->user1)
	{
		//
		// ball is sticking to a wall or ceiling
		//

		// remove after 1.4 seconds
		if (ob->actionTimer >= (int)(1.4*TickBase))
			CK_RemoveObj(ob);
	}
	else
	{
		//
		// ball is flying
		//

		/*
		// animate the ball
		while (ob->actionTimer >= 6)
		{
			ob->actionTimer -= 6;
			if (++ob->gfxChunk > CORONAVIRUS3SPR)
				ob->gfxChunk = CORONAVIRUS1SPR;
		}
		*/

		ck_nextX = ob->velX * tics;
		ck_nextY = ob->velY * tics;
	}
}

void CoronaBallReact(CK_object *ob)
{
	if (ob->topTI || ob->leftTI || ob->rightTI || ob->bottomTI)
	{
		ob->user1 = 1;	// is now sticking to something
		ob->actionTimer = 0;	// reset value (this is now used for the vanish countdown)
	}
	CK_BasicDrawFunc1(ob);
}

CK_action CK_ACT_CoronaPlant = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CoronaPlantThink, CK_DeadlyCol, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_CoronaBall = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CoronaBallThink, CK_DeadlyCol2, CoronaBallReact, NULL};

/*
==============================================================================
	NIPNAP

	Hanging from ceiling. Reaches down when Keen is near, then pauses.

	user1 - internal state (see enum)
==============================================================================
*/

enum {NIPNAP_IDLE, NIPNAP_DOWN, NIPNAP_UP, NIPNAP_PAUSE};

CK_action CK_ACT_Nipnap;

void CK_SpawnNipnap(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->type = CT_Nipnap;
	ob->clipped = CLIP_not;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	ob->gfxChunk = NIPNAPHIDESPR;
	CK_SetAction(ob, &CK_ACT_Nipnap);
}

void NipnapThink(CK_object *ob)
{
	ob->actionTimer += tics;
	switch (ob->user1)
	{
	case NIPNAP_IDLE:
		if (!(ck_keenObj->posY - ob->posY <= 65*PIXGLOBAL
			&& ck_keenObj->clipRects.unitX1 < ob->clipRects.unitX2+24*tics		// Keen's walking speed is 24 units per tic
			&& ck_keenObj->clipRects.unitX2 > ob->clipRects.unitX1-24*tics))	// Keen's walking speed is 24 units per tic
		{
			break;
		}
		SD_PlaySound(GRABBINGSND);
		ob->user1++;	// start going down
		ob->actionTimer = tics;
		// no break here!
	case NIPNAP_DOWN:
		while (ob->actionTimer >= 4)	// need a while loop for constant timing here because MAXTICS is 5
		{
			ob->actionTimer -= 4;
			if (++ob->gfxChunk == NIPNAPGRAB4SPR)
			{
				ob->user1++;	// start going up
				ob->actionTimer = 0;
				break;	// break the while-loop
			}
		}
		break;
	case NIPNAP_UP:
		if (ob->actionTimer >= 16)
		{
			ob->actionTimer -= 16;
			if (--ob->gfxChunk == NIPNAPSLEEPSPR)
			{
				ob->user1++;	// start pause
				ob->actionTimer = 0;
			}
		}
		break;
	case NIPNAP_PAUSE:
		if (ob->actionTimer >= 1*TickBase)
		{
			ob->gfxChunk++;	// open the eye again
			ob->actionTimer = 0;
			ob->user1 = NIPNAP_IDLE;
		}
		break;
	}
}

void NipnapContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		if (ob->user1 == NIPNAP_IDLE)
		{
			// idle NipNap starts reaching down when touched by Keen
			// (next contact will stun Keen)
			ob->user1++;
			ob->gfxChunk++;
			ob->visible = true;
			ob->actionTimer = 0;
			SD_PlaySound(GRABBINGSND);
		}
		else if (ob->user1 != NIPNAP_PAUSE)	// Keen can only get stunned when NipNap is not "sleeping"
		{
			CK_StunKeen(ob->clipRects.unitXmid);
		}
		break;
	case CT_Stunner:
		CK_ShotHit(hit);
		break;
	}
}

CK_action CK_ACT_Nipnap = {0, 0, AT_Frame, false, push_none, 0, 0, 0, NipnapThink, NipnapContact, CK_BasicDrawFunc1, NULL};


/*
==============================================================================
	CRAB

	Can walk on floors, walls and ceilings

	user1 - sprite frame offset (added to ob->gfxChunk for drawing)
	user2 - turn type
	user3 - next state after turning
	user4 - combined x/y tile position for climbing around outer edges
==============================================================================
*/

CK_action CK_ACT_CrabFloor;
CK_action CK_ACT_CrabWall;
CK_action CK_ACT_CrabCeiling;
CK_action CK_ACT_CrabFall;
CK_action CK_ACT_CrabTurn;

typedef enum {
	inner_FloorToLeftWall,
	inner_FloorToRightWall,
	inner_LeftWallToFloor,
	inner_RightWallToFloor,
	inner_CeilingToLeftWall,
	inner_CeilingToRightWall,
	inner_LeftWallToCeiling,
	inner_RightWallToCeiling,

	outer_start,

	outer_FloorToLeftWall=outer_start,
	outer_FloorToRightWall,
	outer_LeftWallToFloor,
	outer_RightWallToFloor,
	outer_CeilingToLeftWall,
	outer_CeilingToRightWall,
	outer_LeftWallToCeiling,
	outer_RightWallToCeiling,

	NUMTURNTYPES
} turntype;

// Note: The left and right walls refer to the left or right side of the tile
// the crab is currently climbing on. It does NOT mean that the wall is on the
// left or right side of the crab. If the crab is climbing the left side of a
// wall, then the wall will be on the right side of the crab (and vice versa).

void CK_SpawnCrab(int tileX, int tileY, boolean onCeiling)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->type = CT_Crab;
	ob->clipped = CLIP_custom;
	ob->actionTimer = US_RndT() & 6;
	ob->user1 = US_RndT() % 3;
	ob->posX = RF_TileToUnit(tileX);
	ob->xDirection = (US_RndT() < 0x80)? motion_Left : motion_Right;
	if (onCeiling)
	{
		ob->posY = RF_TileToUnit(tileY) - CRAB_Y1;
		ob->yDirection = motion_Up;
		CK_SetAction(ob, &CK_ACT_CrabCeiling);
	}
	else
	{
		ob->posY = RF_TileToUnit(tileY) - CRAB_Y2 + TILEGLOBAL-1;
		ob->yDirection = motion_Down;
		CK_SetAction(ob, &CK_ACT_CrabFloor);
	}
}

void CrabTurnReact(CK_object *obj)
{
	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, (obj->user2 & 4)? spritedraw|drawflipped : spritedraw, obj->zLayer);
}

void CrabStartTurn(CK_object *ob, unsigned tx, unsigned ty, turntype turn)
{
	//ck_nextX = ck_nextY = 0;	// redundant: we always call CK_SetAction2(), which sets these to 0

	ob->user2 = turn;
	ob->clipped = CLIP_not;
	switch ((turn >> 1) & 3)
	{
	case 0:
	case 2:
		ob->user3 = (int)&CK_ACT_CrabWall;
		break;
	case 1:
		ob->user3 = (int)&CK_ACT_CrabFloor;
		break;
	case 3:
		ob->user3 = (int)&CK_ACT_CrabCeiling;
	}
	if (turn >= outer_start)
	{
		// These tables define the Crab's position while it is turning, relative
		// to the Crab's position before it started turning. They have no effect
		// on the position of the Crab after it has turned, so they are purely
		// cosmetical. Feel free to adjust these as you please.
#define XOFF (CRAB_XMID/2) 
#define YOFF (CRAB_YMID/2) 
#if 1
		static int xoff[8] = {-XOFF, XOFF,  XOFF, -XOFF, -XOFF,  XOFF, XOFF, -XOFF};
		static int yoff[8] = { YOFF, YOFF, -YOFF, -YOFF, -YOFF, -YOFF, YOFF,  YOFF};
#else
		static int xoff[8] = {-XOFF, XOFF,  XOFF, -XOFF, -XOFF,  XOFF, XOFF, -XOFF-4*PIXGLOBAL};
		static int yoff[8] = { YOFF, YOFF, -YOFF, -YOFF, -YOFF, -YOFF-4*PIXGLOBAL, YOFF,  YOFF-4*PIXGLOBAL};
#endif
#undef XOFF
#undef YOFF
		
		ob->user4 = tx | (ty << 8);

		turn &= 7;
		ob->posX += xoff[turn];
		ob->posY += yoff[turn];

		ob->gfxChunk = ((turn & 3)^2) + CRABCORNER1SPR;
	}
	else
	{
		ob->gfxChunk = (turn & 3) + CRABCORNER1SPR;
	}
	CK_SetAction2(ob, &CK_ACT_CrabTurn);
	CrabTurnReact(ob);
}

void CrabFloorThink(CK_object *ob)
{
	//
	// update animation:
	//
	if (++ob->user1 >= 3)
		ob->user1 = 0;

	//
	// turn around when hitting a block icon:
	//
	if (ob->xDirection > 0)
	{
		if (CA_TileAtPos(RF_UnitToTile(ob->posX+CRAB_X2), RF_UnitToTile(ob->posY+CRAB_Y2), 2) == PLATFORMBLOCK)
		{
			ob->xDirection = -ob->xDirection;
			ck_nextX = -ck_nextX;
		}
	}
	else
	{
		if (CA_TileAtPos(RF_UnitToTile(ob->posX+CRAB_X1), RF_UnitToTile(ob->posY+CRAB_Y2), 2) == PLATFORMBLOCK)
		{
			ob->xDirection = -ob->xDirection;
			ck_nextX = -ck_nextX;
		}
	}
}

void CrabTurnThink(CK_object *ob)
{
	register int turn = ob->user2;
	
	if (turn >= outer_start)
	{
		// these tables now define the EXACT position of the Crab after turning,
		// in relation to the tile it climbed around (in global units):
		static int xoff[8] =
		{
			// outer_FloorToLeftWall, outer_FloorToRightWall:
			-(CRAB_X2+1), TILEGLOBAL-CRAB_X1,
			// outer_LeftWallToFloor, outer_RightWallToFloor:
			-CRAB_XMID+PIXGLOBAL, TILEGLOBAL-CRAB_XMID-PIXGLOBAL,
			// outer_CeilingToLeftWall, outer_CeilingToRightWall:
			-(CRAB_X2+1), TILEGLOBAL-CRAB_X1,
			// outer_LeftWallToCeiling, outer_RightWallToCeiling:
			-CRAB_XMID+PIXGLOBAL, TILEGLOBAL-CRAB_XMID-PIXGLOBAL,
		};
		static int yoff[8] =
		{
			// outer_FloorToLeftWall, outer_FloorToRightWall:
			-CRAB_YMID+PIXGLOBAL, -CRAB_YMID+PIXGLOBAL,
			// outer_LeftWallToFloor, outer_RightWallToFloor:
			-(CRAB_Y2+1), -(CRAB_Y2+1),
			// outer_CeilingToLeftWall, outer_CeilingToRightWall:
			TILEGLOBAL-CRAB_YMID-PIXGLOBAL, TILEGLOBAL-CRAB_YMID-PIXGLOBAL,
			// outer_LeftWallToCeiling, outer_RightWallToCeiling:
			TILEGLOBAL-CRAB_Y1, TILEGLOBAL-CRAB_Y1,
		};
		// You can adjust the offsets that use PIXGLOBAL (change PIXGLOBAL to
		// 2*PIXGLOBAL or 3*PIXGLOBAL etc.), but that may cause problems with
		// slope tiles near the corner the crab climbed around.

		turn &= 7;
#if (G_T_SHIFT == 8) && (sizeof(ob->user4) == 2)
		// optimized version:
		ob->posX = RF_TileToUnit(ob->user4) + xoff[turn];	// don't need to mask off the bits of the y position, they get shifted out anyways
		ob->posY = (ob->user4 & 0xFF00) + yoff[turn];	// don't need to shift here, the bits of the y position are already in the right place
#else
		// safe, but slightly slower version:
		ob->posX = RF_TileToUnit(ob->user4 & 0xFF) + xoff[turn];
		ob->posY = RF_TileToUnit((ob->user4 >> 8) & 0xFF) + yoff[turn];
#endif
	}
	
	ob->clipped = CLIP_custom;
	CK_SetAction2(ob, (CK_action *)ob->user3);
}

void CrabWallThink(CK_object *ob)
{
	int tx, ty;

	//
	// update animation:
	//
	if (++ob->user1 >= 3)
		ob->user1 = 0;

	//
	// turn around when hitting a block icon:
	//
	if (ob->yDirection > 0)
	{
		if (CA_TileAtPos(RF_UnitToTile(ob->posX+CRAB_XMID), RF_UnitToTile(ob->posY+CRAB_Y2), 2) == PLATFORMBLOCK)
		{
			ob->yDirection = -ob->yDirection;
			ck_nextY = -ck_nextY;
		}
	}
	else
	{
		if (CA_TileAtPos(RF_UnitToTile(ob->posX+CRAB_XMID), RF_UnitToTile(ob->posY+CRAB_Y1), 2) == PLATFORMBLOCK)
		{
			ob->yDirection = -ob->yDirection;
			ck_nextY = -ck_nextY;
		}
	}

	//
	// check walls
	// Note: this uses custom clipping, so we can't just use the hitbox values here
	//
	ty = RF_UnitToTile(ob->posY + ck_nextY + CRAB_YMID);

	if (ob->xDirection < 0)
	{
		tx = RF_UnitToTile(ob->posX+CRAB_X1 - PIXGLOBAL);

		if (!TI_ForeRight(CA_TileAtPos(tx, ty, 1)))
		{
			if (ob->yDirection < 0 && TI_ForeTop(CA_TileAtPos(tx, ty+1, 1)) )
			{
				ob->yDirection = motion_Down;
				CrabStartTurn(ob, tx, ty+1, outer_RightWallToFloor);
				return;
			}
			else if (ob->yDirection > 0 && TI_ForeBottom(CA_TileAtPos(tx, ty-1, 1)) )
			{
				ob->yDirection = motion_Up;
				CrabStartTurn(ob, tx, ty-1, outer_RightWallToCeiling);
				return;
			}
			else
			{
				ck_nextY = -ck_nextY;
				ob->yDirection = -ob->yDirection;
			}
		}
	}
	else
	{
		tx = RF_UnitToTile(ob->posX+CRAB_X2 + PIXGLOBAL);

		if (!TI_ForeLeft(CA_TileAtPos(tx, ty, 1)))
		{
			if (ob->yDirection < 0 && TI_ForeTop(CA_TileAtPos(tx, ty+1, 1)) )
			{
				ob->yDirection = motion_Down;
				CrabStartTurn(ob, tx, ty+1, outer_LeftWallToFloor);
				return;
			}
			else if (ob->yDirection > 0 && TI_ForeBottom(CA_TileAtPos(tx, ty-1, 1)) )
			{
				ob->yDirection = motion_Up;
				CrabStartTurn(ob, tx, ty-1, outer_LeftWallToCeiling);
				return;
			}
			else
			{
				ck_nextY = -ck_nextY;
				ob->yDirection = -ob->yDirection;
			}
		}
	}
}

void CrabCeilingThink(CK_object *ob)
{
	//
	// update animation:
	//
	if (++ob->user1 >= 3)
		ob->user1 = 0;

	//
	// turn around when hitting a block icon:
	//
	if (ob->xDirection > 0)
	{
		if (CA_TileAtPos(RF_UnitToTile(ob->posX+CRAB_X2), RF_UnitToTile(ob->posY+CRAB_Y1), 2) == PLATFORMBLOCK)
		{
			ob->xDirection = -ob->xDirection;
			ck_nextX = -ck_nextX;
		}
	}
	else
	{
		if (CA_TileAtPos(RF_UnitToTile(ob->posX+CRAB_X1), RF_UnitToTile(ob->posY+CRAB_Y1), 2) == PLATFORMBLOCK)
		{
			ob->xDirection = -ob->xDirection;
			ck_nextX = -ck_nextX;
		}
	}
	// TODO: start falling when above Keen?
}

void near CrabDraw(CK_object *obj)
{
	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk+obj->user1, spritedraw, obj->zLayer);
}

void near CrabDrawFlipped(CK_object *obj)
{
	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk+obj->user1, spritedraw|drawflipped, obj->zLayer);
}

void CrabFloorReact(CK_object *ob)
{
	if (ob->xDirection > 0 && ob->leftTI)
	{
		ob->yDirection = motion_Up;
		CrabStartTurn(ob, 0, 0, inner_FloorToLeftWall);
		return;
	}
	else if (ob->xDirection < 0 && ob->rightTI)
	{
		ob->yDirection = motion_Up;
		CrabStartTurn(ob, 0, 0, inner_FloorToRightWall);
		return;
	}
	else if (!ob->topTI)
	{
		int tx = RF_UnitToTile(ob->posX - ob->deltaPosX + CRAB_XMID);
		int ty = RF_UnitToTile(ob->posY + CRAB_Y2 + PIXGLOBAL);
		
		if (ob->xDirection > 0)
		{
			if (TI_ForeRight(CA_TileAtPos(tx, ty, 1)))
			{
				ob->xDirection = motion_Left;
				CrabStartTurn(ob, tx, ty, outer_FloorToRightWall);
				return;
			}
		}
		else
		{
			if (TI_ForeLeft(CA_TileAtPos(tx, ty, 1)))
			{
				ob->xDirection = motion_Right;
				CrabStartTurn(ob, tx, ty, outer_FloorToLeftWall);
				return;
			}
		}

		//
		// cannot climb around corner, so just turn around:
		//
		TurnX(ob);
		// start falling if still not touching a tile
		if (!ob->topTI)
		{
			CK_SetAction2(ob, &CK_ACT_CrabFall);
		}
	}
	CrabDraw(ob);
}

void CrabWallReact(CK_object *ob)
{
	if (ob->yDirection < 0 && ob->bottomTI)
	{
		ob->xDirection = -ob->xDirection;
		if (ob->xDirection < 0)
			CrabStartTurn(ob, 0, 0, inner_LeftWallToCeiling);
		else
			CrabStartTurn(ob, 0, 0, inner_RightWallToCeiling);
		return;
	}
	else if (ob->yDirection > 0 && ob->topTI)
	{
		ob->xDirection = -ob->xDirection;
		if (ob->xDirection < 0)
			CrabStartTurn(ob, 0, 0, inner_LeftWallToFloor);
		else
			CrabStartTurn(ob, 0, 0, inner_RightWallToFloor);
		return;
	}
	else if (ob->xDirection < 0 && !ob->rightTI)
	{
		//
		// outer edges are handled by the think function, so just turn around here:
		//
		TurnY(ob);
		// start falling if still not touching a tile
		if (!ob->rightTI)
		{
			CK_SetAction2(ob, &CK_ACT_CrabFall);
			CK_BasicDrawFunc1(ob);
			return;
		}
	}
	else if (ob->xDirection > 0 && !ob->leftTI)
	{
		//
		// outer edges are handled by the think function, so just turn around here:
		//
		TurnY(ob);
		// start falling if still not touching a tile
		if (!ob->leftTI)
		{
			CK_SetAction2(ob, &CK_ACT_CrabFall);
			CK_BasicDrawFunc1(ob);
			return;
		}
	}

	if (ob->yDirection < 0)
		CrabDraw(ob);
	else
		CrabDrawFlipped(ob);
}

void CrabCeilingReact(CK_object *ob)
{
	if (ob->xDirection > 0 && ob->leftTI)
	{
		ob->yDirection = motion_Down;
		CrabStartTurn(ob, 0, 0, inner_CeilingToLeftWall);
		return;
	}
	else if (ob->xDirection < 0 && ob->rightTI)
	{
		ob->yDirection = motion_Down;
		CrabStartTurn(ob, 0, 0, inner_CeilingToRightWall);
		return;
	}
	else if (!ob->bottomTI)
	{
		int tx = RF_UnitToTile(ob->posX - ob->deltaPosX + CRAB_XMID);
		int ty = RF_UnitToTile(ob->posY + CRAB_Y1 - PIXGLOBAL);

		if (ob->xDirection > 0)
		{
			if (TI_ForeRight(CA_TileAtPos(tx, ty, 1)))
			{
				ob->xDirection = motion_Left;
				CrabStartTurn(ob, tx, ty, outer_CeilingToRightWall);
				return;
			}
		}
		else
		{
			if (TI_ForeLeft(CA_TileAtPos(tx, ty, 1)))
			{
				ob->xDirection = motion_Right;
				CrabStartTurn(ob, tx, ty, outer_CeilingToLeftWall);
				return;
			}
		}

		//
		// cannot climb around corner, so just turn around:
		//
		TurnX(ob);
		// start falling if still not touching a tile
		if (!ob->bottomTI)
		{
			CK_SetAction2(ob, &CK_ACT_CrabFall);
		}
	}
	CrabDrawFlipped(ob);
}

void CrabFallReact(CK_object *ob)
{
	if (ob->topTI)
	{
		ob->velY = 0;
		ob->yDirection = motion_Down;
		CK_SetAction2(ob, &CK_ACT_CrabFloor);
	}
	CrabDraw(ob);
}

void CrabContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		CK_KillKeen();
		break;
	case CT_Stunner:
		CK_ShotHit(hit);
		ob->clipped = CLIP_custom;
		if (ob->currentAction == &CK_ACT_CrabWall)
			ob->xDirection = -ob->xDirection;	// walk away from the wall after landing
		CK_SetAction2(ob, &CK_ACT_CrabFall);
	}
}

CK_action CK_ACT_CrabFloor = {CRABWALKL1SPR, CRABWALKR1SPR, AT_UnscaledOnce, false, push_down, 8, 18, 0, CrabFloorThink, CK_DeadlyCol, CrabFloorReact, &CK_ACT_CrabFloor};
CK_action CK_ACT_CrabWall = {CRABWALLW1SPR, CRABWALLE1SPR, AT_UnscaledOnce, false, push_side, 8, 0, 18, CrabWallThink, CrabContact, CrabWallReact, &CK_ACT_CrabWall};
CK_action CK_ACT_CrabCeiling = {CRABWALKL1SPR, CRABWALKR1SPR, AT_UnscaledOnce, false, push_up, 8, 18, 0, CrabCeilingThink, CrabContact, CrabCeilingReact, &CK_ACT_CrabCeiling};
CK_action CK_ACT_CrabFall = {CRABWALKL1SPR, CRABWALKR1SPR, AT_Frame, false, push_none, 0, 0, 0, CK_Fall, CK_DeadlyCol, CrabFallReact, &CK_ACT_CrabFall};
CK_action CK_ACT_CrabTurn = {0,0, AT_UnscaledOnce, false, push_none, 10, 0, 0, CrabTurnThink, CrabContact, CrabTurnReact, &CK_ACT_CrabTurn};


/*
==============================================================================
	BRAIN FUCL

	Grabs onto Keen on touch and triggers the bad ending.

	user1 - state
	user2 - animation counter
	user3 - 
	user4 - 
==============================================================================
*/

CK_action CK_ACT_BrainFucl;
CK_action CK_ACT_KeenBrainFucled;

void CK_SpawnBrainFucl(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->type = CT_BrainFucl;
	ob->clipped = CLIP_not;
	ob->zLayer = 2;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	ob->gfxChunk = BRAINFUCL1SPR;
	CK_SetAction(ob, &CK_ACT_BrainFucl);
	ck_mapState.brainfucl = -1;	// level has BrainFucl, but it's not yet active
}

void BrainFuclThink(CK_object *ob)
{
	if (ob->user1)
	{
		register int maxspeed = tics*2;

		ck_nextX = ck_keenObj->posX + 5*PIXGLOBAL - ob->posX;

		if (ck_nextX < -maxspeed)
			ck_nextX = -maxspeed;
		else if (ck_nextX > maxspeed)
			ck_nextX = maxspeed;
	}

	switch (ob->user1)
	{
	case 1:
		if (ob->posY < ck_keenObj->posY - 44*PIXGLOBAL)
		{
			ck_nextY = tics*(PIXGLOBAL/2);
		}
		else
		{
			ob->user1++;
			ck_keenObj->gfxChunk = KEENLOOKUPSPR;
			ck_keenObj->visible = true;
		}
		break;
	case 2:
		if (ob->posY < ck_keenObj->posY - 24*PIXGLOBAL)
		{
			ck_nextY = tics*(PIXGLOBAL/3);
		}
		else
		{
			ob->user1++;
			ob->gfxChunk++;
			ck_keenObj->gfxChunk = KEENSTANDLSPR;
			ck_keenObj->visible = true;
		}
		break;
	case 3:
		if (ob->posY < ck_keenObj->posY - 17*PIXGLOBAL)
		{
			ck_nextY = tics*(PIXGLOBAL/4);
		}
		else
		{
			ob->user1++;
			ob->gfxChunk++;
		}
		break;
	case 4:
		ob->actionTimer += tics;
		SD_SetMusicRate(350);

		if (ob->actionTimer >= 8)
		{
			ob->actionTimer -= 9;
			ob->gfxChunk ^= BRAINFUCL2SPR ^ BRAINFUCL3SPR;
			ck_gameState.keenScore >>= 1;
			ob->user2++;
			if (ob->user2 == 6)
			{
				ck_keenObj->gfxChunk = VOODOOKEENSPR;
				ck_keenObj->visible = true;
			}
			else if (ob->user2 == 42)
			{
				ck_levelState = LS_BrainFucled;
			}
		}
	}
}

void BrainFuclContact(CK_object *ob, CK_object *hit)
{
	if (hit->type != CT_Player)
		return;

	if (ck_mapState.brainfucl < 0)
	{
		ck_mapState.brainfucl = 1;	// brainfucl is now active and blocks player input
		brainfucled++;
		if (hit->gfxChunk >= KEENPOGORSPR && hit->gfxChunk <= KEENPOGOBOUNCELSPR)
		{
			// make Keen stop pogoing
			CK_SetAction2(hit, &CK_ACT_keenFall1);
		}
	}
	else if (!ob->user1 && hit->currentAction->stickToGround == push_down && hit->posX + 4*PIXGLOBAL <= ob->posX)
	{
		// Keen is on ground and has moved past the BrainFucl thing
		// so activate the BrainFucl thing:
		ob->user1++;
		// and make Keen stop walking
		hit->gfxChunk = KEENSTANDLSPR;
		CK_SetAction2(hit, &CK_ACT_KeenBrainFucled);
	}
}

CK_action CK_ACT_BrainFucl = {0, 0, AT_Frame, false, push_none, 0, 0, 0, BrainFuclThink, BrainFuclContact, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_KeenBrainFucled = {0, 0, AT_Frame, false, push_none, 0, 0, 0, NULL, NULL, CK_BasicDrawFunc1, NULL};

/*
==============================================================================
	FLYING DONUT

	Bonus item that flies around a bit.

	user1 - item number (for bonus handling)
	user2 - initial x position
	user3 - initial y position
	user4 - 
==============================================================================
*/

#define DONUT_ANIM_DELAY 6

extern CK_action CK_ACT_Donut;

void CK_SpawnDonut(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->clipped = CLIP_not;
	ob->type = CT_Item;
	ob->user1 = 12;	// 10000 points
	ob->user2=ob->posX = RF_TileToUnit(tileX);
	ob->user3=ob->posY = RF_TileToUnit(tileY);
	ob->xDirection = US_RndT() < 0x80 ? motion_Left : motion_Right;
	ob->yDirection = US_RndT() < 0x80 ? motion_Up : motion_Down;
	ob->gfxChunk = FLYDONUT1SPR;
	CK_SetAction(ob, &CK_ACT_Donut);
	ck_mapState.itemsTotal++;
}

void DonutThink(CK_object *ob)
{
	ob->actionTimer += tics;
	if (ob->actionTimer >= 2*DONUT_ANIM_DELAY)
	{
		ob->actionTimer -= 2*DONUT_ANIM_DELAY;

		if (US_RndT() < 4)
			ob->xDirection = -ob->xDirection;
		if (US_RndT() < 4)
			ob->yDirection = -ob->yDirection;
	}

	ob->gfxChunk = ob->actionTimer < DONUT_ANIM_DELAY ? FLYDONUT1SPR : FLYDONUT2SPR; 

	if ((int)(ob->user2 - ob->posX) > 74*PIXGLOBAL)
		ob->xDirection = motion_Right;
	if ((int)(ob->posX - ob->user2) > 74*PIXGLOBAL)
		ob->xDirection = motion_Left;

	if ((int)(ob->user3 - ob->posY) > 24*PIXGLOBAL)
		ob->yDirection = motion_Down;
	if ((int)(ob->posY - ob->user3) > 24*PIXGLOBAL)
		ob->yDirection = motion_Up;

	CK_PhysAccelHorz(ob, ob->xDirection, 9);
	CK_PhysAccelVert(ob, ob->yDirection, 8);
}

CK_action CK_ACT_Donut = {0, 0, AT_Frame, false, push_none, 0, 0, 0, DonutThink, NULL, CK_BasicDrawFunc1, NULL};

/*
==============================================================================
	BALLOON SMIKRY

	Floats up and disappears as soon as it's off-camera. Just eye candy.

	user1 -
	user2 -
	user3 -
	user4 - 
==============================================================================
*/

extern CK_action CK_ACT_BalloonSmirky;

void CK_SpawnBalloonSmirky(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->clipped = CLIP_not;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	ob->gfxChunk = SMIRKYBALLOON1SPR;
	CK_SetAction(ob, &CK_ACT_BalloonSmirky);
}

void BalloonSmirkyThink(CK_object *ob)
{
	CK_PhysAccelVert(ob, motion_Up, 12);

	if (!CK_ObjectVisible(ob))
		CK_RemoveObj(ob);

	if ((ob->actionTimer += tics) >= 20)
	{
		ob->actionTimer -= 44;
		ob->gfxChunk ^= SMIRKYBALLOON1SPR ^ SMIRKYBALLOON2SPR;
	}
}

CK_action CK_ACT_BalloonSmirky = {0, 0, AT_Frame, false, push_none, 0, 0, 0, BalloonSmirkyThink, NULL, CK_BasicDrawFunc1, NULL};

/*
==============================================================================
	FUNGUSAMONGUS

	Kind of a combination of Bluebird (idle until touched, like the egg,
	stunnable like the bird) and Berkeloid (chase behavior).

	user1 - internal state (see enum)
	user2 - cycle counter for wakeup animation
	user3 -
	user4 - 
==============================================================================
*/

extern CK_action CK_ACT_Fungus;
extern CK_action CK_ACT_FungusFall;

#define FU_WALK_SPEED (ck_gameState.difficulty == D_Hard? 25 : 24)
#define FU_ANIM_DELAY 8
#define FU_STUN_DELAY (ck_gameState.difficulty == D_Hard? 2*TickBase : 4*TickBase)
#define FU_WAKEUP_DELAY 16
#define FU_WAKEUP_CYCLES 4

enum {FU_IDLE, FU_WALK, FU_STUNNED, FU_WAKEUP};

void CK_SpawnFungusAmongus(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->posX = RF_TileToUnit(tileX-1);
	ob->posY = RF_TileToUnit(tileY) - 25*PIXGLOBAL;
	ob->gfxChunk = FUNGUSAMONGUSSTUNNEDSPR;
	ob->xDirection = (US_RndT() < 0x90)? motion_Left : motion_Right;
	ob->yDirection = motion_Down;
	ob->user1 = FU_IDLE;
	ob->type = CT_Fungus;
	CK_SetAction(ob, &CK_ACT_Fungus);
}

void FungusThink(CK_object *ob)
{
	switch (ob->user1)
	{
	case FU_WALK:
		if ((ob->actionTimer += tics) >= FU_ANIM_DELAY)
		{
			ob->actionTimer -= FU_ANIM_DELAY;
			if (++ob->gfxChunk > FUNGUSAMONGUS5SPR)
				ob->gfxChunk = FUNGUSAMONGUS1SPR;

			// berkeloid-like behavior: 1/8 chance of turning to chase Keen
			if (US_RndT() <= 44)
			{
				ob->xDirection = ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid? motion_Left : motion_Right;
			}
		}
		ck_nextX = ob->xDirection * tics * FU_WALK_SPEED;
		break;

	case FU_STUNNED:
		if ((ob->actionTimer += tics) >= FU_STUN_DELAY)
		{
			ob->user2 = ob->actionTimer = 0;
			SD_PlaySound(FUNGUSSND);
			ob->user1 = FU_WAKEUP;
		}
		break;

	case FU_WAKEUP:
		if ((ob->actionTimer += tics) >= FU_WAKEUP_DELAY)
		{
			ob->actionTimer -= FU_WAKEUP_DELAY;

			if (++ob->user2 >= FU_WAKEUP_CYCLES)
			{
				ob->user2 = 0;
				ob->user1 = FU_WALK;
				ob->gfxChunk = FUNGUSAMONGUS1SPR;
			}
			else
			{
				ob->gfxChunk = (ob->user2 & 1)? FUNGUSAMONGUS1SPR : FUNGUSAMONGUSSTUNNEDSPR;
			}
		}
		break;
	}
}

void FungusContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		switch (ob->user1)
		{
		case FU_WALK:
			CK_KillKeen();
			break;

		case FU_IDLE:
			ob->user1 = FU_WAKEUP;
			SD_PlaySound(FUNGUSSND);
			break;
		}
		break;

	case CT_Stunner:
		switch (ob->user1)
		{
		case FU_WALK:
		case FU_WAKEUP:
			CK_ShotHit(hit);
			if (ob->gfxChunk != FUNGUSAMONGUSSTUNNEDSPR)
			{
				ob->posY -= 9*PIXGLOBAL;
				ob->gfxChunk = FUNGUSAMONGUSSTUNNEDSPR;
			}
			ob->velY = -32;
			ob->user1 = FU_STUNNED;
			ob->actionTimer = 0;
			CK_SetAction2(ob, &CK_ACT_FungusFall);
			break;
		}
		break;
	}
}

void FungusReact(CK_object *ob)
{
	if (!ob->topTI || (ob->xDirection > 0 && ob->leftTI) || (ob->xDirection < 0 && ob->rightTI))
	{
		ob->xDirection = -ob->xDirection;
		ob->posX -= ob->deltaPosX;
		ob->posY -= ob->deltaPosY;
		CK_SetAction2(ob, ob->currentAction);
		if (!ob->topTI)
			CK_SetAction2(ob, &CK_ACT_FungusFall);
	}
	CK_BasicDrawFunc1(ob);
}

void FungusFallReact(CK_object *ob)
{
	if (ob->topTI)
	{
		ob->velY = 0;
		CK_SetAction2(ob, &CK_ACT_Fungus);
	}
	CK_BasicDrawFunc1(ob);
}

CK_action CK_ACT_Fungus = {0, 0, AT_Frame, false, push_down, 0, 0, 0, FungusThink, FungusContact, FungusReact};
CK_action CK_ACT_FungusFall = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CK_Fall, NULL, FungusFallReact};

/*
==============================================================================
	WORLDMAP NESSIE

	Sticks its head up when Keen is near, then vanishes forever.

	user1 - internal state (0 = waiting, 1 = rising, 2 = looking, 3 = lowering)
	user2 - how far it has risen, in pixels
	user3 -
	user4 - 
==============================================================================
*/

#define WORLDNESSIE_SPRITEHEIGHT 20
#define WORDLNESSIE_DELAY 1

extern CK_action CK_ACT_WorldNessie;

void CK_SpawnWorldNessie(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->clipped = CLIP_not;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	ob->type = CT_WorldNessie;
	CK_SetAction(ob, &CK_ACT_WorldNessie);
}

void WorldNessieThink(CK_object *ob)
{
	int xdist, ydist;

	switch (ob->user1)
	{
	case 0:
		xdist = ob->clipRects.tileXmid - ck_keenObj->clipRects.tileXmid;
		ydist = ob->clipRects.tileY1 - ck_keenObj->clipRects.tileY1;
		if (xdist >= -3 && xdist <= 3 && ydist >= -3 && ydist <= 3)
		{
			// start rising:
			ob->user1++;
			SD_PlaySound(STATUSUPSND);
			break;	// for testing

			// remove all other worldmap nessies from the level:
			// for (ob = ck_keenObj; ob; ob = ob->next)
			// {
			// 	if (ob->type == CT_WorldNessie && ob->user1 == 0)
			// 		CK_RemoveObj(ob);
			// }
		}
		break;
	case 1:
		if ((ob->actionTimer += tics) >= WORDLNESSIE_DELAY)
		{
			ob->actionTimer -= WORDLNESSIE_DELAY;
			ob->visible = true;
			if (++ob->user2 >= WORLDNESSIE_SPRITEHEIGHT)
				ob->user1++;
		}
		break;
	case 2:
		if ((ob->actionTimer += tics) >= 100)
		{
			ob->actionTimer = 0;
			ob->user1++;
			SD_PlaySound(STATUSDOWNSND);
		}
		break;
	case 3:
		if ((ob->actionTimer += tics) >= WORDLNESSIE_DELAY)
		{
			ob->actionTimer -= WORDLNESSIE_DELAY;
			ob->visible = true;
			if (--ob->user2 <= 0)
				ob->user1++;
		}
		break;
	default:
		CK_RemoveObj(ob);
	//	for testing only:  
	//	ob->actionTimer = ob->user1 = ob->user2 = 0;
	}
}

void WorldNessieReact(CK_object *ob)
{
	if (ob->user2)
	{
		// draw with clipping
		int skip = WORLDNESSIE_SPRITEHEIGHT - ob->user2;
		RF_PlaceSpriteClipped(&ob->sde, ob->posX, ob->posY+RF_PixelToUnit(skip), ob->gfxChunk, spritedraw, ob->zLayer, -skip);
	//	CK_BasicDrawFunc1(ob);
	}
	else
	{
		RF_RemoveSprite(&ob->sde);
	}
}

CK_action CK_ACT_WorldNessie = {WORLDNESSIE, WORLDNESSIE, AT_Frame, false, push_none, 0, 0, 0, WorldNessieThink, NULL, WorldNessieReact, NULL};

#undef WORLDNESSIE_SPRITEHEIGHT


/*
==============================================================================
	FROG

	Randomly jumps up. Just eye candy.

	user1 - jump countdown
	user2 - 
	user3 -
	user4 - 
==============================================================================
*/

#define FROG_MINJUMPDELAY (3*TickBase)
#define FROG_MAXJUMPDELAY (18*TickBase)

extern CK_action CK_ACT_Frog;

void CK_SpawnFrog(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	CK_SetAction(ob, &CK_ACT_Frog);
}

void FrogThink(CK_object *ob)
{
	if (ob->user1)
	{
		if ((unsigned)ob->user1 > tics)
		{
			ob->user1 -= tics;
		}
		else
		{
			// start jumping:
			ob->user1 = 0;
			ob->velY = -48;
		}
	}
	else
	{
		CK_Fall2(ob);
	}
}

void FrogReact(CK_object *ob)
{
	if (ob->bottomTI)	// hit ceiling?
		ob->velY = 0;	// start falling

	if (ob->topTI)		// hit floor?
		ob->user1 = US_RndT() * ((FROG_MAXJUMPDELAY-FROG_MINJUMPDELAY)/255) + FROG_MINJUMPDELAY;

	CK_BasicDrawFunc1(ob);
}

CK_action CK_ACT_Frog = {FROGSPR, FROGSPR, AT_Frame, false, push_none, 0, 0, 0, FrogThink, NULL, FrogReact, NULL};


/*
==============================================================================
	COUNCIL PAGE II

	Stands or walks around. Triggers a dreaming interaction when touched by
	Keen.

	user1 - internal state
	user2 - touch cooldown
	user3 - dream number
	user4 - 
==============================================================================
*/

extern CK_action CK_ACT_KeenLook;
extern CK_action CK_ACT_KeenShrug;

enum {PAGE_AMBUSH, PAGE_WALK, PAGE_STAND, PAGE_DREAM};

void PageWalk(CK_object *ob)
{
	unsigned tx;

	// update touch cooldown:
	if ((ob->user2 -= tics) < 0)
		ob->user2 = 0;

	switch (ob->user1)
	{
	case PAGE_WALK:
		// start standing still when countdown ends:
		if ((ob->actionTimer -= tics) < 0)
		{
			ob->user1 = PAGE_STAND;	// start standing still
			ob->actionTimer = 100;
			break;
		}
		ck_nextX = ob->xDirection * tics * (PAGE_WALKSPEED/2);
		ob->gfxChunk = PAGEWALKLSPR+1 + ob->xDirection;

		// turn around when  hitting a block icon:
		tx = RF_UnitToTile(ob->clipRects.unitXmid + ck_nextX);
		if (tx != ob->clipRects.tileXmid)
		{
			if (CA_TileAtPos(tx, ob->clipRects.tileY2, 2) == PLATFORMBLOCK)
			{
				ob->user1 = PAGE_STAND;
				ck_nextX = 0;
				ob->gfxChunk = PAGESLEEPLSPR+1 + ob->xDirection;
				ob->timeUntillThink = ob->actionTimer = 50;	// wait 50 ticks before turning, stand for another 50
				ob->xDirection = -ob->xDirection;
			}
		}
		break;

	case PAGE_STAND:
		ob->gfxChunk = PAGESLEEPLSPR+1 + ob->xDirection;
		if ((ob->actionTimer -= tics) < 0)
		{
			ob->actionTimer = US_RndT()*4 + 100;	// pick a random delay for standing still
			ob->user1 = PAGE_WALK;	// start walking again
		}
		break;

	case PAGE_DREAM:
		ob->actionTimer += tics;
		ob->gfxChunk = ((ob->actionTimer >> 4) & 1) + PAGEDREAM1SPR;
		if (ob->actionTimer >= 30 && ob->actionTimer - tics < 30)
		{
			RemoveDreamBubbles();	// just in case Keen is dreaming or something...
			SpawnDreamBubble(ob->posX+4*PIXGLOBAL, ob->posY-30*PIXGLOBAL, 0x8001 + ob->user3);
			ob->user3 ^= 1;	// show other dream bubble next time
		}
		if (ob->actionTimer > 5*TickBase || ck_keenObj->currentAction != &CK_ACT_KeenLook)	// Remember: dream bubble lasts over 5 seconds
		{
			ob->active = OBJ_ACTIVE;	// no longer always active
			ob->user2 = 3*TickBase;	// start touch cooldown (3 seconds)
			ob->user1 = PAGE_STAND;	// start standing
			ob->actionTimer = 100;
			RemoveDreamBubbles();	// in case the dream bubble is still active

			if (ck_keenObj->currentAction == &CK_ACT_KeenLook)
				CK_SetAction2(ck_keenObj, &CK_ACT_KeenShrug);
		}
		break;
	}
}

void PageContact(CK_object *ob, CK_object *hit)
{
	if (hit->type == CT_Player)
	{
		if (ob->user1 == PAGE_AMBUSH)	// page is waiting for first contact
		{
			ob->user1 = PAGE_STAND;	// change to standing
			ob->actionTimer = 0.5*TickBase;	// start walking after 0.5 seconds
			ob->user2 = 1.5*TickBase;	// page starts interacting with Keen 1 second after he starts walking
		}
		else if (ob->user2 == 0 && ob->user1 != PAGE_DREAM && hit->topTI
			&& (hit->currentAction->think == CK_KeenStandingThink
			 || hit->currentAction->think == CK_KeenRunningThink))
		{
			ob->active = OBJ_ALWAYS_ACTIVE;	// just to make sure the dream animation ends
			ob->user1 = PAGE_DREAM;	// start the animation
			ob->actionTimer = 0;

			// change Keen into the "cutscene" state:
			hit->gfxChunk = KEENANGRY1SPR;
			CK_SetAction2(hit, &CK_ACT_KeenLook);
		}
	}
}

void PageWalkReact(CK_object *ob)
{
	unsigned y;

	y = ob->posY;

	if ((ob->xDirection > 0 && ob->leftTI) || (ob->xDirection < 0 && ob->rightTI) || !ob->topTI)
	{
		ob->posX -= ob->deltaPosX;
		ob->posY -= ob->deltaPosY;
		ob->gfxChunk = PAGESLEEPLSPR+1 + ob->xDirection;
		ob->xDirection = -ob->xDirection;
		ob->timeUntillThink = 10 + US_RndT() / 16;
	}

	switch (ob->gfxChunk)
	{
	case PAGEWALKLSPR:
	case PAGEWALKRSPR:
		y -= (((ob->posX-1) << 1) & (PIXGLOBAL << 3)) >> 3;	// adds up & down movement while walking
	}
	RF_PlaceSprite(&ob->sde, ob->posX, y, ob->gfxChunk, spritedraw, ob->zLayer);
}

void KeenLook(CK_object *ob)
{
	ob->actionTimer += tics;
	if (ob->actionTimer > 100)
		ob->actionTimer -= 100;
	ob->gfxChunk = ob->actionTimer > 90? KEENANGRY1SPR : KEENANGRY2SPR;
}

CK_action CK_ACT_PageWalk = {0, 0, AT_Frame, false, push_down, 0, 0, 0, PageWalk, PageContact, PageWalkReact, NULL};
CK_action CK_ACT_KeenLook = {0, 0, AT_Frame, false, push_down, 0, 0, 0, KeenLook, CK_KeenColFunc, CK_KeenDrawFunc, NULL};
CK_action CK_ACT_KeenShrug = {JEDIBOOTSPR, JEDIBOOTSPR, AT_UnscaledOnce, false, push_down, 80, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenStanding}; 

/*
==============================================================================
	QUICKSAND PLATFORM

	Allows Keen to stand and walk inside the quicksand tiles. Slowly moves down
	to make Keen sink into the quicksand.

	user1 - 
	user2 - 
	user3 - 
	user4 - 
==============================================================================
*/

extern CK_action CK_ACT_Quicksand;

void CK_SpawnQuicksand(unsigned posX, unsigned posY)
{
	CK_object *ob = CK_GetNewObj(true);
	if (ob == &tempObj)
		return;

	ck_mapState.quicksand = ob;
	ob->type = CT_Platform;
	ob->clipped = CLIP_not;
	ob->clipRects.unitX2 = (ob->posX = ob->clipRects.unitX1 = posX) + TILEGLOBAL;
	ob->clipRects.unitY2 = (ob->posY = ob->clipRects.unitY1 = posY) + TILEGLOBAL;
	CK_SetAction(ob, &CK_ACT_Quicksand);

	ob->deltaPosY = -TILEGLOBAL;	// allow Keen to land
	CK_PhysPushY(ck_keenObj, ob);
	ob->deltaPosY = 0;
}

void QuicksandThink(CK_object *ob)
{
	ck_nextY = tics*2;	// platform moves down and therefore allows Keen to sink deeper into the quicksand
}

// Note: The sprites are only used for their hitbox, they are never drawn!
CK_action CK_ACT_Quicksand = {WORLDKEENL1SPR, WORLDKEENL1SPR, AT_Frame, false, push_none, 0, 0, 0, QuicksandThink, NULL, NULL, NULL};


/*
==============================================================================
	FALLING BLOCK

	description.

	user1 - internal state and falling counter (0 = waiting, 1..4 = falling)
	user2 - 
	user3 - foreground sprite pointer
	user4 - 
==============================================================================
*/

extern CK_action CK_ACT_Fallblock;

void CK_SpawnFallBlock(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(true);

	ob->active = OBJ_ALWAYS_ACTIVE;
	ob->type = CT_FallBlock;
	ob->clipped = CLIP_custom;
	ob->posX = RF_TileToUnit(tileX) - 8*PIXGLOBAL;
	ob->posY = RF_TileToUnit(tileY);
	CK_SetAction(ob, &CK_ACT_Fallblock);
}

void FallBlockThink(CK_object *ob)
{
	if (ob->user1 == 0)
	{
		// start falling when Keen is below the block:
		if (ck_keenObj->clipRects.unitXmid >= ob->clipRects.unitX1
			&& ck_keenObj->clipRects.unitXmid <= ob->clipRects.unitX2
			&& ck_keenObj->clipRects.unitY1 > ob->clipRects.unitY2)
		{
			// abort if any blocking tiles are in the way:
			register int y;
			register unsigned mapoff = mapbwidthtable[ob->clipRects.tileY2]/2 + ob->clipRects.tileXmid;
			for (y = ob->clipRects.tileY2; y <= ck_keenObj->clipRects.tileY1; y++, mapoff+=mapwidth)
			{
				if (TI_ForeTop(mapsegs[1][mapoff]))
					return;
			}

			// start falling:
			ob->user1++;
			ck_mapState.shaketics = TickBase/4;	// shake the screen
			SD_PlaySound(YETICLOUDSND);	
		}
	}
	else
	{
		if (ob->user1 < 4)
			ob->user1++;
		CK_Fall(ob);
	}
}

void FallBlockContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		if (ob->user1 == 0 && hit->velY > 30)
		{
			ob->user1++;	// start falling
			ck_mapState.shaketics = TickBase/4;	// shake the screen
			SD_PlaySound(YETICLOUDSND);	
		}
		break;
	case CT_Stunner:
		CK_ShotHit(hit);
		break;
	}
}

void FallBlockReact(CK_object *ob)
{
	if (ob->topTI)
	{
		if (ob->user1 >= 4)
		{
			ck_mapState.shaketics = ob->velY;	// shake the screen
			SD_PlaySound(KITTENEXPLODESND);	
		}
		ob->user1 = ob->velY = 1;
	}
	CK_BasicDrawFunc1(ob);
	RF_PlaceSprite((void**)&ob->user3, ob->posX, ob->posY, SANDSTONEFORESPR, spritedraw, MASKEDTILEPRIORITY -1);
}

CK_action CK_ACT_Fallblock = {SANDSTONESPR, SANDSTONESPR, AT_Frame, false, push_none, 0, 0, 0, FallBlockThink, FallBlockContact, FallBlockReact, NULL};


/*
==============================================================================
	OBELISK

	Stationary obstacle that blocks Keen and Keen's shots. Sinks into the ground
	when shot.

	user1 - clipping value (must be negative)
	user2 - shake & sink counter
	user3 - 
	user4 - 
==============================================================================
*/

extern CK_action CK_ACT_Obelisk;

#define OBELISK_SINKDELAY	8	// takes this many tics to sink by one pixel
#define OBELISK_MAXSINK		80	// how many pixels the Obelisk can sink in total
#define OBELISK_PERSHOT		8	// sink this many pixels after each shot

void CK_SpawnObelisk(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);

	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	ob->clipped = CLIP_not;
	ob->type = CT_Obelisk;
	CK_SetAction(ob, &CK_ACT_Obelisk);
}

void ObeliskThink(CK_object *ob)
{
	if (ob->user2)
	{
		if ((ob->actionTimer += tics) > OBELISK_SINKDELAY)
		{
			SD_PlaySound(YETICLOUDSND);	
			ob->actionTimer -= OBELISK_SINKDELAY;
			ob->user2--;
			if (ob->user1 > -OBELISK_MAXSINK)	// don't sink more than this
			{
				// sink by one pixel:
				ob->user1--;	// adjust clipping (negative value -> cut off pixels at the bottom)
				ob->posY += PIXGLOBAL;
				ob->clipRects.unitY1 += PIXGLOBAL;	// move the bottom part of the hitbox down
				
			}
			else
			{
				// cannot sink any more:
				ob->user2 = 0;	// stop shaking
				ob->posX |= PIXGLOBAL;
			}
		}
		ob->posX ^= PIXGLOBAL;	// make it shake
		ob->visible = true;	// force redraw
	}
}

void ObeliskContact(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		// quick hack to make sure Keen always gets pushed out of the Obelisk's hitbox:
		ob->deltaPosX = hit->clipRects.unitXmid < ob->clipRects.unitXmid? -3*TILEGLOBAL : 3*TILEGLOBAL;
		PushKeenX(hit, ob);
		break;

	case CT_Stunner:
		// make it shake and sink:
		ob->user2 = OBELISK_PERSHOT;
		CK_ShotHit(hit);
		break;
	}
}

void ObeliskReact(CK_object *ob)
{
	RF_PlaceSpriteClipped(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, spritedraw, ob->zLayer, ob->user1);
}

CK_action CK_ACT_Obelisk = {OBELISKSPR, OBELISKSPR, AT_Frame, false, push_none, 0, 0, 0, ObeliskThink, ObeliskContact, ObeliskReact, NULL};

/*
==============================================================================
	MUMMY

	Type (either CT_Mummy1 or CT_Mummy2) defines teleport destination for Keen.

	user1 - internal state (mummy) / visibility value (spawner)
	user2 - height clipping (in pixels)
	user3 - look counter
	user4 - 


	KEEN MUMMY

	Stores mummy's type as its own type (for teleport destination)

	user1 - internal state
	user2 - old originxglobal \_  for detecting when the screen stops
	user3 - old originyglobal /   scrolling to teleport destination
	user4 - 
==============================================================================
*/

extern CK_action CK_ACT_MummySpawner;
extern CK_action CK_ACT_Mummy;
extern CK_action CK_ACT_MummyLook;
extern CK_action CK_ACT_KeenMummy;
extern CK_action CK_ACT_Bandage;

#define MUMMY_HEIGHT	36		// height of the mummy sprite(s) in pixels
#define MUMMY_RISEDELAY 5
#define MUMMY_SINKDELAY 5
#define MUMMY_OFFSCREENDELAY	(5*TickBase)	// hide mummy if not visible during the last 5 seconds
#define MUMMY_RESPAWNDELAY	(ck_gameState.difficulty == D_Hard? 2*TickBase : 4*TickBase)	// hidden mummy waits 4 seconds before allowing a respawn
#define BANDAGE_ANIMDELAY	12

enum {MUMMY_STAND, MUMMY_RISE, MUMMY_SINK, MUMMY_HIDDEN, MUMMY_RUN, MUMMY_SPIN, MUMMY_SPINDONE, MUMMY_LOOK, MUMMY_AFTERLOOK};
enum {KMUMMY_FALL, KMUMMY_PREWRAP, KMUMMY_WRAP, KMUMMY_POSTWRAP, KMUMMY_VANISH, KMUMMY_WAITSCROLL, KMUMMY_UNWRAP, KMUMMY_DONE};

void CK_SpawnMummySpawner(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);

	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY) - 20*PIXGLOBAL;
	CK_SetAction(ob, &CK_ACT_MummySpawner);
}

void MummySpawnerThink(CK_object *ob)
{
	boolean wasVisible;

	// update visibility variable (and remember old value):
	wasVisible = ob->user1;
	ob->user1 = CK_ObjectVisible(ob);

	// if no mummy exists and spawner is visible then randomly spawn a mummy:
	if (!ck_mapState.mummy && ob->user1 && US_RndT() < 0x10)
	{
		CK_object *ob2 = CK_GetNewObj(true);
		ob2->posX = ob->posX;
		ob2->posY = ob->posY;
		ob2->type = ob->posX >= 60*TILEGLOBAL ? CT_Mummy1 : CT_Mummy2;
		ob2->active = OBJ_ALWAYS_ACTIVE;
		ob2->xDirection = motion_Left;
		if (wasVisible)
		{
			// make mummy rise out of the floor:
			ob2->user1 = MUMMY_RISE;
			SD_PlaySound(MUMMYSND);
			ob2->user2 = MUMMY_HEIGHT;
		}
		CK_SetAction(ob2, &CK_ACT_Mummy);
		ck_mapState.mummy = ob2;
	}
}

void MummyCheckForKeen(CK_object *ob)
{
	unsigned xdist = abs(ck_keenObj->clipRects.unitXmid - ob->clipRects.unitXmid);
	unsigned ydist = abs(ck_keenObj->posY - ob->posY);

	if (xdist < 5*TILEGLOBAL && ydist < 2*TILEGLOBAL)
	{
		// start running, but only if Keen isn't in a mummy state
		if (ck_keenObj->currentAction != &CK_ACT_KeenMummy)
		{
			// TODO? maybe play a sound here?
			ob->xDirection = ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid? -1 : 1;
			ob->user1 = MUMMY_RUN;
			CK_SetAction2(ob, &CK_ACT_Mummy);
		}
	}
}

void MummyThink(CK_object *ob)
{
	switch (ob->user1)
	{
	case MUMMY_STAND:
#if 0
	//	if (US_RndT() <= tics)
		{
set_direction:
			if (ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid)
				ob->xDirection = motion_Left;
			else
				ob->xDirection = motion_Right;
		}

	//	ck_nextX = 8*ob->xDirection;	// allow mummy to move

		// hide the mummy when it hasn't been visible for a while:
		if (CK_ObjectVisible(ob))
		{
			ob->actionTimer = 0;	// reset counter
		}
		else if ((ob->actionTimer += tics) >= MUMMY_OFFSCREENDELAY)
		{
			ob->user1 = MUMMY_HIDDEN;
			ob->user2 = MUMMY_HEIGHT;
		}
#else
		if ((ob->actionTimer += tics) >= 28)
		{
		//	ob->actionTimer = 0;	// Note: no need to reset actionTimer, action change resets it
			ob->currentAction = &CK_ACT_MummyLook;
			ob->user1 = MUMMY_LOOK;
		}
#endif
		MummyCheckForKeen(ob);
		break;

	case MUMMY_LOOK:
		if ((ob->actionTimer += tics) >= 48)
		{
			// stop looking:
			ob->currentAction = &CK_ACT_Mummy;	// Note: no need to reset actionTimer, action change resets it
			ob->user1 = MUMMY_AFTERLOOK;
		}
		MummyCheckForKeen(ob);
		break;

	case MUMMY_AFTERLOOK:
		if ((ob->actionTimer += tics) >= 48)
		{
			ob->actionTimer = 0;
			if (++ob->user3 >= 3)
			{
				// looked both directions, so start sinking:
				ob->user1 = MUMMY_SINK;
				SD_PlaySound(MUMMYSND);
				return;
			}

			ob->xDirection = -ob->xDirection;
			ob->user1 = MUMMY_STAND;
			ob->visible = true;
		}
		MummyCheckForKeen(ob);
		break;

	case MUMMY_RISE:
		if ((ob->actionTimer += tics) >= MUMMY_RISEDELAY)
		{
			ob->actionTimer -= MUMMY_RISEDELAY;
			ob->xDirection = -ob->xDirection;
			ob->visible = true;
			
			if ((ob->user2 -= 4) <= 0)
			{
				ob->user2 = 0;
				ob->user1 = MUMMY_STAND;
				if (ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid)
					ob->xDirection = motion_Left;
				else
					ob->xDirection = motion_Right;
			}
		}
		break;

	case MUMMY_SINK:
		if ((ob->actionTimer += tics) >= MUMMY_SINKDELAY)
		{
			ob->actionTimer -= MUMMY_SINKDELAY;
			ob->xDirection = -ob->xDirection;
			ob->visible = true;
			if ((ob->user2 += 4) >= MUMMY_HEIGHT)
			{
				ob->user2 = MUMMY_HEIGHT;
				ob->user1 = MUMMY_HIDDEN;
			}
		}
		break;

	case MUMMY_RUN:
	//	ob->xDirection = (ob->clipRects.unitXmid > ck_keenObj->clipRects.unitXmid)? motion_Left : motion_Right;
		ck_nextX = ob->xDirection*tics*35;
		// TODO: stop running if it can't reach Keen
		break;

	case MUMMY_SPIN:
		ck_nextX = ob->xDirection*tics*38;
		
		// We don't want the mummy to move (too far) past the destination, but
		// we also need to make sure the mummy's xDirection matches the movement
		// for the current frame so that it doesn't get stuck when moving off an
		// egde. That's why we need two checks for each direction: one to make
		// sure the mummy doesnt move past the destination and another to make
		// it turn once it has reached the destination.
		
		if (ob->xDirection > 0)
		{
			register unsigned destx = ck_keenObj->clipRects.unitXmid+TILEGLOBAL;
			if (ob->clipRects.unitXmid >= destx)
			{
				ob->xDirection = motion_Left;
				ob->zLayer = ck_keenObj->zLayer-1;
				ck_nextX = -ck_nextX;
			}
			else if (ob->clipRects.unitXmid+ck_nextX > destx)
			{
				ck_nextX = destx - ob->clipRects.unitXmid;
			}
		}
		else
		{
			register unsigned destx = ck_keenObj->clipRects.unitXmid-TILEGLOBAL;
			if (ob->clipRects.unitXmid <= destx)
			{
				ob->xDirection = motion_Right;
				ob->zLayer = ck_keenObj->zLayer+1;
				ck_nextX = -ck_nextX;
			}
			else if (ob->clipRects.unitXmid+ck_nextX < destx)
			{
				ck_nextX = destx - ob->clipRects.unitXmid;
			}
		}
		break;

	case MUMMY_SPINDONE:
		// TODO: move away from Keen?
		ob->user1 = MUMMY_SINK;
		SD_PlaySound(MUMMYSND);
		break;

	case MUMMY_HIDDEN:
		// remove the hidden mummy after a while:
		// (this allows the mummy spawner to spawn a new mummy)
		if ((ob->actionTimer += tics) >= MUMMY_RESPAWNDELAY)
		{
			if (ck_mapState.mummy == ob)
				ck_mapState.mummy = NULL;
			CK_RemoveObj(ob);
		}
		break;

	}
}
/*
void MummyLookThink(CK_object *ob)
{
	int xdist = ck_keenObj->clipRects.unitXmid - ob->clipRects.unitXmid;
	int ydist = abs(ck_keenObj->posY - ob->posY);

	ob->actionTimer += tics;
	if (ob->xDirection > 0)
	{
		if (xdist >= 0 && xdist < 4*TILEGLOBAL && ydist < 2*TILEGLOBAL)
		{
start_running:
			// start running, but only if Keen isn't in a mummy state
			if (ob->user1 != MUMMY_RUN && ck_keenObj->currentAction != &CK_ACT_KeenMummy)
			{
				ob->user1 = MUMMY_RUN;
			}
		}
	}
	else
	{
		if (xdist <= 0 && xdist > -4*TILEGLOBAL && ydist < 2*TILEGLOBAL)
		{
			goto start_running;
		}
	}

	if (ob->actionTimer >= 48)
	{
		// stop looking:
		ob->currentAction = &CK_ACT_Mummy;
	}
}
*/

void MummyContact(CK_object *ob, CK_object *hit)
{
	// fake a smaller hitbox when mummy is sinking/rising:
	if (ob->user1 == MUMMY_HIDDEN || hit->clipRects.unitY2 < ob->clipRects.unitY1 + RF_PixelToUnit(ob->user2))
		return;

	switch (hit->type)
	{
	case CT_Player:
		if (CK_KeenIsDead())
			break;

		switch (ob->user1)
		{
		case MUMMY_RUN:
		case MUMMY_STAND:
		case MUMMY_LOOK:
			ob->user1 = MUMMY_SPIN;
			SD_PlaySound(MUMMYENWRAPPINGSND);
			CK_SetAction2(ob, &CK_ACT_Mummy);

			hit->type = ob->type;
			hit->clipped = CLIP_normal;
			hit->zLayer = 1;
			hit->gfxChunk = KEENSTANDSPR;
			hit->user1=hit->user2=hit->user3=hit->user4 = 0;
			CK_SetAction2(hit, &CK_ACT_KeenMummy);
		}
		break;

	case CT_Stunner:
		CK_ShotHit(hit);
		CK_SetAction2(ob, &CK_ACT_Mummy);
		ob->user1 = MUMMY_SINK;
		break;
	}
}

void MummyReact(CK_object *ob)
{
	if (!ob->topTI || ob->leftTI && ob->xDirection > 0 || ob->rightTI && ob->xDirection < 0)
	{
		register int oldtimer = ob->actionTimer;
		ob->posX -= ob->deltaPosX;
		ob->xDirection = -ob->xDirection;
		CK_SetAction2(ob, ob->currentAction);
		ob->actionTimer = oldtimer;	// don't interrupt the offscreen counter
		switch (ob->user1)
		{
		case MUMMY_SPIN:
			ob->zLayer = ck_keenObj->zLayer+ob->xDirection;
			break;

		case MUMMY_RUN:
			ob->user1 = MUMMY_SPINDONE;
			break;
		}
	}
	RF_PlaceSpriteClipped(&ob->sde, ob->posX, ob->posY+RF_PixelToUnit(ob->user2), ob->gfxChunk, spritedraw, ob->zLayer, -ob->user2);
}

void KeenMummyThink(CK_object *ob)
{
	switch (ob->user1)
	{
	case KMUMMY_FALL:
		if ((ob->actionTimer += tics) >= 2*TickBase)
		{
			// Mummy couldn't reach Keen, so reset Keen:
			ob->currentAction = &CK_ACT_keenFall1;
			ob->type = CT_Player;

			// reset mummy:
			if (ck_mapState.mummy)
				ck_mapState.mummy->user1 = MUMMY_SPINDONE;
		}
		// no break here - both substates need gravity

	case KMUMMY_PREWRAP:
		CK_PhysGravityHigh(ob);
		break;

	case KMUMMY_WRAP:
		// wait until mummy is gone:
		if (!ck_mapState.mummy || ck_mapState.mummy->user1 == MUMMY_HIDDEN)
		{
			ob->actionTimer = 0;
			ob->user1++;
		}
		break;

	case KMUMMY_POSTWRAP:
		// wait two seconds before making the bandages fall:
		if ((ob->actionTimer += tics) >= 2*TickBase)
		{
			ob->actionTimer -= BANDAGE_ANIMDELAY;
			if (++ob->gfxChunk == BANDAGEFALL3SPR)
			{
				ob->actionTimer = 0;
				ob->user1++;
			}
		}
		break;

	case KMUMMY_VANISH:
		// wait a while, then start moving the camera to the destination:
		if ((ob->actionTimer += tics) >= 2*TickBase)
		{
			// spawn a removable bandage where Keen was standing:
			CK_object *bandage;
			bandage = CK_GetNewObj(true);
			bandage->posX = ob->posX;
			bandage->posY = ob->posY;
			bandage->zLayer = 1;
			bandage->gfxChunk = BANDAGEFALL3SPR;
			CK_SetAction(bandage, &CK_ACT_Bandage);

			// move Keen to the destination:
			ob->velX=ob->velY=ob->user2=ob->user3=ob->user4 = 0;
			if (ob->type == CT_Mummy1)
			{
				CK_SpawnKeen(92, 79, 0);
			}
			else
			{
				CK_SpawnKeen(71, 68, 0);
			}
			ob->currentAction = &CK_ACT_KeenMummy;
			ob->gfxChunk = KEENMUMMY4SPR;
			ob->user1 = KMUMMY_WAITSCROLL;
		}
		break;

	case KMUMMY_WAITSCROLL:
		// waiting for the camera to stop scrolling:
		if (ob->user2 == originxglobal && ob->user3 == originyglobal)
		{
			ob->user1++;
		}
		else
		{
			ob->user2 = originxglobal;
			ob->user3 = originyglobal;
		}
		break;

	case KMUMMY_UNWRAP:
		if ((ob->actionTimer += tics) >= 3*TickBase)
		{
			// spawn falling bandages:
			CK_object *bandage;
			bandage = CK_GetNewObj(true);
			bandage->posX = ob->posX;
			bandage->posY = ob->posY;
			bandage->zLayer = 2;	// must be in front of Keen
			bandage->gfxChunk = BANDAGEFALL1SPR;
			CK_SetAction(bandage, &CK_ACT_Bandage);

			ob->gfxChunk = KEENSTANDSPR;
			ob->user1++;
			ob->actionTimer = 0;
		}
		break;

	case KMUMMY_DONE:
		if ((ob->actionTimer += tics) >= 5*BANDAGE_ANIMDELAY)
		{
			// allow player to control Keen again:
			ob->currentAction = &CK_ACT_keenStanding;
			ob->user1=ob->user2=ob->user3=ob->user4 = 0;
		}
	}
}

void KeenMummyContact(CK_object *ob, CK_object *hit)
{
	// always run Keen's default contact routine first to allow Keen to land on platform objects:
	CK_KeenColFunc(ob, hit);
	
	if (ob->user1 > KMUMMY_PREWRAP)
		return;

	switch (hit->type)
	{
	case CT_Mummy1:
	case CT_Mummy2:
		//  only update the wrapping state if Keen is standing on something solid
		//	and the mummy has moved past Keen during the current frame:
		if (ob->topTI && hit->clipRects.unitXmid >= ob->clipRects.unitXmid
			&& hit->clipRects.unitXmid-hit->deltaPosX < ob->clipRects.unitXmid)
		{
			ob->user1 = KMUMMY_PREWRAP;
			ob->visible = true;
			RemoveDreamBubbles();	// Nisaba patch: just in case Keen is dreaming

			// only switch to the next wrap sprite if Mummy is drawn in front of Keen:
			if (hit->zLayer <= ob->zLayer)
				break;

			if (ob->gfxChunk == KEENSTANDSPR)
			{
				ob->gfxChunk = KEENMUMMY1SPR;
			}
			else
			{
				ob->gfxChunk++;
				if (ob->gfxChunk == KEENMUMMY6SPR)
				{
					ob->actionTimer = 0;
					ob->user1++;
					hit->user1 = MUMMY_SPINDONE;
				}
			}
		}
	}
}

void BandageThink(CK_object *ob)
{
	if (ob->gfxChunk == BANDAGEFALL3SPR)
	{
		if (!CK_ObjectVisible(ob) || ++ob->user1 >= 20)
			CK_RemoveObj(ob);
	}
	else
	{
		ob->gfxChunk++;
	}
}

CK_action CK_ACT_MummySpawner = {MUMMYLSPR, MUMMYRSPR, AT_Frame, false, push_none, 0, 0, 0, MummySpawnerThink, NULL, NULL, NULL};
CK_action CK_ACT_Mummy = {MUMMYLSPR, MUMMYRSPR, AT_Frame, false, push_down, 0, 0, 0, MummyThink, MummyContact, MummyReact, NULL};
CK_action CK_ACT_MummyLook = {MUMMYLOOKLSPR, MUMMYLOOKRSPR, AT_Frame, false, push_down, 0, 0, 0, MummyThink, MummyContact, MummyReact, NULL};
CK_action CK_ACT_KeenMummy = {0, 0, AT_Frame, false, push_none, 0, 0, 0, KeenMummyThink, KeenMummyContact, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_Bandage = {0, 0, AT_UnscaledOnce, false, push_none, BANDAGE_ANIMDELAY, 0, 0, BandageThink, NULL, CK_BasicDrawFunc1, &CK_ACT_Bandage};

/*
==============================================================================
	LEMON SHARK

	Floats left and disappears as soon as it's off-camera. Just eye candy.

	user1 -
	user2 -
	user3 -
	user4 - 
==============================================================================
*/

extern CK_action CK_ACT_LemonShark;

void CK_SpawnLemonShark(int tileX, int tileY)
{
	CK_object *ob = CK_GetNewObj(false);
	ob->clipped = CLIP_not;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
//	ob->gfxChunk = LEMONSHARKSPR;
	CK_SetAction(ob, &CK_ACT_LemonShark);
}

void LemonSharkThink(CK_object *ob)
{
	CK_PhysAccelHorz(ob, motion_Left, 8);
	CK_PhysAccelVert(ob, motion_Down, 2);

	if (!CK_ObjectVisible(ob))
		CK_RemoveObj(ob);
}

CK_action CK_ACT_LemonShark = {LEMONSHARKSPR, LEMONSHARKSPR, AT_Frame, false, push_none, 0, 0, 0, LemonSharkThink, NULL, CK_BasicDrawFunc1, NULL};

/*
==============================================================================
	DONUT CLOUD

	Appears on the worldmap, hovers in place when 500000 points are reached
	and disappears again at 800000 points. 

	user1 - initial y position (for hovering)
	user2 - remaining chase ticks
	user3 - amplitude
==============================================================================
*/

extern CK_action CK_ACT_DonutCloud;

void CK_SpawnDonutCloud(int tileX, int tileY)
{
	if (ck_gameState.keenScore >= 500000 && !ck_gameState.cheated && !ck_gameState.levelsDone[MAP_BONUSLEVEL])
//	if (ck_gameState.keenScore >=    800 &&  ck_gameState.keenScore < 1300 && !ck_gameState.cheated) // just for testing reasons
	{
		CK_object *ob;
	
		ob = CK_GetNewObj(false);
		ob->type = CT_DonutCloud;
	
		ob->clipped = CLIP_not;
		ob->zLayer = 3;
		ob->posX = RF_TileToUnit(tileX) - 8*PIXGLOBAL;
		ob->posY = RF_TileToUnit(tileY) - 6*PIXGLOBAL;
		ob->user1 = ob->posY;
		ob->user3 = 24;
	//	ob->xDirection = motion_None;
		ob->yDirection = motion_Up;
		CK_SetAction(ob, &CK_ACT_DonutCloud);
		
		CK_SpawnStaticSprite(tileX, tileY+2, DONUTCLOUDSHADOWSPR, 1);
	}
}

void DonutCloudThink(CK_object *ob)
{
	if (ob->yDirection == motion_Up && ob->posY < ob->user1-ob->user3)
	{
		ob->yDirection = motion_Down;
	}
	else if (ob->yDirection == motion_Down && ob->posY > ob->user1+ob->user3)
	{
		ob->yDirection = motion_Up;
	}

	CK_PhysAccelVert(ob, ob->yDirection, 4);
//	if (!CK_ObjectVisible(ob))
//		CK_RemoveObj(ob);
}

/*
void DonutCloudContact(CK_object *ob, CK_object *hit)
{
	if (hit->type != CT_Player)
		return;
}
*/

CK_action CK_ACT_DonutCloud = {DONUTCLOUDSPR, DONUTCLOUDSPR, AT_Frame, false, push_none, 0, 0, 0, DonutCloudThink, NULL, CK_BasicDrawFunc1, NULL};


/*
==============================================================================
	JEDI BOOT

	Object to enter the secret TARDIS level or the MUSHROOM2 level when touched.
	
	user1 - non-zero after explode sound has been played
	user2 - 
	user3 - 
	user4 - 
==============================================================================
*/

extern CK_action CK_ACT_JediBoot;
extern CK_action CK_ACT_KeenSmoke;

void CK_SpawnJediBoot(int tileX, int tileY)
{
	CK_object *ob;
	
	ob = CK_GetNewObj(false);
	ob->type = CT_JediBoot;
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	CK_SetAction(ob, &CK_ACT_JediBoot);	
//	ob->user4 = 0;
}

void JediBootThink(CK_object *ob)
{
//	ob->gfxChunk = JEDIBOOTSPR;
	
	if (ck_gameState.currentLevel == MAP_MUSHROOMLEVEL1 && !ob->user1)
	{
		ob->user1 = 1;	// don't play it again, Sam!
		SD_PlaySound(KITTENEXPLODESND);
	}
}

void JediBootContact(CK_object *ob, CK_object *hit) 
{
	if (hit->type == CT_Player)
	{
		if (ck_gameState.currentLevel == MAP_MUSHROOMLEVEL1)
		{
		//	ck_levelState = LS_MushroomTrigger;
			
			// turn Keen into smoke cloud:
			hit->gfxChunk = SMIRKYPOOF1SPR;
			hit->type = CT_Friendly;	// prevent other object from interacting with Keen
			hit->clipped = CLIP_not;
			hit->posX -= 8*PIXGLOBAL;
			hit->user1 = 0;
			CK_SetAction2(hit, &CK_ACT_KeenSmoke);
			hit->actionTimer = 10;
			SD_PlaySound(KITTENEXPLODESND);
		}
		
		else if (ck_gameState.currentLevel == MAP_WORLDMAP)
		{
		//	SD_PlaySound(KITTENEXPLODESND);
			CK_RemoveObj(ob);	// remove the JediBoot object
		//	CK_HALDialog();
			CK_MiscDialog(-1);
		}
		
		else if (ck_gameState.currentLevel == MAP_MUSHROOMLEVEL2 && !ck_gameState.cheated)
		{
			ck_levelState = LS_SecretTrigger;
		}
		
		else
		{
		}
	}
}

void KeenSmokeThink(CK_object *ob)
{
	if ((ob->actionTimer -= tics) <= 0)
	{
		if (ob->gfxChunk)
		{
			ob->actionTimer += 10;
			if (++ob->gfxChunk > SMIRKYPOOF5SPR)
			{
				ob->gfxChunk = 0;
				ob->actionTimer += 1*TickBase;	// level ends 1 second after smoke is gone
				ck_gameState.numShots = 0;	// just in case...
			}
		}
		else
		{
			ck_levelState = LS_MushroomTrigger;
		}
	}
	
	// make Keen explode like an ammo pinata:
	if (ck_gameState.numShots > 0 && (ob->user1 -= tics) <= 0)
	{
		CK_object *gun;
		
		ob->user1 += 8;
		
		if ((ck_gameState.numShots -= 5) < 0)
			ck_gameState.numShots = 0;
		
		gun = CK_GetNewObj(true);
		gun->posX = ob->posX+8*PIXGLOBAL;
		gun->posY = ob->posY;
		gun->gfxChunk = ITEMSTUNNER1SPR;
		gun->velX = 16 + (US_RndT() >> 3);	// random value between 16 and 47
		gun->velY = -40 - (US_RndT() >> 4);	// random value between 32 and 47
		gun->zLayer = FOREGROUNDPRIORITY;
		gun->clipped = CLIP_not;
		CK_SetAction(gun, &CK_ACT_EggPiece);
	}
}

CK_action CK_ACT_JediBoot = {JEDIBOOTSPR, JEDIBOOTSPR, AT_Frame, false, push_none, 0, 0, 0, JediBootThink, JediBootContact, NULL, NULL};
CK_action CK_ACT_KeenSmoke = {0,0, AT_Frame, false, push_none, 0,0,0, KeenSmokeThink, NULL, CK_BasicDrawFunc1, NULL};


/*
==============================================================================
	SLUG CANS

	Static Object. Mainly eyecandy. Appears depending on a certain amount of 
	collected points.
	Cans are displayed in the BWB level, depending on how many cans have 
	already been collected. → 7, 11 or all 17
==============================================================================
*/

void CK_SpawnSlugCans(int tileX, int tileY)
{
	CK_object *ob;
	
	if (ck_gameState.slugcans >= 7 && ck_gameState.slugcans < 17)
	{
		ob = CK_SpawnStaticSprite(tileX, tileY, SLUGCANS1SPR, 4);
		ob->posX += -5*PIXGLOBAL;
		ob->posY += 37*PIXGLOBAL;
	}
	if (ck_gameState.slugcans >= 11)
	{
		ob = CK_SpawnStaticSprite(tileX, tileY, SLUGCANS3SPR, 4);
		ob->posX += 126*PIXGLOBAL;
		ob->posY += -13*PIXGLOBAL;
	}
	if (ck_gameState.slugcans >= 17)
	{
		ob = CK_SpawnStaticSprite(tileX, tileY, SLUGCANS2SPR, 4);
		ob->posX += -5*PIXGLOBAL;
		ob->posY += 7*PIXGLOBAL;
	}
}

/*
==============================================================================
	BIG SHROOM
	
	just eyecandy (spawning smoke clouds)
==============================================================================
*/

void CK_SpawnBigShroom(int tileX, int tileY)
{
	CK_object *ob;
	
	ob = CK_GetNewObj(false);
	ob->posX = RF_TileToUnit(tileX);
	ob->posY = RF_TileToUnit(tileY);
	ob->zLayer = FOREGROUNDPRIORITY-1;
	ob->gfxChunk = SMIRKYPOOF1SPR;
	ob->user1 = SMIRKYPOOF4SPR;
	ob->actionTimer = US_RndT() & 7;
	CK_SetAction(ob, &CK_ACT_Smoke);
}
