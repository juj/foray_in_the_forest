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

// KD_PHYS.C

#include "CK_DEF.H"
#pragma	hdrstop

/*
   =============================================================================

   LOCAL CONSTANTS

   =============================================================================
   */

// maximum difference between two adjacent positions on a slope, in global units
// (see ck_physSlopeHeight)

#define SLOPEMAX	16

#define MAXMOVE	(TILEGLOBAL-1-SLOPEMAX)
#define MAXFALLSPEED 70

// K1n9_Duk3 says:
// The maximum falling speed of 70 units per tic is a source for unintended
// behavior when the game is running at a very low frame rate. Since the
// clipping routines don't allow anything to move more than 1 tile per frame,
// this means that once the game takes 4 or more tics per frame (17.5 fps or
// less), the level objects don't fall as fast as they are supposed to.
// At 4 tics and 70 units per tic, the objects would need to move by 280 units,
// but the clipping routines only move it by MAXMOVE (or MAXMOVE+16) units.
// So since the objects don't fall as fast as they are supposed to, they can
// actually jump longer distances at low frame rates.
//
// A maximum falling speed of 48 would be the highest value you can use without
// running into issues like this, assuming the MINTICS value is set to 5 like
// in Keen 4-6. If MINTICS is 6, then the maximum speed would be 40.


/*
   =============================================================================

   GLOBAL VARIABLES

   =============================================================================
   */


/*
   =============================================================================

   LOCAL VARIABLES

   =============================================================================
   */



static int ck_physSlopeHeight[8][16] = {
	{ 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100 },
	{   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0 },
	{   0x0,   0x8,  0x10,  0x18,  0x20,  0x28,  0x30,  0x38,  0x40,  0x48,  0x50,  0x58,  0x60,  0x68,  0x70,  0x78 },
	{  0x80,  0x88,  0x90,  0x98,  0xa0,  0xa8,  0xb0,  0xb8,  0xc0,  0xc8,  0xd0,  0xd8,  0xe0,  0xe8,  0xf0,  0xf8 },
	{   0x0,  0x10,  0x20,  0x30,  0x40,  0x50,  0x60,  0x70,  0x80,  0x90,  0xa0,  0xb0,  0xc0,  0xd0,  0xe0,  0xf0 },
	{  0x78,  0x70,  0x68,  0x60,  0x58,  0x50,  0x48,  0x40,  0x38,  0x30,  0x28,  0x20,  0x18,  0x10,  0x08,  0x0  },
	{  0xF8,  0xF0,  0xE8,  0xE0,  0xD8,  0xD0,  0xC8,  0xC0,  0xB8,  0xB0,  0xA8,  0xA0,  0x98,  0x90,  0x88,  0x80 },
	{  0xF0,  0xE0,  0xD0,  0xC0,  0xB0,  0xA0,  0x90,  0x80,  0x70,  0x60,  0x50,  0x40,  0x30,  0x20,  0x10,  0x0  }
};

CK_objPhysDataU ck_oldRects;
CK_objPhysData ck_deltaRects;
int ck_nextX;
int ck_nextY;

extern CK_object *ck_keenObj;

// K1n9_Duk3 addition (for less redundancy):

void CK_CalcTileClipRects(CK_object *obj)
{
	obj->clipRects.unitXmid = (obj->clipRects.unitX2 - obj->clipRects.unitX1) / 2 + obj->clipRects.unitX1;

	obj->clipRects.tileX1 = RF_UnitToTile(obj->clipRects.unitX1);
	obj->clipRects.tileX2 = RF_UnitToTile(obj->clipRects.unitX2);
	obj->clipRects.tileY1 = RF_UnitToTile(obj->clipRects.unitY1);
	obj->clipRects.tileY2 = RF_UnitToTile(obj->clipRects.unitY2);

	obj->clipRects.tileXmid = RF_UnitToTile(obj->clipRects.unitXmid);
}

// end of addition

void CK_ResetClipRects(CK_object *obj)
{
	//NOTE: As tile rects are rarely used, keen does not calculate them here.
	spritetabletype far *spr = &spritetable[obj->gfxChunk - STARTSPRITES];

	obj->clipRects.unitX1 = obj->posX + spr->xl;
	obj->clipRects.unitX2 = obj->posX + spr->xh;
	obj->clipRects.unitY1 = obj->posY + spr->yl;
	obj->clipRects.unitY2 = obj->posY + spr->yh;

	CK_CalcTileClipRects(obj);	// K1n9_Duk3 mod (less redundancy)
}

// K1n9_Duk3 addition:

void CK_CustomClipRects(CK_object *obj)
{
	switch (obj->type)
	{
	case CT_SneakyTree:
		// Note: I would like to make the hitbox even more wide, but
		// that would cause problems on 45 degree slopes. A 32 pixel
		// wide hitbox seems to be the maximum width that is still
		// able to walk up on all slopes without getting stuck.
		// Most sprites in Keen 4-6 appear to have a hitbox width of
		// 32 pixels or less, including RoboRed. And those that do
		// have a bigger hitbox are either placed on flat ground or
		// don't walk/slide on the ground at all.
		// Maybe some tiles/attributes would need to be modified to
		// avoid this problem.
		obj->clipRects.unitX1 = obj->posX + 6*PIXGLOBAL;
		obj->clipRects.unitX2 = obj->posX + 38*PIXGLOBAL;
		obj->clipRects.unitY1 = obj->posY;
		obj->clipRects.unitY2 = obj->posY + 39*PIXGLOBAL;
		break;
	case CT_Crab:
		obj->clipRects.unitX1 = obj->posX + CRAB_X1;
		obj->clipRects.unitX2 = obj->posX + CRAB_X2;
		obj->clipRects.unitY1 = obj->posY + CRAB_Y1;
		obj->clipRects.unitY2 = obj->posY + CRAB_Y2;
		break;
	case CT_FallBlock:
		obj->clipRects.unitX1 = obj->posX + 8*PIXGLOBAL;
		obj->clipRects.unitX2 = obj->posX + 53*PIXGLOBAL;
		obj->clipRects.unitY1 = obj->posY + 16*PIXGLOBAL;
		obj->clipRects.unitY2 = obj->posY + 44*PIXGLOBAL;
		break;
	default:
		//Quit("CK_CustomClipRects: Bad obclass");
		SM_Quit(S_BADOBCLASS_CUSTOM);
	}

	CK_CalcTileClipRects(obj);
}

// end of addition

void CK_SetOldClipRects(CK_object *obj)
{
	ck_oldRects.unitX1 = obj->clipRects.unitX1;
	ck_oldRects.unitX2 = obj->clipRects.unitX2;
	ck_oldRects.unitY1 = obj->clipRects.unitY1;
	ck_oldRects.unitY2 = obj->clipRects.unitY2;

	ck_oldRects.unitXmid = obj->clipRects.unitXmid;

	ck_oldRects.tileX1 = obj->clipRects.tileX1;
	ck_oldRects.tileX2 = obj->clipRects.tileX2;
	ck_oldRects.tileY1 = obj->clipRects.tileY1;
	ck_oldRects.tileY2 = obj->clipRects.tileY2;

	ck_oldRects.tileXmid = obj->clipRects.tileXmid;
}

void CK_SetDeltaClipRects(CK_object *obj)
{
	ck_deltaRects.unitX1 = obj->clipRects.unitX1 - ck_oldRects.unitX1;
	ck_deltaRects.unitY1 = obj->clipRects.unitY1 - ck_oldRects.unitY1;
	ck_deltaRects.unitX2 = obj->clipRects.unitX2 - ck_oldRects.unitX2;
	ck_deltaRects.unitY2 = obj->clipRects.unitY2 - ck_oldRects.unitY2;

	ck_deltaRects.unitXmid = obj->clipRects.unitXmid - ck_oldRects.unitXmid;

	// We don't calculate tile deltas.
}


