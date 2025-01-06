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

// KD_GAME.C

#include "CK_DEF.H"
#pragma	hdrstop

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define RLETAG  0xABCD

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

int			fadecount;

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

int			ck_lastLevelFinished;
//==========================================================================



//===========================================================================

// K1n9_Duk3 additions:

// Addition for the collectibles in FITF.
// HandleLevelDone must be called after winning each level and before loading a
// new level!

void near HandleLevelDone(void)
{
	// add the collectible(s) collected in the current level to the collection:
	ck_gameState.collectiblesTotal |= ck_mapState.collectiblesNew;

	ck_gameState.janitorsSpawned |= ck_mapState.janitorsSpawned;
	// ck_gameState.janitorsSkipped |= ck_mapState.janitorsSkipped;

	// allow Keen to keep the current points:
	ck_gameState.oldScore = ck_gameState.keenScore;

	ck_gameState.slugcans += ck_mapState.slugcans;
			
	ck_mapState.doneFlags |= DF_DONE;

	if (ck_gameState.cheated)
		ck_mapState.doneFlags |= DF_CHEATED;
	
	if (ck_mapState.itemsCollected && ck_mapState.itemsCollected == ck_mapState.itemsTotal)
		ck_mapState.doneFlags |= DF_PERFECT;

#if 0
	// quick hack for alpha-testing
	if (ck_gameState.currentLevel)	// only show statistics when NOT on world map
	{
		VW_FixRefreshBuffer ();
		US_CenterWindow(26, 8);
		US_CPrint("ALPHA Statistics:\n");
		sprintf(str, "%u of %u bonus items\n%lu of %lu points\n", ck_mapState.itemsCollected, ck_mapState.itemsTotal, ck_mapState.pointsCollected, ck_mapState.pointsTotal);
		US_CPrint(str);
		sprintf(str, "%u of %u lives\n%u of %u drops\n", ck_mapState.livesCollected, ck_mapState.livesTotal, ck_mapState.dropsCollected, ck_mapState.dropsTotal);
		US_CPrint(str);
		sprintf(str, "%u of %u stunners", ck_mapState.ammoCollected, ck_mapState.ammoTotal);
		US_CPrint(str);
		VW_UpdateScreen();
		VW_WaitVBL(30);
		IN_ClearKeysDown();
		IN_Ack();
	}
#endif
}

// Nisaba patch:
// end of additions
static void near pascal ApplyDifficulty(GameDiff difficulty)
{
        switch (difficulty)
        {
        case gd_Easy:
                ck_gameState.numLives = 5;
                ck_gameState.numShots = 10;
                break;
        case gd_Normal:
                ck_gameState.numLives = 3;
                ck_gameState.numShots = 5;
                break;
        case gd_Hard:
                ck_gameState.numLives = 2;
                ck_gameState.numShots = 3;
                break;
        }
}

void CK_NewGame(void)
{
	memset(&ck_gameState, 0, sizeof(ck_gameState));
	ck_gameState.nextKeenAt = 20000;
	ck_gameState.numLives = DEFAULT_LIVES;
	ck_gameState.numShots = DEFAULT_SHOTS;
//	ck_gameState.currentLevel = 0;	// already set to 0 by memset

//	UndoWallDebug();
	
	// Nisaba patch:
	if (NoWait)
        {
            ck_gameState.mapPosX = ck_gameState.mapPosY = RF_TileToUnit(29);
            ck_gameState.levelsDone[MAP_BWBMEGAROCKET] = DF_DONE;
        }

#ifdef DEVBUILD
    UndoWallDebug();
#endif
}

void CK_GameOver(void)
{
	// HACK: US_CPrint modifies strings. So don't use
	// literals directly. (char * is NOT ok; Use char arrays.)
//	static char gameOverString[] = "Game Over!";

	VWB_Bar(panx&7, 0, 320, 200, BLACK);	// VWB_Bar adds pansx and pansy, so we must either add the low 3 bits of panx (equivalent to panx-pansx) to the x coordinate or increase the width by 8 to get the result we need
	US_CenterWindow(16, 3);
//	US_PrintCentered(gameOverString);
	SM_PrintCentered(S_GAMEOVER);
	IN_ClearKeysDown();
	FizzleDown(1);	// fizzle at double speed
	VW_UpdateScreen();
	IN_ClearKeysDown();
	IN_UserInput(4*TickBase, false);
}

#define SAVECOST	1	// how man lives does saving cost?

boolean SavingWillCost(void)
{
	return ck_gameState.currentLevel != 0 && ck_gameState.currentLevel != 1 && ck_gameState.difficulty != D_Easy;
}

boolean SavingAllowed(void)
{
	return !SavingWillCost() || ck_gameState.numLives >= SAVECOST;
}

boolean SaveGameStuff(int handle)
{
	int i;
	unsigned bufsize;

//	ck_keenState.platform = NULL;
	ck_gameState.brainfucled = brainfucled;

	if (!CA_FarWrite(handle, (byte far *)&ck_gameState, sizeof (ck_gameState)))
		return false;

	if (!CA_FarWrite(handle, (byte far *)&ck_mapState, sizeof (ck_mapState)))
		return false;

	bufsize = mapwidth * mapheight * 2;

	// Save the three map planes (no compression!)
	for (i = 0; i < 3; i++)
	{
		if (!CA_FarWrite(handle, (byte far *)mapsegs[i], bufsize))
			return false;
	}

	// K1n9_Duk3 fix: save camera position and tile animation states
	if (!RF_SaveState(handle))
		return false;
	// end of fix

	// Save the object list

	// Note: This code saves the ENTIRE object list, which allows the
	// game to restore it to the correct state and keep any object
	// pointers in the user variables intact.
	/*
	if (write(handle, &ck_freeObject, sizeof(ck_freeObject)) == -1)
		return false;
	if (write(handle, &ck_lastObject, sizeof(ck_lastObject)) == -1)
		return false;
	if (write(handle, &ck_numObjects, sizeof(ck_numObjects)) == -1)
		return false;
	if (write(handle, &ck_objArray, sizeof(ck_objArray)) == -1)
		return false;
	*/
	// Nisaba patch to overcome possible save game read/write buffer errors:
	if (write(handle, &ck_freeObject, sizeof(ck_freeObject)) != sizeof(ck_freeObject))
		return false;
	if (write(handle, &ck_lastObject, sizeof(ck_lastObject)) != sizeof(ck_lastObject))
		return false;
	if (write(handle, &ck_numObjects, sizeof(ck_numObjects)) != sizeof(ck_numObjects))
		return false;
	if (write(handle, &ck_objArray, sizeof(ck_objArray)) != sizeof(ck_objArray))
		return false;
	// We don't need to save the ck_keenObj and ck_scoreBoxObj pointers
	// since they are ALWAYS assigned to index 0 and 1 in the object
	// list (see CK_CK_SetupObjArray).

	return true;
}

boolean LoadGameStuff(int handle)
{
	int i; 
	unsigned bufsize;
	CK_object *ob;

	// NOTICE: This is supposed to be called AFTER the gamestate struct
	// was read and the level was initialized!

	if( !CA_FarRead( handle, (byte far *)&ck_mapState, sizeof (ck_mapState) ) )
		return false;

	bufsize = mapwidth * mapheight * 2;

	// Load the three map planes (no compression!)
	for (i = 0; i < 3; i++)
	{
		if (!CA_FarRead(handle, (byte far *)mapsegs[i], bufsize))
			return false;
	}

	// K1n9_Duk3 fix: load camera position and tile animation states
	if (!RF_LoadState(handle))
		return false;
	// end of fix

	// Load object list
	//CK_SetupObjArray();	// don't really need this, but it wouldn't cause issues
	/*
	if (read(handle, &ck_freeObject, sizeof(ck_freeObject)) == -1)
		return false;
	if (read(handle, &ck_lastObject, sizeof(ck_lastObject)) == -1)
		return false;
	if (read(handle, &ck_numObjects, sizeof(ck_numObjects)) == -1)
		return false;
	if (read(handle, &ck_objArray, sizeof(ck_objArray)) == -1)
		return false;
	*/
	// Nisaba patch to overcome possible save game read/write buffer errors
	if (read(handle, &ck_freeObject, sizeof(ck_freeObject)) != sizeof(ck_freeObject))
		return false;
	if (read(handle, &ck_lastObject, sizeof(ck_lastObject)) != sizeof(ck_lastObject))
		return false;
	if (read(handle, &ck_numObjects, sizeof(ck_numObjects)) != sizeof(ck_numObjects))
		return false;
	if (read(handle, &ck_objArray, sizeof(ck_objArray)) != sizeof(ck_objArray))
		return false;
	// Clear the sprite pointers for all the objects and force them to redraw:
	for (ob=ck_keenObj; ob; ob=ob->next)
	{
		ob->visible = true;	// force redraw
		ob->sde = NULL;	// clear sprite pointer

		switch (ob->type)
		{
		case CT_FallBlock:
		case CT_StunnedCreature:
		case CT_Slug:
			ob->user3 = 0;	// clear sprite pointer for the stunned stars/foreground/donut sprite
			break;
		case CT_Platform:
			ob->user2 = ob->user3 = 0;	// clear sprite pointer for the thrusters
			break;
		}
	}

	// Note: Since we're loading the entire object list, any links between the
	// objects stay intact and we don't have to reset those pointers in here.

	ck_scoreBoxObj->user1 = -1;
	ck_scoreBoxObj->user2 = -1;
	ck_scoreBoxObj->user3 = -1;
	ck_scoreBoxObj->user4 = -1;

	return true;
}

static char checkpointfile[] = "CHKPOINT."EXTENSION;

boolean LoadCheckpoint(void)
{
	CK_GameState oldstate = ck_gameState;
	boolean result;
	int handle = open(checkpointfile, O_BINARY|O_RDONLY);

	if (handle == -1)
		return false;

	if( !CA_FarRead( handle, (byte far *)&ck_gameState, sizeof (ck_gameState) ) )
		return false;

	// certain parts of the gamestate should not reset:
	if (ck_gameState.numShots < DEFAULT_SHOTS)
		ck_gameState.numShots = DEFAULT_SHOTS;
	if (ck_gameState.numShots < oldstate.numShots)
		ck_gameState.numShots = oldstate.numShots;
	ck_gameState.numLives = oldstate.numLives;
	ck_gameState.nextKeenAt = oldstate.nextKeenAt;
	ck_gameState.numVitalin = oldstate.numVitalin;
	ck_gameState.janitorMessageShown = oldstate.janitorMessageShown;
	
	
#ifdef DEVBUILD		// Nisaba patch:
	if (!ck_gameState.cheated)
		UndoWallDebug();
#endif

#if 0
	// don't mess with cache levels before/after loading the level!
	CK_LoadLevel(true);	// re-load level and also cache in all the graphics
	if (mmerror)
	{
		mmerror = false;
		return false;
	}
#else
	// I think we don't actually need to re-load the level,
	// we just need to reset parts of the refresh manager:
	{
		void RFL_InitSpriteList (void);

		RFL_InitSpriteList ();
		// We don't need to call RFL_InitAnimList here, RF_NewPosition takes care of that.
		
		//Nisaba patch:
		// make sure any auto-cached special death sprites are purgable:
		CA_UncacheGrChunk(KEENBURNED8SPR);
		CA_UncacheGrChunk(KEENQUICKSANDSPR);
		CA_UncacheGrChunk(NESSI4SPR);
		}
#endif

	result = LoadGameStuff(handle);
	close(handle);
	return result;
}

