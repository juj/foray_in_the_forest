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

// KD_INTER.C

#include "CK_DEF.H"
#include <stddef.h>
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

void CK_PlayDemo(int demoNumber);
void CK_SubmitHighScore(long score, unsigned arg_4, int cans, int arg_5);
void CK_DoHighScores(void);
void CK_DrawTerminator(void);
void CK_DrawStarWars(void);


/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

/*
 * CK_INTER: Holds an assortment of screen drawing and state switching routines
 */


boolean ck_inHighScores = false;

void CK_MainMenu(void)
{
	US_ControlPanel();
	if (restartgame)
	{
		ck_levelState = LS_StartingNew;	// K1n9_Duk3 fix: use enum name instead of number
	}
	else if (loadedgame)
	{
		ck_levelState = LS_StartingSaved;	// K1n9_Duk3 fix: use enum name instead of number
	}
}

void CK_HandleDemoKeys()
{
	if (!LastScan)
		return;

	/*
	 * if Keen started with demo parameter
	 * ck_levelState = 5;
	 * startingDifficulty = 2;
	 * IN_ClearKeysDown();
	 * CK_NewGame(); (init_keen_stats)
	 * return;
	 */

	if (LastScan == sc_F1)
	{
		HelpScreens();
		return;
	}

	// Otherwise, start the wristwatch menu
	CK_MainMenu();
}

//===========================================================================
//
// TERMINATOR SCREEN
// 
//===========================================================================

#define TERMINATORSCREENWIDTH 248


// Pointers to the two monochrome bitmaps that are scrolled during the intro

typedef struct introbmptypestruct {
	int height, width;
	unsigned linestarts[200];
	byte data[];
} introbmptype;


// K1n9_Duk3 says: Using these _seg * variables is unsafe and unnecessary. They
// are basically the same as grsegs[I_KEENEXTERN] and grsegs[I_COMMANDEREXTERN],
// they just use a different type to provide access to the header information.
// But unlike the grsegs values, these pointers don't get updated when the
// memory manager sorts memory blocks. You'd have to re-assign these variables
// to the grsegs values after running any code that might cause a memory sort.
// Those assignments were missing in a couple of places in the original code.
// It is much simpler and safer to define macros that wrap the grsegs entries
// in a typecast to replace these variables.
	
//introbmptype _seg *ck_introKeen;
//introbmptype _seg *ck_introCommander;
#define ck_introKeen ((introbmptype _seg *)(grsegs[I_KEENEXTERN]))
#define ck_introCommander ((introbmptype _seg *)(grsegs[I_COMMANDEREXTERN]))


// segment pointers to various graphic chunks
memptr introbuffer;
memptr introbuffer2;
memptr shiftedCmdrBMPsegs[8];

byte terminator_blackmasks[] = {0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01}; 
byte terminator_lightmasks[] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE}; 

// terminator_palette1 is the "zoom out" palette:
byte terminator_palette1[17] = {0,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,2, 0}; // K1n9_Duk3 mod
/*
This should use the non-overlapping KEEN/COMMANDER text color for everything,
except for the background color (the first number, which should be the same as
the one in the terminator_palette2 array) and the border color (the last number
in the array, which doesn't matter, because it will be overwritten anyway).
*/

// regular "Terminator" intro palette:
byte terminator_palette2[17] = {
	0,	// background color
	2,	// KEEN color
	2,	// COMMANDER color
	0x1A,	// overlapping color
	
	0x00,0x00,0x00,0x00,	// credits plaque color #1 (dummy values!)
	0x05,0x05,0x05,0x05,	// credits plaque color #2 (dummy values!)
	0x1C,0x1C,0x1C,0x1C,	// credits plaque color #3 (dummy values!)
	
	0	// border color dummy value
};
/*
Note: The credits plaque color values here will be overwritten with values from
the palettes array in AdvanceTerminatorCredit(). The border color dummy value
will always be overwritten with the current bordercolor value before the palette
is applied.

Also: The palettes should use the numbers 0,1,2,3,4,5,6,7 for the low-intensity
(dark) colors and 0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F for the high-intensity
(light) colors.
*/

// chunk #'s for the vertically scrolling pics
int terminator_pics[5] = {I_IDMODPIC, I_ARTDIRECTORPIC, I_PROGRAMMERPIC, I_TOOLSMITHPIC, I_MUSICIANPIC};
// storage for the vertically scrolling pics
memptr terminator_pic_memptrs[5];


unsigned ck_introCommanderWidth;
unsigned ck_introScreenWidth;


// type-identified variables
int  ck_currentTermPicTimeRemaining;
int ck_currentTermPicStartTime;

// Which terminator credit image we're drawing
int ck_currentTerminatorCredit;

// What action the terminator credit is currently doing (loading/flying up/pausing/flying out)
int ck_termCreditStage;
// int rowsToDraw;

unsigned *word_499D1;
memptr ck_currentTermPicSeg;

unsigned word_499CB;
// unknown vairables


word word_46CA2[2];
word terminatorPageOn; // always 0 or 1? Maybe a pageflip thing?
word word_46CB0;


word word_499C7;
word terminatorOfs;	 	// Start of the screen buffer; the KEEN graphic is aligned to the left edge of this
byte terminatorPlaneOn;
word ck_currentTermPicNextLineDist;
word ck_currentTermPicSize;
word ck_currentTermPicHeight;
word ck_currentTermPicHalfWidth;
word ck_currentTermPicWidth;
word ck_termPicHeights[5];
word ck_termPicWidths[5];

word word_499FF;



void DoubleSizeTerminatorPics(int pic);
int AdvanceTerminatorCredit(int elapsedTime);
void ScrollTerminatorCredits(word elapsedTime, word xpixel);
void AnimateTerminator(void);
void TerminatorExpandRLE(word far *src, byte far *dest);
void JoinTerminatorPics(void);
void ZoomOutTerminator_1(word far *arg0, word arg4, int leftOffset, word far *arg8);
void ZoomOutTerminator(void);
void FinishPage(void);

// Caches vertically scrolling terminator pics
// and unsignedly extends each byte of the pic to a word

// Presumably this is because we're drawing in a different graphics mode?
void DoubleSizeTerminatorPics(int pic)
{

	byte far *picptr;
	word far *double_picptr;
	int width, height;
	unsigned i, size;
	int picchunk, pictableindex;

	// Cache the standard-sized picture
	picchunk = terminator_pics[pic];
	CA_CacheGrChunk(picchunk);

	// Allocate memory for the expanded picture
	// and save its dimensions
	pictableindex = picchunk - STARTPICS;
	ck_termPicWidths[pic] = width = (pictable + (pictableindex))->width;
	ck_termPicHeights[pic] = height = (pictable + (pictableindex))->height;
	size = width * height * 2;
	MM_GetPtr(&terminator_pic_memptrs[pic], size * 2);

	// Copy the data to the expanded buffer
	picptr = grsegs[picchunk];
	double_picptr = (word far *)terminator_pic_memptrs[pic];

	for (i = 0; i < size; i++)
		*double_picptr++ = *picptr++ * 2;

	// Free the standard-sized picture
	MM_FreePtr(&grsegs[picchunk]); 
}

