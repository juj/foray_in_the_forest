/* "Foray in the Forest" Source Code
 * Copyright (C) 2022-2023 Nisaba
 * Copyright (C) 2019-2021 K1n9_Duk3
 *
 * Primarily based on:
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

// CK_MISC.C

#include "CK_DEF.H"
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

/*
const char *ck_levelNames[100] = {

	"The Ancient Forest",
	"The Outpost",
	"The Forgotten Pyramid",
	"TESTLEVEL",
	"TEST\nLEVEL",
};
*/

// HACK: Sorry, the strings need to be in WRITABLE storage,
// because US_CPrint (temporarily) modifies them.

/*
char ck_levelEntryText_0[] =
"Keen wanders the\nAncient Forest";

char ck_levelEntryText_1[] =
"Keen investigates\nthe Outpost";

char ck_levelEntryText_2[] =
"Keen discovers\the Forgotten Pyramid";

char ck_levelEntryText_3[] =
"Keen launches into the\nBWB Megarocket";

char *ck_levelEntryTexts[100] = {
	ck_levelEntryText_0,
	ck_levelEntryText_1,
	ck_levelEntryText_2,
	ck_levelEntryText_3,
};
*/

/*
	 =============================================================================

	 LOCAL VARIABLES

	 =============================================================================
 */

void CK_ObjBadState (void);

CK_action CK_ACT_Null = {0, 0, AT_Frame, false, false, 0, 0, 0, (void (*)(CK_object *))CK_ObjBadState, (void (*)(CK_object *, CK_object *))CK_ObjBadState, (void (*)(CK_object *))CK_ObjBadState, NULL};



//===========================================================================

// Contains some keen-5 specific functions.



// K1n9_Duk3 mod: turned the defines into an enum
// also moved the lumps into an include file to make sure the lumps are always
// in the correct order in the ck_lumpStarts and ck_lumpEnd arrays
typedef enum {

#define MAKELUMP(x) x##LUMP
#include "LUMPS.INC"
#undef MAKELUMP

	NUMLUMPS
} lumpnames;

int ck_lumpsNeeded[NUMLUMPS];

int ck_lumpStarts[] = {

#define MAKELUMP(x) x##_LUMP_START
#include "LUMPS.INC"
#undef MAKELUMP

};

int ck_lumpEnds[] = {

#define MAKELUMP(x) x##_LUMP_END
#include "LUMPS.INC"
#undef MAKELUMP

};

#define NeedLump(x) {ck_lumpsNeeded[x##LUMP] = true;}

// Nisaba patch
static void near pascal SetMusicNum(int infoValue)
{
	if (infoValue >= INFONUMBER_0 && infoValue <= INFONUMBER_MAX)
	{
		infoValue -= INFONUMBER_0;
		if (infoValue < LASTMUSIC)
		{
			ck_mapState.music = infoValue;
		}
	}
}