boolean SaveCheckpoint(void)
{
	int handle = open(checkpointfile, O_CREAT|O_BINARY|O_WRONLY|O_TRUNC, S_IREAD|S_IWRITE|S_IFREG);

	if (handle == -1)
		return false;

	ck_mapState.checkpointSaved = true;

	ck_mapState.checkpointSaved = SaveGameStuff(handle);
	close(handle);
	return ck_mapState.checkpointSaved;
}

/*
boolean CopyCheckpoint(int infile, int outfile)
{
	extern boolean bombonerror;
	boolean oldbomb;
	memptr buffer;
	unsigned bufsize = mapwidth * mapheight * 2;

// Note: The smallest possible valid map size is 24 x 17 tiles, so the buffer
// size is at least 816 bytes. That should be more than enough to store
// CK_GameState and CK_MapState, and also the CK_object structs.

#if (max(sizeof(ck_gameState)+sizeof(ck_mapState), sizeof(CK_object)) > 816)
#error CopyCheckpoint buffer is not big enough!
#endif

#if 0
	// if you're paranoid, use this to make sure there won't be any issues
	if (bufsize < max(sizeof(ck_gameState)+sizeof(ck_mapState), sizeof(CK_object)))
		goto slow_version;
#endif

	oldbomb = bombonerror;
	MM_BombOnError(false);
	MM_GetPtr(&buffer, bufsize);
	MM_BombOnError(oldbomb);
	if (mmerror)
	{
		mmerror = false;
slow_version:
		while (!eof(infile))
		{
			// Using a variable in the data segment for the read/write buffer
			// should be faster than using a stack variable. I'll just use the
			// tedlevelnum variable, since it is only used at startup.
			if (read(infile, &tedlevelnum, 1) != 1)
				return false;
			if (write(outfile, &tedlevelnum, 1) != 1)
				return false;
		}
		return true;
	}
	else
	{
		boolean ok = false;
		register int i;

		if (!CA_FarRead(infile, buffer, sizeof(ck_gameState)+sizeof(ck_mapState)))
			goto done;
		if (!CA_FarWrite(outfile, buffer, sizeof(ck_gameState)+sizeof(ck_mapState)))
			goto done;

		for (i=0; i<3; i++)
		{
			if (!CA_FarRead(infile, buffer, bufsize))
				goto done;
			if (!CA_FarWrite(outfile, buffer, bufsize))
				goto done;
		}

		if (!CA_FarRead(infile, buffer, 10))
			goto done;
		if (!CA_FarWrite(outfile, buffer, 10))
			goto done;

		for (i=0; i<CK_MAX_OBJECTS; i++)
		{
			if (!CA_FarRead(infile, buffer, sizeof(CK_object)))
				goto done;
			if (!CA_FarWrite(outfile, buffer, sizeof(CK_object)))
				goto done;
		} 

		ok = true;
done:
		MM_FreePtr(&buffer);
		return ok;
	}
}
*/
// Nisaba patch to overcome possible save game read/write buffer errors
boolean CopyCheckpoint(int infile, int outfile)
{
	unsigned long bytesToCopy = filelength(infile) - tell(infile);
	
	// This uses the always available misc buffer to copy any remaining bytes
	// from infile to outfile.
	
	while (bytesToCopy)
	{
		if (bytesToCopy <= BUFFERSIZE)
		{
			if (!CA_FarRead(infile, bufferseg, bytesToCopy))
				return false;
			if (!CA_FarWrite(outfile, bufferseg, bytesToCopy))
				return false;
			
			return true;
		}
		else
		{
			if (!CA_FarRead(infile, bufferseg, BUFFERSIZE))
				return false;
			if (!CA_FarWrite(outfile, bufferseg, BUFFERSIZE))
				return false;
			
			bytesToCopy -= BUFFERSIZE;
		}
	}
	return true;
}

boolean
SaveTheGame(int handle)
{
	// K1n9_Duk3 addition: saving mid-level costs a life (except on easy mode)
	if (SavingWillCost())
	{
		ck_gameState.numLives -= SAVECOST;
	}

	if (!SaveGameStuff(handle))
		return false;

	if (ck_mapState.checkpointSaved)
	{
		boolean ok;
		int handle2 = open(checkpointfile, O_BINARY|O_RDONLY);

		if (handle2 == -1)
			return false;

		ok = CopyCheckpoint(handle2, handle);

		close(handle2);

		return ok;
	}

	return true;
}

boolean
LoadTheGame(int handle)
{
	if( !CA_FarRead( handle, (byte far *)&ck_gameState, sizeof (ck_gameState) ) )
		return false;

//Nisaba patch:
#ifdef DEVBUILD
	if (!ck_gameState.cheated)
		UndoWallDebug();
#endif

	ca_levelbit >>= 1;
	ca_levelnum--;
/*
	CK_LoadLevel(false);
	if( mmerror ) {
		mmerror = 0;
		US_CenterWindow( 20, 8 );
		PrintY += 20;
		//US_CPrint( "Not enough memory\nto load game!" );
		SM_CPrint(S_LOADGAME_NOMEM);
		VW_UpdateScreen();
		IN_Ack();
		return false;
	}
	
	ca_levelbit <<= 1;
	ca_levelnum++;
*/
	CK_LoadLevel(false);
	
	ca_levelbit <<= 1;
	ca_levelnum++;

	if (mmerror)
	{
		mmerror = 0;
		US_CenterWindow( 20, 8 );
		PrintY += 20;
	//	US_CPrint( "Not enough memory\nto load game!" );
		SM_CPrint(S_LOADGAME_NOMEM);
		VW_UpdateScreen();
		IN_Ack();
		return false;
	}
	
	if (ck_mapState.brainfucl && ck_gameState.brainfucled != brainfucled)
	{
#if true
		// loading the game restarts the current level:
		ck_gameState.keenScore = ck_gameState.oldScore;
		ck_mapState.checkpointSaved = false;
		CK_CentreCamera(ck_keenObj);
		return true;
#else
		// loading the game causes an error:
		errno = -1;
		return false;
#endif
	}

	if( !LoadGameStuff(handle))
		return false;

	if (ck_mapState.checkpointSaved)
	{
		int handle2 = open(checkpointfile, O_CREAT|O_BINARY|O_WRONLY|O_TRUNC, S_IREAD|S_IWRITE|S_IFREG);

		if (handle2 == -1)
		{
			ck_mapState.checkpointSaved = false;
		}
		else
		{
			if (!CopyCheckpoint(handle, handle2))
				ck_mapState.checkpointSaved = false;

			close(handle2);
		}
	}
	SD_StopSound();	// K1n9_Duk3 addition
	return true;
}

void CK_ResetGame (void)
{
	CK_NewGame();
	ca_levelnum--;
	ca_levelbit >>= 1;
	CA_ClearMarks();
	ca_levelbit <<= 1;
	ca_levelnum++;
	
/*
	switch (restartgame)
	{
	case gd_Easy:
		ck_gameState.numLives = 5;
		ck_gameState.numShots = 10;
		break;
	case gd_Normal:
		ck_gameState.numLives = 3;
		ck_gameState.numShots = 5;
		break;
	case gd_Hard:
		ck_gameState.numLives = 2;
		ck_gameState.numShots = 3;
		break;
	}
*/
	//Nisaba patch:
	ApplyDifficulty(restartgame);
}

//===========================================================================
//
// PALETTE
//
//===========================================================================

// Note: The VGA palettes are using 6-bit RGB values. Therefor every 8-bit
// colour value needs to be devided by 4. For e.g.:
// #00A880 = 0x00, 0x2A, 0x20

byte far defaultpalette[] = {
	0x00, 0x00, 0x00, 0x00,	// black	    #000000
	0x01, 0x00, 0x00, 0x2A,	// dark blue	#0000A8
	0x02, 0x00, 0x2A, 0x00,	// dark green	#00A800
	0x03, 0x00, 0x2A, 0x2A,	// dark cyan	#00A8A8
	0x04, 0x2A, 0x00, 0x00,	// dark red	    #A80000
	0x05, 0x2A, 0x00, 0x2A,	// dark magenta	#A800A8
	0x06, 0x2A, 0x15, 0x00,	// dark orange	#A85400
	0x07, 0x2A, 0x2A, 0x2A,	// grey		    #A8A8A8
	0x18, 0x15, 0x15, 0x15,	// dark grey	#545454
	0x19, 0x15, 0x15, 0x3F,	// light blue	#5454FC
	0x1A, 0x15, 0x3F, 0x15,	// light green	#54FC54
	0x1B, 0x15, 0x3F, 0x3F,	// light cyan	#54FCFC
	0x1C, 0x3F, 0x15, 0x15,	// light red    #FC5454
	0x1D, 0x3F, 0x15, 0x3F,	// magenta		#FC54FC
	0x1E, 0x3F, 0x3F, 0x15,	// yellow 		#FCFC54
	0x1F, 0x3F, 0x3F, 0x3F,	// white		#FCFCFC
};

byte far worldmappalette[] = {	// Worldmap
	0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x2A,
	0x02, 0x01, 0x28, 0x00,	// dark lime green         #04A000
	0x03, 0x00, 0x2A, 0x2A, // ...
	0x04, 0x2A, 0x00, 0x00,
	0x05, 0x2A, 0x00, 0x2A,
	0x06, 0x2A, 0x15, 0x00,
	0x07, 0x2A, 0x2A, 0x2A,
	0x18, 0x15, 0x15, 0x15,
	0x19, 0x15, 0x15, 0x3F,
	0x1A, 0x08, 0x3D, 0x08, // vivid lime green         #20f422
//	0x1B, 0x00, 0x2A, 0x1C,	// light cyan -> lime green	#00A870
	0x1B, 0x05, 0x29, 0x17,	// cyan green /lime green	#14A45C
	0x1C, 0x3F, 0x15, 0x15,
	0x1D, 0x3F, 0x15, 0x3F,
	0x1E, 0x3F, 0x3F, 0x15,
	0x1F, 0x3F, 0x3F, 0x3F,
};	


