/* "Foray in the Forest" Source Code
 * Copyright (C) 2022-2023 Nisaba
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/////////////////////////////////////////////////
//
// MUSE Header for "Foray in the Forest"
//
/////////////////////////////////////////////////

#define NUMSOUNDS		LASTSOUND
#define NUMMUSIC		LASTMUSIC
#define NUMSNDCHUNKS		(2*NUMSOUNDS+NUMMUSIC)

//
// Base offsets
//
#define STARTPCSOUNDS		0
#define STARTADLIBSOUNDS	(STARTPCSOUNDS+NUMSOUNDS)
#define STARTMUSIC		(STARTADLIBSOUNDS+NUMSOUNDS)

//
// Sound names & indexes
//
typedef enum {
	KEENWALK1SND,             // 0
	KEENWALK2SND,             // 1
	KEENJUMPSND,              // 2
	KEENLANDSND,              // 3
	KEENSHOOTSND,             // 4
	WORMMOUTHBITESND,         // 5
	SLICEBUMPSND,             // 6
	KEENPOGOSND,              // 7
	GOTITEMSND,               // 8
	GOTSTUNNERSND,            // 9
	GOTVITALINSND,            // 10
	GOTEXTRALIFEK6SND,        // 11
	ENTERLEVELSND,            // 12
	LEVELDONESND,             // 13
	OLDNOWAYSND,              // 14
	KEENHITCEILINGSND,        // 15
	TENTACLESWOOSHSND,        // 16
	GOTEXTRALIFESND,          // 17
	OPENGEMDOORSND,           // 18
	GOTGEMSND,                // 19
	KEENFALLSND,              // 20
	CLICKSND,                 // 21
	SQUISHSND,                // 22
	KEENDIESND,               // 23
	KITTENMEOWSND,            // 24
	KEENSHOTHITSND,           // 25
	KEENSWIM1SND,             // 26
	KEENSWIM2SND,             // 27
	BOUNCESND,                // 28
	SMIRKYSTEALSND,           // 29
	SMIRKYTELESND,            // 30
	COUNCILSAVESND,           // 31
	OPTIQUATLICKSND,          // 32
	BERKELOIDTHROWSND,        // 33
	STATUSDOWNSND,            // 34
	STATUSUPSND,              // 35
	KEENBUBBLESND,            // 36
	KITTENEXPLODESND,         // 37
	SPRITESHOOTSND,           // 38
	BIRDLAYSEGGSND,           // 39
	FIREBALLLANDSND,          // 40
	SLUGSLIMESND,             // 41
	COLLECTEDALLPOINTSSND,    // 42
	FLAGSPINSND,              // 43
	FLAGLANDSND,              // 44
	FOOTAPPEARSND,            // 45
	YETICLOUDSND,             // 46
	KEENPADDLESND,            // 47
	BALLBOUNCESND,            // 48
	COMPPADDLESND,            // 49
	COMPSCOREDSND,            // 50
	KEENSCOREDSND,            // 51
	DEADTHNESSISND,	          // 52
	REFILLUNITSND,	          // 53
	NOPESND,                  // 54
	KEENSTUNNEDSND,	          // 55
	ROOTSSND,                 // 56
	CHECKPOINT2SND,           // 57
	GRABBINGSND,              // 58
	GLITCHSND,                // 59
	CHECKPOINTSND,            // 60
	GOODIESSND,               // 61
	DEATHTENTACLENESTSND,     // 62
	MUMMYSND,                 // 63
	MUMMYENWRAPPINGSND,       // 64
	JANITORDIALOGSND,         // 65
	BIRDSND,                  // 66
	EGGCRACKSND,              // 67
	FUNGUSSND,                // 68
	
	LASTSOUND
} soundnames;

#define NOWAYSND REFILLUNITSND
//
// Music names & indexes
//
typedef enum {
	TSK_Keenish_Tune,              // 0
	Kohntrakosz__Commercials_v3,   // 1
	MOM4Evr_The_Lady_is_Frightful, // 2
	MR_M_Nitelite,                 // 3
	kvee_Oasis_v17,                // 4
	BobbyPrince_Jazzin_Them_Nazis, // 5
	BobbyPrince_Guess,             // 6
	MOM4Evr_Algeria,               // 7
	unknown_JAZZK,                 // 8
	eggsbox_Fonk,                  // 9
	kvee_2hot_v4,                  // 10
	eggsbox_Spoops,                // 11
	MR_M_Education,                // 12
	Kohntrakosz__Tropic_v2_640,    // 13
	eggsbox_pyrska2,               // 14
	eggsbox_littlekeenthing,       // 15
	kvee_icy_f_10rppq_nisaba_rmx,  // 16
	eggsbo_Return_v2,              // 17
	Gridlock_Colder,               // 18
	Kohntrakosz_Strut_500,         // 19
	TMST_kickkeen,                 // 20
	Summers_Beach,                 // 21
	kvee_Midnight_v5_slow,         // 22
	IncredibleMachine_Cruise,      // 23
	kvee_Slowasis_v4,              // 24
	IncredibleMachine_Fusion,      // 25
	LeeJackson_dopejaws,           // 26	
	eggsbox_Underground,           // 27
	JosephCollins_Slipping_Slug,   // 28
	kvee_icy_f_10rppq,             // 16
	kvee_Too_Hot_v5b,              // 30
	Kohntrakosz__Nashville,        // 31
	MrM_MortLab,                   // 32
	eggsbox_fonk_vari_2,           // 33
	eggsbox_fonk_vari_3b,          // 34
	eggsbox_doodle_rmx,            // 35
	Kohntrakosz__JoJackson,        // 36
	eggsbox_bugs_will_bite,        // 37
	TheBigV_Hill_Yoshi,            // 38

	LASTMUSIC
} musicnames;

/////////////////////////////////////////////////
//
// Thanks for playing with MUSE!
//
/////////////////////////////////////////////////