// Returns the Y-position of the terminator credit
int AdvanceTerminatorCredit(int elapsedTime)
{
	static byte palettes[15] = {
		0x00, 0x19, 0x07,	// for 1st pic (mod)	blue: 	0x00, 0x01, 0x19					
		0x00, 0x05, 0x14,	// for 2nd pic (Nisaba) purple: 0x00, 0x05, 0x14,
		0x00, 0x04, 0x1C,	// for 3rd pic (K1n9_Duk3) red: 0x00, 0x04, 0x1C,
		0x00, 0x1F, 0x07,	// for 4th pic (tools) 	white: 0x00, 0x1E, 0x06 / 0x1C
		0x00, 0x1E, 0x1C	// for 4th pic (music) 	yellow: 0x00, 0x1E, 0x06 / 0x1C
	};

	int bx;

	switch(ck_termCreditStage)
	{

	case -1:
		// loading
		ck_currentTermPicSeg = terminator_pic_memptrs[ck_currentTerminatorCredit];
		ck_currentTermPicWidth = ck_termPicWidths[ck_currentTerminatorCredit];
		ck_currentTermPicHalfWidth = (ck_currentTermPicWidth+3)>>1;
		ck_currentTermPicHeight = ck_termPicHeights[ck_currentTerminatorCredit];
		ck_currentTermPicNextLineDist = linewidth - (ck_currentTermPicWidth+1);
		ck_currentTermPicSize = (ck_currentTermPicWidth * ck_currentTermPicHeight) << 1;
		ck_termCreditStage++;

		// copy the color bytes from the palettes array into the
		// terminator_palette2 array, extending each byte into 4
		// bytes and then apply the new palette.
		asm {
			push	ds;
			pop	es;
			mov	ax, ck_currentTerminatorCredit;
			mov	si, ax;
			shl	ax, 1;
			add	ax, offset palettes;
			add	si, ax;	// si is now &palettes[3*ck_currentTerminatorCredit]
			mov	di, (offset terminator_palette2) + 4;
			mov	cx, 3;	// read 3 bytes
		}
copyloop:
		asm {
			lodsb;
			stosb;	// write
			stosb;	// each
			stosb;	// byte
			stosb;	// 4 times
			loop	copyloop;
			mov	dx, offset terminator_palette2;
			mov	ax, 1002h;
			int	10h;
		}

		ck_currentTermPicStartTime = elapsedTime;
		ck_currentTermPicTimeRemaining = 200 + 1 + (ck_currentTermPicHeight>>1);
		// no break, flow into case 0

	case 0:

		// Flying up from bottom of screen
		ck_currentTermPicTimeRemaining -= (elapsedTime - ck_currentTermPicStartTime)<<1;

		if (ck_currentTermPicTimeRemaining < 100)
		{
			ck_currentTermPicTimeRemaining = 100;
			ck_termCreditStage++;
		}

		ck_currentTermPicStartTime = elapsedTime;

		return ck_currentTermPicTimeRemaining - (ck_currentTermPicHeight>>1);


	case 1:

		// Pausing in the middle
		if (elapsedTime - ck_currentTermPicStartTime > 214)	// Nisaba patch: reduce speed
		{
			ck_termCreditStage++;
			ck_currentTermPicStartTime = elapsedTime;
		}

		return 100 - (ck_currentTermPicHeight>>1);

	case 2:

		// Flying up and out of the top of the screen
		ck_currentTermPicTimeRemaining -= (elapsedTime - ck_currentTermPicStartTime)<<1;

		if (ck_currentTermPicTimeRemaining < -1 - (int)(ck_currentTermPicHeight>>1))	// ck_currentTermPicHeight is unsgined, but we need a signed comparison here!
		{
			ck_currentTermPicTimeRemaining = -1 - (int)(ck_currentTermPicHeight>>1);	// +1 in case the height is an odd value, to make sure it's out of sight

			// Are there any more terminator credit images?
			if (++ck_currentTerminatorCredit < 5)
				ck_termCreditStage = -1;
			else
				ck_termCreditStage = 4;
		}

		ck_currentTermPicStartTime = elapsedTime;

		return ck_currentTermPicTimeRemaining - (ck_currentTermPicHeight>>1);
	
	default:
		break;

	}

		return -ck_currentTermPicHeight;
}

void ScrollTerminatorCredits(word elapsedTime, word xpixel)
{
	word pelpan, var4, varC,var10;

	// Vars are static because BP is used during ASM draw routine
	static int rowsToDraw;


	int creditY1, varA, creditY2, var12, varE;

	pelpan = xpixel & 7;
	var4 = terminatorOfs + (xpixel>>3) + (20 - ck_currentTermPicWidth/2);

	EGAMAPMASK(0xC);

	creditY1 = AdvanceTerminatorCredit(elapsedTime);
	creditY2 = creditY1 + ck_currentTermPicHeight;

	if (creditY2 < 0)
		creditY2 = 0;

	varA = word_46CA2[terminatorPageOn];

	// Erasing the area underneath the credit
	if (creditY2 < 200 && varA > creditY2)
	{
		varC = varA - creditY2;
		var10 = ylookup[creditY2] + var4;

asm		mov es, screenseg;
asm		mov bx, linewidth;				// bx = next line distance
asm		sub bx, ck_currentTermPicHalfWidth;
asm		sub bx, ck_currentTermPicHalfWidth;
asm		mov di, var10;
asm		mov dx, varC;
asm		mov si, ck_currentTermPicHalfWidth;
asm		xor ax, ax;


nextrow1:
asm		mov cx, si;
asm		rep stosw;
asm		add di, bx;
asm		dec dx;
asm		jnz nextrow1;
	}

	// loc_140AB
	if (creditY2 > 200)
		creditY2 = 200;

	word_46CA2[terminatorPageOn] = creditY2;

	rowsToDraw = ck_currentTermPicHeight;
	varE = 0;

	if (creditY1 < 0)
	{
		// clipped by screen top
		varE = -creditY1 * (ck_currentTermPicWidth<<1);
		rowsToDraw += creditY1;
		creditY1 = 0;
	}
	else if (creditY1 + ck_currentTermPicHeight > 200)
	{
		// clipped by screen bot
		rowsToDraw = 200 - creditY1;
	}


	// loc_14108
	word_499CB = varE + ck_currentTermPicSize;

	if (rowsToDraw > 0)
	{
		word_499D1 = shifttabletable[pelpan];

		word_46CB0 = ylookup[creditY1] + var4;

asm		mov bx, varE;
asm		push bp;
asm		mov bp, word_499D1;
asm		mov es, screenseg;
asm		mov ds, ck_currentTermPicSeg;
asm		mov ah, 4;
asm		mov ss:terminatorPlaneOn, ah;

nextplane:
asm		mov dx, SC_INDEX;
asm		mov al, SC_MAPMASK;
asm		out dx, ax;
asm		mov dx, ss:rowsToDraw;
asm		mov di, ss:word_46CB0;

nextrow:
asm		mov cx, ss:ck_currentTermPicWidth;
asm		xor al, al;

nextX:
asm		mov si, [bx];
asm		add bx, 2;
asm		xor ah, ah;
asm		or ax, [bp+si];
asm		stosb;
asm		mov al, ah;
asm		loop nextX;


asm		stosb;
asm		mov word ptr es:[di], 0;
asm		add di, ss:ck_currentTermPicNextLineDist;
asm		dec dx;
asm		jnz nextrow;


asm		mov bx, ss:word_499CB;
asm		mov ah, ss:terminatorPlaneOn;
asm		shl ah, 1;
asm		mov ss:terminatorPlaneOn, ah;
asm		cmp ah, 0x10;
asm		jnz nextplane;

asm		pop bp;				// Restore stack
asm		mov ax, ss;
asm		mov ds, ax;

				
	}

}