void CK_ScanInfoLayer(void)
{

	CK_object *obj;
	int x, y, infoValue, i,j;

	memset(ck_lumpsNeeded, 0, sizeof(ck_lumpsNeeded));

	infoValue = mapsegs[2][0];

	// K1n9_Duk3 addition:
	// Music number must now be placed as an InfoPlane number icon in
	// the lop left corner of the map (at position 0, 0).
	/*
	if (infoValue >= INFONUMBER_0 && infoValue <= INFONUMBER_MAX)
	{
		infoValue -= INFONUMBER_0;
		if (infoValue < LASTMUSIC)
		{
			ck_mapState.music = infoValue;
		}
	}
	*/
	SetMusicNum(mapsegs[2][0]);
	// Note: The music will be set to song number 0 by default!
	

	for (y = 0, i = 0; y < mapheight; y++)
	{
		for (x = 0; x < mapwidth; x++, i++)
		{
			infoValue = mapsegs[2][i];

			switch (infoValue)
			{
				case 1:
					CK_SpawnKeen(x, y, 1);
					SetMusicNum(mapsegs[2][i-mapwidth]);	// use info number directly above the Keen icon as music number
					NeedLump(KEEN);
					SpawnScore();
					CA_MarkGrChunk(SCOREBOXSPR);
					break;

				case 81:
					if (ck_gameState.levelsDone[MAP_BWBMEGAROCKET])
						break;
				case 2:
					CK_SpawnKeen(x, y, -1);
					SetMusicNum(mapsegs[2][i-mapwidth]);	// use info number directly above the Keen icon as music number
					NeedLump(KEEN);
					SpawnScore();
					CA_MarkGrChunk(SCOREBOXSPR);
					break;

				case 3:
					CK_SpawnMapKeen(x, y);
					NeedLump(WORLDKEEN);

					SpawnScore();
					CA_MarkGrChunk(SCOREBOXSPR);
					break;

				case 4:
					obj = CK_SpawnItem(x, y, 6);	// spawn cotton candy object
					obj->gfxChunk++;	// and change sprite number into lollypop sprite
					obj->user2++;
					obj->user3++;
					CK_ResetClipRects(obj);	// hitboxes for the lollypop and the cotton candy sprites should be mostly identical, but just in case...
					break;

				case 5:
					CK_SpawnTentacle(x, y);
					NeedLump(TENTACLE);
					break;

				case 6:
					CK_SpawnLindsey(x, y, 0);
					NeedLump(LINDSEY);
					NeedLump(SMIRKYPOOF);
					break;

				case 7:
					CK_SpawnWormouth(x, y);
					NeedLump(WORMOUTH);
					break;

				case 8:
					CK_SpawnSkypest(x, y);
					NeedLump(SKYPEST);
					break;

				case 9:
					CK_SpawnPoleknawel(x, y);
					NeedLump(POLEKNAWEL);
					break;

				case 10:
					CK_SpawnKitten(x, y);
					NeedLump(KITTEN);
					break;

				case 11:
					CK_SpawnGellyGhost(x, y);
					NeedLump(GELLYGHOST);
					NeedLump(KEENKNOCKBACK);
					break;

				case 12:
					CK_SpawnBounder(x, y);
					NeedLump(BOUNDER);
					break;

				case 13:
					CK_SpawnBrainFucl(x, y);
					NeedLump(BRAINFUCL);
					break;

				case 14:
					CK_SpawnXkyLick(x, y);
					NeedLump(XKYLICK);
					break;

				case 15:
					CK_SpawnRoots(x, y);
					NeedLump(ROOTS);
					break;

				case 17:
					CK_SpawnYeti(x, y);
					NeedLump(YETI);
					break;

				case 18:
					CK_SpawnBalloonSmirky(x, y);
					NeedLump(SMIRKY);
					break;

				case 19:
					CK_SpawnCloud(x, y);
					CA_MarkGrChunk(BIGCLOUDSPR);
					break;

				case 20:
					CK_SpawnMolet(x, y);
					NeedLump(MOLET);
					/*
					NeedLump(TREASUREPOOF);
					*/
					break;

				case 21:
					CK_SpawnSneakyTree(x, y);
					NeedLump(SNEAKYTREE);
					break;
			/*
				case 19:
					CK_SpawnMimrock(x, y);
					NeedLump(MIMROCK);
					break;
			*/
				case 22:
					CK_SpawnSlug(x, y);
					NeedLump(SLUG);
					break;

				case 23:
					CK_SpawnBird(x, y);
					NeedLump(BIRD);
					break;

				case 24:
					CK_SpawnCroc(x, y);
					NeedLump(CROCO);
					break;

				case 25:
					RF_SetScrollBlock(x,y,true);
					break;
				case 26:
					RF_SetScrollBlock(x,y,false);
					break;

				case 27:
				case 28:
				case 29:
				case 30:
					CK_SpawnAxisPlatform(x,y,infoValue-27);
					NeedLump(PLATFORM);
					break;

				// 31 is the platform blocker icon!

				case 32:
					CK_SpawnFallPlat(x,y);
					NeedLump(PLATFORM);
					break;

				case 33:
					CK_SpawnTentacleNest(x, y);
					NeedLump(GRETENTA);
					CA_MarkGrChunk(EFREEKEEN1SPR);
					CA_MarkGrChunk(EFREEKEEN2SPR);
					break;

				case 34:
					if (ck_gameState.numShots < DEFAULT_SHOTS)
						CK_SpawnItem(x, y, 11)->user4 = -1;	// spawn it and mark it as an IF-Stunner
					break;

				case 35:
					CK_SpawnCheckpoint(x, y);
					NeedLump(CHECKPOINT);
					NeedLump(SMIRKYPOOF);
					break;

				case 36:
					CK_SpawnLeaveaTrip(x, y);
					NeedLump(LEAVEA);
					break;

				case 37:
				case 38:
					CK_SpawnPatPat(x,y,infoValue-37);
					NeedLump(PATPAT);
					break;

				case 39:
					CK_SpawnWarble(x, y);
					NeedLump(WARBLE);
					break;

				case 40:
					if (ck_gameState.currentLevel == MAP_BWBMEGAROCKET)
					{
						CK_SpawnStaticSprite(x, y, BWBSPR, 3);
						CK_SpawnStaticSprite(x, y, BWBBACKSPR, 0);
					}
					else
					{
					CK_SpawnStaticSprite(x, y, BWBSPR, FOREGROUNDPRIORITY);
					CK_SpawnStaticSprite(x, y, BWBBACKSPR, 0);
					}
					// Note: CK_SpawnStaticSprite marks the required chunk!
					break;

				case 41:
					// just cache the graphics - Keen's idle code spawns the object when needed
					NeedLump(EFREE);
					break;

				case 42:
					CK_SpawnNisasi(x, y);
					NeedLump(NISASI);
					NeedLump(YETICLOUD);	// for the smoke cloud sprites
					break;

				case 43:
					// this is the cloud start marker, search for the cloud end marker in the same row
					for (j=1; j+x < mapwidth; j++)
					{
						if (mapsegs[2][j+i] == 44)
						{
							// found the end marker, so remove it from the map to avoid the error message
							mapsegs[2][j+i] = 0;
							// spawn an invisible cloud object with the given hitbox (everything is in "global" units!)
							CK_SpawnInvisibleCloud(RF_TileToUnit(x), RF_TileToUnit(y), RF_TileToUnit(j+1), 11*PIXGLOBAL);
							goto found_cloud_end;
						}
					}
					// couldn't find a cloud end marker, indicating an error in the level design
					SM_Quit(S_BADCLOUDSTART);
found_cloud_end:
					break;
				case 44:
					// found a cloud end marker that hasn't been used, indicating an error in the level design
					SM_Quit(S_BADCLOUDEND);

				case 46:
					CK_SpawnWorldNessie(x, y);
					CA_MarkGrChunk(WORLDNESSIE);
					break;

				case 47:
					CK_SpawnFrog(x, y);
					CA_MarkGrChunk(FROGSPR);
					break;

				case 48:
					CK_SpawnCouncilPage(x, y, 1);
					NeedLump(PAGE);	
					/*
					CK_SpawnJediBoot(x, y);
					*/
					CA_MarkGrChunk(JEDIBOOTSPR);
					break;

				case 49:
					// maps with this scrolling type don't need the invisible 2-tile border
					if (!ck_mapState.wrapScroll)
					{
						extern void RFL_ClearScrollBlocks (void);
						ck_mapState.wrapScroll = true;
						originxmin += -MAPBORDER*TILEGLOBAL;
						originymin += -MAPBORDER*TILEGLOBAL;
						originxmax += MAPBORDER*TILEGLOBAL;
						originymax += MAPBORDER*TILEGLOBAL;
						RFL_ClearScrollBlocks();
					}
					break;

				case 50:
					CK_SpawnCouncilPage(x, y, 0);
					CK_SpawnSlideDoor(x, y, 0);
					NeedLump(PAGE);
					NeedLump(PAGEDOORS);
					break;

				case 51:
					NeedLump(POLEKNAWEL);
					CK_SpawnCloneMachine(x, y, 0, motion_Right);
					NeedLump(SCANNER);
					break;

				case 52:
					NeedLump(MOLET);
					CK_SpawnCloneMachine(x, y, 1, motion_Right);
					NeedLump(SCANNER);
					break;

				case 53:
					NeedLump(CHEMOLAIN);
					CK_SpawnCloneMachine(x, y, 2, motion_Right);
					NeedLump(SCANNER);
					break;

				case 54:
					NeedLump(NISASI);
					CK_SpawnCloneMachine(x, y, 3, motion_Right);
					NeedLump(SCANNER);
					break;

				case 55:
					NeedLump(COUNCIL);
					CK_SpawnCloneMachine(x, y, 4, motion_Right);
					NeedLump(SCANNER);
					break;

				case 56:
				//	CK_SpawnCouncilMember(x, y, 0, true);	
					CK_SpawnCouncilMember(x, y, 0);	// ALWAYS spawn!   // Nisaba patch
					NeedLump(COUNCIL);
					break;

				case 16:
					CK_SpawnFungusAmongus(x, y);
					NeedLump(FUNGUSAMONGUS);
					break;

					// Items
				case 57:
				case 58:
				case 59:
				case 60:
				case 61:
				case 62:
				case 63:
				case 64:
				case 65:
				case 66:
				case 67:
				case 68:
					CK_SpawnItem(x,y,infoValue-57);	
					break;

					// new object types added by K1n9_Duk3:
				case 69:
					CK_SpawnDungBeetle(x, y);
					NeedLump(DUNGBEETLE);
					break;

				case 70:
					CK_SpawnChemolain(x, y);
					NeedLump(CHEMOLAIN);
					NeedLump(KEENKNOCKBACK);
					break;

				case 71:
				case 72:
				case 73:
					CK_SpawnLogPlatform(x, y, infoValue-72);
					CA_MarkGrChunk(LOGSPR);	//don't really need to create a lump for this
					break;

				case 74:
					CK_SpawnSalamander(x, y);
					NeedLump(SALAMANDER);
					break;

				case 75:
					CK_SpawnShrubbery(x, y);
					NeedLump(SHRUBBERY);
					NeedLump(KEENKNOCKBACK);
					break;

				case 76:
				case 77:
				case 78:
					CK_SpawnCoronaPlant(x, y, infoValue-77);
					NeedLump(CORONA);
					break;

				case 79:
					CK_SpawnNipnap(x, y);
					NeedLump(NIPNAP);
					break;

				case 45:
				case 80:
					CK_SpawnCrab(x, y, infoValue & 1);
					NeedLump(CRAB);
					break;

				case 82:
				case 83:
				case 84:
				case 85:
				case 86:
				case 87:
				//case 88:
				CK_SpawnCollectible(x, y, infoValue-82);
					// Note: CK_SpawnCollectible marks the required chunks!
					break;
				
				case 88:
					CK_SpawnObelisk(x, y);
					CA_MarkGrChunk(OBELISKSPR);
					//CK_SpawnStaticSprite(x, y, OBELISKBOTTOMSPR, 1);
					//CK_SpawnStaticSprite(x, y, OBELISKTOPSPR, 0);
					break;

				case 89:
					CK_SpawnLindsey(x, y, 1);
					NeedLump(LINDSEY);
					NeedLump(SMIRKYPOOF);
					break;

				case 90:
					CK_SpawnDonut(x, y);
					NeedLump(FLYDONUT);
					break;

				case 99:
				case 100:
				case 101:
				case 102:
				case 103:
				case 104:
				case 105:
				case 106:
				case 107:
				case 108:
				case 109:
				case 110:
				case 111:
				case 112:
				case 113:
				case 114:
				//	CK_SpawnCouncilMember(x, y, infoValue-99, false);	
					CK_SpawnCouncilMember(x, y, infoValue-99);  // Nisaba patch
					NeedLump(COUNCIL);
					break;

				case 115:
					CK_SpawnFallBlock(x, y);
					NeedLump(SANDSTONE);
					break;

				case 116:
					CK_SpawnMummySpawner(x, y);
					NeedLump(MUMMY);
					break;
					
				case 117:
					CK_SpawnLemonShark(x, y);
					NeedLump(LEMONSHARK);
					break;
					
				case 118:
					CK_SpawnDonutCloud(x, y);
					CA_MarkGrChunk(DONUTCLOUDSPR);
					CA_MarkGrChunk(DONUTCLOUDSHADOWSPR);
					break;
				/*	
				case 119:
					CK_SpawnDonutCloudShadow(x, y);
					CA_MarkGrChunk(DONUTCLOUDSHADOWSPR);
					break;
				*/
				case 120:
					CK_SpawnJediBoot(x, y);
					CA_MarkGrChunk(JEDIBOOTSPR);
					break;
					
				case 121:
					CK_SpawnSlugCans(x, y);
					NeedLump(SLUGCANS);
					break;
					
				case 122:
					CK_SpawnBigShroom(x, y);
					NeedLump(SMIRKYPOOF);
					break;
					
				// Nisaba patch: Donut-Slug
				case 123:
					CK_SpawnSlug(x, y);
					ck_lastObject->user1 = 1;	// slug carries a donut!
					NeedLump(SLUG);
					ck_mapState.itemsTotal++;
					break;
			}

			// K1n9_Duk3 addition (for level statistics):
			infoValue = TI_ForeMisc(mapsegs[1][i]) & 0x7F;
			switch (infoValue)
			{
			case MF_Points100:
			case MF_Points200:
			case MF_Points500:
			case MF_Points1000:
			case MF_Points2000:
			case MF_Points5000:
				//ck_mapState.pointsTotal += CK5_ItemPoints[infoValue+4-MF_Points100];
				ck_mapState.itemsTotal++;
				break;
/*
			case MF_1UP:
				ck_mapState.livesTotal++;
				break;
			case MF_Stunner:
				ck_mapState.ammoTotal++;
				break;
			case MF_Centilife:
				ck_mapState.dropsTotal++;
				break;
*/
			}
			// end of addition
		}
	}

	// Inactivate all objects
	for (obj = ck_keenObj; obj != NULL; obj = obj->next)
	{
		if (obj->active != OBJ_ALWAYS_ACTIVE)
			obj->active = OBJ_INACTIVE;
	}

	// Cache chunks
	for (i = 0; i < NUMLUMPS; i++)
	{
		if (!ck_lumpsNeeded[i])
			continue;

		for (j = ck_lumpStarts[i]; j <= ck_lumpEnds[i]; j++)
		{
			grneeded[j] |= ca_levelbit;
		}
	}
}

