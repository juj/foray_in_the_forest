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

// KD_PLAY.C

#include "CK_DEF.H"
#pragma	hdrstop

/*
=============================================================================

   LOCAL CONSTANTS

=============================================================================
*/

#define	INACTIVATEDIST	10

/*
=============================================================================

   GLOBAL VARIABLES

=============================================================================
*/


boolean		button0held,button1held;

unsigned	originxtilemax,originytilemax;

ControlInfo	ck_inputFrame;


CK_object ck_objArray[CK_MAX_OBJECTS];

CK_object *ck_freeObject;
CK_object *ck_lastObject;
int ck_numObjects;

CK_object *ck_keenObj;
CK_object *ck_scoreBoxObj;

CK_object tempObj;

// The rectangle within which objects are active.
int ck_activeX0Tile;
int ck_activeY0Tile;
int ck_activeX1Tile;
int ck_activeY1Tile;


void CK_KeenCheckSpecialTileInfo(CK_object *obj);
void StartMusic(unsigned level, boolean repeat);
void StopMusic(void);

CK_GameState ck_gameState;
CK_MapState ck_mapState;
int brainfucled = 0;

CK_LevelState ck_levelState;

// Switch to toggle Camera following keen.
boolean ck_scrollDisabled = false;

// Set if game started with /DEMO parm
int ck_demoParm;

// ScoreBox
boolean showscorebox;

// Fix jerky motion
//boolean ck_fixJerkyMotion;

// Gamepad
boolean ck_gamePadEnabled;

// Pogo timer for two-button firing
int ck_pogoTimer;

// Two button shooting variables
//ScanCode	firescan = sc_Space;
/*
ScanCode scoreboxscan = sc_BackSpace;
ScanCode statusscan = sc_Enter;
*/
boolean		oldshooting;


/*
=============================================================================

   LOCAL VARIABLES

=============================================================================
*/

// for asm scaning of map planes
//unsigned	mapx,mapy,mapxcount,mapycount,maptile,mapspot;	// K1n9_Duk3 mod: these are never used

// Active object boundaries
int ck_activeX0Tile, ck_activeX1Tile, ck_activeY0Tile, ck_activeY1Tile;

//int			screenYpx;	// K1n9_Duk3 mod: moved variable into ck_mapState
byte	wallcheat;	// K1n9_Duk3 addition: remember which cheats are active

static void near pascal ContinueMusic(unsigned num);	// K1n9_Duk3 addition


/*
=============================================================================

						 SCORE BOX ROUTINES

=============================================================================
*/

void	SpawnScore (void);
void CK_UpdateScoreBox (CK_object *ob);
void ScoreReact (CK_object *ob);

void MemDrawChar (int char8,byte far *dest,unsigned width,unsigned planesize);

CK_action CK_ACT_ScoreBox = {NULL,NULL,AT_Frame,false, false,0, 0,0, NULL , NULL, NULL, NULL};
CK_action CK_ACT_DemoSign = {DEMOSIGNSPR,DEMOSIGNSPR,AT_Frame,false, false,0, 0,0, NULL , NULL, NULL, NULL};


/*
======================
=
= SpawnScore
=
======================
*/

void	SpawnScore (void)
{
	ck_scoreBoxObj->type = CT_Friendly;
	ck_scoreBoxObj->active = OBJ_ALWAYS_ACTIVE;
	ck_scoreBoxObj->clipped = false;
	*((long *)&(ck_scoreBoxObj->user1)) = -1L;	// force score to be updated
	ck_scoreBoxObj->user3 = -1;			// and shots
	ck_scoreBoxObj->user4 = -1;			// and lives

	if (ck_inHighScores)
	{
		// Don't display anything in the high scores
		//ck_scoreBoxObj->currentAction = NULL;	// EXTREMELY bad idea! NULL actions will cause errors, at least on real hardware.
		CK_SetAction(ck_scoreBoxObj, &CK_ACT_ScoreBox);	// K1n9_Duk3 fix (this state doesn't do anything anyway)
	}
	else if (DemoMode)
	{
		// If this is a demo, display the DEMO banner
		CK_SetAction(ck_scoreBoxObj, &CK_ACT_DemoSign);
		CA_CacheGrChunk(DEMOSIGNSPR);
	}
	else
	{
		// If a normal game, display the scorebox 
		CK_SetAction(ck_scoreBoxObj, &CK_ACT_ScoreBox);
	}
}

void CK_DemoSign( CK_object *demo)
{
	if (	demo->posX == originxglobal && demo->posY == originyglobal ) 
		return;
	demo->posX = originxglobal;
	demo->posY = originyglobal;

	//place demo sprite in center top
	RF_PlaceSprite( &(demo->sde), demo->posX+0x0A00 - 0x200, demo->posY+0x80,DEMOSIGNSPR,spritedraw,3);
}


/*
======================
=
= MemDrawChar
=
======================
*/

#if GRMODE == EGAGR

void MemDrawChar (int char8,byte far *dest,unsigned width,unsigned planesize)
{
#if 0

// Note, I had to re-write this in C because the inline assembly
// version was behaving strangely depending on the number of chunks
// in the graphics archive... weird!

	byte far *src;
	int i, j;

	src = (byte far *)grsegs[STARTTILE8] + char8*32;

	for (i = 0; i < 4; i++) {
		byte far *d = dest;
		for (j = 0; j < 8; j++) {
			*d = *src++;
			d += width;
		}
		dest += planesize;
	}

#else
// K1n9_Duk3 says:
//
// I modified the assembly code below to match the code from KEEN 4-6, but then
// I realized that I could just assign the pseudo-register-variable _DS in C
// code to maintain the efficiency of the assembly code and also avoid the error
// that popped up when trying to compile the assembly version with an odd value
// for STARTTILE8 (655 in this case).

//	void _seg *source = grsegs[STARTTILE8];	// KEEN 4-6 fix

asm	mov	si,[char8]
asm	shl	si,1
asm	shl	si,1
asm	shl	si,1
asm	shl	si,1
asm	shl	si,1		// index into char 8 segment

//asm	mov	ds,[WORD PTR grsegs+STARTTILE8*2]	// KEEN DREAMS code
	_DS = (unsigned)grsegs[STARTTILE8];	// FITF code :)
//asm	mov	ds,source	// KEEN 4-6 fix
asm	mov	es,[WORD PTR dest+2]

asm	mov	cx,4		// draw four planes
asm	mov	bx,[width]
asm	dec	bx

planeloop:

asm	mov	di,[WORD PTR dest]

asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb
asm	add	di,bx
asm	movsb

asm	mov	ax,[planesize]
asm	add	[WORD PTR dest],ax

asm	loop	planeloop

asm	mov	ax,ss
asm	mov	ds,ax

#endif
}
#endif

#if GRMODE == CGAGR
void MemDrawChar (int char8,byte far *dest,unsigned width,unsigned planesize)
{
asm	mov	si,[char8]
asm	shl	si,1
asm	shl	si,1
asm	shl	si,1
asm	shl	si,1		// index into char 8 segment

asm	mov	ds,[WORD PTR grsegs+STARTTILE8*2]
asm	mov	es,[WORD PTR dest+2]

asm	mov	bx,[width]
asm	sub	bx,2

asm	mov	di,[WORD PTR dest]

asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw

asm	mov	ax,ss
asm	mov	ds,ax

	planesize++;		// shut the compiler up
}
#endif


/*
====================
=
= ShiftScore
=
====================
*/
#if GRMODE == EGAGR
#ifdef NEWMODS
#define ShiftScore() /*nothing*/
#else
void ShiftScore (void)
{
	spritetabletype far *spr;
	spritetype _seg *dest;

	spr = &spritetable[SCOREBOXSPR-STARTSPRITES];
	dest = (spritetype _seg *)grsegs[SCOREBOXSPR];

	CAL_ShiftSprite (FP_SEG(dest),dest->sourceoffset[0],
		dest->sourceoffset[1],spr->width,spr->height,2);

	CAL_ShiftSprite (FP_SEG(dest),dest->sourceoffset[0],
		dest->sourceoffset[2],spr->width,spr->height,4);

	CAL_ShiftSprite (FP_SEG(dest),dest->sourceoffset[0],
		dest->sourceoffset[3],spr->width,spr->height,6);
}
#endif
#endif

/*
===============
=
= CK_UpdateScoreBox
=
===============
*/

void CK_ForceScoreboxUpdate(void)
{
	register CK_object *ob = ck_scoreBoxObj;

	ob->user1 = ob->user2 = ob->user3 = ob->user4 = -1;

	if (showscorebox)
	{
		if (!ck_inHighScores && !DemoMode && grsegs[SCOREBOXSPR])
			CK_UpdateScoreBox(ob);
	}
	else if (ob->sde)
	{
		RF_RemoveSprite(&ob->sde);
	}
}

void CK_UpdateScoreBox (CK_object *ob)
{
	char		str[10],*ch;
	spritetype	_seg	*block;
	byte		far *dest;
	unsigned	i, length, width, planesize, number;
	boolean 	updated = false;

	// Don't draw anything for the high score level
	if (ck_inHighScores)
		return;

	// Show the demo sign for the demo mode
	if (DemoMode)
	{
		CK_DemoSign(ob);
		return;
	}

	if (!showscorebox)
		return;
//
// score changed
//
	/*
	if ((ck_gameState.keenScore>>16) != ob->user1
		|| (unsigned)ck_gameState.keenScore != ob->user2 )
	{
	*/
	// Nisaba patch:
	if ((int)(ck_gameState.keenScore>>16) != ob->user1
		|| (int)ck_gameState.keenScore != ob->user2 )
	{
		block = (spritetype _seg *)grsegs[SCOREBOXSPR];
		width = block->width[0];
		planesize = block->planesize[0];
		dest = (byte far *)grsegs[SCOREBOXSPR]+block->sourceoffset[0]
			+ planesize + width*4 + 1*CHARWIDTH;

		ltoa (ck_gameState.keenScore,str,10);

		// erase leading spaces
		length = strlen(str);
		for (i=8;i>length;i--)
			MemDrawChar (6,dest+=CHARWIDTH,width,planesize);

		// draw digits
		ch = str;
		while (*ch)
			MemDrawChar (*ch++ - '0'+7,dest+=CHARWIDTH,width,planesize);

#if GRMODE == EGAGR
		ShiftScore ();
#endif
		updated = true;
		ob->user1 = ck_gameState.keenScore>>16;
		ob->user2 = ck_gameState.keenScore;
	}

//
// shots changed
//
	number = ck_gameState.numShots;
	if (number > 99) number = 99;	// K1n9_Duk3 mod: cap at 99 here to avoid useless sprite changes
	if (number != ob->user3)
	{
		block = (spritetype _seg *)grsegs[SCOREBOXSPR];
		width = block->width[0];
		planesize = block->planesize[0];
		dest = (byte far *)grsegs[SCOREBOXSPR]+block->sourceoffset[0]
			+ planesize + width*20 + 7*CHARWIDTH;

#if 0
		// Keen 4-6 version:
		if (number > 99)
			strcpy (str,"99");
		else
			ltoa (number,str,10);
#else
		//FITF version (elimitates the string literal):
		ltoa (number,str,10);
#endif

		// erase leading spaces
		length = strlen(str);
		for (i=2;i>length;i--)
			MemDrawChar (6,dest+=CHARWIDTH,width,planesize);

		// draw digits
		ch = str;
		while (*ch)
			MemDrawChar (*ch++ - '0'+7,dest+=CHARWIDTH,width,planesize);

#if GRMODE == EGAGR
		ShiftScore ();
#endif
		updated = true;
		ob->user3 = number;	// K1n9_Duk3 mod: remember the displayed number, not the real ammo value
	}

//
// lives changed
//
	number = ck_gameState.numLives;
	if (number > 99) number = 99;	// K1n9_Duk3 mod: cap at 99 here to avoid useless sprite changes
	if (number != ob->user4)
	{
		block = (spritetype _seg *)grsegs[SCOREBOXSPR];
		width = block->width[0];
		planesize = block->planesize[0];
		dest = (byte far *)grsegs[SCOREBOXSPR]+block->sourceoffset[0]
			+ planesize + width*20 + 2*CHARWIDTH;

#if 0
		// Keen 4-6 version:
		if (number > 99)
			strcpy (str,"99");
		else
			ltoa (number,str,10);
#else
		//FITF version (elimitates the string literal):
		ltoa (number,str,10);
#endif

		// erase leading spaces
		length = strlen(str);
		for (i=2;i>length;i--)
			MemDrawChar (6,dest+=CHARWIDTH,width,planesize);

		// draw digits
		ch = str;
		while (*ch)
			MemDrawChar (*ch++ - '0'+7,dest+=CHARWIDTH,width,planesize);

#if GRMODE == EGAGR
		ShiftScore ();
#endif
		updated = true;
		ob->user4 = number;	// K1n9_Duk3 mod: remember the displayed number, not the real lives value
	}

#if 0
	// K1n9_Duk3 mod: move scorebox to the top-right corner of the screen
	// when Keen is near the left edge of the screen

	i = (originxglobal+128*PIXGLOBAL > (ck_mapState.mindcontrolled? ck_mapState.mindcontrolled : ck_keenObj)->clipRects.unitX1)? originxglobal+228*PIXGLOBAL : originxglobal;
	if (i != ob->posX || originyglobal != ob->posY)
	{
		ob->posX = i;
		ob->posY = originyglobal;
		updated = true;
	}
#else
	// original code
	if (originxglobal != ob->posX || originyglobal != ob->posY)
	{
		ob->posX = originxglobal;
		ob->posY = originyglobal;
		updated = true;
	}
#endif


	if (updated)
#if GRMODE == EGAGR
	RF_PlaceSprite (&ob->sde
		,ob->posX+4*PIXGLOBAL
		,ob->posY+4*PIXGLOBAL
		,SCOREBOXSPR
		,spritedraw
		,MAXPRIORITY);
#endif

#if GRMODE == CGAGR
	RF_PlaceSprite (&ob->sde
		,ob->posX+8*PIXGLOBAL
		,ob->posY+8*PIXGLOBAL
		,SCOREBOXSPR
		,spritedraw
		,MAXPRIORITY);
#endif
}
//===========================================================================
//
// DROPDOWN STATUS WINDOW
//
//===========================================================================

// The status window is drawn to scratch area in the video buffer
// Then it is copied from the scratch to the screen using the RF_ Hook

void CK_DrawLongRight(int x, int y, int digits, int zerotile, long value);
void CK_DrawStatusWindow(void);
void CK_ScrollStatusWindow(void);
/*
void CK_ShowStatusWindow(void);
*/

//extern const char *ck_levelNames[];
int ck_statusWindowYPx;
boolean ck_statusDown;
unsigned statusWindowOfs;  // screen buffer


// K1n9_Duk3 addition:
// The following DrawTile8 routine supports any pixel position. EGA only.

