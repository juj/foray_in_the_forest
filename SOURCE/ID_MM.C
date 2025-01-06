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

// NEWMM.C

/*
=============================================================================

		   ID software memory manager
		   --------------------------

Primary coder: John Carmack

RELIES ON
---------
Quit (char *error) function


WORK TO DO
----------
MM_SizePtr to change the size of a given pointer

Multiple purge levels utilized

EMS / XMS unmanaged routines

=============================================================================
*/

#include "ID_HEADS.H"
#pragma hdrstop

#pragma warn -pro
#pragma warn -use

/*
=============================================================================

							LOCAL INFO

=============================================================================
*/

#define LOCKBIT		0x80	// if set in attributes, block cannot be moved
#define NONPURGE		0x40	// K1n9_Duk3 addition: can be moved but not purged
#define PURGEBITS	3		// 0-3 level, 0= unpurgable, 3= purge first
#define PURGEMASK	0xfffc
#define BASEATTRIBUTES	0	// unlocked, non purgable
#define MOVESIZE	0xf00

#define MAXUMBS		10

typedef struct mmblockstruct
{
	unsigned	start,length;
	unsigned	attributes;
	memptr		*useptr;	// pointer to the segment start
	struct mmblockstruct far *next;
} mmblocktype;


//#define GETNEWBLOCK {if(!(mmnew=mmfree))Quit("MM_GETNEWBLOCK: No free blocks!")\
//	;mmfree=mmfree->next;}

#define GETNEWBLOCK {if(!mmfree)MML_ClearBlock();mmnew=mmfree;mmfree=mmfree->next;}

#define FREEBLOCK(x) {*x->useptr=NULL;x->next=mmfree;mmfree=x;}

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

mminfotype	mminfo;
memptr		bufferseg;
boolean		mmerror;

unsigned	mmsorting;	// K1n9_Duk3 addition

void		(* beforesort) (void);
void		(* aftersort) (void);

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

boolean		mmstarted;

void far	*farheap;
void		*nearheap;

mmblocktype	far mmblocks[MAXBLOCKS]
			,far *mmhead,far *mmfree,far *mmrover,far *mmnew;

boolean		bombonerror;

unsigned	totalEMSpages,freeEMSpages,EMSpageframe,EMSpagesmapped,EMShandle;

void		(* XMSaddr) (void);		// far pointer to XMS driver

unsigned	numUMBs,UMBbase[MAXUMBS];

unsigned	numDUMBs,DUMBbase[MAXUMBS];	// K1n9_Duk3 addition: DOS UMBs = DUMBs ;)
unsigned	oldstrat;
byte		oldlink;

boolean		DUMBstarted;

//==========================================================================

//
// local prototypes
//

boolean		MML_CheckForEMS (void);
void 		MML_ShutdownEMS (void);
void 		MM_MapEMS (void);
boolean 	MML_CheckForXMS (void);
void 		MML_ShutdownXMS (void);
void		MML_UseSpace (unsigned segstart, unsigned seglength);
void 		MML_ClearBlock (void);
static void near MML_DefragMem(void);

//==========================================================================

/*
======================
=
= MML_SetupUMB		(K1n9_Duk3 addition)
=
= Grab any UMBs available from DOS (XMS won't work when DOS manages UMBs)
=
=======================
*/

void MML_SetupUMB (void)
{
	unsigned	base,size;

	numDUMBs = 0;

// K1n9_Duk3 says:
// If the destination of a jump address is too far away for a conditional jump
// ('jc', 'jnc' etc.) then the inline assembler will generate the wrong code
// instead of showing a warning or an error message, so double-check the result
// when you modify this code!

asm	mov	ax, 5800h;
asm	int	21h;		// get old memory allocation strategy
asm	jc 	error;	// (can't use 'jc done' since label is too far away)
asm	mov	oldstrat, ax;

asm	mov	ax, 5802h;
asm	int	21h;		// get UMB link state
asm	jc 	done;
asm	mov	oldlink, al;

	DUMBstarted = true;

asm	mov	ax, 5803h;
asm	mov	bx, 1;
asm	int	21h;		// add UMBs in DOS memory chain
asm	jc 	done;		// (will fail when DOS doesn't manage UMBs)

asm	mov	ax, 5801h;
asm	mov	bx, 40h;
asm	int	21h;		// set memory allocation strategy: high memory
error:
asm	jc 	done;

getloop:
asm	mov	ah, 48h;
asm	mov	bx, -1;		// get largest possible size
asm	int	21h;
//asm	jnc	gotone;
asm	or	bx, bx;
asm	jz	done;

asm	mov	ah, 48h;
asm	int	21h;		// allocate block
asm	jc	done;

gotone:
asm	mov	[base], ax;
asm	mov	[size], bx;

	MML_UseSpace(base, size);
	DUMBbase[numDUMBs++] = base;
	mminfo.XMSmem += ((longword)size)*16;
	if (numDUMBs < MAXUMBS)
		goto getloop;
done:;
}