byte far level4palette[] = {	// Secret Reservoir
	0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x23, // darker blue				#00008	
	0x02, 0x00, 0x2C, 0x09,	
	0x03, 0x00, 0x2A, 0x2A,
	0x04, 0x2A, 0x00, 0x00,
	0x05, 0x2A, 0x00, 0x2A,
	0x06, 0x28, 0x18, 0x01,	// dark orange / brown tone #a26106
	0x07, 0x2A, 0x2A, 0x2A,
	0x18, 0x15, 0x16, 0x19, // very dark grayish yellow	#606050
	0x19, 0x10, 0x10, 0x3D, // bright blue				#4444F4
	0x1A, 0x18, 0x3B, 0x18, // soft lime green			#61ef61
	0x1B, 0x15, 0x39, 0x3F,	// light green -> soft cyan	#54e4fc	
	0x1C, 0x3F, 0x15, 0x15,
	0x1D, 0x3F, 0x15, 0x3F,
	0x1E, 0x3F, 0x3F, 0x15,
	0x1F, 0x3F, 0x3F, 0x3F,
};	

byte far level5palette[] = {	// Spiky Village
	0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x2A,
	0x02, 0x00, 0x26, 0x10,	// dark green -> dark lime green	#009840
	0x03, 0x00, 0x2A, 0x2A,
	0x04, 0x2A, 0x00, 0x00,
	0x05, 0x2A, 0x00, 0x2A,
	0x06, 0x2A, 0x15, 0x00,
	0x07, 0x2A, 0x2A, 0x2A,
	0x18, 0x17, 0x15, 0x16, // very dark grayish pink			#655a5d
	0x19, 0x15, 0x15, 0x3F,
	0x1A, 0x15, 0x38, 0x15,	// light green -> soft light green	#54e054
	0x1B, 0x15, 0x3F, 0x3F,
	0x1C, 0x3F, 0x1C, 0x15, // soft red   						#fc7054
	0x1D, 0x3F, 0x15, 0x3F,
	0x1E, 0x3F, 0x3F, 0x15,
	0x1F, 0x3F, 0x3F, 0x3F,
};	

byte far level6palette[] = { // Troubled Water
	0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x2A,
	0x02, 0x04, 0x29, 0x06, // dark lime green			#16a418
	0x03, 0x00, 0x2A, 0x2A,
	0x04, 0x2A, 0x00, 0x00,
	0x05, 0x2A, 0x00, 0x2A,
	0x06, 0x27, 0x16, 0x02,	// dark orange / brown tone #9C5808	
	0x07, 0x2A, 0x2A, 0x2A,
	0x18, 0x15, 0x15, 0x11, // very dark grayish yellow	#606050
	0x19, 0x15, 0x15, 0x3F,
	0x1A, 0x15, 0x3F, 0x15,
	0x1B, 0x15, 0x3F, 0x3F,
	0x1C, 0x3F, 0x15, 0x15,
	0x1D, 0x3F, 0x15, 0x3F,
	0x1E, 0x3F, 0x3F, 0x15,
	0x1F, 0x3F, 0x3F, 0x3F,
};	

byte far level7palette[] = { // Lost Mine
	0x00, 0x00, 0x00, 0x00,	
	0x01, 0x00, 0x00, 0x2A,	 
	0x02, 0x00, 0x27, 0x03,	// slightly darker dark green
	0x03, 0x00, 0x2A, 0x2A,	
	0x04, 0x2A, 0x00, 0x00,	 
	0x05, 0x2A, 0x00, 0x2A,	 
	0x06, 0x2A, 0x15, 0x00,	 
	0x07, 0x25, 0x25, 0x25,	// slightly darker light grey
	0x18, 0x10, 0x10, 0x10,	// even darker grey
	0x19, 0x15, 0x15, 0x3F,	
	0x1A, 0x15, 0x3D, 0x17,	// slightly darker light green
	0x1B, 0x15, 0x3F, 0x3F,	
	0x1C, 0x3F, 0x15, 0x15,	
	0x1D, 0x3F, 0x15, 0x3F,	
	0x1E, 0x3F, 0x38, 0x18,	// yellow with an orange touch
	0x1F, 0x3F, 0x3F, 0x3F,
	
};

byte far level8palette[] = { // Donut Dome
	0x00, 0x03, 0x00, 0x02,	// very dark (mostly black) pink#0c0008
	0x01, 0x00, 0x00, 0x2A,	 
	0x02, 0x16, 0x2A, 0x00,	// slightly dimm dark green		#58a808
	0x03, 0x01, 0x26, 0x26,	// dark cyan					#049696
	0x04, 0x2A, 0x00, 0x00,	 
	0x05, 0x2A, 0x00, 0x16,	// dark pink					#a80058
	0x06, 0x2B, 0x18, 0x04, // dark orange, brown tone		#AC6016	 
	0x07, 0x29, 0x25, 0x25,	// dark grayish red				#ac9c9c
	0x18, 0x12, 0x0F, 0x12,	// very dark grayish magenta	#483c48
	0x19, 0x15, 0x15, 0x3F,	
	0x1A, 0x24, 0x3F, 0x15,	// slightly dimm light green	#90FC64
	0x1B, 0x10, 0x36, 0x36, // vivid cyan					#10D8D8	
	0x1C, 0x3F, 0x15, 0x15,
	0x1D, 0x35, 0x01, 0x35,	// strong magenta				#d704d7
	0x1E, 0x3F, 0x3F, 0x15,	
	0x1F, 0x3F, 0x3C, 0x3D,	//  light grayish pink			#FCF0F4
};

byte far level9palette[] = { // Fauwlty Towers 
	0x00, 0x00, 0x00, 0x00,
	0x01, 0x0C, 0x00, 0x2A,	// dark blue -> dark violet		#3000A8
	0x02, 0x00, 0x2A, 0x00,
	0x03, 0x00, 0x2A, 0x2A,
	0x04, 0x2A, 0x00, 0x00,
	0x05, 0x2A, 0x00, 0x2A,
	0x06, 0x2A, 0x15, 0x00,
	0x07, 0x2A, 0x2A, 0x2A,
	0x18, 0x15, 0x15, 0x15,
	0x19, 0x1C, 0x0E, 0x3F,	// light blue -> soft violet	#7038FC
	0x1A, 0x15, 0x3F, 0x15,
	0x1B, 0x15, 0x3F, 0x3F,
	0x1C, 0x3F, 0x15, 0x15,
	0x1D, 0x3F, 0x15, 0x3F,
	0x1E, 0x3F, 0x3F, 0x15,
	0x1F, 0x3F, 0x3F, 0x3F,
};

byte far level10palette[] = { // Asparagus Roots 
	0x00, 0x00, 0x00, 0x00,	
	0x01, 0x00, 0x00, 0x2A,	
	0x02, 0x00, 0x2A, 0x12,	// dark lime green				#00a816
	0x03, 0x00, 0x2A, 0x2A,	
	0x04, 0x2A, 0x00, 0x00,	
	0x05, 0x2A, 0x00, 0x2A,	
	0x06, 0x2A, 0x12, 0x00,	// dark orange brown tone		#a84700
	0x07, 0x2A, 0x2A, 0x2A,	
	0x18, 0x13, 0x11, 0x13,	// very dark grayish magenta	#4d464f
	0x19, 0x15, 0x15, 0x3F,	
	0x1A, 0x23, 0x3F, 0x15,	// light yellowish green		#8cff54
	0x1B, 0x14, 0x3F, 0x3F,	
	0x1C, 0x3F, 0x15, 0x15,
	0x1D, 0x3F, 0x15, 0x3F,	
	0x1E, 0x3F, 0x3F, 0x15,	
	0x1F, 0x3F, 0x3F, 0x3F,	
};

byte far level11palette[] = { // Habitato Piscum Scwimmeatum (Three-Tooth-Cavern)
	0x00, 0x00, 0x00, 0x00,
	0x01, 0x09, 0x09, 0x20, // dark blue 					#262682
	0x02, 0x08, 0x22, 0x08,	// dark lime green 				#208820
	0x03, 0x0C, 0x17, 0x17,	// very dark desaturated cyan	#325c5c
	0x04, 0x2A, 0x03, 0x00, // dark soft red 				#9b0d0d
	0x05, 0x23, 0x06, 0x23, // dark magenta 				#8e1a8e
	0x06, 0x27, 0x11, 0x00, // dark orange brown tone  		#8f4700
	0x07, 0x29, 0x24, 0x24, // dark grayish red 			#a49393
	0x18, 0x12, 0x0F, 0x0F, // very dark grayish red 		#483d3d   
	0x19, 0x1A, 0x1A, 0x38, // moderate blue 				#547fb9
	0x1A, 0x1F, 0x35, 0x1F, // slightly desaturated lime green #7cd47c
	0x1B, 0x20, 0x33, 0x33,	// slightly desaturated cyan 	#83cdcd
	0x1C, 0x33, 0x1B, 0x15,	// moderate red  				#cf6d56
	0x1D, 0x31, 0x15, 0x38, // soft magenta 				#c56fe1
	0x1E, 0x3F, 0x34, 0x15,	// soft yellow 					#fcd254
	0x1F, 0x3B, 0x39, 0x39, // light grayish red 			#ece4e4
};	

byte far level13palette[] = { // Shilin Forest
	0x00, 0x00, 0x00, 0x00,	
	0x01, 0x00, 0x00, 0x20,	// (bit more) dark blue	#000080
	0x02, 0x00, 0x2A, 0x00,	
	0x03, 0x00, 0x2A, 0x2A,	
	0x04, 0x2A, 0x00, 0x00,	
	0x05, 0x2A, 0x00, 0x2A,	
	0x06, 0x2A, 0x15, 0x00,	
	0x07, 0x2A, 0x2A, 0x2A,	
	0x18, 0x15, 0x15, 0x15,	
	0x19, 0x15, 0x15, 0x3F,
	0x1A, 0x15, 0x3F, 0x15,	
	0x1B, 0x12, 0x3F, 0x33,	// light lime cyan	#48FDCC
	0x1C, 0x3F, 0x15, 0x15,	
	0x1D, 0x3F, 0x15, 0x3F,	
	0x1E, 0x3F, 0x3F, 0x15,	
	0x1F, 0x3F, 0x3F, 0x3F,	
};

byte far level14palette[] = { // Concealed Ruin (Pyramid of falling stones)
	0x00, 0x00, 0x00, 0x00,	 
	0x01, 0x00, 0x00, 0x2A,	 
	0x02, 0x00, 0x2A, 0x0C,	// dark cyan lime green			#00a832
	0x03, 0x00, 0x2A, 0x2A,	 
	0x04, 0x28, 0x00, 0x02,	// dark red	    				#a0000a
	0x05, 0x2A, 0x00, 0x2A,	 
	0x06, 0x2D, 0x17, 0x00,	// strong orange				#b45f00
	0x07, 0x2A, 0x2A, 0x2A,	 
	0x18, 0x17, 0x15, 0x15,	// dark redish grey				#5f5555
	0x19, 0x15, 0x15, 0x3F,	
	0x1A, 0x03, 0x3E, 0x1A,	// lime green vivid cyan		#0ef868
	0x1B, 0x15, 0x3F, 0x3F,	 
	0x1C, 0x3A, 0x15, 0x1A,	// soft red    					#e85468
	0x1D, 0x3F, 0x15, 0x3F,	
	0x1E, 0x3F, 0x3A, 0x15,	// mustard yellow 				#fce854
	0x1F, 0x3F, 0x3F, 0x3F,	
};