void CK_PhysUpdateX(CK_object *obj, int deltaUnitX)
{
	obj->posX += deltaUnitX;
	obj->clipRects.unitX1 += deltaUnitX;
	obj->clipRects.unitX2 += deltaUnitX;
	obj->clipRects.unitXmid += deltaUnitX;	// GARG!

	//Now update the tile rect
	// WARNING: Keen _didn't_ update tileXmid!

	obj->clipRects.tileX1 = RF_UnitToTile(obj->clipRects.unitX1);
	obj->clipRects.tileX2 = RF_UnitToTile(obj->clipRects.unitX2);
	obj->clipRects.tileXmid = RF_UnitToTile(obj->clipRects.unitXmid);	// GARG!
}

void CK_PhysUpdateY(CK_object *obj, int deltaUnitY)
{
	obj->posY += deltaUnitY;
	obj->clipRects.unitY1 += deltaUnitY;
	obj->clipRects.unitY2 += deltaUnitY;

	obj->clipRects.tileY1 = RF_UnitToTile(obj->clipRects.unitY1);
	obj->clipRects.tileY2 = RF_UnitToTile(obj->clipRects.unitY2);
}

void CK_PhysKeenClipDown(CK_object *obj)
{
	int slope;

	// Amount we're moving in each direction.
	int deltaX, deltaY;
	// The part of the slope we care about (in px)
	int midTileXOffset;
	// The top of the tile at Keen's feet.
	int topTI;
	// Is there space above the slope?
	boolean spaceAbove = false;

	// Performing some special clipping for Keen.

	// If we're moving right:
	if (obj->xDirection == 1)
	{
		// We care about the lefthand-most side of any slope we're about to touch.
		midTileXOffset = 0;
		deltaX = obj->clipRects.unitX2 - obj->clipRects.unitXmid;
		spaceAbove = (TI_ForeTop(CA_TileAtPos(obj->clipRects.tileX2, obj->clipRects.tileY2 - 1, 1)) == 0);
		topTI = TI_ForeTop(CA_TileAtPos(obj->clipRects.tileX2, obj->clipRects.tileY2, 1));

		// If we're being blocked by something, return.
		if (TI_ForeLeft(CA_TileAtPos(obj->clipRects.tileX2, obj->clipRects.tileY2 - 2, 1)) || TI_ForeLeft(CA_TileAtPos(obj->clipRects.tileX2, obj->clipRects.tileY2 - 1, 1)) || TI_ForeTop(CA_TileAtPos(obj->clipRects.tileX2, obj->clipRects.tileY2 - 1, 1)))
		{
			return;
		}
	}
	else // We're moving left:
	{
		// We care about the righthand-most side of the slope
		midTileXOffset = 15;
		deltaX = obj->clipRects.unitX1 - obj->clipRects.unitXmid;
		spaceAbove = (TI_ForeTop(CA_TileAtPos(obj->clipRects.tileX1, obj->clipRects.tileY2 - 1, 1)) == 0);
		topTI = TI_ForeTop(CA_TileAtPos(obj->clipRects.tileX1, obj->clipRects.tileY2, 1));

		// If we're being blocked, return.
		if (TI_ForeRight(CA_TileAtPos(obj->clipRects.tileX1, obj->clipRects.tileY2 - 2, 1)) || TI_ForeRight(CA_TileAtPos(obj->clipRects.tileX1, obj->clipRects.tileY2 - 1, 1)) || TI_ForeTop(CA_TileAtPos(obj->clipRects.tileX1, obj->clipRects.tileY2 - 1, 1)))
		{
			return;
		}
	}


	// If we're about to land on something flat:
	// TODO: This doesn't make any sense. Why would we make sure we're not
	// on a slope before doing slope calculations. Something fishy, methinks.
	if (spaceAbove && (topTI == 1))
	{
		slope = ck_physSlopeHeight[(topTI & 7)][midTileXOffset];
		deltaY = (obj->clipRects.tileY2 << 8) + slope - 1 - obj->clipRects.unitY2;
		if (deltaY <= 0 && -(ck_deltaRects.unitY2) <= deltaY)
		{
			obj->topTI = topTI;
			CK_PhysUpdateY(obj, deltaY);
			CK_PhysUpdateX(obj, deltaX);
		}
	}
}

void CK_PhysKeenClipUp(CK_object *obj)
{
	int deltaX, deltaY;
	int midTileXOffset;
	int bottomTI;
	boolean spaceBelow;
	int slopeAmt;

	if (obj->xDirection == 1)
	{
		midTileXOffset = 0;
		bottomTI = TI_ForeBottom(CA_TileAtPos(obj->clipRects.tileX2, obj->clipRects.tileY1, 1));
		spaceBelow = (TI_ForeBottom(CA_TileAtPos(obj->clipRects.tileX2, obj->clipRects.tileY1 + 1, 1)) == 0);
		if (TI_ForeLeft(CA_TileAtPos(obj->clipRects.tileX2, obj->clipRects.tileY1 + 2, 1)) || TI_ForeLeft(CA_TileAtPos(obj->clipRects.tileX2, obj->clipRects.tileY1 + 3, 1)) || TI_ForeBottom(CA_TileAtPos(obj->clipRects.tileX2, obj->clipRects.tileY1 + 1, 1)))
		{
			return;
		}
	}
	else
	{
		midTileXOffset = 15;
		bottomTI = TI_ForeBottom(CA_TileAtPos(obj->clipRects.tileX1, obj->clipRects.tileY1, 1));
		spaceBelow = (TI_ForeBottom(CA_TileAtPos(obj->clipRects.tileX1, obj->clipRects.tileY1 + 1, 1)) == 0);
		if (TI_ForeRight(CA_TileAtPos(obj->clipRects.tileX1, obj->clipRects.tileY1 + 2, 1)) || TI_ForeRight(CA_TileAtPos(obj->clipRects.tileX1, obj->clipRects.tileY1 + 3, 1)) || TI_ForeBottom(CA_TileAtPos(obj->clipRects.tileX1, obj->clipRects.tileY1 + 1, 1)))
		{
			return;
		}
	}

	if (spaceBelow && bottomTI)
	{
		slopeAmt = ck_physSlopeHeight[bottomTI & 0x07][midTileXOffset];
		deltaY = ((obj->clipRects.tileY1 + 1) << 8) - slopeAmt - obj->clipRects.unitY1;
		if (deltaY >= 0 && (-ck_deltaRects.unitY1) >= deltaY)
		{
			obj->bottomTI = bottomTI;
			CK_PhysUpdateY(obj, deltaY);
		}
	}

}

boolean CK_NotStuckInWall(CK_object *obj)
{
	unsigned y, x, tile;
	for (y = obj->clipRects.tileY1; y <= obj->clipRects.tileY2; ++y)
	{
		for (x = obj->clipRects.tileX1; x <= obj->clipRects.tileX2; ++x)
		{
			tile = CA_TileAtPos(x, y, 1);
			if (TI_ForeTop(tile) || TI_ForeRight(tile) || TI_ForeBottom(tile) || TI_ForeLeft(tile))
				return false;
		}
	}
	return true;
}

#if 0
static boolean near pascal CK_SlopesNearby(unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned walltype)
{
	unsigned far *map;
	unsigned mapskip, x;
	
	// swap coordinates if necessary to make sure x1, y1 is the top left corner
	// and x2, y2 ist the bottom right corner of the area we want to check
	if (x1 > x2)
	{
		x = x2;
		x2 = x1;
		x1 = x;
	}
	if (y1 > y2)
	{
		x = y2;
		y2 = y1;
		y1 = x;
	}
	
	// check that area for tiles with sloped edges:
	map = &CA_TileAtPos(x1, y1, 1);
	mapskip = mapwidth - (x2 - x1 + 1);
	
	for (; y1 <= y2; y1++, map += mapskip)
	{
		for (x = x1; x <= x2; x++, map++)
		{
			if ((tinf[walltype + *map] & SLOPEMASK) > 1)
				return true;
		}
	}
	
	return false;
}
#endif