// Does the terminator scrolling 
void AnimateTerminator(void)
{

	int var6;
	long delaytime;

	// At the end of the terminator scrolling, left edge of the commander graphic is
	// this many pixels from the Left edge of the Visible screen
	// (i.e., a negative value means an offset to the left)
	int finalCmdrPos;

	// The COMMANDER graphic scrolls this many pixels to the left
	int totalCmdrScrollDist;

	// How far it's currently scrolled
	int elapsedCmdrScrollDist;
	
	// How many pixels the left edge of the screen is away from the start of the KEEN image
	int xpixel;

	// start of the visible area in the screen buffer
	unsigned screenofs;

	unsigned elapsedTime;

	// The pixel modulo 8 that the left edge of the scrolling CMDR graphic is aligned to,
	// relative to the left edge of the buffer
	int cmdrBMPpelpan;

	// Which of the shifted "COMMANDER" graphics to use
	memptr bmpsrcseg;
	
	// variables of unknown type
	int screenWidthInPx, maxTime, var18, var1A, var1C, var1E, var20, var22;

	EGAWRITEMODE(0);
	EGAREADMAP(0);
	ck_introScreenWidth = ck_introKeen->width + 200;

	EGAREADMAP(1);
	
	finalCmdrPos = 120 - ck_introCommander->width;
	screenWidthInPx = 320;
	totalCmdrScrollDist = finalCmdrPos - screenWidthInPx;
	maxTime = ABS(totalCmdrScrollDist);

	terminatorPageOn = terminatorOfs = 0;

	// delay for a tic before starting
	lasttimecount = TimeCount;
	while (lasttimecount == TimeCount)
		;

	lasttimecount = TimeCount;

	// Update the terminator
	for (elapsedTime = 0; elapsedTime <= maxTime; elapsedTime += tics)
	{

		// Reposition the left edge of the screen in direct proportionality to time elapsed
		xpixel = ((long)ck_introScreenWidth*(long)(maxTime-elapsedTime)) / (long)maxTime;

		// Scroll the Credits graphics 
		ScrollTerminatorCredits(elapsedTime, xpixel);

		elapsedCmdrScrollDist = screenWidthInPx + ((long)totalCmdrScrollDist * (long)elapsedTime)/ (long)maxTime;
		elapsedCmdrScrollDist += xpixel&7;

		var6 = (elapsedCmdrScrollDist + 0x800) / 8 - 0x100;
		cmdrBMPpelpan = ((elapsedCmdrScrollDist + 0x800) & 7);

		bmpsrcseg = shiftedCmdrBMPsegs[cmdrBMPpelpan];
		var18 = 0;

		screenofs = terminatorOfs + xpixel/8;

		if (var6 > 0)
		{
			// The COMMANDER graphic has not started to scroll off the left edge of the screen
			var1A = (var6+1)/2;

			if (var6 & 1)
				screenofs--;

			var1C = 21 - var1A;
			var1E = 0;
		}
		else if (var6 > (int)(320/8 - ck_introCommanderWidth))
		{
			// The COMMANDER graphic is clipped on both left and right edges
			var1A = 0;
			var1C = 21;
			var1E = 0;
			var18 -= var6;
		}
		else
		{
			// The COMMANDER graphic is only clipped by the left side of the screen
			var1A = 0;
			var1C = (ck_introCommanderWidth+var6)/2;
			var1E = 21 - var1C;
			var18 -= var6;
		}

		// loc_1445B:
		var20 = ck_introCommanderWidth - (var1C<<1);
		var22 = TERMINATORSCREENWIDTH - ((var1A + var1C + var1E) << 1);

		EGAMAPMASK(2);


asm		mov di, screenofs;
asm		mov es, screenseg;
asm		mov si, var18;
asm		mov word_499FF, si;
asm		mov ds, bmpsrcseg;

// 		For each row
asm		mov dx, 200;

loopb0:
// Write black pixels to the left of COMMANDER
asm		xor ax, ax;
asm		mov cx, var1A;
asm		rep stosw;

// Write COMMANDER bmp data
asm		mov cx, var1C;
asm		rep movsw;

// Write black pixels to the right of COMMANDER
asm		xor ax, ax;
asm		mov cx, var1E;
asm		rep stosw;


asm		test dx, 1;
asm		jnz odd;

even:
asm		mov si, ss:word_499FF;
asm		jmp ahead;

odd:
asm		add si, var20;
asm		mov ss:word_499FF, si;

ahead:
asm		add di, var22;
asm		dec dx;
asm		jnz loopb0;


asm		mov ax, ss;
asm		mov ds, ax;



		// Flip to the back page while panning the screen to the left 
		VW_SetScreen(xpixel/8 + terminatorOfs,xpixel&7);

		// Set the new back page
		if (terminatorPageOn ^=1)
			terminatorOfs = TERMINATORSCREENWIDTH/2;
		else
			terminatorOfs = 0;


		// Delay
		do {
			delaytime = TimeCount;
		} while ((tics = delaytime - lasttimecount) < 1);	// K1n9_Duk3 mod: only wait 1 tic per frame instead of 2

		lasttimecount = delaytime;


		// Stop drawing if key pressed
		if (IN_IsUserInput() && LastScan != sc_F1)	// K1n9_Duk3 fix: this is "!= sc_F1" in Keen 4-6
			LastScan = sc_Space;

		if (LastScan)
			return;

	}

	word_499C7 = xpixel/8;

}

// RLE-Expands one line of monochrome terminator BMP data
// Each source line is a sequence of WORD data, terminated with an 0xFFFF flag
// Each line of BMP data is a sequence of word values that encodes alternating runs of black and white pixels,
//  always starting with BLACK pixels

void TerminatorExpandRLE(word far *src, byte far *dest)
{
	// cx = next source word
	// si = number of pixels of one color remaining, in one byte, before transition to the alternate color
	// dl = byte to write at the transition between black and white
	register word si, di, cx;
	register byte dl;

	si = dl = 0;

	while ((cx = *src++) != 0xFFFF)
	{
		// Expand a Black Run of pixels
		if ((si += cx) > 7)
		{
			// write the transition byte
			*dest++ = dl;

			// the leftmost pixels of the transition byte will be black
			dl = 0;

			// write complete bytes
			di = si/8-1;
			while (di--)
				*dest++ = 0;

			// count the leftover pixels on this last byte
			si &= 7;
		}

		// loc_145CC
		// Check if we've hit a stop signal
		if ((cx = *src++) == 0xFFFF)
		{
			*dest++ = dl;
			*dest = 0;
			return;
		}

		// the lowest bits in this byte will be black (zero)
		// so we OR the right most remaining bits so that they are drawn white
		dl |= terminator_blackmasks[si];

		// Expand a Light Run of pixels
		if ((si += cx) > 7)
		{
			// write the transition byte
			*dest++ = dl;

			// the leftmost pixels of the transition byte will be light
			dl = 0xFF;

			// write runs of 8 light pixels
			di = si/8-1;
			while (di--)
				*dest++ = 0xFF;

			// count the leftover light pixels on this last byte
			si &= 7;
		}

		// the lowest bites in this byte will be light ( set)
		// so we AND the rightmost remaining bits so that they are drawn black
		dl &= terminator_lightmasks[si];
	}

	// write the final byte, and one padding byte of black
	*dest++ = dl;
	*dest = 0;
}

// Generate an RLE-compressed bitmap with COMMANDER KEEN separated by some space
// Except... this RLE-compressed bitmap seems to be in a slightly different format
// than those stored in the game archive
//
// Here, alternating runlengths of pixels are not specified by their length, but by their start position
// 
void JoinTerminatorPics(void)
{

	// Points to the data in the stitched bmp
	word far *introBMPDataPtr;

	int i;

	// Allocate memory for the COMMANDER KEEN graphic
	MM_GetPtr(&introbuffer2, 30000);

	// Where we start writing the bitmap data
	introBMPDataPtr = MK_FP(introbuffer2, offsetof(introbmptype, data));

	// for each row
	for (i = 0; i < 200; i++)
	{
		word count, inword;
		word far *linestart;

		// Generate a pointer to this line of data
		((introbmptype far *)introbuffer2)->linestarts[i] = FP_OFF(introBMPDataPtr);

		count = 0;
		EGAREADMAP(1);
		
		linestart = MK_FP(ck_introCommander, ck_introCommander->linestarts[i]); 
		inword = *linestart++;

		do 
		{
			*introBMPDataPtr++ = count;
			count += inword;
			inword = *linestart++;

		} while (inword != 0xFFFF);

		// Add some space between the COMMANDER and the KEEN
		count += 80;

		EGAREADMAP(0);


		linestart = MK_FP(ck_introKeen, ck_introKeen->linestarts[i]);
		linestart++;
		inword = *linestart++;

		do {

			*introBMPDataPtr++ = count;
			count += inword;
			inword = *linestart++;

		} while (inword != 0xFFFF);

		*introBMPDataPtr++ = count;
		*introBMPDataPtr++ = 0xFFFF;

	}
}

//ZoomOutTerminator_1(var16, si+yBottom, leftOffset, introbuffer);
void ZoomOutTerminator_1(word far *arg0, word arg4, int leftOffset, word far *arg8)
{

	word si;


	// X coord where the zoomed bitmap is clipped by the left edge of the screen
	int leftclip;

	int dx, var8;

	byte far *vidPtr; // Pointer into video memory
	byte writebyte;

	unsigned pixelrun;  // The width?
	unsigned varA;

	writebyte = pixelrun = 0;

	var8 = 320-leftOffset;

	vidPtr = MK_FP(0xA000 , (ylookup[arg4] + terminatorOfs + word_499C7));

	if (leftOffset < 0)
	{
		// The zooming bitmap is clipped by the left edge of the screen
		// So find out where we start drawing it
		
		leftclip = -leftOffset;
		writebyte = pixelrun = 0;

		do {

			si = *arg0++;
			dx = *(arg8 + si);
			if ( dx > leftclip)
				goto loc_14852;

			si = *arg0++;
			dx = *(arg8 + si);
			if ( dx > leftclip)
				goto loc_147FB;

		} while (1);
	}
	else
	{
		// loc_147C2:
		// The zooming bitmap is not clipped by left edge of screen
		writebyte = 0;
		pixelrun = leftOffset&7;
		vidPtr += leftOffset/8;
		leftclip = 0;
		arg0++;
		goto loc_147E5;
	}


loc_14852:
	do
	{
		// Writing a run of pixels
		varA = dx - leftclip;
		leftclip = dx;

		// get the run of pixels for the first byte
		writebyte |= terminator_blackmasks[pixelrun];

		if ((pixelrun += varA) > 7)
		{

			int cx;
			// write the first byte
			*vidPtr++ = writebyte; 

			// write bytes of pixels
			writebyte = 0xFF;
			for (cx = pixelrun/8 - 1; cx; cx--)
				*vidPtr++ = 0xFF; // = writebyte;	

			// remaining pixels to write
			pixelrun &= 7;
		}

		if (dx > var8)
			return;

		writebyte &= terminator_lightmasks[pixelrun];

loc_147E5:
		si = *arg0++;
		dx = *(arg8 + si);

loc_147FB:

		// Writing a run of pixels
		varA = dx - leftclip;
		leftclip = dx;
		if ((pixelrun += varA)> 7)
		{

			int cx;
			// write the first byte
			*vidPtr++ = writebyte; 

			// write bytes of pixels
			writebyte = 0;
			for (cx = pixelrun/8 - 1; cx; cx--)
				*vidPtr++ = 0; // = writebyte;	

			// remaining pixels to write
			pixelrun &= 7;

		}

		if (dx > var8)
			return;

		if ((si = *arg0++) != 0xFFFF)
			dx = *(arg8 + si);
		else
			break;
	} while (1);

	// Write black until the end of the screen?
	varA = 320 - leftclip;
	if ((pixelrun += varA) > 7)
	{

		int cx;
		// write the first byte
		*vidPtr++ = writebyte; 

		// write bytes of pixels
		writebyte = 0;
		for (cx = pixelrun/8 - 1; cx; cx--)
		       *vidPtr++ = 0; // = writebyte;	

		// remaining pixels to write
		pixelrun &= 7;
		// return;
	}

	return;

}