void DrawTile8 (int x, int y, int tile)
{
	byte buffer[40];

	// turn the tile into a masked tile, so we can use VW_ShiftlessMaskBlock:
#if 1
	// optimized version (faster and slightly smaller code):
	asm {
		mov	ax, ss;
		mov	es, ax;
		lea	di, buffer;
		xor	ax, ax;
		mov	cx, 4;
		rep stosw;
		mov	si, tile;
		mov	cl, 5;
		shl	si, cl;
		mov	cx, 16;
//		mov	ax, word ptr grsegs+(2*STARTTILE8);
//		push	ds;
//		mov	ds, ax;
		//mov	ds, word ptr grsegs+(2*STARTTILE8);
	}
		_DS = (unsigned)grsegs[STARTTILE8];	//inline assembly version doesn't work when STARTTILE8 is not even
	asm {
		rep	movsw;
		mov	ax, ss;
		mov	ds, ax;
//		pop	ds;
	}
#else
	// for those who don't understand assembly:
	memset(buffer, 0, 8);	// set the mask plane to 0s (nothing is transparent)
	_fmemcpy(buffer+8, ((byte far *)grsegs[STARTTILE8])+tile*32, 32);	// insert the color planes after the mask plane
#endif

	// now draw the tile with shitfs:
	x+=pansx;
	y+=pansy;
	if (VW_MarkUpdateBlock(x,y,x+7,y+7))
		VW_ShiftlessMaskBlock((memptr)FP_SEG(buffer), FP_OFF(buffer), bufferofs+ylookup[y]+(x>>3),1,8,8, (x&7));
}

void ClipSprite(int x, int y, int chunknum, unsigned skiptop, unsigned h, unsigned drawtype)
{
	spritetabletype far *spr;
	spritetype _seg	*block;
	unsigned	dest,off,width,height;

	// temporarily cache the sprite if it's not loaded:
	if (!grsegs[chunknum])
	{
		// try to cache it:
		MM_BombOnError(false);
		CA_CacheGrChunk(chunknum);
		MM_BombOnError(true);
		if (mmerror)
		{
			mmerror = false;
			return;
		}
		// and make it purgable again:
		CA_UnmarkGrChunk(chunknum);
		MM_SetPurge(&grsegs[chunknum], 1);
	}

	x+=pansx;
	y+=pansy;

	spr = &spritetable[chunknum-STARTSPRITES];
	block = (spritetype _seg *)grsegs[chunknum];

	height = spr->orgy>>G_P_SHIFT;
	if (skiptop >= height)
	{
		skiptop -= height;
		height = 0;
	}
	else
	{
		height -= skiptop;
		skiptop = 0;
	}
	y+=height;
	if (h)
	{
		h-=height;
		if ((int)h <= 0)
			return;
	}
	x+=spr->orgx>>G_P_SHIFT;

	dest = bufferofs + ylookup[y];
	if (x>=0)
		dest += x/SCREENXDIV;
	else
		dest += (x+1)/SCREENXDIV;

	width = block->width[0];
	height = spr->height;
	off = block->sourceoffset[0];

	if (skiptop)
	{
		if (skiptop < height)
		{
			off += width*skiptop;
			height -= skiptop;
		}
		else return;
	}
	if (h)
	{
		if (h < height)
			height = h;
	}

	if (VW_MarkUpdateBlock (x&SCREENXMASK,y,(x&SCREENXMASK)+width*SCREENXDIV-1,y+height-1))
	{
		switch (drawtype)
		{
		case whitedraw:
			VW_WhiteMaskBlock(block,off,dest,width,height,WHITE,(x&7));
			break;
		case darkgraydraw:
			VW_WhiteMaskBlock(block,off,dest,width,height,DARKGRAY,(x&7));
			break;
		case lightgraydraw:
			VW_WhiteMaskBlock(block,off,dest,width,height,LIGHTGRAY,(x&7));
			break;
		default:
			VW_ShiftlessMaskBlock(block,off,dest,width,height,block->planesize[0],(x&7));
		}
	}
}
// end of addition

void CK_DrawLongRight(int x, int y, int digits, int zerotile, long value)
{
	char s[20];
	int len,i;


	ltoa(value, s, 10);
	len = strlen(s);

	if (len > digits)
	{
		zerotile+=10;
		while (digits--)
		{
			VWB_DrawTile8(x, y, zerotile);
			x += 8;
		}
		return;
	}

	for (i = digits; i > len; i--)
	{
		VWB_DrawTile8(x, y, 6);
		x+=8;
	}

	while (i > 0)
	{
		VWB_DrawTile8(x,y,zerotile + s[len-i] - 47);
		i--;
		x+=8;
	}

}

void CK_DrawStatusWindowBackground(void)
{
	int n;
	// Draw the backdrop
	int winX = 64;
	int winY = 16;
	int winW = 184;
	int winH = 144;


	VWB_DrawTile8(winX, winY, 36);	// top-left corner
	VWB_DrawTile8(winX, winY+winH, 41);	// bottom-left corner
	n = winX + 8;

	while (winX + winW - 8 >= n)
	{

		VWB_DrawTile8(n, winY, 37);	// top edge
		VWB_DrawTile8(n, winY + winH, 42);	// bottom edge
		n += 8;
	}

	VWB_DrawTile8(n, winY, 38);	// top-right corner
	VWB_DrawTile8(n, winY + winH, 43);	// bottom-right corner

	n = winY + 8;

	while (winY + winH - 8 >= n)
	{

		VWB_DrawTile8(winX, n, 39);	// left edge
		VWB_DrawTile8(winX+winW, n, 40);	// right edge
		n += 8;
	}

	VWB_Bar(72, 24, 176, 136, 7);	// fill center area
}

void CK_DrawStatusWindow(void)
{
	unsigned strW, strH;
	int x, i, oldcolor;

	CK_DrawStatusWindowBackground();

	// Print the stuff
	oldcolor = fontcolor;
	
	// Level
	PrintY = 27;
	WindowX = 80;
	WindowW = 161;
	fontcolor = WHITE;
	//US_CPrint("LOCATION");
	SM_CPrint(S_LOCATION);
	VWB_Bar(79-1, 38-1, 162+2, 20+2, DARKGRAY);
	VWB_Bar(79, 38, 162, 20, BLACK);
	//strcpy(str, ck_levelNames[ck_gameState.currentLevel]);
	SM_CopyString(str, S_LEVELNAME0+ck_gameState.currentLevel);
	CK_MeasureMultiline(str, &strW, &strH);
	PrintY = (20 - strH)/2 + 40 - 2;
	fontcolor = LIGHTGREEN;
	US_CPrint(str);

	// Extra man
	PrintY = 61;
	WindowX = 80;
	WindowW = 64;
	fontcolor = WHITE;
	//US_CPrint("EXTRA");
	SM_CPrint(S_EXTRA);
	VWB_Bar(79, 71, 66, 10, BLACK);
	CK_DrawLongRight(80, 72, 8, 6, ck_gameState.nextKeenAt);

	// Score
	PrintY = 85;
	WindowX = 81;
	WindowW = 64;
	//US_CPrint("SCORE");
	SM_CPrint(S_SCORE);
	VWB_Bar(79, 95, 66, 10, BLACK);
	CK_DrawLongRight(80, 96, 8, 6, ck_gameState.keenScore);

	// Difficulty
	PrintY = 61;
	WindowX = 177;
	WindowW = 64;
	fontcolor = WHITE;
	//US_CPrint("LEVEL");
	SM_CPrint(S_LEVEL);
	VWB_Bar(175, 71, 66, 10, BLACK);

	PrintY = 72;
	WindowX = 176;
	WindowW = 64;
	fontcolor = LIGHTGREEN;

#if 1
	// quick hack for alpha-testing
	if (ck_gameState.cheated)
		//US_CPrint("CHEATER!");
		SM_CPrint(S_CHEATER);
	else
#endif
	switch (ck_gameState.difficulty)
	{
	case 1:
		//US_CPrint("Easy");
		SM_CPrint(S_EASY);
		break;
	case 2:
		//US_CPrint("Normal");
		SM_CPrint(S_NORMAL);
		break;
	case 3:
		//US_CPrint("Hard");
		SM_CPrint(S_HARD);
		break;
	}

	// Key gems
	PrintX = 194;
	PrintY = 85;
	fontcolor = WHITE;
	//US_Print("KEYS");
	SM_Print(S_KEYS);

	VWB_Bar(175, 95, 66, 10, BLACK);

	for (i = 0, x=184; i < 4; i++, x+=13)
	{
		if (ck_mapState.keyGems[i])
			DrawTile8(x, 96, i);
		else
			DrawTile8(x, 96, 4);	// K1n9_Duk3 addition
	}

	// AMMO
	PrintX = 80;
	PrintY = 144;
	//US_Print("AMMO");
	SM_Print(S_AMMO);
	VWB_Bar(127, 143, 18, 10, BLACK);
	CK_DrawLongRight(128, 144, 2, 6, ck_gameState.numShots);

	// Lives
	PrintX = 80;
	PrintY = 112;
	//US_Print("KEENS");
	SM_Print(S_KEENS);
	VWB_Bar(127, 111, 18, 10, BLACK);
	CK_DrawLongRight(128, 112, 2, 6, ck_gameState.numLives);

	// Lifeups
	PrintX = 80;
	PrintY = 128;
	//US_Print("DROPS");
	SM_Print(S_DROPS);
	VWB_Bar(127, 127, 18, 10, BLACK);
	CK_DrawLongRight(128, 128, 2, 6, ck_gameState.numVitalin);

	PrintY = 117;
	WindowX = 177;
	WindowW = 64;
	//US_CPrint("GOODIES");
	SM_CPrint(S_GOODIES);
	{
		unsigned bits, spr;
		bits = ck_gameState.collectiblesTotal|ck_mapState.collectiblesNew;
		spr = COLLECTIBLETEDDY1SPR;
		VWB_Bar(175, 127, 66, 10, BLACK);
		VWB_Bar(175, 143, 66, 10, BLACK);
		x = 182;
		for (i=1; i<(1<<3); i<<=1, x+=18, spr += 2)
		{
			ClipSprite(x, 128, spr, 1, 8, (bits & i)? spritedraw : darkgraydraw);
		}
		x = 182;
		for (; i<(1<<6); i<<=1, x+=18, spr += 2)
		{
			ClipSprite(x, 144, spr, 1, 8, (bits & i)? spritedraw : darkgraydraw);
		}
	}

#if 0
	// quick hack for alpha testing:
	CK_DrawLongRight(152, 72, 2, 6, ck_gameState.slugcans+ck_mapState.slugcans);
#endif

	fontcolor = oldcolor;


}

long distance(int x1, int y1, int x2, int y2)
{
	long xd, yd;
	xd = (x1-x2);
	yd = (y1-y2);
	return (xd*xd + yd*yd);	// we don't need to calculate the square root here
}

