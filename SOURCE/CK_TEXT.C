/* "Foray in the Forest" Source Code
 * Copyright (C) 2022-2023 Nisaba
 * Copyright (C) 2019-2021 K1n9_Duk3
 *
 * The code in this file is based on:
 * Wolfenstein 3-D Source Code: WL_TEXT.C
 * under the Limited Use Software License Agreement.
*/

// CK_TEXT.C

#include "CK_DEF.H"
#pragma	hdrstop

/*
=============================================================================

TEXT FORMATTING COMMANDS
------------------------
^C<hex digit>  			Change text color
^E[enter]				End of layout (all pages)
^G<y>,<x>,<pic>[enter]	Draw a graphic and push margins
^P[enter]				start new page, must be the first chars in a layout
^L<x>,<y>[ENTER]		Locate to a specific spot, x in pixels, y in lines

=============================================================================
*/

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define BACKCOLOR		GREEN	// K1n9_Duk3 mod: green backgrounds
// K1n9_Duk3 additions:
#define DEFTEXTCOLOR		YELLOW		// default text color
#define PAGENUMCOLOR		LIGHTGREEN	// text color for "pg X of Y"
// Note: The page number will only use the correct color if the background in
// the text border image matches the background color defined above.


#define WORDLIMIT		80
#define FONTHEIGHT		10
#define TOPMARGIN		10
#define BOTTOMMARGIN	10
#define LEFTMARGIN		10
#define RIGHTMARGIN		10
#define PICMARGIN		8
#define TEXTROWS		((200-TOPMARGIN-BOTTOMMARGIN)/FONTHEIGHT)
#define SPACEWIDTH		7
#define SCREENPIXWIDTH	320
#define SCREENMID		(SCREENPIXWIDTH/2)
#define TABWIDTH		16		// K1n9_Duk3 addition

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

int			pagenum,numpages;

unsigned	leftmargin[TEXTROWS],rightmargin[TEXTROWS];
unsigned char		far *text;	// K1n9_Duk3 fix: need unsigned chars!
unsigned	rowon;

int			picx,picy,picnum,picdelay;
boolean		layoutdone;
int			ck_helpSelection;
unsigned	ck_helpScreenChunks[] = { T_HELPSTORY, T_HELPGAME, T_HELPCONTROLS, T_HELPCOMPUTERWRIST, T_HELPKEENINFO, T_HELPCREDITS};

void RipToEOL (void);
int	ParseNumber (void);
void	ParsePicCommand (void);
void	ParseTimedCommand (void);
void	TimedPicCommand (void);
void HandleCommand (void);
void NewLine (void);
void HandleCtrls (void);
void HandleWord (void);
void PageLayout (boolean shownumber);
void BackPage (void);
void CacheLayoutGraphics (long size);
int ShowHelp (void);
void HelpScreens (void);

//===========================================================================

/*
=====================
=
= RipToEOL
=
=====================
*/

void RipToEOL (void)
{
	while (*text++ != '\n')		// scan to end of line
	;
}


/*
=====================
=
= ParseNumber
=
=====================
*/

int	ParseNumber (void)
{
	unsigned char	ch;	// K1n9_Duk3 fix: need unsigned chars!
	char	num[80],*numptr;

//
// scan until a number is found
//
	ch = *text;
	while (ch < '0' || ch >'9')
		ch = *++text;

//
// copy the number out
//
	numptr = num;
	do
	{
		*numptr++ = ch;
		ch = *++text;
	} while (ch >= '0' && ch <= '9');
	*numptr = 0;

	return atoi (num);
}

/*
=====================
=
= ParseHexNumber		(K1n9_Duk3 addition)
=
=====================
*/