// The COMMANDER and KEEN RLE-Encoded bitmaps are first joined together into one big
// RLE-encoded bitmap, which is then scaled and translated to its final position.

// NOTE: I think that this still has errors, which are visible when setting maxTime to a large value
// But... it looks good enough at normal game speed
void ZoomOutTerminator(void)
{
	long startingLeftOffset;
	unsigned yBottom, var18;
	unsigned var20[2];
	unsigned var1C, si;

	// The px offset of the left edge of the bitmap from the left edge of the screen
	// (i.e., negative number means graphic is clipped by left edge of screen
	int leftOffset;

	unsigned elapsedTime, maxTime;
	word far* var16;

	// finalHeight looks like final Height?
	word newTime, var8, finalHeight, varC, varE;


	// Set the palette
asm	mov     ax, ds;
asm	mov	es, ax;
asm	mov	dx, offset terminator_palette1;
asm	mov	ax, 0x1002;
asm 	int	0x10;

	EGAREADMAP(1);

	// The starting (negative) offset of the COMMANDER graphic from the left edge of the screen
	startingLeftOffset = 120 - ck_introCommander->width;

	JoinTerminatorPics();


	MM_GetPtr((memptr *)&introbuffer, 5000);
		
	var8 = 256;
	finalHeight = 33;
	var20[0] = var20[1] = 200;
	elapsedTime = 1;
	maxTime = 30; //3000; // Set to large value to slow down the zoom
	

	// elapsedTime seems to be a timer, maxTime is the max time
	while (elapsedTime <= maxTime)
	{

		if (elapsedTime == maxTime)
		{
			// We're done
			var8 = finalHeight;
			leftOffset = 0;
			yBottom = 4;
		}
		else
		{
			// These casts to long don't actually exist in disasm, but are required for 
			// testing large values of maxtime
			var8 = 256 - (((long)(256 - finalHeight) * (long)elapsedTime) / (long)maxTime);

			leftOffset = (startingLeftOffset * (long)(maxTime-elapsedTime))/ (long)maxTime;

			yBottom = (long)(elapsedTime*4)/(long)maxTime;
		}



		// Generate a table of 2500 multiples of var8 * 256
asm		xor    ax, ax;
asm		xor    dx, dx;
asm		mov    cx, 2500;
asm		mov    bx, var8;
asm		mov    es, introbuffer;
asm		xor    di, di;

loop1:

asm		mov    es:[di], ah;
asm		inc    di;
asm		mov    es:[di], dl;
asm		inc    di;
asm		add    ax, bx;
asm		adc    dx, 0;
asm		loop   loop1;

		if (elapsedTime == maxTime)
			leftOffset = 0;
		else
			leftOffset = ((long)(maxTime-elapsedTime) * startingLeftOffset) / (long)maxTime;


		var18 = *((word far *)introbuffer + 200);
		varC =0 ;
		varE = 0x10000L / var8;
		bufferofs = terminatorOfs + word_499C7;

		if (yBottom > 0)
			VW_Bar(0,0,320,yBottom, 0);


		EGAWRITEMODE(0);
		EGAMAPMASK(0xF);

		// Draw each line to the screen
		for (si = 0; si < var18; si++)
		{
			var16 = MK_FP(introbuffer2, ((introbmptype far*)introbuffer2)->linestarts[varC>>8]);
			ZoomOutTerminator_1(var16, si+yBottom, leftOffset, introbuffer);
			varC += varE;
		}

		bufferofs = terminatorOfs + word_499C7;
		var1C = var18 + yBottom;
		if (var20[terminatorPageOn] > var1C)
		{
			VW_Bar(0, var1C, 320,var20[terminatorPageOn],0);
			var20[terminatorPageOn] = var1C;
		}

		// Flip the page
		VW_SetScreen(terminatorOfs + word_499C7, 0);
		if (terminatorPageOn ^= 1)
			terminatorOfs = TERMINATORSCREENWIDTH/2;
		else
			terminatorOfs = 0;

		// loc_14B4F;
		newTime = TimeCount;
		tics = newTime-lasttimecount;
		lasttimecount = newTime;

		if (tics > 8)
			tics = 8;

		if (elapsedTime == maxTime)
			break;

		elapsedTime += tics;

		if (elapsedTime > maxTime)
			elapsedTime = maxTime;


		if (IN_IsUserInput() && LastScan != sc_F1)	// K1n9_Duk3 fix: this is "!= sc_F1" in Keen 4-6
			LastScan = sc_Space;

		if (LastScan)
			// return;// should this be break instead? Want to free intro buffers!
			break; 

	}

	MM_FreePtr(&introbuffer);
	MM_FreePtr(&introbuffer2);
}



// The Fizzlefade routine
// Operates by drawing the title graphic into offscreen video memory, then
// using the hardware to copy the source to the display area, pixel-by-pixel

// The same effect can be achieved in omnispeak by drawing the title screen
// to an new surface, then copying from the surface to the screen

// K1n9_Duk3 mod: FizzeDown used to be part of the FinishPage routine, but I
// moved it into a separate routine so that it can be re-used for other parts
// of the game. 
boolean FizzleDown(int speedmask)
{
#define FIZZLE_WIDTH  (320)
#define FIZZLE_HEIGHT (200)
#define FIZZLE_SPEED  (2)
	int i;
	//byte bitmasks[] = {1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80};	// this mapping is incorrect and causes issues with panning!
	static byte bitmasks[] = {0x80, 0x40, 0x20, 0x10, 8, 4, 2, 1};	// this mapping is correct!
	int columns1[FIZZLE_WIDTH], rows1[FIZZLE_HEIGHT];
	
	int pageDifference;
	word offs, n, row1;


	// Generate a random array from 0 to 320

	// Make array
	for (i = 0; i < FIZZLE_WIDTH; i++) 
		columns1[i] = i;

	// Randomly switch pairs of integers
	for (i =0; i< FIZZLE_WIDTH; i++)
	{
		int indexToSwitch = random(FIZZLE_WIDTH);  
		int valueToSwitch = columns1[indexToSwitch];
		columns1[indexToSwitch] = columns1[i];
		columns1[i] = valueToSwitch;
	}

	for (i=0; i < FIZZLE_HEIGHT; i++)
		rows1[i] = columns1[i];

	// Copy the pixels
	pageDifference = displayofs - bufferofs;

	asm mov es, screenseg;

	for (i = 0; i < (FIZZLE_HEIGHT + FIZZLE_WIDTH/FIZZLE_SPEED); i++)
	{
		int y, maxY;
		int minY = i - FIZZLE_WIDTH/FIZZLE_SPEED;
		if (minY < 0)
			minY = 0;

		maxY = i;
		if (maxY >= FIZZLE_HEIGHT)
			maxY = FIZZLE_HEIGHT-1;

		for (y = minY; y <= maxY; y++)
		{
			offs = bufferofs + ylookup[y+pany];	// K1n9_Duk3 mod: added pany (should be 0 during Terminator intro anyway)
			for (n = 0; n < FIZZLE_SPEED; n++)
			{
				int x = columns1[rows1[y]]+panx;	// K1n9_Duk3 mod: added panx (should be 0 during Terminator intro anyway)
				if (++rows1[y] == FIZZLE_WIDTH)
					rows1[y] = 0;

				//loc_14D96
asm				mov     cx, x;
asm				mov     si, cx;
asm				and     si, 7;
asm				cli;
asm				mov     dx, GC_INDEX;
asm				mov     al, GC_BITMASK;
asm				mov     ah, byte ptr bitmasks[si];
asm				out     dx, ax;
asm				sti;

asm				mov     si, offs;
asm				shr     cx, 1;
asm				shr     cx, 1;
asm				shr     cx, 1;
asm				add     si, cx;
asm				mov     di, si;
asm				add     di, pageDifference;

asm				mov     dx, SC_INDEX;
asm				mov     ax, (1 * 256) + SC_MAPMASK;
asm				out     dx, ax          
asm				mov     dx, GC_INDEX;
asm				mov     ax, (0 * 256) + GC_READMAP;
asm				out     dx, ax        
asm				mov     bl, es:[si];
asm				xchg    bl, es:[di];


asm				mov     dx, SC_INDEX;
asm				mov     ax, (2 * 256) + SC_MAPMASK;
asm				out     dx, ax;
asm				mov     dx, GC_INDEX;
asm				mov     ax, (1 * 256) + GC_READMAP;
asm				out     dx, ax;         
asm				mov     bl, es:[si];
asm				xchg    bl, es:[di];

asm				mov     dx, SC_INDEX;
asm				mov     ax, (4 * 256) + SC_MAPMASK;
asm				out     dx, ax;          
asm				mov     dx, GC_INDEX;
asm				mov     ax, (2 * 256) + GC_READMAP;
asm				out     dx, ax;
asm				mov     bl, es:[si];
asm				xchg    bl, es:[di];

asm				mov     dx, SC_INDEX;
asm				mov     ax, (8 * 256) + SC_MAPMASK;
asm				out     dx, ax; 
asm				mov     dx, GC_INDEX;
asm				mov     ax, (3 * 256) + GC_READMAP; 
asm				out     dx, ax;
asm				mov     bl, es:[si];
asm				xchg    bl, es:[di];


			}

		}

		if (!(i & speedmask))
			VW_WaitVBL(1);

		if (IN_IsUserInput())
			if (LastScan != sc_F1)	// K1n9_Duk3 fix: this is "!= sc_F1" in Keen 4-6
				LastScan = sc_Space;

		if (LastScan)
		{
			EGABITMASK(0xFF);
			EGAMAPMASK(0x0F);
			return true;
		}

	}
	
	EGABITMASK(0xFF);
	EGAMAPMASK(0x0F);
	return false;
}


