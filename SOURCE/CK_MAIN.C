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

// KD_MAIN.C
/*
=============================================================================

							KEEN DREAMS

					An Id Software production

=============================================================================
*/

#include "mem.h"
#include "string.h"

#include "CK_DEF.H"
#pragma hdrstop

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

char		str[80],str2[20];
boolean		tedlevel;
unsigned	tedlevelnum;
int		extravbls; // Imported from Catacomb 3-D

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

void	DebugMemory (void);
void	TestSprites(void);
int		DebugKeys (void);
void	ShutdownId (void);
void	Quit (char *error);
void	InitGame (void);
void	main (void);

//===========================================================================



//===========================================================================


/*
 * Measure the containing box size of a string that spans multiple lines
 */
void CK_MeasureMultiline(const char *str, unsigned *w, unsigned *h)
{
	char c;
	unsigned x, y;
	char buf[80];
	char *p;

	*h = *w = (unsigned) 0;
	p = buf;	/* must be a local buffer */

	while ( (c = *str++) != 0 )
	{
		*p++ = c;

		if ( c == '\n' || *str == 0 )
		{
			*p = 0;		// K1n9_Duk3 fix: make sure we have a terminating 0
			VW_MeasurePropString( buf, &x, &y);

			*h += y;
			if ( *w < x )
				*w = x;

			p = (char *) buf;
			// Shouldn't buf be cleared so that a newline is not read over by
			// VH_MeasurePropString?
			//
			// K1n9_Duk3 note: No, it doesn't matter, since newline characters
			// have a width of 0 and don't affect the height value returned by
			// VW_MeasurePropString.
		}
	}
}

/*
==========================
=
= ShutdownId
=
= Shuts down all ID_?? managers
=
==========================
*/

void ShutdownId (void)
{
	US_Shutdown ();
	SD_Shutdown ();
	IN_Shutdown ();
	RF_Shutdown ();
	VW_Shutdown ();
	CA_Shutdown ();
	MM_Shutdown ();
	SM_Shutdown ();	// K1n9_Duk3 addition
}

//===========================================================================

/*
==========================
=
= Quit
=
==========================
*/

void Quit (char *error)
{

	void _seg *textseg = 0;

	if (!error)
	{
		register int chunk = (ck_levelState == LS_BrainFucled)? ANSI_BADEND : ANSI_ENDTEXT;
		CA_SetAllPurge();
		CA_CacheGrChunk(chunk);
		textseg = grsegs[chunk];
	}

	if (screenseg)	// K1n9_Duk3 fix: screenseg is 0 if VW_Startup wasn't executed
	{
		VW_ClearVideo(0);	// this would be an EXTREMELY bad idea when screenseg is 0!
		VW_SetLineWidth(40);
	}

	ShutdownId ();
	if (error && *error)
	{
		clrscr();
		puts(error);
		puts("\n");
		exit(1);
	}

	// K1n9_Duk3 addition: bad ending tries to start Keen Dreams (if present):
	if (ck_levelState == LS_BrainFucled)
		execlp("kdreams.exe", "kdreams.exe", "detour");	// some versions require the "detour" parameter to actually start the game

	// Note:
	// If Keen Dreams is present, then starting it effectively quits
	// FITF and none of the following code will be executed. If it is
	// NOT present, we still get to show the text we wanted to show.

	if (textseg)
	{
		movedata((int)textseg, 0, 0xb800,0, 2*80*25);
		gotoxy(1, 24);
	}

	exit (0);
}

//===========================================================================

/*
==========================
=
= InitGame
=
= Load a few things right away
=
==========================
*/

#if 0
#include "piracy.h"
#endif