byte far level15palette[] = { // Pyramid of Doors
	0x00, 0x00, 0x00, 0x00,	
	0x01, 0x0f, 0x00, 0x2A,	//  dark violette 				#3300a8
	0x02, 0x00, 0x2A, 0x00,	
	0x03, 0x00, 0x2A, 0x2A,	
	0x04, 0x2A, 0x0A, 0x00, //	dark red (brownish touch)	#a82800
	0x05, 0x2A, 0x00, 0x2A,	 
	0x06, 0x27, 0x14, 0x04,	//  dark orange (brown tone)	#9c500f
	0x07, 0x2A, 0x2A, 0x2A,	
	0x18, 0x15, 0x15, 0x15,
	0x19, 0x1E, 0x15, 0x3F,	//  soft blue					#7854fc
	0x1A, 0x15, 0x3F, 0x15,	
	0x1B, 0x15, 0x3F, 0x3F,	
	0x1C, 0x38, 0x14, 0x14,	//	soft red					#e35151
	0x1D, 0x3F, 0x15, 0x3F,	
	0x1E, 0x3F, 0x3F, 0x15,	
	0x1F, 0x3F, 0x3F, 0x3F,
};

byte far level16palette[] = { // Magic Forest pt I
	0x00, 0x09, 0x00, 0x07,	// very dark magenta			#21001b	
	0x01, 0x0D, 0x00, 0x2A,	// dark violet					#3700a8
	0x02, 0x00, 0x2A, 0x00,	
	0x03, 0x00, 0x2A, 0x2A,	
	0x04, 0x2A, 0x00, 0x00,	
	0x05, 0x2A, 0x00, 0x2A,	
	0x06, 0x24, 0x0A, 0x16,	// dark pink					#922858
//	0x07, 0x27, 0x23, 0x23,	// grey		    				#9c8c8c
	0x07, 0x25, 0x21, 0x21,	// grey		    				#9c8c8c
	0x18, 0x11, 0x0D, 0x0D,	// dark greyish red				#473434
	0x19, 0x15, 0x15, 0x3F,	
	0x1A, 0x15, 0x3F, 0x15,
//	0x1A, 0x08, 0x00, 0x06,	// very dark magenta			#21001b
	0x1B, 0x15, 0x3F, 0x3F,	
	0x1C, 0x3F, 0x15, 0x15,	
	0x1D, 0x3F, 0x15, 0x3F,	
	0x1E, 0x3F, 0x3F, 0x15,	
	0x1F, 0x39, 0x38, 0x35,	// light greyish yellow			#e5e2d7
};

byte far level18palette[] = { // Temple of Doom		//TODO, make it look slightly darker, but not to dark. Some fine tuning is need.
	0x00, 0x00, 0x00, 0x00,		    
	0x01, 0x00, 0x00, 0x25,	// slightly darker blue	
	0x02, 0x00, 0x25, 0x00,	// slightly darker green	
	0x03, 0x00, 0x25, 0x25,	// slightly darker cyan	
	0x04, 0x25, 0x00, 0x00,	// slightly darker red	    
	0x05, 0x25, 0x00, 0x25,	// slightly darker magenta	
	0x06, 0x25, 0x10, 0x00,	// slightly darker orange	
	0x07, 0x25, 0x25, 0x25,	// slightly darker grey		    
	0x18, 0x10, 0x10, 0x10,	// slightly darker dark grey	
	0x19, 0x10, 0x10, 0x3b,	// slightly darker light blue	
	0x1A, 0x10, 0x3b, 0x10,	// slightly darker light green	
	0x1B, 0x10, 0x3b, 0x3b,	// slightly darker light cyan	
	0x1C, 0x3b, 0x10, 0x10,	// slightly darker light red    
	0x1D, 0x3b, 0x10, 0x3b,	// slightly darker magenta		
	0x1E, 0x3b, 0x3b, 0x10,	// slightly darker yellow 		
	0x1F, 0x3b, 0x3b, 0x3b,	// slightly darker white		
};

byte far level19palette[] = { // Slugidarium
	0x00, 0x00, 0x00, 0x00,	
	0x01, 0x00, 0x00, 0x2A,	
	0x02, 0x12, 0x2A, 0x00,	// dark green					#4ba800
	0x03, 0x00, 0x2A, 0x2A,	
	0x04, 0x2A, 0x00, 0x00,	
	0x05, 0x2A, 0x00, 0x2A,	
	0x06, 0x26, 0x18, 0x04,	// dark orange brone tone		#986112		
	0x07, 0x2A, 0x2A, 0x2A,	
	0x18, 0x15, 0x15, 0x15,	
	0x19, 0x15, 0x15, 0x3F,
	0x1A, 0x15, 0x3F, 0x15,	
	0x1B, 0x23, 0x33, 0x3F,	// very light blue				#8ecbff
	0x1C, 0x3F, 0x15, 0x15,	
	0x1D, 0x3F, 0x15, 0x3F,	
	0x1E, 0x3F, 0x3F, 0x15,	
	0x1F, 0x3F, 0x3F, 0x3F,	
};

byte far level20palette[] = { // Donut heavens (Candy Clouds)
	0x00, 0x00, 0x00, 0x00,	
	0x01, 0x00, 0x00, 0x2A,	 
	0x02, 0x00, 0x2A, 0x00,	
	0x03, 0x00, 0x2A, 0x2A,	
	0x04, 0x2A, 0x00, 0x00,	 
	0x05, 0x2A, 0x00, 0x2A,	 
	0x06, 0x2A, 0x15, 0x00,	 
	0x07, 0x2A, 0x2A, 0x2A,	
	0x18, 0x15, 0x15, 0x15,	
	0x19, 0x15, 0x15, 0x3F,	
	0x1A, 0x15, 0x3F, 0x15,
	0x1B, 0x15, 0x36, 0x3F,	// soft cyan					#54dafc	
	0x1C, 0x3F, 0x15, 0x15,	
	0x1D, 0x3F, 0x15, 0x3F,	
	0x1E, 0x3F, 0x3F, 0x15,	
	0x1F, 0x3F, 0x3F, 0x3F,	
};

byte far level22palette[] = { // Magic Forest pt II
	0x00, 0x00, 0x00, 0x00,	
	0x01, 0x00, 0x00, 0x2A,	 
	0x02, 0x00, 0x2A, 0x00,	
	0x03, 0x00, 0x2A, 0x2A,	
	0x04, 0x2A, 0x00, 0x00,	 
	0x05, 0x2A, 0x00, 0x2A,	 
	0x06, 0x2A, 0x15, 0x00,	 
	0x07, 0x2A, 0x2A, 0x2A,	
	0x18, 0x13, 0x10, 0x13,	// very dark grayish magenta	#4d464f
	0x19, 0x15, 0x15, 0x3F,	
	0x1A, 0x15, 0x3F, 0x15,	
	0x1B, 0x15, 0x3F, 0x3F,	
	0x1C, 0x3F, 0x15, 0x15,	
	0x1D, 0x3F, 0x15, 0x3F,	
	0x1E, 0x3F, 0x3F, 0x15,	
	0x1F, 0x3F, 0x3F, 0x3F,
	
};

byte far level24palette[] = { // ACME Inc. Lab
	0x00, 0x00, 0x00, 0x00,	
	0x01, 0x05, 0x0A, 0x25,	 // dark blue					#172b95
	0x02, 0x05, 0x29, 0x0E,	 // dark lime green				#16a638
	0x03, 0x00, 0x2A, 0x2A,	
	0x04, 0x2A, 0x00, 0x00,	 
	0x05, 0x2A, 0x00, 0x2A,	 
	0x06, 0x2A, 0x15, 0x00,	 
//	0x07, 0x28, 0x26, 0x23,	// dark grayish magenta 		#a0988d	
	0x07, 0x27, 0x25, 0x22,	// dark grayish magenta 		#948b7f
	0x18, 0x10, 0x10, 0x10,	// slightly darker dark grey	
	0x19, 0x15, 0x19, 0x35,	// moderate blue				#5467d5
	0x1A, 0x2A, 0x3C, 0x13,	// soft green					#A8f14f
	0x1B, 0x1D, 0x35, 0x3B,	// soft cyan					#75d7ec
	0x1C, 0x3F, 0x15, 0x15,	
	0x1D, 0x3F, 0x15, 0x3F,	
	0x1E, 0x3F, 0x3C, 0x15,	// soft yellow 					#fef254
	0x1F, 0x3b, 0x3b, 0x3b,	// slightly darker white
	
};

byte far level25palette[] = {
	0x00, 0x00, 0x00, 0x00,	
	0x01, 0x06, 0x06, 0x1C,	// darker blue			#1818A4
	0x02, 0x02, 0x26, 0x02,	// dark lime green		#089C08
	0x03, 0x04, 0x22, 0x22, // very dark cyan		#108888
	0x04, 0x2A, 0x00, 0x00,	
	0x05, 0x2A, 0x00, 0x2A,	
	0x06, 0x24, 0x14, 0x00,	// brown tone			#905000
	0x07, 0x28, 0x2A, 0x2A,	// dark grayish cyan	#A0A8A8
	0x18, 0x15, 0x15, 0x15,	
	0x19, 0x14, 0x14, 0x30,	// moderate blue		#5050C0
	0x1A, 0x14, 0x34, 0x14,	// soft lime green		#50DC50
	0x1B, 0x14, 0x29, 0x29,	// moderate cyan		#54C0C0
	0x1C, 0x3F, 0x15, 0x15,	
	0x1D, 0x3F, 0x15, 0x3F,	
	0x1E, 0x3F, 0x3F, 0x15,	
	0x1F, 0x37, 0x3F, 0x3F,	// light grayish cyan	#DCFCFC
};

/*
byte far testpalette[] = {
	0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x20,
	0x02, 0x00, 0x20, 0x00,
	0x03, 0x00, 0x20, 0x20,
	0x04, 0x2F, 0x00, 0x00,
	0x05, 0x2F, 0x00, 0x20,
	0x06, 0x2F, 0x10, 0x00,
	0x07, 0x2F, 0x20, 0x20,
	0x18, 0x1F, 0x10, 0x10,
	0x19, 0x1F, 0x10, 0x30,
	0x1a, 0x1F, 0x30, 0x10,
	0x1b, 0x1F, 0x30, 0x30,
	0x1c, 0x3F, 0x10, 0x10,
	0x1d, 0x3F, 0x10, 0x30,
	0x1e, 0x3F, 0x30, 0x10,
	0x1f, 0x3F, 0x30, 0x30,
};
*/