/*
======================
=
= MML_ShutdownUMB	(K1n9_Duk3 addition)
=
= Free any UMBs allocated from DOS
=
=======================
*/
void MML_ShutdownUMB (void)
{
	if (DUMBstarted)
	{
		while (numDUMBs)
		{
			_ES = DUMBbase[--numDUMBs];
		asm	mov	ah, 49h;
		asm	int	21h;		// free block
		}

	asm	mov	ax, 5801h;
	asm	mov	bx, oldstrat;
	asm	int	21h;		// back to old memory allocation strategy

	asm	mov	ax, 5803h;
	asm	mov	bl, oldlink;
	asm	xor	bh, bh;
	asm	int	21h;		// back to old UMB link mode
	}

}

//==========================================================================


/*
======================
=
= MML_CheckForEMS
=
= Routine from p36 of Extending DOS
=
=======================
*/

char	emmname[9] = "EMMXXXX0";

boolean MML_CheckForEMS (void)
{
asm	mov	dx,OFFSET emmname[0]
asm	mov	ax,0x3d00
asm	int	0x21		// try to open EMMXXXX0 device
asm	jc	error

asm	mov	bx,ax
asm	mov	ax,0x4400

asm	int	0x21		// get device info
asm	jc	error

asm	and	dx,0x80
asm	jz	error

asm	mov	ax,0x4407

asm	int	0x21		// get status
asm	jc	error
asm	or	al,al
asm	jz	error

asm	mov	ah,0x3e
asm	int	0x21		// close handle
asm	jc	error

//
// EMS is good
//
  return true;

error:
//
// EMS is bad
//
  return false;
}


/*
======================
=
= MML_SetupEMS
=
=======================
*/

void MML_SetupEMS (void)
{
	char	str[80],str2[10];
	unsigned	error;

	totalEMSpages = freeEMSpages = EMSpageframe = EMSpagesmapped = 0;

asm {
	mov	ah,EMS_STATUS
	int	EMS_INT						// make sure EMS hardware is present
	or	ah,ah
	jnz	error

	mov	ah,EMS_VERSION
	int	EMS_INT
	or	ah,ah
	jnz	error
	cmp	al,0x32						// only work on ems 3.2 or greater
	jb	error

	mov	ah,EMS_GETFRAME
	int	EMS_INT						// find the page frame address
	or	ah,ah
	jnz	error
	mov	[EMSpageframe],bx

	mov	ah,EMS_GETPAGES
	int	EMS_INT						// find out how much EMS is there
	or	ah,ah
	jnz	error
	mov	[totalEMSpages],dx
	mov	[freeEMSpages],bx
	or	bx,bx
	jz	noEMS						// no EMS at all to allocate

	cmp	bx,4
	jle	getpages					// there is only 1,2,3,or 4 pages
	mov	bx,4						// we can't use more than 4 pages
	}

getpages:
asm {
	mov	[EMSpagesmapped],bx
	mov	ah,EMS_ALLOCPAGES			// allocate up to 64k of EMS
	int	EMS_INT
	or	ah,ah
	jnz	error
	mov	[EMShandle],dx
	}
	return;

error:
	error = _AH;
	//strcpy (str,"MML_SetupEMS: EMS error 0x");
	SM_CopyString(str, S_SETUPEMSERROR);
	itoa(error,str2,16);
	strcpy (str,str2);
	Quit (str);

noEMS:
;
}


/*
======================
=
= MML_ShutdownEMS
=
=======================
*/