void CK_PhysClipVert(CK_object *obj)
{


#if 0
	// K1n9_Duk3 note: This appears to be the Keen Dreams ClipToEnds() code,
	// modified to work with this code's naming scheme. I have not tested if
	// this works correctly.
	
	unsigned	far *map,tile,facetile,info,wall;
	int	leftpix,rightpix,midtiles,toppix,bottompix;
	int	x,y,clip,move,totalmove,maxmove,midxpix;

	midxpix = (ob->clipRects.unitXmid&0xf0) >> 4;

	maxmove = -abs(ck_deltaRects.unitXmid) - ck_deltaRects.unitY2 - SLOPEMAX;
	map = (unsigned far *)mapsegs[1] +
		mapbwidthtable[ck_oldRects.tileY2-1]/2 + ob->clipRects.tileXmid;
	for (y=ck_oldRects.tileY2-1 ; y<=ob->clipRects.tileY2 ; y++,map+=mapwidth)
	{
		if (wall = tinf[NORTHWALL+*map])
		{
			clip = ck_physSlopeHeight[wall&7][midxpix];
			move = ( (y<<G_T_SHIFT)+clip - 1) - ob->clipRects.unitY2;
			if (move<0 && move>=maxmove)
			{
				ob->topTI = wall;
				CK_PhysUpdateY (ob,move);
				return;
			}
		}
	}

	maxmove = abs(ck_deltaRects.unitXmid) - ck_deltaRects.unitY1 + SLOPEMAX;
	map = (unsigned far *)mapsegs[1] +
		mapbwidthtable[ck_oldRects.tileY2+1]/2 + ob->clipRects.tileXmid;
	for (y=ck_oldRects.tileY1+1 ; y>=ob->clipRects.tileY1 ; y--,map-=mapwidth)
	{
		if (wall = tinf[SOUTHWALL+*map])
		{
			clip = ck_physSlopeHeight[wall&7][midxpix];
			move = ( ((y+1)<<G_T_SHIFT)-clip ) - ob->clipRects.unitY1;
			if (move > 0 && move<=maxmove)
			{
				totalmove = ob->deltaPosY + move;
				if (totalmove < TILEGLOBAL && totalmove > -TILEGLOBAL)
				{
					ob->bottomTI = wall;
					CK_PhysUpdateY (ob,move);
				}
			}
		}
	}
#endif

	// K1n9_Duk3 note: This appears to be based on Omnispeak/NetKeen or some
	// other decompilation effort. I have modified the code below to make it
	// a little faster and/or smaller. Instead of storing the tile number in
	// a variable and looking up its top/bottom edge in the tile attributes
	// every time, I made the code look up the edge setting right away and
	// store it in a variable. I also made the ceiling check store the
	// result of 'slopeAmt - objYOffset' in variable dy and use then use dy
	// in the if-checks instead of re-calculating 'slopeAmt - objYOffset'
	// every single time.
	
	int y, x, wall, slopeAmt, objYOffset, dy;

	int midTileXOffset = (obj->clipRects.unitXmid >> 4) & 0x0F;

	// Checking collisions with the floor
	// The maximum amount we can move upwards due to tile clipping (i.e., being pushed up by a 45 degree slope)
	// is equal to the horizontal displacement, so add this to the vertical displacement (+ 1 pixel)
#if 1
	// old code:
	int vertDisplace = -abs(ck_deltaRects.unitXmid) - ck_deltaRects.unitY2 - SLOPEMAX;	// Move above slope first.
#else
	// FITF version:
	int vertDisplace = -ck_deltaRects.unitY2;
	
	// K1n9_Duk3 note:
	// The additional adjustment (+/- SLOPEMAX) for the vertical displacement is
	// necessary for 45 degree slopes. If an object moves by 1 global unit along
	// the x axis, this can already move the center position to a different pixel
	// position and result in a floor/ceiling that is one pixel higher/lower than
	// the one at the old position, therefore the code must be allowed to move
	// the object by SLOPEMAX units more than the x-movement to prevent the
	// object from moving through the sloped floor/ceiling.
	//
	// The problem with the old code is that the additional vertical displacement
	// is ALWAYS allowed, even when there are no slope tiles nearby. This can
	// cause some jumps to be cut short prematurely (the object hits the floor
	// while it is still moving up). This problem was present in the original
	// Keens as well, but the limited frame rate made it less likely to pop up.
	
	if (CK_SlopesNearby(obj->clipRects.tileXmid, obj->clipRects.tileY2, ck_oldRects.tileXmid, ck_oldRects.tileY2, NORTHWALL))
	{
		vertDisplace -= abs(ck_deltaRects.unitXmid) + SLOPEMAX;
	}
#endif

	// Now check all tiles in a vertical strip over the new xMid tile, within the
	// sprite's Y boundaries for top blocking-ness, starting from top to bottom

	// For a sprite to collide with a sloped floor, it has to be moved underneath the floor first,
	for (y = ck_oldRects.tileY2 - 1; y <= obj->clipRects.tileY2; ++y)
	{
		wall = TI_ForeTop(CA_TileAtPos(obj->clipRects.tileXmid, y, 1));
		if (wall)
		{
			objYOffset = obj->clipRects.unitY2 - RF_TileToUnit(y);
			slopeAmt = ck_physSlopeHeight[wall & SLOPEMASK][midTileXOffset];

			// dy = the amount to move the sprite vertically to set it one global unit
			// above the slope
			dy = (slopeAmt - objYOffset) - 1;
			if ((dy < 0) && (dy >= vertDisplace))
			{
				obj->topTI = wall;
				CK_PhysUpdateY(obj, dy);
				return;
			}
		}
	}


	// Checking collisions with ceiling
#if 1
	// old code:
	vertDisplace = abs(ck_deltaRects.unitXmid) - ck_deltaRects.unitY1 + SLOPEMAX;
#else
	// FITF version:
	vertDisplace = -ck_deltaRects.unitY1;

	if (CK_SlopesNearby(obj->clipRects.tileXmid, obj->clipRects.tileY1, ck_oldRects.tileXmid, ck_oldRects.tileY1, SOUTHWALL))
	{
		vertDisplace += abs(ck_deltaRects.unitXmid) + SLOPEMAX;
	}
#endif

	for (y = ck_oldRects.tileY1 + 1; y >= (signed int)obj->clipRects.tileY1; --y)	// K1n9_Duk3 fix: cast to signed int to avoid infinite loops when tileY1 is 0
	{
		wall = TI_ForeBottom(CA_TileAtPos(obj->clipRects.tileXmid, y, 1));
		if (wall)
		{
			objYOffset = obj->clipRects.unitY1 - RF_TileToUnit(y + 1);
			slopeAmt = -ck_physSlopeHeight[wall & SLOPEMASK][midTileXOffset];
			
			dy = slopeAmt - objYOffset;
			// Only move the object if movement doesn't exceed the maximum displacement
			// and doesn't cause the object to move by more than one tile in total
			if ((dy > 0) && (dy <= vertDisplace) && (ck_nextY + dy < TILEGLOBAL) && (ck_nextY + dy > -TILEGLOBAL))
			{
				obj->bottomTI = wall;
				CK_PhysUpdateY(obj, dy);
			}
		}
	}
}