void CK_DrawDetectorWindow(void)
{
	unsigned strW, strH;
	unsigned x, i, oldcolor;

	CK_object *ob;
	unsigned lives=0, drops=0, ammo=0, keyBits=0, collectibles=0;
	long points=0;
	long bestAmmoDist = 0x7FFFFFFF, dist;
	int bestAmmoX, bestAmmoY;

	// scan level for items:
	for (i=mapwidth*mapheight; i--; )	// visit all tiles in reverse order
	{
		x = TI_ForeMisc(mapsegs[1][i]) & 0x7F;
		switch (x)
		{
		case MF_Centilife:
			drops++;
			break;
		case MF_Points100:
		case MF_Points200:
		case MF_Points500:
		case MF_Points1000:
		case MF_Points2000:
		case MF_Points5000:
			points += CK5_ItemPoints[x+4-MF_Points100];
			break;
		case MF_1UP:
			if (ck_gameState.difficulty == D_Normal)
				lives += 2;
			else
				lives++;
			break;
/*
		case MF_3UP:
			lives += 3;
			break;
*/
/*
		case MF_Stunner:
		case MF_AmmoStation:
*/
		// Nisaba patch
		case MF_AmmoStation:
			if (mapsegs[2][i] == INFONUMBER_0)
				break;
			// no break here
		case MF_Stunner:
			// We need x and y coordinates to find the nearest stunner!
			// Let's re-use strW and strH for that.
#define tileX strW
#define tileY strH
			tileX = i % mapwidth;
			tileY = i / mapwidth;
			dist = distance((tileX << G_P_SHIFT) + 8, (tileY << G_P_SHIFT) + 8, ck_keenObj->clipRects.unitXmid >> G_P_SHIFT, (ck_keenObj->clipRects.unitY2 >> G_P_SHIFT) - 16);
			if (dist < bestAmmoDist)
			{
				bestAmmoDist = dist;
				bestAmmoX = tileX;
				bestAmmoY = tileY;
			}
#undef tileX
#undef tileY
			ammo++;
			break;
		}
	}

	// scan objects:
	for (ob=ck_keenObj; ob; ob=ob->next)
	{
		switch (ob->type)
		{
		case CT_Item:
			if (ob->user1 < 4)
			{
				keyBits |= (1<<ob->user1);
			}
			else if (ob->user1 == 10)
			{
				if (ck_gameState.difficulty == D_Normal)
					lives += 2;
				else 
					lives++;
			}
			else if (ob->user1 == 11)
			{
				dist = distance(ob->clipRects.unitXmid >> G_P_SHIFT, (ob->clipRects.unitY1 >> G_P_SHIFT) + 8, ck_keenObj->clipRects.unitXmid >> G_P_SHIFT, (ck_keenObj->clipRects.unitY2 >> G_P_SHIFT) - 16);
				if (dist < bestAmmoDist)
				{
					bestAmmoDist = dist;
					bestAmmoX = ob->clipRects.tileX1;
					bestAmmoY = ob->clipRects.tileY1;
				}
				ammo++;
			}
			points += CK5_ItemPoints[ob->user1];
			break;
		case CT_Slug:
			if (ob->user1)	// if slug is carrying donut
				points += CK5_ItemPoints[9];	// add points for donut item
			break;
		case CT_Collectible:
			collectibles |= ob->user1;
			break;
		}
	}

	CK_DrawStatusWindowBackground();

	// Print the stuff
	oldcolor = fontcolor;
	
	// Level
	PrintY = 27;
	WindowX = 80;
	WindowW = 161;
	fontcolor = WHITE;
	//US_CPrint("LOCATION");
	SM_CPrint(S_LOCATION);
	VWB_Bar(79-1, 38-1, 162+2, 20+2, DARKGRAY);
	VWB_Bar(79, 38, 162, 20, BLACK);
	//strcpy(str, ck_levelNames[ck_gameState.currentLevel]);
	SM_CopyString(str, S_LEVELNAME0+ck_gameState.currentLevel);
	CK_MeasureMultiline(str, &strW, &strH);
	PrintY = (20 - strH)/2 + 40 - 2;
	fontcolor = LIGHTRED;
	US_CPrint(str);
	fontcolor = WHITE;

	PrintY = 68;
	WindowX = 80;
	WindowW = 160;
	VWB_Bar(WindowX-1, PrintY-2, WindowW+2, 13, DARKGRAY);
	VWB_Bar(WindowX, PrintY-1, WindowW, 11, BLACK);
	fontcolor = LIGHTRED;
	//US_CPrint("ITEMS DETECTED NEARBY:");
	SM_CPrint(S_ITEMSDETECTED);
	fontcolor = WHITE;

	// Points
	PrintY = 85;
	WindowX = 81;
	WindowW = 64;
	//US_CPrint("POINTS");
	SM_CPrint(S_POINTS);
	VWB_Bar(79, 95, 66, 10, BLACK);
	CK_DrawLongRight(80, 96, 8, 16, points);

	// Key gems
	PrintX = 194;
	PrintY = 85;
	//US_Print("KEYS");
	SM_Print(S_KEYS);

	VWB_Bar(175, 95, 66, 10, BLACK);

	for (i = 1, x=184; i < (1<<4); i<<=1, x+=13)
	{
		if (keyBits & i)
			DrawTile8(x, 96, 5);
		else
			DrawTile8(x, 96, 4);
	}

	// AMMO
	PrintX = 80;
	PrintY = 144;
	//US_Print("AMMO");
	SM_Print(S_AMMO);
	VWB_Bar(127, 143, 18, 10, BLACK);
	if (ammo)
	{
		// calculate distance to player (in pixels):
		bestAmmoX = RF_TileToUnit(bestAmmoX)+8*PIXGLOBAL - ck_keenObj->clipRects.unitXmid;
		bestAmmoY = RF_TileToUnit(bestAmmoY)+8*PIXGLOBAL - (ck_keenObj->clipRects.unitY2-16*PIXGLOBAL);
		bestAmmoX >>= G_P_SHIFT;
		bestAmmoY >>= G_P_SHIFT;

		// get compass direction tile based on distance values:
		if (bestAmmoY < 0)
		{
			bestAmmoY = -bestAmmoY;
			if (bestAmmoX < 0)
			{
				// north/west quadrant:
				bestAmmoX = -bestAmmoX;
				if (bestAmmoY > 2*bestAmmoX)
				{
					x = 28;	// north
				}
				else if (bestAmmoX > 2*bestAmmoY)
				{
					x = 34;	// west
				}
				else
				{
					x = 35;	// northwest
				}
			}
			else
			{
				// north/east quadrant:
				if (bestAmmoY > 2*bestAmmoX)
				{
					x = 28;	// north
				}
				else if (bestAmmoX > 2*bestAmmoY)
				{
					x = 30;	// east
				}
				else
				{
					x = 29;	// northeast
				}
			}
		}
		else
		{
			if (bestAmmoX < 0)
			{
				// south/west quadrant:
				bestAmmoX = -bestAmmoX;
				if (bestAmmoY > 2*bestAmmoX)
				{
					x = 32;	// south
				}
				else if (bestAmmoX > 2*bestAmmoY)
				{
					x = 34;	// west
				}
				else
				{
					x = 33;	// southwest
				}
			}
			else
			{
				// south/east quadrant:
				if (bestAmmoY > 2*bestAmmoX)
				{
					x = 32;	// south
				}
				else if (bestAmmoX > 2*bestAmmoY)
				{
					x = 30;	// east
				}
				else
				{
					x = 31;	// southeast
				}
			}
		}
		// draw the compass tile:
		DrawTile8(128+4, 144, x);
	}
	else
	{
		// draw blank compass (no ammo left):
		DrawTile8(128+4, 144, 27);
	}

	// Lives
	PrintX = 80;
	PrintY = 112;
	//US_Print("LIVES");
	SM_Print(S_LIVES);
	VWB_Bar(127, 111, 18, 10, BLACK);
	CK_DrawLongRight(128, 112, 2, 16, lives);

	// Lifeups
	PrintX = 80;
	PrintY = 128;
	//US_Print("DROPS");
	SM_Print(S_DROPS);
	VWB_Bar(127, 127, 18, 10, BLACK);
	CK_DrawLongRight(128, 128, 2, 16, drops);

	PrintY = 117;
	WindowX = 177;
	WindowW = 64;
	//US_CPrint("GOODIES");
	SM_CPrint(S_GOODIES);
	{
		unsigned spr;
		spr = COLLECTIBLETEDDY1SPR;
		VWB_Bar(175, 127, 66, 10, BLACK);
		VWB_Bar(175, 143, 66, 10, BLACK);
		x = 182;
		for (i=1; i<(1<<3); i<<=1, x+=18, spr += 2)
		{
			ClipSprite(x, 128, spr, 1, 8, (collectibles & i)? lightgraydraw : darkgraydraw);
		}
		x = 182;
		for (; i<(1<<6); i<<=1, x+=18, spr += 2)
		{
			ClipSprite(x, 144, spr, 1, 8, (collectibles & i)? lightgraydraw : darkgraydraw);
		}
	}

	fontcolor = oldcolor;


}

void CK_ScrollStatusWindow(void)
{
	int dest, height, source;

	if (ck_statusWindowYPx > 152)
	{
		height = ck_statusWindowYPx - 152;
		source = masterofs + panadjust + 8;	// K1n9_Duk3 fix : use masterofs instead of statusWindowOfs
		dest = bufferofs + panadjust + 8;
		VW_ScreenToScreen(source, dest, 24, height);

		VW_ClipDrawMPic((pansx + 136)/8, pansy - (16-height), STATUSTOPPICM);
		source = statusWindowOfs + panadjust + 0x408;
		dest = bufferofs + panadjust + (height << 6) + 8;
		height = 152;
	}
	else 
	{
		source = statusWindowOfs + panadjust + ((152-ck_statusWindowYPx)<<6) + 0x408;
		dest = bufferofs + panadjust + 8;
		height = ck_statusWindowYPx;
	}

	if (height > 0)
		VW_ScreenToScreen(source, dest, 24, height);

	// Draw the tile map underneath the scrolling status box
	if (ck_statusDown)
	{
		// Coming back up, need to redraw the map back underneath
		height = 168 - ck_statusWindowYPx;
		source = masterofs + panadjust + (ck_statusWindowYPx << 6) + 8;
		dest = bufferofs + panadjust + (ck_statusWindowYPx << 6) + 8;
		VW_ScreenToScreen(source, dest, 24, height);

		// Draw underneath the left masked pic
		//height = ck_statusWindowYPx;	// causes issues at 14 fps or less
		if ((height = ck_statusWindowYPx + 8*MAXTICS*2-72) > 96)
			height = 96;	// K1n9_Duk3 fix for extremely low frame rates
		source = masterofs + panadjust + 8 - 3;	// K1n9_Duk3 fix : use masterofs instead of statusWindowOfs
		dest = bufferofs + panadjust + 8 - 3;

		if (height > 0)
			VW_ScreenToScreen(source, dest, 3, height);

	}
	else
	{
		// Going down
		height = ck_statusWindowYPx - 72;

		if (height > 0)
		{
			source = masterofs + panadjust + 8 - 3;	// K1n9_Duk3 fix : use masterofs instead of statusWindowOfs
			dest = bufferofs + panadjust + 8 - 3;
			if (height > 0) // ??
				VW_ScreenToScreen(source, dest, 3, height);
		}
	}

	if (ck_statusWindowYPx >= 72)
		VW_ClipDrawMPic((pansx + 40)/8, pansy + ck_statusWindowYPx - 168, STATUSLEFTPICM);

 	VW_UpdateScreen();


}

extern void RFL_InitAnimList(void);
void CK_ShowStatusWindow(boolean isDetector)
{

	int oldBufferofs;
	
	WindowX = 0;
	WindowW = 320;
	WindowY = 0;
	WindowH = 200;

	// K1n9_Duk3 mod: debug keys are always active, so we dont' need this check anymore
	/*
	// This function is called when enter pressed; check for A+2 to enable debug mode
	if (Keyboard[sc_A] && Keyboard[sc_2])
	{
		US_CenterWindow(18, 2);
		PrintY+=2;
		//US_Print("Debug keys active");
		SM_Print(S_DEBUGKEYSACTIVE);
		VW_UpdateScreen();
		IN_Ack();//IN_WaitButton();
		// debugactive = 1;
		return;
	}
	*/



	RF_Refresh();

	// Clear out all animating tiles
	RFL_InitAnimList();

	// Draw the status window to scratch area in the buffer
	oldBufferofs = bufferofs;
	bufferofs = statusWindowOfs = RF_FindFreeBuffer();
	//VW_ScreenToScreen(displayofs, displayofs, 44, 224);	// K1n9_Duk3 mod: this appears to be useless anyway
	VW_ScreenToScreen(displayofs, masterofs, 44, 224);
	//VW_ScreenToScreen(displayofs, bufferofs, 44, 168);	// K1n9_Duk3 mod: don't need this anymore
	if (isDetector)
		CK_DrawDetectorWindow();
	else
		CK_DrawStatusWindow();
	bufferofs = oldBufferofs;
	RF_Refresh();
	
	// Scroll the window down
	SD_PlaySound(STATUSDOWNSND);
	ck_statusWindowYPx = 16;
	ck_statusDown = false;
	RF_SetRefreshHook(CK_ScrollStatusWindow);
	do 
	{
		RF_Refresh();
		if (ck_statusWindowYPx == 168)
			break;


		if ((ck_statusWindowYPx += tics*8) <= 168)
			continue;

		ck_statusWindowYPx = 168;

	} while (1);

	RF_Refresh();
	RF_SetRefreshHook(NULL);
	IN_ClearKeysDown();
	IN_Ack();//IN_WaitButton();


	// Scroll the window up
	SD_PlaySound(STATUSUPSND);
	ck_statusWindowYPx -= 16;
	ck_statusDown = true;
	RF_SetRefreshHook(CK_ScrollStatusWindow);

	do 
	{
		RF_Refresh();
		if (ck_statusWindowYPx == 0)
			break;


		if ((ck_statusWindowYPx -= tics*8) >= 0)
			continue;

		ck_statusWindowYPx = 0;

	} while (1);

	RF_SetRefreshHook(NULL);

	ck_scoreBoxObj->posX = 0;

}


//===========================================================================
//
// DEBUG ROUTINES AND INPUT HANDLING
//
//===========================================================================

// Nisaba patch: deactive debugging codes for final release
#ifdef DEVBUILD

void CK_CountActiveObjects()
{
	unsigned activeobjects, inactiveobjects;
	CK_object *ob;

	activeobjects = inactiveobjects = 0;
	for (ob=ck_keenObj; ob; ob=ob->next)
	{
		if (ob->active)
		{
			activeobjects++;
		}
		else
		{
			inactiveobjects++;
		}
	}
	VW_FixRefreshBuffer();
	US_CenterWindow(18, 8);
	PrintY += 7;
	//US_Print("Active Objects: ");
	SM_Print(S_ACTIVEOBJECTS);
	US_PrintUnsigned(activeobjects);
	//US_Print("\nInactive Objects: ");
	SM_Print(S_INACTIVEOBJECTS);
	US_PrintUnsigned(inactiveobjects);
	//US_Print("\nObjects Total: ");
	SM_Print(S_OBJECTSTOTAL);
	US_PrintUnsigned(ck_numObjects);
	SM_Print(S_SPRITESUSED);
	US_PrintUnsigned(RF_SpritesUsed());
	SM_Print(S_SPRITESFREE);
	US_PrintUnsigned(RF_SpritesAvailable());
	VW_UpdateScreen();
	IN_Ack();}

void DebugMemory (void)
{
#if 0
	// original code:
	/*
	VW_FixRefreshBuffer ();
	US_CenterWindow (16,7);
	*/
	// K1n9_Duk3 mod (more detailed info):
	VW_FixRefreshBuffer();
	US_CenterWindow(16, 10);

	US_CPrint ("Memory Usage");
	US_CPrint ("------------");
	US_Print ("Total     :");
	US_PrintUnsigned ((mminfo.mainmem+mminfo.EMSmem+mminfo.XMSmem)/1024);
	US_Print ("k\nFree      :");
	US_PrintUnsigned (MM_UnusedMemory()/1024);
	US_Print ("k\nWith purge:");
	US_PrintUnsigned (MM_TotalFree()/1024);
	US_Print ("k\n");
	VW_UpdateScreen();
	IN_Ack ();
#else
	// K1n9_Duk3 mod (more detailed info):
	VW_FixRefreshBuffer();
	US_CenterWindow(16, 8);
	//US_CPrint("Memory Usage");
	//US_CPrint("------------");
	SM_CPrint(S_MEMUSAGE);
	//US_Print("Total:");
	SM_Print(S_MEMTOTAL);
	PrintX = WindowX+80;
	US_PrintUnsigned((mminfo.mainmem+mminfo.EMSmem+mminfo.XMSmem));
	//US_Print("\nFree:");
	SM_Print(S_MEMFREE);
	PrintX = WindowX+80;
	US_PrintUnsigned(MM_UnusedMemory());
	//US_Print("\nWith purge:");
	SM_Print(S_WITHPURGE);
	PrintX = WindowX+80;
	US_PrintUnsigned(MM_TotalFree());
	//US_Print("\nUsed:");
/*	
	SM_Print(S_MEMUSED);
	PrintX = WindowX+80;
	US_PrintUnsigned((mminfo.mainmem+mminfo.EMSmem+mminfo.XMSmem)-MM_TotalFree());
*/	
	SM_Print(S_MEMUSED);
	PrintX = WindowX+80;
	US_PrintUnsigned((mminfo.mainmem+mminfo.EMSmem+mminfo.XMSmem)-MM_TotalFree());
	{
		unsigned blocks;
		
		SM_Print(S_BLOCKSFREE);
		blocks = MM_BlocksFree();
		PrintX = WindowX+80;
		US_PrintUnsigned(blocks);
		SM_Print(S_WITHPURGE);
		blocks += MM_BlocksPurgable();
		PrintX = WindowX+80;
		US_PrintUnsigned(blocks);
	}
/*
	//
	// show how large the stack can get:
	//
	// stack space must be around 1500 bytes or more to provide enough
	// space for the local variables in the FizzleDown routine and the
	// routines that call it
	//
	{
		extern void *nearheap;
		PrintX = WindowX+80;
		PrintY+=10;
		US_PrintUnsigned(_SP-(FP_OFF(nearheap)+mminfo.nearheap));
	}
*/
	VW_UpdateScreen();
	IN_Ack();
#endif
#if GRMODE == EGAGR
	MM_ShowMemory();
/*
	// Quick test
	StopMusic();
	{
		memptr foo = NULL;
		MM_BombOnError(false);
		MM_GetPtr(&foo, MM_TotalFree()-64);
		MM_BombOnError(true);
		if (mmerror)
		{
			mmerror = false;
			SD_PlaySound(NOWAYSND);
		}
		if (foo)
			MM_FreePtr(&foo);
		MM_ShowMemory();
	}
	StartMusic(ck_mapState.music, false);
*/
#endif
}