void MML_ShutdownEMS (void)
{
	if (!EMShandle)
		return;

asm	{
	mov	ah,EMS_FREEPAGES
	mov	dx,[EMShandle]
	int	EMS_INT
	or	ah,ah
	jz	ok
	}

	//Quit ("MML_ShutdownEMS: Error freeing EMS!");
	SM_Quit(S_FREEEMSERROR);

ok:
;
}

/*
====================
=
= MM_MapEMS
=
= Maps the 64k of EMS used by memory manager into the page frame
= for general use.  This only needs to be called if you are keeping
= other things in EMS.
=
====================
*/

void MM_MapEMS (void)
{
	char	str[80],str2[10];
	unsigned	error;
	int	i;

	for (i=0;i<EMSpagesmapped;i++)
	{
	asm	{
		mov	ah,EMS_MAPPAGE
		mov	bx,[i]			// logical page
		mov	al,bl			// physical page
		mov	dx,[EMShandle]	// handle
		int	EMS_INT
		or	ah,ah
		jnz	error
		}
	}

	return;

error:
	error = _AH;
	//strcpy (str,"MM_MapEMS: EMS error 0x");
	SM_CopyString(str, S_MAPEMSERROR);
	itoa(error,str2,16);
	strcpy (str,str2);
	Quit (str);
}

//==========================================================================

/*
======================
=
= MML_CheckForXMS
=
= Check for XMM driver
=
=======================
*/

boolean MML_CheckForXMS (void)
{
	numUMBs = 0;

asm {
	mov	ax,0x4300
	int	0x2f				// query status of installed diver
	cmp	al,0x80
	je	good
	}
	return false;
good:
	return true;
}


/*
======================
=
= MML_SetupXMS
=
= Try to allocate all upper memory block
=
=======================
*/

void MML_SetupXMS (void)
{
	unsigned	base,size;

asm	{
	mov	ax,0x4310
	int	0x2f
	mov	[WORD PTR XMSaddr],bx
	mov	[WORD PTR XMSaddr+2],es		// function pointer to XMS driver
	}

getmemory:
asm	{
	mov	ah,XMS_ALLOCUMB
	mov	dx,0xffff					// try for largest block possible
	call	[DWORD PTR XMSaddr]
	or	ax,ax
	jnz	gotone

	cmp	bl,0xb0						// error: smaller UMB is available
	jne	done;

	mov	ah,XMS_ALLOCUMB
	call	[DWORD PTR XMSaddr]		// DX holds largest available UMB
	or	ax,ax
	jz	done						// another error...
	}

gotone:
asm	{
	mov	[base],bx
	mov	[size],dx
	}
	MML_UseSpace (base,size);
	mminfo.XMSmem += ((longword)size)*16;	// K1n9_Duk3 fix: cast to longword to avoid overflow errors
	UMBbase[numUMBs] = base;
	numUMBs++;
	if (numUMBs < MAXUMBS)
		goto getmemory;

done:;
}


/*
======================
=
= MML_ShutdownXMS
=
======================
*/

void MML_ShutdownXMS (void)
{
	int	i;
	unsigned	base;

	for (i=0;i<numUMBs;i++)
	{
		base = UMBbase[i];

asm	mov	ah,XMS_FREEUMB
asm	mov	dx,[base]
asm	call	[DWORD PTR XMSaddr]
	}
}

//==========================================================================

/*
======================
=
= MML_UseSpace
=
= Marks a range of paragraphs as usable by the memory manager
= This is used to mark space for the near heap, far heap, ems page frame,
= and upper memory blocks
=
======================
*/

void MML_UseSpace (unsigned segstart, unsigned seglength)
{
	mmblocktype far *scan,far *last;
	unsigned	oldend;
	long		extra;

	scan = last = mmhead;
	mmrover = mmhead;		// reset rover to start of memory

//
// search for the block that contains the range of segments
//
	while (scan->start+scan->length < segstart)
	{
		last = scan;
		scan = scan->next;
	}

//
// take the given range out of the block
//
	oldend = scan->start + scan->length;
	extra = oldend - (segstart+seglength);
	if (extra < 0)
		//Quit ("MML_UseSpace: Segment spans two blocks!");
		SM_Quit(S_SEGMENTSPANSBLOCKS);

	if (segstart == scan->start)
	{
		last->next = scan->next;			// unlink block
		FREEBLOCK(scan);
		scan = last;
	}
	else
		scan->length = segstart-scan->start;	// shorten block

	if (extra > 0)
	{
		GETNEWBLOCK;
		mmnew->next = scan->next;
		scan->next = mmnew;
		mmnew->start = segstart+seglength;
		mmnew->length = extra;
		mmnew->attributes = LOCKBIT;
	}

}