void CK_PhysClipHorz(register CK_object *obj)
{
	int tileY1 = obj->clipRects.tileY1;
	register int tileY2 = obj->clipRects.tileY2;
	register int y;

	// K1n9_Duk3 note: If the low byte of unitY2 is 0xFF, then the bottom end of
	// the hitbox is directly above the tile the object is standing on and this
	// code should NOT skip checking the bottom-most tile. Similarly for the top
	// edge, if the low byte of unitY1 is 0, then the object is directly below
	// the ceiling and this code should NOT skip checking the top-most tile.
	// Checking the low byte is necessary for Keen at tics >= 5.
	
	// ignore blocking sides at the bottom when bottom of hitbox is touching a slope
	if ((obj->topTI & SLOPEMASK) > 1 && ((byte)obj->clipRects.unitY2) != 0xFF || (obj == ck_keenObj && ck_mapState.quicksand))	//If we're on a slope (or Keen is walking in quicksand)
	{
		tileY2--;	// don't check bottom-most tile
	}
	// ignore blocking sides at the top when top of hitbox is touching a slope
	if ((obj->bottomTI & SLOPEMASK) > 1 && ((byte)obj->clipRects.unitY1) != 0)
	{
		tileY1++;	// don't check top-most tile
	}

#if 0
	// original code (always checks tileX1 first):
	
	//Check if our left side is intersecting with a wall.
	for (y = tileY1; y <= tileY2; ++y)
	{
		// K1n9_Duk3 fix: only overwrite rightTI when touching a blocking tile
		if ((_AX = TI_ForeRight(CA_TileAtPos(obj->clipRects.tileX1, y, 1))) != 0)
		{
			obj->rightTI = _AX;
			// Clipping right
			// Push us right until we're no-longer intersecting.
			CK_PhysUpdateX(obj, ((obj->clipRects.tileX1 + 1) << 8) - obj->clipRects.unitX1);
			return;
		}
	}

	//Similarly for the right side (left side of intersecting tile).
	for (y = tileY1; y <= tileY2; ++y)
	{
		// K1n9_Duk3 fix: only overwrite leftTI when touching a blocking tile
		if ((_AX = TI_ForeLeft(CA_TileAtPos(obj->clipRects.tileX2, y, 1))) != 0)
		{
			obj->leftTI = _AX;
			// Push us left until we're no-longer intersecting.
			CK_PhysUpdateX(obj, (obj->clipRects.tileX2 << 8) - 1 - obj->clipRects.unitX2);
			return;

		}
	}
#else
	// FITF version:
	{
		byte right, left;
		
		// Check if the left or right end of the object's hitbox is inside a tile
		// with a solid (blocking) edge:
		for (right = left = 0; tileY1 <= tileY2; ++tileY1)
		{
			// We could just bitwise-or the new edge values into the variables,
			// but a) this might cause problems with drillable walls and b) it's
			// probably faster to check the variable and skip reading the tile and
			// its attribute when we already found a blocking wall on that side.
			if (right == 0)
				right = TI_ForeRight(CA_TileAtPos(obj->clipRects.tileX1, tileY1, 1));
			if (left == 0)
				left = TI_ForeLeft(CA_TileAtPos(obj->clipRects.tileX2, tileY1, 1));
		}
		
		// We'll check the walls in different order depending on which direction
		// the object has moved. The assumption is that preventing the object from
		// moving into blocking tiles is more important than pushing the object
		// out of any blocking tiles it is trying to leave.
		
		// If the object has not moved to the right, check the left end of the
		// hitbox first, like the old code did:
		if (ck_deltaRects.unitXmid <= 0 && right)
		{
hit_right:
			// The left end of the object's hitbox is inside a tile with a solid
			// right edge, so move the object out of that tile (push to the right):
			obj->rightTI = right;
			CK_PhysUpdateX(obj, ((obj->clipRects.tileX1 + 1) << 8) - obj->clipRects.unitX1);
			return;
		}
		else if (left)
		{
			// The right end of the object's hitbox is inside a tile with a solid
			// left edge, so move the object out of that tile (push to the left):
			obj->leftTI = left;
			CK_PhysUpdateX(obj, (obj->clipRects.tileX2 << 8) - 1 - obj->clipRects.unitX2);
			return;
		}
		else if (right)
		{
			goto hit_right;
		}
	}
#endif
}

//TODO: Finish

void CK_PhysUpdateNormalObj(CK_object *obj)
{
	int oldUnitX, oldUnitY;
	CK_PushType wasPushed = push_none;
	oldUnitX = obj->posX;
	oldUnitY = obj->posY;
	
	switch (obj->currentAction->stickToGround)
	{
	case push_down:
		// If the object is resting on a platform
		if (obj->topTI == 0x19)
		{
			//ck_nextY = 145;	// K1n9_Duk3 mod: we don't need this
			
			// Note: This code is not really required for KEEN 4-6. If you go back
			// to the KEEN DREAMS source, you will see that it has no eqivalent to
			// the CK_KeenRidePlatform() routine. Instead, the ClipToSprite() code
			// was responsible for moving Keen sideways when he was standing on a
			// moving Canteloupe Cart in that game. That would make it necessary to
			// push Keen down into the other object if that object was moving up or
			// down, otherwise the objects would no longer touch each other and the
			// ClipToSprite() code would not be executed.
			//
			// Setting ck_nextY to a relatively high value in here was probably
			// just an intermediate step in development, before the new routine
			// CK_KeenRidePlatform() was implemented. But it might also have been
			// intended to allow other objects besides the player character to
			// stand on top of another game object.
		}
		else
		{
			// Push the object down by a little more than the amount that it moves
			// along the x-axis to make sure it sticks to the ground on 45° slopes and
			// doesn't end up floating in the air.
			if (ck_nextX > 0)
			{
				ck_nextY = (ck_nextX) + SLOPEMAX;
			}
			else
			{
				ck_nextY = -(ck_nextX) + SLOPEMAX;
			}
			wasPushed = push_down;
		}
		break;
	case push_up:
		// Push the object up by a little more than the amount that it moves along
		// the x-axis to make sure it sticks to the ceiling on 45° slopes and does
		// not end up floating in the air.
		if (ck_nextX > 0)
		{
			ck_nextY = -(ck_nextX) - SLOPEMAX;
		}
		else
		{
			ck_nextY = (ck_nextX) - SLOPEMAX;
		}
		wasPushed = push_up;
		break;
	case push_side:
		if (obj->xDirection > 0)
		{
			ck_nextX = motion_Right * SLOPEMAX;
			wasPushed = push_right;
		}
		else
		{
			ck_nextX = motion_Left * SLOPEMAX;
			wasPushed = push_left;
		}
		break;
	case push_left:
		ck_nextX = motion_Left * SLOPEMAX;
		wasPushed = push_left;
		break;
	case push_right:
		ck_nextX = motion_Right * SLOPEMAX;
		wasPushed = push_right;
		break;
	}

	if (ck_nextX > MAXMOVE)
	{
		ck_nextX = MAXMOVE;
	}
	else if (ck_nextX < -MAXMOVE)
	{
		ck_nextX = -MAXMOVE;
	}

	if (ck_nextY > MAXMOVE+SLOPEMAX)	// +SLOPEMAX because we also add SLOPEMAX for stickToGround
	{
		ck_nextY = MAXMOVE+SLOPEMAX;	// +SLOPEMAX because we also add SLOPEMAX for stickToGround
	}
	else if (ck_nextY < -(MAXMOVE+SLOPEMAX))	// +SLOPEMAX for sticking to ceiling
	{
		ck_nextY = -(MAXMOVE+SLOPEMAX);	// +SLOPEMAX for sticking to ceiling
	}

	if (obj->clipped == CLIP_custom)	// K1n9_Duk3 addition
		CK_CustomClipRects(obj);

	obj->posX += ck_nextX;
	obj->posY += ck_nextY;


	obj->visible = true;

	if (obj->gfxChunk)
	{
		CK_SetOldClipRects(obj);

retry:
		if (obj->clipped == CLIP_custom)	// K1n9_Duk3 addition
			CK_CustomClipRects(obj);
		else
			CK_ResetClipRects(obj);

		//Reset the tile clipping vars.
		obj->topTI = 0;
		obj->bottomTI = 0;
		obj->leftTI = 0;
		obj->rightTI = 0;

		if (obj->clipped)
		{
			unsigned oldXmid;	// K1n9_Duk3 addition
			
			//NOTE: Keen calculates a clip-rect delta here.

			CK_SetDeltaClipRects(obj);

			CK_PhysClipVert(obj);

			// Set keen on top of a flat tile in case he's just about to miss
			// landing on it. But only if Keen isn't getting pushed around, so
			// Keen can actually get pushed off egdes reliably.
			if (obj == ck_keenObj && !ck_mapState.keenGetsPushed)
			{
				if (!obj->topTI && ck_deltaRects.unitY2 > 0)
				{
					CK_PhysKeenClipDown(obj);
				}
				if (!obj->bottomTI && ck_deltaRects.unitY1 < 0)
				{
					CK_PhysKeenClipUp(obj);
				}
			}
			
			oldXmid = obj->clipRects.unitXmid;	// K1n9_Duk3 addition
			
			CK_PhysClipHorz(obj);
			
			// K1n9_Duk3 addition: Compare current and old Xmid position. If the
			// object was moved sideways, redo vertical clipping to make sure the
			// topTI and bottomTI values are set correctly and the object doesn't
			// end up thinking it hit a floor (or ceiling) when that's no longer
			// the case at the corrected x position. This is important if the tile
			// next to the wall doesn't have a flat surface.
			//
			// KEEN 6 is the only one of the original games that has tiles with
			// sloped top surfaces right next to walls in some of its levels (6, 7
			// and 8) and it had extra code that attempted to fix this problem.
			// I found the KEEN 6 solution insufficient for the FITF levels, so I
			// came up with this:
			if (oldXmid != obj->clipRects.unitXmid)
			{
				obj->topTI = obj->bottomTI = 0;
				// back to y position this object had before clipping
				CK_PhysUpdateY(obj, oldUnitY + ck_nextY - obj->posY);
				// and clip to floor/ceiling
				CK_SetDeltaClipRects(obj);
				CK_PhysClipVert(obj);
			}
			// end of addition			
			
			if (obj->clipped == CLIP_custom)	// K1n9_Duk3 addition
				CK_ResetClipRects(obj);

		}

		// Check if the object is no longer touching the surface it was supposed
		// to stick to:

		if ((!obj->topTI && wasPushed==push_down) || (!obj->bottomTI && wasPushed==push_up)
			|| (!obj->leftTI && wasPushed==push_right) || (!obj->rightTI && wasPushed==push_left))
		{
			// The object was pushed down to make it stick to the ground on 45° slopes
			// but now it's not touching the ground anymore. That means we should undo
			// the pushing down part (keep oldUnitY and DON'T add ck_nextY)!
#if 1
			// FITF code:
			if (wasPushed <= push_up)
			{
				// object was pushed up or down, so keep x movement and undo y movement
				obj->posX = oldUnitX + ck_nextX;
				obj->posY = oldUnitY;
			}
			else
			{
				// object was pushed left or right, so keep y movement and undo x movement
				obj->posX = oldUnitX;
				obj->posY = oldUnitY + ck_nextY;
			}
			
			// K1n9_Duk3 note: I am going to make sure the movement is clipped.
			// Most actors that use stickToGround will undo the movement in their
			// react code when they are no longer touching the floor/ceiling/wall,
			// so it wouldn't matter if the movement was clipped or not, but this
			// is not the case for Keen himself.
			
			wasPushed = push_none;
			goto retry;	// make sure the movement is clipped!
			//CK_ResetClipRects(obj);
#else
			// CKSOURCEMOD code:
			obj->posX = oldUnitX + ck_nextX;
			obj->posY = oldUnitY + ck_nextY; // Note: Vanilla Keen does not have the +ck_nextY,
							 // but I've added it, as it stops walking sprites from
							 // floating into the air when they should be
							 // moving down a slope

			// K1n9_Duk3 says: If objects end up floating in the air when they should be
			// walking down a slope, then there is an error in the way the sprite's
			// hitboxes are designed and that is what causes the object to float.
			// Use the Sprite Test debug command and look at the sprites in question.
			// The value for the bottom of the hitbox (yh) must be the same for all
			// frames of the walking animation. The middle of the hitbox (xl+(xh-xl)/2)
			// also has to have the same value for all the walking sprites.
			//
			// The "fix" above means that floating ground-based objects will be pushed
			// down a little bit every frame until they eventually touch a tile that
			// they can stand on or they fall out of the level. It does NOT fix the
			// problem, since the object will still be floating for at least one frame,
			// which will cause it to turn around or do whatever else it was programmed
			// to do when it no longer walks on solid ground. This also means that some
			// objects won't be able to turn around correctly when they reach an edge,
			// because the code that lets them turn around only resets the x position
			// and not the y position, causing the object to be partially stuck inside
			// a tile.
			//
			// And to top it all off, this code adds movement without clipping it.
			//
			// That's why I don't use this version of the code in Foray in the Forest.
			
			CK_ResetClipRects(obj);
#endif
		}

		obj->deltaPosX += obj->posX - oldUnitX;
		obj->deltaPosY += obj->posY - oldUnitY;
	}
}