void MergeTile16M(unsigned dest, unsigned source)
{
	unsigned _seg *sourceseg;
	unsigned _seg *destseg;
	unsigned sourceval, sourcemask, maskindex;
	int i;

//	CA_CacheGrChunk(STARTTILE16M+source);	// caching and uncaching in here requires less memory but takes longer

	sourceseg = grsegs[STARTTILE16M+source];
	destseg = grsegs[STARTTILE16M+dest];
	for (i=0; i<64; i++)
	{
		maskindex = i & 15;
		sourceval = sourceseg[16+i];
		sourcemask = sourceseg[maskindex];
		destseg[maskindex] &= sourcemask;
		destseg[16+i] &= sourcemask;
		destseg[16+i] |= sourceval;
	}

//	CA_UncacheGrChunk(STARTTILE16M+source);
}

void near ShowWorkingMessage(void)
{
	VW_FixRefreshBuffer();
	US_CenterWindow(24, 3);
	//US_PrintCentered("WORKING");
	SM_PrintCentered(S_WORKING);
	VW_UpdateScreen();
}

// TODO: make this interoperable between episodes
void CK_WallDebug()
{
#define WALLTILESTART 0
#define NUMWALLTILES  18	// 7 tops + 7 bottoms + 2 right + 2 left
#define WALLTILEEND   (WALLTILESTART+NUMWALLTILES)
	int i, val;

	ShowWorkingMessage();

	ck_gameState.cheated = true;
	wallcheat |= 1;

	// cache the collision icons:
	for (i=STARTTILE16M+WALLTILESTART; i<STARTTILE16M+WALLTILEEND; i++)
	{
		CA_CacheGrChunk(i);
	}

	// apply the collision icons to all tile graphics in the cache
	for (i=0; i<NUMTILE16M; i++)
	{
		if (!grsegs[STARTTILE16M+i])
		{
			continue;
		}
		val = tinf[i+NORTHWALL] & 7;
		if (val)
		{
			MergeTile16M(i, val+WALLTILESTART-1);
		}
		val = tinf[i+SOUTHWALL] & 7;
		if (val)
		{
			MergeTile16M(i, val+WALLTILESTART+7-1);
		}
		val = tinf[i+EASTWALL];
		if (val > 2)
		{
			/*
			//strcpy(str, "WallDebug: East wall > 2:");
			SM_CopyString(str, S_WALLDEBUGEASTWALL);
			itoa(i, str2, 10);
			strcat(str, str2);
			Quit(str);
			*/
			val = 2;
		}
		if (val)
		{
			MergeTile16M(i, val+WALLTILESTART+14-1);
		}
		val = tinf[i+WESTWALL];
		if (val > 2)
		{
			/*
			//strcpy(str, "WallDebug: West wall > 2:");
			SM_CopyString(str, S_WALLDEBUGWESTWALL)
			itoa(i, str2, 10);
			strcat(str, str2);
			Quit(str);
			*/
			val = 2;
		}
		if (val)
		{
			MergeTile16M(i, val+WALLTILESTART+16-1);
		}
	}
	// uncache the collision icons again (they take up 2880 bytes of memory)
	for (i=STARTTILE16M+WALLTILESTART; i<STARTTILE16M+WALLTILEEND; i++)
	{
		CA_UncacheGrChunk(i);
	}
#undef WALLTILESTART
#undef WALLTILEEND
#undef NUMWALLTILES
}

#define DISPWIDTH	110
#define	TEXTWIDTH   40
void TestSprites(void)
{
	int hx,hy,sprite,oldsprite,bottomy,topx,shift;
	spritetabletype far *spr;
	spritetype _seg	*block;
	unsigned	mem,scan;


	VW_FixRefreshBuffer ();
	US_CenterWindow (30,17);

	//US_CPrint ("Sprite Test");
	//US_CPrint ("-----------");
	SM_CPrint(S_SPRITETEST);

	hy=PrintY;
	hx=(PrintX+56)&(~7);
	topx = hx+TEXTWIDTH;

	//US_Print ("Chunk:\nWidth:\nHeight:\nOrgx:\nOrgy:\nXl:\nYl:\nXh:\nYh:\n"
	//		  "Shifts:\nMem:\n");
	SM_Print(S_SPRITETESTSTUFF);

	bottomy = PrintY;

	sprite = STARTSPRITES;
	shift = 0;

	do
	{
		if (sprite>=STARTTILE8)
			sprite = STARTTILE8-1;
		else if (sprite<STARTSPRITES)
			sprite = STARTSPRITES;

		spr = &spritetable[sprite-STARTSPRITES];
		block = (spritetype _seg *)grsegs[sprite];

		VWB_Bar (hx,hy,TEXTWIDTH,bottomy-hy,WHITE);

		PrintX=hx;
		PrintY=hy;
		US_PrintUnsigned (sprite-STARTSPRITES);US_Print ("\n");PrintX=hx;	// K1n9_Duk3 mod: display sprite number, not chunk number
		US_PrintUnsigned (spr->width);US_Print ("\n");PrintX=hx;
		US_PrintUnsigned (spr->height);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->orgx);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->orgy);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->xl);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->yl);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->xh);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->yh);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->shifts);US_Print ("\n");PrintX=hx;
		if (!block)
		{
			US_Print ("-----");
		}
		else
		{
		/*
			mem = block->sourceoffset[3]+5*block->planesize[3];
			mem = (mem+15)&(~15);		// round to paragraphs
			US_PrintUnsigned (mem);
		*/
			mem = block->sourceoffset[MAXSHIFTS-1]+5*block->planesize[MAXSHIFTS-1];
			mem = (mem+15)&(~15);		// round to paragraphs
			US_PrintUnsigned (mem);
		}
		oldsprite = sprite;
		do
		{
		//
		// draw the current shift, then wait for key
		//
			VWB_Bar(topx,hy,DISPWIDTH,bottomy-hy,WHITE);
			if (block)
			{
				PrintX = topx;
				PrintY = hy;
				//US_Print ("Shift:");
				SM_Print(S_SHIFT);
				US_PrintUnsigned (shift);
				//US_Print ("\n");
				SM_Print(S_NEWLINE);
				VWB_DrawSprite (topx+16+shift*2,PrintY,sprite);
			}

			VW_UpdateScreen();

			scan = IN_WaitForKey ();

			switch (scan)
			{
			case sc_UpArrow:
				sprite++;
				break;
			case sc_DownArrow:
				sprite--;
				break;
			case sc_LeftArrow:
				if (--shift == -1)
					shift = 3;
				break;
			case sc_RightArrow:
				if (++shift == 4)
					shift = 0;
				break;
			// K1n9_Duk3 addition:
			case sc_Enter:
				if (!block)
				{
					CA_CacheGrChunk(sprite);
					grneeded[sprite]&=~ca_levelbit;
					block = (spritetype _seg *)grsegs[sprite];
					oldsprite = -1;	//force update
				}
				break;
			// end of addition
			case sc_Escape:
				return;
			}

		} while (sprite == oldsprite);

  } while (1);


}

#define SOUNDTEST

#ifdef SOUNDTEST
void CK_TestSound(void)
{
	int oldsound, sound;
	int hx, hy, bottomy;
	int scan;

	VW_FixRefreshBuffer ();
	US_CenterWindow (20,5);

	//US_CPrint ("Sound Test");
	//US_CPrint ("-----------");
	SM_CPrint(S_SOUNDTEST);

	hy=PrintY;
	hx=(PrintX+56)&(~7);

	//US_Print ("Sound:\n");
	SM_Print(S_SOUNDNUM);

	bottomy = PrintY;

	do
	{
		if (sound>=NUMSOUNDS)
			sound = NUMSOUNDS-1;
		else if (sound<0)
			sound = 0;

		VWB_Bar (hx,hy,TEXTWIDTH,bottomy-hy,WHITE);

		PrintX=hx;
		PrintY=hy;
		US_PrintUnsigned (sound);/*US_Print ("\n");PrintX=hx;*/

		oldsound = sound;
		do
		{
			VW_UpdateScreen();

			scan = IN_WaitForKey ();

			switch (scan)
			{
				case sc_UpArrow:
					sound++;
					break;
				case sc_DownArrow:
					sound--;
					break;
				case sc_Enter:
					SD_StopSound();
					SD_PlaySound(sound);
					break;
				case sc_Escape:
					return;
			}

		} while (sound == oldsound);

	} while (1);

}
#endif


void UndoWallDebug(void)
{
	register int i;

	// re-load tile attributes:
	if (wallcheat & 2)
		//CA_ReadFile("MAPHEAD."EXTENSION, (memptr*)&tinf);
		CA_ReadFile(SM_GetString(S_MAPHEADFILE), (memptr*)&tinf);

	// re-load tile images:
	if (wallcheat & 1)
	{
		for (i=STARTTILE16M; i<STARTTILE16M+NUMTILE16M; i++)
		{
			if (grsegs[i])
			{
				MM_FreePtr(&grsegs[i]);
				CA_CacheGrChunk(i);
			}
		}
	}
	wallcheat = 0;
}
#endif 

void CK_ItemCheat()
{
	VW_FixRefreshBuffer ();
	US_CenterWindow (12,3);
	//US_PrintCentered ("Free items!");
	SM_PrintCentered(S_FREEITEMS);
	ck_gameState.numShots = MAXAMMO;
	//ck_gameState.securityCard=1;	// only in Keen 5
	ck_mapState.keyGems[0] =
	ck_mapState.keyGems[1] =
	ck_mapState.keyGems[2] =
	ck_mapState.keyGems[3] = 1;
	VW_UpdateScreen();
	IN_Ack ();
	ck_gameState.cheated = true;
}

boolean CK_DebugKeys()
{
	boolean esc;
	int level;

	// NOTE: Following the update to the ID Engine from Catacomb 3-D,
	// some cheat codes have further been imported from the same game,
	// while some additional code has been restored separately.
	// Furthermore, scancode definitions from ID_IN.H are used
	// used now instead of scancode values hardcoded here.

	if (Keyboard[sc_B] && ingame)		// B = border color
	{
		US_CenterWindow(24,3);
		PrintY+=6;
		//US_Print(" Border color (0-15):");
		SM_Print(S_BORDERCOLOR);
		VW_UpdateScreen();
		esc = !US_LineInput (px,py,str,NULL,true,2,0);
		if (!esc)
		{
			level = atoi (str);
			if (level>=0 && level<=15)
				VW_ColorBorder (level);
		}
		return 1;
	}

#ifdef DEVBUILD
	if (Keyboard[sc_D] && ingame)		// D = start / end demo record
	{
		if (DemoMode == demo_Off)
			CK_StartDemoRecord ();
		else if (DemoMode == demo_Record)
		{
			CK_EndDemoRecord ();
			ck_levelState = LS_LevelComplete;
		}
		return 1;
	}
#if 1

	if (Keyboard[sc_C] && ingame)		// DEBUG: C = count objects
	{
		CK_CountActiveObjects();
		return 1;
	}

#endif
#endif

	if (Keyboard[sc_E] && ingame)	// DEBUG: end + 'E' to quit level
	{
		/*
		if (tedlevel)
			TEDDeath();
		*/
		ck_levelState = LS_LevelComplete;
		ck_gameState.cheated = true;
	}

	if (Keyboard[sc_G] && ingame)		// G = god mode
	{
		VW_FixRefreshBuffer ();
		US_CenterWindow (12,2);
		if (ck_gameState.godMode)
		  //US_PrintCentered ("God mode OFF");
		  SM_PrintCentered(S_GODMODE_OFF);
		else
		  //US_PrintCentered ("God mode ON");
		  SM_PrintCentered(S_GODMODE_ON);
		VW_UpdateScreen();
		IN_Ack();
		ck_gameState.godMode ^= 1;
		ck_gameState.cheated = true;
		return 1;
	}
	else if (Keyboard[sc_I] && ingame)			// I = item cheat
	{
		CK_ItemCheat();
		ck_gameState.numLives++;
		return 1;
	}
	else if (Keyboard[sc_J] && ingame)			// J = jump cheat
	{
		ck_gameState.jumpCheat^=1;
		VW_FixRefreshBuffer ();
		US_CenterWindow (18,3);
		if (ck_gameState.jumpCheat)
			//US_PrintCentered ("Jump cheat ON");
			SM_PrintCentered(S_JUMPCHEAT_ON);
		else
			//US_PrintCentered ("Jump cheat OFF");
			SM_PrintCentered(S_JUMPCHEAT_OFF);
		VW_UpdateScreen();
		IN_Ack ();
		ck_gameState.cheated = true;
		return 1;
	}

	else if (Keyboard[sc_N] && ingame)			// N = No clipping
	{
		VW_FixRefreshBuffer ();
		US_CenterWindow (18,3);
		if (ck_keenObj->clipped)
		{
			//US_PrintCentered ("No clipping ON");
			SM_PrintCentered(S_NOCLIP_ON);
			ck_keenObj->clipped = false;
		}
		else
		{
			//US_PrintCentered ("No clipping OFF");
			SM_PrintCentered(S_NOCLIP_OFF);
			ck_keenObj->clipped = true;
		}
		VW_UpdateScreen();
		IN_Ack ();
		ck_gameState.cheated = true;
		return 1;
	}
	
	else if (Keyboard[sc_P] && ingame)			// P = pause with no screen disruptioon
	{
		IN_Ack();
		return 1;
	}
	
//===========================================================================
//	
// DEBUG: Debugging keys only meant for develoment phase 
//
//===========================================================================

#ifdef DEVBUILD
	else if (Keyboard[sc_M] && ingame)		// DEBUG: M = memory info
	{
		DebugMemory();
		return 1;
	}
#ifdef SOUNDTEST
	else if (Keyboard[sc_O] && ingame)		// DEBUG: O = sound test
	{
		CK_TestSound();
		return 1;
	}
#endif
	else if (Keyboard[sc_P] && ingame)		// P = pause with no screen disruptioon
	{
		IN_Ack();
		return 1;
	}
	else if (Keyboard[sc_S] && ingame)		// DEBUG: S = slow motion
	{
		ck_gameState.slowMotionEnabled^=1;
		VW_FixRefreshBuffer ();
		US_CenterWindow (18,3);
		if (ck_gameState.slowMotionEnabled)
			//US_PrintCentered ("Slow motion ON");
			SM_PrintCentered(S_SLOMO_ON);
		else
			//US_PrintCentered ("Slow motion OFF");
			SM_PrintCentered(S_SLOMO_OFF);
		VW_UpdateScreen();
		IN_Ack ();
		ck_gameState.cheated = true;
		return 1;
	}
	else if (Keyboard[sc_T])			// DEBUG: T = sprite test
	{
		TestSprites();
		return 1;
	}
	else if (Keyboard[sc_Y])	// DEBUG: Walldebug
	{
		CK_WallDebug();
		IN_ClearKeysDown();
		return 1;
	}
	else if (Keyboard[sc_F])	// DEBUG: Wallcheat, display objects infront of tile layer
	{
		register int i;

		ShowWorkingMessage();

		wallcheat |= 2;

		for (i=0; i<NUMTILE16M; i++)
		{
			tinf[INTILE+i] &= 0x7F;	// erase the foreground bit for all tiles
		}
		IN_ClearKeysDown();
		ck_gameState.cheated = true;
		return 1;
	}
	else if (Keyboard[sc_U])	// DEBUG: undo WallDebug and restore foreground bits:
	{
		ShowWorkingMessage();

		UndoWallDebug();

		IN_ClearKeysDown();
		return 1;
	}
	else if (Keyboard[sc_V])			// V = extra VBLs
	{
		US_CenterWindow(30,3);
		PrintY+=6;
		//US_Print("  Add how many extra VBLs(0-8):");
		SM_Print(S_EXTRAVBLS);
		VW_UpdateScreen();
		esc = !US_LineInput (px,py,str,NULL,true,2,0);
		if (!esc)
		{
			level = atoi (str);
			if (level>=0 && level<=8)
				extravbls = level;
		}
		return 1;
	}
#endif	
	
	else if (Keyboard[sc_W] && ingame)	// W = warp to level
	{
		VW_FixRefreshBuffer ();
		US_CenterWindow(26,3);
		PrintY+=6;
		//US_Print("  Warp to which level(0-17):");
		SM_Print(S_LEVELWARP);
		VW_UpdateScreen();
		esc = !US_LineInput (px,py,str,NULL,true,2,0);
		if (!esc)
		{
			level = atoi (str);
	//		if (level>=0 && level<=NUMMAPS)  // develoment phase, ToDo replace NUMMAPS with actual levelnumber (=17)
			if (level>=0 && level<=17)
			{
				ck_gameState.currentLevel = level;
				ck_levelState = LS_WarpToLevel;
				ck_gameState.cheated = true;
			}
		}
		return 1;
	}
	return 0;
}