//==========================================================================

/*
====================
=
= MML_ClearBlock
=
= We are out of blocks, so free a purgable block
=
====================
*/

void MML_ClearBlock (void)
{
	mmblocktype far *scan,far *last;

	scan = mmhead->next;

	while (scan)
	{
		if (!(scan->attributes&LOCKBIT) && (scan->attributes&PURGEBITS) )
		{
			MM_FreePtr(scan->useptr);
			return;
		}
		scan = scan->next;
	}

	//Quit ("MM_ClearBlock: No purgable blocks!");
	SM_Quit(S_NOPURGABLEBLOCKS);
}


//==========================================================================

/*
===================
=
= MM_Startup
=
= Grabs all space from turbo with malloc/farmalloc
= Allocates bufferseg misc buffer
=
===================
*/

//static	char *ParmStrings[] = {"noems","noxms",""};	// K1n9_Duk3 mod: don't need these anymore

void MM_Startup (void)
{
	int i;
	unsigned 	long length;
	void far 	*start;
	unsigned 	segstart,seglength,endfree;

	if (mmstarted)
		MM_Shutdown ();


	mmstarted = true;
	bombonerror = true;
//
// set up the linked list (everything in the free list;
//
	mmhead = NULL;
	mmfree = &mmblocks[0];
	for (i=0;i<MAXBLOCKS-1;i++)
		mmblocks[i].next = &mmblocks[i+1];
	mmblocks[MAXBLOCKS-1].next = NULL;

//
// locked block of all memory until we punch out free space
//
	GETNEWBLOCK;
	mmhead = mmnew;				// this will allways be the first node
	mmnew->start = 0;
	mmnew->length = 0xffff;
	mmnew->attributes = LOCKBIT;
	mmnew->next = NULL;
	mmrover = mmhead;


//
// get all available near conventional memory segments
//
	length=coreleft();
	start = (void far *)(nearheap = malloc(length));

	length -= 16-(FP_OFF(start)&15);
	length -= SAVENEARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
	MML_UseSpace (segstart,seglength);
	mminfo.nearheap = length;

//
// get all available far conventional memory segments
//
	length=farcoreleft();
	start = farheap = farmalloc(length);
	length -= 16-(FP_OFF(start)&15);
	length -= SAVEFARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
	MML_UseSpace (segstart,seglength);
	mminfo.farheap = length;
	mminfo.mainmem = mminfo.nearheap + mminfo.farheap;


//
// detect EMS and allocate up to 64K at page frame
//
	mminfo.EMSmem = 0;
#if 0
	// original code:
	for (i = 1;i < _argc;i++)
	{
		if ( US_CheckParm(_argv[i],ParmStrings) == 0)
			goto emsskip;				// param NOEMS
	}
#else
	// K1n9_Duk3 mod:
	if (SM_ParmPresent(S_NOEMS))
		goto emsskip;
#endif

	if (MML_CheckForEMS())
	{
		MML_SetupEMS();					// allocate space
		MML_UseSpace (EMSpageframe,EMSpagesmapped*0x400);
		MM_MapEMS();					// map in used pages
		mminfo.EMSmem = EMSpagesmapped*0x4000l;
	}

//
// detect XMS and get upper memory blocks
//
emsskip:
	mminfo.XMSmem = 0;
#if 0
	// original code:
	for (i = 1;i < _argc;i++)
	{
		if ( US_CheckParm(_argv[i],ParmStrings) == 1/*0*/)	// K1n9_Duk3 fix
			goto xmsskip;				// param NOXMS
	}
#else
	// K1n9_Duk3 mod:
	if (SM_ParmPresent(S_NOXMS))
		goto xmsskip;
#endif

	if (MML_CheckForXMS())
		MML_SetupXMS();					// allocate as many UMBs as possible

xmsskip:

// K1n9_Duk3 addition:
//
// allocate upper memory blocks from DOS (in case CONFIG.SYS uses DOS=UMB)
//

	if (_osmajor >= 5 && !SM_ParmPresent(S_NOUMB))	// need at least DOS 5.0 for DOS UMB allocation
		MML_SetupUMB();
// end of addition

//
// allocate the misc buffer
//
	mmrover = mmhead;		// start looking for space after low block

	MM_GetPtr (&bufferseg,BUFFERSIZE);
}

