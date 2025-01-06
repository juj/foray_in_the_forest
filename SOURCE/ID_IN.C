/* "Foray in the Forest" Source Code
 * Copyright (C) 2019-2021 K1n9_Duk3
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
//	ID Engine
//	ID_IN.c - Input Manager
//	v1.0d1
//	By Jason Blochowiak
//

//
//	This module handles dealing with the various input devices
//
//	Depends on: Memory Mgr (for demo recording), Sound Mgr (for timing stuff),
//				User Mgr (for command line parms)
//
//	Globals:
//		LastScan - The keyboard scan code of the last key pressed
//		LastASCII - The ASCII value of the last key pressed
//	DEBUG - there are more globals
//

#include "ID_HEADS.H"
#pragma	hdrstop

#define	KeyInt	9	// The keyboard ISR number

// Stuff for the joystick
#define	JoyScaleMax		32768
#define	JoyScaleShift	8
#define	MaxJoyValue		5000

// 	Global variables
		boolean		Keyboard[NumCodes],
					JoysPresent[MaxJoys],
					MousePresent;
		boolean		Paused;
		char		LastASCII;
		ScanCode	LastScan;
		KeyboardDef	KbdDefs[MaxKbds] = {{0x1d,0x38,0x39,0x47,0x48,0x49,0x4b,0x4d,0x4f,0x50,0x51}};	// K1n9_Duk3 mod: added 0x39 (button2)
		JoystickDef	JoyDefs[MaxJoys];
		ControlType	Controls[MaxPlayers];

		Demo		DemoMode = demo_Off;
		byte _seg	*DemoBuffer;
		word		DemoOffset,DemoSize;

//	Internal variables
static	boolean		IN_Started;
static	boolean		CapsLock;
static	ScanCode	CurCode,LastCode;
static	byte        far ASCIINames[] =		// Unshifted ASCII for scan codes
					{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,27 ,'1','2','3','4','5','6','7','8','9','0','-','=',8  ,9  ,	// 0
	'q','w','e','r','t','y','u','i','o','p','[',']',13 ,0  ,'a','s',	// 1
	'd','f','g','h','j','k','l',';',39 ,'`',0  ,92 ,'z','x','c','v',	// 2
	'b','n','m',',','.','/',0  ,'*',0  ,' ',0  ,0  ,0  ,0  ,0  ,0  ,	// 3
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,'7','8','9','-','4','5','6','+','1',	// 4
	'2','3','0',127,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0		// 7
					},
					far ShiftNames[] =		// Shifted ASCII for scan codes
					{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,27 ,'!','@','#','$','%','^','&','*','(',')','_','+',8  ,9  ,	// 0
	'Q','W','E','R','T','Y','U','I','O','P','{','}',13 ,0  ,'A','S',	// 1
	'D','F','G','H','J','K','L',':',34 ,'~',0  ,'|','Z','X','C','V',	// 2
	'B','N','M','<','>','?',0  ,'*',0  ,' ',0  ,0  ,0  ,0  ,0  ,0  ,	// 3
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,'7','8','9','-','4','5','6','+','1',	// 4
	'2','3','0',127,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0   	// 7
					},
					far SpecialNames[] =	// ASCII for 0xe0 prefixed codes
					{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 0
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,13 ,0  ,0  ,0  ,	// 1
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 2
	0  ,0  ,0  ,0  ,0  ,'/',0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 3
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 4
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0   	// 7
	
#ifndef __GAMESTR_H__	// K1n9_Duk3 mod: moved scan names into string file
					},

					*ScanNames[] =		// Scan code names with single chars
					{
	"?","?","1","2","3","4","5","6","7","8","9","0","-","+","?","?",
	"Q","W","E","R","T","Y","U","I","O","P","[","]","|","?","A","S",
	"D","F","G","H","J","K","L",";","\"","?","?","?","Z","X","C","V",
	"B","N","M",",",".","/","?","?","?","?","?","?","?","?","?","?",
	"?","?","?","?","?","?","?","?","\xf","?","-","\x15","5","\x11","+","?",
	"\x13","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?",
	"?","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?",
	"?","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?"
					},	// DEBUG - consolidate these
					far ExtScanCodes[] =	// Scan codes with >1 char names
					{
	1,0xe,0xf,0x1d,0x2a,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,
	0x3f,0x40,0x41,0x42,0x43,0x44,0x57,0x59,0x46,0x1c,0x36,
	0x37,0x38,0x47,0x49,0x4f,0x51,0x52,0x53,0x45,0x48,
	0x50,0x4b,0x4d,0x00
					},
					*ExtScanNames[] =	// Names corresponding to ExtScanCodes
					{
	"Esc","BkSp","Tab","Ctrl","LShft","Space","CapsLk","F1","F2","F3","F4",
	"F5","F6","F7","F8","F9","F10","F11","F12","ScrlLk","Enter","RShft",
	"PrtSc","Alt","Home","PgUp","End","PgDn","Ins","Del","NumLk","Up",
	"Down","Left","Right",""
#endif
					};
static	Direction	DirTable[] =		// Quick lookup for total direction
					{
						dir_NorthWest,	dir_North,	dir_NorthEast,
						dir_West,		dir_None,	dir_East,
						dir_SouthWest,	dir_South,	dir_SouthEast
					};

static	void			(*INL_KeyHook)(void);
static	void interrupt	(*OldKeyVect)(void);


#ifndef __GAMESTR_H__
// original code:
static	char			*ParmStrings[] = {"nojoys","nomouse",nil};
#else
// FITF code:
static	stringnames	ParmStrings[] = {S_NOJOYS,S_NOMOUSE,-1};
#endif
//	Internal routines

///////////////////////////////////////////////////////////////////////////
//
//	INL_KeyService() - Handles a keyboard interrupt (key up/down)
//
///////////////////////////////////////////////////////////////////////////
static void interrupt
INL_KeyService(void)
{
static	boolean	special;
		byte	k,c,
				temp;

	k = inportb(0x60);	// Get the scan code

	// Tell the XT keyboard controller to clear the key
	outportb(0x61,(temp = inportb(0x61)) | 0x80);
	outportb(0x61,temp);

	if (k == 0xe0)		// Special key prefix
		special = true;
	else if (k == 0xe1)	// Handle Pause key
		Paused = true;
	else
	{
		// K1n9_Duk3 addition:
		if (special && (k & 0x7f) == sc_LShift)
		{
			// ignore LShift prefixes
			k = 0x80;
		}
		// end of addition

		if (k & 0x80)	// Break code
		{
			k &= 0x7f;

// DEBUG - handle special keys: ctl-alt-delete, print scrn

			Keyboard[k] = false;
		}
		else			// Make code
		{
			LastCode = CurCode;
			CurCode = LastScan = k;
			Keyboard[k] = true;

			if (special)
				c = SpecialNames[k];
			else
			{
				if (k == sc_CapsLock)
				{
					CapsLock ^= true;
					// DEBUG - make caps lock light work
				}

				if (Keyboard[sc_LShift] || Keyboard[sc_RShift])	// If shifted
				{
					c = ShiftNames[k];
					if ((c >= 'A') && (c <= 'Z') && CapsLock)
						c += 'a' - 'A';
				}
				else
				{
					c = ASCIINames[k];
					if ((c >= 'a') && (c <= 'z') && CapsLock)
						c -= 'a' - 'A';
				}
			}
			if (c)
				LastASCII = c;
		}

		special = false;
	}

	if (INL_KeyHook && !special)
		INL_KeyHook();
	outportb(0x20,0x20);


	// K1n9_Duk3 addition:
	if (Keyboard[sc_Control] && Keyboard[sc_Alt] && Keyboard[sc_Delete])
	{
		extern char str[], str2[];
		unsigned oldcs, oldip;

		asm	mov	ax, [bp+18];
		asm	mov	dx, [bp+20];
		asm	sub	dx, seg exit;	// exit routine is in the first segment, subtraction gives us a clean (non-relocated) segment address
		asm	mov	oldip, ax;
		asm	mov	oldcs, dx;

		itoa(oldcs, str, 16);
		itoa(oldip, str2, 16);
		strcat(str, ":");
		strcat(str, str2);
		Quit(str);
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetMouseDelta() - Gets the amount that the mouse has moved from the
//		mouse driver
//
///////////////////////////////////////////////////////////////////////////
static void
INL_GetMouseDelta(int *x,int *y)
{
	Mouse(MDelta);
	*x = _CX;
	*y = _DX;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetMouseButtons() - Gets the status of the mouse buttons from the
//		mouse driver
//
///////////////////////////////////////////////////////////////////////////
static word
INL_GetMouseButtons(void)
{
	word	buttons;

	Mouse(MButtons);
	buttons = _BX;
	return(buttons);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetJoyAbs() - Reads the absolute position of the specified joystick
//
///////////////////////////////////////////////////////////////////////////
void
IN_GetJoyAbs(word joy,word *xp,word *yp)
{
	byte	xb,yb,
			xs,ys;
	word	x,y;

	x = y = 0;
	xs = joy? 2 : 0;		// Find shift value for x axis
	xb = 1 << xs;			// Use shift value to get x bit mask
	ys = joy? 3 : 1;		// Do the same for y axis
	yb = 1 << ys;

// Read the absolute joystick values
asm		pushf				// Save some registers
asm		push	si
asm		push	di
asm		cli					// Make sure an interrupt doesn't screw the timings


asm		mov		dx,0x201
asm		in		al,dx
asm		out		dx,al		// Clear the resistors

asm		mov		ah,[xb]		// Get masks into registers
asm		mov		ch,[yb]

asm		xor		si,si		// Clear count registers
asm		xor		di,di
asm		xor		bh,bh		// Clear high byte of bx for later

asm		push	bp			// Don't mess up stack frame
asm		mov		bp,MaxJoyValue

loop:
asm		in		al,dx		// Get bits indicating whether all are finished

asm		dec		bp			// Check bounding register
asm		jz		done		// We have a silly value - abort

asm		mov		bl,al		// Duplicate the bits
asm		and		bl,ah		// Mask off useless bits (in [xb])
asm		add		si,bx		// Possibly increment count register
asm		mov		cl,bl		// Save for testing later

asm		mov		bl,al
asm		and		bl,ch		// [yb]
asm		add		di,bx

asm		add		cl,bl
asm		jnz		loop 		// If both bits were 0, drop out

done:
asm     pop		bp

asm		mov		cl,[xs]		// Get the number of bits to shift
asm		shr		si,cl		//  and shift the count that many times

asm		mov		cl,[ys]
asm		shr		di,cl

asm		mov		[x],si		// Store the values into the variables
asm		mov		[y],di

asm		pop		di
asm		pop		si
asm		popf				// Restore the registers

	*xp = x;
	*yp = y;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetJoyDelta() - Returns the relative movement of the specified
//		joystick (from +/-127, scaled adaptively)
//
///////////////////////////////////////////////////////////////////////////
static void
INL_GetJoyDelta(word joy,int *dx,int *dy,boolean adaptive)
{
	word		x,y;
	longword	time;
	JoystickDef	*def;
static	longword	lasttime;

	IN_GetJoyAbs(joy,&x,&y);
	def = JoyDefs + joy;

	if (x < def->threshMinX)
	{
		if (x < def->joyMinX)
			x = def->joyMinX;

		x = -(x - def->threshMinX);
		x *= def->joyMultXL;
		x >>= JoyScaleShift;
		*dx = (x > 127)? -127 : -x;
	}
	else if (x > def->threshMaxX)
	{
		if (x > def->joyMaxX)
			x = def->joyMaxX;

		x = x - def->threshMaxX;
		x *= def->joyMultXH;
		x >>= JoyScaleShift;
		*dx = (x > 127)? 127 : x;
	}
	else
		*dx = 0;

	if (y < def->threshMinY)
	{
		if (y < def->joyMinY)
			y = def->joyMinY;

		y = -(y - def->threshMinY);
		y *= def->joyMultYL;
		y >>= JoyScaleShift;
		*dy = (y > 127)? -127 : -y;
	}
	else if (y > def->threshMaxY)
	{
		if (y > def->joyMaxY)
			y = def->joyMaxY;

		y = y - def->threshMaxY;
		y *= def->joyMultYH;
		y >>= JoyScaleShift;
		*dy = (y > 127)? 127 : y;
	}
	else
		*dy = 0;

	if (adaptive)
	{
#if 0
		// original code:
		time = (TimeCount - lasttime) / 2;
		if (time)
		{
			if (time > 8)
				time = 8;
			*dx *= time;
			*dy *= time;
		}
#else
		// K1n9_Duk3 fix:
		time = TimeCount - lasttime;
		// adaptive scaling *must* multiply by 0 when no time has passed
		
		if (time > 8)
			time = 8;
		
		// time is unsigned, so it will be between 0 and 8 after the check above
		// which means we can perform a 16-bit multiplication instead of the slow
		// 32 bit multiplication here:
		*dx *= (int)time;
		*dy *= (int)time;
#endif
	}
	lasttime = TimeCount;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetJoyButtons() - Returns the button status of the specified
//		joystick
//
///////////////////////////////////////////////////////////////////////////
static word
INL_GetJoyButtons(word joy)
{
register	word	result;

	result = inportb(0x201);	// Get all the joystick buttons
	result >>= joy? 6 : 4;	// Shift into bits 0-1
	result &= 3;				// Mask off the useless bits
	result ^= 3;
	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetJoyButtonsDB() - Returns the de-bounced button status of the
//		specified joystick
//
///////////////////////////////////////////////////////////////////////////
word
IN_GetJoyButtonsDB(word joy)
{
	longword	lasttime;
	word		result1,result2;

	do
	{
		result1 = INL_GetJoyButtons(joy);
		lasttime = TimeCount;
		while (TimeCount == lasttime)
			;
		result2 = INL_GetJoyButtons(joy);
	} while (result1 != result2);
	return(result1);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_StartKbd() - Sets up my keyboard stuff for use
//
///////////////////////////////////////////////////////////////////////////
static void
INL_StartKbd(void)
{
	INL_KeyHook = 0;	// Clear key hook

	IN_ClearKeysDown();

	OldKeyVect = getvect(KeyInt);
	setvect(KeyInt,INL_KeyService);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_ShutKbd() - Restores keyboard control to the BIOS
//
///////////////////////////////////////////////////////////////////////////
static void
INL_ShutKbd(void)
{
	poke(0x40,0x17,peek(0x40,0x17) & 0xfaf0);	// Clear ctrl/alt/shift flags

	setvect(KeyInt,OldKeyVect);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_StartMouse() - Detects and sets up the mouse
//
///////////////////////////////////////////////////////////////////////////
static boolean
INL_StartMouse(void)
{
	if (getvect(MouseInt))
	{
		Mouse(MReset);
		if (_AX == 0xffff)
			return(true);
	}
	return(false);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_ShutMouse() - Cleans up after the mouse
//
///////////////////////////////////////////////////////////////////////////
static void
INL_ShutMouse(void)
{
}

//
//	INL_SetJoyScale() - Sets up scaling values for the specified joystick
//
static void
INL_SetJoyScale(word joy)
{
	JoystickDef	*def;

	def = &JoyDefs[joy];
	def->joyMultXL = JoyScaleMax / (def->threshMinX - def->joyMinX);
	def->joyMultXH = JoyScaleMax / (def->joyMaxX - def->threshMaxX);
	def->joyMultYL = JoyScaleMax / (def->threshMinY - def->joyMinY);
	def->joyMultYH = JoyScaleMax / (def->joyMaxY - def->threshMaxY);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_SetupJoy() - Sets up thresholding values and calls INL_SetJoyScale()
//		to set up scaling values
//
///////////////////////////////////////////////////////////////////////////
void
IN_SetupJoy(word joy,word minx,word maxx,word miny,word maxy)
{
	word		d,r;
	JoystickDef	*def;

	def = &JoyDefs[joy];

	def->joyMinX = minx;
	def->joyMaxX = maxx;
	r = maxx - minx;
	d = r / 5;
	def->threshMinX = ((r / 2) - d) + minx;
	def->threshMaxX = ((r / 2) + d) + minx;

	def->joyMinY = miny;
	def->joyMaxY = maxy;
	r = maxy - miny;
	d = r / 5;
	def->threshMinY = ((r / 2) - d) + miny;
	def->threshMaxY = ((r / 2) + d) + miny;

	INL_SetJoyScale(joy);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_StartJoy() - Detects & auto-configures the specified joystick
//					The auto-config assumes the joystick is centered
//
///////////////////////////////////////////////////////////////////////////
static boolean
INL_StartJoy(word joy)
{
	word		x,y;

	IN_GetJoyAbs(joy,&x,&y);

	if
	(
		((x == 0) || (x > MaxJoyValue - 10))
	||	((y == 0) || (y > MaxJoyValue - 10))
	)
		return(false);
	else
	{
		IN_SetupJoy(joy,0,x * 2,0,y * 2);
		return(true);
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_ShutJoy() - Cleans up the joystick stuff
//
///////////////////////////////////////////////////////////////////////////
static void
INL_ShutJoy(word joy)
{
	JoysPresent[joy] = false;
}

//	Public routines

///////////////////////////////////////////////////////////////////////////
//
//	IN_Startup() - Starts up the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void
IN_Startup(void)
{
	boolean	checkjoys,checkmouse;
	word	i;

	if (IN_Started)
		return;

#ifndef __GAMESTR_H__
	// original code:
	checkjoys = true;
	checkmouse = true;
	for (i = 1;i < _argc;i++)
	{
		switch (US_CheckParm(_argv[i],ParmStrings))
		{
		case 0:
			checkjoys = false;
			break;
		case 1:
			checkmouse = false;
			break;
		}
	}
#else
	// FITF code:
	checkjoys = !SM_ParmPresent(S_NOJOYS);
	checkmouse = !SM_ParmPresent(S_NOMOUSE);
#endif

	INL_StartKbd();
	MousePresent = checkmouse? INL_StartMouse() : false;

	for (i = 0;i < MaxJoys;i++)
		JoysPresent[i] = checkjoys? INL_StartJoy(i) : false;

	IN_Started = true;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_Default() - Sets up default conditions for the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void
IN_Default(boolean gotit,ControlType in)
{
	if
	(
		(!gotit)
	|| 	((in == ctrl_Joystick1) && !JoysPresent[0])
	|| 	((in == ctrl_Joystick2) && !JoysPresent[1])
	|| 	((in == ctrl_Mouse) && !MousePresent)
	)
		in = ctrl_Keyboard1;
	IN_SetControlType(0,in);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_Shutdown() - Shuts down the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void
IN_Shutdown(void)
{
	word	i;

	if (!IN_Started)
		return;

	INL_ShutMouse();
	for (i = 0;i < MaxJoys;i++)
		INL_ShutJoy(i);
	INL_ShutKbd();

	IN_Started = false;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_SetKeyHook() - Sets the routine that gets called by INL_KeyService()
//			everytime a real make/break code gets hit
//
///////////////////////////////////////////////////////////////////////////
void
IN_SetKeyHook(void (*hook)())
{
	INL_KeyHook = hook;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_ClearKeyDown() - Clears the keyboard array
//
///////////////////////////////////////////////////////////////////////////
void
IN_ClearKeysDown(void)
{
	int	i;

	LastScan = sc_None;
	LastASCII = key_None;
	for (i = 0;i < NumCodes;i++)
		Keyboard[i] = false;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_AdjustCursor() - Internal routine of common code from IN_ReadCursor()
//
///////////////////////////////////////////////////////////////////////////
static void
INL_AdjustCursor(CursorInfo *info,word buttons,int dx,int dy)
{
	if (buttons & (1 << 0))
		info->button0 = true;
	if (buttons & (1 << 1))
		info->button1 = true;

	info->x += dx;
	info->y += dy;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_ReadCursor() - Reads the input devices and fills in the cursor info
//		struct
//
///////////////////////////////////////////////////////////////////////////
void
IN_ReadCursor(CursorInfo *info)
{
	word	i,
			buttons;
	int		dx,dy;

	info->x = info->y = 0;
	info->button0 = info->button1 = false;

	if (MousePresent)
	{
		buttons = INL_GetMouseButtons();
		INL_GetMouseDelta(&dx,&dy);
		INL_AdjustCursor(info,buttons,dx,dy);
	}

	for (i = 0;i < MaxJoys;i++)
	{
		if (!JoysPresent[i] || Controls[0] != ctrl_Joystick1+i)	// K1n9_Duk3 mod: joystick can't move the cursor unless selected as input device
			continue;

		buttons = INL_GetJoyButtons(i);
		INL_GetJoyDelta(i,&dx,&dy,true);
		// K1n9_Duk3 fix: divide deltas by 8 instead of 64:
		dx >>= 3;
		dy >>= 3;
		INL_AdjustCursor(info,buttons,dx,dy);
	}
}
	
///////////////////////////////////////////////////////////////////////////
//
//	IN_ReadMenuCursor() - Reads the input devices and fills in the cursor
//		info struct
//
///////////////////////////////////////////////////////////////////////////

#define MinMove 40
#define RepeatDelay (TickBase/2)
#define RepeatInterval 7

typedef struct
{
	word joyheld, uptime, downtime, lefttime, righttime;
} JoyMenuCursor;

void
IN_ReadMenuCursor(CursorInfo *info)
{
	static word lasttime;
	static int xmove, ymove;
	
#if 1
	// This version emulates keyboard repeat rate for joystick movement, which
	// means the cursor will move one step, then wait half a second before moving
	// further in that direction. This is the better version, but the code is a
	// lot bigger than the other version.
	static JoyMenuCursor joys[MaxJoys];
	word	i, time,
			buttons;
	int		dx,dy;
	JoyMenuCursor *joy;

	time = (unsigned)TimeCount - lasttime;
	lasttime = TimeCount;
	
	if (time > TickBase)
	{
		memset(joys, 0, sizeof(joys));
		IN_ReadMenuCursor(info);	// "flush" mouse and joystick
		time=xmove=ymove=0;
	}

	info->x = info->y = 0;
	info->button0 = info->button1 = false;
	
	if (MousePresent)
	{
		buttons = INL_GetMouseButtons();
		INL_GetMouseDelta(&dx,&dy);
		xmove += dx;
		ymove += dy;
		if (xmove < -MinMove)
		{
			dx = motion_Left;
			xmove += MinMove;
		}
		else if (xmove > MinMove)
		{
			dx = motion_Right;
			xmove -= MinMove;
		}
		else
		{
			dx = motion_None;
		}
		if (ymove < -MinMove)
		{
			dy = motion_Up;
			ymove += MinMove;
		}
		else if (ymove > MinMove)
		{
			dy = motion_Down;
			ymove -= MinMove;
		}
		else
		{
			dy = motion_None;
		}
		INL_AdjustCursor(info,buttons,dx,dy);
	}

	for (i = 0, joy=joys; i < MaxJoys; i++,joy++)
	{
		if (!JoysPresent[i] || Controls[0] != ctrl_Joystick1+i)
			continue;

		buttons = INL_GetJoyButtons(i);
		INL_GetJoyDelta(i,&dx,&dy,false);
		
		if (dx < -8)	// if stick is moved left
		{
			joy->lefttime += time;
			if (joy->joyheld & 1)	// if stick was moved left before
			{
				if (joy->lefttime > RepeatDelay)
				{
					joy->lefttime -= RepeatInterval;
					dx = motion_Left;
				}
				else
				{
					dx = motion_None;
				}
			}
			else
			{
				joy->joyheld |= 1;
				dx = motion_Left;
			}
			joy->joyheld &= ~2;	// stick is definitely not moved right anymore
			joy->righttime = 0;
		}
		else if (dx > 8)	// if stick is moved right
		{
			joy->righttime += time;
			if (joy->joyheld & 2)	// if stick was moved right before
			{
				if (joy->righttime > RepeatDelay)
				{
					joy->righttime -= RepeatInterval;
					dx = motion_Right;
				}
				else
				{
					dx = motion_None;
				}
			}
			else
			{
				joy->joyheld |= 2;
				dx = motion_Right;
			}
			joy->joyheld &= ~1;	// stick is definitely not moved left anymore
			joy->lefttime = 0;
		}
		else if (dx)
		{
			dx = 0;
		}
		else
		{
			joy->joyheld &= ~(1|2);	// stick is neither left nor right
			joy->lefttime = joy->righttime = 0;
		}
		
		if (dy < -8)	// if stick is moved up
		{
			joy->uptime += time;
			if (joy->joyheld & 4)	// if stick was moved up before
			{
				if (joy->uptime > RepeatDelay)
				{
					joy->uptime -= RepeatInterval;
					dy = motion_Up;
				}
				else
				{
					dy = motion_None;
				}
			}
			else
			{
				joy->joyheld |= 4;
				dy = motion_Up;
			}
			joy->joyheld &= ~8;	// stick is definitely not moved down anymore
			joy->downtime = 0;
		}
		else if (dy > 8)	// if stick is moved down
		{
			joy->downtime += time;
			if (joy->joyheld & 8)	// if stick was moved down before
			{
				if (joy->downtime > RepeatDelay)
				{
					joy->downtime -= RepeatInterval;
					dy = motion_Down;
				}
				else
				{
					dy = motion_None;
				}
			}
			else
			{
				joy->joyheld |= 8;
				dy = motion_Down;
			}
			joy->joyheld &= ~4;	// stick is definitely not moved up anymore
			joy->uptime = 0;
		}
		else if (dy)
		{
			dy = 0;
		}
		else
		{
			joy->joyheld &= ~(4|8);	// stick is neither up nor down
			joy->uptime = joy->downtime = 0;
		}
		INL_AdjustCursor(info,buttons,dx,dy);
	}
#else
	// This is the shorter version. It works much more like the original menu
	// navigation code of Keen 4-6, but it's harder to make the menu cursor move
	// just one single step with the joystick than it would be with the version
	// above.
	if ((word)TimeCount - lasttime > TickBase)
	{
		// clear the mouse movement buffer and reset joystick's adaptive scaling:
		IN_ReadCursor(info);
		// and also clear the local movement buffers:
		xmove=ymove=0;
	}
	lasttime = (word)TimeCount;
	
	IN_ReadCursor(info);
	xmove += info->x;
	ymove += info->y;
	info->x = info->y = motion_None;
	
	if (xmove < -MinMove)
	{
		xmove += MinMove;
		info->x = motion_Left;
	}
	else if (xmove > MinMove)
	{
		xmove -= MinMove;
		info->x = motion_Right;
	}
	
	if (ymove < -MinMove)
	{
		ymove += MinMove;
		info->y = motion_Up;
	}
	else if (ymove > MinMove)
	{
		ymove -= MinMove;
		info->y = motion_Down;
	}
#endif
	
	// Keyboard keys for menu navigation are also checked in here, to reduce
	// redundancy in the code that uses menu navigation (i.e. the Control Panel,
	// Help Menu, Help Texts, Jukebox and so on).
	// Note that this uses IN_ClearKeysDown() instead of just LastScan = sc_None
	// because the function call is one byte smaller than the assignment. And
	// the menu code does not use the Keyboard array (or the IN_KeyDown() macro)
	// anyway, so clearing it won't hurt.
#define scan _AL
	scan = LastScan;
	if (scan == sc_Escape)
	{
		info->button1 = true;
		IN_ClearKeysDown();
	}
	else if (scan == sc_Enter || scan == KbdDefs[0].button0 || scan == KbdDefs[0].button1 || scan == KbdDefs[0].button2)
	{
		info->button0 = true;
		IN_ClearKeysDown();
	}
	else if (scan == sc_UpArrow)
	{
		info->y = motion_Up;
		IN_ClearKeysDown();
	}
	else if (scan == sc_LeftArrow)
	{
		info->x = motion_Left;
		IN_ClearKeysDown();
	}
	else if (scan == sc_RightArrow)
	{
		info->x = motion_Right;
		IN_ClearKeysDown();
	}
	else if (scan == sc_DownArrow)
	{
		info->y = motion_Down;
		IN_ClearKeysDown();
	}
#undef scan
}
#undef MinMove
#undef RepeatDelay
#undef RepeatInterval	

///////////////////////////////////////////////////////////////////////////
//
//	IN_ReadControl() - Reads the device associated with the specified
//		player and fills in the control info struct
//
///////////////////////////////////////////////////////////////////////////
void
IN_ReadControl(int player,ControlInfo *info)
{
			boolean		realdelta;
			byte		dbyte;
			word		buttons;
			int			dx,dy;
			Motion		mx,my;
			ControlType	type;
register	KeyboardDef	*def;

	dx = dy = 0;
	mx = my = motion_None;
	buttons = 0;

	if (DemoMode == demo_Playback)
	{
		dbyte = DemoBuffer[DemoOffset + 1];
		my = (dbyte & 3) - 1;
		mx = ((dbyte >> 2) & 3) - 1;
		buttons = (dbyte >> 4) & 7;	// K1n9_Duk3 mod: use 7 instead of 3 as mask

		if (!(--DemoBuffer[DemoOffset]))
		{
			DemoOffset += 2;
			if (DemoOffset >= DemoSize)
				DemoMode = demo_PlayDone;
		}

		realdelta = false;
	}
	else if (DemoMode == demo_PlayDone)
		//Quit("Demo playback exceeded");
		SM_Quit(S_DEMOPLAYBACKEXCEEDED);
	else
	{
		switch (type = Controls[player])
		{
		case ctrl_Keyboard1:
		case ctrl_Keyboard2:
			def = &KbdDefs[type - ctrl_Keyboard];

			if (Keyboard[def->upleft])
				mx = motion_Left,my = motion_Up;
			else if (Keyboard[def->upright])
				mx = motion_Right,my = motion_Up;
			else if (Keyboard[def->downleft])
				mx = motion_Left,my = motion_Down;
			else if (Keyboard[def->downright])
				mx = motion_Right,my = motion_Down;

			if (Keyboard[def->up])
				my = motion_Up;
			else if (Keyboard[def->down])
				my = motion_Down;

			if (Keyboard[def->left])
				mx = motion_Left;
			else if (Keyboard[def->right])
				mx = motion_Right;

			if (Keyboard[def->button0])
				buttons += 1 << 0;
			if (Keyboard[def->button1])
				buttons += 1 << 1;
			if (Keyboard[def->button2])	// K1n9_Duk3 addition
				buttons += 1 << 2;
			realdelta = false;
			break;
		case ctrl_Joystick1:
		case ctrl_Joystick2:
			INL_GetJoyDelta(type - ctrl_Joystick,&dx,&dy,false);
			buttons = INL_GetJoyButtons(type - ctrl_Joystick);
			// K1n9_Duk3 hack: joystick uses keyboard input for third button
			{
				extern boolean oldshooting;
				if ((!oldshooting || DemoMode == demo_Record) && Keyboard[KbdDefs[0].button2])
				{
					buttons |= (1 << 2);
				}
			}
			// end of hack
			realdelta = true;
			break;
		case ctrl_Mouse:
			INL_GetMouseDelta(&dx,&dy);
			buttons = INL_GetMouseButtons();
			realdelta = true;
			break;
		}
	}

	if (realdelta)
	{
		mx = (dx < 0)? motion_Left : ((dx > 0)? motion_Right : motion_None);
		my = (dy < 0)? motion_Up : ((dy > 0)? motion_Down : motion_None);
	}
	else
	{
		dx = mx * 127;
		dy = my * 127;
	}

	info->x = dx;
	info->xaxis = mx;
	info->y = dy;
	info->yaxis = my;
	info->button0 = buttons & (1 << 0);
	info->button1 = buttons & (1 << 1);
	info->button2 = buttons & (1 << 2);	// K1n9_Duk3 addition
	info->dir = DirTable[((my + 1) * 3) + (mx + 1)];

	if (DemoMode == demo_Record)
	{
		// Pack the control info into a byte
		dbyte = (buttons << 4) | ((mx + 1) << 2) | (my + 1);

		if
		(
			(DemoBuffer[DemoOffset + 1] == dbyte)
		&&	(DemoBuffer[DemoOffset] < 255)
		)
			(DemoBuffer[DemoOffset])++;
		else
		{
			if (DemoOffset || DemoBuffer[DemoOffset])
				DemoOffset += 2;

			if (DemoOffset >= DemoSize)
				//Quit("Demo buffer overflow");
				SM_Quit(S_DEMOBUFFEROVERFLOW);

			DemoBuffer[DemoOffset] = 1;
			DemoBuffer[DemoOffset + 1] = dbyte;
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_SetControlType() - Sets the control type to be used by the specified
//		player
//
///////////////////////////////////////////////////////////////////////////
void
IN_SetControlType(int player,ControlType type)
{
	// DEBUG - check that requested type is present?
	Controls[player] = type;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_StartDemoRecord() - Starts the demo recording, using a buffer the
//		size passed. Returns if the buffer allocation was successful
//
///////////////////////////////////////////////////////////////////////////
#ifdef DEVBUILD

boolean
IN_StartDemoRecord(word bufsize)
{
	if (!bufsize)
		return(false);

	MM_GetPtr((memptr *)&DemoBuffer,bufsize);
	DemoMode = demo_Record;
	DemoSize = bufsize & ~1;
	DemoOffset = 0;
	DemoBuffer[0] = DemoBuffer[1] = 0;

	return(true);
}
#endif

///////////////////////////////////////////////////////////////////////////
//
//	IN_StartDemoPlayback() - Plays back the demo pointed to of the given size
//
///////////////////////////////////////////////////////////////////////////
void
IN_StartDemoPlayback(byte _seg *buffer,word bufsize)
{
	DemoBuffer = buffer;
	DemoMode = demo_Playback;
	DemoSize = bufsize & ~1;
	DemoOffset = 0;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_StopDemo() - Turns off demo mode
//
///////////////////////////////////////////////////////////////////////////
void
IN_StopDemo(void)
{
	if ((DemoMode == demo_Record) && DemoOffset)
		DemoOffset += 2;

	DemoMode = demo_Off;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_FreeDemoBuffer() - Frees the demo buffer, if it's been allocated
//
///////////////////////////////////////////////////////////////////////////
void
IN_FreeDemoBuffer(void)
{
	if (DemoBuffer)
		MM_FreePtr((memptr *)&DemoBuffer);
}


#ifndef __GAMESTR_H__
// original code:
///////////////////////////////////////////////////////////////////////////
//
//	IN_GetScanName() - Returns a string containing the name of the
//		specified scan code
//
///////////////////////////////////////////////////////////////////////////
byte *
IN_GetScanName(ScanCode scan)
{
	byte		**p;
	ScanCode	far *s;

	for (s = ExtScanCodes,p = ExtScanNames;*s;p++,s++)
		if (*s == scan)
			return(*p);

	return(ScanNames[scan]);
}
#else
// FITF code:
///////////////////////////////////////////////////////////////////////////
//
//	IN_GetScanName() - Returns a string number for the name of the
//		specified scan code
//
///////////////////////////////////////////////////////////////////////////
stringnames
IN_GetScanName(ScanCode scan)
{
	if ((unsigned)scan < (unsigned)(S_LASTKEYNAME-S_KEYNAME0))
		return scan + S_KEYNAME0;
	
	return S_LASTKEYNAME;
}
#endif

///////////////////////////////////////////////////////////////////////////
//
//	IN_WaitForKey() - Waits for a scan code, then clears LastScan and
//		returns the scan code
//
///////////////////////////////////////////////////////////////////////////
ScanCode
IN_WaitForKey(void)
{
	ScanCode	result;

	while (!(result = LastScan))
		;
	LastScan = 0;
	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_WaitForASCII() - Waits for an ASCII char, then clears LastASCII and
//		returns the ASCII value
//
///////////////////////////////////////////////////////////////////////////
char
IN_WaitForASCII(void)
{
	char		result;

	while (!(result = LastASCII))
		;
	LastASCII = '\0';
	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_AckBack() - Waits for either an ASCII keypress or a button press
//
///////////////////////////////////////////////////////////////////////////
void
IN_AckBack(void)
{
	word	i;

	while (!LastScan)
	{
		if (MousePresent)
		{
			if (INL_GetMouseButtons())
			{
				while (INL_GetMouseButtons())
					;
				return;
			}
		}

		for (i = 0;i < MaxJoys;i++)
		{
			if (JoysPresent[i])
			{
				if (IN_GetJoyButtonsDB(i))
				{
					while (IN_GetJoyButtonsDB(i))
						;
					return;
				}
			}
		}
	}

	IN_ClearKey(LastScan);
	LastScan = sc_None;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_Ack() - Clears user input & then calls IN_AckBack()
//
///////////////////////////////////////////////////////////////////////////
void
IN_Ack(void)
{
	word	i;

	IN_ClearKey(LastScan);
	LastScan = sc_None;

	if (MousePresent)
		while (INL_GetMouseButtons())
					;
	for (i = 0;i < MaxJoys;i++)
		if (JoysPresent[i])
			while (IN_GetJoyButtonsDB(i))
				;

	IN_AckBack();
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_IsUserInput() - Returns true if a key has been pressed or a button
//		is down
//
///////////////////////////////////////////////////////////////////////////
boolean
IN_IsUserInput(void)
{
	boolean	result;
	word	i;

	result = LastScan;

	if (MousePresent)
		if (INL_GetMouseButtons())
			result = true;

	for (i = 0;i < MaxJoys;i++)
		if (JoysPresent[i])
			if (INL_GetJoyButtons(i))
				result = true;

	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_UserInput() - Waits for the specified delay time (in ticks) or the
//		user pressing a key or a mouse button. If the clear flag is set, it
//		then either clears the key or waits for the user to let the mouse
//		button up.
//
///////////////////////////////////////////////////////////////////////////
boolean
IN_UserInput(longword delay,boolean clear)
{
	longword	lasttime;

	lasttime = TimeCount;
	do
	{
		if (IN_IsUserInput())
		{
			if (clear)
				IN_AckBack();
			return(true);
		}
	} while (TimeCount - lasttime < delay);
	return(false);
}
