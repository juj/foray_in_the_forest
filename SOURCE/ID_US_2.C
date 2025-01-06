/* "Foray in the Forest" Source Code
 * Copyright (C) 2022-2023 Nisaba
 * Copyright (C) 2019-2023 K1n9_Duk3
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

//
//      ID Engine
//      ID_US.c - User Manager - User interface
//      By Jason Blochowiak
//      Hacked up for Foray in the Forest by K1n9_Duk3
//

// This is a heavily modified version of the original Catacomb 3-D UI code.
// All structs and routines have been modified to move all string literals into
// the external string file. And the Load/Save menu code has ben modified to
// allow Quick-Saving and Quick-Loading.
//
// All references to other games (#ifdefs) have been removed since they were not
// used in FITF anyway and there was no point in adapting the unused code to the
// new string manager system. The code is a lot easier to read now.

#include "ID_HEADS.H"
#pragma hdrstop

#pragma warn    -pia

//      Special imports
extern	boolean         showscorebox;
extern	boolean         oldshooting;

//      Global variables
		boolean         ingame,abortgame,loadedgame;
		GameDiff        restartgame = gd_Continue;

//      Internal variables
static  boolean         GameIsDirty,
					QuitToDos,
					CtlPanelDone;

//      Forward reference prototypes
static void near     USL_SetupCard(void);

//      Control panel data

#define CtlPanelSX      74
#define CtlPanelSY      48
#define CtlPanelEX      234
#define CtlPanelEY      150
#define CtlPanelW       (CtlPanelEX - CtlPanelSX)
#define CtlPanelH       (CtlPanelEY - CtlPanelSY)

#define TileBase        44	// K1n9_Duk3 mod for FITF

// Hacks for Keen Dreams
#ifdef EGAGR
#define BackColor               8	// dark grey
#define HiliteColor             2	// light green
#define WarningColor            6	// yellow
#define NohiliteColor   10
#else // CGAGR
#define BackColor               0
#define HiliteColor             2
#define WarningColor            2
#define NohiliteColor   1
#endif

typedef enum
		{
			uc_None,
			uc_Return,
			uc_Abort,
			uc_Quit,
			uc_Loaded,
			uc_PayToSave,	// K1n9_Duk3 addition
			uc_Help,			// K1n9_Duk3 addition
			uc_ViewScores,	// K1n9_Duk3 addition
			uc_SEasy,
			uc_SNormal,
			uc_SHard,
		} UComm;
typedef enum
		{
			uii_Bad,
			uii_Button,uii_RadioButton,uii_Folder
		} UIType;
typedef enum
		{
			ui_Normal = 0,
			ui_Pushed = 1,
			ui_Selected = 2,
			ui_Disabled = 4,
			ui_Separated = 8
		} UIFlags;
#define UISelectFlags (ui_Pushed | ui_Selected | ui_Disabled)

typedef enum
		{
			uic_SetupCard,uic_DrawCard,uic_TouchupCard,
			uic_DrawIcon,uic_Draw,uic_Hit
		} UserCall;

typedef struct  UserItem
		{
				UIType                  type;
				UIFlags                 flags;
				ScanCode                hotkey;
				stringnames             text;
				UComm                   comm;
				void                    far *child;     // Should be (UserItemGroup *)

				word                    x,y;
		} UserItem;
typedef struct  UserItemGroup
		{
				word                    x,y;
				graphicnums             title;
				//ScanCode                hotkey;	// K1n9_Duk3 mod: is never used anyway
				UserItem                far *items;
				boolean                 (*custom)(UserCall,struct UserItem far *);      // Custom routine

				word                    cursor;
		struct  UserItemGroup   far *parent;
		} UserItemGroup;

static  stringnames     BottomS1,BottomS2,BottomS3;
static  UComm           Communication;
static  ScanCode        dope;
static  ScanCode        *KeyMaps[] =
					{
						// for movement menu:
						&KbdDefs[0].upleft,
						&KbdDefs[0].up,
						&KbdDefs[0].upright,
						&KbdDefs[0].right,
						&KbdDefs[0].downright,
						&KbdDefs[0].down,
						&KbdDefs[0].downleft,
						&KbdDefs[0].left,
						// for buttons menu:
						&KbdDefs[0].button0,
						&KbdDefs[0].button1,
						&KbdDefs[0].button2,	// K1n9_Duk3 mod
						&dope,
						// for function buttons menu:
						&config.statusscan,
						&config.detectorscan,
						&config.scoreboxscan,
						&config.savemenuscan,
						&config.loadmenuscan,
						&config.quicksavescan,
						&config.quickloadscan
					};
static	MenuType	menutype;
static	word	QuickSaveSlot = MaxSaveGames;

static void near USL_UpLevel(void);	// K1n9_Duk3 addition

// Custom routine prototypes
static	boolean	USL_ConfigCustom(UserCall call,struct UserItem far *item),
				USL_KeyCustom(UserCall call,struct UserItem far *item),
				USL_KeySCustom(UserCall call,struct UserItem far *item),
				USL_Joy1Custom(UserCall call,struct UserItem far *item),
				USL_Joy2Custom(UserCall call,struct UserItem far *item),
				USL_LoadCustom(UserCall call,struct UserItem far *item),
				USL_SaveCustom(UserCall call,struct UserItem far *item),
				USL_OptionsCustom(UserCall call, struct UserItem far *item),
				USL_ScoreCustom(UserCall call,struct UserItem far *item),
				USL_CompCustom(UserCall call,struct UserItem far *item),
				USL_PongCustom(UserCall call,struct UserItem far *item);

#define DefButton(key,text)         uii_Button,ui_Normal,key,text
#define DefRButton(key,text)        uii_RadioButton,ui_Normal,key,text
#define DefFolder(key,text,child)   uii_Folder,ui_Normal,key,text,uc_None,child

#define CustomGroup(title,custom)   0,0,title,NULL,custom

	// Sound menu
	UserItem far soundi[] =
	{
		{DefRButton(sc_N,S_NOSOUNDEFFECTS)},
		{DefRButton(sc_P,S_PCSPEAKER)},
		{DefRButton(sc_A,S_ADLIBSOUNDBLASTER)},
		{uii_Bad}
	};
	UserItemGroup   far soundgroup = {8,0,CP_SOUNDMENUPIC,soundi};

	// Music menu
	UserItem far musici[] =
	{
		{DefRButton(sc_N,S_NOMUSIC)},
		{DefRButton(sc_A,S_ADLIBSOUNDBLASTER)},
		{uii_Bad}
	};
	UserItemGroup   far musicgroup = {8,0,CP_MUSICMENUPIC,musici};

	// New game menu
	UserItem far newgamei[] =
	{
		{DefButton(sc_E,S_BEGINEASYGAME),uc_SEasy},
		{DefButton(sc_N,S_BEGINNORMALGAME),uc_SNormal},
		{DefButton(sc_H,S_BEGINHARDGAME),uc_SHard},
		{uii_Bad}
	};
	UserItemGroup   far newgamegroup = {8,0,CP_NEWGAMEMENUPIC,newgamei,0,1};

	// Load/Save game menu
	UserItem far loadsavegamei[] =
	{
		{DefButton(sc_1,-1)},
		{DefButton(sc_2,-1)},
		{DefButton(sc_3,-1)},
		{DefButton(sc_4,-1)},
		{DefButton(sc_5,-1)},
		{DefButton(sc_6,-1)},
		{uii_Bad}
	};
	UserItemGroup   far loadgamegroup = {4,3,CP_LOADMENUPIC,loadsavegamei,USL_LoadCustom};
	UserItemGroup   far savegamegroup = {4,3,CP_SAVEMENUPIC,loadsavegamei,USL_SaveCustom};

	// Options menu
	UserItem far optionsi[] =
	{
		{DefButton(sc_S,S_SCOREBOX)},
		{DefButton(sc_C,S_SVGACOMPATIBILITY)},
		{DefButton(sc_F,S_FIXJERKYMOTION)},
		{DefButton(sc_T,S_TWOBUTTONFIRING)},
		{DefButton(sc_M,S_MAXIMUMFRAMERATE)},
		{DefButton(sc_P,S_PAYTOSAVE)},
		{DefButton(sc_Q,S_QUICKLOAD)},
		{DefButton(sc_Q,S_QUICKSAVE)},
		{uii_Bad}
	};
	UserItemGroup   far optionsgroup = {0,0,CP_OPTIONSMENUPIC,optionsi,USL_OptionsCustom};

	// Keyboard menu
	UserItem far keyi[] =
	{
		{DefButton(sc_None,S_UPLEFT)},
		{DefButton(sc_None,S_UP)},
		{DefButton(sc_None,S_UPRIGHT)},
		{DefButton(sc_None,S_RIGHT)},
		{DefButton(sc_None,S_DOWNRIGHT)},
		{DefButton(sc_None,S_DOWN)},
		{DefButton(sc_None,S_DOWNLEFT)},
		{DefButton(sc_None,S_LEFT)},
		{uii_Bad}
	};
	UserItemGroup   far keygroup = {0,0,CP_KEYMOVEMENTPIC,keyi,USL_KeyCustom};
	UserItem far keybi[] =
	{
		{DefButton(sc_J,S_JUMP)},
		{DefButton(sc_P,S_POGO)},
		{DefButton(sc_F,S_FIRE)},
		{DefButton(sc_None,S_JOKEKEYNAME1)},
		{uii_Bad}
	};
	UserItemGroup   far keybgroup = {0,0,CP_KEYBUTTONPIC,keybi,USL_KeyCustom};
	UserItem far keyfbi[] =
	{
		{DefButton(sc_S,S_STATUS)},
		{DefButton(sc_D,S_DETECTOR)},
		{DefButton(sc_T,S_TOGGLESCOREBOX)},
		{DefButton(sc_S,S_SAVEMENU)},
		{DefButton(sc_L,S_LODAMENU)},
		{DefButton(sc_Q,S_QUICKSAVE)},
		{DefButton(sc_Q,S_QUICKLOAD)},
		{uii_Bad}
	};
	UserItemGroup   far keyfbgroup = {0,0,CP_KEYBUTTONPIC,keyfbi,USL_KeyCustom};
	UserItem far keysi[] =
	{
		{DefFolder(sc_M,S_MOVEMENT,&keygroup)},
		{DefFolder(sc_A,S_ACTIONBUTTONS,&keybgroup)},
		{DefFolder(sc_F,S_FUNCTIONBUTTONS,&keyfbgroup)},
		{uii_Bad}
	};
	UserItemGroup   far keysgroup = {8,0,CP_KEYBOARDMENUPIC,keysi,USL_KeySCustom};

	// Joystick #1 & #2
	UserItemGroup   far joy1group = {CustomGroup(CP_JOYSTICKMENUPIC,USL_Joy1Custom)};
	UserItemGroup   far joy2group = {CustomGroup(CP_JOYSTICKMENUPIC,USL_Joy2Custom)};

	// Config menu
	UserItem far configi[] =
	{
		{DefFolder(sc_S,S_SOUND,&soundgroup)},
		{DefFolder(sc_M,S_MUSIC,&musicgroup)},
		{DefFolder(sc_O,S_OPTIONS,&optionsgroup)}, // Bring this back for Keen Dreams
		{uii_Folder,ui_Separated,sc_K,S_USEKEYBOARD,uc_None,&keysgroup},
		{DefFolder(sc_J,S_USEJOYSTICK1,&joy1group)},
		{DefFolder(sc_J,S_USEJOYSTICK2,&joy2group)},
		{uii_Bad}
	};
	UserItemGroup   far configgroup = {8,0,CP_CONFIGMENUPIC,configi,USL_ConfigCustom};

	// Main menu
	UserItemGroup   far ponggroup = {0,0,0,0,USL_PongCustom};
	UserItem far rooti[] =
	{
		{DefFolder(sc_N,S_NEWGAME,&newgamegroup)},
		{DefFolder(sc_L,S_LOADGAME,&loadgamegroup)},
		{DefFolder(sc_S,S_SAVEGAME,&savegamegroup)},
		{DefButton(sc_R,-1),uc_Return},        // Return to Game/Demo
		{DefButton(sc_E,-1),uc_Abort},         // End Game / View Scores
		{DefFolder(sc_M,S_PADDLEWAR,&ponggroup)},	// Minigame
		{DefButton(sc_H,S_HELPGAMEINFO),uc_Help},	// K1n9_Duk3 addition
		{DefFolder(sc_C,S_CONFIGURE,&configgroup)},		// K1n9_Duk3 mod (changed menu order)
		{DefButton(sc_Q,S_QUIT1),uc_Quit},
		{uii_Bad}
	};
	UserItemGroup   far rootgroup = {24,0,CP_MAINMENUPIC,/*sc_None,*/rooti};
