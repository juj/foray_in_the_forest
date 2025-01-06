/* "Foray in the Forest" Source Code
 * Copyright (C) 2019-2020 K1n9_Duk3
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

#ifndef __KD_SM_H__
#define __KD_SM_H__

#include "GAMESTR.H"

void SM_Startup(void);
void SM_Shutdown(void);

char * SM_GetString(stringnames num);

char * SM_CopyString(char *dest, stringnames num);
char * SM_ConcatString(char *dest, stringnames num);
void SM_Quit(stringnames message);
void SM_Print(stringnames num);
void SM_CPrint(stringnames num);
void SM_PrintCentered(stringnames num);
void SM_MeasureMultiline(stringnames num, unsigned *w, unsigned *h);
int SM_CheckParm(char *parm, stringnames *nums);
boolean SM_ParmPresent(stringnames num);
void SM_MeasureString(stringnames num, word *w, word *h);
void SM_PutLines(stringnames first, stringnames last);
void SM_DrawString(stringnames num);
void SM_QuitCannotOpen(char *filename);

#endif
