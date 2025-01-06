/* "Foray in the Forest" Source Code
 * Copyright (C) 2019-2023 K1n9_Duk3
 *
 * Primarily based on:
 * Catacomb 3-D Source Code* 
 * 
 * Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
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

// ID_GLOB.H


#include <ALLOC.H>
#include <ctype.h>
#include <DOS.H>
#include <ERRNO.H>
#include <FCNTL.H>
#include <IO.H>
#include <MEM.H>
#include <process.h>
#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>
#include <SYS\STAT.H>

// K1n9_Duk3 fix:
// If you include CONIO.H here, then there won't be any fatal name confilcts.
#include <CONIO.H>
#include <STDDEF.H>	// for offsetof macro

#define __ID_GLOB__

#define	EXTENSION	"CKS"

extern	char far introscn; // Patch for Catacomb 3-D's ID_US_1.C

#include "GRAPHCKS.H"
#include "AUDIOCKS.H"

// An alternative to the KEEN macro for features specific to this release
#define KEEN

// Modifications to the Keen Dreams techdemo
#define NEWMODS

#define	TEXTGR	0
#define	CGAGR	1
#define	EGAGR	2
#define	VGAGR	3

#define GRMODE	EGAGR

#if GRMODE == EGAGR
#define GREXT	"EGA"
#endif
#if GRMODE == CGAGR
#define GREXT	"CGA"
#endif

				
// Note to modders: 
// For debugging purposes you can re-enable the programs debugging features 
// by uncommenting the following line:

// #define DEVBUILD


//#define PROFILE

//
//	ID Engine
//	Types.h - Generic types, #defines, etc.
//	v1.0d1
//

#ifndef	__TYPES__
#define	__TYPES__

typedef	enum	{false,true}	boolean;
typedef	unsigned	char		byte;
typedef	unsigned	int			word;
typedef	unsigned	long		longword;
typedef	byte *					Ptr;

typedef	struct
		{
			int	x,y;
		} Point;
typedef	struct
		{
			Point	ul,lr;
		} Rect;

#define	nil	((void *)0)

#endif

#include "KD_SM.H"	// K1n9_Duk3 addition for FITF
#include "ID_MM.H"
#include "ID_CA.H"
#include "ID_VW.H"
#include "ID_RF.H"
#include "ID_IN.H"
#include "ID_SD.H"
#include "ID_US.H"

void	Quit (char *error);		// defined in user program
boolean	SavingAllowed(void);		// K1n9_Duk3 addition for FITF
boolean SavingWillCost(void);		// K1n9_Duk3 addition for FITF
void StopMusic(void);           // K1n9_Duk3 addition for FITF
void HelpScreens(void);         // K1n9_Duk3 addition for FITF