int ParseHexNumber (int val)
{
	unsigned char ch;	// K1n9_Duk3 fix: need unsigned chars!

//
// scan until a hex number is found
//
	ch = toupper(*text);
	while (!( (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') ))
	{
		if (ch == '\n' || ch == '\r')
			return val;
		ch = toupper(*++text);
	}

//
// copy the number out
//
	val = 0;
	while (true)
	{
		if (ch >= '0' && ch <= '9')
		{
			val <<= 4;
			val += ch-'0'+0;
		}
		else if (ch >= 'A' && ch <= 'F')
		{
			val <<= 4;
			val += ch-'A'+0xA;
		}
		else
		{
			break;
		}
		ch = toupper(*++text);
	}

	return val;
}

/*
=====================
=
= ParsePicCommand
=
= Call with text pointing just after a ^P
= Upon exit text points to the start of next line
=
=====================
*/

void	ParsePicCommand (void)
{
	picy=ParseNumber();
	picx=ParseNumber();
	picnum=ParseNumber();
	RipToEOL ();
}


void	ParseTimedCommand (void)
{
	picy=ParseNumber();
	picx=ParseNumber();
	picnum=ParseNumber();
	picdelay=ParseNumber();
	RipToEOL ();
}


/*
=====================
=
= TimedPicCommand
=
= Call with text pointing just after a ^P
= Upon exit text points to the start of next line
=
=====================
*/

void	TimedPicCommand (void)
{
	ParseTimedCommand ();

//
// update the screen, and wait for time delay
//
	VW_WaitVBL(1);
	VW_ScreenToScreen(bufferofs, displayofs, 40, 200);

//
// wait for time
//
	TimeCount = 0;
	while (TimeCount < picdelay)
	;

//
// draw pic
//
	VWB_DrawPic (picx&~7,picy,picnum);
}


/*
=====================
=
= HandleCondition		(K1n9_Duk3 addition)
=
=====================
*/

void	HandleCondition (void)
{
	int num, skip;

	num = ParseNumber();
	RipToEOL();

	//
	// check the condition; return if condition is true
	//
	switch (num)
	{
	case 0:
		return;
	case 1:	// 1 - collected at least 17 slug cans
		if (ck_gameState.slugcans >= 17)
			return;
		break;
	case 2:	// 2 - all 6 collectibles collected
		if ((ck_gameState.collectiblesTotal & 0x3F) == 0x3F)
			return;
		break;
	case 3:	// 3 - at least 1 collectible collected
		if ((ck_gameState.collectiblesTotal & 0x3F) != 0 && (ck_gameState.collectiblesTotal & 0x3F) != 0x3F)
			return;
		break;
	case 4:	// 4 - no collectibles collected
		if ((ck_gameState.collectiblesTotal & 0x3F) == 0)
			return;
		break;
	case 5:	// 5 - bonus level completed
		if (ck_gameState.levelsDone[MAP_BONUSLEVEL])
			return;
		break;
	case 6:	// 6 - bonus level NOT completed
		if (!ck_gameState.levelsDone[MAP_BONUSLEVEL])
			return;
		break;
		// Nisaba patch
	case 7:	// 7 - bonus level completed but less then 17 slug cans collected	
		if (ck_gameState.levelsDone[MAP_BONUSLEVEL] && ck_gameState.slugcans < 17)
			return;
		break;
	case 8:	// 8 - bonus level completed and all 17 slug cans collected
		if (ck_gameState.levelsDone[MAP_BONUSLEVEL] && ck_gameState.slugcans >= 17)
			return;
		break;
	case 9:	// 9 - TARDIS level completed
		if (ck_gameState.levelsDone[MAP_TARDISLEVEL])
			return;
		break;
	case 10: // 10 - TARDIS level NOT completed
		if (!ck_gameState.levelsDone[MAP_TARDISLEVEL])
			return;
		break;
	case 11: // 11 - SLUGTEMPLE level completed
		if (ck_gameState.levelsDone[MAP_SLUGBUSTSLEVEL])
			return;
		break;
	case 12: // 12 - SLUGTEMPLE level NOT completed
		if (!ck_gameState.levelsDone[MAP_SLUGBUSTSLEVEL])
			return;
		break;
	case 13: // 13 - cloned some monsters
		if (ck_mapState.machinesActive > 1)
			return;
		break;
	case 14: // 14 - did NOT clone any monsters
		if (!(ck_mapState.machinesActive > 1))
			return;
		break;
	case 15: // 15 - difficulty level is not hard
		if (!(ck_gameState.difficulty == D_Hard))
			return;
		break;
	}

	//
	// condition is false, so skip to next ^I
	//
	while (!(text[0] == '^' && toupper(text[1]) == 'I'))
		text++;
}


/*
=====================
=
= HandleCommand
=
=====================
*/

void HandleCommand (void)
{
	int	i,margin,top,bottom;
	int	picwidth,picheight,picmid;

	switch (toupper(*++text))
	{
	case 'B':
		picy=ParseNumber();
		picx=ParseNumber();
		picwidth=ParseNumber();
		picheight=ParseNumber();
#if 0
		// Keen 4-6 code:
		VWB_Bar(picx,picy,picwidth,picheight,BACKCOLOR);
#else
		// K1n9_Duk3 mod:
		i = ParseHexNumber(BACKCOLOR);
		VWB_Bar(picx,picy,picwidth,picheight,i);
#endif
		RipToEOL();
		break;
	case ';':		// comment
		RipToEOL();
		break;
	case 'P':		// ^P is start of next page, ^E is end of file
	case 'E':
		layoutdone = true;
		text--;    	// back up to the '^'
		break;

	case 'C':		// ^c<hex digit> changes text color
		i = toupper(*++text);	// K1n9_Duk3 note: toupper returns an unsigned char (only -1 is retuned as -1)
		if (i>='0' && i<='9')
			fontcolor = i-'0';
		else if (i>='A' && i<='F')
			fontcolor = i-'A'+10;

#if 0
		fontcolor *= 16;
		i = toupper(*++text);	// K1n9_Duk3 note: toupper returns an unsigned char (only -1 is retuned as -1)
		if (i>='0' && i<='9')
			fontcolor += i-'0';
		else if (i>='A' && i<='F')
			fontcolor += i-'A'+10;
#endif

		fontcolor ^= BACKCOLOR;
		text++;
		break;

	case '>':		// Indent halfway
		px = 160;
		text++;
		break;

	case 'L':
		py=ParseNumber();
		rowon = (py-TOPMARGIN)/FONTHEIGHT;
		py = TOPMARGIN+rowon*FONTHEIGHT;
		px=ParseNumber();
		while (*text++ != '\n')		// scan to end of line
		;
		break;

	case 'T':		// ^Tyyy,xxx,ppp,ttt waits ttt tics, then draws pic
		TimedPicCommand ();
		break;

	case 'G':		// ^Gyyy,xxx,ppp draws graphic
		ParsePicCommand ();
		if (picnum >= STARTPICS && picnum < STARTPICS+NUMPICS)
		{
			VWB_DrawPic (picx&~7,picy,picnum);
			picwidth = pictable[picnum-STARTPICS].width*8;
			picheight = pictable[picnum-STARTPICS].height;
		}
		else if (picnum >= STARTSPRITES && picnum < STARTSPRITES+NUMSPRITES)
		{
			VWB_DrawSprite (picx-(spritetable[picnum-STARTSPRITES].orgx>>G_P_SHIFT),picy-(spritetable[picnum-STARTSPRITES].orgy>>G_P_SHIFT),picnum);
			picwidth = spritetable[picnum-STARTSPRITES].width*8;
			picheight = spritetable[picnum-STARTSPRITES].height;
		}
		else if (picnum >= STARTTILE16 && picnum < STARTTILE16+NUMTILE16)
		{
			VWB_DrawTile16 (picx&~7,picy,picnum-STARTTILE16);
			picwidth = picheight = 16;
		}
		else if (picnum >= STARTTILE16M && picnum < STARTTILE16M+NUMTILE16M)
		{
			VWB_DrawTile16M (picx&~7,picy,picnum-STARTTILE16M);
			picwidth = picheight = 16;
		}
		//
		// adjust margins
		//
		picmid = picx + picwidth/2;
		if (picmid > SCREENMID)
			margin = picx-PICMARGIN;			// new right margin
		else
			margin = picx+picwidth+PICMARGIN;	// new left margin

		top = (picy-TOPMARGIN)/FONTHEIGHT;
		if (top<0)
			top = 0;
		bottom = (picy+picheight-TOPMARGIN)/FONTHEIGHT;
		if (bottom>=TEXTROWS)
			bottom = TEXTROWS-1;

		for (i=top;i<=bottom;i++)
			if (picmid > SCREENMID)
				rightmargin[i] = margin;
			else
				leftmargin[i] = margin;

		//
		// adjust this line if needed
		//
		if (px < leftmargin[rowon])
			px = leftmargin[rowon];
		break;

	case 'H':		// ^H draws high scores (K1n9_Duk3 addition)
		i = px;
		bottom = py;
		CK_DrawHighscores();
		text++;
		px = i;
		py = bottom;
		break;

	case 'I':		// ^I checks a condition and skips to the next ^I if it is false (K1n9_Duk3 addition)
		HandleCondition();
		break;

	case 'M':		// music commands
		switch (toupper(text[1]))
		{
		case 'P':	// ^MP -- Music Pause
			SD_MusicOff();
			break;

		case 'R':	// ^MR -- Music Resume
			SD_MusicOn();
			break;

		case 'O':	// ^MOxxx -- Music Once (play music number xxx but don't repeat it)
			StartMusic(ParseNumber(), false);
			break;

		default:	// ^Mxxx -- Play music number xxx
			StartMusic(ParseNumber(), true);
			break;
		}
		RipToEOL();
		break;

	case 'S':		// ^Sxxx -- Play sound number xxx
		SD_PlaySound(ParseNumber());
		RipToEOL();
		break;

	case 'W':		// Wait until sound is done
		SD_WaitSoundDone();
		RipToEOL();
		break;

	case 'Q':		// Quiet - stop sound and music
		StopMusic();
		SD_StopSound();
		RipToEOL();
		break;

	case 'D':		// ^Dttt waits ttt tics
		picdelay = ParseNumber();
		RipToEOL();
		TimeCount = 0;
		VW_WaitVBL(1);
		VW_ScreenToScreen(bufferofs, displayofs, 40, 200);
		do {} while (TimeCount < picdelay);
		break;
		
	case 'F':	// ^Fn set font number to n
		fontnumber = ParseNumber();
		break;
	}
}


/*
=====================
=
= NewLine
=
=====================
*/

void NewLine (void)
{
	char	ch;

	if (++rowon == TEXTROWS)
	{
	//
	// overflowed the page, so skip until next page break
	//
		layoutdone = true;
		do
		{
			if (*text == '^')
			{
				ch = toupper(*(text+1));
				if (ch == 'E' || ch == 'P')
				{
					layoutdone = true;
					return;
				}
			}
			text++;

		} while (1);

	}
	px = leftmargin[rowon];
	py+= FONTHEIGHT;
}



/*
=====================
=
= HandleCtrls
=
=====================
*/

void HandleCtrls (void)
{
	char	ch;

	ch = *text++;			// get the character and advance

	switch (ch)
	{
	case '\n':
		NewLine ();
		break;
	case '\t':				// K1n9_Duk3 addition: handle tabs
		px += TABWIDTH-LEFTMARGIN;
		px += LEFTMARGIN-(px % TABWIDTH);
		break;
	}

}


/*
=====================
=
= HandleWord
=
=====================
*/

void HandleWord (void)
{
	char		word[WORDLIMIT];
	int			i,wordindex;
	unsigned	wwidth,wheight,newpos;


	//
	// copy the next word into [word]
	//
	word[0] = *text++;
	wordindex = 1;
	while (*text>32)
	{
		word[wordindex] = *text++;
		if (++wordindex == WORDLIMIT)
			//Quit ("PageLayout: Word limit exceeded");
			SM_Quit(S_WORDLIMITEXCEEDED);
	}
	word[wordindex] = 0;		// stick a null at end for C

	//
	// see if it fits on this line
	//
	VW_MeasurePropString (word,&wwidth,&wheight);

	while (px+wwidth > rightmargin[rowon])
	{
		NewLine ();
		if (layoutdone)
			return;		// overflowed page
	}

	//
	// print it
	//
	newpos = px+wwidth;
	VWB_DrawPropString (word);
	px = newpos;

	//
	// suck up any extra spaces
	//
	while (*text == ' ')
	{
		px += SPACEWIDTH;
		text++;
	}
}

/*
=====================
=
= PageLayout
=
= Clears the screen, draws the pics on the page, and word wraps the text.
= Returns a pointer to the terminating command
=
=====================
*/

void PageLayout (boolean shownumber)
{
	int		i,oldfontcolor;
	unsigned char	ch;	// K1n9_Duk3 fix: need unsigned chars!

	oldfontcolor = fontcolor;

	fontcolor = DEFTEXTCOLOR^BACKCOLOR;	// K1n9_Duk3 fix:

//
// clear the screen
//
	VWB_Bar (0,0,320,200,BACKCOLOR);
	VWB_DrawPic (0,0,H_TOPWINDOWPIC);
	VWB_DrawPic (0,8,H_LEFTWINDOWPIC);
	VWB_DrawPic (312,8,H_RIGHTWINDOWPIC);

	if (shownumber)
		VWB_DrawPic (8,176,H_BOTTOMINFOPIC);
	else
		VWB_DrawPic (8, 192, H_BOTTOMWINDOWPIC);


	for (i=0;i<TEXTROWS;i++)
	{
		leftmargin[i] = LEFTMARGIN;
		rightmargin[i] = SCREENPIXWIDTH-RIGHTMARGIN;
	}

	px = LEFTMARGIN;
	py = TOPMARGIN;
	rowon = 0;
	layoutdone = false;

//
// make sure we are starting layout text (^P first command)
//
	while (*text <= 32)
		text++;

	if (*text != '^' && toupper(*++text) != 'P')
		//Quit ("PageLayout: Text not headed with ^P");
		SM_Quit(S_BADTEXTSTART);

	while (*text++ != '\n')
	;


//
// process text stream
//
	do
	{
		ch = *text;

		if (ch == '^')
			HandleCommand ();
		else
/*
		if (ch == 9)
		{
		//Tab
		 px = (px+8)&0xf8;
		 text++;
		}
		else 
*/
		if (ch <= 32)
			HandleCtrls ();
		else
			HandleWord ();

	} while (!layoutdone);

	pagenum++;

	if (shownumber)
	{
		//strcpy (str,"pg ");
		SM_CopyString(str, S_PAGE);
		itoa (pagenum,str2,10);
		strcat (str,str2);
		//strcat (str," of ");
		SM_ConcatString(str, S_OF);
		itoa (numpages,str2,10);
		strcat (str,str2);

		py = 186;
		px = 218;
		fontcolor = PAGENUMCOLOR^BACKCOLOR;	// K1n9_Duk3 fix

		VWB_DrawPropString (str);
	}

	fontcolor = oldfontcolor;
}

//===========================================================================

/*
=====================
=
= BackPage
=
= Scans for a previous ^P
=
=====================
*/

void BackPage (void)
{
	pagenum--;
	do
	{
		text--;
		if (*text == '^' && toupper(*(text+1)) == 'P')
			return;
	} while (1);
}


//===========================================================================


/*
=====================
=
= CacheLayoutGraphics
=
= Scans an entire layout file (until a ^E) marking all graphics used, and
= counting pages, then caches the graphics in
=
=====================
*/
void CacheLayoutGraphics (long size)
{
	unsigned oldoff;	// K1n9_Duk3 addition
	char	far *bombpoint, far *textstart;
	char	ch;

	textstart = text;
	bombpoint = text+(size <= 0xFFFF? size : 0xFFFF);	// K1n9_Duk3 mod
	numpages = pagenum = 0;

	CA_ClearMarks();

	do
	{
		oldoff = FP_OFF(text);	// K1n9_Duk3 addition

		if (*text == '^')
		{
			ch = toupper(*++text);
			if (ch == 'P')		// start of a page
				numpages++;
			if (ch == 'E')		// end of file, so load graphics and return
			{
				CA_MarkGrChunk(H_TOPWINDOWPIC);
				CA_MarkGrChunk(H_LEFTWINDOWPIC);
				CA_MarkGrChunk(H_RIGHTWINDOWPIC);
				CA_MarkGrChunk(H_BOTTOMINFOPIC);
				CA_MarkGrChunk(H_BOTTOMWINDOWPIC);
				CA_CacheMarks (NULL);
				text = textstart;
				return;
			}
			if (ch == 'G')		// draw graphic command, so mark graphics
			{
				ParsePicCommand ();
				CA_MarkGrChunk (picnum);
			}
			if (ch == 'T')		// timed draw graphic command, so mark graphics
			{
				ParseTimedCommand ();
				CA_MarkGrChunk (picnum);
			}
			// K1n9_Duk3 addition: also handle condition here, so we won't
			// cache graphics that we don't need
			if (ch == 'I')
			{
				HandleCondition();
			}
			if (ch == 'F')
			{
				unsigned font = ParseNumber();
				CA_MarkGrChunk(STARTFONT+font);
			}
		}
		else
			text++;

	} while (text<bombpoint && FP_OFF(text) > oldoff);	// K1n9_Duk3 mod: exit on 64k overflow

	//Quit ("CacheLayoutGraphics: No ^E to terminate file!");
	SM_Quit(S_BADTEXTEND);
}


/*
=====================
=
= ShowHelp
=
=====================
*/

#define NUMHELPTEXTS (sizeof(ck_helpScreenChunks)/sizeof(ck_helpScreenChunks[0]))
#define MAXHELPTEXT (NUMHELPTEXTS-1)

int ShowHelp (void)
{
	CursorInfo cursor;

	// Draw the backdrop
	VWB_Bar(0,0,320,200, BACKCOLOR);

	// Load border graphics
	CA_CacheGrChunk(H_MENUPIC);
	CA_CacheGrChunk(PAGEWALKRSPR);
	CA_CacheGrChunk(H_TOPWINDOWPIC);
	CA_CacheGrChunk(H_LEFTWINDOWPIC);
	CA_CacheGrChunk(H_RIGHTWINDOWPIC);
	CA_CacheGrChunk(H_BOTTOMWINDOWPIC);

	// Draw border graphics
	VWB_DrawPic(0,0,H_TOPWINDOWPIC);
	VWB_DrawPic(0,8,H_LEFTWINDOWPIC);
	VWB_DrawPic(312,8,H_RIGHTWINDOWPIC);
	VWB_DrawPic(8,192,H_BOTTOMWINDOWPIC);
	VWB_DrawPic(96,8,H_MENUPIC);

	//ydelta = 0;
	IN_ClearKeysDown();

	// Move Hand up and down
	while (true)
	{
		if (ck_helpSelection < 0)
			ck_helpSelection = 0;
		else if (ck_helpSelection > MAXHELPTEXT)
			ck_helpSelection = MAXHELPTEXT;

		// Draw the pointer
		VWB_DrawSprite(72 + 4, 48 + 20 * ck_helpSelection, PAGEWALKRSPR);
		VW_UpdateScreen();

		// Erase the pointer
		VWB_Bar(72, 48 + 20 * ck_helpSelection, 96-72, 40, BACKCOLOR);

		IN_ReadMenuCursor(&cursor);
		
		if (cursor.y < 0)
			ck_helpSelection--;
		else if (cursor.y > 0)
			ck_helpSelection++;
		else if (cursor.button0)
		{
			VW_ClearVideo(BACKCOLOR);
			return ck_helpSelection;
		}
		else if (cursor.button1)
		{
			VW_ClearVideo(BACKCOLOR);
			do IN_ReadCursor(&cursor); while (cursor.button1);
			return -1;
		}
	}
}

//===========================================================================

static	int	oldfont;

static void near TextEnter(void)
{
	oldfont = fontnumber;
	fontnumber = 0;
	
	CA_UpLevel();
	CA_SetGrPurge();
	
	VW_ClearVideo(BACKCOLOR);
	CK_DefaultPalette();
	RF_FixOfs();	// clear panning variables
	
	bufferofs = 0x8000;
	displayofs = 0;
	VW_SetScreen(displayofs, 0);
}

static void near TextExit(void)
{
	fontnumber = oldfont;
	
	CA_DownLevel();
	
	VW_ClearVideo(BACKCOLOR);
	RF_FixOfs();	// reset displayofs & bufferofs for Refresh Manager use
	
	IN_ClearKeysDown();
}

static void near SetupText(unsigned grchunk)
{
	CA_CacheGrChunk(grchunk);
	text = grsegs[grchunk];
	MM_SetLock(&grsegs[grchunk], lk_full);
	CacheLayoutGraphics(MM_GetSize(&grsegs[grchunk]));
}

static void near FlipPages(void)
{
	// swap displayofs and bufferofs:
	_AX = bufferofs;
	_DX = displayofs;
	bufferofs = _DX;
	displayofs = _AX;
	
	VW_SetScreen(displayofs, 0);
}


//===========================================================================


/*
=================
=
= HelpScreens
=
=================
*/
void HelpScreens (void)
{
	boolean refresh;
	int	artnum;
	int	helppage;
	
	CursorInfo cursor;

	TextEnter();
	
	StartMusic(23, true);

	//loop start
	while (1)
	{

		// Choose which help screen to draw
		helppage = ShowHelp();
		VW_ClearVideo(BACKCOLOR);

		if (helppage == -1)
		{
			// Exit

			// K1n9_Duk3 fix: stop the music before re-caching to make more memory available
			StopMusic();
			TextExit();
			return;
		}

		SetupText(artnum = ck_helpScreenChunks[helppage]);
		refresh = true;

		// Flip pages
		while (1)
		{
			if (refresh)
			{
				refresh = false;
				PageLayout(true);

				FlipPages();
			}
			
			IN_ReadMenuCursor(&cursor);
			
			if (cursor.button1)
			{
				do IN_ReadMenuCursor(&cursor); while (cursor.button1);
				
				MM_FreePtr(&grsegs[artnum]);
				IN_ClearKeysDown();
				break;
			}
			else if (cursor.x < 0 || cursor.y < 0 || LastScan == sc_PgUp)
			{
				LastScan = sc_None;
				if (pagenum > 1)
				{
					BackPage();
					BackPage();
					refresh = true;
				}
			}
			else if (cursor.x > 0 || cursor.y > 0 || LastScan == sc_PgDn)
			{
				LastScan = sc_None;
				if (pagenum < numpages)
				{
					refresh = true;
				}
			}
		}
	}
}

void EndText(int num)
{
	int i;

	CA_ClearMarks();	// don't re-cache in-level graphics after the text, THE GAME IS OVER
	
	TextEnter();

	SetupText(num);

#if 0
	// the flashing arrows version:
	
	// cache the arrow graphics:
	CA_CacheGrChunk(TEXTARROW2PIC);
	CA_CacheGrChunk(TEXTARROW1PIC);

	while (pagenum < numpages)
	{
		PageLayout(false);
		IN_ClearKeysDown();
		VW_SetScreen(bufferofs, 0);	// display buffer page to make flashing arrows visible without having to call VW_UpdateScreen()
		do {
			VWB_DrawPic(298, 184, TEXTARROW1PIC);
			for (i=0; i<TickBase; i++)
			{
				if (num == T_END_LOSE && MusicMode != smm_Off && !SD_MusicPlaying())	// K1n9_Duk3 addition: lose text "ends" when music ends
					goto done;

				if (IN_IsUserInput())
				{
					goto nextpage;
				}
				VW_WaitVBL(1);
			}
			VWB_DrawPic(298, 184, TEXTARROW2PIC);
			for (i=0; i<TickBase; i++)
			{
				if (IN_IsUserInput())
				{
					goto nextpage;
				}
				VW_WaitVBL(1);
			}
		} while (1);
nextpage:
		FlipPages();
	}
#else
	// the no feedback version (FITF doesn't have these arrow graphics):
	while (pagenum < numpages)
	{
		PageLayout(false);
		FlipPages();
		
		IN_ClearKeysDown();
		
		do
		{
			if (num == T_END_LOSE && MusicMode != smm_Off && !SD_MusicPlaying())	// K1n9_Duk3 addition: lose text "ends" when music ends
				goto done;
		} while (!IN_IsUserInput());
	}
#endif
done:
	StopMusic();

	MM_FreePtr(&grsegs[num]);	// we MUST either free or unlock the block here!
	
	TextExit();
}