static void near pascal DoMenu(MenuType type)
{
	VW_FixRefreshBuffer();
	US_ControlPanelEx(type);
	
	if (restartgame)
	{
		ck_levelState = LS_StartingNew;	// K1n9_Duk3 fix: use enum name instead of number
	}
	else if (loadedgame)
	{
		ck_levelState = LS_StartingSaved;	// K1n9_Duk3 fix: use enum name instead of number
	}
	else if (abortgame)
	{
		ck_levelState = LS_Aborted;	// K1n9_Duk3 fix: use enum name instead of number
	}
	else
	{
		// resuming current game
		ContinueMusic(ck_mapState.music);
		
		CK_LevelPalette();
		CK_ForceScoreboxUpdate();
		IN_ClearKeysDown();
		RF_ForceRefresh(); 
	}
	abortgame = false;
}

void CK_CheckKeys(void)
{
	// TODO: Also check for a gamepad button when relevant

	if (screenfaded)
		return;

	// Drop down status
	if (Keyboard[config.statusscan])
	{
		CK_ShowStatusWindow(false);
		IN_ClearKeysDown();
		RF_ForceRefresh();
		//TimeCount = lasttimecount; // K1n9_Duk3 fix: useless/not required after RF_ForceRefresh()
	}
	else if (Keyboard[config.detectorscan])
	{
		CK_ShowStatusWindow(true);
		IN_ClearKeysDown();
		RF_ForceRefresh();
	}

	if (IN_KeyDown(config.scoreboxscan))
	{
		IN_ClearKey(config.scoreboxscan);
		showscorebox ^= true;
		CK_ForceScoreboxUpdate();
	}

	// TODO: If Paused
	if (Paused)
	{
		SD_MusicOff();
		VW_FixRefreshBuffer();
		US_CenterWindow(8, 3);
		//US_PrintCentered("PAUSED");
		SM_PrintCentered(S_PAUSED);
		VW_UpdateScreen(); 
		IN_Ack();//IN_WaitButton();
		RF_ForceRefresh();
		Paused = false;
		SD_MusicOn();
	}

	// HELP
	if (LastScan == sc_F1)
	{
		StopMusic();

		//CK_JanitorMonolog();	// TODO: remove this for the final release!

		HelpScreens();
		//StartMusic(ck_mapState.music, true);
		ContinueMusic(ck_mapState.music);	// K1n9_Duk3 mod

		// Force scorebox redraw if it's enabled
		CK_ForceScoreboxUpdate();

		CK_LevelPalette();

		RF_ForceRefresh();

	}

	if (!ck_demoParm)
	{
		if (LastScan == sc_Escape)
		{
			DoMenu(mt_Normal);
		}
		else if (LastScan == config.savemenuscan)
		{
			DoMenu(mt_Save);
		}
		else if (LastScan == config.loadmenuscan)
		{
			DoMenu(mt_Load);
		}
		else if (LastScan == config.quicksavescan)
		{
			DoMenu(mt_QuickSave);
		}
		else if (LastScan == config.quickloadscan)
		{
			DoMenu(mt_QuickLoad);
		}
		
	#ifdef MINIGAME2
		// Do Boss Key			// TODO: include second minigame SNAILA5
		if (LastScan == sc_F8)
		{	
			// switch to SNAILA5 minigame
			VW_FixRefreshBuffer();
			//StopMusic();
			US_CenterWindow(26,11);
			if (!grsegs[TRANSCEIVERPIC])
			{
				CA_CacheGrChunk(TRANSCEIVERPIC);
				CA_UncacheGrChunk(TRANSCEIVERPIC);	// Note: this just makes the chunk purgable, it can still be drawn
			}
   			VWB_DrawPic(190, 74, TRANSCEIVERPIC);
			RunSnailas();
			IN_Ack();//IN_WaitButton();
			
			// resuming current game
			//StartMusic(ck_mapState.music, true);
			CK_LevelPalette();
			CK_ForceScoreboxUpdate();
			IN_ClearKeysDown();
			RF_ForceRefresh();
			fontcolor = WHITE; 
			
			/*			
			// Alternative way:
			CK_DialogEnter();
	
			CA_CacheGrChunk(STARTFONT+1);
			fontnumber = 1;	// use menu font
			
			// draw a dark gray "window" with a white 1-pixel-thick frame:
			VWB_Bar(75-1, 49-1, 159+2, 101+2, WHITE);
			VWB_Bar(75, 49, 159, 101, DARKGRAY);
			
			// cache and draw title pic:
			CA_CacheGrChunk(CP_PADDLEWARPIC);
			VWB_DrawPic(74 + 56, 48, CP_PADDLEWARPIC);
			
			PlayMinigame();
			
			IN_ClearKeysDown();
			
			fontnumber = 0;
			fontcolor = WHITE;
			
			CK_DialogExit();
			*/
		}
	#endif
	}

	// BAT ITEM CHEAT
	if (Keyboard[sc_B] && Keyboard[sc_A] && Keyboard[sc_T])
	{
		CK_ItemCheat();
		RF_ForceRefresh();
		//TimeCount = lasttimecount; // K1n9_Duk3 fix: useless/not required after RF_ForceRefresh()
		ck_gameState.numLives++;
	}

	// Debug Keys
	if (Keyboard[sc_F10])
	{
		if (CK_DebugKeys())
		{
			RF_ForceRefresh(); //RF_ResetScreen();
			//TimeCount = lasttimecount;	 // K1n9_Duk3 fix: useless/not required after RF_ForceRefresh()
		}
	}
/*	
	// TODO: delete before release
	if (LastScan == sc_F11)	// only to test certain conditions and mechanics

		ck_gameState.keenScore = 1000000;	// Perfect Score
	//	ck_gameState.keenScore =  500000;	// Unlock Bonus Level
	//	ck_gameState.keenScore =   20000;	// Random Score
	//	ck_gameState.keenScore =    1000;	// Minimum Score
	//	ck_gameState.cheated = true;
		ck_gameState.slugcans = 17;
	//	ck_gameState.slugcans = 11;
	//	ck_gameState.slugcans =  7;
	//	ck_gameState.collectiblesTotal = 0x3F;
		ck_gameState.collectiblesTotal = 0;
	//	ck_gameState.levelsDone[MAP_BONUSLEVEL] = true;
		ck_gameState.levelsDone[MAP_TARDISLEVEL] = true;
		ck_gameState.levelsDone[MAP_SLUGBUSTSLEVEL] = true;
	//	ck_mapState.machinesActive = 1;	// Cloned one Monster
	//	ck_mapState.machinesActive = 0;	// Cloned zero Monsters
		ck_levelState = LS_ClonedJanitor;	// Win Game
	//	RunSnailas();
	}
*/
	// TODO: CTRL+S sound

	// CTRL + Q
	if (Keyboard[sc_Control] && LastScan == sc_Q)
	{
		IN_ClearKeysDown();
		Quit(NULL);
	}
}


/*
======================================
=
= CK_JukeBox	(K1n9_Duk3 addition)
=
======================================
*/

// Here's the playlist for the jukebox. Feel free to change the order of the
// songs as you please. You can delete the songs you don't want.
// (The names were copied from the musicnames enum. The order was changed around.)
static musicnames playlist[] = {
	TSK_Keenish_Tune,              // 0
	Kohntrakosz__Nashville,        // 31
	Kohntrakosz__Commercials_v3,   // 1
	MOM4Evr_The_Lady_is_Frightful, // 2
	MR_M_Nitelite,                 // 3
	kvee_Oasis_v17,                // 4
	kvee_Slowasis_v4,              // 24
	BobbyPrince_Jazzin_Them_Nazis, // 5
	BobbyPrince_Guess,             // 6
	MOM4Evr_Algeria,               // 7
	unknown_JAZZK,                 // 8
	eggsbox_Fonk,                  // 9
	kvee_2hot_v4,                  // 10
	kvee_Too_Hot_v5b,              // 30
	eggsbox_Spoops,                // 11
	MR_M_Education,                // 12
	Kohntrakosz__Tropic_v2_640,    // 13
	eggsbox_pyrska2,               // 14
	eggsbox_littlekeenthing,       // 15
	eggsbox_fonk_vari_2,           // 33
	eggsbox_fonk_vari_3b,          // 34
	eggsbox_doodle_rmx,            // 35
	kvee_icy_f_10rppq_nisaba_rmx,  // 16
	kvee_icy_f_10rppq,             // 29
	eggsbo_Return_v2,              // 17
	eggsbox_Underground,           // 27
	Gridlock_Colder,               // 18
	Kohntrakosz_Strut_500,         // 19
	TMST_kickkeen,                 // 20
	kvee_Midnight_v5_slow,         // 22
	TheBigV_Hill_Yoshi,            // Worldmap Bonus
	/*
	Summers_Beach,                 // 21
	IncredibleMachine_Cruise,      // 23
	IncredibleMachine_Fusion,      // 25
	LeeJackson_dopejaws,           // 26	
	JosephCollins_Slipping_Slug,   // 28
	MrM_MortLab,                   // 32
	Kohntrakosz__JoJackson,        // 36
	eggsbox_bugs_will_bite,        // 37		
	*/
};

#define NUMSONGS (sizeof(playlist)/sizeof(playlist[0]))

// Note: If you want to turn the unlocked music list from a "per playthrough"
// list into a global list, you need to remove the musicplayed array from the
// CK_GameState struct in CK_DEF.H and add it at the end of the ConfigType
// struct in ID_US.H so that the USL_ReadConfig() and USL_WriteConfig() routines
// will save and load it as part of the config file. I would strongly recommend
// increasing the ConfigVersion constant in ID_US_1.C after this change, because
// the changes to ConfigType have altered the config file format.
// You also need to make CacheMusic() and CK_Jukebox() use 'config.musicplayed'
// instead of 'ck_gameState.musicplayed'.

// Note: caller is responsible for cache level adjustments now!

void CK_Jukebox(void)
{
	int oldsong=-1, song;
	int NameY, NameH;
	CursorInfo cursor;
	
#if 1
	// try to find the current level music in the playlist:
	for (song = 0; song < NUMSONGS && playlist[song] != ck_mapState.music; song++);
	if (song >= NUMSONGS)
		return;	// no music unlocked!
#else
	// we don't really need to search the playlist if the jukebox can only be
	// activated in the BWB level and we know exactly which music is playing
	// in the section with the jukebox a.k.a. Photachion Transceiver:
	song = 2;	// index of the "inside BWB" music in the playlist
#endif

	// caller takes care of this now
	/*
	VW_FixRefreshBuffer ();
	CA_UpLevel();
	CA_SetGrPurge();
	*/
	US_CenterWindow (26,12);
	
	CA_CacheGrChunk(TRANSCEIVERPIC);
	VWB_DrawPic(WindowX, WindowY+8, TRANSCEIVERPIC);
	CA_UncacheGrChunk(TRANSCEIVERPIC);

	//US_CPrint ("The Photachion Transceiver\nis currently playing:");
	SM_CPrint(S_JUKEBOXTOP);
	NameY = PrintY + 2;
	
	PrintY = WindowY + WindowH - 30;
	NameH = PrintY - NameY;
	fontcolor = WHITE ^ 7;
	//US_CPrint("Press Up & Down to change\nEsc to back out");
	SM_CPrint(S_JUKEBOXBOTTOM);
	
	WindowX += 65;
	WindowW -= 65;
	
	// optional: display song info using menu font (might allow longer names):
	CA_CacheGrChunk(STARTFONT+1);
	fontnumber = 1;
	
	for(;;)
	{
		unsigned w, h;
		
		PrintX = WindowX;
		PrintY = NameY;
		
		// text is drawn in XOR mode, so fontcolor must be an XOR combination of
		// background color (WHITE in here) and desired text color:
		if (SD_CurrentMusic() == playlist[song])
			fontcolor = WHITE ^ GREEN;
		else if (ck_gameState.musicplayed[playlist[song]] == 0)
			fontcolor = WHITE ^ RED;
		else
			fontcolor = WHITE ^ BLACK;
		
		VWB_Bar (WindowX,NameY,WindowW,NameH,WHITE);	// erase the name from the back buffer
		//itoa(song, str, 10);	// just testing...
		SM_CopyString(str, S_MUSICNAME0+playlist[song]);	// for the final version
		CK_MeasureMultiline(str, &w, &h);
		PrintY += (NameH - h)/2;
		US_CPrint(str);
		VW_UpdateScreen();	// make the name visible on screen
		
		IN_ClearKeysDown();
		IN_ReadMenuCursor(&cursor);

		oldsong = song;
		do
		{
			IN_ReadMenuCursor(&cursor);
			
			if (cursor.button1)
			{
				do IN_ReadCursor(&cursor); while (cursor.button1);
#if 1
				// make the level play the current music instead of the original song:
				oldsong = ck_mapState.music;
				ck_mapState.music = SD_CurrentMusic();
				
				// optional: replace all music triggers
				#define off NameH
				off = mapwidth*mapheight;
				while (off--)
				{
					if ((TI_ForeMisc(mapsegs[1][off]) & 0x7F) == MF_MusicTrigger)
						if (mapsegs[2][off] == INFONUMBER_0+oldsong)
							mapsegs[2][off] = INFONUMBER_0+ck_mapState.music;
				}
				#undef off
#else
				// back to the level's music:
				if (playlist[song] != ck_mapState.music)
					StartMusic(ck_mapState.music, true);
#endif
				fontnumber = 0;	// back to default font
				fontcolor = F_BLACK;
				// caller takes care of this now
				/*
				IN_ClearKeysDown();
				CA_DownLevel();
				if (ca_levelnum == 0)
				{
					CK_ForceScoreboxUpdate();
					RF_ForceRefresh();
				}
				*/
				return;	
			}
			else if (cursor.button0)
			{
#ifdef DEVBUILD
				// for debugging: Enter unlocks and plays the selected song when God Mode is on
				if (ck_gameState.musicplayed[playlist[song]] == 0 && !ck_gameState.godMode)
#else
				if (ck_gameState.musicplayed[playlist[song]] == 0)
#endif
				{
					SD_PlaySound(NOWAYSND);
				}
				else
				{
					StartMusic(playlist[song], true);
					oldsong = -1;	// redraw title
				}
				do IN_ReadCursor(&cursor); while (cursor.button0);
			}
			else if (cursor.x < 0)
			{
				if (song > 0)
					song--;
			}
			else if (cursor.x > 0)
			{
				if (song < NUMSONGS-1)
					song++;
			}
		} while (song == oldsong);
	}
}