//==========================================================================

/*
====================
=
= MM_Shutdown
=
= Frees all conventional, EMS, and XMS allocated
=
====================
*/

void MM_Shutdown (void)
{
  if (!mmstarted)
	return;

  farfree (farheap);
  free (nearheap);
  MML_ShutdownEMS ();
  MML_ShutdownXMS ();
  MML_ShutdownUMB ();			// K1n9_Duk3 addition
}

//==========================================================================

/*
====================
=
= MM_GetPtr
=
= Allocates an unlocked, unpurgable block
=
====================
*/

void MM_GetPtr (memptr *baseptr,unsigned long size)
{
	mmblocktype far *scan,far *lastscan,far *endscan
				,far *purge,far *next;
	int			search;
	unsigned	needed,startseg;

	needed = (size+15)/16;		// convert size from bytes to paragraphs

	GETNEWBLOCK;				// fill in start and next after a spot is found
	mmnew->length = needed;
	mmnew->useptr = baseptr;
	mmnew->attributes = BASEATTRIBUTES;

	for (search = 0; search<6; search++)
	{
	//
	// first  search:  try to allocate right after the rover, then on up
	// second search:  search from the head pointer up to the rover
	// third  search:  compress memory, then scan from start
	// fourth search:  defrag memory, then scan from start
	// fifth  search:  stop music and defrag again
	// sixth  search:  turn sounds off and defrag again
	//
		if (search == 1 && mmrover == mmhead)
			search++;

		switch (search)
		{
		case 0:
			lastscan = mmrover;
			scan = mmrover->next;
			endscan = NULL;
			break;
		case 1:
			lastscan = mmhead;
			scan = mmhead->next;
			endscan = mmrover;
			break;
		case 2:
			MM_SortMem ();
			lastscan = mmhead;
			scan = mmhead->next;
			endscan = NULL;
			break;
		case 5:
			if (bombonerror)
			{
				SD_SetSoundMode(sdm_Off);	// this also calls SD_StopSound()
				CA_LoadAllSounds();	// make sounds purgable
			}
			// no break here!
		case 4:
			StopMusic();
			// no break here!
		case 3:
			MML_DefragMem ();
			lastscan = mmhead;
			scan = mmhead->next;
			endscan = NULL;
			break;
		}

		startseg = lastscan->start + lastscan->length;

		while (scan != endscan)
		{
			if (scan->start - startseg >= needed)
			{
			//
			// got enough space between the end of lastscan and
			// the start of scan, so throw out anything in the middle
			// and allocate the new block
			//
				purge = lastscan->next;
				lastscan->next = mmnew;
				mmnew->start = *(unsigned *)baseptr = startseg;
				mmnew->next = scan;
				while ( purge != scan)
				{	// free the purgable block
					next = purge->next;
					FREEBLOCK(purge);
					purge = next;		// purge another if not at scan
				}
				mmrover = mmnew;
				return;	// good allocation!
			}

			//
			// if this block is purge level zero or locked, skip past it
			//
			if ( (scan->attributes & LOCKBIT)
				|| !(scan->attributes & PURGEBITS) )
			{
				lastscan = scan;
				startseg = lastscan->start + lastscan->length;
			}


			scan=scan->next;		// look at next line
		}
	}

	if (bombonerror)
		//Quit ("MM_GetPtr: Out of memory!");
		SM_Quit(S_OUTOFMEMORY);
	else
		mmerror = true;
}

//==========================================================================

/*
====================
=
= MM_FreePtr
=
= Allocates an unlocked, unpurgable block
=
====================
*/

void MM_FreePtr (memptr *baseptr)
{
	mmblocktype far *scan,far *last;

	last = mmhead;
	scan = last->next;

	if (baseptr == mmrover->useptr)	// removed the last allocated block
		mmrover = mmhead;

	while (scan->useptr != baseptr && scan)
	{
		last = scan;
		scan = scan->next;
	}

	if (!scan)
		//Quit ("MM_FreePtr: Block not found!");
		SM_Quit(S_FREEPTR_BLOCKNOTFOUND);

	last->next = scan->next;

	FREEBLOCK(scan);
}
//==========================================================================