//TODO: Finish Implementing!!!!

void CK_PhysFullClipToWalls(CK_object *obj)
{
	int delX, delY;

	int oldUnitX = obj->posX;
	int oldUnitY = obj->posY;

	if (ck_nextX > MAXMOVE)
		ck_nextX = MAXMOVE;
	else if (ck_nextX < -MAXMOVE)
		ck_nextX = -MAXMOVE;
	if (ck_nextY > MAXMOVE)
		ck_nextY = MAXMOVE;
	else if (ck_nextY < -MAXMOVE)
		ck_nextY = -MAXMOVE;

	obj->posX += ck_nextX;
	obj->posY += ck_nextY;
	obj->visible = true;

	//TODO: Verify object class (need callback to episode struct)
	/*
	if ((obj->type != CT_SliceStar) && (obj->type != CT_Sphereful))
	{
		Quit("FullClipToWalls: Bad obclass");
	}
	*/

	switch (obj->type)
	{
/*
	// Just in case anybody wants to use full clipping with the log objects in FITF:
	case CT_LogPlatform:
	case CT_OccupiedLog:
		delX = 38*PIXGLOBAL-1;
		delY = 19*PIXGLOBAL-1;
		break;
*/
	default:
		delX = 512;
		delY = 512;
		break;
	}

	obj->clipRects.unitX2 = obj->posX + delX;
	obj->clipRects.unitX1 = obj->posX;
	obj->clipRects.unitY1 = obj->posY;
	obj->clipRects.unitY2 = obj->posY + delY;
	obj->clipRects.tileX1 = RF_UnitToTile(obj->clipRects.unitX1);
	obj->clipRects.tileX2 = RF_UnitToTile(obj->clipRects.unitX2);
	obj->clipRects.tileY1 = RF_UnitToTile(obj->clipRects.unitY1);
	obj->clipRects.tileY2 = RF_UnitToTile(obj->clipRects.unitY2);
	//Reset the tile clipping vars.
	obj->topTI = 0;
	obj->bottomTI = 0;
	obj->leftTI = 0;
	obj->rightTI = 0;

	if (!CK_NotStuckInWall(obj))
	{
		CK_PhysUpdateX(obj, -ck_nextX);
		if (CK_NotStuckInWall(obj))
		{
			if (ck_nextX > 0)
				obj->leftTI = 1;
			else
				obj->rightTI = 1;
		}
		else
		{
			if (ck_nextY > 0)
				obj->topTI = 1;
			else
				obj->bottomTI = 1;

			CK_PhysUpdateX(obj, ck_nextX);
			CK_PhysUpdateY(obj, -ck_nextY);
			if (!CK_NotStuckInWall(obj))
			{
				CK_PhysUpdateX(obj, -ck_nextX);
				if (ck_nextX > 0)
					obj->leftTI = 1;
				else
					obj->rightTI = 1;
			}
		}
	}
	obj->deltaPosX += (obj->posX - oldUnitX);
	obj->deltaPosY += (obj->posY - oldUnitY);

	CK_ResetClipRects(obj);
}

void CK_PhysUpdateSimpleObj(CK_object *obj)
{
	int oldUnitX, oldUnitY;
	oldUnitX = obj->posX;
	oldUnitY = obj->posY;


	obj->posX += ck_nextX;
	obj->posY += ck_nextY;


	obj->visible = true;

	if (obj->gfxChunk)
	{

		CK_SetOldClipRects(obj);

		CK_ResetClipRects(obj);

		//Reset the tile clipping vars.
		//obj->topTI = 0;
		//obj->bottomTI = 0;
		//obj->leftTI = 0;
		//obj->rightTI = 0;

		if (obj->clipped)
		{
			CK_SetDeltaClipRects(obj);
			CK_PhysClipVert(obj);
			CK_PhysClipHorz(obj);
		}

		obj->deltaPosX += obj->posX - oldUnitX;
		obj->deltaPosY += obj->posY - oldUnitY;
	}

}