/*
void near CacheAndDrawPic(int x, int y, graphicnums pic)
*/
static void near pascal CacheAndDrawPic(int x, int y, graphicnums pic)
{
	if (!grsegs[pic])
	{
		CA_CacheGrChunk(pic);
		CA_UncacheGrChunk(pic);	// Note: this just makes the chunk purgable, it can still be drawn
	}
	VWB_DrawPic(x, y, pic);
}

/*
void near CK_DialogWindow(graphicnums pic, stringnames str, boolean left, int wait)
*/
static void near pascal CK_DialogWindow(graphicnums pic, stringnames str, boolean left, int wait)
{
	unsigned w, h;

	US_CenterWindow(26, 8);
	WindowW -= 48;
	if (left)
	{
		CacheAndDrawPic(WindowX, WindowY, pic);
		WindowX += 48;
	}
	else
	{
		CacheAndDrawPic(WindowX+WindowW, WindowY, pic);
	}
	SM_MeasureMultiline(str, &w, &h);
	PrintY += (WindowH-h)/2;
	SM_CPrint(str);
	VW_UpdateScreen();
	if (wait)
	{
		VW_WaitVBL(wait);
		IN_ClearKeysDown();
		IN_Ack();
	}
}

void near CK_DialogThumbsUp(int wait)
{
	CacheAndDrawPic(WindowX+WindowW, WindowY, CD_KEENPIC);
	CacheAndDrawPic(WindowX+WindowW, WindowY+44, CD_ONEPIC);
	VW_UpdateScreen();
	if (wait)
	{
		VW_WaitVBL(wait);
		IN_ClearKeysDown();
		IN_Ack();
	}
}