#undef  DefButton
#undef  DefFolder

#define MaxCards        7
	word                    cstackptr;
	UserItemGroup   far *cardstack[MaxCards],
					far *topcard;

//      Card stack code
static void near
USL_SetupStack(void)
{
	// FITF code:
	switch (menutype)
	{
	case mt_Load:
		cardstack[0] = topcard = &loadgamegroup;
		break;
	case mt_Save:
		cardstack[0] = topcard = &savegamegroup;
		break;
	default:
		cardstack[0] = topcard = &rootgroup;
		break;
	}
	cstackptr = 0;
	
	if (topcard->custom)
		topcard->custom(uic_SetupCard,nil);
}

static void near
USL_PopCard(void)
{
	if (!cstackptr)
		return;

	topcard = cardstack[--cstackptr];
}

static void near pascal
USL_PushCard(UserItemGroup far *card)
{
	if (cstackptr == MaxCards - 1)
		return;

	topcard = cardstack[++cstackptr] = card;
}

static void near pascal
USL_DrawItemIcon(UserItem far *item)
{
	word    flags,tile;

	if (topcard->custom && topcard->custom(uic_DrawIcon,item))
		return;

	flags = item->flags;
	if (flags & ui_Disabled)
		tile = TileBase + ((flags & ui_Selected)? 5 : 4);
	else if ((item->type == uii_RadioButton) && (!(flags & ui_Pushed)))
		tile = TileBase + ((flags & ui_Selected)? 3 : 2);
	else
		tile = TileBase + ((flags & ui_Selected)? 1 : 0);
	VWB_DrawTile8(item->x,item->y,tile);
}

static void near pascal
USL_DrawItem(UserItem far *item)
{
	if (topcard->custom && topcard->custom(uic_Draw,item))
		return;

	VWB_Bar(CtlPanelSX + 1,item->y,
			CtlPanelEX - CtlPanelSX - 1,8,BackColor);       // Clear out background
	USL_DrawItemIcon(item);
	if ((item->flags & ui_Selected) && !(item->flags & ui_Disabled))
		fontcolor = HiliteColor;
	else
		fontcolor = NohiliteColor;
	px = item->x + 8;
	py = item->y + 1;
	SM_DrawString(item->text);
	fontcolor = F_BLACK;
}

// Color hack for Keen Dreams is here too
#define MyLine(y)       VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,y,NohiliteColor);

static void near
USL_DrawBottom(void)
{
	word    w,h;

	fontcolor = NohiliteColor;

	px = CtlPanelSX + 4;
	py = CtlPanelEY - 15;
	SM_DrawString(BottomS1);

	SM_MeasureString(BottomS2,&w,&h);
	px = CtlPanelEX - 4 - w;
	SM_DrawString(BottomS2);

	SM_MeasureString(BottomS3,&w,&h);
	px = CtlPanelSX + ((CtlPanelEX - CtlPanelSX - w) / 2);
	py += h + 1;
	SM_DrawString(BottomS3);

	fontcolor = F_WHITE;
	MyLine(CtlPanelEY - 17);
}

static void near
USL_DrawCtlPanelContents(void)
{
	int                             x,y;
	UserItem                far *item;

	if (topcard->custom && topcard->custom(uic_DrawCard,nil))
		return;

	if (topcard->title)
	{
		// Draw the title
		MyLine(CtlPanelSY + 7);
		VWB_DrawPic(CtlPanelSX + 6,CtlPanelSY,topcard->title);
	}

	USL_DrawBottom();

	if (!topcard->items)
		return;

	x = topcard->x + CtlPanelSX;
	if (x % 8)
		x += 8 - (x % 8);
	y = topcard->y + CtlPanelSY + 12;
	for (item = topcard->items;item->type != uii_Bad;item++)
	{
		if (item->flags & ui_Separated)
			y += 8;

		item->x = x;
		item->y = y;
		USL_DrawItem(item);
		y += 8;
	}
	if (topcard->custom)
		topcard->custom(uic_TouchupCard,nil);
}

static void near
USL_DrawCtlPanel(void)
{
	if (topcard->items || topcard->title)
	{
		// Draw the backdrop
		VWB_DrawPic(0,0,CP_MENUSCREENPIC);

		// Draw the contents
		USL_DrawCtlPanelContents();
	}

	// Refresh the screen
	VW_UpdateScreen();

	if (screenfaded)
		VW_FadeIn();
}