void CK_SetPaletteVGA(void far *palette)
{
	if (videocard != VGAcard)
		return;
	
	asm {
		push	ds;
		lds	si, palette;
		mov	dx, 3C8h;
		mov	cx, 16;
	}
outloop:
	asm {
		lodsb;
		out	dx, al;
		inc	dx;
		lodsb;
		out	dx, al;
		lodsb;
		out	dx, al;
		lodsb;
		out	dx, al;
		dec	dx;
		loop	outloop;
		pop	ds;
	}
}

byte far *GetLevelPalette(void)
{
	switch (ck_gameState.currentLevel)
	{
	case 0:
		return (worldmappalette);
	case 4:
		return (level4palette);
	case 5:
		return (level5palette);
	case 6:
		return (level6palette);
	case 7:
		return (level7palette);
	case 8:
		return (level8palette);
	case 9:
		return (level9palette);
	case 10:
		return (level10palette);
	case 11:
		return (level11palette);
	case 13:
		return (level13palette);
	case 14:
		return (level14palette);
	case 15:
		return (level15palette);
	case 16:
		return (level16palette);
	case 18:
		return (level18palette);
	case 19:
		return (level19palette);
	case 20:
		return (level20palette);
	case 22:
		return (level22palette);
	case 24:
		return (level24palette);
	case 25:
		return (level25palette);
	default:
		return (defaultpalette);
	}
}

#define FADESTEPS 32

// Note: VW_WaitVBL() can miss the VBL signal several times in a row when music
// is playing and the interrupts for the music are being processed when the VBL
// signal is active, which can cause VW_WaitVBL() to wait a lot longer than the
// given amount of VBLs. Therefore the fading routines in here will wait for
// timer ticks instead of waiting for a VBL.

void FadeOut(void)
{
	// This is based on the assumption that the EGA palette code will only ever
	// use the colors 0 to 31. Therefore fading out the first 32 VGA colors
	// should be enough.
	byte buffer[3*32];
	byte palette[sizeof(buffer)];
	unsigned step, c;
	unsigned oldtime;
	
	//oldtime = TimeCount;	// not really necessary
	// Note: Initializing oldtime would be good practice, but we don't really
	// need to do it here. The worst thing that will happen is that the code
	// may or may not skip waiting for 1 tic during the first step of the fade.
	
	// read in the first 32 colors of the current VGA palette:
	outportb(0x3C7, 0);
	for (c=0; c<sizeof(buffer); c++)
	{
		palette[c] = inportb(0x3C9);
	}
	
	for (step=FADESTEPS; step-- != 0;)
	{
		// calculate faded color values:
		for (c=0; c<sizeof(buffer); c++)
		{
			buffer[c] = (palette[c]*step)/FADESTEPS;
		}
		
		// wait until at least 1 tic has elapsed since last palette update:
		while (oldtime == (unsigned)TimeCount)
			;
		oldtime = TimeCount;
		
		// apply the faded palette data:
		outportb(0x3C8, 0);
		for (c=0; c<sizeof(buffer); c++)
		{
			outportb(0x3C9, buffer[c]);
		}
	}
	screenfaded = true;
}

void FadeIn(byte far *palette)
{
	byte buffer[4*16];
	unsigned step, c, n;
	unsigned oldtime;
	
	//oldtime = TimeCount;	// not really necessary
	// Note: Initializing oldtime would be good practice, but we don't really
	// need to do it here. The worst thing that will happen is that the code
	// may or may not skip waiting for 1 tic during the first step of the fade.
	
	for (step=0; step<=FADESTEPS; step++)
	{
		// calculate faded color values:
		for (c=0; c<sizeof(buffer);)
		{
			buffer[c] = palette[c];
			c++;
			for (n=0; n<3; n++)
			{
				buffer[c] = (palette[c]*step)/FADESTEPS;
				c++;
			}
		}
		
		// wait until at least 1 tic has elapsed since last palette update:
		while (oldtime == (unsigned)TimeCount)
			;
		oldtime = TimeCount;
		
		// apply the faded palette data:
		CK_SetPaletteVGA(buffer);
	}
	screenfaded = false;
}

#undef FADESTEPS

// Note: CK_FadeIn() will ALWAYS fade in to the VGA palette for the current
// level. That is okay for FITF, since VGA fading is only used while playing
// the levels and not while showing the menu or the help texts.

void CK_FadeIn(void)
{
	if (videocard != VGAcard)
	{
		VW_FadeIn();
	}
	else
	{
		VW_SetDefaultColors();
		FadeIn(GetLevelPalette());
	}
}

void CK_FadeOut(void)
{
	if (videocard != VGAcard)
	{
		VW_FadeOut();
	}
	else
	{
		FadeOut();
	}
}

void CK_DefaultPalette(void)
{
	VW_SetDefaultColors();
	CK_SetPaletteVGA(defaultpalette);
}

void CK_LevelPalette(void)
{
	VW_SetDefaultColors();
	CK_SetPaletteVGA(GetLevelPalette());
}

//===========================================================================


//===========================================================================
//
// LEVEL CACHING 
//
//===========================================================================

void CK_SetLoadingTransition(void)
{
	CK_FadeOut();
	fadecount = 0;
	RF_SetRefreshHook (&FadeAndUnhook);
}
/*
==========================
=
= FadeAndUnhook
=
= Latch this onto the refresh so the screen only gets faded in after two
= refreshes.  This lets all actors draw themselves to both pages before
= fading the screen in.
=
==========================
*/

// OMNISPEAK: UpdateFadeDrawing
void FadeAndUnhook (void)
{
	if (++fadecount==2)
	{
		CK_FadeIn();
		RF_SetRefreshHook (NULL);
		// K1n9_Duk3 fix:
		//lasttimecount = TimeCount;	// don't adaptively time the fade
		TimeCount = lasttimecount;	// this is how it's done in Keen4-6
		CK_BonusLevelDialog();
	}
}

// Cache Box Routines
// These are accessed as callbacks by the caching manager

static int ck_cacheCountdownNum, ck_cacheBoxChunksPerPic, ck_cacheBoxChunkCounter;
static int ck_cacheBasePic = CD_KEENPIC;

//extern const char *ck_levelNames[];
//extern char *ck_levelEntryTexts[]; 

// The string may be temporarily modified by US_CPrint, hence it's non-const.
void CK_BeginCacheBox (char *title, unsigned numChunks)
{
	int i;
	unsigned w, h;

	// Vanilla keen checks if > 2kb free here
	// If not, it doesn't cache all of the keen counting down graphics
	// But not necessary for omnispeak
	if (MM_TotalFree() > 2048)
	{
		ck_cacheCountdownNum = 0;
	}
	else
	{
		ck_cacheCountdownNum = 5;
	}

	ck_cacheCountdownNum = 0;

	// Cache the Keen countdown graphics
	for (i = 0; i < 6; i++)
	{
		register int n = CD_KEENDONUTPIC + i;

		if (i==0)
			n = ck_cacheBasePic;

		CA_CacheGrChunk(n);
		grneeded[n] &= ~ca_levelbit;

		// If a pic can't be cached, forget updating the hand pics
		// by setting the countdown counter at 5
		if (!grsegs[n])
		{
			mmerror = false;
			ck_cacheCountdownNum = 5;
			break;
		}

		MM_SetPurge(grsegs + n, 3);
	}

	US_CenterWindow(26, 8);
	WindowW -= 48;

	if (grsegs[ck_cacheBasePic])
		VWB_DrawPic(WindowX+WindowW, WindowY, ck_cacheBasePic);
	else
		ck_cacheCountdownNum = 5;

	//grneeded[CD_KEENPIC] &= !ca_levelbit;	//redundant
	//WindowX += 48;
	// Omnispeak FIXME: Start printX at the right spot
	// The following line is not present in Keen 5
	//PrintX = WindowX;	//not required here (US_CPrint doesn't use PrintX)
	CK_MeasureMultiline(title, &w, &h);
	PrintY += (WindowH - h) / 2 - 4;
	US_CPrint(title);
	VW_UpdateScreen(); //VL_Present();

	ck_cacheBoxChunkCounter = ck_cacheBoxChunksPerPic = numChunks / 6;

	// K1n9_Duk3 mod: don't need this check anymore, we now draw the final pic at the end
	/*
	if (!ck_cacheBoxChunksPerPic && !ck_cacheCountdownNum)
	{
		ck_cacheCountdownNum = 5;
		if (grsegs[CD_ONEPIC])
		{

			VWB_DrawPic(WindowX+WindowW, WindowY + 44, CD_ONEPIC);
		}

		VW_UpdateScreen();
	}
	*/
}

void CK_UpdateCacheBox()
{
	ck_cacheBoxChunkCounter--;

	if ( ck_cacheBoxChunkCounter == 0 && ck_cacheCountdownNum <= 4 )
	{

		ck_cacheBoxChunkCounter = ck_cacheBoxChunksPerPic;
		if ( grsegs[CD_FIVEPIC + ck_cacheCountdownNum] )
		{
			VWB_DrawPic( WindowX+WindowW, WindowY + 44, CD_FIVEPIC + ck_cacheCountdownNum);
			VW_UpdateScreen();
		}
		ck_cacheCountdownNum++;
	}
}

void CK_FinishCacheBox()
{
	// K1n9_Duk3 addition: try to make sure the final countdown pic is drawn:
	if (!mmerror)
	{
		if (!grsegs[CD_ONEPIC])
		{
			CA_CacheGrChunk(CD_ONEPIC);
		}
		if (grsegs[CD_ONEPIC])
		{
			CA_UncacheGrChunk(CD_ONEPIC);
			VWB_DrawPic(WindowX+WindowW, WindowY + 44, CD_ONEPIC);
			VW_UpdateScreen();
		}
		mmerror = 0;	// just in case...
	}
}

//Nisaba patch:
static void near pascal DrawSelectionBox(int y, int color)
{
	VWB_Hlin( WindowX + 4, WindowX + WindowW - 4, y, color );
	VWB_Hlin( WindowX + 4, WindowX + WindowW - 4, y + 1, color );
	VWB_Hlin( WindowX + 4, WindowX + WindowW - 4, y + 12, color );
	VWB_Hlin( WindowX + 4, WindowX + WindowW - 4, y + 13, color );
	VWB_Vlin( y + 1, y + 11, WindowX + 4, color );
	VWB_Vlin( y + 1, y + 11, WindowX + 5, color );
	VWB_Vlin( y + 1, y + 11, WindowX + WindowW - 4, color );
	VWB_Vlin( y + 1, y + 11, WindowX + WindowW - 5, color );
}