/*
=====================
=
= MM_GetSize		(K1n9_Duk3 addition)
=
= Returns the size of the block (in bytes, always rounded up to paragraphs)
=
=====================
*/

long MM_GetSize (memptr *baseptr)
{
	mmblocktype far *start;

	start = mmrover;

	do
	{
		if (mmrover->useptr == baseptr)
			break;

		mmrover = mmrover->next;

		if (!mmrover)
			mmrover = mmhead;
		else if (mmrover == start)
			//Quit ("MM_GetSize: Block not found!");
			SM_Quit(S_GETSIZE_BLOCKNOTFOUND);

	} while (1);

	return ((long)mmrover->length) << 4;
}

//==========================================================================

/*
=====================
=
= MM_SetPurge
=
= Sets the purge level for a block (locked blocks cannot be made purgable)
=
=====================
*/

void MM_SetPurge (memptr *baseptr, int purge)
{
	mmblocktype far *start;

	start = mmrover;

	do
	{
		if (mmrover->useptr == baseptr)
			break;

		mmrover = mmrover->next;

		if (!mmrover)
			mmrover = mmhead;
		else if (mmrover == start)
			//Quit ("MM_SetPurge: Block not found!");
			SM_Quit(S_SETPURGE_BLOCKNOTFOUND);

	} while (1);

	mmrover->attributes &= ~PURGEBITS;
	if (!(mmrover->attributes & NONPURGE))
		mmrover->attributes |= purge;
}

//==========================================================================

/*
=====================
=
= MM_SetLock
=
= Locks / unlocks the block
=
=====================
*/

void MM_SetLock (memptr *baseptr, mmlocktype locked)
{
	mmblocktype far *start;

	start = mmrover;

	do
	{
		if (mmrover->useptr == baseptr)
			break;

		mmrover = mmrover->next;

		if (!mmrover)
			mmrover = mmhead;
		else if (mmrover == start)
			//Quit ("MM_SetLock: Block not found!");
			SM_Quit(S_SETLOCK_BLOCKNOTFOUND);

	} while (1);

	// K1n9_Duk3 mod:
	switch (locked)
	{
	case lk_none:
		mmrover->attributes &= ~(LOCKBIT|NONPURGE);
		break;
	case lk_full:
		mmrover->attributes |= LOCKBIT;
		break;
	case lk_movable:
		mmrover->attributes |= NONPURGE;
		break;
#if 1
	default:
		//Quit("MM_SetLock: Unhandled lock type");
		SM_Quit(S_SETLOCK_BADTYPE);
#endif
	}
	// end of mod
}

//==========================================================================

static int oldborder, playing;

static void near MML_BeforeSort(void)
{
	//
	// lock down a currently playing sound
	//
	playing = SD_SoundPlaying ();
	if (playing)
	{
		switch (SoundMode)
		{
		case sdm_PC:
			playing += STARTPCSOUNDS;
			break;
		case sdm_AdLib:
			playing += STARTADLIBSOUNDS;
			break;
		}
		MM_SetLock(&(memptr)audiosegs[playing],true);
	}


	//SD_StopSound();	// K1n9_Duk3 fix
	oldborder = bordercolor;
	VW_ColorBorder (15);

	if (beforesort)
		beforesort();
}

static void near MML_AfterSort(void)
{
	if (aftersort)
		aftersort();

	VW_ColorBorder (oldborder);

	if (playing)
		MM_SetLock(&(memptr)audiosegs[playing],false);
}

/*
=====================
=
= MM_SortMem
=
= Throws out all purgable stuff and compresses movable blocks
=
=====================
*/