static void near pascal
USL_DialogSetup(word w,word h,word *x,word *y)
{
	VWB_DrawMPic(CtlPanelSX,CtlPanelSY,CP_MENUMASKPICM);

	*x = CtlPanelSX + ((int)(CtlPanelW - w) / 2);	// K1n9_Duk3 mod: use signed division
	*y = CtlPanelSY + ((int)(CtlPanelH - h) / 2);	// K1n9_Duk3 mod: use signed division
	VWB_Bar(*x,*y,w + 1,h + 1,BackColor);
	VWB_Hlin(*x - 1,*x + w + 1,*y - 1,NohiliteColor);
	VWB_Hlin(*x - 1,*x + w + 1,*y + h + 1,NohiliteColor);
	VWB_Vlin(*y - 1,*y + h + 1,*x - 1,NohiliteColor);
	VWB_Vlin(*y - 1,*y + h + 1,*x + w + 1,NohiliteColor);
}

static void near pascal
USL_ShowLoadSave(stringnames s,char *name)
{
	char    msg[MaxGameName + 4];
	
	// basically sprintf(msg, "'%s'", name):
	{
		char	*ptr = msg;

		*ptr++ = '\'';
		strcpy(ptr, name);
		while (*ptr) ptr++;
		*ptr++ = '\'';
		*ptr++ = 0;
	}
	// the code is split into two blocks with their own local variables to let
	// the compiler see that ptr is not used anymore and the register assigned
	// to that variable can be re-used in the code below.
	{
		word    x,y,
				w,h,
				tw,sw;
				
		SM_MeasureString(s,&sw,&h);
		USL_MeasureString(msg,&w,&h);
		tw = ((sw > w)? sw : w) + 6;
		USL_DialogSetup(tw,(h * 2) + 2,&x,&y);
		py = y + 2;
		px = x + ((int)(tw - sw) / 2);
		SM_DrawString(s);
		py += h;
		px = x + ((int)(tw - w) / 2);
		USL_DrawString(msg);

		VW_UpdateScreen();

		IN_UserInput(100, true);
	}
}

static boolean near pascal
USL_CtlDialog(stringnames s1,stringnames s2,stringnames s3, boolean strict)
{
	word            w,h,sh,
				w1,w2,w3,
				x,y;
	ScanCode        c;
	CursorInfo      cursorinfo;

	SM_MeasureString(s1,&w1,&h);
	SM_MeasureString(s2,&w2,&h);
	if (s3>=0)
		SM_MeasureString(s3,&w3,&h);
	else
		w3 = 0;
	w = (w1 > w2)? ((w1 > w3)? w1 : w3) : ((w2 > w3)? w2 : w3);
	w += 7;
	sh = h;
	h *= s3>=0? 5 : 4;

	USL_DialogSetup(w,h,&x,&y);

	fontcolor = HiliteColor;
	px = x + ((w - w1) / 2);
	py = y + sh + 1;
	SM_DrawString(s1);
	py += (sh * 2) - 1;

	VWB_Hlin(x + 3,x + w - 3,py,NohiliteColor);
	py += 2;

	fontcolor = NohiliteColor;
	px = x + ((w - w2) / 2);
	SM_DrawString(s2);
	py += sh;

	if (s3>=0)
	{
		px = x + ((w - w3) / 2);
		SM_DrawString(s3);
	}
	
	// FITF hack: display additional hint message during PayToSave dialog:
	if (s1 == S_SAVINGWILLCOSTALIFE)
	{
		SM_MeasureMultiline(S_HINTDISABLECONFIRM, &WindowW, &WindowH);
		WindowW += 6;
		WindowH += 2;
		WindowX = CtlPanelSX + ((int)(CtlPanelW - WindowW) / 2);
		WindowY = CtlPanelSY + CtlPanelH-4 - WindowH;
		VWB_Bar(WindowX-2, WindowY-2, WindowW+3, WindowH+3, HiliteColor);
		VWB_Bar(WindowX-1, WindowY-1, WindowW+1, WindowH+1, BackColor);
//		PrintX = WindowX+1;
		PrintY = WindowY+1;
		SM_CPrint(S_HINTDISABLECONFIRM);
	}
	// end of hack

	VW_UpdateScreen();

	IN_ClearKeysDown();
	for(;;)
	{
		IN_ReadCursor(&cursorinfo);
		if (cursorinfo.button0)
		{
			c = sc_Y;
			break;
		}
		else if (cursorinfo.button1)
		{
			c = sc_Escape;
			break;
		}
		else
		{
			c = LastScan;
			if (c == sc_Y || c == sc_Escape)
				break;
			if (c != sc_None)
			{
				if (!strict)
					break;
				SD_PlaySound(NOWAYSND);
				IN_ClearKeysDown();
			}
		}
	}
	do
	{
		IN_ReadCursor(&cursorinfo);
	} while (cursorinfo.button0 || cursorinfo.button1);

	IN_ClearKeysDown();
	USL_DrawCtlPanel();
	return(c == sc_Y);
}

static boolean near pascal
USL_Notify(stringnames s1, stringnames s2)
{
	return USL_CtlDialog(s1,s2,-1,false);
}

