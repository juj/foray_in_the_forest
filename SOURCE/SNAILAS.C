/* "Foray in the Forest" Source Code
 * Copyright (C) 2023 Spleen
 * Copyright (C) 2022-2023 Nisaba
 * Copyright (C) 2019-2023 K1n9_Duk3
 *
 * The code in this file is based on BASIC code originally written by
 * Commander Spleen, converted into C code and adapted for FITF by
 * K1n9_Duk3.
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

#include "CK_DEF.H"

/*
=============================================================================

			LOCAL CONSTANTS

=============================================================================
*/

#define SNAILS_EXTERN	// load pre-defined snail data from EXTERN chunk

#ifdef SCREENWIDTH
#undef SCREENWIDTH
#endif

#define NUMSNAILS 40
#define SCREENWIDTH 80
#define SCREENHEIGHT 25
#define SNAILWIDTH 3
#define SNAILHEIGHT 1
#define MAXX (SCREENWIDTH-SNAILWIDTH)
#define MAXY (SCREENHEIGHT-SNAILHEIGHT)
#define SCREENSIZE (SCREENWIDTH*SCREENHEIGHT)

#define BACKGROUND (GREEN << 4)   // (BLACK << 4)
#define TEXTCOLOR DARKGRAY           // LIGHTGRAY
#define DEFAULTCOLORS (TEXTCOLOR | BACKGROUND)
#define SNAILCOLORS (TEXTCOLOR | BACKGROUND)

#define INITIALGLYPH 0x00	// symbol the screen is filled with at the beginning
#define ERASEGLYPH 0x00	// symbol left behind by the snails when they move

/*
=============================================================================

			LOCAL TYPES

=============================================================================
*/

// If we're using pre-defined snail data, we also need to make sure this code
// uses the same word alignemnt setting as the code that generated and saved the
// pre-defined data. We'll turn word alignemnt off because that results in a
// smaller file.
#pragma option -a-	// word alignment OFF

typedef struct
{
	unsigned char c;
	unsigned char f;
} chartype;


typedef struct
{
	signed char x, y;
	signed char dir, sl;
	unsigned char col;
} snailtype;

// Note: All arrays use 0-based index values. The snail's x/y position is also
// 0-based. x is the position of the leftmost part (head or tail), x+1 is the
// position of the '@' symbol. dir is either 1 or -1, so x+1+dir is the head's
// position and x+1-dir is the tail's position.

typedef struct {
	chartype map[SCREENSIZE];
	unsigned char coll[SCREENSIZE];
	snailtype snails[NUMSNAILS];
} TSnailVars;

/*
=============================================================================

			LOCAL VARIABLES

=============================================================================
*/

#ifndef SNAILS_EXTERN
static memptr snailbuffer;	// dynamic buffer for the TSnailVars struct data
#endif

/*
=============================================================================

			LOCAL MACROS

=============================================================================
*/

// HACK: We are going to reassign register DS to the dynamically allocated
// buffer in snailbuffer, which allows us to use near pointers to make the code
// smaller and more efficient. The struct can be accessed at offset 0 in our
// temporary data segment. The following macro provides easy access to the data:
#define snailvars (*((TSnailVars near *)0))

#define SETCOLOR(pos, color) {snailvars.map[pos].f = color;}
#define SETGLYPH(pos, glyph) {snailvars.map[pos].c = glyph;}
#define SETBLOCK(pos, block) {snailvars.coll[pos] = block;}
#define GETBLOCK(pos) (snailvars.coll[pos])
#define FIRSTSNAIL (snailvars.snails)
#define MAPSTART (snailvars.map)

/*
=============================================================================
*/

#undef random
static unsigned pascal near random(unsigned range)
{
	// Note: US_RndT() doesn't access any variables in the default data segment,
	// so we don't need to change register DS before calling it.
	return US_RndT() % range;
}

#ifndef SNAILS_EXTERN
static void pascal near PutText(/*stringnames*/ char* s, word x, word y, byte colors)
{
	// this function will always return with DS set to snailbuffer!
	
	// we need to go back to the default data segment for SM_GetString()
	asm {
		mov	ax, ss;
		mov	ds, ax;
	}
	
	(char *)_SI = /*SM_GetString(s)*/ s;
	
	// calculate map offset for x, y
	asm {
		mov	ax, SCREENWIDTH;
		mul	y;
		add	ax, x;
		shl	ax, 1;
		mov	di, ax;
		mov	es, snailbuffer;
		mov	ah, colors;
	}
	
#if (offsetof(TSnailVars, map) != 0)
	// add the map field's offset (should always be 0, but just in case...):
	_DI += offsetof(TSnailVars, map);
#endif
	
	// "draw" the string to the map buffer:
	asm	jmp	nextbyte;
putloop:
	asm	stosw;
nextbyte:
	asm {
		lodsb;
		or 	al, al;
		jnz	putloop;
	}
done:

	// and go back to our temporary data segment:
	_DS = (unsigned)snailbuffer;
}
#endif