static void near MML_SortMem(void)
{
	mmblocktype far *scan,far *last,far *next;
	unsigned	start,length,source,dest;

	scan = mmhead;

	last = NULL;		// shut up compiler warning

	while (scan)
	{
		if (scan->attributes & LOCKBIT)
		{
		//
		// block is locked, so try to pile later blocks right after it
		//
			start = scan->start + scan->length;
		}
		else
		{
			if (scan->attributes & PURGEBITS)
			{
			//
			// throw out the purgable block
			//
				next = scan->next;
				FREEBLOCK(scan);
				last->next = next;
				scan = next;
				continue;
			}
			else
			{
			//
			// push the non purgable block on top of the last moved block
			//
				if (scan->start != start)
				{
					length = scan->length;
					source = scan->start;
					dest = start;
					mmsorting = source;	// K1n9_Duk3 addition
					
					while (length > MOVESIZE)
					{
						movedata(source,0,dest,0,MOVESIZE*16);
						length -= MOVESIZE;
						source += MOVESIZE;
						dest += MOVESIZE;
					}
					movedata(source,0,dest,0,length*16);

					scan->start = start;
					*(unsigned *)scan->useptr = start;
					
					mmsorting = 0;	// K1n9_Duk3 addition
				}
				start = scan->start + scan->length;
			}
		}

		last = scan;
		scan = scan->next;		// go to next block
	}

	mmrover = mmhead;

}

void MM_SortMem (void)
{
	MML_BeforeSort();

	MML_SortMem();

	MML_AfterSort();
}

/*
=====================
=
= MML_DefragMem		(K1n9_Duk3 addition)
=
= Moves memory blocks around to merge all free memory into one big block
=
=====================
*/

static void near MML_DefragMem(void)
{
	mmblocktype far *scan, far *lastscan, far *targetScan, far *targetEndScan,
		far *destScan, far *bestScan, far *bestLast;
	unsigned targetStart, targetEnd, targetSize, destStart, destSize, temp, source;

	//
	// prepare for memory sorting:
	//
	MML_BeforeSort();

	//
	// find biggest gap between two locked blocks (will be our target range):
	//
	targetStart=targetEnd=targetSize = 0;
	scan = mmhead;
	while (scan)
	{
		if (scan->attributes & LOCKBIT)
		{
			lastscan = scan;
			destStart = scan->start + scan->length;	// start directly after locked block
			destSize = 0;
			scan = scan->next;
			while (scan)
			{
				if (scan->attributes & LOCKBIT)
				{
					destSize = scan->start-destStart;	// end directly before the next locked block
					break;
				}
				scan = scan->next;
			}
			if (destSize > targetSize)
			{
				targetStart = destStart;
				targetEnd = destStart+destSize;
				targetSize = destSize;
				targetScan = lastscan;
				targetEndScan = scan;
			}
		}
		else
		{
			scan = scan->next;
		}
	}

	while (targetSize)
	{
		//
		// find the biggest free block OUTSIDE the target range
		// (will be our "dest" area)
		//
		temp=destSize=destStart=0;
		destScan = NULL;
		scan = mmhead;
		while (scan)
		{
			if (scan->start != temp)	// unused space between blocks?
			{
				unsigned size = scan->start-temp;
				if (size > destSize && !(temp >= targetStart && temp+size <= targetEnd))
				{
					destStart = temp;
					destSize = size;
					destScan = lastscan;
				}
			}
			if (scan == targetScan)
			{
				scan = targetEndScan;	// skip the blocks in the target area
			}
			temp = scan->start+scan->length;
			lastscan = scan;
			scan = scan->next;
		}

		if (!destSize)	// no free memory found?
			break;

		//
		// find the biggest block inside the target range that can be
		// moved over into the "dest" area (will be our "best" block)
		//
		scan = targetScan->next;
		lastscan = targetScan;
		temp = 0;
		while (scan && scan->start < targetEnd)
		{
			if (scan->start >= targetStart && !(scan->attributes & LOCKBIT))
			{
				if (scan->length == destSize)
				{
					bestScan = scan;
					bestLast = lastscan;
					goto move_it;
				}
				else if (scan->length < destSize && scan->length > temp)
				{
					temp = scan->length;
					bestScan = scan;
					bestLast = lastscan;
				}
			}
			lastscan = scan;
			scan = scan->next;
		}

		if (!temp)	// no block found in target range?
			break;

move_it:
		//
		// move the "best" block to the "dest" area
		//
		mmsorting = bestScan->start;

		bestLast->next = bestScan->next;	// unlink "best" node at old spot
		bestScan->next = destScan->next;	// insert "best" node at new spot
		destScan->next = bestScan;
		
		destSize = bestScan->length;
		temp = destStart;
		source = bestScan->start;

		// move the data
		while (destSize > MOVESIZE)
		{
			movedata(source,0,destStart,0,MOVESIZE*16);
			source += MOVESIZE;
			destStart += MOVESIZE;
			destSize -= MOVESIZE;
		}
		movedata(source,0,destStart,0,destSize*16);
		
		(*(unsigned*)bestScan->useptr) = bestScan->start = temp;	// update addresses
		
		mmsorting = 0;
		
//		MM_ShowMemory();	// for debugging
	}
	
	//
	// compress blocks in target range to merge free spaces into one big block
	//
	MML_SortMem();
	MML_AfterSort();
}