void near CK_DialogEnter(void)
{
	CA_UpLevel();
	CA_SetGrPurge();
	VW_FixRefreshBuffer();
}

void near CK_DialogExit(void)
{
	CA_DownLevel();
	CK_ForceScoreboxUpdate();
	RF_ForceRefresh();
}

void CK_JanitorDialog(int number)
{
	unsigned bit = 1 << number;
	//TODO: stop music/start different music?
	CK_DialogEnter();
	SD_PlaySound(JANITORDIALOGSND);

	if (ck_gameState.janitorMessageShown & bit)
		CK_DialogWindow(JANITORPIC, S_JANITORJOKE0+number, true, 60);
	
	if (number == 0)	// Nisaba patch: Level 2 Dialog 
	{
		CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE0, true, 60);
		CK_DialogWindow(CD_KEENPIC, S_JANITORREPLY0, false, 30);
		CK_DialogWindow(JANITORMOPPIC, S_JANITORMESSAGE1, true, 60);
		CK_DialogWindow(KEENTALKPIC, S_JANITORREPLY1, false, 30);
		CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE2, true, 60);
	
		ck_gameState.janitorMessageShown |= bit;
		CK_DialogExit();
	}
	else if (number == 1)	// Level 4 Dialog 
	{
		CK_DialogWindow(KEENTALKPIC, S_JANITORREPLY2, false, 30);
		CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE3, true, 60);
		CK_DialogWindow(CD_KEENPIC, S_JANITORREPLY3, false, 30);
		CK_DialogWindow(JANITORMOPPIC, S_JANITORMESSAGE4, true, 60);
	
		ck_gameState.janitorMessageShown |= bit;
		CK_DialogExit();
	}
	else if (number == 2)	// Level 8 Dialog 
	{
		CK_DialogWindow(JANITORMOPPIC, S_JANITORMESSAGE5, true, 60);
		CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE6, true, 60);
		CK_DialogWindow(KEENTALKPIC, S_JANITORREPLY4, false, 30);
		CK_DialogThumbsUp(30);
	
		ck_gameState.janitorMessageShown |= bit;
		CK_DialogExit();
	}
	else if (number == 3)	// Level 9 Dialog 
	{
		CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE7, true, 60);
		CK_DialogWindow(KEENTALKPIC, S_JANITORREPLY5, false, 30);
	
		ck_gameState.janitorMessageShown |= bit;
		CK_DialogExit();
	}
	else if (number == 4)	// Level 11 Dialog 
	{
		CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE8, true, 60);
		CK_DialogWindow(KEENDIVERPIC, S_JANITORREPLY6, false, 30);
	
		ck_gameState.janitorMessageShown |= bit;
		CK_DialogExit();
	}
	else if (number == 5)	// Level 14 Dialog 
	{
		CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE9, true, 60);
		CK_DialogWindow(KEENMASKPIC, S_JANITORREPLY7, false, 30);
				
		ck_gameState.janitorMessageShown |= bit;
		CK_DialogExit();
	}
	else if (number == 6)	// Level 15 Dialog 
	{
		CK_DialogWindow(JANITORMOPPIC, S_JANITORMESSAGE10, true, 60);
		CK_DialogWindow(KEENTALKPIC, S_JANITORREPLY8, false, 30);
		CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE11, true, 60);
		CK_DialogWindow(KEENTALKPIC, S_JANITORREPLY9, false, 30);
			
		ck_gameState.janitorMessageShown |= bit;
		CK_DialogExit();
	}
	else if (number == 7)	// Level 24.1 Dialog 
	{
		CK_DialogWindow(KEENTALKPIC, S_JANITORREPLY10, false, 30);
		CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE12, true, 60);
		CK_DialogWindow(CD_KEENPIC, S_JANITORREPLY15, false, 30);
		CK_DialogWindow(JANITORMOPPIC, S_JANITORMESSAGE18, true, 60);
		CK_DialogWindow(KEENTALKPIC, S_JANITORREPLY9, false, 30);
		CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE19, true, 60);
			
		ck_gameState.janitorMessageShown |= bit;
		CK_DialogExit();
	}
	else if (number == 8)	// Level 24.2 Dialog 
	{
		CK_DialogWindow(KEENTALKPIC, S_JANITORREPLY11, false, 30);
		CK_DialogWindow(JANITORMOPPIC, S_JANITORMESSAGE13, true, 60);
		CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE14, true, 60);
		CK_DialogWindow(JANITORMOPPIC, S_JANITORMESSAGE20, true, 60);
	
		ck_gameState.janitorMessageShown |= bit;
		CK_DialogExit();
	}
	else if (number == 9)	// Level 24.3 Dialog 
	{
		CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE15, true, 60);
		CK_DialogWindow(KEENTALKPIC, S_JANITORREPLY12, false, 30);
		CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE16, true, 60);
		CK_DialogWindow(CD_KEENDONUTPIC, S_JANITORREPLY13, false, 30);
		
		ck_gameState.janitorMessageShown |= bit;
		CK_DialogExit();
	}
	else if (number == 10)	// Level 24.4 Dialog 
	{
		CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE17, true, 60);
		CK_DialogWindow(KEENTALKPIC, S_JANITORREPLY14, false, 30);
		
		ck_gameState.janitorMessageShown |= bit;
		CK_DialogExit();
	}
	/*
	else	
	{
	CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE0+number, true, 60);
	CK_DialogWindow(KEENTALKPIC, S_JANITORREPLY0+number, false, 30);
	CK_DialogWindow(JANITORPIC, S_JANITORMESSAGE1+number, true, 60);
	CK_DialogWindow(KEENTALKPIC, S_JANITORREPLY1+number, false, 30);
	CK_DialogThumbsUp(30);

	ck_gameState.janitorMessageShown |= bit;

	CK_DialogExit();
	}	
	*/
}