void InitGame (void)
{
	extern void		MML_UseSpace (unsigned segstart, unsigned seglength);

	SM_Startup ();	// K1n9_Duk3 addition
	// K1n9_Duk3 addition: help parameter
	if (SM_ParmPresent(S_HELP1) || SM_ParmPresent(S_HELP2) || SM_ParmPresent(S_HELP3))
	{
		clrscr();
		SM_PutLines(S_HELPTEXT1, S_HELPTEXTEND);
		SM_Shutdown();
		exit(0);
	}
	// end of help parameter check
	MM_Startup ();

#if 0
	// Handle piracy screen...
	//
	movedata(FP_SEG(PIRACY),(unsigned)PIRACY,0xb800,displayofs,4000);
	while ((bioskey(0)>>8) != sc_Return);
#endif


#if GRMODE == EGAGR
	if (MM_TotalFree() < 468000-BUFFERSIZE) //335l*1024	// TODO: use correct value in final game
/*
	{
		int i;

		clrscr();
		//puts ("There is not enough memory available to play the game reliably.  You can");
		//puts ("play anyway, but an out of memory condition will eventually pop up.  The");
		//puts ("correct solution is to unload some TSRs or rename your CONFIG.SYS and");
		//puts ("AUTOEXEC.BAT to free up more memory.\n");
		//puts ("Do you want to (Q)uit, or (C)ontinue?");
		for (i=S_NOTENOUGHMEMORY1; i<= S_NOTENOUGHMEMORY5; i++)
			puts(SM_GetString(i));
		i = bioskey (0);
		if ( (i>>8) != sc_C)
		{
			//Quit ("");
			SM_Quit(S_LOADGAME_NOMEM);
		}
	}
*/
// Nisaba patch:
	{
		unsigned i;

		clrscr();
		SM_PutLines(S_NOTENOUGHMEMORY1, S_NOTENOUGHMEMORYEND);
		i = bioskey (0);
		if ( (i>>8) != sc_C)
		{
			//Quit ("");
			SM_Quit(S_QUITTING);
		}
	}
#endif

	US_TextScreen();

	VW_Startup ();
	RF_Startup ();
	IN_Startup ();
	SD_Startup ();
	US_Startup ();

	US_UpdateTextScreen();

	CA_Startup ();

//
// Set the cache box callbacks
//
	drawcachebox = CK_BeginCacheBox;
	updatecachebox = CK_UpdateCacheBox; 
	finishcachebox = CK_FinishCacheBox;


	US_Setup ();

//
// load in and lock down some basic chunks
//

	CA_ClearMarks ();

	CA_MarkGrChunk(STARTFONT);
	CA_MarkGrChunk(STARTTILE8);
	CA_MarkGrChunk(STARTTILE8M);
	CA_MarkGrChunk(STATUSLEFTPICM);
	CA_MarkGrChunk(STATUSTOPPICM);

	CA_CacheMarks (NULL);

	// K1n9_Duk3 mod: The original code locked these to prevent them
	// from being purged. This meant that the memory manager couldn't
	// move them around when sorting memory blocks. My revision of the
	// memory manager allows blocks to be locked in a 'movable' state,
	// which means they will never be purged, but can be moved by the
	// memory manager.
	// Note: 'true' is the same as 'lk_full', 'false' is 'lk_none'
	// for backwards compatibility with the rest of the code.
	MM_SetLock (&grsegs[STARTFONT], lk_movable);
	MM_SetLock (&grsegs[STARTTILE8], lk_movable);
	MM_SetLock (&grsegs[STARTTILE8M], lk_movable);
	MM_SetLock (&grsegs[STATUSLEFTPICM], lk_movable);
	MM_SetLock (&grsegs[STATUSTOPPICM], lk_movable);


	CA_LoadAllSounds ();

	fontcolor = WHITE;

	US_FinishTextScreen();

//
// reclaim the memory from the linked in text screen
//
#if 0
	{
		unsigned	segstart,seglength;

		segstart = FP_SEG(&introscn);
		seglength = 4000/16;
		if (FP_OFF(&introscn))
		{
			segstart++;
			seglength--;
		}

		MML_UseSpace (segstart,seglength);
	}
#else
	// K1n9_Duk3 HACK:
	// introscn always has an offset value of 0 and the linker
	// inserts some padding to make the next block start at a
	// segment-aligned address as well (introscn is at least
	// 4007 bytes long), so we can just do this:
	MML_UseSpace (FP_SEG(&introscn), (4007+15)/16);
#endif

	VW_SetScreenMode (GRMODE);
	VW_ColorBorder (3);
	VW_ClearVideo (BLACK);

//
// initialize variable(s)
//
	VW_SetLineWidth(SCREENWIDTH);

	US_SetLoadSaveHooks(LoadTheGame,SaveTheGame,CK_ResetGame);
}

/*
=====================
=
= DemoLoop
=
=====================
*/