void FinishPage(void)
{
// K1n9_Duk3 fix:
// The "COMMANDER KEEN" text would normally appear in white after switching back
// to the default palette. This code makes sure the text stays dark cyan after
// changing the palette by clearing the red and intensity planes of the EGA
// memory so that only the blue and green planes store the text.
//
// If you want the text to appear in a different color, change the parameter in
// the first EGAMAPMASK call to the desired color mask as follows:
// 0x0: white, 0x1: yellow, 0x2: light magenta, 0x3: light red, 0x4: light cyan,
// 0x5: light green, 0x6: light blue, 0x7: dark grey, 0x8: light grey,
// 0x9: brown, 0xA: dark magenta, 0xB: dark red, 0xC: dark cyan,
// 0xD: dark green, 0xE: dark blue, 0xF: black (would be useless)
//
// These numbers are valid for the default EGA palette. If you're using a
// different palette, you should be smart enough to figure out on your own
// which value you need to use.

//	EGAWRITEMODE(1);
	EGAMAPMASK(0xD);	//make it dark green
asm	mov ax, 0A000h;
asm	mov es, ax;
asm	xor di, di;
asm	xor ax, ax;
asm	mov cx, 8000h
asm	rep stosw;
//	EGAWRITEMODE(0);	//this was the previous write mode
	EGAMAPMASK(0xF);	//write to all planes


	VW_SetDefaultColors();

	if (terminatorPageOn)
	{
		bufferofs = word_499C7 + TERMINATORSCREENWIDTH/2;
		displayofs = word_499C7;
	}
	else
	{
		bufferofs = word_499C7;
		displayofs = bufferofs + TERMINATORSCREENWIDTH/2;
	}
	

	// Set screen to the display buffer
	VW_SetScreen(displayofs, 0);
	
	// Draw the title screen pic (to the back buffer)
	VW_DrawPic(0,0, TITLEPIC);

	// 
	if (!FizzleDown(0))
		IN_UserInput(15*TickBase, false);	// Nisaba: show title screen for 15 instead of 12 seconds
}

void CK_DrawTerminator(void)
{
	unsigned i,j;
	unsigned cmdrWidthX100;

	word far *linestart; 
	byte far *vidPtr; // perhaps srcPtr, destPtr, respectively


	//word unkptrs[200];	// K1n9_Duk3 mod: don't need this, it just wastes stack space


	boolean terminator_complete = false;
	CA_SetAllPurge();

	// Prepare screen with black palette
	colors[0][16] = bordercolor;
	_ES=FP_SEG(&colors[0]);
	_DX=FP_OFF(&colors[0]);
	_AX=0x1002;
	geninterrupt(0x10);
	CK_DefaultPalette();

	VW_ClearVideo(0);
	RF_FixOfs();
	VW_SetLineWidth(TERMINATORSCREENWIDTH);


	// Cache Intro Bitmaps
	CA_CacheGrChunk(TITLEPIC);
	CA_CacheGrChunk(I_COMMANDEREXTERN);
	CA_CacheGrChunk(I_KEENEXTERN);


	//ck_introKeen = grsegs[I_KEENEXTERN];
	//ck_introCommander = grsegs[I_COMMANDEREXTERN];

	EGAMAPMASK(1);


	// Because "KEEN" needs to end up on the right side of the screen,
	// an extra padding amount is added to the left side of the Keen graphic
	ck_introScreenWidth = ck_introKeen->width + 25 * 8;

	// Set the screen to the right of the "KEEN" graphic
	VW_SetScreen(ck_introScreenWidth/8 + 1, 0);

	// Copy each line of the KEEN graphic into video memory, accounting for the padding amount
	for (i = 0; i < 200; i++)
	{
		linestart = MK_FP(ck_introKeen,  ck_introKeen->linestarts[i]); 
		vidPtr = MK_FP(0xA000 , ylookup[i] + 25);

		TerminatorExpandRLE(linestart, vidPtr);
	}

	// Copy the KEEN graphic to the second page
	VW_ScreenToScreen(0,TERMINATORSCREENWIDTH/2,109,200);

	ck_introCommanderWidth = (ck_introCommander->width+7)>>3;
	ck_introCommanderWidth = (ck_introCommanderWidth+3)&0xFFFE;
	cmdrWidthX100 = ck_introCommanderWidth * 100;


	for (j = 0; j < 8; j++)
		MM_GetPtr(&shiftedCmdrBMPsegs[j],cmdrWidthX100);


	//ck_introKeen = grsegs[I_KEENEXTERN];
	//ck_introCommander = grsegs[I_COMMANDEREXTERN];


	for (i = 0; i < 100; i++)
	{

		//unkptrs[2*i] = unkptrs[2*i+1] = i * ck_introCommanderWidth;	// K1n9_Duk3 mod: don't use the array

		linestart = MK_FP(ck_introCommander, ck_introCommander->linestarts[2*i]);
		//vidPtr = MK_FP(shiftedCmdrBMPsegs[0],unkptrs[2*i]);	// K1n9_Duk3 mod: don't use the array
		vidPtr = MK_FP(shiftedCmdrBMPsegs[0],i * ck_introCommanderWidth);	// K1n9_Duk3 mod: don't use the array

		TerminatorExpandRLE(linestart, vidPtr);

	}


	// loc_150A5
	// Looks like we're making 8 shifts of the "Commander" graphic
	for (j = 1; j < 8; j++)
	{
		memptr var10 = shiftedCmdrBMPsegs[j-1];
		memptr var12 = shiftedCmdrBMPsegs[j];

asm		mov ds, var10;
asm		mov es, var12;
asm		mov cx, cmdrWidthX100;
asm		clc;
asm		xor si, si;
asm		xor di, di;

copyloop:
asm		lodsb;
asm		rcr al, 1;
asm		stosb;
asm		loop copyloop;

asm		mov ax, ss;
asm		mov ds, ax;
	}


	// Set the terminator palette
	terminator_palette2[16] = bordercolor;
	terminator_palette1[16] = bordercolor;

	// and the border.
	_ES = FP_SEG(terminator_palette2);
	_DX = FP_OFF(terminator_palette2);
	_AX = 0x1002;
	geninterrupt(0x10);

	for (i = 0; i < 5; i++)
		DoubleSizeTerminatorPics(i);


	// Do the terminator
	ck_currentTermPicStartTime = ck_currentTerminatorCredit = 0;
	ck_termCreditStage = -1;
	AnimateTerminator();


	for (i = 0; i < 5; i++)
		MM_FreePtr(&terminator_pic_memptrs[i]);

	for (j = 0; j < 8; j++)
		MM_FreePtr(&shiftedCmdrBMPsegs[j]);


	// Do the Zooming-out transition to the fizzlefade
	if (!LastScan)
	{
		ZoomOutTerminator();
	}

	// Do the fizzlefade
	if (!LastScan)
	{
		FinishPage();
		terminator_complete = true;
	}

	// Free the COMMANDER and KEEN bitmaps
	//MM_SetPurge(&(memptr)grsegs[I_COMMANDEREXTERN], 3);
	//MM_SetPurge(&(memptr)grsegs[I_KEENEXTERN], 3);

	// Restore video mode to normal
	VW_ClearVideo(0);
	VW_SetLineWidth(SCREENWIDTH);
	VW_SetDefaultColors();
	RF_FixOfs();
	CA_ClearMarks();

	// Handle any keypresses
	if (!LastScan)
		return;

	// Go to help screen
	if (LastScan == sc_F1)
	{
		HelpScreens();
		return;
	}

	if (!terminator_complete)
	{
		RF_FixOfs();

		// Draw Title Screen to back buffer and set screen there
		CA_CacheGrChunk(TITLEPIC);
		VW_DrawPic(0, 0, TITLEPIC);
		VW_UpdateScreen();
		VW_SetScreen(bufferofs, 0);

		// Wait for input
		IN_Ack();//IN_WaitButton();

		// Stop music if it hasn't already stopped
		StopMusic();

		CA_ClearMarks();

		// TODO: If started with /DEMO PARM
#if 0
		if (DemoSwitch)
		{
			ck_levelState = LS_StartingNew;	// K1n9_Duk3 fix: use enum name instead of number
			restartgame = D_Normal;
			IN_ClearKeysDown();
			CK_NewGame();
			return;
		}
#endif

	}

	CK_MainMenu();
}

