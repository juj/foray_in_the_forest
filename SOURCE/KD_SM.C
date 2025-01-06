/* "Foray in the Forest" Source Code
 * Copyright (C) 2019-2021 K1n9_Duk3
 *
 * Some parts taken from Catacomb 3-D Source Code
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
// KD_SM.C

/*
=============================================================================

K1n9_Duk3's String Manager
---------------------------

Must be started BEFORE all the other managers, because they now rely on the
string manager to produce the correct error messages

=============================================================================
*/

#include "ID_HEADS.H"

extern void CK_MeasureMultiline(const char *str, unsigned *w, unsigned *h);

typedef unsigned /*long*/ offset_t;	// as long as the string file is below 64k, we don't need long offsets

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

static int sm_handle = -1; 
static char sm_filename[] = "GAMESTR."EXTENSION;

//K1n9_Duk3 patch:
static char sm_buffer[MAXGAMESTRINGLENGTH+1];	// +1 to make sure there's a 0 at the end
static offset_t sm_bufferstart = 0xFFFFFFFFl;
static stringnames sm_bufferednum = NUMGAMESTRINGS;	// not a valid number!

//===========================================================================

/*
=======================
=
= SM_Startup
=
=======================
*/
/*
void SM_Startup(void)
{
	sm_handle = open(sm_filename, O_BINARY|O_RDONLY);
	if (sm_handle == -1)
	{
		Quit(sm_filename);	// not much of an error message, but we're trying to save space
	}
}
*/
// K1n9_Duk3 patch:
void SM_Startup(void)
{
	sm_handle = open(sm_filename, O_BINARY|O_RDONLY);
	if (sm_handle != -1)
	{
		// first offset should be (NUMGAMESTRINGS+1)*sizeof(offset_t)
		offset_t test;
		if (read(sm_handle, &test, sizeof(test)) == sizeof(test) && test == (NUMGAMESTRINGS+1)*sizeof(offset_t))
		{
			// all checks passed -- string file is OK
			return;
		}
	}
	
	// string file is missing, out of date, or corrupted
	Quit(sm_filename);	// not much of an error message, but we're trying to save space
}

/*
=======================
=
= SM_Shutdown
=
=======================
*/

void SM_Shutdown(void)
{
	if (sm_handle != -1)
	{
		close(sm_handle);
		sm_handle = -1;
	}
}

/*
=======================
=
= SM_GetString
= Load desired string into buffer and return a (near) pointer to it
=
=======================
*/

char * near SML_GetString(stringnames num)
{
	offset_t offsets[2];	// two offsets, so we can calculate size from that
	/*
	if (num < 0 || num >= NUMGAMESTRINGS)
		return sm_filename+sizeof(sm_filename)-1;	// pointer to the terminating nul character in the file name
	*/
	// Nisaba patch
		if ((unsigned)num >= (unsigned)NUMGAMESTRINGS)
		return sm_filename+sizeof(sm_filename)-1;	// pointer to the terminating nul character in the file name
		
	// avoid reading offsets if we don't have to
	if (num == sm_bufferednum)
		return sm_buffer;

	// TODO: keep offsets in memory (in dynamic buffer)
	lseek(sm_handle, num*sizeof(offset_t), SEEK_SET);
	read(sm_handle, offsets, sizeof(offsets));

	// only re-load when desired string is NOT in the buffer:
	if (offsets[0] >= sm_bufferstart && offsets[1] < sm_bufferstart+MAXGAMESTRINGLENGTH)
	{
		return sm_buffer+(offsets[0]-sm_bufferstart);
	}
	else
	{
		lseek(sm_handle, offsets[0], SEEK_SET);
		read(sm_handle, sm_buffer, MAXGAMESTRINGLENGTH);
		sm_bufferstart = offsets[0];
		sm_bufferednum = num;
		return sm_buffer;
	}
}

char * SM_GetString(stringnames num)
{
	return SML_GetString(num);
}

/*
=============================================================================

		DROP-IN REPLACEMENTS FOR SOME COMMON ROUTINES

=============================================================================
*/

char * SM_CopyString(char *dest, stringnames num)
{
	return strcpy(dest, SML_GetString(num));
}

char * SM_ConcatString(char *dest, stringnames num)
{
	return strcat(dest, SML_GetString(num));
}

void SM_Quit(stringnames message)
{
	Quit(SML_GetString(message));
}

void SM_Print(stringnames num)
{
	US_Print(SML_GetString(num));
}

void SM_CPrint(stringnames num)
{
	US_CPrint(SML_GetString(num));
}

void SM_PrintCentered(stringnames num)
{
	US_PrintCentered(SML_GetString(num));
}

void SM_MeasureMultiline(stringnames num, unsigned *w, unsigned *h)
{
	CK_MeasureMultiline(SML_GetString(num), w, h);
}

/*
int SM_CheckParm(char *parm, stringnames *nums)
{
	char    cp,cs,
			*p,*s;
	int             i;

	while (!isalpha(*parm)) // Skip non-alphas
		parm++;

	for (i = 0; nums[i]>0; i++)
	{
		for (s = SML_GetString(nums[i]),p = parm,cs = cp = 0;cs == cp;)
		{
			cs = *s++;
			if (!cs)
				return(i);
			cp = *p++;

			if (isupper(cs))
				cs = tolower(cs);
			if (isupper(cp))
				cp = tolower(cp);
		}
	}
	return(-1);
}

boolean SM_CheckArg(stringnames num)
{
	char* strings[2];
	int i;

	strings[0] = SML_GetString(num);
	strings[1] = NULL;

	for (i=1; i<_argc; i++)
	{
		if (US_CheckParm(_argv[i], strings) != -1)
			return true;
	}
	return false;
}
*/
// K1n9_Duk3 patch:
int SM_CheckParm(char *parm, stringnames *nums)
{
	char	cp,cs,*p,*s;
	int	i;

	p = parm;
	// Note: isalpha() doesn't work correctly when using signed char values!
	
	// Nisaba patch:
//	while (*p != '?' && !isalpha((unsigned char)*p)) // Skip non-alphas, except '?'
	while (*p != 0 && *p != '?' && !isalpha((unsigned char)*p)) // Skip non-alphas, except '?'
		p++;
	parm = p;	// remember start of parameter for later

	for (i = 0; nums[i]>=0; i++)
	{
		s = SML_GetString(nums[i]);
		do
		{
			cs = tolower(*s);s++;
			cp = tolower(*p);p++;
			if ((cp | cs) == 0)
				return(i);
		} while (cp == cs);
		p = parm;	// back to start of parameter
	}
	return(-1);
}

// Nisaba patch:
boolean SM_ParmPresent(stringnames num)
{
	stringnames strings[2];
	int i;

	strings[0] = num;
	strings[1] = -1;

	for (i=1; i<_argc; i++)
	{
		if (SM_CheckParm(_argv[i], strings) != -1)
			return true;
	}
	return false;
}

void SM_DrawString(stringnames num)
{
	USL_DrawString(SML_GetString(num));
}

void SM_QuitCannotOpen(char *filename)
{
	static char end[] = "!";
	char str[32];

	SM_CopyString(str, S_CANNOTOPEN);
	strcat(str, filename);
	strcat(str, end);
	Quit(str);
}

void SM_MeasureString(stringnames num, word *w, word *h)
{
	USL_MeasureString(SML_GetString(num), w, h);
}

void SM_PutLines(stringnames first, stringnames last)
{
	while (first <= last)
	{
		puts(SML_GetString(first++));
	}
}