void CK_HandleInput()
{
	IN_ReadControl(0, &ck_inputFrame);

	// K1n9_Duk3 addition:
	if (ck_mapState.nisasi)
	{
		ck_inputFrame.yaxis=ck_inputFrame.y=ck_inputFrame.button0=ck_inputFrame.button1= 0;
		ck_inputFrame.x = -ck_inputFrame.x;
		ck_inputFrame.xaxis = -ck_inputFrame.xaxis;
		if (ck_inputFrame.xaxis < 0)
		{
			ck_inputFrame.dir = dir_West;
		}
		else if (ck_inputFrame.xaxis > 0)
		{
			ck_inputFrame.dir = dir_East;
		}
		else
		{
			ck_inputFrame.dir = dir_None;
		}
	}
	else if (ck_mapState.brainfucl > 0)
	{
		memset(&ck_inputFrame, 0, sizeof(ck_inputFrame));
		if (ck_keenObj->currentAction->stickToGround == push_down)
		{
			ck_inputFrame.x = ck_inputFrame.xaxis = -1;
			ck_inputFrame.dir = dir_East;
		}
		else
		{
			ck_inputFrame.dir = dir_None;
		}
	}
	// end of addition

	if (ck_inputFrame.yaxis != -1)
		ck_keenState.keenSliding = false;

	if (!ck_gamePadEnabled || (DemoMode != demo_Off))
	{
		// Two button firing mode is used for demo playback
		if (!oldshooting || (DemoMode != demo_Off))	// K1n9_Duk3 mod: demo NEVER uses two button firing
		{
			ck_keenState.jumpIsPressed = ck_inputFrame.button0;
			ck_keenState.pogoIsPressed = ck_inputFrame.button1;
			ck_keenState.shootIsPressed = ck_inputFrame.button2; // K1n9_Duk3 mod
		//	ck_keenState.shootIsPressed = Keyboard[firescan];
			if (!ck_keenState.jumpIsPressed) ck_keenState.jumpWasPressed = false;
			if (!ck_keenState.pogoIsPressed) ck_keenState.pogoWasPressed = false;
			if (!ck_keenState.shootIsPressed) ck_keenState.shootWasPressed = false;
		}
		else
		{

			// Check for two-button firing
			if (ck_inputFrame.button0 && ck_inputFrame.button1)
			{
				ck_keenState.shootIsPressed = true;
				ck_keenState.jumpIsPressed = false;
				ck_keenState.pogoIsPressed = false;
				ck_keenState.jumpWasPressed = false;
				ck_keenState.pogoWasPressed = false;
				goto restrictions;
			}

			ck_keenState.shootWasPressed = false;
			ck_keenState.shootIsPressed = false;

			if (ck_inputFrame.button0)
			{
				ck_keenState.jumpIsPressed = true;
			}
			else
			{
				ck_keenState.jumpWasPressed = false;
				ck_keenState.jumpIsPressed = false;
			}

			if (ck_inputFrame.button1)
			{
				// Here be dragons!
				// In order to better emulate the original trilogy's controls, a delay
				// is introduced when pogoing in two-button firing.
				if (ck_pogoTimer <= 8)
				{
					ck_pogoTimer += tics;
				}
				else
				{
					ck_keenState.pogoIsPressed = true;
				}
			}
			else
			{
				// If the player lets go of pogo, pogo immediately.
				if (ck_pogoTimer)
				{
					ck_keenState.pogoIsPressed = true;
				}
				else
				{
					ck_keenState.pogoWasPressed = false;
					ck_keenState.pogoIsPressed = false;
				}
				ck_pogoTimer = 0;
			}
		}
	}
	else
	{
		// TODO: Gravis Gamepad input handling
	}

	// K1n9_Duk3 addition
	restrictions:
	if (ck_mapState.nisasi)
	{
		ck_keenState.shootIsPressed = ck_keenState.shootWasPressed = false;
	}
	if (ck_mapState.quicksand)
	{
		ck_keenState.jumpIsPressed = ck_keenState.jumpWasPressed
			= ck_keenState.pogoIsPressed = ck_keenState.pogoWasPressed = false;
	}

	// Nisaba addition
	if (ck_gameState.currentLevel == MAP_MUSHROOMLEVEL1 || ck_gameState.currentLevel == MAP_TARDISLEVEL)
	{
		if (ck_keenState.pogoIsPressed || ck_keenState.jumpIsPressed)
		{
			ck_keenState.jumpIsPressed = true;
			ck_keenState.pogoIsPressed = false;
		}
		else
		{
			ck_keenState.jumpWasPressed = false;
			ck_keenState.jumpIsPressed = false;
		}
	}
	// end of addition
}

// K1n9_Duk3 addition: CK_RemoveSprite

void CK_RemoveSprite(CK_object *obj)
{
	RF_RemoveSprite(&obj->sde);

	switch (obj->type)
	{
	case CT_Platform:	// remove the thruster sprites
		RF_RemoveSprite((void *)&obj->user2);
		// no break here!

	case CT_FallBlock:        // remove foreground sprite
	case CT_StunnedCreature:  // remove the stunned stars sprites
	case CT_Slug:             // remove the donut sprite (if it exists)
		RF_RemoveSprite((void *)&obj->user3);
		break;
	}
}

//===========================================================================
//
// OBJECT LIST
//
//===========================================================================

void CK_SetupObjArray()
{
		int i;
		for (i = 0; i < CK_MAX_OBJECTS; ++i)
		{
				ck_objArray[i].prev = &(ck_objArray[i + 1]);
				ck_objArray[i].next = 0;
		}

		ck_objArray[CK_MAX_OBJECTS - 1].prev = 0;

		ck_freeObject = &ck_objArray[0];
		ck_lastObject = 0;
		ck_numObjects = 0;

		ck_keenObj = CK_GetNewObj(false);

		ck_scoreBoxObj = CK_GetNewObj(false);
}

CK_object *CK_GetNewObj(boolean nonCritical)
{
		CK_object *newObj;

		if (!ck_freeObject)
		{
				if (nonCritical)
				{
						//printf("Warning: No free spots in objarray! Using temp object\n");
						return &tempObj;
				}
				else
						//Quit("GetNewObj: No free spots in objarray!");
						SM_Quit(S_OBJARRAYFULL);
		}

		newObj = ck_freeObject;
		ck_freeObject = ck_freeObject->prev;

		//Clear any old crap out of the struct.
		memset(newObj, 0, sizeof (CK_object));


		if (ck_lastObject)
		{
				ck_lastObject->next = newObj;
		}
		newObj->prev = ck_lastObject;


		newObj->active = OBJ_ACTIVE;
		newObj->clipped = CLIP_normal;

		ck_lastObject = newObj;
		ck_numObjects++;


		return newObj;
}

void CK_RemoveObj(CK_object *obj)
{
		if (obj == ck_keenObj)
		{
			//	Quit("RemoveObj: Tried to remove the player!");
				SM_Quit(S_REMOVEPLAYER);
		}
		CK_ObjDropNisasi(obj);	// K1n9_Duk3 mod: just in case...

		// TODO: Make a better spritedraw handler that
		// replaces the user int variables
		CK_RemoveSprite(obj);

		if (obj == ck_lastObject)
				ck_lastObject = obj->prev;
		else
				obj->next->prev = obj->prev;

		obj->prev->next = obj->next;
	//	obj->next = 0;

		obj->prev = ck_freeObject;
		ck_freeObject = obj;
		ck_numObjects--;
}


//===========================================================================
//
// ACTION STATES
//
//===========================================================================

int CK_ActionThink(CK_object *obj, int time)
{
	int newTime, remainingTime;
	CK_action *action = obj->currentAction;


	// ThinkMethod: 2
	if (action->type == AT_Frame)
	{
		if (action->think)
		{
			if (obj->timeUntillThink)
			{
				obj->timeUntillThink--;
			}
			else
			{
				action->think(obj);
			}
		}
		return 0;
	}

	newTime = time + obj->actionTimer;

	// If we won't run out of time.
	if (action->timer > newTime || !(action->timer))
	{
		obj->actionTimer = newTime;

		if (action->type == AT_ScaledOnce || action->type == AT_ScaledFrame)
		{
			if (obj->xDirection)
			{
				//TODO: Work out what to do with the nextVelocity stuff.
				ck_nextX += action->velX * (time * obj->xDirection);
			}
			if (obj->yDirection)
			{
				ck_nextY += action->velY * (time * obj->yDirection);
			}
		}

		if (action->type == AT_UnscaledFrame || action->type == AT_ScaledFrame)
		{
			if (action->think)
			{
				if (obj->timeUntillThink)
					obj->timeUntillThink--;
				else
				{
					action->think(obj);
				}
			}
		}
		return 0;
	}

	remainingTime = action->timer - obj->actionTimer;
	newTime -= action->timer;
	obj->actionTimer = 0;

	if (action->type == AT_ScaledOnce || action->type == AT_ScaledFrame)
	{
		if (obj->xDirection)
		{
			ck_nextX += action->velX * (remainingTime * obj->xDirection);
		}
		if (obj->yDirection)
		{
			ck_nextY += action->velY * (remainingTime * obj->yDirection);
		}
	}
	else /*if (action->type == AT_UnscaledOnce || action->type == AT_UnscaledFrame)*/
	{
		if (obj->xDirection)
		{
			ck_nextX += action->velX * obj->xDirection;
		}
		if (obj->yDirection)
		{
			ck_nextY += action->velY * obj->yDirection;
		}
	}

	if (action->think)
	{
		if (obj->timeUntillThink)
			obj->timeUntillThink--;
		else
		{
			action->think(obj);
		}
	}

	if (action != obj->currentAction)
	{
		if (!obj->currentAction)
		{
			return 0;
		}
	}
	else
	{
		obj->currentAction = action->next;
	}
	return newTime;
}

void CK_RunAction(CK_object *obj)
{
	int ticsLeft;
	CK_action *prevAction;
	int oldChunk = obj->gfxChunk;

	//TODO: Check these
	//int oldPosX = obj->posX;
	//int oldPosY = obj->posY;

	obj->deltaPosX = obj->deltaPosY = 0;

	ck_nextX = ck_nextY = 0;
	// K1n9_Duk3 addition: always clear adjustment indicator
	ck_keenState.mustAdjustNextX = false;	// will be set by Keen's think routine

	prevAction = obj->currentAction;

	ticsLeft = CK_ActionThink(obj, tics);

	if (obj->currentAction != prevAction)
	{
		obj->actionTimer = 0;
		prevAction = obj->currentAction;
	}
	// TODO/FIXME(?): Vanilla code doesn't check if prevAction is non-NULL,
	// but the new code may crash as a consequence (try no-clip cheat).
	while (prevAction && ticsLeft)
	{
		if (prevAction->protectAnimation || prevAction->timer > ticsLeft)
		{
			ticsLeft = CK_ActionThink(obj, ticsLeft);
		}
		else
		{
			ticsLeft = CK_ActionThink(obj, prevAction->timer - 1);
		}

		if (obj->currentAction != prevAction)
		{
			obj->actionTimer = 0;
			prevAction = obj->currentAction;
		}

	}

	if (!prevAction)
	{
		CK_RemoveObj(obj);
		return;
	}
	if (prevAction->chunkRight)
	{
		obj->gfxChunk = (obj->xDirection > 0) ? prevAction->chunkRight : prevAction->chunkLeft;
	}
	if (obj->gfxChunk == -1)
	{
		obj->gfxChunk = 0;
	}
	
#if 0
	if (obj->currentAction->chunkLeft && obj->xDirection <= 0) obj->gfxChunk = obj->currentAction->chunkLeft;
	if (obj->currentAction->chunkRight && obj->xDirection > 0) obj->gfxChunk = obj->currentAction->chunkRight;
#endif
	if (obj->gfxChunk != oldChunk || ck_nextX || ck_nextY || obj->topTI == 0x19 || obj->currentAction->stickToGround)	// K1n9_Duk3 addition: added stickToGround check
	{
		// K1n9_Duk3 mod:
		// We need to adjust Keen's walking speed in here instead of doing it in
		// CK_KeenRunningThink() because CK_KeenRunningThink() might be executed
		// twice during the same frame, and in that case the adjustment would be
		// applied twice instead of just once.
		if (ck_keenState.mustAdjustNextX)
		{
			extern int ck_KeenRunXVels[];
			
			// apply slope speed adjustment:
			ck_nextX += ck_KeenRunXVels[obj->topTI & SLOPEMASK] * tics;
			
			// HACK: cut speed in half if Keen is being slowed down:
			if (ck_mapState.nisasi || ck_mapState.inwater)
				ck_nextX >>= 1;
		}
		// end of mod
		
		
		if (obj->clipped == CLIP_full)
			CK_PhysFullClipToWalls(obj);
		else if (obj->clipped == CLIP_simple)	// K1n9_Duk3 fix: added 'else'
			CK_PhysUpdateSimpleObj(obj);
		else
			CK_PhysUpdateNormalObj(obj);
	}
}