void CK_TryAgainMenu()
{
	unsigned w, h; 
	int y1, y2, sel;
	int y, clr;

	char buf[80];

	// HACK: US_CPrint modifies strings. So don't use
	// literals directly. (char * is NOT ok; Use char arrays.)
	//static char youDidntMakeItPastString[] = "You didn't make it past";
	//static char tryAgainString[] = "Try Again";
	//static char restartString[] = "Restart from Checkpoint";
	//static char exitToArmageddonString[] = "Exit to Ancient Forest";

	/* Copy and measure the level name */
	//strcpy( buf, ck_levelNames[ck_gameState.currentLevel] );
	SM_CopyString(buf, S_LEVELNAME0+ck_gameState.currentLevel);
	CK_MeasureMultiline( buf, &w, &h );

	/* Take away all gems */
	// K1n9_Duk3 mod: CK_LoadLevel takes care of this now.
	//memset( ck_gameState.keyGems, 0, sizeof (ck_gameState.keyGems) );

	/* If lives remain, see if they want to try this level again */
	if ( --ck_gameState.numLives >= 0 )
	{
	//	VW_FixRefreshBuffer();
		US_CenterWindow( 22, 8 );
		PrintY+= 3;
	//	US_CPrint(youDidntMakeItPastString);
		SM_CPrint(S_YOUDIDNTMAKEITPAST);
		y1 = PrintY + 22;

		/* Center the level name vertically */
		if ( h < 15 )
			PrintY += 4;	
		US_CPrint(buf);

		PrintY = y1 + 2;
		if (ck_mapState.checkpointSaved)
		//	US_CPrint(restartString);
			SM_CPrint(S_RESTARTFROMCHECKPOINT);
		else
		//	US_CPrint(tryAgainString);
			SM_CPrint(S_TRYAGAIN);
		PrintY += 4;
		y2 = PrintY - 2;
	//	US_CPrint(exitToArmageddonString);
		SM_CPrint(S_EXITTOWORLDMAP);

		IN_ClearKeysDown();
		sel = 0;
		while ( 1 )
		{
			VW_WaitVBL(1);

			/* Decide which selection to draw */
			if ( sel != 0 )
				y = y2;
			else
				y = y1;

			/* Choose a color to draw it in */
			if ( (TimeCount >> 4) & 1 )
				clr = 12;
			else
				clr = 1;

			/* 
			//And draw the selection box
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 4, y, clr );
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 4, y + 1, clr );
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 4, y + 12, clr );
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 4, y + 13, clr );
			VWB_Vlin( y + 1, y + 11, WindowX + 4, clr );
			VWB_Vlin( y + 1, y + 11, WindowX + 5, clr );
			VWB_Vlin( y + 1, y + 11, WindowX + WindowW - 4, clr );
			VWB_Vlin( y + 1, y + 11, WindowX + WindowW - 5, clr );
			// VL_Present();
			VW_UpdateScreen();
			
			// Erase the box for next time
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 4, y, 15 );
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 4, y + 1, 15 );
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 4, y + 12, 15 );
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 4, y + 13, 15 );
			VWB_Vlin( y + 1, y + 11, WindowX + 4, 15 );
			VWB_Vlin( y + 1, y + 11, WindowX + 5, 15 );
			VWB_Vlin( y + 1, y + 11, WindowX + WindowW - 4, 15 );
			VWB_Vlin( y + 1, y + 11, WindowX + WindowW - 5, 15 );
			*/
			
			//Nisaba patch:
			// And draw the selection box
			DrawSelectionBox(y, clr);
			// VL_Present();
			VW_UpdateScreen();

			// Erase the box for next time
			DrawSelectionBox(y, WHITE);
			

			/* If they press Esc, they want to go back to the Map */
			if ( LastScan == sc_Escape )
			{
				ck_gameState.currentLevel = 0;
				IN_ClearKeysDown();
				return;
			}

			IN_ReadControl( 0, &ck_inputFrame );
			if ( ck_inputFrame.button0 || ck_inputFrame.button1 || LastScan == sc_Enter || LastScan == sc_Space )
			{
				/* If they want to go back to the Map, set the current level to zero */
				if ( sel != 0 )
				{
					ck_gameState.currentLevel = 0;
				}
				/*
				else if (ck_mapState.checkpointSaved)
				{
					if (CheckpointLoad())
					{
						ck_levelState = LS_Playing;
					}
					else
					{
						//Quit("can't load checkpoint");
						SM_Quit(S_CANTLOADCKECKPOINT);
						ck_levelState = LS_Aborted;	// an error occured
					}
				}
				*/
				// Nisaba patch: This will let a faulty checkpoint file restart 
				// instead of crashing the program with an error message.
				else if (ck_mapState.checkpointSaved)
				{
					CK_GameState oldstate = ck_gameState;	// create backup in case loading the checkpoint fails
					
					if (CheckpointLoad())
					{
						// checkpoint load succeeded
						ck_levelState = LS_Playing;
					}
					else
					{
						// checkpoint load failed
						ck_gameState = oldstate;	// restore gamestate and restart current level
					}
				}
				IN_ClearKeysDown();
				return;
			}

			if ( ck_inputFrame.yaxis == -1 || LastScan == sc_UpArrow )
			{
				sel = 0;
			}
			else if ( ck_inputFrame.yaxis == 1 || LastScan == sc_DownArrow )
			{
				sel = 1;
			}
		} /* while */
	}
}

boolean CK_ContinueMenu(void)
{

#define BOTTOMHEIGHT 36	// pixel height of the Quit/Continue part of the menu

	//VW_FadeOut();
	CK_FadeOut();
	RF_FixOfs();
	VWB_Bar(0, 0, 320, 200, BLACK);

	// select the current message and draw a window that is big enough
	// for the message and the actual menu part:
	{
		unsigned w, h, th;
		stringnames msg;

		msg = ck_gameState.continuesUsed + S_RANOUTOFLIVES;
		if (msg > S_RANOUTOFJOKES)
			msg = S_RANOUTOFJOKES;
		else
			ck_gameState.continuesUsed++;	// only increase counter when we have NOT reached the final joke string

		SM_MeasureMultiline(msg, &w, &h);
		w = (w + 7)/8;			// convert from pixels to tiles (round up)
		th = (h + BOTTOMHEIGHT + 7)/8;	// convert from pixels to tiles (round up)
		// Note: we still need the pixel height of the text, so that value stays in h

		// draw a window that is big enough
		// minimal window size is same as the size of the "cache box" window
		US_CenterWindow( max(26, w), max(8, th) );

		// draw the message string vertically centered
		// in the top part of the window
		PrintY += (WindowH - h - BOTTOMHEIGHT)/2;
		SM_CPrint(msg);
	}

	// draw the menu part and handle user input:
	{
		int y1, y2, sel;
		int y, clr, timeout;

		y1 = WindowY + WindowH - BOTTOMHEIGHT;
		PrintY = y1 + 2;
		SM_CPrint(S_CONTINUE);

		PrintY += 4;
		y2 = PrintY - 2;
		SM_CPrint(S_QUIT);

		IN_ClearKeysDown();
		sel = 1;
		timeout = (WindowW-8)*4;
		while (LastScan != sc_Escape)
		{
			VW_WaitVBL(1);

			/* Decide which selection to draw */
			if ( sel != 0 )
				y = y2;
			else
				y = y1;

			/* Choose a color to draw it in */
			if ( (TimeCount >> 4) & 1 )
				clr = 12;
			else
				clr = 1;

			/* 
			//And draw the selection box
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 5, y, clr );
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 5, y + 1, clr );
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 5, y + 12, clr );
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 5, y + 13, clr );
			VWB_Vlin( y + 1, y + 11, WindowX + 4, clr );
			VWB_Vlin( y + 1, y + 11, WindowX + 5, clr );
			VWB_Vlin( y + 1, y + 11, WindowX + WindowW - 6, clr );
			VWB_Vlin( y + 1, y + 11, WindowX + WindowW - 5, clr );
			if (timeout)
				VWB_Bar(WindowX+4, WindowY+WindowH-6, timeout/4, 3, 2);
			VW_UpdateScreen();
			if (screenfaded)
				VW_FadeIn();
			*/ 
			
			// Nisaba patch:
			/* And draw the selection box */
			DrawSelectionBox(y, clr);
			if (timeout)
				VWB_Bar(WindowX+4, WindowY+WindowH-6, timeout/4, 3, 2);
			VW_UpdateScreen();
			if (screenfaded)
				//VW_FadeIn();
				CK_FadeIn();

			/* Erase the box for next time */
			DrawSelectionBox(y, WHITE);
			VWB_Bar(WindowX+4, WindowY+WindowH-6, WindowW-8, 3, WHITE);	// erase timeout bar
			
			/*
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 5, y, 15 );
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 5, y + 1, 15 );
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 5, y + 12, 15 );
			VWB_Hlin( WindowX + 4, WindowX + WindowW - 5, y + 13, 15 );
			VWB_Vlin( y + 1, y + 11, WindowX + 4, 15 );
			VWB_Vlin( y + 1, y + 11, WindowX + 5, 15 );
			VWB_Vlin( y + 1, y + 11, WindowX + WindowW - 6, 15 );
			VWB_Vlin( y + 1, y + 11, WindowX + WindowW - 5, 15 );
			VWB_Bar(WindowX+4, WindowY+WindowH-6, WindowW-8, 3, 15);
			*/

			IN_ReadControl( 0, &ck_inputFrame );
			if ( ck_inputFrame.button0 || ck_inputFrame.button1 || LastScan == sc_Enter || LastScan == sc_Space )
			{
				IN_ClearKeysDown();
				return !sel;
			}

			if ( ck_inputFrame.yaxis == -1 || LastScan == sc_UpArrow )
			{
				sel = 0;
				timeout = LastScan = 0;
			}
			else if ( ck_inputFrame.yaxis == 1 || LastScan == sc_DownArrow )
			{
				sel = 1;
				timeout = LastScan = 0;
			}
			if (timeout > 0)
			{
				if (--timeout == 0)
					break;
			}
		} /* while */
	}

	IN_ClearKeysDown();
	return false;

#undef BOTTOMHEIGHT
}


void CK_MapLevelMarkAsDone(void)
{
	int y, x, level, i;
	unsigned flags, w;
	unsigned far *pw;

	i = 0;
	pw = mapsegs[2];

	/* Look through the map for level-related tiles */
	for ( y = 0; y < mapheight; y++ )
	{
		for ( x = 0; x < mapwidth; x++, pw++, i++ )
		{
			w = *pw;
			level = w & 0xFF;
			if (w == (0xC000|MAP_TEMPLELEVEL) && ck_gameState.levelsDone[MAP_TEMPLELEVEL])
			{
				*pw = 0xC000|MAP_ACMELABLEVEL;	// replace temple level entrance with lab level entrance
				continue;	// don't remove this lab level entrance, even if lab level is marked as done
			}
			if ( level >= 1 && level <= 24 && ck_gameState.levelsDone[level] && level != MAP_BWBMEGAROCKET)
			{	/* Is this a level tile */
				flags = w >> 8;
				/* Set the info tile at this position to 0 */
				*pw = 0;

				// Nisaba patch:
				if ( flags == 0xD0 )
				{
					/* If this is a 'blocking' part of the level */
					/* Set the foreground tile at this position to 0 also (remove the fences) */
					mapsegs[1][i] = 0;
				//	*(mapsegs[1] + y * mapwidth + x) = 0;
				}
				
				// Nisaba patch:
				else if ( flags == 0xD1 )
				{
					/* Set foreground tile to 35 (invisible blocking tile) */
					mapsegs[1][i] = 35;
				}
				else if ( flags == 0xF0 )
				{	
					/* If this is the flag holder for the level */
					int flagoff = 0;
					if (ck_gameState.levelsDone[level] & DF_PERFECT)
						flagoff = FLAG2FLIP1SPR-FLAGFLIP1SPR;

					if ( ck_lastLevelFinished == level )
						CK_SpawnRisingFlag(x,y,flagoff);
					else
						CK_MapFlagSpawn(x,y,flagoff);
				}
			}
		}
	}

}