void CK_JanitorSleepMonolog(void)
{
	CK_DialogEnter();
	SD_PlaySound(JANITORDIALOGSND);

	CK_DialogWindow(JANITORMOPPIC, S_JANITORMONOLOG0+ck_mapState.dreamMsg, true, 0);
	IN_ClearKeysDown();
	IN_UserInput(8*TickBase, true);	// show message for 8 seconds, then resume the game

	if (++ck_mapState.dreamMsg >= S_JANITORMONOLOGEND-S_JANITORMONOLOG0)
		ck_mapState.dreamMsg = 0;

	CK_DialogExit();
}

void CK_JanitorStunnedMonolog(void)
{
	CK_DialogEnter();
	SD_PlaySound(JANITORDIALOGSND);

	CK_DialogWindow(JANITORPIC, S_JANITORSTUNNEDMSG0, true, 60);

	CK_DialogExit();
}

void CK_JanitorNisasiDialog(int number)
{
	unsigned oldfont = fontnumber;

	CK_DialogEnter();
	SD_PlaySound(JANITORDIALOGSND);

	CA_CacheGrChunk(STARTFONT+3);

	CK_DialogWindow(JANITORPIC, S_JANITORNISASIMESSAGE0+number, true, 60);
	fontnumber = 3;
	CK_DialogWindow(KEENNISASIPIC, S_JANITORNISASIREPLY0+number, false, 30);
	fontnumber = oldfont;

	CK_DialogExit();
}

