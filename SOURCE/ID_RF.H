/* "Foray in the Forest" Source Code
 * Copyright (C) 2022-2023 Nisaba
 * Copyright (C) 2019-2020 K1n9_Duk3
 *
 * Primarily based on:
 * Catacomb 3-D Source Code
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// ID_RF.H

#define __ID_RF__

#ifndef __ID_MM__
#include "ID_MM.H"
#endif

/*
=============================================================================

							CONSTANTS

=============================================================================
*/

#define	MINTICS				1	// K1n9_Duk3 mod: old games use 2 (35 fps), but we can use 1 (70 fps) here
#define	MAXTICS				5	// K1n9_Duk3 mod: Keen 4-6 use 5, only Dreams used 6
#define DEMOTICS			3

#define	MAPBORDER			2		// map border must be at least 1

// Keen Dreams patch
#define	MAXSPRITES			50/*50*/		// max tracked sprites
#define	MAXANIMTILES		100/*90*/		// max animating tiles on screen
#define MAXANIMTYPES		100/*50*/		// max different unique anim tiles on map

#define	MAXMAPHEIGHT		250

#define	PRIORITIES			5/*4*/	// K1n9_Duk3 mod: additional foreground layer to avoid scorebox/flag conflicts on world map
#define	MASKEDTILEPRIORITY	3		// planes go: 0,1,2,MTILES,3

#define TILEGLOBAL			256
#define PIXGLOBAL			16

#define	G_T_SHIFT			8		// global >> ?? = tile
#define	G_P_SHIFT			4		// global >> ?? = pixels
#define P_T_SHIFT			4		// pixels >> ?? = tile

#define RF_UnitToTile(u)	((unsigned)(u) >> G_T_SHIFT)
#define RF_TileToUnit(t)	((t) << G_T_SHIFT)
#define RF_PixelToUnit(p)	((p) << G_P_SHIFT)

#define	PORTTILESWIDE		21      // all drawing takes place inside a
#define	PORTTILESHIGH		14		// non displayed port of this size

//#define	PORTGLOBALWIDE		(21*TILEGLOBAL)
//#define	PORTGLOBALHIGH		(14*TILEGLOBAL)

#define UPDATEWIDE			(PORTTILESWIDE+1)
#define UPDATEHIGH			PORTTILESHIGH


//===========================================================================

typedef enum {spritedraw,maskdraw,whitedraw,darkgraydraw,lightgraydraw, drawflipped=0x8000} drawtype;

/*
=============================================================================

						 PUBLIC VARIABLES

=============================================================================
*/


extern	boolean		compatability;			// crippled refresh for wierdo SVGAs

extern	unsigned	tics, mintics;
extern	long		lasttimecount;

extern	unsigned	originxglobal,originyglobal;
extern	unsigned	originxtile,originytile;
extern	unsigned	originxscreen,originyscreen;

extern	unsigned	mapwidth,mapheight,mapbyteswide,mapwordswide
					,mapbytesextra,mapwordsextra;
extern	unsigned	mapbwidthtable[MAXMAPHEIGHT];

extern	unsigned	originxmin,originxmax,originymin,originymax;

extern	unsigned	masterofs;

//
// the floating update window is also used by the view manager for
// double buffer tracking
//

extern	byte		*updateptr;				// current start of update window

#if GRMODE == CGAGR
extern	byte		*baseupdateptr;
#endif

extern unsigned	blockstarts[UPDATEWIDE*UPDATEHIGH];
extern unsigned	updatemapofs[UPDATEWIDE*UPDATEHIGH];
extern unsigned	uwidthtable[UPDATEHIGH];		// lookup instead of multiple

#define	UPDATETERMINATE	0x0301

/*
=============================================================================

						 PUBLIC FUNCTIONS

=============================================================================
*/

void RF_Startup (void);
void RF_Shutdown (void);

void RF_FixOfs (void);
void RF_NewMap (void);
void RF_MarkTileGraphics (void);
void RF_SetScrollBlock (int x, int y, boolean horizontal);
void RF_NewPosition (unsigned x, unsigned y);
void RF_Scroll (int x, int y);

void RF_MapToMap (unsigned srcx, unsigned srcy,
				  unsigned destx, unsigned desty,
				  unsigned width, unsigned height);
void RF_MemToMap (unsigned far *source, unsigned plane,
				  unsigned destx, unsigned desty,
				  unsigned width, unsigned height);

void RF_ClearBlock (int	x, int y, int width, int height);
void RF_RedrawBlock (int x, int y, int width, int height);

void RF_PlaceSpriteAfter (void **user,unsigned globalx,unsigned globaly,
	unsigned spritenumber, drawtype draw, int priority, void **masteruser);
void RF_PlaceSpriteClipped (void **user,unsigned globalx,unsigned globaly,
	unsigned spritenumber, drawtype draw, int priority, int clipY);
/*
void RF_PlaceSprite (void **user,unsigned globalx,unsigned globaly,
	unsigned spritenumber, drawtype draw, int priority);
*/
#define RF_PlaceSprite(user, gx, gy, num, draw, p)	\
	RF_PlaceSpriteClipped(user, gx, gy, num, draw, p, 0)

void RF_RemoveSprite (void **user);

void RF_CalcTics (void);

void RF_Refresh (void);
void RF_ForceRefresh (void);
void RF_SetRefreshHook (void (*func) (void) );

unsigned RF_FindFreeBuffer (void);

// K1n9_Duk3 additions:

unsigned RF_SpritesUsed(void);
unsigned RF_SpritesAvailable(void);
boolean RF_SaveState(int handle);
boolean RF_LoadState(int handle);