void CK_PhysPushX(CK_object *pushee, CK_object *pusher)
{
	int deltaPosX = pusher->deltaPosX - pushee->deltaPosX;
	int deltaX_1 = pusher->clipRects.unitX2 - pushee->clipRects.unitX1;
	int deltaX_2 = pushee->clipRects.unitX2 - pusher->clipRects.unitX1;

	ck_nextY = 0;	// K1n9_Duk3 fix: don't push up or down unless we need to stickToGround!

	// Push object to the right
	if ((deltaX_1 > 0) && (deltaX_1 <= deltaPosX))
	{
		ck_nextX = deltaX_1;
		if (pushee->currentAction->stickToGround)
			ck_nextY = deltaX_1 + SLOPEMAX;	// K1n9_Duk3 fix: it's deltaX_1, not -deltaX_1
		CK_PhysUpdateNormalObj(pushee);
		pushee->rightTI = 1;
		return;
	}

	// Push object to the left
	if ((deltaX_2 > 0) && (-deltaPosX >= deltaX_2))
	{
		ck_nextX = -deltaX_2;
		if (pushee->currentAction->stickToGround)
			ck_nextY = deltaX_2 + SLOPEMAX;
		CK_PhysUpdateNormalObj(pushee);
		pushee->leftTI = 1;
		return;
	}
}

/*
void CK_PhysPushY(CK_object *pushee, CK_object *pusher)
{
	boolean pusheeSticksToGround;
	int deltaDeltaY = pushee->deltaPosY - pusher->deltaPosY;
	int deltaClipY = pushee->clipRects.unitY2 - pusher->clipRects.unitY1;

	ck_nextX = 0;	// K1n9_Duk3 fix: don't push left/right!

	if ((deltaClipY >= 0) && (deltaClipY <= deltaDeltaY))
	{
		//If the pushee is keen, set ck_keenState.currentPlatform to pusher
		// (I'm not sure I like this)
		if (pushee == ck_keenObj)
			ck_keenState.platform = pusher;

		ck_nextY = -deltaClipY;
		pusheeSticksToGround = pushee->currentAction->stickToGround;
		pushee->currentAction->stickToGround = false;
		CK_PhysUpdateNormalObj(pushee);
		pushee->currentAction->stickToGround = pusheeSticksToGround;

		if (!pushee->bottomTI)
			pushee->topTI = 0x19; // Platform?
	}
}

void CK_ClipToSprite(CK_object *push, CK_object *solid, boolean squish)
{
	//
	// slightly modified Keen 4-6 code:
	//
	int xmove, ymove, leftinto, rightinto, topinto, bottominto;

	xmove = solid->deltaPosX - push->deltaPosX;
	ck_nextX = ck_nextY = 0;
	leftinto = solid->clipRects.unitX2 - push->clipRects.unitX1;
	rightinto = push->clipRects.unitX2 - solid->clipRects.unitX1;
	if (leftinto > 0 && xmove+1 >= leftinto)
	{
_left:
		ck_nextX = leftinto;
		push->velX = 0;
		CK_PhysUpdateSimpleObj(push);
		if (squish && push->leftTI)
		{
			CK_KillKeenEx(true);	// always kill, ignore invincibility
		}
		push->rightTI = 1;
		return;
	}
	else if (rightinto > 0 && -xmove+1 >= rightinto)
	{
_right:
		ck_nextX = -rightinto;
		push->velX = 0;
		CK_PhysUpdateSimpleObj(push);
		if (squish && push->rightTI)
		{
			CK_KillKeenEx(true);	// always kill, ignore invincibility
		}
		push->leftTI = 1;
		return;
	}
	ymove = push->deltaPosY - solid->deltaPosY;
	topinto = solid->clipRects.unitY2 - push->clipRects.unitY1;
	bottominto = push->clipRects.unitY2 - solid->clipRects.unitY1;
	if (bottominto >= 0 && bottominto <= ymove)
	{
_bottom:
		if (push == ck_keenObj)
		{
			ck_keenState.platform = solid;
		}
		ck_nextY = -bottominto;
		CK_PhysUpdateSimpleObj(push);
		if (squish && push->bottomTI)
		{
			CK_KillKeenEx(true);	// always kill, ignore invincibility
		}
		if (!push->bottomTI)
		{
			push->topTI = 25;
		}
		return;
	}
	else if (topinto >= 0 && topinto <= -ymove)	// Keen 4-6 used '<= ymove' here, which didn't work
	{
_top:
		ck_nextY = topinto;
		CK_PhysUpdateSimpleObj(push);
		if (squish && push->topTI)
		{
			CK_KillKeenEx(true);	// always kill, ignore invincibility
		}
		push->bottomTI = 25;
		return;
	}

	//
	// K1n9_Duk3 addition:
	// always pushes push out of solid, taking the shortest possible path
	//
#define mininto xmove
#define dir ymove
	mininto = 0x7FFF;
	dir = 4;
	if (leftinto > 0 && leftinto < mininto)
	{
		mininto = leftinto;
		dir = 0;
	}
	if (rightinto > 0 && rightinto < mininto)
	{
		mininto = rightinto;
		dir = 1;
	}
	if (bottominto > 0 && bottominto < mininto)
	{
		mininto = bottominto;
		dir = 2;
	}
	if (topinto > 0 && topinto < mininto)
	{
		mininto = topinto;
		dir = 3;
	}

	switch (dir)
	{
	case 0:
		goto _left;
	case 1:
		goto _right;
	case 2:
		goto _bottom;
	case 3:
		goto _top;
	}
#undef mininto
#undef dir
}
*/

void CK_PhysPushY(CK_object *pushee, CK_object *pusher)
{
	boolean pusheeSticksToGround;
	int deltaDeltaY = pushee->deltaPosY - pusher->deltaPosY;
	int deltaClipY = pushee->clipRects.unitY2 - pusher->clipRects.unitY1;

	ck_nextX = 0;	// K1n9_Duk3 fix: don't push left/right!

	if ((deltaClipY >= 0) && (deltaClipY <= deltaDeltaY))
	{
		//If the pushee is keen, set ck_keenState.currentPlatform to pusher
		// (I'm not sure I like this)
		if (pushee == ck_keenObj)
			ck_keenState.platform = pusher;

		ck_nextY = -deltaClipY - 16;	// K1n9_Duk3 mod: - 16 for consistency with CK_KeenRidePlatform()
		pusheeSticksToGround = pushee->currentAction->stickToGround;
		pushee->currentAction->stickToGround = false;
		CK_PhysUpdateNormalObj(pushee);
		pushee->currentAction->stickToGround = pusheeSticksToGround;

		if (!pushee->bottomTI)
			pushee->topTI = 0x19; // Platform?
	}
}