static boolean near pascal
USL_ConfirmComm(UComm comm)
{
	boolean confirm,dialog;
	stringnames    s1,s2,s3;

	if (!comm)
		SM_Quit(S_EMPTYCOMM);

	confirm = true;
	dialog = false;
	s3 = S_ESCTOBACKOUT;
	switch (comm)
	{
	case uc_Abort:
		s1 = S_REALLYENDCURRENTGAME;
		s2 = S_PRESSYTOENDIT;
		if (ingame && GameIsDirty)
			dialog = true;
		break;
	case uc_Quit:
		s1 = S_REALLYQUIT;
		s2 = S_PRESSYTOQUIT;
		dialog = true;
		break;
	case uc_Loaded:
		s1 = S_YOUREINAGAME;
		s2 = S_PRESSYTOLOADGAME;
		if (ingame && GameIsDirty)
			dialog = true;
		break;
	case uc_PayToSave:	// K1n9_Duk3 addition
		s1 = S_SAVINGWILLCOSTALIFE;
		s2 = S_PRESSYTOSAVE;
		dialog = true;
		break;
	case uc_SEasy:
	case uc_SNormal:
	case uc_SHard:
		s1 = S_YOUREINAGAME;
		s2 = S_PRESSYFORNEWGAME;
		if (ingame && GameIsDirty)
			dialog = true;
		break;
	case uc_Help:	// K1n9_Duk3 addition
		HelpScreens();
		USL_DrawCtlPanel();
		return false;
	}

	confirm = dialog? USL_CtlDialog(s1,s2,s3,true) : true;
	if (confirm)
	{
		Communication = comm;
		CtlPanelDone = true;
	}
	return(confirm);
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_HandleError() - Handles telling the user that there's been an error
//
///////////////////////////////////////////////////////////////////////////
static void near pascal
USL_HandleError(int num)
{
	stringnames msg = S_ERROR0+num;

	if ((unsigned)num >= (unsigned)(S_ERRORDISKFULL-S_ERROR0))
		msg = S_ERROR0;
	else if (num == ENOMEM)
		msg = S_ERRORDISKFULL;
	else if (num == EINVFMT)
		msg = S_ERRORINCOMPLETE;

#ifdef USECURSOR
	VW_HideCursor();
#endif

	USL_Notify(msg,S_PRESSANYKEY);
}

//      Custom routines
static void
USL_SetOptionsText(void)
{
	// disable or enable the "FIRE" button entry in the keyboard buttons menu
	// depending on whether two-button firing is disabled or enabled:
	keybi[2].flags &= ~ui_Disabled;
	if (oldshooting)
		keybi[2].flags |= ui_Disabled;
}

#pragma argsused
static boolean
USL_OptionsCustom(UserCall call,UserItem far *item)
{
	extern word latchpel;

	int i = item-optionsi;
	stringnames onoff = -1;
	unsigned w, h;

	switch (call)
	{
	case uic_Draw:
		VWB_Bar(CtlPanelSX + 1,item->y,
				CtlPanelEX - CtlPanelSX - 1,8,BackColor);       // Clear out background
		USL_DrawItemIcon(item);
		if ((item->flags & ui_Selected) && !(item->flags & ui_Disabled))
			fontcolor = HiliteColor;
		else
			fontcolor = NohiliteColor;
		px = item->x + 8;
		py = item->y + 1;
		SM_DrawString(item->text);
		px = CtlPanelEX - 2;
		switch (i)
		{
		case 0:
			onoff = showscorebox? S_ON : S_OFF;
			break;
		case 1:
			onoff = compatability? S_ON : S_OFF;
			break;
		case 2:
			onoff = latchpel==0? S_ON : S_OFF;
			break;
		case 3:
			onoff = oldshooting? S_ON : S_OFF;
			break;
		case 4:
			onoff = S_FRAMERATE1-MINTICS+mintics;
			break;
		case 5:
			onoff = (config.cpflags & CPF_SkipPayConfirm)? S_AUTO : S_CONFIRM;
			break;
		case 6:
			onoff = (config.cpflags & CPF_SkipQuickLoadConfirm)? S_AUTO : S_CONFIRM;
			break;
		case 7:
			onoff = (config.cpflags & CPF_SkipQuickSaveConfirm)? S_AUTO : S_CONFIRM;
			break;
		}
		SM_MeasureString(onoff, &w, &h);
		px -= w;
		SM_DrawString(onoff);
		fontcolor = F_BLACK;
		return true;
		
	case uic_Hit:
		switch (i)
		{
		case 0:
			showscorebox ^= true;
			break;
		case 1:
			compatability ^= true;
			break;
		case 2:
			latchpel ^= true;
			break;
		case 3:
			oldshooting ^= true;
			break;
		case 4:
			if (++mintics > MAXTICS)
				mintics = MINTICS;
			break;
		case 5:
			config.cpflags ^= CPF_SkipPayConfirm;
			break;
		case 6:
			config.cpflags ^= CPF_SkipQuickLoadConfirm;
			break;
		case 7:
			config.cpflags ^= CPF_SkipQuickSaveConfirm;
			break;
		}
		USL_OptionsCustom(uic_Draw, item);
		USL_SetOptionsText();
		return true;
	}
	return false;
}

#pragma argsused
static boolean
USL_ConfigCustom(UserCall call,UserItem far *item)
{
	word	w,h,tw;
	
	if (call == uic_TouchupCard)
	{
		SM_MeasureString(S_CONTROL,&w,&h);
		tw = w;
		SM_MeasureString(S_CONTROLNAME0+Controls[0],&w,&h);
		tw += w;
		py = CtlPanelEY - 18 - h;
		px = CtlPanelSX + ((CtlPanelW - tw) / 2);
		fontcolor = NohiliteColor;
		SM_DrawString(S_CONTROL);
		SM_DrawString(S_CONTROLNAME0+Controls[0]);
	}
	return(false);
}

#define KEYBOX_XOFF (90+22)
#define KEYBOX_WIDTH (40)
#define KEYBOX_HEIGHT (8)

static void near pascal
USL_CKSetKey(UserItem far *item,word i)
{
	boolean         on;
	word            j;
	ScanCode        scan;
	longword        time;
	CursorInfo      cursorinfo;

	on = false;
	time = 0;
	LastScan = sc_None;
	fontcolor = HiliteColor;
	do
	{
		if (TimeCount >= time)
		{
			on ^= true;
			VWB_Bar(item->x + KEYBOX_XOFF,item->y,KEYBOX_WIDTH,KEYBOX_HEIGHT,fontcolor ^ BackColor);
			VWB_Bar(item->x + KEYBOX_XOFF + 1,item->y + 1,KEYBOX_WIDTH - 2,KEYBOX_HEIGHT - 2,BackColor);
			if (on)
				VWB_DrawTile8(item->x + KEYBOX_XOFF + 16,item->y,TileBase + 8);
			VW_UpdateScreen();

			time = TimeCount + (TickBase / 2);
		}

		IN_ReadCursor(&cursorinfo);
		while (cursorinfo.button0 || cursorinfo.button1)
		{
			IN_ReadCursor(&cursorinfo);
			LastScan = sc_Escape;
		}

#if 0	// this hack is no longer needed, keyboard handler now skips LShift prefixes	--K1n9_Duk3
	asm     pushf
	asm     cli
		if (LastScan == sc_LShift)
			LastScan = sc_None;
	asm     popf
#endif
	} while (!(scan = LastScan));

	if (scan != sc_Escape)
	{
		for (j = 0,on = false;j < sizeof(KeyMaps)/sizeof(KeyMaps[0]);j++)
		{
			if (j == i)
				continue;
			if (*(KeyMaps[j]) == scan)
			{
				on = true;
				break;
			}
		}
		if (on)
			USL_Notify(S_KEYALREADYUSED,S_PRESSAKEY);
		else
			*(KeyMaps[i]) = scan;
	}
	IN_ClearKeysDown();
}

#pragma argsused
static boolean
USL_KeySCustom(UserCall call,UserItem far *item)
{
	if (call == uic_SetupCard)
		Controls[0] = ctrl_Keyboard;
	return(false);
}

#pragma argsused
static boolean
USL_KeyCustom(UserCall call,UserItem far *item)
{
	boolean result;
	word    i;

	result = false;
	
	if (topcard == &keygroup)
		i = item - keyi;
	else if (topcard == &keybgroup)
		i = (item - keybi) + 8;
	else
		i = (item - keyfbi) + 8 + 4;
	
	switch (call)
	{
	case uic_Draw:
		VWB_Bar(CtlPanelSX + 1,item->y,
				CtlPanelEX - CtlPanelSX - 1,8,BackColor);       // Clear out background
		USL_DrawItemIcon(item);
		fontcolor = (item->flags & ui_Selected)? HiliteColor : NohiliteColor;
		px = item->x + 8;
		py = item->y + 1;
		SM_DrawString(item->text);
		VWB_Bar(item->x + KEYBOX_XOFF,item->y,KEYBOX_WIDTH,KEYBOX_HEIGHT,fontcolor ^ BackColor);
		VWB_Bar(item->x + KEYBOX_XOFF + 1,item->y + 1,KEYBOX_WIDTH - 2,KEYBOX_HEIGHT - 2,BackColor);
		px = item->x + KEYBOX_XOFF + 6;
		py = item->y + 1;
		SM_DrawString(IN_GetScanName(*KeyMaps[i]));
		result = true;
		break;
	case uic_Hit:
		USL_KeyCustom(uic_Draw,item);
		USL_CKSetKey(item,i);
		USL_DrawCtlPanel();
		result = true;
		break;
	}
	return(result);
}

static void near pascal
USL_CJDraw(stringnames s1,stringnames s2)
{
	word    w,h;

	SM_MeasureString(s1,&w,&h);
	px = CtlPanelSX + ((CtlPanelW - w) / 2);
	py = CtlPanelEY - 34;
	VWB_Bar(CtlPanelSX + 1,py,CtlPanelW - 2,h * 2,BackColor);
	fontcolor = HiliteColor;
	SM_DrawString(s1);
	py += h;
	SM_MeasureString(s2,&w,&h);
	px = CtlPanelSX + ((CtlPanelW - w) / 2);
	SM_DrawString(s2);
}

static boolean near pascal
USL_CJGet(word joy,word button,word x,word y,word *xaxis,word *yaxis)
{
	boolean         on;
	longword        time;

	while (IN_GetJoyButtonsDB(joy))
		if (LastScan == sc_Escape)
			return(false);

	on = false;
	time = 0;
	while (!(IN_GetJoyButtonsDB(joy) & (1 << button)))
	{
		if (TimeCount >= time)
		{
			on ^= true;
			time = TimeCount + (TickBase / 2);
			VWB_DrawTile8(x,y,TileBase + on);
			VW_UpdateScreen();
		}

		if (LastScan == sc_Escape)
			return(false);
	}
	IN_GetJoyAbs(joy,xaxis,yaxis);
	return(true);
}

static boolean near pascal
USL_ConfigJoystick(word joy)
{
	word    x,y,
			minx,miny,
			maxx,maxy;

	BottomS1 = BottomS2 = -1;
	BottomS3 = S_ESCBACK;
	USL_DrawCtlPanel();
	x = CtlPanelSX + 60;
	y = CtlPanelSY + 19;
	VWB_DrawPic(x,y,CP_JOYSTICKPIC);

	USL_CJDraw(S_CALIBMIN1,S_CALIBMIN2/*"Move Joystick to upper left","and press button #1"*/);
	VWB_DrawTile8(x + 24,y + 8,TileBase + 6);
	VWB_DrawTile8(x + 8,y + 8,TileBase + 1);
	VWB_DrawTile8(x + 8,y + 24,TileBase + 0);
	VW_UpdateScreen();
	if (!USL_CJGet(joy,0,x + 8,y + 8,&minx,&miny))
		return(false);

	USL_CJDraw(S_CALIBMAX1,S_CALIBMAX2/*"Move Joystick to lower right","and press button #2"*/);
	//VWB_DrawTile8(x + 24,y + 8,TileBase - 25);
	VWB_Bar((x + 24)&~7, y + 8, 8, 8, 8);	// K1n9_Duk3 mod: we're getting rid of the other tile
	VWB_DrawTile8(x + 40,y + 24,TileBase + 7);
	VWB_DrawTile8(x + 8,y + 8,TileBase + 0);
	VWB_DrawTile8(x + 8,y + 24,TileBase + 1);
	VW_UpdateScreen();
	if (!USL_CJGet(joy,1,x + 8,y + 24,&maxx,&maxy))
		return(false);

	while (IN_GetJoyButtonsDB(joy))
		;

//	if (minx != maxx && miny != maxy)	// K1n9_Duk3 mod: added this check (it's also present in Keen 4-6 v1.4)
	if (maxx >= minx+2 && maxy >= miny+2)	// K1n9_Duk3 fix: this should avoid divide by 0 errors (the other check wasn't enough)
	{
		IN_SetupJoy(joy,minx,maxx,miny,maxy);
		return(true);
	}
	USL_Notify(S_CALIBFAILED,S_PRESSANYKEY);	// K1n9_Duk3 addition
	return(false);
}

#pragma argsused
static boolean
USL_Joy1Custom(UserCall call,UserItem far *item)
{
	if (call == uic_SetupCard)
	{
		if (USL_ConfigJoystick(0))
		{
			Controls[0] = ctrl_Joystick1;
			USL_Notify(S_USINGJOY1,S_PRESSANYKEY);
		}
		return(true);
	}
	else
		return(false);
}

#pragma argsused
static boolean
USL_Joy2Custom(UserCall call,UserItem far *item)
{
	if (call == uic_SetupCard)
	{
		if (USL_ConfigJoystick(1))
		{
			Controls[0] = ctrl_Joystick2;
			USL_Notify(S_USINGJOY2,S_PRESSANYKEY);
		}
		return(true);
	}
	else
		return(false);
}

static void near
USL_DrawFileIcon(UserItem far *item)
{
	word    color;

	item->y = topcard->y + CtlPanelSY + 12;
	item->y += (item - loadsavegamei) * 11;

	fontcolor = (item->flags & ui_Selected)? HiliteColor : NohiliteColor;
	color = fontcolor ^ BackColor;  // Blech!
	VWB_Hlin(item->x,item->x + (CtlPanelW - 12),item->y,color);
	VWB_Hlin(item->x,item->x + (CtlPanelW - 12),item->y + 9,color);
	VWB_Vlin(item->y,item->y + 9,item->x,color);
	VWB_Vlin(item->y,item->y + 9,item->x + (CtlPanelW - 12),color);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_LoadGame() - Loads a saved game from the given slot number
//              returns error number, or 0 for success
//
///////////////////////////////////////////////////////////////////////////
static word near pascal
US_LoadGame(word num)
{
	char *filename;
	word err;
	int file;
	SaveGame *game;
	
	if (num >= MaxSaveGames)
		return -1;
	
	game = &Games[num];
	err = 0;
	filename = USL_GiveSaveName(num);
	if ((file = open(filename,O_BINARY | O_RDONLY)) != -1)
	{
		if (read(file,game,sizeof(*game)) == sizeof(*game))
		{
			if (USL_LoadGame)
				if (!USL_LoadGame(file))
				{
					err = errno;
					if (!err)
						err = EINVFMT;	// report "File is Incomplete"
				}
				else
				{
					loadedgame = true;
				}
		}
		else
			err = errno;
		close(file);
	}
	else
	{
		err = errno;
	}
	
	if (err)
	{
		// reset game state (avoid adding highscore entry for corrupted game state)
		if (USL_ResetGame)
			USL_ResetGame();
		abortgame = true;
	}
	
	return err;
}

///////////////////////////////////////////////////////////////////////////
//
//      US_SaveGame() - Saves a game to the given slot number under the given title
//              returns error number, or 0 for success
//
///////////////////////////////////////////////////////////////////////////
static word near pascal
US_SaveGame(word num, char *title)
{
	char *filename;
	word err;
	int file;
	SaveGame *game;
	
	if (num >= MaxSaveGames)
		return -1;
	
	game = &Games[num];
	if (title != game->name)
		strcpy(game->name, title);
	game->oldtest = &PrintX;
	game->present = true;
	err = 0;
	filename = USL_GiveSaveName(num);
	file = open(filename,O_CREAT | O_BINARY | O_WRONLY | O_TRUNC,	// K1n9_Duk3 mod: added O_TRUNC
				S_IREAD | S_IWRITE | S_IFREG);
	if (file != -1)
	{
		if (write(file,game,sizeof(*game)) == sizeof(*game))
		{
			if (USL_SaveGame)
				if (!USL_SaveGame(file))
				{
					err = errno;
					if (!err)
						err = ENOMEM;	// report "Disk is Full"
				}
				else
				{
					game->present = true;
				}
		}
		else
			err = (errno == ENOENT)? ENOMEM : errno;
		close(file);
		
		if (err)
		{
			game->present = false;
			remove(filename);
		}
	}
	else
		err = (errno == ENOENT)? ENOMEM : errno;
	
	return err;
}

///////////////////////////////////////////////////////////////////////////
//
//      US_TimeString() - Generates a string for the current date and time
//           format is "YYYY-MM-DD hh:mm:ss", buffer must be at least 20 bytes
//
///////////////////////////////////////////////////////////////////////////
static void near pascal
US_TimeString(char *string)
{
	int year;
	byte month, day, hour, minute, second;
	register unsigned char *s;

	_AH = 0x2A;
	geninterrupt(0x21);
	year = _CX;
	month = _DH;
	day = _DL;

	_AH = 0x2C;
	geninterrupt(0x21);
	hour = _CH;
	minute = _CL;
	second = _DH;

	s = string;
	itoa(year, s, 10);
	while (*s) s++;
	*s++ = '-';
	if (month < 10) *s++ = '0';
	itoa(month, s, 10);
	while (*s) s++;
	*s++ = '-';
	if (day < 10) *s++ = '0';
	itoa(day, s, 10);
	while (*s) s++;
	*s++ = ' ';
	if (hour < 10) *s++ = '0';
	itoa(hour, s, 10);
	while (*s) s++;
	*s++ = ':';
	if (minute < 10)	*s++ = '0';
	itoa(minute, s, 10);
	while (*s) s++;
	*s++ = ':';
	if (second < 10)	*s++ = '0';
	itoa(second, s, 10);
}

static void near
USL_DoLoadGame(UserItem far *item)
{
	char            *filename;
	word            n,
				err;
	int                     file;
	SaveGame        *game;

	if (!USL_ConfirmComm(uc_Loaded))
		return;

	n = item - loadsavegamei;
	game = &Games[n];

	USL_ShowLoadSave(S_LOADING,game->name);

	err = US_LoadGame(n);
	if (err)
	{
		USL_HandleError(err);
		Communication = uc_None;
		CtlPanelDone = true;	// return to demo loop (to avoid menu bugs)
	}
	else
	{
		QuickSaveSlot = n;	// K1n9_Duk3 addition
		Paused = true;
	}
	// we will always exit from the control panel here, so no need to redraw it
}

static boolean
USL_LoadCustom(UserCall call,UserItem far *item)
{
	boolean result;
	word    i;

	result = false;
	switch (call)
	{
	case uic_SetupCard:
		for (i = 0;i < MaxSaveGames;i++)
		{
			if (Games[i].present)
				loadsavegamei[i].flags &= ~ui_Disabled;
			else
				loadsavegamei[i].flags |= ui_Disabled;
		}
		break;
	case uic_DrawIcon:
		USL_DrawFileIcon(item);
		result = true;
		break;
	case uic_Draw:
		USL_DrawFileIcon(item);
		VWB_Bar(item->x + 1,item->y + 2,CtlPanelW - 12 - 2,7,BackColor);

		i = item - loadsavegamei;
		if (Games[i].present)
			px = item->x + 2;
		else
			px = item->x + 60;
		py = item->y + 2;
		USL_DrawString(Games[i].present? Games[i].name : SM_GetString(S_EMPTY));
		result = true;
		break;
	case uic_Hit:
		USL_DoLoadGame(item);
		result = true;
		break;
	}
	return(result);
}

static void near
USL_DoSaveGame(UserItem far *item)
{
	boolean         ok;
	char            *filename;
	word            n,err;
	int         file;
	SaveGame        *game;

	// K1n9_Duk3 addition
	if (SavingWillCost() && !(config.cpflags & CPF_SkipPayConfirm))
	{
		if (!USL_ConfirmComm(uc_PayToSave))
			return;

		// optional: don't immediately return to game after saving
		Communication = uc_None;
		CtlPanelDone = false;
	}
	// end of addition

	BottomS1 = S_TYPENAME;
	BottomS2 = S_ENTERACCEPTS;
	USL_DrawCtlPanel();

	n = item - loadsavegamei;
	game = &Games[n];
	fontcolor = HiliteColor;
	VWB_Bar(item->x + 1,item->y + 2,CtlPanelW - 12 - 2,7,BackColor);
	ok = US_LineInput(item->x + 2,item->y + 2,
						game->name,(game->present && !game->timestamped)? game->name : nil,
						true,MaxGameName,
						CtlPanelW - 13);
	
	if (ok)
	{
		if (!strlen(game->name))
		{
			// entered no name -- assign time stamp as name:
			US_TimeString(game->name);
			game->timestamped = true;
		}
		else
		{
			// entered a name -- name is definitely not a time stamp anymore:
			game->timestamped = false;
		}
		
		USL_ShowLoadSave(S_SAVING,game->name);

		err = US_SaveGame(n, game->name);
		if (err)
		{
			USL_HandleError(err);
		}
		else
		{
			// K1n9_Duk3 addition
			QuickSaveSlot = n;
			if (menutype != mt_Normal)
				USL_ConfirmComm(uc_Return);
			// end of addition
			GameIsDirty = false;
		}
	}

	// K1n9_Duk3 addition: exit from save game menu when saving is no longer allowed
	if (menutype == mt_Normal && !SavingAllowed())
	{
		rooti[2].flags |= ui_Disabled;  // Save Game
		USL_UpLevel();
	}
	// end of addition

	USL_SetupCard();

}

static boolean
USL_SaveCustom(UserCall call,UserItem far *item)
{
	word    i;

	switch (call)
	{
	case uic_SetupCard:
		for (i = 0;i < MaxSaveGames;i++)
			loadsavegamei[i].flags &= ~ui_Disabled;
		return(false);
	case uic_Hit:
		USL_DoSaveGame(item);
		return(true);
//              break;
	}
	return(USL_LoadCustom(call,item));
}

#if 0

#define PaddleMinX      (CtlPanelSX + 3)
#define PaddleMaxX      (CtlPanelEX - 15)
#define BallMinX        (CtlPanelSX + 2)
#define BallMinY        (CtlPanelSY + 12 + 2)
#define BallMaxX        (CtlPanelEX - 6)
#define BallMaxY        (CtlPanelEY - 13)
#define CPaddleY        (BallMinY + 4)
#define KPaddleY        (BallMaxY - 2)
void
USL_DrawPongScore(word k,word c)
{
	fontcolor = HiliteColor;
	PrintY = py = CtlPanelSY + 4;
	px = CtlPanelSX + 6;
	VWB_Bar(px,py,42,6,BackColor);
	SM_DrawString(S_PWKEEN);
	PrintX = px;
	US_PrintUnsigned(k);
	px = CtlPanelSX + 108;
	VWB_Bar(px,py,50,6,BackColor);
	SM_DrawString(S_PWCOMP);
	PrintX = px;
	US_PrintUnsigned(c);
}

void
USL_PlayPong(void)
{
	boolean         ball,killball,revdir,done,lastscore;
	word            cycle,
				x,y,
				kx,cx,
				rx,
				bx,by,
				kscore,cscore,
				speedup;
	int                     bdx,bdy;
	longword        balltime,waittime;
	CursorInfo      cursorinfo;

	kx = cx = PaddleMinX + ((PaddleMaxX - PaddleMinX) / 2);
	bx = by = bdx = bdy = 0;
	kscore = cscore = 0;
	USL_DrawPongScore(0,0);
	cycle = 0;
	revdir = false;
	killball = true;
	done = false;
	lastscore = false;
	do
	{
		waittime = TimeCount;

		IN_ReadCursor(&cursorinfo);
		if (((cursorinfo.x < 0) || IN_KeyDown(sc_LeftArrow)) && (kx > PaddleMinX))
			kx -= 2;
		else if (((cursorinfo.x > 0) || IN_KeyDown(sc_RightArrow)) && (kx < PaddleMaxX))
			kx += 2;

		if (killball)
		{
			ball = false;
			balltime = TimeCount + TickBase;
			speedup = 10;
			killball = false;
		}

		if (ball && (cycle++ % 3))
		{
			x = (bx >> 2);
			if (!(x & 1))
				x += (US_RndT() & 1);

			if ((cx + 6 < x) && (cx < PaddleMaxX))
				cx += 1;
			else if ((cx + 6 > x) && (cx > PaddleMinX))
				cx -= 1;
		}

		VWB_Bar(BallMinX,BallMinY - 1,
				BallMaxX - BallMinX + 5,BallMaxY - BallMinY + 7,
				BackColor);
		VWB_DrawSprite(cx,CPaddleY,PADDLESPR);
		VWB_DrawSprite(kx,KPaddleY,PADDLESPR);
		if (ball)
		{
			if
			(
				(((bx + bdx) >> 2) > BallMaxX)
			||      (((bx + bdx) >> 2) < BallMinX)
			)
			{
				SD_PlaySound(BALLBOUNCESND);
				bdx = -bdx;
			}
			bx += bdx;

			if (((by + bdy) >> 2) > BallMaxY)
			{
				killball = true;
				lastscore = false;
				cscore++;
				SD_PlaySound(COMPSCOREDSND);
				USL_DrawPongScore(kscore,cscore);
				if (cscore == 21)
				{
					USL_Notify(S_PWLOST,S_PRESSAKEY);
					done = true;
					continue;
				}
			}
			else if (((by + bdy) >> 2) < BallMinY)
			{
				killball = true;
				lastscore = true;
				kscore++;
				SD_PlaySound(KEENSCOREDSND);
				USL_DrawPongScore(kscore,cscore);
				if (kscore == 21)
				{
					USL_Notify(S_PWWON,S_PRESSAKEY);
					done = true;
					continue;
				}
			}
			by += bdy;

			x = bx >> 2;
			y = by >> 2;
			if (!killball)
			{
				if
				(
					(bdy < 0)
				&&      ((y >= CPaddleY) && (y < CPaddleY + 3))
				&&      ((x >= (cx - 5)) && (x < (cx + 11)))
				)
				{
					rx = cx;
					revdir = true;
					SD_PlaySound(COMPPADDLESND);
				}
				else if
				(
					(bdy > 0)
				&&      ((y >= (KPaddleY - 3)) && (y < KPaddleY))
				&&      ((x >= (kx - 5)) && (x < (kx + 11)))
				)
				{
					if (((bdy >> 2) < 3) && !(--speedup))
					{
						bdy++;
						speedup = 10;
					}
					rx = kx;
					revdir = true;
					SD_PlaySound(KEENPADDLESND);
				}
				if (revdir)
				{
					bdy = -bdy;
					bdx = ((x + 5 - rx) >> 1) - (1 << 2);
					if (!bdx)
						bdx--;
					revdir = false;
				}
			}
#ifdef NEWMODS
			VWB_DrawSprite(x,y, BALLSPR);	// K1n9_Duk3 fix: we don't need BALL1PIXELTOTHERIGHTSPR anymore
#else
			VWB_DrawSprite(x,y,(x & 1)? BALL1PIXELTOTHERIGHTSPR : BALLSPR);
#endif
		}
		else if (TimeCount >= balltime)
		{
			ball = true;
			bdx = 1 - (US_RndT() % 3);
			bdy = 3; // Keen Dreams patch
			if (lastscore)
				bdy = -bdy;
			bx = (BallMinX + ((BallMaxX - BallMinX) / 2)) << 2;
			by = (BallMinY + ((BallMaxY - BallMinY) / 2)) << 2;
		}
		VW_UpdateScreen();
		while (waittime == TimeCount)
			;       // DEBUG - do adaptiveness
	} while ((LastScan != sc_Escape) && !done);
	IN_ClearKeysDown();
}

#pragma argsused
static boolean
USL_PongCustom(UserCall call,struct UserItem far *item)
{
	if (call != uic_SetupCard)
		return(false);

	VWB_DrawPic(0,0,CP_MENUSCREENPIC);
	VWB_DrawPic(CtlPanelSX + 56,CtlPanelSY,CP_PADDLEWARPIC);
	VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,CtlPanelSY + 12,HiliteColor ^ BackColor);
	VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,CtlPanelEY - 7,HiliteColor ^ BackColor);
	USL_PlayPong();

	return(true);
}