void
DemoLoop (void)
{
	int demoNumber;

//
// demo loop
//
/*
	 * Commander Keen could be 'launched' from the map editor TED to test a map.
	 * This was implemented by having TED launch keen with the /TEDLEVEL xx
	 * parameter, where xx is the level number.
	 */
	
#ifdef DEVBUILD
	if (tedlevel)
	{
		//NoWait = true; // this isn't present in Keen 4-6 and it's useless here

		CK_NewGame();
		//ck_gameState.cheated = true;
		CA_LoadAllSounds();
		ck_gameState.currentLevel = tedlevelnum;
		restartgame = gd_Normal;

		// Support selecting difficulty via an extra command line
		// argument ("easy", "normal", "hard")
		{
			static stringnames args[] = {S_EASY, S_NORMAL, S_HARD, -1};
			int i;

			for (i=1; i<_argc; i++)
			{
				switch (SM_CheckParm(_argv[i], args))
				{
				case 0:
					restartgame = gd_Easy;
					break;
				case 1:
					restartgame = gd_Normal;
					break;
				case 2:
					restartgame = gd_Hard;
					break;
				}
			}
		}
		
		CK_GameLoop();
		Quit(0); // run_ted
	}
#endif
	// Given we're not coming from TED, run through the demos.

	demoNumber = 0;
	ck_levelState = LS_Playing;	// K1n9_Duk3 fix: use enum name instead of number
	
	if (NoWait)
		goto menu;

	while (true)
	{
		// K1n9_Duk3 hack:
		if (abortgame == 2)
		{
			CK_DoHighScores();
			//demoNumber = 4;	// play highscore demo next
			menu:
			abortgame = false;
			CK_MainMenu();
		}
		else	// end of hack
		switch (demoNumber++)
		{
		case 0:	// Terminator scroller and Title Screen
			// If no pixel panning capability
			// Then the terminator screen isn't shown
#if GRMODE == EGAGR
				CK_DrawTerminator();	//TODO: Move this to an episode struct.
#elif GRMODE == CGAGR
				CK_ShowTitleScreen();
#endif
			break;
		case 1:
			CK_DrawStarWars();
			CK_HandleDemoKeys();
			break;
		case 2:
			CK_PlayDemo(0);
			break;
		case 3:
			CK_PlayDemo(2);
			break;
		case 4:
			CK_PlayDemo(3);
			break;
		case 5:
			CK_PlayDemo(4);
			break;
		case 6:
			CK_DoHighScores();
			CK_HandleDemoKeys();	// K1n9_Duk3 addition (CK_DoHighscores doesn't call this anymore)
			break;
		case 7:
			// CK_PlayDemo(3);
			demoNumber = 0;
			break;
		}

		// Game Loop
		/*
		while (1)
		{
			if (ck_levelState == LS_StartingNew || ck_levelState == LS_StartingSaved)	// K1n9_Duk3 fix: use enum names instead of numbers
			{
				CK_GameLoop();
				CK_DoHighScores();
				CK_HandleDemoKeys();	// K1n9_Duk3 addition (CK_DoHighscores doesn't call this anymore)
				if (ck_levelState == LS_StartingNew || ck_levelState == LS_StartingSaved)	// K1n9_Duk3 fix: use enum names instead of numbers
					continue;

				//CK_ShowTitleScreen();	// K1n9_Duk3 mod (for the "View Scores" option)

				if (ck_levelState == LS_StartingNew || ck_levelState == LS_StartingSaved)	// K1n9_Duk3 fix: use enum names instead of numbers
					continue;
			}
			else
			{
				break;
			}
		}
		*/
		// Nisaba patch
		while (ck_levelState == LS_StartingNew || ck_levelState == LS_StartingSaved)	// K1n9_Duk3 fix: use enum names instead of numbers
		{
			CK_GameLoop();
			CK_DoHighScores();
			CK_HandleDemoKeys();	// K1n9_Duk3 addition (CK_DoHighscores doesn't call this anymore)
		}
	}
}


//===========================================================================

#if 0
void SpritePosTest(void)
{
	int spr1, spr2;
	int x1, x2, y1, y2;

	spr1 = CORONAL1SPR;
	spr2 = NISASIGRABRSPR;

	x1=y1=48;
	x2=0;
	y2=-14;

	VW_ClearVideo(WHITE);
	VW_InitDoubleBuffer();

	while (true)
	{
		VWB_Bar(0, 0, 320, 200, WHITE);
		CA_CacheGrChunk(spr1);
		VWB_DrawSprite(x1, y1, spr1);
		CA_UncacheGrChunk(spr1);
		CA_CacheGrChunk(spr2);
		VWB_DrawSprite(x1+x2, y1+y2, spr2);
		CA_UncacheGrChunk(spr2);
		px = 8;
		py = 188;
		sprintf(str, "x=%i y=%i", x2, y2);
		VWB_DrawPropString(str);
		VW_UpdateScreen();

		switch(IN_WaitForKey())
		{
		case sc_Escape:
			return;
		case sc_UpArrow:
			y2--;
			break;
		case sc_DownArrow:
			y2++;
			break;
		case sc_LeftArrow:
			x2--;
			break;
		case sc_RightArrow:
			x2++;
			break;
		case sc_PgUp:
			if (spr2 > STARTSPRITES)
				spr2--;
			break;
		case sc_PgDn:
			if (spr2 < STARTSPRITES+NUMSPRITES-2)
				spr2++;
			break;
		case sc_W:
			y1--;
			break;
		case sc_S:
			y1++;
			break;
		case sc_A:
			x1--;
			break;
		case sc_D:
			x1++;
			break;
		case sc_Q:
			if (spr1 > STARTSPRITES)
				spr1--;
			break;
		case sc_E:
			if (spr1 < STARTSPRITES+NUMSPRITES-2)
				spr1++;
			break;
		case sc_R:
			spr2 = NISASIGRABRSPR;
			break;
		case sc_L:
			spr2 = NISASIGRABLSPR;
			break;
		}
	}
}
#else
#define SpritePosTest() /*nothing*/
#endif

/*
==========================
=
= main
=
==========================
*/

void main (void)
{
	InitGame();

	if (!tedlevel)
		SpritePosTest();

	DemoLoop();					// DemoLoop calls Quit when everything is done
	//Quit("Demo loop exited???");
	SM_Quit(S_DEMOLOOPEXITED);
}