void CK_LindseyDialog(int number)
{
	CK_DialogEnter();
	SD_PlaySound(FOOTAPPEARSND);
	CK_DialogWindow(LINDSEYPIC, S_LINDSEYMESSAGE0+number, true, 60);
	CK_DialogWindow(number==2? KEENDONUTPIC : CD_KEENPIC, S_LINDSEYREPLY0+number, false, 30);

	if (number == 2)
	{
		CK_DialogWindow(LINDSEYPIC, S_LINDSEYMESSAGE3, true, 60);
	/*
		VW_FadeOut();
		VW_ClearVideo(BLACK);
		VW_SetDefaultColors();
	*/
		CA_DownLevel();	// don't redraw the level, we're going straight to the loading screen anyway
	}
	else
	{
		CK_DialogThumbsUp(30);
		CK_DialogExit();
	}
}

void CK_MiscDialog(int number)
{
	CK_DialogEnter();
	
	switch (number)
	{
	case -1:	// Spaceman Spiff message
		StartMusic(TheBigV_Hill_Yoshi, true);
		SD_PlaySound(COMPSCOREDSND);
		CK_DialogWindow(SPACEMANSPIFFPIC, S_SPACEMANSPIFFMESSAGE1, false, 30);
		CK_DialogWindow(SPACEMANSPIFFPIC, S_SPACEMANSPIFFMESSAGE2, false, 30);
		
		// draw a centered window that is 36 TILE8M tiles wide and 21 tiles high:
		// (the size is the client area, i.e. the white part; 36x21 tiles = 288x168 pixels)
		US_CenterWindow(36, 21);
		PrintY += 4;	
		SM_CPrint(S_SPACEMANSPIFFMESSAGE3);
		VW_UpdateScreen();

		VW_WaitVBL(30);	// wait a while before reacting to user input
		IN_ClearKeysDown();
		IN_Ack();
	
		StartMusic(ck_mapState.music, true);
		break;
		
	case 0:	// HAL dialog
		SD_PlaySound(COMPSCOREDSND);
		CK_DialogWindow(KEENTALKPIC, S_HALMESSAGE1, false, 30);
		SD_PlaySound(COMPSCOREDSND);
		CK_DialogWindow(HALPIC, S_HALREPLY1, true, 60);
		CK_DialogWindow(KEENTALKPIC, S_HALMESSAGE2, false, 30);
		SD_PlaySound(COMPSCOREDSND);
		CK_DialogWindow(HALPIC, S_HALREPLY2, true, 60);
		SD_PlaySound(REFILLUNITSND);
		break;
		
	case 1:	// Jukebox
		if (MusicMode == smm_AdLib)
		{
			CK_Jukebox();
			
			// this will get merged with the Snailas and Foobs In Space! cases:
			IN_ClearKeysDown();
		}
		else
		{
			SD_PlaySound(GLITCHSND);
			CK_DialogWindow(TRANSCEIVERPIC, S_RADIOMESSAGE1, true, 30);
		}
		break;
		
	case 2:	// Snailas
		CK_FadeOut();	// optional
		RunSnailas();
		
		// this will get merged with the Foobs In Space! case:
		CK_SetLoadingTransition();	// to avoid display glitches in DOSBox
		IN_ClearKeysDown();
		break;
		
	case 3:	// Foobs In Space!
		CK_FadeOut();	// optional
		RunFoobsInSpace();
		
		CK_SetLoadingTransition();	// for consistency with Snailas behavior
		IN_ClearKeysDown();
		break;
		
	default:
		number -= 4;
		if ((unsigned)number < S_MISCMESSAGEEND-S_MISCMESSAGE4)
		{
			CK_DialogWindow(KEENTALKPIC, S_MISCMESSAGE4+number, false, 30);
		}
		else
		{
			SD_PlaySound(NOWAYSND);
		}

	}
	
	CK_DialogExit();
}