// Note: MoveSnail is called with register DS set to our temporary data segment,
// so we MUST NOT call any routines that try to access variables in the default
// data segment from within this routine!
static void pascal near MoveSnail(snailtype *snail, int xmove, int ymove)
{
	int newx, newy;
	int ok;
	int lx;
	unsigned newpos, oldpos;
	
	newx = snail->x + xmove;
	newy = snail->y + ymove;
	
	if (newx > MAXX)
		newx = 0;
	if (newx < 0)
		newx = MAXX;
	if (newy > MAXY)
		newy = 0;
	if (newy < 0)
		newy = MAXY;

	newpos = newy*SCREENWIDTH + newx;
	oldpos = snail->y*SCREENWIDTH + snail->x;
	
	// make old position non-blocking:
	for (lx = 0; lx < SNAILWIDTH; lx++)
	{
		SETBLOCK(oldpos+lx, 0);
	}
	
	// check if new position is blocked:
	for (lx = 0; lx < SNAILWIDTH; lx++)
	{
		if (GETBLOCK(newpos+lx) != 0)
		{
			snail->dir = -snail->dir;	// make the snail turn around when it's blocked
			goto done;
		}
	}
	
	// remove snail at old position:
#if (SNAILCOLORS != DEFAULTCOLORS)
	for (lx = 0; lx < SNAILWIDTH; lx++)
	{
		SETGLYPH(oldpos+lx, ERASEGLYPH);
		SETCOLOR(oldpos+lx, DEFAULTCOLORS);
	}
#else
	for (lx = 0; lx < SNAILWIDTH; lx++)
	{
		SETGLYPH(oldpos+lx, ERASEGLYPH);
	}
	SETCOLOR(oldpos+1, DEFAULTCOLORS);
#endif
	
	// place snail at new position:
	SETGLYPH(newpos+1, '@');	// 64
	SETCOLOR(newpos+1, snail->col);
	SETGLYPH(newpos+1+snail->dir, '"');	// 34
	SETGLYPH(newpos+1-snail->dir, '_');	// 95
#if (SNAILCOLORS != DEFAULTCOLORS)
	SETCOLOR(newpos+0, SNAILCOLORS);
	SETCOLOR(newpos+2, SNAILCOLORS);
#endif
	
	snail->x = newx;
	snail->y = newy;
	oldpos = newpos;	// because we use oldpos below
	
done:
	// make snail's position blocking again:
	for (lx = 0; lx < SNAILWIDTH; lx++)
	{
		SETBLOCK(oldpos+lx, 1);
	}
}

// Note: caller is responsible for cache level adjustments now!