//===========================================================================
//
// STAR WARS
// 
//===========================================================================

// Dynamically assembles 200 scaler functions, one for each row of the screen, that 
// stretch or compress an input row of pixels by a fixed amount and write these to the screen
//
// The dynamic assembly doesn't seem to be necessary, as the message data doesn't 
// actually affect the assembly process.  

#define	EGAPORTSCREENWIDE	42

// Table that adds a row-displacement when selecting a line of the master text image to draw, 
// given a row on visible screen as the index
// Because the table values are skewed, this causes the text to bunch up at the top of the screen.
word ck_SWScreenRowToMasterRowTrans[200];


// The total height, in rows, of the entire star wars message (before graphic transformations)
unsigned ck_starWarsTotalHeight;

// The table of byte values 
memptr ck_starWarsTable;

// The buffer that holds all of the dynamically assembled instructions
memptr ck_SWDrawFuncSeg;

// The entire, unscaled, starwars text message is drawn to screen and then copied to this seg
memptr ck_starWarsMasterText;



void (* ck_SWCurrentDrawFunc)(void);
void (* ck_SWDrawFuncs[200])(void);


// Generates a table of byte values
// This table somehow translates master text data into maskbytes for drawing pixels on screen
void starwars2(void)
{
	byte far *var6;
	word di, var2, si;


	MM_GetPtr((memptr *)&ck_starWarsTable, 0x4000);
	var6 = MK_FP(ck_starWarsTable, 0);

	for (di = 1; di < 0x100; di <<= 1)
		for (var2 = 1; var2 < 0x100; var2 <<=1)
			for (si = 0; si < 0x100; si++)
				*var6++ = (si & di)? (byte)var2 : 0;

}

void BuildScalers(void)
{
	
	longword varA, var10, swDrawFuncSegSize, var14;
	word var8, si, var2A, var28, var24;
	byte var1D, var1E;

	// The width of the text trapezoid at the bottom of the screen
	// The trapezoid width is decreased by delTrapWidth for each row
	// Units are in 1 / (2 ^ 11) 'ths of a pixel
	long trapezoidWidth;
	int delTrapWidth;

	// This points to the location where machine code is written
	// In the disassembly, this is a register variable stored in ES:DI for
	// the duration of the loop
	word register far *esdi;

	// esdi and drawFuncStart might just be one variable in the original source
	// as drawFuncStart doesn't seem to do anything
	word far *drawFuncStart;

	// the width of the text at this row on screen
	unsigned rowwidth;

	// the screen x coord where the starwars text is drawn
	unsigned rowleftmargin;

	// the offset in the video buffer where this row starts
	unsigned rowofs;

	// the offset, mod 8, of the row left margin
	byte rowmask;

	// loop variable
	int row;



	// Generate the table
	starwars2();

	// Allocate a gigantic buffer for the dynamically assembled instructions
	swDrawFuncSegSize = 190000L;
	MM_GetPtr((memptr *)&ck_SWDrawFuncSeg, swDrawFuncSegSize);
	// Nisaba patch:
	MM_SetLock((memptr *)&ck_SWDrawFuncSeg, lk_full);

	// Set the pointer that writes the assembled instructions to the 
	// start of the previously allocated buffer
	drawFuncStart = MK_FP(ck_SWDrawFuncSeg, 0);
	esdi = drawFuncStart;

	// Set the dimensions of the text trapezoid
	trapezoidWidth = 0xA0000L; 
	delTrapWidth = 0xB33;

	var10 = 0;
	
	// Generate a dynamically-assembled function for each screen line
	for (row = 199; row >= 0; row--)
	{
		// Draw a strip of grey directly into the screen area
		// (because we've set bufferofs to 0, and HLin draws to bufferofs+ylookup)
		VW_Hlin(0,320,row,1);

		// Save the function address for this line
		drawFuncStart = esdi;
		ck_SWDrawFuncs[row] = (void (*)(void))drawFuncStart;

		// make the row width even
		rowwidth = (trapezoidWidth/2 >> 0xB) << 1;

		// The text scrunching is negatively proportional to the width of the row
		// Smaller row width = more scrunched text
		ck_SWScreenRowToMasterRowTrans[row] = var10 >> 0xB;

		var8 = 0xA8000L/rowwidth;

		var14 = 0;
		var10 += var8;

		// 160 is the center of the screen
		rowleftmargin = 160 - rowwidth/2;
		rowofs = ylookup[row] + rowleftmargin/8;
		rowmask = rowleftmargin & 7;

		// machine code!
		// Remember that the low byte comes first

		*esdi++ = 0xD18C;		// mov cx, ss
		*esdi++ = 0xD08E;		// mov ss, ax
		*esdi++ = 0xC781;		// add di, rowofs
		*esdi++ = rowofs;		// 
		*esdi++ = 0xC030;		// xor al, al

		var2A = 0xFFFF;

		// for each byte that is drawn to the screen
		for (si = 0; si < rowwidth; si ++)
		{

			var24 = var14 >> 0xB;
			var1D = var24 / 8;			// some xbyte
			var1E = var24 & 7;			// and the corresponding mask

			var28 = ((7 - var1E) * 8 + 7 - rowmask) << 8;

			if (var1D != var2A)
			{
				// remember that SS = AX, and [bp+dist] uses SS: as segment
				// When called, [SS:BP] -> MasterTextSeg:masterofs 
				*esdi++ = 0x5E8A;		// mov bl, [bp + var1D]
				*((byte far *)esdi)++ = var1D;  
				var2A = var1D;
			}

				// Assume BH = 0;
			*esdi++ = 0x870A;			// or al, [bx + var28] 
			*esdi++ = var28;
			if (++rowmask == 8)
			{
				rowmask = 0;
				*esdi++ = 0x30AA;			// stosb
				*((byte far *)esdi)++ = 0xC0;		// xor al, al
			}

			var14 += var8;
		}

		if (rowmask)
			*((byte far *)esdi)++ = 0xAA;			// stosb

		*esdi++ = 0xD18E;					// mov ss, cx
		*((byte far *)esdi)++ = 0xCB;				// retf;


		// Normalize esdi
		esdi = MK_FP(FP_SEG(esdi) + (FP_OFF(esdi) >> 4), FP_OFF(esdi) & 0xF);

		// Shrink the scale distance for the next line
		trapezoidWidth -= delTrapWidth;

		// The backdrop picture was drawn to the backbuffer (at 0x8000) in CK_DrawStarWars
		// Copy one row of the backdrop, so the image appears to fill the screen from the bottom
		//  as we prepare the star wars effect
		VW_ScreenToScreen(0x8000 + ylookup[row], ylookup[row], 40, 1); // copy 320 x 1 pixels

		if (LastScan)
			return;
	}
}


#define STARWARS_END '^'	// This must mark the end of the text file