unsigned bestscore;
void PlayMinigame(void){}

#else

#include "MINIGAME.C"

#pragma argsused
static boolean
USL_PongCustom(UserCall call,struct UserItem far *item)
{
	if (call != uic_SetupCard)
		return(false);

	VWB_DrawPic(0,0,CP_MENUSCREENPIC);
	VWB_DrawPic(CtlPanelSX + 56,CtlPanelSY,CP_PADDLEWARPIC);
	//VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,CtlPanelSY + 12,HiliteColor ^ BackColor);
	//VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,CtlPanelEY - 7,HiliteColor ^ BackColor);
	PlayMinigame();

	return(true);
}
#endif

//      Flag management stuff
static void near
USL_ClearFlags(UserItemGroup far *node)
{
	UserItem        far *i;

	if (!node->items)
		return;

	for (i = node->items;i->type != uii_Bad;i++)
	{
		i->flags &= ~UISelectFlags;
		if (i->child)
			USL_ClearFlags((UserItemGroup far *)i->child);
	}
}

static int near
USL_FindPushedItem(UserItemGroup far *group)
{
	word            i;
	UserItem        far *item;

	for (item = group->items,i = 0;item->type != uii_Bad;item++,i++)
		if (item->flags & ui_Pushed)
			return(i);
	return(-1);
}