// K1n9_Duk3 fix:

static void near PurgeSpritesAndTiles(void)
{
	register int i;

	// purge all sprites except the paddle war sprites
	// (in case the menu still needs them)
	for (i=DEMOSIGNSPR; i<STARTSPRITES+NUMSPRITES; i++)
	{
		if (grsegs[i])
		{
			MM_SetPurge(&grsegs[i], 1);
		}
	}
	// purge all tiles (unmasked and masked)
	for (i=STARTTILE16; i<STARTTILE16M+NUMTILE16M; i++)
	{
		if (grsegs[i])
		{
			MM_SetPurge(&grsegs[i], 1);
		}
	}
}

// end of fix

/*
==========================
=
= SetupGameLevel
=
= Load in map mapon and cache everything needed for it
=
==========================
*/

#define MAP_EXISTS(num) (((long far *)(tinf+2))[num])	// ugly hack, but it works...

void CK_LoadLevel(boolean doCache)
{
	unsigned mapnum;	// K1n9_Duk3 addition
	char *loadmsg = NULL;	// K1n9_Duk3 mod

	if (DemoMode != demo_Off)
	{
		// If we're recording or playing back a demo, the game needs
		// to be deterministic. Seed the RNG at 0 and set difficulty
		// to normal.
		US_InitRndT(false);
		ck_gameState.difficulty = D_Normal;
	}
	else
	{
		US_InitRndT(true);
	}

	memset(&ck_mapState, 0, sizeof(ck_mapState));	// K1n9_Duk3 addition

	// K1n9_Duk3 fix: make sure there is some memory available so CA_CacheMap won't fail
	PurgeSpritesAndTiles();	// can't just call CA_SetGrPurge here - menu might still need some graphics chunks
	StopMusic();

	// K1n9_Duk3 mod: let's draw the loading screen before caching the map
	if (doCache)
	{
		//static char demoString[] = "DEMO";
		

		// HACK: US_CPrint modifies strings. So don't use
		// literals directly. (char * is NOT ok; Use char arrays.)
		if (ck_inHighScores)
		{
			loadmsg = NULL;
		}
		else if (DemoMode != demo_Off)
		{
			//loadmsg = demoString;
			loadmsg = SM_GetString(S_DEMO);
		}
		else if (ck_levelState == LS_MushroomTrigger)
		{
			// draw a fake loading screen:
			US_CenterWindow(26, 8);
			WindowW -= 48;
			CA_CacheGrChunk(KEENSMOKECLOUDPIC);
			VWB_DrawPic(WindowX+WindowW, WindowY, KEENSMOKECLOUDPIC);
			PrintY += 14;
			SM_CPrint(S_LEVELTEXT22);
			VW_UpdateScreen();
			
			// don't show the real loading screen:
			loadmsg = NULL;
		}		
		else if (ck_levelState == LS_BonusComplete)
		{
			loadmsg = SM_GetString(S_LEVELTEXTBONUSDONE);
			ck_cacheBasePic = CD_KEENDONUTPIC;	// different pic for loading screen
		}
		else if (ck_gameState.currentLevel == 0 && ck_gameState.mapPosX == 0)
		{
			// no loading screen for loading the world map for the first time
			loadmsg = NULL;
		}
		else if (ck_gameState.currentLevel == MAP_BWBMEGAROCKET && !ck_gameState.levelsDone[MAP_BWBMEGAROCKET])
		{
			// special message for entering the BWB for the first time (i.e. landing the BWB)
			loadmsg = SM_GetString(S_LEVELTEXTLAND);
		}
		else
		{
			//loadmsg = ck_levelEntryTexts[ck_gameState.currentLevel];
			loadmsg = SM_GetString(S_LEVELTEXT0+ck_gameState.currentLevel);
		}

		// HACK: draw loading window
		if (loadmsg)
			CK_BeginCacheBox(loadmsg, 0x7FFF);
	}

	// K1n9_Duk3 addition: try to load difficulty-specific map
#if 0
	// old version:
	// maps  0 to 29 are for easy difficulty
	// maps 30 to 59 are for normal difficulty
	// maps 60 to 89 are for hard difficulty
	mapnum = ck_gameState.currentLevel + (ck_gameState.difficulty-gd_Easy)*30;
	
	// undo skill adjustment for demos, or when map number is out of range or
	// when skill-adjusted map does not exist:
	if (DemoMode || (unsigned)ck_gameState.currentLevel >= 30 || mapnum >= 100 || !MAP_EXISTS(mapnum))
	{
		mapnum = ck_gameState.currentLevel;
	}
#else
	// new version:
	// maps  0 to 29 are for normal difficulty
	// maps 30 to 59 are for easy difficulty
	// maps 60 to 89 are for hard difficulty
	mapnum = ck_gameState.currentLevel;
	
	// don't apply skill adjustment for demos or when map number is out of range:
	if (!DemoMode && mapnum < 30)
	{
		switch (ck_gameState.difficulty)
		{
		case gd_Easy:
			mapnum += 30;
			break;
		case gd_Hard:
			mapnum += 60;
			break;
		}
		// undo skill adjustment when skill-adjusted map does not exist:
		if (mapnum >= 100 || !MAP_EXISTS(mapnum))
		{
			mapnum = ck_gameState.currentLevel;
		}
	}
#endif
	// end of addition
		
	
	CA_CacheMap(mapnum);	// K1n9_Duk3 mod: use mapnum variable instead of ck_gameState.currentLevel
	RF_NewMap();
	CA_ClearMarks();

	CK_SetupObjArray();
	
	CK_ScanInfoLayer();

	if (ck_gameState.currentLevel == 0)
	{
		CK_MapLevelMarkAsDone(); 
	}

	RF_MarkTileGraphics();
	MM_BombOnError(false);
	CA_LoadAllSounds();

	// Cache Marked graphics and draw loading box
	if (doCache)
	{
		CA_CacheMarks(loadmsg);	// K1n9_Duk3 mod
		SD_WaitSoundDone();	// K1n9_Duk3 mod: don't fade out before the sound is done
	}

	ck_cacheBasePic = CD_KEENPIC;	// K1n9_Duk3 mod: always go back to default loading pic here

	// Fade the screen
	MM_BombOnError(true);

	if (mmerror || !doCache)
		return;

	CK_SetLoadingTransition();

	//CK_LevelPalette();

}

//===========================================================================
//
// MENUS
//
// Begin/Update/Finish CacheBox
//===========================================================================

//===========================================================================
//
// DEMO PLAYBACK
// 
//===========================================================================

#ifdef DEVBUILD
void CK_StartDemoRecord(void)
{
	VW_FixRefreshBuffer();
	US_CenterWindow(30, 3);
	PrintY += 6;
	//US_Print("Record a demo from level(0-3)");
	SM_Print(S_RECORDDEMO);
	VW_UpdateScreen();

	if (US_LineInput(px, py, str, 0, true, 2, 0))
	{
		int level = atoi(str);

	//	if (level > 0 && level < NUMMAPS)	// development phase
		if (level > 0 && level <= 17)
		{
			ck_gameState.currentLevel = level;
			ck_levelState = LS_AboutToRecordDemo;
			IN_StartDemoRecord(0x1000);
		}
	};

}

void CK_EndDemoRecord (void)
{

	//char path[] = "DEMO?."EXTENSION;
	char path[10];
	SM_CopyString(path, S_DEMOFILE);
	IN_StopDemo();
	VW_FixRefreshBuffer();
	US_CenterWindow(22, 3);
	PrintY += 6;
	//US_Print (" Save as demo #(0-9):");
	SM_Print(S_SAVEDEMO);
	VW_UpdateScreen();

	if (US_LineInput(px, py, str, 0, true, 2, 0))
	{
		if (str[0] >= '0' && str[0] <= '9')
		{
			int handle;
			path[4] = str[0];
			if ((handle = open(path, O_BINARY | O_CREAT | O_WRONLY, S_IFREG | S_IREAD | S_IWRITE)) == -1)
			{
				//Quit("EndDemoRecord: Cannot write demo file!");
				SM_Quit(S_CANNOTWRITEDEMO);
			}

			write(handle, &mapon, sizeof(mapon));
			write(handle, &DemoOffset, sizeof(DemoOffset));
			CA_FarWrite(handle, DemoBuffer, DemoOffset);
			close(handle);
		}
	}

	IN_StopDemo();

}
#endif

/*
============================
=
= GameLoop
=
= A game has just started (after the cinematic or load game)
=
============================
*/