//==========================================================================

#ifdef DEVBUILD
/*
=====================
=
= MM_ShowMemory
=
=====================
*/

void MM_ShowMemory (void)
{
	mmblocktype far *scan;
	unsigned color,temp;
	long	end,owner;
	char    scratch[80],str[10];

	VW_SetDefaultColors();
	VW_SetLineWidth(40);
	temp = bufferofs;
	bufferofs = 0;
	VW_SetScreen (0,0);

	scan = mmhead;

	end = -1;

//CA_OpenDebug ();

	while (scan)
	{
		if (scan->attributes & PURGEBITS)
			color = 5;		// dark purple = purgable
		else
			color = 9;		// medium blue = non purgable
		if (scan->attributes & LOCKBIT)
			color = 12;		// red = locked
/*		if (scan->attributes & NONPURGE)
			color = 1;		// dark blue = can move but not purge
		if (!scan->useptr)
			color = 4;		// dark red = unmanaged (and usually locked)
*/		if (scan->start<=end)
			//Quit ("MM_ShowMemory: Memory block order currupted!");
			SM_Quit(S_BLOCKORDERCORRUPTED);
		end = scan->start+scan->length-1;
		VW_Hlin(scan->start,(unsigned)end,0,color);
		VW_Plot(scan->start,0,15);	// white = start
		if (scan->next->start > end+1)
			VW_Hlin(end+1,scan->next->start,0,0);	// black = free

#if 0
strcpy (scratch,"Size:");
ltoa ((long)scan->length*16,str,10);
strcat (scratch,str);
strcat (scratch,"\tOwner:0x");
owner = (unsigned)scan->useptr;
ultoa (owner,str,16);
strcat (scratch,str);
strcat (scratch,"\n");
write (debughandle,scratch,strlen(scratch));
#endif

		scan = scan->next;
	}

//CA_CloseDebug ();

	IN_Ack();
	VW_SetLineWidth(64);
	bufferofs = temp;
}
//==========================================================================
#endif	// DEVBUILD


/*
======================
=
= MM_UnusedMemory
=
= Returns the total free space without purging
=
======================
*/

long MM_UnusedMemory (void)
{
	unsigned free;
	mmblocktype far *scan;

	free = 0;
	scan = mmhead;

	while (scan->next)
	{
		free += scan->next->start - (scan->start + scan->length);
		scan = scan->next;
	}

	return free*16l;
}

//==========================================================================


/*
======================
=
= MM_TotalFree
=
= Returns the total free space with purging
=
======================
*/

long MM_TotalFree (void)
{
	unsigned free;
	mmblocktype far *scan;

	free = 0;
	scan = mmhead;

	while (scan->next)
	{
		if ((scan->attributes&PURGEBITS) && !(scan->attributes&LOCKBIT))
			free += scan->length;
		free += scan->next->start - (scan->start + scan->length);
		scan = scan->next;
	}

	return free*16l;
}

//==========================================================================

/*
=====================
=
= MM_BombOnError
=
=====================
*/

void MM_BombOnError (boolean bomb)
{
	bombonerror = bomb;
}



#ifdef DEVBUILD
/*
=====================
=
= MM_BlocksFree	(K1n9_Duk3 addition)
=
= Returns number of free memory blocks
=
=====================
*/

unsigned MM_BlocksFree(void)
{
	unsigned n;
	mmblocktype far *block;
	
	for (n=0, block=mmfree; block; block = block->next, n++);
	
	return n;
}

/*
=====================
=
= MM_BlocksPurgable	(K1n9_Duk3 addition)
=
= Returns number of purgable memory blocks
=
=====================
*/

unsigned MM_BlocksPurgable(void)
{
	unsigned n;
	mmblocktype far *block;
	
	for (n=0, block=mmhead; block; block=block->next)
	{
		if ((block->attributes & PURGEBITS) && !(block->attributes & LOCKBIT))
			n++;
	}
	
	return n;
}
#endif	// DEVBUILD