static void near
USL_SelectItem(UserItemGroup far *group,word index,boolean draw)
{
	UserItem        far *item;

	if (index != group->cursor)
	{
		item = &group->items[group->cursor];
		item->flags &= ~ui_Selected;
		if (draw)
			USL_DrawItem(item);
	}

	group->cursor = index;
	item = &group->items[group->cursor];
	group->items[group->cursor].flags |= ui_Selected;
	if (draw)
		USL_DrawItem(item);
}

static void near
garg(UserItemGroup far *group,word index,boolean draw)
{
	word            i;
	UserItem        far *item;

	USL_SelectItem(group,index,draw);
	for (item = group->items,i = 0;item->type != uii_Bad;item++,i++)
	{
		if (item->type != uii_RadioButton)
			continue;

		if (i == index)
		{
			item->flags |= ui_Pushed;
			if (draw)
				USL_DrawItem(item);
		}
		else if (item->flags & ui_Pushed)
		{
			item->flags &= ~ui_Pushed;
			if (draw)
				USL_DrawItem(item);
		}
	}
}

static void near
USL_NextItem(void)
{
	if (topcard->items[topcard->cursor + 1].type == uii_Bad)
		return;
	USL_SelectItem(topcard,topcard->cursor + 1,true);
}

static void near
USL_PrevItem(void)
{
	if (!topcard->cursor)
		return;
	USL_SelectItem(topcard,topcard->cursor - 1,true);
}