// outdated
/*
void CK_HALDialog(void)
{
	if (ck_gameState.currentLevel == MAP_ACMELABLEVEL) 
	{
		CK_DialogEnter();
	
		SD_PlaySound(COMPSCOREDSND);
		CK_DialogWindow(KEENTALKPIC, S_HALMESSAGE1, false, 30);
		SD_PlaySound(COMPSCOREDSND);
		CK_DialogWindow(HALPIC, S_HALREPLY1, true, 60);
		CK_DialogWindow(KEENTALKPIC, S_HALMESSAGE2, false, 30);
		SD_PlaySound(COMPSCOREDSND);
		CK_DialogWindow(HALPIC, S_HALREPLY2, true, 60);
	
		CK_DialogExit();
		SD_PlaySound(REFILLUNITSND);
	}

	else if (ck_gameState.currentLevel == MAP_BWBMEGAROCKET) 
	{
		if (MusicMode == smm_AdLib)
		{
			void CK_Jukebox(void);
			CK_Jukebox();
			return;
		}
		
		CK_DialogEnter();
	
		SD_PlaySound(GLITCHSND);
		CK_DialogWindow(TRANSCEIVERPIC, S_RADIOMESSAGE1, true, 30);
			
		CK_DialogExit();
*/		
		/*	// TODO: add mini game 2 to switch
		
		// switch to SNAILA5 minigame
		VW_FixRefreshBuffer();
		//StopMusic();
		US_CenterWindow(26,11);
		if (!grsegs[TRANSCEIVERPIC])
		{
		CA_CacheGrChunk(TRANSCEIVERPIC);
		CA_UncacheGrChunk(TRANSCEIVERPIC);	// Note: this just makes the chunk purgable, it can still be drawn
		}
		VWB_DrawPic(190, 74, TRANSCEIVERPIC);
		PlayMinigame();
		IN_Ack();//IN_WaitButton();
		
		// resuming current game
		//StartMusic(ck_mapState.music, true);
		CK_LevelPalette();
		CK_ForceScoreboxUpdate();
		IN_ClearKeysDown();
		RF_ForceRefresh();
		fontcolor = WHITE; 
		*/