//===========================================================================
//
// LEVEL MUSIC
//
//===========================================================================
/*
void StartMusic(unsigned level, boolean repeat)
{
	int songtoplay;

	if (level >= LASTMUSIC && level != (unsigned)(-1))
	{
		//Quit("StartMusic() - bad level number");
		SM_Quit(S_BADMUSIC);
	}
	// K1n9_Duk3 fix: Use StopMusic instead of SD_MusicOff to make sure old music is purgable
	StopMusic();

	songtoplay = level;


	if (songtoplay == -1)
		return;
	if (MusicMode != smm_AdLib)
		return;
	MM_BombOnError(false);
	CA_CacheAudioChunk(STARTMUSIC + songtoplay);
	MM_BombOnError(true);

	if (mmerror)
	{
		int faded;
		mmerror = false;
		if (DemoMode)
			return;
		US_CenterWindow(20, 8);
		PrintY += 20;
		//US_CPrint("Insufficient memory\nfor background music!");
		SM_CPrint(S_MUSIC_NOMEM);
		VW_UpdateScreen();
		RF_ClearBlock(0, 0, PORTTILESWIDE*16, PORTTILESHIGH*16);	// K1n9_Duk3 fix: force a full refresh
		faded = screenfaded;
		if (faded)
			VW_SetDefaultColors();
		IN_ClearKeysDown();
		IN_UserInput(210, false);
		if (faded)
			VW_FadeOut();
		return;	// K1n9_Duk3 fix
	}
	// K1n9_Duk3 fix: prevent memory manager from messing with the music
	MM_SetLock((memptr *) &audiosegs[STARTMUSIC + songtoplay], true);

	SD_SetMusicRate(DEFAULT_MUSIC_RATE);	// every song starts at the default rate (for convenience)
	RepeatMusic = repeat;
	SD_StartMusic((MusicGroup far *) audiosegs[STARTMUSIC + songtoplay]);
}

void StopMusic(void)
{
	int i;
	SD_MusicOff();
	for (i = 0; i < LASTMUSIC; i++)

		if (audiosegs[STARTMUSIC + i])
		{
			MM_SetPurge((memptr *) &audiosegs[STARTMUSIC + i], 3);
			// K1n9_Duk3 fix:
			MM_SetLock((memptr *) &audiosegs[STARTMUSIC + i], false);
		}
}
*/

/*
=====================
=
= CacheMusic	(K1n9_Duk3 addition)
=
= Tries to cache the audio chunk for the given music number.
= Returns true when music was loaded successfully.
=
=====================
*/
static boolean near CacheMusic(unsigned num)
{
	if (num < LASTMUSIC)
	{
		ck_gameState.musicplayed[num] = true;
		
		if (MusicMode == smm_AdLib)
		{
			MM_BombOnError(false);
			CA_CacheAudioChunk(STARTMUSIC + num);
			MM_BombOnError(true);
			if (!mmerror)
				return true;
			mmerror = false;
		
			if (!DemoMode)
			{
				int faded;
				
				US_CenterWindow(20, 8);
				PrintY += 20;
				//US_CPrint("Insufficient memory\nfor background music!");
				SM_CPrint(S_MUSIC_NOMEM);
				VW_UpdateScreen();
				RF_ClearBlock(0, 0, PORTTILESWIDE*16, PORTTILESHIGH*16);	// K1n9_Duk3 fix: force a full refresh
				faded = screenfaded;
				if (faded)
					//VW_SetDefaultColors();
					CK_DefaultPalette();
				IN_ClearKeysDown();
				IN_UserInput(210, false);
				if (faded)
					//VW_FadeOut();
					CK_FadeOut();
			}
		}
	}
	return false;
}

/*
=====================
=
= ContinueMusic	(K1n9_Duk3 addition)
=
= Tries to resume playback of the given music number if it was the last music
= played by the sound manager. Otherwise starts the music from the beginning.
= Music will always be played at the default rate and repeat at the end.
=
=====================
*/
static void near pascal ContinueMusic(unsigned num)
{
	// can't continue the music if it isn't cached:
	if (CacheMusic(num))
	{
		// check if desired music number is what was played before
		if (SD_CurrentMusic() == num)
		{
			// resume playback:
			SD_MusicOn();
			// and just in case:
			RepeatMusic = true;
			SD_SetMusicRate(DEFAULT_MUSIC_RATE);
		}
		else
		{
			// just restart the music from the beginning:
			StartMusic(num, true);
		}
	}
}

/*
=====================
=
= StartMusic
=
= Tries to play the given music number from the beginning.
=
=====================
*/
void StartMusic(unsigned level, boolean repeat)
{
	int songtoplay;

	if (level >= LASTMUSIC && level != (unsigned)(-1))
	{
		//Quit("StartMusic() - bad level number");
		SM_Quit(S_BADMUSIC);
	}
	// K1n9_Duk3 fix: Use StopMusic instead of SD_MusicOff to make sure old music is purgable
	StopMusic();

	songtoplay = level;


	if (songtoplay == -1)
		return;
	if (MusicMode != smm_AdLib)
		return;
	
	if (CacheMusic(songtoplay))
	{
		// Note: don't need to lock & unlock music chunks anymore -- K1n9_Duk3

		SD_SetMusicRate(DEFAULT_MUSIC_RATE);	// every song starts at the default rate (for convenience)
		RepeatMusic = repeat;
		//SD_StartMusic((MusicGroup far *) audiosegs[STARTMUSIC + songtoplay]);
		SD_StartMusic(songtoplay);
	}
}

/*
=====================
=
= StopMusic
=
= Stops music playback and makes all music buffers purgable.
=
=====================
*/
void StopMusic(void)
{
	int i;
	
	SD_MusicOff();
	for (i = 0; i < LASTMUSIC; i++)

		if (audiosegs[STARTMUSIC + i])
		{
			MM_SetPurge((memptr *) &audiosegs[STARTMUSIC + i], 3);
			// Note: don't need to lock & unlock music chunks anymore -- K1n9_Duk3
		}
}


//===========================================================================
//
// CAMERA
//
//===========================================================================
void CK_SetActivityLimits(void)
{
	originxtilemax = originxtile + PORTTILESWIDE - 1;
	originytilemax = originytile + PORTTILESHIGH - 1;

	ck_activeX0Tile = originxtile - 6;
	if (ck_activeX0Tile < 0)
		ck_activeX0Tile = 0;

	ck_activeX1Tile = originxtilemax + 6;
	if (ck_activeX1Tile < 0)
		ck_activeX1Tile = 0;

	ck_activeY0Tile = originytile - 6;
	if (ck_activeY0Tile < 0)
		ck_activeY0Tile = 0;

	ck_activeY1Tile = originytilemax + 6;
	if (ck_activeY1Tile < 0)
		ck_activeY1Tile = 0;
}

void CK_CentreCamera(CK_object *obj)
{
	unsigned screenX, screenY;

	ck_mapState.screenYpx = 140;

	if (obj->posX < (152 << 4))
		screenX = 0;
	else
		screenX = obj->posX - (152 << 4);

	if (ck_gameState.currentLevel == 0)
	{
		// World Map
		if (obj->posY < (80 << 4))
			screenY = 0;
		else
			screenY = obj->posY - (80 << 4);
	}
	else
	{
		// In Level
		if (obj->clipRects.unitY2 < (140 << 4))
			screenY = 0;

		else
			screenY = obj->clipRects.unitY2 - (140 << 4);
	}

	// TODO: Find out why this is locking the game up
	if (!ck_inHighScores)
		RF_NewPosition(screenX, screenY);

	CK_SetActivityLimits();
}

/*
 * Move the camera that follows keen on the world map
 */

void CK_MapCamera( CK_object *keen )
{
	int scr_y, scr_x;

	if (ck_scrollDisabled)
		return;

	// Scroll Left, Right, or nowhere
	if ( keen->clipRects.unitX1 < originxglobal + (144 << 4) )
		scr_x = keen->clipRects.unitX1 - (originxglobal + (144 << 4));
	else if ( keen->clipRects.unitX2 > originxglobal + (192 << 4) )
		scr_x = keen->clipRects.unitX2 /*+ 16*/ - (originxglobal + (192 << 4));
	else
		scr_x = 0;

	// Scroll Up, Down, or nowhere
	if ( keen->clipRects.unitY1 < originyglobal + (80 << 4) )
		scr_y = keen->clipRects.unitY1 - (originyglobal + (80 << 4));
	else if ( keen->clipRects.unitY2 > originyglobal + (112 << 4) )
		scr_y = keen->clipRects.unitY2 - (originyglobal + (112 << 4));
	else
		scr_y = 0;

	// Limit scrolling to 256 map units (1 tile)
	// And update the active boundaries of the map
	if ( scr_x != 0 || scr_y != 0 )
	{
		if ( scr_x >= 256 )
			scr_x = 255;
		else if ( scr_x <= -256 )
			scr_x = -255;

		if ( scr_y >= 256 )
			scr_y = 255;
		else if ( scr_y <= -256 )
			scr_y = -255;

		RF_Scroll( scr_x, scr_y );

		CK_SetActivityLimits();

	}
}

// Run the normal camera which follows keen

void CK_NormalCamera(CK_object *obj)
{

	int pxToMove, cmpAmt;
	int deltaX = 0, deltaY = 0;	// in Units

	//TODO: some unknown var must be 0
	//This var is a "ScrollDisabled flag." If keen dies, it's set so he 
	// can fall out the bottom
	if (ck_scrollDisabled)
		goto shake;

	// End level if keen makes it out either side
	if (((obj->clipRects.unitX1 < originxmin || obj->clipRects.unitX2 > originxmax + 320*PIXGLOBAL)
		&& obj->clipped)	// K1n9_Duk3 mod: don't win a level while in noclip mode!
		|| obj->clipRects.unitY1 < TILEGLOBAL && !CK_KeenIsDead())	// K1n9_Duk3 mod: levels can be exited at the top
	{
		ck_levelState = LS_LevelComplete;	// K1n9_Duk3 fix: use enum name instead of number
		return;
	}

	// Kill keen if he falls out the bottom
	if (obj->clipRects.unitY2 > (originymax + (208 << 4)))
	{
		obj->posY -= obj->clipRects.unitY2 - (originymax + (208 << 4));
		SD_PlaySound(KEENFALLSND);
		ck_gameState.godMode = false;
		CK_KillKeen();
		goto shake;
	}


	// Keep keen's x-coord between 144-192 pixels
	if (obj->posX < (originxglobal + (144 << 4)))
		deltaX = obj->posX - (originxglobal + (144 << 4));

	if (obj->posX > (originxglobal + (192 << 4)))
		deltaX = obj->posX - (originxglobal + (192 << 4));


	// Keen should be able to look up and down.
	if (obj->currentAction == &CK_ACT_keenLookUp2)
	{
		if (ck_mapState.screenYpx + tics > 167)
		{
			// Keen should never be so low on the screen that his
			// feet are more than 167 px from the top.
			pxToMove = 167 - ck_mapState.screenYpx;
		}
		else
		{
			// Move 1px per tick.
			pxToMove = tics;
		}
		ck_mapState.screenYpx += pxToMove;
		deltaY = (-pxToMove) << 4;

	}
	else if (obj->currentAction == &CK_ACT_keenLookDown3)
	{
		int pxToMove;
		if (ck_mapState.screenYpx - tics < 20)
		{
			// Keen should never be so high on the screen that his
			// feet are fewer than 33 px from the top.
			pxToMove = ck_mapState.screenYpx - 20;
		}
		else
		{
			// Move 1px/tick.
			pxToMove = tics;
		}
		ck_mapState.screenYpx -= pxToMove;
		deltaY = pxToMove << 4;
	}
	else if (ck_mapState.screenYpx < 48)
	{
		ck_mapState.screenYpx += tics;
	}

	// If we're attached to the ground, or otherwise awesome
	// do somethink inscrutible.
	if (obj->topTI || !obj->clipped || obj->currentAction == &CK_ACT_keenHang1 || obj != ck_keenObj || obj->currentAction == &CK_ACT_keenMindcontrol)
	{
		if (obj->currentAction != &CK_ACT_keenPull1 &&
				obj->currentAction != &CK_ACT_keenPull2 &&
				obj->currentAction != &CK_ACT_keenPull3 &&
				obj->currentAction != &CK_ACT_keenPull4)
		{
			deltaY += obj->deltaPosY;

			//TODO: Something hideous
			// TODO: Convert to 16-bit once the rest is converted
			// (due to unsigned vs signed mess)
			cmpAmt = (ck_mapState.screenYpx << 4) + originyglobal + deltaY;
			if (cmpAmt != obj->clipRects.unitY2)
			{
				int oneDiff, otherDiff;
				if (obj->clipRects.unitY2 < cmpAmt)
					oneDiff = cmpAmt - obj->clipRects.unitY2;
				else
					oneDiff = obj->clipRects.unitY2 - cmpAmt;

				// TODO: Unsigned shift left,
				// followed by a signed shift right...
				otherDiff = (signed)((unsigned)oneDiff << 4) >> 7;
				if (otherDiff > 48)
					otherDiff = 48;
				otherDiff *= tics;
				if (otherDiff < 16)
				{
					if (oneDiff < 16)
						otherDiff = oneDiff;
					else
						otherDiff = 16;
				}
				if (obj->clipRects.unitY2 < cmpAmt)
					deltaY -= otherDiff;
				else
					deltaY += otherDiff;
#if 0
				int adjAmt = (((ck_mapState.screenYpx << 4) + originyglobal + deltaY - obj->clipRects.unitY2));
				int adjAmt2 = abs(adjAmt / 8);
				adjAmt2 = (adjAmt2 <= 48) ? adjAmt2 : 48;
				if (adjAmt > 0)
					deltaY -= adjAmt2;
				else
					deltaY += adjAmt2;
#endif
			}
		}

	}
	else
	{
		// Reset to 140px.
		ck_mapState.screenYpx = 140;
	}


	// Scroll the screen to keep keen between 33 and 167 px.
	cmpAmt = originyglobal + deltaY + ((obj->deltaPosY>=0)? 20*PIXGLOBAL : 48*PIXGLOBAL);
	//cmpAmt = originyglobal + deltaY + 48*PIXGLOBAL;
	if (obj->clipRects.unitY2 < cmpAmt)
		deltaY += obj->clipRects.unitY2 - cmpAmt;


	if (obj->clipRects.unitY2 > (originyglobal + deltaY + (168 << 4)))
		deltaY += obj->clipRects.unitY2 - (originyglobal + deltaY + (168 << 4));


shake:
	if (ck_mapState.shaketics)
	{
		deltaX -= ck_mapState.shakeX;
		deltaY -= ck_mapState.shakeY;

		ck_mapState.shakeY = -ck_mapState.shakeX;
		if (++ck_mapState.shakecount & 1)
		{
			ck_mapState.shakeX = ck_mapState.shaketics;
		}
		else
		{
			ck_mapState.shakeX = -ck_mapState.shaketics;
		}

		deltaX += ck_mapState.shakeX;
		deltaY += ck_mapState.shakeY;

		if ((ck_mapState.shaketics -= tics) < 0)
			ck_mapState.shaketics = 0;
	}

	//Don't scroll more than one tile's worth per frame.
	if (deltaX || deltaY)
	{
		if (deltaX > 255) deltaX = 255;
		else if (deltaX < -255) deltaX = -255;

		if (deltaY > 255) deltaY = 255;
		else

			if (deltaY < -255) deltaY = -255;

		// Do the scroll!
		if (ck_keenObj->currentAction == &CK_ACT_keenMindcontrol)
		{
			if (abs(deltaX) > PIXGLOBAL)
				deltaX /= 2;
			if (abs(deltaY) > PIXGLOBAL)
				deltaY /= 2;
		}
		RF_Scroll(deltaX, deltaY);

		CK_SetActivityLimits();

	}
}