static void near
USL_SetupCard(void)
{
	BottomS1 = S_ARROWSMOVE;
	BottomS2 = S_ENTERSELECTS;
	BottomS3 = (cstackptr || menutype != mt_Normal)? S_ESCBACK : S_ESCQUIT;	// K1n9_Duk3 mod: added "|| menutype != mt_Normal"

	USL_SelectItem(topcard,topcard->cursor,false);
	USL_DrawCtlPanel();     // Contents?
}

static void near
USL_DownLevel(UserItemGroup far *group)
{
	if (!group)
		SM_Quit(S_NILCARD);
	
	USL_PushCard(group);
	if (group->custom && group->custom(uic_SetupCard,nil))
		USL_PopCard();
	USL_SetupCard();
}

static void near
USL_UpLevel(void)
{
	if (!cstackptr)
	{
		// K1n9_Duk3 mod: normal menu quits to DOS, other menus return to game
		if (menutype == mt_Normal)
			USL_ConfirmComm(uc_Quit);
		else
			USL_ConfirmComm(uc_Return);
		return;
	}

	if (topcard->items)
		topcard->items[topcard->cursor].flags &= ~ui_Selected;
	USL_PopCard();
	USL_SetupCard();
}

static void near
USL_DoItem(void)
{
	// DEBUG - finish this routine
	UserItem                far *item;

	item = &topcard->items[topcard->cursor];
	if (item->flags & ui_Disabled)
		SD_PlaySound(NOWAYSND);
	else
	{
		switch (item->type)
		{
		case uii_Button:
			if (!(topcard->custom && topcard->custom(uic_Hit,item)))
				USL_ConfirmComm(item->comm);
			break;
		case uii_RadioButton:
			garg(topcard,topcard->cursor,true);
			break;
		case uii_Folder:
			USL_DownLevel(item->child);
			break;
		}
	}
}