void CK_ClipToSprite(CK_object *push, CK_object *solid, boolean squish)
{
	//
	// slightly modified Keen 4-6 code:
	//
	int xmove, ymove, leftinto, rightinto, topinto, bottominto;
	CK_object temp;	// K1n9_Duk3 addition

	xmove = solid->deltaPosX - push->deltaPosX;
	ck_nextX = ck_nextY = 0;
	leftinto = solid->clipRects.unitX2 - push->clipRects.unitX1;
	rightinto = push->clipRects.unitX2 - solid->clipRects.unitX1;
	if (leftinto > 0 && xmove+1 >= leftinto)
	{
_left:
		ck_nextX = leftinto;
		push->leftTI = push->velX = 0;	// K1n9_Duk3 mod: also clear leftTI
		temp = *push;	// K1n9_Duk3 addition: backup old values
		CK_PhysUpdateSimpleObj(push);
		if (squish && push->leftTI)
		{
			// K1n9_Duk3 hack: undo Y movement and push again:
			*push = temp;
			ck_nextY = -push->deltaPosY;
			CK_PhysUpdateSimpleObj(push);
			if (push->leftTI)
			{
				CK_KillKeenEx(true);	// always kill, ignore invincibility
			}
			else
			{
				// redo Y movement only
				ck_nextX = 0;
				ck_nextY = -ck_nextY;
				CK_PhysUpdateSimpleObj(push);
			}
		}
		push->rightTI = 1;
		return;
	}
	else if (rightinto > 0 && -xmove+1 >= rightinto)
	{
_right:
		ck_nextX = -rightinto;
		push->rightTI = push->velX = 0;	// K1n9_Duk3 mod: also clear rightTI
		temp = *push;	// K1n9_Duk3 addition: backup old values
		CK_PhysUpdateSimpleObj(push);
		if (squish && push->rightTI)
		{
			// K1n9_Duk3 hack: undo Y movement and push again:
			*push = temp;
			ck_nextY = -push->deltaPosY;
			CK_PhysUpdateSimpleObj(push);
			if (push->rightTI)
			{
				CK_KillKeenEx(true);	// always kill, ignore invincibility
			}
			else
			{
				// redo Y movement only
				ck_nextX = 0;
				ck_nextY = -ck_nextY;
				CK_PhysUpdateSimpleObj(push);
			}
		}
		push->leftTI = 1;
		return;
	}
	ymove = push->deltaPosY - solid->deltaPosY;
	topinto = solid->clipRects.unitY2 - push->clipRects.unitY1;
	bottominto = push->clipRects.unitY2 - solid->clipRects.unitY1;
	if (bottominto >= 0 && bottominto <= ymove)
	{
_bottom:
		if (push == ck_keenObj)
		{
			ck_keenState.platform = solid;
		}
		ck_nextY = -bottominto - 16;	// K1n9_Duk3 mod: - 16 for consistency with CK_KeenRidePlatform()
		CK_PhysUpdateSimpleObj(push);
		if (squish && push->bottomTI)
		{
			CK_KillKeenEx(true);	// always kill, ignore invincibility
		}
		if (!push->bottomTI)
		{
			push->topTI = 25;
		}
		return;
	}
	else if (topinto >= 0 && topinto <= -ymove)	// Keen 4-6 used '<= ymove' here, which didn't work
	{
_top:
		ck_nextY = topinto;
		CK_PhysUpdateSimpleObj(push);
		if (squish && push->topTI)
		{
			CK_KillKeenEx(true);	// always kill, ignore invincibility
		}
		push->bottomTI = 25;
		return;
	}

	//
	// K1n9_Duk3 addition:
	// always pushes push out of solid, taking the shortest possible path
	//
#define mininto xmove
#define dir ymove
	mininto = 0x7FFF;
	dir = 4;
	if (leftinto > 0 && leftinto < mininto)
	{
		mininto = leftinto;
		dir = 0;
	}
	if (rightinto > 0 && rightinto < mininto)
	{
		mininto = rightinto;
		dir = 1;
	}
	if (bottominto > 0 && bottominto < mininto)
	{
		mininto = bottominto;
		dir = 2;
	}
	if (topinto > 0 && topinto < mininto)
	{
		mininto = topinto;
		dir = 3;
	}

	switch (dir)
	{
	case 0:
		goto _left;
	case 1:
		goto _right;
	case 2:
		goto _bottom;
	case 3:
		goto _top;
	}
#undef mininto
#undef dir
}

void CK_SetAction(CK_object *obj, CK_action *act)
{
	CK_ClipType oldClipping;

	obj->currentAction = act;

	if (act->chunkRight && obj->xDirection > 0)
		obj->gfxChunk = act->chunkRight;
	else if (act->chunkLeft)
		obj->gfxChunk = act->chunkLeft;

	if (obj->gfxChunk == -1)
		obj->gfxChunk = 0;

	ck_nextX = 0;
	ck_nextY = 0;

#if 0
	// old code:
	
	// K1n9_Duk3 note: The original KEEN 4-6 code didn't have CK_ResetClipRects()
	// as a separate routine, so it temporarilily disabled clipping and ran the
	// normal clipping routine to initialize the object's hitbox to the correct
	// values for its current sprite.
	oldClipping = obj->clipped;
	obj->clipped = CLIP_not;
	CK_PhysUpdateNormalObj(obj);
	obj->clipped = oldClipping;

	//TODO: Handle platforms
	if (obj->clipped == CLIP_full)	// K1n9_Duk3 fix: this was a fullclip check in the original code
		CK_PhysFullClipToWalls(obj);
	else if (obj->clipped != CLIP_not)	// K1n9_Duk3 mod: add support for custom clipping
		CK_PhysUpdateNormalObj(obj);
#else
	// K1n9_Duk3's new version for FITF:

	// first, initialize the hitbox values:
	if (obj->gfxChunk)
	{
		if (obj->clipped == CLIP_full)
		{
			// Running the full clipping code is pretty useless here. Since the
			// object hasn't moved, the routine will not change the object's
			// position. Apart from setting the hitbox to the correct values for
			// the current position and sprite number at the end, all this does is
			// set topTI, bottomTI, leftTI and rightTI to 0. Or, if there are any
			// blocking tiles touching the full clipping hitbox, bottomTI and
			// rightTI will be set to 1.
			CK_PhysFullClipToWalls(obj);
		}
		else if (obj->clipped == CLIP_not)
		{
			// Just initialitze the hitbox for the current sprite:
			CK_ResetClipRects(obj);
		}
		else
		{
			// Get the initial hitbox values for the current sprite:
			if (obj->clipped == CLIP_custom)
				CK_CustomClipRects(obj);
			else
				CK_ResetClipRects(obj);
			
			// And save it as the "old" hitbox:
			CK_SetOldClipRects(obj);
			
			// Now shrink the "old" hitbox:
			ck_oldRects.unitY1 = ck_oldRects.unitY2 = (ck_oldRects.unitY2-ck_oldRects.unitY1)/2 + ck_oldRects.unitY1;
			ck_oldRects.tileY1 = ck_oldRects.tileY2 = RF_UnitToTile(ck_oldRects.unitY1);
			// The shrunken hitbox allows the clipping code to push the object out
			// of the floor or the ceiling.
			
			CK_SetDeltaClipRects(obj);
			
			CK_PhysClipVert(obj);
			CK_PhysClipHorz(obj);
			
			// If the objects needs to stick to a surface, run the clipping routine
			// to push the object against that surface and to set topTI, bottomTI,
			// leftTI, rightTI. Otherwise, if the object uses a custom hitbox, set
			// the hitbox back to the correct values for its current sprite.
			if (obj->currentAction->stickToGround)
				CK_PhysUpdateNormalObj(obj);
			else if (obj->clipped == CLIP_custom)
				CK_ResetClipRects(obj);		
		}
	}
	obj->visible = true;
#endif
}

// ChangeAction
void CK_SetAction2(CK_object *obj, CK_action *act)
{
	obj->currentAction = act;
	obj->actionTimer = 0;

	if (act->chunkRight)
	{
		obj->gfxChunk = (obj->xDirection > 0) ? act->chunkRight : act->chunkLeft;
	}
#if 0
	if (act->chunkRight && obj->xDirection > 0) obj->gfxChunk = act->chunkRight;
	else if (act->chunkLeft) obj->gfxChunk = act->chunkLeft;
#endif
	if (obj->gfxChunk == -1) obj->gfxChunk = 0;

	obj->visible = true;
	ck_nextX = 0;
	ck_nextY = 0;


	if (obj->topTI != 0x19)
		CK_PhysUpdateNormalObj(obj);

}

boolean CK_ObjectVisible(CK_object *obj)
{
	// TODO: Use ScrollX0_T,  ScrollX1_T and co. directly?
	if (obj->clipRects.tileX2 < RF_UnitToTile(originxglobal) || obj->clipRects.tileY2 < RF_UnitToTile(originyglobal) || obj->clipRects.tileX1 > ((originxglobal >> 8) + (320 >> 4)) || obj->clipRects.tileY1 > ((originyglobal >> 8) + (208 >> 4)))
	{
		return false;
	}

	return true;
}