void CK_GameLoop()
{
	do
	{
		if (ck_levelState != LS_StartingSaved)	// K1n9_Duk3 fix: use enum name instead of number
		{
resetDifficultyAndLevel:
			if (!NoWait)	// K1n9_Duk3 mod (for quick playtesting)
				CK_DrawStarWars();	// TODO: uncomment this, once game develompent phase is over
			ck_gameState.difficulty = restartgame;
			restartgame = D_NotPlaying;
loadLevel:
			ck_gameState.keenScore = ck_gameState.oldScore;	// K1n9_Duk3 addition: reset score!
			CK_LoadLevel(true);
			
			// Nisaba patch:
			//if (!NoWait)	// K1n9_Duk3 mod (for quick playtesting and speedrunning)
			//	CK_DrawStarWars();
							
			if (mmerror)
			{
				if (ck_gameState.currentLevel != 0)
				{
					mmerror = false;
					US_CenterWindow(20, 8);
					PrintY += 20;
				//	US_CPrint("Insufficient memory\nto load level!");
					SM_CPrint(S_LOADLEVEL_NOMEM);
					VW_UpdateScreen();
					IN_Ack();
					ck_gameState.currentLevel = 0;
					CK_LoadLevel(true);
				}
				if (mmerror)
				{
					//Quit("GameLoop: Insufficient memory to load world map!");
					SM_Quit(S_LOADWORDLMAP_NOMEM);
				}
			}

			// HACK: always save a checkpoint at the beginning of a level
			if (ck_gameState.currentLevel != 0)
				SaveCheckpoint();
				
		}
		else
		{
			CK_LevelPalette();
		}

replayLevel:
		ck_scrollDisabled = false;
		SD_WaitSoundDone();
		CK_PlayLoop();

		// K1n9_Duk3 mod:
		// We don't need to reset level variables (like keyGems) here anymore.
		// CK_LoadLevel automatically takes care of this now.

		VW_FixRefreshBuffer();
		ck_lastLevelFinished = -1;
		switch (ck_levelState)
		{
		case LS_Died:
			CK_TryAgainMenu();
			if (ck_levelState == LS_Playing)	// loaded a checkpoint?
				goto replayLevel;
			else if (ck_levelState == LS_Aborted)	// failed to load checkpoint?
				return;
		//	ck_gameState.currentLevel = ck_nextMapNumber;
			break;
		
		case LS_MushroomTrigger:			// Nisaba patch: interconnecting maps:
		//	ck_lastLevelFinished = ck_gameState.currentLevel;
			ck_gameState.currentLevel = MAP_MUSHROOMLEVEL2;
		//	VW_FadeOut();
			CK_FadeOut();
			VW_ClearVideo(BLACK);
		//	VW_SetDefaultColors();
			CK_DefaultPalette();
			// Note: You could skip the sound, but it would make the loading window
			// disappear as soon as loading the map data and graphics is done,
			// which means it would be impossible to read the message on fast
			// systems.
		//	SD_PlaySound(YETICLOUDSND);
			SD_PlayTwoSounds(FOOTAPPEARSND,YETICLOUDSND);
			HandleLevelDone();
			break;
		
		case LS_SecretTrigger:				// Nisaba patch: interconnecting maps:
			HandleLevelDone();
			ck_gameState.levelsDone[MAP_MUSHROOMLEVEL1] = ck_mapState.doneFlags;
			ck_gameState.levelsDone[ck_gameState.currentLevel] = ck_mapState.doneFlags;
		//	ck_lastLevelFinished = ck_gameState.currentLevel;
			ck_gameState.currentLevel = MAP_TARDISLEVEL;
			SD_PlaySound(ENTERLEVELSND);
			break;	
					
		case LS_BonusComplete:
			CK_LindseyDialog(2);
			StopMusic();
			ck_gameState.collectiblesTotal |= 0x40;	// donut icon (re-uses mario slot)
			// no break here!
		
		case LS_LevelComplete:
		case LS_Landed:
		case 7:
		case 13:
			HandleLevelDone();
			if (ck_gameState.currentLevel == 0)
			{
				// K1n9_Duk3 mod: The "One Moment" window was just here to
				// have some sort of a loading screen while the game was
				// loading the map planes. The real loading screen was only
				// shown while loading the graphics. But we now draw the
				// loading screen before loading the map planes, so this
				// window is no longer needed.
				/*
				unsigned bufsave = bufferofs;
				bufferofs = displayofs;

				US_CenterWindow(0x1A, 8);
				PrintY += 0x19;
				//US_CPrint("One Moment");
				SM_CPrint(S_ONEMOMENT);

				bufferofs = bufsave;
				*/

				ck_gameState.currentLevel = ck_mapState.nextLevel;
			}
			/*
			else
			{
				//We've beat the level, return to main map.
				//Mark level as done (and more)
				SD_PlaySound(LEVELDONESND);
				backToMap:
				ck_lastLevelFinished = ck_gameState.currentLevel;
				ck_gameState.levelsDone[ck_gameState.currentLevel] = ck_mapState.doneFlags;
				ck_gameState.currentLevel = 0;
				// BonusLevel Dialog:
				CK_BonusLevelDialog();
				
				// If Keen 4-6 was launched with /Demo switch
				// Then function returns here based on mapon
				// Not required for FITF
			}
			break;
			*/
			// Nisaba patch: interconnect two maps without having to pass 
			// through the world map to go from one level to another.
			else if (ck_gameState.currentLevel == MAP_TEMPLELEVEL)		
			{
				//We've beaten the level, head on to the final level.
				//ck_lastLevelFinished = ck_gameState.currentLevel;
				ck_gameState.levelsDone[MAP_TEMPLELEVEL] = ck_mapState.doneFlags;	// K1n9_Duk3 addition: can't enter it a second time
				ck_gameState.currentLevel = MAP_ACMELABLEVEL;
				SD_PlaySound(ENTERLEVELSND);
			}
			/* // this is not used anymore (MAP_MUSHROOMLEVEL1 ends with LS_MushroomTrigger):
			else if (ck_gameState.currentLevel == MAP_MUSHROOMLEVEL1)
			{
				//interconnecting maps:
				//ck_lastLevelFinished = ck_gameState.currentLevel;
				ck_gameState.currentLevel = MAP_MUSHROOMLEVEL2;
				SD_PlaySound(ENTERLEVELSND);
			}
			*/
			else
			{
				// K1n9_Duk3 mod: merged the MAP_TARDISLEVEL and MAP_MUSHROOMLEVEL2 branches into the regular level branch
				if (ck_gameState.currentLevel == MAP_MUSHROOMLEVEL2)
				{
					ck_gameState.levelsDone[MAP_MUSHROOMLEVEL1] = ck_mapState.doneFlags;
				}
				
				// We've beat the level, return to main map.
				// Mark level as done (and more)
				ck_lastLevelFinished = ck_gameState.currentLevel;
				ck_gameState.levelsDone[ck_gameState.currentLevel] = ck_mapState.doneFlags;
				ck_gameState.currentLevel = 0;
				SD_PlaySound(LEVELDONESND);
				
				// If Keen 4-6 was launched with /Demo switch
				// Then function returns here based on mapon
				// Not required for FITF
			}
			break;

		// Starting New game
		case LS_StartingNew:
			goto resetDifficultyAndLevel;

		// Loading Game
		case LS_StartingSaved:
			CK_LevelPalette();
			goto replayLevel;

		case LS_Aborted:
			// Quit to Dos
			IN_ClearKeysDown();
			return;
		case LS_ClonedJanitor:
			HandleLevelDone();
			if (ck_gameState.cheated)
			{
				StartMusic(28, true);
				EndText(T_END_CHEAT);
			}
			// Nisaba patch
		//	else if (ck_gameState.keenScore >= 1000)	// quick test
			else if (ck_gameState.keenScore >= 1000000)
			{
				StartMusic(36, true);
				EndText(T_END_ALL);
				CK_SubmitHighScore(ck_gameState.keenScore, ck_gameState.collectiblesTotal, (ck_gameState.slugcans >= 17), ck_gameState.difficulty);	// Nisaba patch
			}
			else if (ck_gameState.keenScore <= 3140)
			{
				StopMusic();
				SD_PlaySound(LEVELDONESND);
				EndText(T_END_MINIMUM);
				SD_PlaySound(CLICKSND);
			}
			else
			{
				StartMusic(25, true);
				EndText(T_END_WIN);
				CK_SubmitHighScore(ck_gameState.keenScore, ck_gameState.collectiblesTotal, (ck_gameState.slugcans >= 17), ck_gameState.difficulty);
			}
			return;

		case LS_BrainFucled:
			HandleLevelDone();
			StartMusic(26, false);
			EndText(T_END_LOSE);
			Quit(NULL);
			return;

#if 0
		case 14:
			// The level has been ended by fuse destruction
			SD_PlaySound(LEVELDONESND);
		//	ck_lastLevelFinished = mapon;
			ck_gameState.levelsDone[ck_gameState.currentLevel] = 14;
			CK5_FuseMessage();
			ck_gameState.currentLevel = 0;
			break;
#endif

		case 15:
			// The QED was destroyed
			/*
			 * purge_chunks()
			 * RF_Reset();
			 * VW_FixRefreshBuffer();
			 */
			// help_endgame();
#if 0
			CK_SubmitHighScore(ck_gameState.keenScore, 0);
#endif
			return;
			// Warping level
			// This code is added for omnispeak so that the warp functionality works
			// Case 4 normally switches to default
			// UPDATE (Mar 6 2014): Not needed anymore.
		case LS_WarpToLevel:
			// ck_gameState.currentLevel = ck_nextMapNumber;
			break;


		case 3:
		case 9:
		case 10:
		case 11:
		case 12:
			break;
		}

		if (ck_gameState.numLives < 0)
		{
			CK_GameOver();
			if (CK_ContinueMenu())
			{
				unsigned joke;
				unsigned winW = WindowW/8, winH = WindowH/8;	// use same window size as the continue menu by default
				unsigned strW, strH, h;

				joke = ck_gameState.jokeNumber + S_CONTINUEJOKE1;
				if (joke > S_FINALCONTINUEJOKE)
					joke = S_FINALCONTINUEJOKE;
				while (*SM_GetString(joke))
				{
					SM_MeasureMultiline(joke, &strW, &strH);
//printf("joke=%u, w=%u h=%u\n", joke, strW, strH);
					strW = (strW + 7) / 8;	// convert from pixels to tiles (round up)
					h = (strH + 7) / 8;	// convert from pixels to tiles (round up)
					if (strW > winW)
						winW = strW;
					if (h > winH)
						winH = h;
					US_CenterWindow( winW, winH );
					PrintY += (winH*8 - strH)/2;	// center the text
					SM_CPrint(joke);
					VW_UpdateScreen();
					IN_Ack();
					joke++;
				}
				ck_gameState.jokeNumber = joke + 1 - S_CONTINUEJOKE1;
				VWB_Bar(0, 0, 320, 200, BLACK);	// erase the window for the upcoming loading screen
/*
				US_CenterWindow( 26, 8 );
				PrintY += 2;
				//US_CPrint("Just kidding! You don't have to\npay any money to continue.\nWhat do you think this is?\nA mobile game!?\n\nWe'll just reset your score...");
				SM_CPrint(S_CONTINUEJOKE2);
				VW_UpdateScreen();
				IN_Ack();
*/

				ck_gameState.oldScore = 0;
				ck_gameState.nextKeenAt = 20000;
				ck_gameState.numLives = DEFAULT_LIVES;
				ck_gameState.numShots = DEFAULT_SHOTS;
				ck_gameState.numVitalin = 0;
				ck_gameState.currentLevel = 0;
				/*
				switch (ck_gameState.difficulty)
				{
				case gd_Easy:
					ck_gameState.numLives = 5;
					ck_gameState.numShots = 10;
					break;
				case gd_Normal:
					ck_gameState.numLives = 3;
					ck_gameState.numShots = 5;
					break;
				case gd_Hard:
					ck_gameState.numLives = 2;
					ck_gameState.numShots = 3;
					break;
				}
				*/
				// Nisaba patch:
				ApplyDifficulty(ck_gameState.difficulty);
			}
			else
			{
				// TODO: don't allow a highscore entry if the player cheated?
				
				CK_SubmitHighScore(ck_gameState.keenScore, ck_gameState.collectiblesTotal, (ck_gameState.slugcans >= 17), ck_gameState.difficulty);
				return;
			}
		}

		goto loadLevel; //livesLeft >= 0
	}	while (true);
	
	// Keen4/6 simple game over (unreachable in FITF)
	/*
	CK_GameOver();

	CK_SubmitHighScore(ck_gameState.keenScore, 0);
	*/
}