// Draws the Star Wars text
void PrintStarWars(void)
{
	unsigned size;
	char far *storytext;
	char textbuffer[81];
	char c;

	WindowX = 0;
	WindowW = EGAPORTSCREENWIDE * 8; //336;
	PrintY = 1;		// Save a 1 pixel high blank row at the start of the master text buffer
	bufferofs = 0;
	panadjust = 0;

#if 0
	// for the final release:
	CA_CacheGrChunk(T_STARWARS);
#else
#ifdef DEVBUILD
	// for the development version:
	if (grsegs[T_STARWARS])
		MM_FreePtr(&grsegs[T_STARWARS]);
	if (!CA_LoadFile("starwars.txt", &grsegs[T_STARWARS]))
		CA_CacheGrChunk(T_STARWARS);
#else
	// for the final release:
	CA_CacheGrChunk(T_STARWARS);
#endif	// DEVBUILD
	
#endif
	size = MM_GetSize(&grsegs[T_STARWARS]);
	storytext = grsegs[T_STARWARS];
	ck_starWarsTotalHeight = 0;

	// Draw Each Line using the User manager print routines
	c = *storytext;
	while (c != STARWARS_END)
	{
		char *si = textbuffer;

		do {
			if (FP_OFF(storytext) >= size)
				SM_Quit(S_BADSTARWARSEND);

			c = *storytext++;
			if (c == STARWARS_END)
			{
				break;
			}
			else if (c != '\r')
			{
				*si++ = c;
			}
			if (si == textbuffer+sizeof(textbuffer))
				SM_Quit(S_BADSTARWARSLINE);

		} while (c != '\n');


		*si = 0;

		US_CPrint(textbuffer);

		// Move the back buffer down by one line of text
		bufferofs += ylookup[PrintY];
		ck_starWarsTotalHeight += PrintY;
		PrintY = 0;

	}

	MM_FreePtr(&grsegs[T_STARWARS]);

	// Get a buffer big enough to hold the graphics data
	// And move into it all the text that has just been drawn
	MM_GetPtr((memptr *)&ck_starWarsMasterText, bufferofs);
	EGAREADMAP(1);
	movedata(screenseg, 0, FP_SEG(ck_starWarsMasterText), 0, bufferofs);

	// reset the back buffer pointer and erase all the text that was drawn to the visible area
	bufferofs = 0;
	VW_Bar(0,0, 320,200, 0);

}

void StarWarsLoop(void)
{
	unsigned scrollDist, newTime, delaytime;
	int masterRowToDraw, row;

	TimeCount = lasttimecount = 0L;
	tics = 0;
	
	EGAWRITEMODE(0);
	EGAMAPMASK(8);

	scrollDist = 0;

	// The scroll will move the entire height of the starwars text + 400 pixels
	while (scrollDist < ck_starWarsTotalHeight + 400)
	{
		// Updating rows from the bottom

		for (row = 199; row >= 0; row--)
		{
			masterRowToDraw = scrollDist - ck_SWScreenRowToMasterRowTrans[row];
			if (masterRowToDraw >= 0 && masterRowToDraw < ck_starWarsTotalHeight)
				// We have more text left to draw
				// Pick the source row from the master text buffer
				masterofs = masterRowToDraw * EGAPORTSCREENWIDE;
			else
				// The text hasn't scrolled this far, or it's scrolled beyond this screen line
				// So draw that one pixel-high blank row that we saved earlier!
				masterofs = 0;

		ck_SWCurrentDrawFunc = ck_SWDrawFuncs[row];

		asm	mov	es, [screenseg];
		asm	mov	di, [terminatorOfs];
		asm	mov	ds, [ck_starWarsTable];
		asm	push	bp;
		asm	mov	bp, ss:masterofs;
		asm	mov	ax, ss:ck_starWarsMasterText;
		asm	xor	bh,bh;

		// For reference, here is the entire function that is called
		// Pseudo-conditional-assembly directives are used

		// mov cx, ss			; save stack
		// mov ss, ax			; SS:[BP] -> start of source row (MasterTextSeg:masterofs)
		// add di, rowofs		; ES:[DI] -> start of drawing area
		// xor al, al


		// FOR (si = 0; si <= rowwidth; si++) ; for each byte drawn to the screen
		// IF (var1D != var2A)
		// mov bl, [bp + var1D]
		// ENDIF

		// or al, [bx + var28] 		; DS:[BX] -> TableSeg:

		// IF (++rowmask == 8)
		// stosb
		// xor al, al
		// ENDIF
		// ENDFOR


		// IF (rowmask)
		// stosb
		// ENDIF


		// mov ss, cx				; restore stack
		// retf;				

		asm	cli;
		asm	call	ss:ck_SWCurrentDrawFunc;
		asm	sti;
		

		asm	pop	bp;
		asm	mov	ax, ss;			// restore stack
		asm	mov	ds, ax;

		}


	// Flip pages
	VW_SetScreen(terminatorOfs, 0);
	terminatorPageOn ^= 1;
	terminatorOfs = terminatorPageOn << 0xF; 	// 2nd page at 0x8000


	tics += TimeCount - lasttimecount;
	lasttimecount = TimeCount;

	if (tics > 20)
		tics = 20;

	// scrollDist += 1;
       	scrollDist += tics/4;

	tics &= 3;

	if (IN_IsUserInput() && LastScan != sc_F1)	// K1n9_Duk3 fix: this is "!= sc_F1" in Keen 4-6
		LastScan = sc_Space;

	if (LastScan)
		return;

	}


}

char ck_starWarsPalette[] = {
	0x00, 0x01, 0x03, 0x02, 0x06, 0x04, 0x07, 0x18,
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00};

static byte far starwarspalette[] = {
	0x00, 0x00, 0x00, 0x00,	// black        #000000
	0x01, 0x04, 0x04, 0x19,	// dark blue    #0000A8
	0x02, 0x04, 0x19, 0x04,	// dark green   #00A800
	0x03, 0x04, 0x19, 0x19,	// dark cyan    #00A8A8
	0x04, 0x19, 0x04, 0x04,	// dark red     #A80000
	0x05, 0x2A, 0x00, 0x2A,	// dark magenta #A800A8
	0x06, 0x19, 0x0E, 0x04,	// dark orange  #A85400
	0x07, 0x1D, 0x1D, 0x1D,	// grey	        #A8A8A8
	0x18, 0x0E, 0x0E, 0x0E,	// dark grey    #545454
	0x19, 0x15, 0x15, 0x3F,	// light blue   #5454FC
	0x1A, 0x15, 0x3F, 0x15,	// light green  #54FC54
	0x1B, 0x15, 0x3F, 0x3F,	// light cyan   #54FCFC
	0x1C, 0x3F, 0x15, 0x15,	// light red    #FC5454
	0x1D, 0x3F, 0x15, 0x3F,	// magenta      #FC54FC
	0x1E, 0x3F, 0x3F, 0x15,	// yellow       #FCFC54
	0x1F, 0x3F, 0x3F, 0x3F,	// white        #FCFCFC
};

void CK_DrawStarWars(void)
{

	// Set palette to black
	colors[0][16] = bordercolor;
	_ES=FP_SEG(&colors[0]);
	_DX=FP_OFF(&colors[0]);
	_AX=0x1002;
	geninterrupt(0x10);
	CK_SetPaletteVGA(starwarspalette);

	// Clear out video memory and set the visible screen offest to 0
	// so that we have a clean area to draw the master text to 
	VW_ClearVideo(0);
	VW_SetLineWidth(EGAPORTSCREENWIDE);
	VW_SetScreen(0,0);
	terminatorOfs = terminatorPageOn = 0;

	CA_SetAllPurge();

	// Draw the entire text message to the master buffer
	CA_CacheGrChunk(STARTFONT+2);
	fontnumber = 2;
	PrintStarWars();
	fontnumber = 0;

	// Cache the bg image and draw it to the back buffer
	CA_CacheGrChunk(STARWARSPIC);
	bufferofs = 0x8000;
	VW_DrawPic(0,0,STARWARSPIC);
	CA_SetAllPurge();

	// Set the starwars palette
	ck_starWarsPalette[16] = bordercolor;
	_ES=FP_SEG(&ck_starWarsPalette);
	_DX=FP_OFF(&ck_starWarsPalette);
	_AX=0x1002;
	geninterrupt(0x10);

	// Generate the compiled scalers
	bufferofs = 0;
	BuildScalers();

	// Do the star wars effect
	if (!LastScan)
	{
		StartMusic(22, true);
		StarWarsLoop();
		StopMusic();
	}

	// Free everything and restore normal video settings
	MM_FreePtr((memptr *)&ck_SWDrawFuncSeg);
	MM_FreePtr((memptr *)&ck_starWarsTable);
	MM_FreePtr((memptr *)&ck_starWarsMasterText);

	VW_ClearVideo(0);

	VW_SetLineWidth(SCREENWIDTH);
	VW_SetDefaultColors();
	CK_DefaultPalette();
	RF_FixOfs();
	CA_ClearMarks();

	//CK_HandleDemoKeys();

}

//===========================================================================

void CK_ShowTitleScreen()
{
	panadjust = 0;
	CA_CacheGrChunk(TITLEPIC);

	CK_DefaultPalette();

	// Draw title pic to back buffer
	VW_DrawPic(0,0,TITLEPIC);
	VW_SetScreen(displayofs, 0);

	// Copy the image from the back buffer to the visible buffer
	VW_ScreenToScreen(bufferofs, displayofs, 42, 224);

	// Wait for a bit
	IN_UserInput(420, false);
	CA_ClearMarks();
	CK_HandleDemoKeys();
}