/*
	}
	else if (ck_gameState.currentLevel == MAP_WORLDMAP) 
	{
		CK_DialogEnter();
		StopMusic();
		StartMusic(38, true);
		SD_PlaySound(COMPSCOREDSND);
		CK_DialogWindow(SPACEMANSPIFFPIC, S_SPACEMANSPIFFMESSAGE1, false, 30);
		CK_DialogWindow(SPACEMANSPIFFPIC, S_SPACEMANSPIFFMESSAGE2, false, 30);
		
		// draw a centered window that is 36 TILE8M tiles wide and 21 tiles high:
		// (the size is the client area, i.e. the white part; 36x21 tiles = 288x168 pixels)
		US_CenterWindow(36, 21);
		PrintY += 4;	
		SM_CPrint(S_SPACEMANSPIFFMESSAGE3);
		VW_UpdateScreen();

		VW_WaitVBL(30);	// wait a while before reacting to user input
		IN_ClearKeysDown();
		IN_Ack();
	
		CK_DialogExit();
		StopMusic();
		StartMusic(ck_mapState.music, true);
	}
}
*/

void CK_BonusLevelDialog(void)
{
//	if (ck_gameState.currentLevel == 0 && ck_gameState.keenScore >= 800		// just for testing reasons
	if (ck_gameState.currentLevel == 0 && ck_gameState.keenScore >= 500000
		&& !ck_gameState.levelsDone[MAP_BONUSLEVEL] && !ck_gameState.cheated)
	{
		// make sure dialog is only shown once:
		if (ck_gameState.collectiblesTotal & 0x8000)
			return;
		ck_gameState.collectiblesTotal |= 0x8000;
		
		CK_DialogEnter();
		
		SD_PlayTwoSounds(FOOTAPPEARSND, COLLECTEDALLPOINTSSND);
	//	CK_DialogWindow(H_CANDYOVERFLOWPIC, ck_gameState.keenScore ==    800? S_BONUSLEVELINFO1 : S_BONUSLEVELINFO2, true, 30);   // just for testing reasons
		CK_DialogWindow(H_CANDYOVERFLOWPIC, ck_gameState.keenScore == 500000? S_BONUSLEVELINFO1 : S_BONUSLEVELINFO2, true, 30);
			
		CK_DialogExit();
	}
}