static void near
USL_SetControlValues(void)
{
	garg(&soundgroup,SoundMode,false);
	garg(&musicgroup,MusicMode,false);
	if (!AdLibPresent)
	{
		soundi[2].flags |= ui_Disabled; // AdLib sound effects
		musici[1].flags |= ui_Disabled; // AdLib music
	}

	if (!JoysPresent[0])
		configi[4].flags |= ui_Disabled; // OPTIONS is back for Keen Dreams
	if (!JoysPresent[1])
		configi[5].flags |= ui_Disabled; // OPTIONS is back for Keen Dreams

	rooti[3].text = ingame? S_RETURNTOGAME : S_RETURNTODEMO;	// K1n9_Duk3 mod (changed menu order)
	rooti[4].text = ingame? S_ENDGAME : S_VIEWSCORES;
	rooti[4].hotkey = ingame? sc_E : sc_V;
	rooti[4].comm = ingame? uc_Abort : uc_ViewScores;
	if (!ingame)
	{
		rooti[2].flags |= ui_Disabled;  // Save Game
		//rooti[4/*5*/].flags |= ui_Disabled;  // End Game	// K1n9_Duk3 mod (changed menu order)
	}
	else if (!SavingAllowed())	// K1n9_Duk3 addition
	{
		rooti[2].flags |= ui_Disabled;  // Save Game
	}
	// end of addition
	rootgroup.cursor = ingame? 3 : 0;	// K1n9_Duk3 mod (changed menu order)
	USL_SetOptionsText();
	// DEBUG - write the rest of this
		// assign random joke key name
	keybi[3].text = rand() % (S_JOKEKEYNAMEEND + 1 - S_JOKEKEYNAME1) + S_JOKEKEYNAME1;
	keybi[3].flags |= ui_Disabled;	// joke key entry is always disabled
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_SetUpCtlPanel() - Sets the states of the UserItems to reflect the
//              values of all the appropriate variables
//
///////////////////////////////////////////////////////////////////////////
static void near
USL_SetUpCtlPanel(void)
{
	int     i;

	// Cache in all of the stuff for the control panel
	CA_UpLevel();
	CA_ClearMarks();	// K1n9_Duk3 addition
	for (i = CONTROLS_LUMP_START;i <= CONTROLS_LUMP_END;i++)
		CA_MarkGrChunk(i);
	for (i = PADDLE_LUMP_START;i <= PADDLE_LUMP_END;i++)
		CA_MarkGrChunk(i);
	CA_MarkGrChunk(STARTFONT+1);            // Little font
	CA_MarkGrChunk(CP_MENUMASKPICM);        // Mask for dialogs
	CA_CacheMarks(SM_GetString(S_CONTROLPANEL));
	CA_LoadAllSounds();

	// Do some other setup
	fontnumber = 1;
	US_SetPrintRoutines(VW_MeasurePropString,VWB_DrawPropString);
	fontcolor = F_BLACK;
	//VW_Bar (0,0,320,200,3); // CAT3D patch
	// K1n9_Duk3 addition:
	VW_ClearVideo(GREEN);
	//VW_FadeOut();
	{
		void CK_DefaultPalette(void);
		CK_DefaultPalette();
	}
	// end of addition
	RF_FixOfs();
	VW_InitDoubleBuffer();

	Communication = uc_None;
	USL_ClearFlags(&rootgroup);
	USL_SetControlValues();
	USL_SetupStack();
	USL_SetupCard();

	if (ingame)
		GameIsDirty = true;

	IN_ClearKeysDown();
}

static void near
USL_HandleComm(UComm comm)
{
	switch (comm)
	{
	case uc_Loaded:
	case uc_PayToSave:	// K1n9_Duk3 addition
	case uc_Return:
		break;
	case uc_Abort:
		abortgame = true;
		break;
	case uc_ViewScores:	// K1n9_Duk3 addition
		abortgame = 2;	// ugly hack, but it works...
		break;
	case uc_Quit:
		QuitToDos = true;
		break;
	case uc_SEasy:
		restartgame = gd_Easy;
		break;
	case uc_SNormal:
		restartgame = gd_Normal;
		break;
	case uc_SHard:
		restartgame = gd_Hard;
		break;

	default:
		SM_Quit(S_UNKNOWNCOMM);
		break;
	}
}

static void near
USL_GetControlValues(void)
{
	int     i;

	// DEBUG - write the rest of this
	i = USL_FindPushedItem(&soundgroup);
	if (i != SoundMode)
		SD_SetSoundMode(i);

	i = USL_FindPushedItem(&musicgroup);
	if (i != MusicMode)
		SD_SetMusicMode(i);
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_TearDownCtlPanel() - Given the state of the control panel, sets the
//              modes and values as appropriate
//
///////////////////////////////////////////////////////////////////////////
static void near
USL_TearDownCtlPanel(void)
{
	USL_GetControlValues();
	if (Communication)
		USL_HandleComm(Communication);

	fontnumber = 0; // Normal font
	fontcolor = F_BLACK;
	if (restartgame && USL_ResetGame)
	{
		USL_ResetGame();
	}
	else if (QuitToDos)
	{
#if 0
		if (tedlevel)
			TEDDeath();
		else
#endif
		{
			US_CenterWindow(20,3);
			fontcolor = F_SECONDCOLOR;
			SM_PrintCentered(S_QUITTING);
			fontcolor = F_BLACK;
			VW_UpdateScreen();
			Quit(nil);
		}
	}

	IN_ClearKeysDown();
	SD_WaitSoundDone();
	// VW_Bar (0,0,320,200,3); // CAT3D patch
	VW_ClearVideo(BLACK);
	CA_DownLevel();
	CA_LoadAllSounds();
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_ConfirmQuick() - Asks user for confirmation for QuickSave/QuickLoad
//              returns true when confirmed, false when aborted
//
///////////////////////////////////////////////////////////////////////////

// quick compile-time checks to make sure the cpflags check works as intended:
#if (CPF_SkipQuickLoadConfirm << true) != (CPF_SkipQuickSaveConfirm)
#error CPF constants not set up correctly!
#endif
#if (CPF_SkipQuickLoadConfirm << false) != (CPF_SkipQuickLoadConfirm)
#error CPF constants not set up correctly!
#endif

static boolean near pascal
USL_ConfirmQuick(boolean save)
{
	word skipconfirm = config.cpflags & (CPF_SkipQuickLoadConfirm << save);
	boolean cost = save && SavingWillCost();
	
	//
	// if we're saving and saving will cost, then we can't skip the quicksave
	// confirmation unless the pay-to-save confirmation can be skipped as well
	//
	if (cost && skipconfirm)
		skipconfirm = config.cpflags & CPF_SkipPayConfirm;
	
	if (!skipconfirm)
	{
		stringnames sn;
		char *s;
		unsigned w, h;
		
		// draw an additional window that tells the user that the confirmation
		// dialog window can be disabled in the option menu:
		US_DrawWindow((MaxX/8 - 20)/2, MaxY/8 - 6, 20, 4);
		PrintY++;
		SM_CPrint(S_HINTDISABLECONFIRM);
		
		// the main dialog window:
		US_CenterWindow(30, 6+cost*2);	// window will be 48 or 64 pixels high
		
		// draw top of message (leave room for slot number):
		sn = S_QUICKLOADTOP+save;
		SM_MeasureString(sn, &w, &h);
		PrintX = WindowX + (int)(WindowW - w - 9)/2;
		PrintY += 2;
		SM_Print(sn);
		// draw slot number:
		US_PrintUnsigned(QuickSaveSlot+1);
		// draw save title:
		s = Games[QuickSaveSlot].name;
		USL_MeasureString(s, &w, &h);
		PrintX = WindowX + (int)(WindowW - w)/2;
		PrintY = WindowY + 20;
		US_Print(s);
		
		PrintY = WindowY + 36;
		if (cost)
		{
			PrintY += 2;
			fontcolor = LIGHTRED^WHITE;
			SM_CPrint(S_SAVINGWILLCOSTALIFE);
			fontcolor = F_BLACK;
			PrintY += 4;
		}
		
		// draw bottom of message (Y/N prompt):
		SM_CPrint(S_QUICKLOADBOT+save);
		
		VW_UpdateScreen();
		
		IN_ClearKeysDown();
		while (LastScan != sc_Y)
		{
			if (LastScan == sc_N || LastScan == sc_Escape)
			{
				IN_ClearKeysDown();
				return false;
			}
		}
		
		// clear the big window for the Loading/Saving message:
		US_ClearWindow();
		return true;
	}
	else
	{
		// just prepare a window for the Loading/Saving message:
		US_CenterWindow(10, 3);
		return true;
	}
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_QuickLoad() - handles QuickLoad
//
///////////////////////////////////////////////////////////////////////////
static void near
USL_QuickLoad(void)
{
	if (USL_ConfirmQuick(false))
	{
		SM_PrintCentered(S_LOADING);
		VW_UpdateScreen();
		
		CA_UpLevel();	// loading MUST be done a cache level above the game cache level!
		if (US_LoadGame(QuickSaveSlot) == 0)
		{
			// loading succeeded
		}
		else
		{
			// an error occured
		}
		CA_DownLevel();
		IN_ClearKeysDown();
	}
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_QuickSave() - handles QuickSave
//
///////////////////////////////////////////////////////////////////////////
static void near
USL_QuickSave(void)
{
	if (USL_ConfirmQuick(true))
	{
		SM_PrintCentered(S_SAVING);
		VW_UpdateScreen();

		CA_UpLevel();	// optional, FITF doesn't need additional buffers or cache level adjustment for saving
		if (Games[QuickSaveSlot].timestamped)
			US_TimeString(Games[QuickSaveSlot].name);
		if (US_SaveGame(QuickSaveSlot, Games[QuickSaveSlot].name) == 0)
		{
			// saving succeeded
		}
		else
		{
			// an error occured
		}
		CA_DownLevel();	// optional
		IN_ClearKeysDown();
	}
}

///////////////////////////////////////////////////////////////////////////
//
//      US_ControlPanelEx() - This is the main routine for the control panel
//
///////////////////////////////////////////////////////////////////////////
#define MoveMin 40
void
US_ControlPanelEx(MenuType type)
{
	boolean         resetitem,on;
	word            i;
	longword        flashtime;
	UserItem        far *item;
	CursorInfo      cursorinfo;

	// K1n9_Duk3 addition:
	switch (type)
	{
	case mt_QuickLoad:
		if (QuickSaveSlot < MaxSaveGames)
		{
			USL_QuickLoad();
			return;
		}
		// no break here!
	case mt_Load:
		menutype = mt_Load;
		break;
		
	case mt_QuickSave:
		if (QuickSaveSlot < MaxSaveGames && SavingAllowed())
		{
			USL_QuickSave();
			return;
		}
		// no break here!
	case mt_Save:
		if (!SavingAllowed())
		{
			SD_PlaySound(NOWAYSND);
			IN_ClearKeysDown();
			return;
		}
		menutype = mt_Save;
		break;
		
	default:
		menutype = mt_Normal;
	}
	
	SD_MusicOff();	// pause music when entering control panel for real
	// end of addition
	
	USL_SetUpCtlPanel();
	USL_DrawCtlPanel();

	for (CtlPanelDone = false,resetitem = on = true;!CtlPanelDone;)
	{
		item = &(topcard->items[topcard->cursor]);

		if (resetitem)
		{
			flashtime = TimeCount + (TickBase / 2);
			resetitem = false;
		}

		if (TimeCount >= flashtime)
		{
			on ^= true;
			resetitem = true;
			if (!on)
				item->flags &= ~ui_Selected;
			USL_DrawItemIcon(item);
			item->flags |= ui_Selected;
		}

		VW_UpdateScreen();
		
		IN_ReadMenuCursor(&cursorinfo);
		if (cursorinfo.button1)
		{
			do IN_ReadCursor(&cursorinfo); while (cursorinfo.button1);
			USL_UpLevel();
			resetitem = true;
		}
		else if (cursorinfo.button0)
		{
			do IN_ReadCursor(&cursorinfo); while (cursorinfo.button0);
			USL_DoItem();
			resetitem = true;
		}
		else if (cursorinfo.y < 0)
		{
			USL_PrevItem();
			resetitem = true;
		}
		else if (cursorinfo.y > 0)
		{
			USL_NextItem();
			resetitem = true;
		}
		else if (LastScan == sc_F1)
		{
			HelpScreens();
			USL_DrawCtlPanel();
			IN_ClearKeysDown();
			resetitem = true;
		}
		else if (LastScan && !resetitem)
		{
			i = topcard->cursor;
			item = topcard->items+i;
			if (item->hotkey == LastScan)
			{
				word old = i;
				
				for (item++,i++;item->type != uii_Bad;item++,i++)
				{
					if (item->hotkey == LastScan)
						goto setit;
				}
				for (item = topcard->items,i = 0;i < old;item++,i++)
				{
					if (item->hotkey == LastScan)
						goto setit;
				}
			}
			else
			{
				for (item = topcard->items,i = 0;item->type != uii_Bad;item++,i++)
				{
					if (item->hotkey == LastScan)
					{
setit:
						USL_SelectItem(topcard,i,true);
						IN_ClearKeysDown();
						resetitem = true;
						break;
					}
				}
			}
		}
	}

	USL_TearDownCtlPanel();
}

///////////////////////////////////////////////////////////////////////////
//
//      US_ControlPanel() - Wrapper for US_ControlPanelEx() during demo
//
///////////////////////////////////////////////////////////////////////////
void
US_ControlPanel(void)
{
	US_ControlPanelEx(mt_Normal);
}