// K1n9_Duk3 addition:
void CK_WrapScroll(void)
{
	if (ck_mapState.wrapScroll)
	{
		int dx, dy;

		if (originxglobal < originxmin+TILEGLOBAL)
		{
			dx = originxmax - (originxmin+2*TILEGLOBAL);
		}
		else if (originxglobal > originxmax-TILEGLOBAL)
		{
			dx = originxmin - (originxmax-2*TILEGLOBAL);
		}
		else
		{
			dx = 0;
		}

		if (originyglobal < originymin+TILEGLOBAL)
		{
			dy = originymax - (originymin+2*TILEGLOBAL);
		}
		else if (originyglobal > originymax-TILEGLOBAL)
		{
			dy = originymin - (originymax-2*TILEGLOBAL);
		}
		else
		{
			dy = 0;
		}

		if (dx || dy)
		{
			// move screen:
			RF_NewPosition(originxglobal+dx, originyglobal+dy);

			// move Keen:
			ck_keenObj->posX += dx;
			ck_keenObj->posY += dy;
			CK_ResetClipRects(ck_keenObj);	// update hitbox
			//ck_keenObj->visible = true;	// force redraw
			CK_BasicDrawFunc1(ck_keenObj);	// redraw
			
			// move shots:
			{
				CK_object *currentObj;
				
				for (currentObj=ck_keenObj; currentObj; currentObj=currentObj->next)
				{
					if (currentObj->type == CT_Stunner)
					{
						currentObj->posX += dx;
						currentObj->posY += dy;
						//currentObj->visible = true;	// force redraw
						CK_ResetClipRects(currentObj);	// update hitbox
						CK_BasicDrawFunc1(currentObj);	// redraw
					}
				}
			}
		}
	}
}
// end of addition


//===========================================================================


/*
   ============================
   =
   = PlayLoop
   =
   ============================
*/

void CK_PlayLoop (void)
{

	CK_object *currentObj, *collideObj;
	/*
	StartMusic(ck_mapState.music, true);
	*/
	ContinueMusic(ck_mapState.music);

	ck_pogoTimer = 0;
	memset(&ck_keenState, 0, sizeof(ck_keenState));
	ck_scrollDisabled = false;
	ck_keenState.jumpWasPressed = ck_keenState.pogoWasPressed = ck_keenState.shootWasPressed = false;
	ingame = 1;

	if (ck_levelState == LS_StartingSaved)
	{
		RF_NewPosition (originxglobal,originyglobal);
		CK_SetActivityLimits();
	}
	else
	{
		CK_CentreCamera(ck_keenObj);
	}

	// If this is nonzero, the level will quit immediately.
	ck_levelState = LS_Playing;

	//ck_keenState.pogoTimer = ck_scrollDisabled = ck_keenState.invincibilityTimer = 0;

	// Note that this appears in CK_LoadLevel as well
	if (DemoMode)
		US_InitRndT(false);
	else
		US_InitRndT(true);

	tics = 3;
	lasttimecount = 3;
	TimeCount = 3;
 
	while (ck_levelState == LS_Playing)
	{
		if (ck_mapState.nisasi)
			SD_SetMusicRate(350);

		CK_HandleInput();

		// Set, unset active objects.
		for (currentObj = ck_keenObj; currentObj; currentObj = currentObj->next)
		{

			if (!currentObj->active &&
					(currentObj->clipRects.tileX2 >= originxtile - 1) &&
					(currentObj->clipRects.tileX1 <= originxtilemax + 1) &&
					(currentObj->clipRects.tileY1 <= originytilemax + 1) &&
					(currentObj->clipRects.tileY2 >= originytile - 1))
			{
				currentObj->visible = true;
				currentObj->active = OBJ_ACTIVE;
			}
			if (currentObj->active)
			{
				if ((currentObj->clipRects.tileX2 < ck_activeX0Tile) ||
				    (currentObj->clipRects.tileX1 > ck_activeX1Tile) ||
				    (currentObj->clipRects.tileY1 > ck_activeY1Tile) ||
				    (currentObj->clipRects.tileY2 < ck_activeY0Tile))
				{
					if (currentObj->active == OBJ_EXISTS_ONLY_ONSCREEN)
					{
						CK_RemoveObj(currentObj);
						continue;
					}
					else if (currentObj->active != OBJ_ALWAYS_ACTIVE)
					{
						if (US_RndT() < tics * 2 || screenfaded)
						{
							CK_RemoveSprite(currentObj);
							currentObj->active = OBJ_INACTIVE;
							continue;
						}
					}
				}

				// Object is active, so process its action amd update its physics
				CK_RunAction(currentObj);
				
				// K1n9_Duk3 HACK:
				// When the frame rate is extremely low, Keen's stunner shots will
				// move by very large amounts each frame, which can lead to shots
				// that fly right through some enemies without hitting them.
				// To avoid this problem, I am going to extend the shot's hitbox
				// temporarily to make the hitbox cover the entire area the shot
				// has just traveled through.
				if (currentObj->type == CT_Stunner)
				{
					if (currentObj->deltaPosX > 0)
						currentObj->clipRects.unitX1 -= currentObj->deltaPosX;
					else
						currentObj->clipRects.unitX2 -= currentObj->deltaPosX;
					
					if (currentObj->deltaPosY > 0)
						currentObj->clipRects.unitY1 -= currentObj->deltaPosY;
					else
						currentObj->clipRects.unitY2 -= currentObj->deltaPosY;
				}
				// end of HACK

			}
		}

		/*
		if (ck_keenState.platform)
			CK_KeenRidePlatform(ck_keenObj);
		*/	

		// K1n9_Duk3 fix: use quicksand object as platform when platform is NULL:
		if (!ck_keenState.platform)
			ck_keenState.platform = ck_mapState.quicksand;
		
		if (ck_keenState.platform)
			CK_KeenRidePlatform(ck_keenObj);
		
#if 0
		// original code (collisions with Keen are checked FIRST):
		for (currentObj = ck_keenObj; currentObj; currentObj = currentObj->next)
		{
			// Some strange Keen4 stuff here. Ignoring for now.

			if (!currentObj->active) continue;
			
			for (collideObj = currentObj->next; collideObj; collideObj = collideObj->next)
			{
				if (!collideObj->active)
					continue;

				if (	(currentObj->clipRects.unitX2 > collideObj->clipRects.unitX1) &&
						(currentObj->clipRects.unitX1 < collideObj->clipRects.unitX2) &&
						(currentObj->clipRects.unitY1 < collideObj->clipRects.unitY2) &&
						(currentObj->clipRects.unitY2 > collideObj->clipRects.unitY1) )
				{
					if (currentObj->currentAction->collide)
						currentObj->currentAction->collide(currentObj, collideObj);
					if (collideObj->currentAction->collide)
						collideObj->currentAction->collide(collideObj, currentObj);
				}
			}
		}
#else
		// K1n9_Duk3 hack (collisions with Keen are checked LAST):
		
		currentObj = ck_scoreBoxObj;	// start AFTER Keen
		for (;;)
		{
			// Note: ck_scoreBoxObj can never be NULL, so we don't need to check
			// if currentObj is NULL when entering the loop.
			
			if (currentObj->active)
			{
				for (collideObj = currentObj->next; collideObj; collideObj = collideObj->next)
				{
					if (!collideObj->active)
						continue;

					if (	(currentObj->clipRects.unitX2 > collideObj->clipRects.unitX1) &&
							(currentObj->clipRects.unitX1 < collideObj->clipRects.unitX2) &&
							(currentObj->clipRects.unitY1 < collideObj->clipRects.unitY2) &&
							(currentObj->clipRects.unitY2 > collideObj->clipRects.unitY1) )
					{
						if (currentObj->currentAction->collide)
							currentObj->currentAction->collide(currentObj, collideObj);
						if (collideObj->currentAction->collide)
							collideObj->currentAction->collide(collideObj, currentObj);
					}
				}
			}
			
			// end the loop when Keen has been processed:
			if (currentObj == ck_keenObj)
				break;
			
			// go to next object:
			currentObj = currentObj->next;
			
			// process Keen's collisions when end of object list has been reached:
			if (!currentObj)
				currentObj = ck_keenObj;
		}
#endif		

		// Check special tiles:

		if (ck_gameState.currentLevel == 0)
			CK_MapMiscFlagsCheck(ck_keenObj);
		else
			CK_KeenCheckSpecialTileInfo(ck_keenObj);


		for (currentObj = ck_keenObj; currentObj; currentObj = currentObj->next)
		{
			if (currentObj->active)
			{
				// K1n9_Duk3 addition: reset hitbox for stunner shots (undo HACK)
				if (currentObj->type == CT_Stunner)
					CK_ResetClipRects(currentObj);
				// end of addition

				if (currentObj->clipRects.tileY2 >= (mapheight - 1))
				{
					if (currentObj == ck_keenObj)	// K1n9_Duk3 mod: this used to check the type and not the pointer
					{
						// Kill Keen if he exits the bottom of the map.
						ck_levelState = LS_Died;
						continue;
					}
					else
					{
						CK_RemoveObj(currentObj);
						continue;
					}
				}
				if (currentObj->visible && currentObj->currentAction->draw)
				{
					currentObj->visible = false;	//We don't need to render it twice!
					currentObj->currentAction->draw(currentObj);
				}

				// K1n9_Duk3 addition: remove sprites of invisible objects:
				if (currentObj == ck_scoreBoxObj || currentObj == ck_keenObj)
					continue;	// never remove scorebox or Keen's sprite

				if ((currentObj->clipRects.tileX2 < ck_activeX0Tile) ||
				    (currentObj->clipRects.tileX1 > ck_activeX1Tile) ||
				    (currentObj->clipRects.tileY1 > ck_activeY1Tile) ||
				    (currentObj->clipRects.tileY2 < ck_activeY0Tile))
				{
					CK_RemoveSprite(currentObj);
					currentObj->visible = true;
				}
			}
		}

		// Follow the player with the camera.
		if (ck_gameState.currentLevel == 0)
			CK_MapCamera(ck_keenObj);
		else if (ck_mapState.mindcontrolled)
			CK_NormalCamera(ck_mapState.mindcontrolled);
		else
			CK_NormalCamera(ck_keenObj);

		CK_WrapScroll();	// K1n9_Duk3 addition

		//Draw the scorebox
		CK_UpdateScoreBox(ck_scoreBoxObj);

		// Clear the loaded game flag, in case this is the 
		// first playloop iteration after we've resumed a savegame
		if (loadedgame)
			loadedgame = false;

		// 0xef for the X-direction to match EGA keen's 2px horz scrolling.
		RF_Refresh();

		// handle warping here:
		if (ck_mapState.warpInfo)
		{
			if (!CK_KeenIsDead())
			{
				CK_KeenTeleport(ck_keenObj, ck_mapState.warpInfo);
				CK_SetAction(ck_keenObj, ck_keenObj->currentAction);
			}
			ck_mapState.warpInfo = 0;
		}


		if (ck_mapState.invincibilityTimer)
		{
			ck_mapState.invincibilityTimer -= tics;
			if (ck_mapState.invincibilityTimer < 0)
				ck_mapState.invincibilityTimer = 0;
		}

		if (ck_mapState.stunCooldown)
		{
			ck_mapState.stunCooldown -= tics;
			if (ck_mapState.stunCooldown <= 0)
			{
				ck_mapState.stunCooldown = ck_mapState.stunCounter = 0;
			}
		}

		//TODO: Slow-mo, extra VBLs.
		if (ck_gameState.slowMotionEnabled)
		{
			VW_WaitVBL(14);
			TimeCount = lasttimecount;	// K1n9_Duk3 fix: never modify lasttimecount (otherwise demos go out of sync)
		}

		// TODO: Extra VBLs come here
		// VW_UpdateScreen();

		if (ck_mapState.checkpointTouched)
		{
			CheckpointSave(ck_mapState.checkpointTouched);
		}

		// End the bonus level after all items have been collected:
		if (ck_gameState.currentLevel == MAP_BONUSLEVEL && ck_mapState.itemsCollected == ck_mapState.itemsTotal)
		{
			int done = false;
			//ck_levelState = LS_LevelComplete;

			// remove any Lindsey objects from the level:
			for (currentObj = ck_keenObj; currentObj; currentObj = currentObj->next)
			{
				if (currentObj->type == CT_Lindsey)
					CK_LindseyVanish(currentObj);
				else if (currentObj->type == CT_LindseySpecial)
					done = true;
			}

			if (!done)
			{
				// spawn special Lindsey right next to Keen:
				CK_SpawnLindsey(ck_keenObj->clipRects.tileX1+2, ck_keenObj->clipRects.tileY2, -1);
				currentObj = SpawnSmoke(RF_TileToUnit(ck_keenObj->clipRects.tileX1+2)-8*PIXGLOBAL, RF_TileToUnit(ck_keenObj->clipRects.tileY2-1));
				currentObj->type = CT_LindseyVanish;	// indicates faster smoke animation
				// special Lindsey shows some dialog and ends the level after a few tics
			}
		}

		// If we've finished playing back our demo, or the player presses a key,
		// exit the playloop.
		if (DemoMode == demo_Playback)
		{
			if (!screenfaded && (LastScan != sc_None))
			{
				ck_levelState = LS_LevelComplete;
				if (LastScan != sc_F1)
					LastScan = sc_Space;
			}
		}
		else if (DemoMode == demo_PlayDone)
		{
			ck_levelState = LS_LevelComplete;
		}
		else
		{
			CK_CheckKeys();
		}

		// End-Of-Game cheat
#if 0
		if (Keyboard[sc_E] && Keyboard[sc_N] && Keyboard[sc_D])
		{
			ck_levelState = LS_DestroyedQED;	// K1n9_Duk3 fix: use enum name instead of number
		}
#endif
	}
	// K1n9_Duk3 mod: Don't stop music on LS_BonusComplete
	// (music will be stopped after the dialog in CK_GameLoop).
	if (ck_levelState != LS_BonusComplete)
		StopMusic();


	ingame = false;
	// Patch from Catacomb 3-D for updated ID Engine (control panel)
	abortgame = false;
}