void RunSnailas(void)
{
	void alOut(byte, byte);	// hack import for AdLib sound
	
	int i;
	snailtype *snail;
	unsigned lasttime = TimeCount;
	
	// set up instrument for AdLib sound:
	if (SoundMode == sdm_AdLib)
	{
		// seq contains packed register/value entries to set up the instrument for
		// our random sound effects (high byte is register, low byte is value)
		
		//static word seq[] = {0x202C,0x23A1,0x40C0, 0x4300, 0x60F9, 0x63C0, 0x80FF, 0x83FF, 0xE000, 0xE300};
		static word seq[] = {0x2061,0x2322,0x408A, 0x4315, 0x6075, 0x6374, 0x801F, 0x830F, 0xE000, 0xE300};
		
		for (i=0; i<(sizeof(seq)/sizeof(seq[0])); i++)
		{
			// get packed value:
			_DX = seq[i];
			// split it into register and value:
			asm	xor	ax, ax;
			asm	xchg	dh, al;
			// now _AX is register, _DX is value
			alOut(_AX, _DX);
		}
		
		// Note: We could just play a sound effect here to set up the instrument
		// for the AdLib sound and thus get rid of this code (and the seq array)
		// to make the code smaller. The sound should have a length of 1 for this.
	}
	
	VW_ClearVideo(BLACK);	// avoids glitches when switching to text mode
	
	// caller takes care of this now
	/*
	CA_UpLevel();
	CA_SetGrPurge();	// make room for our dynamic buffer
	*/
	
	IN_ClearKeysDown();
	
	// clear screen and set 80x25 text mode:
	asm	mov	ax, 3;
	asm	int	10h;
	
	// hide cursor:
	asm	mov	ax,  100h;
	asm	mov	cx, 2000h;
	asm	int	10h;
	
	// allocate buffer for snail variables:
#ifdef SNAILS_EXTERN
	// cache the pre-defined SNAILAS data:
	CA_CacheGrChunk(SNAILAS_DATA);
	
	// and set DS to that buffer, so we can use near pointers and direct access:
	_DS = (unsigned)grsegs[SNAILAS_DATA];
	
#else
	
	// Note: MM_GetPtr will quit to DOS if there is not enough memory available,
	// but since we made all (non-locked) graphics purgable above, we should
	// always have enough free memory at this point.
	MM_GetPtr(&snailbuffer, sizeof(TSnailVars));
	
	// and set DS to that buffer, so we can use near pointers and direct access:
	_DS = (unsigned)snailbuffer;
	
	// clear the screen buffer:
	for (i = 0; i < SCREENSIZE; i++)
	{
		SETCOLOR(i, DEFAULTCOLORS);
		SETGLYPH(i, INITIALGLYPH);
		SETBLOCK(i, 0);	// need to initialize this as well...
	}
	
	// initialize the snails:
	for (snail = FIRSTSNAIL; snail < FIRSTSNAIL+NUMSNAILS; snail++)
	{
		unsigned pos;
		
		snail->x = random(MAXX+1);
		snail->y = random(MAXY+1);
		snail->dir = random(2)*2 - 1;	// random(2) returns 0 or 1, dir is set to -1 or 1 here
		snail->sl = 0;
		snail->col = (random(6)+1+8) | BACKGROUND;
		
		pos = snail->x + snail->y*SCREENWIDTH;
		
		for (i = 0; i < SNAILWIDTH; i++)
		{
			SETBLOCK(pos + i, 1);
			// the rest is useless (will be overwritten below anyway)
			//SETGLYPH(pos + i, ERASEGLYPH);
			//SETCOLOR(pos + i, DEFAULTCOLORS);
		}
		SETGLYPH(pos + 1, '@');	// 64
		SETGLYPH(pos + 1 + snail->dir, '"');	// 34
		SETGLYPH(pos + 1 - snail->dir, '_');	// 95
		
		SETCOLOR(pos + 1, snail->col);
#if (SNAILCOLORS != DEFAULTCOLORS)
		SETCOLOR(pos + 0, SNAILCOLORS);
		SETCOLOR(pos + 2, SNAILCOLORS);
#endif
	}
#endif
	
	// run the "simulation":
	i = 0;
	while (true)
	{
		// temporarily switch back to default data segment for sound and delay:
		asm	push	ds;
		asm	mov	ax, ss;
		asm	mov	ds, ax;
		
		// play a random noise on the selected sound hardware:
		if (SoundMode == sdm_PC)
		{
			asm	in 	al, 61h;
			asm	or 	al, 3;
			asm	out	61h, al;
			asm	mov	al, 0B6h;
			asm	out	43h, al;
			_AX = US_RndT()*48 + 0xE62;	// random frequency between 75 Hz and 325 Hz
			asm	out	42h, al;
			asm	mov	al, ah;
			asm	out	42h, al;
		}
		else if (SoundMode == sdm_AdLib)
		{
			alOut(alFreqH + 0, 0x2D);	// change low nybble (last hex digit) to adjust base sound frequency
			alOut(alFreqL + 0, US_RndT());
		}
		// Note that the sound is NOT turned off after the delay below. I chose to
		// keep the sound playing so that we don't get an awkward silence while
		// the code is updating the snails and putting the characters on the
		// screen. The update can take some time on slow systems.
		
		// wait until 3 ticks (about 43 milliseconds) have elapsed
		// (and exit if the user pressed a key or button):
		while ((unsigned)TimeCount - lasttime < 3)
		{
			if (IN_IsUserInput())
			{
				asm	pop	ax;	// remove pushed DS from stack
				goto done;
			}
		}
		lasttime = TimeCount;
		// Note: This delay defines how quickly the sound effect changes to the
		// next frequency and also has an impact on the update rate of the snails.
		// If you want to modify the snail's speed without messing with the sound,
		// change the 'if (i == 5)' check below. Increase the number to make the
		// snails slower, decrease it to make them faster.
		
		// back to our buffer DS:
		asm	pop	ds;
		
		if (i == 6)
		{
			for (snail = FIRSTSNAIL; snail < FIRSTSNAIL+NUMSNAILS; snail++)
			{
				if (snail->sl > 0)	// check sleep counter
				{
					if (--snail->sl != 0)
					{
						SETGLYPH(snail->x + 1 + snail->dir + snail->y*SCREENWIDTH, (snail->sl & 1? 'Z':'z'));	// 90 or 122
					}
					else
					{
						MoveSnail(snail, 0, 0);	// don't move, just redraw
					}
				}
				else
				{
					if (random(70) > 55)
					{
						snail->dir = -snail->dir;
					}
					else if (random(65) > 50)
					{
						MoveSnail(snail, snail->dir, random(2)*2-1);
					}
					else
					{
						MoveSnail(snail, snail->dir, 0);
					}
					
					if (random(50) == 45)
					{
						// start sleeping:
						unsigned pos = snail->x + snail->y*SCREENWIDTH;
						snail->sl = 10;	// sleep for 10 frames
						SETGLYPH(pos + 1 + snail->dir, 'z');	// 122
						SETGLYPH(pos + 1 - snail->dir, ' ');	// 32
					}
				}
			}
			i = 0;
			
			// wait until a vertical retrace has just started:
			// Note: VW_WaitVBL() can miss the vertical retrace when the game is
			// running on a slow system and music is enabled, which can lead to
			// VERY long delays before the screen is updated. It's probably better
			// not to wait for a retrace here. Alternatively, we could stop music
			// playback at the beginning of this routine to avoid this problem.
			
			// Note: VW_WaitVBL() doesn't access any variables in the default data
			// segment, so we don't need to change register DS before calling it.
			
			//VW_WaitVBL(1);
			
#ifndef SNAILS_EXTERN
			// Shadow
			PutText("                     ", 29,  8, BLACK|(BLACK<<4));
			PutText("                     ", 29,  9, BLACK|(BLACK<<4));	
			PutText("                     ", 29, 10, BLACK|(BLACK<<4));	
			PutText("                     ", 29, 11, BLACK|(BLACK<<4));	
			PutText("                     ", 29, 12, BLACK|(BLACK<<4));	
			PutText("                     ", 29, 13, BLACK|(BLACK<<4));	
			PutText("                     ", 29, 14, BLACK|(BLACK<<4));	
			PutText("                     ", 29, 15, BLACK|(BLACK<<4));	
			PutText("                     ", 29, 16, BLACK|(BLACK<<4));	
			PutText("                     ", 29, 17, BLACK|(BLACK<<4));	
			PutText("                     ", 29, 18, BLACK|(BLACK<<4));
			// Border			
			PutText("                     ", 30,  7, BLACK|(BROWN<<4));
			PutText("                     ", 30,  8, BLACK|(BROWN<<4));
			PutText("                     ", 30,  9, BLACK|(BROWN<<4));
			PutText("                     ", 30, 10, BLACK|(RED<<4));
			PutText("                     ", 30, 11, BLACK|(BROWN<<4));
			PutText("                     ", 30, 12, BLACK|(BROWN<<4));
			PutText("                     ", 30, 13, BLACK|(RED<<4));
			PutText("                     ", 30, 14, BLACK|(BROWN<<4));
			PutText("                     ", 30, 15, BLACK|(RED<<4));
			PutText("                     ", 30, 16, BLACK|(RED<<4));
			PutText("                     ", 30, 17, BLACK|(RED<<4));
			// Text field 
			PutText("                 ", 32,  8, YELLOW|(BLACK<<4));
			PutText("    _SNAILAS\"    ", 32,  9, YELLOW|(BLACK<<4));
			PutText("                 ", 32, 10, LIGHTGRAY|(BLACK<<4));
			PutText("    0 CREDIT     ", 32, 11, RED|(BLACK<<4));
			PutText("                 ", 32, 12, LIGHTRED|BLINK|(BLACK<<4));
			PutText("     INSERT      ", 32, 13, LIGHTRED|BLINK|(BLACK<<4));
			PutText("      COIN       ", 32, 14, LIGHTRED|BLINK|(BLACK<<4));
			PutText("    TO PLAY!     ", 32, 15, LIGHTRED|BLINK|(BLACK<<4));
			PutText("                 ", 32, 16, LIGHTGRAY|(BLACK<<4));
#endif
			
			// copy map data into video memory:
			movedata(FP_SEG(MAPSTART), FP_OFF(MAPSTART), 0xB800, 0, SCREENSIZE*2);
		}
		i++;
	}
done:
	
	// make sure sound is stopped:
	SD_StopSound();
	// This will turn off PC Speaker / AdLib sound generation, even when we're
	// not actually playing any sound via the Sound Manager at the moment.
	
#ifdef SNAILS_EXTERN
	// We *could* free the chunk to force the minigame to re-load the initial
	// data from the EGAGRAPH file instead of continuing where we last left off,
	// but we don't have to. Since not freeing the chunk results in smaller code,
	// I am going to leave it commented out for now...
	
	//MM_FreePtr(&grsegs[SNAILAS_DATA]);
#else
	// free the buffer:
	// don't need to check if it's non-zero, MM_GetPtr would have bombed on error
	MM_FreePtr(&snailbuffer);
#endif
	
	VW_SetScreenMode(GRMODE);
	
	// caller takes care of this now
	/*
	CK_SetLoadingTransition();	// fade out to avoid display glitches in DOSBox
	IN_ClearKeysDown();
	CA_DownLevel();
	CK_ForceScoreboxUpdate();
	RF_ForceRefresh();
	*/
}