// K1n9_Duk3 addition: special gravity for Keen in easy mode
void CK_PhysGravityEasyKeen(CK_object *obj)
{
	// It's safe to use int variables instead of long's in the for-loop, since
	// the code in the loop is only using the two least significant bits anyway.
	// We just need to make sure to use the condition "!=" instead of "<".
	int tickCount;
	int lastTimeCount = lasttimecount;
	for (tickCount = lastTimeCount - tics; tickCount != lastTimeCount; tickCount++)
	{
		// Every odd tic...
		if (tickCount & 1)
		{
			if (obj->velY < 0 && obj->velY >= -3)
			{
				ck_nextY += obj->velY;
				obj->velY = 0;
				return;
			}
			
			// we alternate between adding a 3 and a 4 to the velY field, which
			// basically gives us gravity halfway between ...Mid and ...High:
			obj->velY += 3;
			if (tickCount & 2)
				obj->velY++;
			
			if (obj->velY > MAXFALLSPEED)
			{
				obj->velY = MAXFALLSPEED;
			}
		}
		ck_nextY += obj->velY;
	}
}
// end of addition

void CK_PhysGravityHigh(CK_object *obj)
{
#if 0
	// original code:
	long tickCount;
	long lastTimeCount = lasttimecount;
	for (tickCount = lastTimeCount - tics; tickCount < lastTimeCount; tickCount++)
#else
	// optimized FITF code:
	int tickCount;
	int lastTimeCount = lasttimecount;
	for (tickCount = lastTimeCount - tics; tickCount != lastTimeCount; tickCount++)
#endif
	{
		// Every odd tic...
		if (tickCount & 1)
		{
			if (obj->velY < 0 && obj->velY >= -4)
			{
				ck_nextY += obj->velY;
				obj->velY = 0;
				return;
			}
			obj->velY += 4;
			if (obj->velY > MAXFALLSPEED)
			{
				obj->velY = MAXFALLSPEED;
			}
		}
		ck_nextY += obj->velY;
	}
}

void CK_PhysGravityMid(CK_object *obj)
{
#if 0
	// original code:
	long tickCount;
	long lastTimeCount = lasttimecount;
	for (tickCount = lastTimeCount - tics; tickCount < lastTimeCount; tickCount++)
#else
	// optimized FITF code:
	int tickCount;
	int lastTimeCount = lasttimecount;
	for (tickCount = lastTimeCount - tics; tickCount != lastTimeCount; tickCount++)
#endif
	{
		// Every odd tic...
		if (tickCount & 1)
		{
			if (obj->velY < 0 && obj->velY >= -3)
			{
				ck_nextY += obj->velY;
				obj->velY = 0;
				return;
			}
			obj->velY += 3;
			if (obj->velY > MAXFALLSPEED)
			{
				obj->velY = MAXFALLSPEED;
			}
		}
		ck_nextY += obj->velY;
	}
}

//#if 0
void CK_PhysGravityLow(CK_object *obj)
{
	// K1n9_Duk3 says:
	// We can't use the optimized loop version here, since that would change the
	// behavior of this routine. The bugged "if (!tickCount)" check uses all 32
	// bits of the long value. But since this is effectively a zero gravity
	// function, we could just omit this and use "ck_nextY += obj->velY * tics;"
	// instead of "CK_PhysGravityLow(obj);".
	long tickCount;
	long lastTimeCount = lasttimecount;
	for (tickCount = lastTimeCount - tics; tickCount < lastTimeCount; tickCount++)
	{
		// K1n9_Duk3 says:
		// The following condition is bugged. What I think happened here is
		// that the original code used the condition "if (!tickCount & 3)",
		// which was probably supposed to mean "if ((tickCount & 3) == 0)",
		// but instead turned into "if ((tickCount == 0) & 3)" because the
		// boolean not operator "!" has a higher priority than the bitwise
		// and operator "&" in (Borland's version of) the C language.
		//
		// The result is that instead of increasing the falling speed every
		// 4 game tics (17.5 times per second or roughly every 57 millisecs),
		// the falling speed only increases once every 4 billion tics (once in
		// 710 days of non-stop playing). Which means this is basically a zero
		// gravity function.
		//
		// Fixing this bug is NOT recommended, as it would alter the maximum
		// jump height of Keen's pogo stick, which could make certain parts
		// of some levels unreachable.

		if ((tickCount?0:1) & 3)	// BUG: this effectively means "(if (tickCount == 0))"
		{
			obj->velY += 1;
			if (obj->velY > MAXFALLSPEED)
			{
				obj->velY = MAXFALLSPEED;
			}
		}
		ck_nextY += obj->velY;
	}
}
//#endif

void CK_PhysDampHorz(CK_object *obj)
{
	long tickCount, lastTimeCount;
	boolean movingLeft = (obj->velX < 0);
	int xAdj;
	if (obj->velX > 0)
		xAdj = -1;
	else if (obj->velX < 0)
		xAdj = 1;
	else
		xAdj = 0;

	lastTimeCount = lasttimecount;
	for (tickCount = lastTimeCount - tics; tickCount < lastTimeCount; tickCount++)
	{
		// Every odd tic...
		if (tickCount & 1)
		{
			obj->velX += xAdj;
			if ((obj->velX < 0) != movingLeft)
			{
				obj->velX = 0;
			}
		}
		ck_nextX += obj->velX;
	}
}

// K1n9_Duk3 addition:

void CK_PhysDampHorz2(CK_object *obj)
{
	int i = tics;
	boolean movingLeft = (obj->velX < 0);
	int xAdj;

	if (obj->velX > 0)
		xAdj = -1;
	else if (obj->velX < 0)
		xAdj = 1;
	else
		xAdj = 0;

	while (i--)
	{
		obj->velX += xAdj;
		if ((obj->velX < 0) != movingLeft)
		{
			obj->velX = 0;
		}
		ck_nextX += obj->velX;
	}
}

// end of addition

void CK_PhysAccelHorz(CK_object *obj, int accX, int velLimit)
{
	long tickCount;
	boolean isNegative = (obj->velX < 0);
	long lastTimeCount = lasttimecount;
	for (tickCount = lastTimeCount - tics; tickCount < lastTimeCount; tickCount++)
	{
		// Every odd tic...
		if (tickCount & 1)
		{
			obj->velX += accX;
			if ((obj->velX < 0) != isNegative)
			{
				isNegative = (obj->velX < 0);
				obj->xDirection = isNegative ? -1 : 1;
			}

			if (accX > 0 && obj->velX > velLimit)	// K1n9_Duk3 fix: added accX check
			{
				obj->velX = velLimit;
			}
			else if (accX < 0 && obj->velX < -velLimit)	// K1n9_Duk3 fix: added accX check
			{
				obj->velX = -velLimit;
			}
		}
		ck_nextX += obj->velX;
	}
}

void CK_PhysDampVert(CK_object *obj)
{
	long tickCount, lastTimeCount;
	boolean movingLeft = (obj->velY < 0);
	int yAdj;
	if (obj->velY > 0)
		yAdj = -1;
	else if (obj->velY < 0)
		yAdj = 1;
	else
		yAdj = 0;

	lastTimeCount = lasttimecount;
	for (tickCount = lastTimeCount - tics; tickCount < lastTimeCount; tickCount++)
	{
		// Every odd tic...
		if (tickCount & 1)
		{
			obj->velY += yAdj;
			if ((obj->velY < 0) != movingLeft)
			{
				obj->velY = 0;
			}
		}
		ck_nextY += obj->velY;
	}
}

void CK_PhysAccelVert(CK_object *obj, int accY, int velLimit)
{
	long tickCount;
	boolean isNegative = (obj->velY < 0);
	long lastTimeCount = lasttimecount;
	for (tickCount = lastTimeCount - tics; tickCount < lastTimeCount; tickCount++)
	{
		// Every odd tic...
		if (tickCount & 1)
		{
			obj->velY += accY;
			if ((obj->velY < 0) != isNegative)
			{
				isNegative = (obj->velY < 0);
				obj->yDirection = isNegative ? -1 : 1;
			}

			if (accY > 0 && obj->velY > velLimit)	// K1n9_Duk3 fix: added accY check
			{
				obj->velY = velLimit;
			}
			else if (accY < 0 && obj->velY < -velLimit)	// K1n9_Duk3 fix: added accY check
			{
				obj->velY = -velLimit;
			}
		}
		ck_nextY += obj->velY;
	}
}