//===========================================================================
//
// DEMO PLAYBACK
// 
//===========================================================================

void CK_OverlayHighScores(void);

typedef struct {
	unsigned level;
	unsigned size;
	byte data[];
} demotype;


void CK_PlayDemo(int demoNumber)
{
	demotype far *demo;
	int demoChunk;

	CK_NewGame();

	demoChunk = STARTDEMOS + demoNumber;

	CA_CacheGrChunk(demoChunk);
	demo = grsegs[demoChunk];
	ck_gameState.currentLevel = demo->level;
	DemoSize = demo->size; 

	MM_GetPtr((memptr *)&DemoBuffer, DemoSize);
	MM_SetLock(&grsegs[demoChunk], true);
	_fmemcpy(DemoBuffer, &(demo->data), DemoSize);
	MM_FreePtr(&grsegs[demoChunk]);

	IN_StartDemoPlayback(DemoBuffer, DemoSize);

	CK_LoadLevel(true);

	// K1n9_Duk3 fix: don't try to play the demo if we ran out of memory
	if (mmerror)
	{
		mmerror = false;
	}
	else
	{
	
		if (ck_inHighScores)
			CK_OverlayHighScores();
	
		CK_PlayLoop();
	}

	IN_StopDemo();

	// We have to get rid of the demo buffer, as if we want to play it
	// again, we need a fresh copy. ID_IN modifies the buffer.
	MM_FreePtr((memptr *)&DemoBuffer);
	VW_FixRefreshBuffer();
	CA_ClearMarks();

	// What should we do after playing the demo?
	if (!ck_inHighScores)
		CK_HandleDemoKeys();

}

// K1n9_Duk3 fix: use the high scores from ID_US.C to make sure they are saved
// in the config file:
//CK_HighScore ck_highScores[8] =
extern HighScore Scores[MaxScores] =
{
	// 0x01 = Yorp
	// 0x02 = Bear
	// 0x04 = Keen
	// 0x08 = Vort
	// 0x10 = Garg
	// 0x20 = Mort
	// 0x3F = All6
	// 0x7F = All6+Donut
	{"Id Software '91", 10000, 0x04},
	//{"Perfect Score", 1000000, 0x7F, 0x01},
	{"", 10000, 0},
	{"Nisaba        '23",	10000, 0x10},
	{"K1n9_Duk3   '23", 	10000, 0x02},
	{"", 0, 0},
	{"", 0, 0},
	{"pckf.com", 10000, 0},
	{"", 0, 0},
};
#define ck_highScores Scores

#define HIGHSCORE_X 40
#define HIGHSCORE_Y 35
#define HIGHSCORE_COLOR 15

void CK_DrawHighscores(void)
{
	void ClipSprite(int x, int y, int chunknum, unsigned skiptop, unsigned h, unsigned drawtype);
	int entry;

	for (entry = 0; entry < MaxScores; entry++)
	{
		char buf[0x10], *c;
		unsigned w, h;

		// Print the name
		PrintY = entry*16+HIGHSCORE_Y;
		PrintX = HIGHSCORE_X;
		US_Print(ck_highScores[entry].name);
		// Draw the icons for the collectibles
		w = ck_highScores[entry].completed;	// collectible bits
		h = 0;	// collectible index
		while (w)
		{
			if (w & 1)
			{
				// every collectible has its own fixed position now: 
				static far int xoffs[7] = {0, 9, 17, 26, 35, 45, 61}; // Yorp, Bear, Keen, Vort, Garg, Mort, Donut

				ClipSprite(HIGHSCORE_X+103+xoffs[h], entry*16+HIGHSCORE_Y, COLLECTIBLES_LUMP_START+h*2, 0, 10, spritedraw);
			}
			h++;
			w >>= 1;
		}		
		if (ck_highScores[entry].bonus)
			ClipSprite(HIGHSCORE_X+150, entry*16+HIGHSCORE_Y, SLUGCANS2SPR, 0, 10, spritedraw);
		
		// Print the score, right aligned in the second
		// column of the table
		// sprintf(buf, "%d", ck_highScores[entry].score);
		ultoa(ck_highScores[entry].score, buf, 10);

		// Convert it to high score numbers?
		for (c = buf; *c; c++)
		{
			*c += 0x51;
		}
		
		// Right Align it
		VW_MeasurePropString(buf, &w, &h);
		PrintX = (285-w);
		US_Print(buf);
				
		if (ck_highScores[entry].difficulty)
			{				
				if (ck_highScores[entry].difficulty == D_Easy)
					ClipSprite(HIGHSCORE_X+181, entry*16+HIGHSCORE_Y+4, EGASPR, 0, 4, spritedraw);
		
				else if (ck_highScores[entry].difficulty == D_Normal)
					ClipSprite(HIGHSCORE_X+181, entry*16+HIGHSCORE_Y+4, EGASPR, 5, 4, spritedraw);
		
				else if (ck_highScores[entry].difficulty == D_Hard)
					ClipSprite(HIGHSCORE_X+181, entry*16+HIGHSCORE_Y+4, EGASPR, 10, 4, spritedraw);
			}
	}
}

// Draw the high scores overtop the level
void CK_OverlayHighScores(void)
{
	unsigned oldbuf;

	// Draws the map to the master screen and
	// sets the update matrix for both pages, forcing
	// text to be drawn
	RF_NewPosition (0,0);

	// Set the back buffer to start of the tilemap so that
	// the text drawing routines will draw over the tiles
	oldbuf = bufferofs;
	bufferofs = masterofs;

	fontcolor = HIGHSCORE_COLOR;

	CK_DrawHighscores();

	fontcolor = (15);
	bufferofs = oldbuf;
}

// Enter name if a high score has been achieved
extern boolean HighScoresDirty;

void CK_SubmitHighScore(long score, unsigned arg_4, int ny00, int arg_5)
{
	int entry, entryRank, var4;
	HighScore newHighScore;	// K1n9_Duk3 fix: use struct from ID_US.H

//	newHighScore.name[0] = 0;	// K1n9_Duk3 mod: smaller and faster version of 'strcpy(newHighScore.name, "");'
	strcpy(newHighScore.name, "");
	newHighScore.score = score;
//	newHighScore.completed = arg_4 & 0x7F;	// K1n9_Duk3 fix: limit to 7 bits (xoffs array in CK_DrawHighscores() has only 7 entries)
	newHighScore.completed = arg_4;	// K1n9_Duk3 fix: using name from ID_US.H struct
	newHighScore.bonus = ny00;		// Nisaba patch
	newHighScore.difficulty = arg_5; // Nisaba patch


	// Check if this entry made the high scores
	entryRank = -1;
	for (entry = 0; entry < MaxScores; entry++)
	{
		if (ck_highScores[entry].score > newHighScore.score)
			continue;

		if ((newHighScore.score == ck_highScores[entry].score) 
		&& (ck_highScores[entry].completed > newHighScore.completed))	// K1n9_Duk3 fix: using name from ID_US.H struct
			continue;
					
		if ((newHighScore.score == ck_highScores[entry].score) 
		&& (newHighScore.completed == ck_highScores[entry].completed) 
		&& (ck_highScores[entry].bonus >= newHighScore.bonus))			// Nisaba patch
			continue;
		
		// Made it in! 

		// K1n9_Duk3 fix: the lower ranks need to be moved down BEFORE overwriting anything!

		// Move all of the lower ranks down by one
		for (var4 = MaxScores; --var4 > entry; )
			memcpy(&ck_highScores[var4], &ck_highScores[var4-1], sizeof(newHighScore));

		// Overwrite the old high score
		memcpy(&ck_highScores[entry], &newHighScore, sizeof(newHighScore));
		entryRank = entry;
		HighScoresDirty = true;

		break;
	}


	if (entryRank != -1)
	{
		ck_inHighScores = true;
		ck_gameState.currentLevel = MAP_HIGHSCORES;
		CK_LoadLevel(true);
		CK_OverlayHighScores();
		fontcolor = (HIGHSCORE_COLOR);

		// Draw the High scores level to both pages 
		RF_Refresh();
		RF_Refresh();

		PrintY = entry*16 + HIGHSCORE_Y;
		PrintX = HIGHSCORE_X;
		
		US_LineInput(PrintX, PrintY, ck_highScores[entryRank].name, NULL, true, MaxHighName, 101);

		ck_inHighScores = false;
	}

	fontcolor = (15);
}

// Play the high score level
void CK_DoHighScores(void) 
{
	ck_inHighScores = true;
	IN_ClearKeysDown();
	CK_PlayDemo(1);
	ck_inHighScores = false;
}

#include "FOOBS.C"
