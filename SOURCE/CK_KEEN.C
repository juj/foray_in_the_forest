/* "Foray in the Forest" Source Code
 * Copyright (C) 2022-2023 Nisaba
 * Copyright (C) 2019-2021 K1n9_Duk3
 *
 * The code in this file is based on:
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

// KD_KEEN.C

#include "CK_DEF.H"
#pragma hdrstop

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define KEENPRIORITY 1

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/


CK_action CK_ACT_keenStanding;
CK_action CK_ACT_KeenShrug;
CK_action CK_ACT_keenRidePlatform;
CK_action CK_ACT_keenLookUp1;
CK_action CK_ACT_keenLookUp2;
CK_action CK_ACT_keenLookDown1;
CK_action CK_ACT_keenLookDown2;
CK_action CK_ACT_keenLookDown3;
CK_action CK_ACT_keenLookDown4;
CK_action CK_ACT_keenIdle;
CK_action CK_ACT_keenBored1;
CK_action CK_ACT_keenBored2;
CK_action CK_ACT_keenBored3;
CK_action CK_ACT_keenBored4;
CK_action CK_ACT_keenBored5;
CK_action CK_ACT_keenBored6;

CK_action CK_ACT_keenYawn1;
CK_action CK_ACT_keenYawn2;
CK_action CK_ACT_keenYawn3;
CK_action CK_ACT_keenYawn4;
CK_action CK_ACT_keenYawn5;
CK_action CK_ACT_keenYawn6;
CK_action CK_ACT_keenYawn7;
CK_action CK_ACT_keenYawn8;
CK_action CK_ACT_keenYawn9;
CK_action CK_ACT_keenGoToSleep1;
CK_action CK_ACT_keenGoToSleep2;
CK_action CK_ACT_keenGoToSleep3;
CK_action CK_ACT_keenDream;
CK_action CK_ACT_keenDream2;
CK_action CK_ACT_keenGetUp;
CK_action CK_ACT_keenGetUp2;
CK_action CK_ACT_keenGetUp3;
CK_action CK_ACT_dreamBubble;

/*
CK_action CK_ACT_keenOpenBook1;
CK_action CK_ACT_keenOpenBook2;
CK_action CK_ACT_keenOpenBook3;
CK_action CK_ACT_keenOpenBook4;
CK_action CK_ACT_keenReadBook1;
CK_action CK_ACT_keenReadBook2;
CK_action CK_ACT_keenReadBook3;
CK_action CK_ACT_keenStowBook1;
CK_action CK_ACT_keenStowBook2;
CK_action CK_ACT_keenStowBook3;
*/

CK_action CK_ACT_keenNot;
CK_action CK_ACT_keenDie0;
CK_action CK_ACT_keenDie1;
CK_action CK_ACT_keenRun1;
CK_action CK_ACT_keenRun2;
CK_action CK_ACT_keenRun3;
CK_action CK_ACT_keenRun4;
CK_action CK_ACT_keenShoot1;
CK_action CK_ACT_keenShoot2;
CK_action CK_ACT_keenShootUp1;
CK_action CK_ACT_keenShootUp2;
CK_action CK_ACT_keenJump1;
CK_action CK_ACT_keenJump2;
CK_action CK_ACT_keenFall1;
CK_action CK_ACT_keenFall2;
CK_action CK_ACT_keenJumpShoot1;
CK_action CK_ACT_keenJumpShoot2;
CK_action CK_ACT_keenJumpShoot3;
CK_action CK_ACT_keenJumpShootUp1;
CK_action CK_ACT_keenJumpShootUp2;
CK_action CK_ACT_keenJumpShootUp3;
CK_action CK_ACT_keenJumpShootDown1;
CK_action CK_ACT_keenJumpShootDown2;
CK_action CK_ACT_keenJumpShootDown3;
CK_action CK_ACT_keenPogo1;
CK_action CK_ACT_keenPogo2;
CK_action CK_ACT_keenPogo3;
CK_action CK_ACT_keenHang1;
CK_action CK_ACT_keenHang2;
CK_action CK_ACT_keenPull1;
CK_action CK_ACT_keenPull2;
CK_action CK_ACT_keenPull3;
CK_action CK_ACT_keenPull4;
CK_action CK_ACT_keenPull5;
CK_action CK_ACT_keenPoleSit;
CK_action CK_ACT_keenPoleUp1;
CK_action CK_ACT_keenPoleUp2;
CK_action CK_ACT_keenPoleUp3;
CK_action CK_ACT_keenPoleDown1;
CK_action CK_ACT_keenPoleDown2;
CK_action CK_ACT_keenPoleDown3;
CK_action CK_ACT_keenPoleDown4;
CK_action CK_ACT_keenPoleShoot1;
CK_action CK_ACT_keenPoleShoot2;
CK_action CK_ACT_keenPoleShootUp1;
CK_action CK_ACT_keenPoleShootUp2;
CK_action CK_ACT_keenPoleShootDown1;
CK_action CK_ACT_keenPoleShootDown2;
CK_action CK_ACT_keenPoleShoot0;
CK_action CK_ACT_keenPoleShootUp0;
CK_action CK_ACT_keenPoleShootDown0;

CK_action CK_ACT_keenShot1;
CK_action CK_ACT_keenShot2;
CK_action CK_ACT_keenShot3;
CK_action CK_ACT_keenShot4;

CK_action CK_ACT_keenShotHit1;
CK_action CK_ACT_keenShotHit2;

CK_action CK_ACT_keenSlide;

CK_action CK_ACT_keenPlaceGem;

CK_action CK_ACT_keenPressSwitch1;
CK_action CK_ACT_keenPressSwitch2;

CK_action CK_ACT_keenEnterDoor1;
CK_action CK_ACT_keenEnterDoor2;
CK_action CK_ACT_keenEnterDoor3;
CK_action CK_ACT_keenEnterDoor4;
CK_action CK_ACT_keenEnterDoor5;
CK_action CK_ACT_keenEnterDoor6;
CK_action CK_ACT_keenEnteredDoor;


void CK_KeenColFunc(CK_object *a, CK_object *b);
void CK_IncreaseScore(int score);
void CK_SpawnKeen(int tileX, int tileY, int direction);
void CK_KeenGetTileItem(int tileX, int tileY, int itemNumber);
void CK_GetVitalin(int tileX, int tileY);
void CK_KeenGetTileVitalin(int tileX, int tileY);
void CK_KeenCheckSpecialTileInfo(CK_object *obj);
void CK_KeenPressSwitchThink(CK_object *obj);
boolean CK_KeenPressUp(CK_object *obj);
void CK_KeenSlide(CK_object *obj);
void CK_KeenEnterDoor0(CK_object *obj);
void CK_KeenEnterDoor1(CK_object *obj);
void CK_KeenEnterDoor(CK_object *obj);
void CK_KeenPlaceGem(CK_object *obj);
void CK_KeenRidePlatform(CK_object *obj);
boolean CK_KeenTryClimbPole(CK_object *obj);
void CK_KeenRunningThink(CK_object *obj);
void CK_HandleInputOnGround(CK_object *obj);
void CK_KeenStandingThink(CK_object *obj);
void CK_KeenLookUpThink(CK_object *obj);
void CK_KeenLookDownThink(CK_object *obj);
void CK_KeenDrawFunc(CK_object *obj);
void CK_KeenRunDrawFunc(CK_object *obj);
//void CK_KeenReadThink(CK_object *obj);
void CK_KeenJumpThink(CK_object *obj);
void CK_KeenJumpDrawFunc(CK_object *obj);
void CK_KeenPogoBounceThink(CK_object *obj);
void CK_KeenPogoThink(CK_object *obj);
void CK_KeenToggleTrapDoor(int x, int y);
void CK_KeenPogoDrawFunc(CK_object *obj);
void CK_KeenSpecialColFunc(CK_object *obj, CK_object *other);
void CK_KeenSpecialDrawFunc(CK_object *obj);
void CK_KeenHangThink(CK_object *obj);
void CK_KeenPullThink1(CK_object *obj);
void CK_KeenPullThink2(CK_object *obj);
void CK_KeenPullThink3(CK_object *obj);
void CK_KeenPullThink4(CK_object *obj);
void CK_KeenDeathThink(CK_object *obj);
void CK_KillKeen();
void CK_KillKeenEx(boolean force);
void CK_KeenPoleHandleInput(CK_object *obj);
void CK_KeenPoleSitThink(CK_object *obj);
void CK_KeenPoleUpThink(CK_object *obj);
void CK_KeenPoleDownThink(CK_object *obj);
void CK_KeenPoleShootThink(CK_object *obj);
void CK_KeenPoleDownDrawFunc(CK_object *obj);
void CK_SpawnShot(int x, int y, int direction);
void CK_ShotHit(CK_object *obj);
void CK_ShotThink(CK_object *shot);
void CK_ShotDrawFunc(CK_object *obj);
void CK_KeenSpawnShot(CK_object *obj);
void CK_KeenFall(CK_object *obj);
void CK_KeenSink(CK_object *obj);

// New stuff
#if 1
void CK_KeenStartDream(CK_object *obj);
void CK_KeenDreamThink(CK_object *obj);
void CK_DreamThink(CK_object *obj);
void CK_DreamReact(CK_object *obj);

//void CK_DamageKeen(unsigned x, unsigned y);
//void CK_KeenKnockbackReact(CK_object *obj);
void CK_KeenStunnedRecover(CK_object *obj);
void CK_KeenStunnedReact(CK_object *obj);
#endif

CK_action CK_ACT_keenStanding = {KEENSTANDLSPR, KEENSTANDRSPR, AT_UnscaledFrame, 0, 1, 4, 0, 32, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenStanding};

CK_action CK_ACT_keenRidePlatform = {0x8B, 0x8B, AT_UnscaledFrame, 0, 1, 4, 0, 32, CK_HandleInputOnGround, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenRidePlatform};

CK_action CK_ACT_keenLookUp1 = {KEENLOOKUPSPR, KEENLOOKUPSPR, AT_UnscaledFrame, 0, 1, 30, 0, 0, CK_KeenLookUpThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenLookUp2};
CK_action CK_ACT_keenLookUp2 = {KEENLOOKUPSPR, KEENLOOKUPSPR, AT_Frame, 0, 1, 0, 0, 0, CK_KeenLookUpThink, CK_KeenColFunc, CK_KeenDrawFunc, NULL};

CK_action CK_ACT_keenLookDown1 = {KEENLOOKDOWN1SPR, KEENLOOKDOWN1SPR, AT_UnscaledFrame, 0, 1, 6, 0, 0, CK_KeenLookDownThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenLookDown2};
CK_action CK_ACT_keenLookDown2 = {KEENLOOKDOWN2SPR, KEENLOOKDOWN2SPR, AT_UnscaledFrame, 0, 1, 0x18, 0, 0, CK_KeenLookDownThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenLookDown3};
CK_action CK_ACT_keenLookDown3 = {KEENLOOKDOWN2SPR, KEENLOOKDOWN2SPR, AT_Frame, 0, 1, 0, 0, 0, CK_KeenLookDownThink, CK_KeenColFunc, CK_KeenDrawFunc, NULL};
CK_action CK_ACT_keenLookDown4 = {KEENLOOKDOWN1SPR, KEENLOOKDOWN1SPR, AT_UnscaledOnce, 0, 1, 6, 0, 0, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenStanding};

CK_action CK_ACT_keenIdle = {KEENLOOKUPSPR, KEENLOOKUPSPR, AT_UnscaledFrame, 0, 1, 0x3C, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenStanding};

CK_action CK_ACT_keenBored1 = {KEENANGRY2SPR, KEENANGRY2SPR, AT_UnscaledFrame, 0, 1, 0x5A, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenBored2};
CK_action CK_ACT_keenBored2 = {KEENANGRY1SPR, KEENANGRY1SPR, AT_UnscaledFrame, 0, 1, 10, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenBored3};
CK_action CK_ACT_keenBored3 = {KEENANGRY2SPR, KEENANGRY2SPR, AT_UnscaledFrame, 0, 1, 0x5A, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenBored4};
CK_action CK_ACT_keenBored4 = {KEENANGRY1SPR, KEENANGRY1SPR, AT_UnscaledFrame, 0, 1, 10, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenBored5};
CK_action CK_ACT_keenBored5 = {KEENANGRY2SPR, KEENANGRY2SPR, AT_UnscaledFrame, 0, 1, 0x5A, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenBored6};
CK_action CK_ACT_keenBored6 = {KEENSHRUGSPR, KEENSHRUGSPR, AT_UnscaledFrame, 0, 1, 0x46, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenStanding};

CK_action CK_ACT_keenYawn1 = {KEENANGRY2SPR, KEENANGRY2SPR, AT_UnscaledFrame, 0, 1, 0x5A, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenYawn2};
CK_action CK_ACT_keenYawn2 = {KEENANGRY1SPR, KEENANGRY1SPR, AT_UnscaledFrame, 0, 1, 10, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenYawn3};
CK_action CK_ACT_keenYawn3 = {KEENANGRY2SPR, KEENANGRY2SPR, AT_UnscaledFrame, 0, 1, 0x5A, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenYawn4};
CK_action CK_ACT_keenYawn4 = {KEENANGRY1SPR, KEENANGRY1SPR, AT_UnscaledFrame, 0, 1, 10, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenYawn5};
CK_action CK_ACT_keenYawn5 = {KEENANGRY2SPR, KEENANGRY2SPR, AT_UnscaledFrame, 0, 1, 0x5A, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenYawn6};
CK_action CK_ACT_keenYawn6 = {KEENYAWN1SPR, KEENYAWN1SPR, AT_UnscaledFrame, false, true, 20, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenYawn7};
CK_action CK_ACT_keenYawn7 = {KEENYAWN2SPR, KEENYAWN2SPR, AT_UnscaledFrame, false, true, 40, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenYawn8};
CK_action CK_ACT_keenYawn8 = {KEENYAWN3SPR, KEENYAWN3SPR, AT_UnscaledFrame, false, true, 40, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenYawn9};
CK_action CK_ACT_keenYawn9 = {KEENYAWN4SPR, KEENYAWN4SPR, AT_UnscaledFrame, false, true, 30, 0, 0, CK_KeenStandingThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenGoToSleep1};

CK_action CK_ACT_keenGoToSleep1 = {KEENSLEEP1SPR, KEENSLEEP1SPR, AT_UnscaledOnce, false, true, 15, 0, 0, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenGoToSleep2};
CK_action CK_ACT_keenGoToSleep2 = {KEENSLEEP2SPR, KEENSLEEP2SPR, AT_UnscaledOnce, false, true, 15, 0, 0, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenGoToSleep3};
CK_action CK_ACT_keenGoToSleep3 = {KEENSLEEP3SPR, KEENSLEEP3SPR, AT_Frame, false, true, 15, 0, 0, CK_KeenStartDream, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenDream};
CK_action CK_ACT_keenDream  = {KEENSLEEP3SPR, KEENSLEEP3SPR, AT_UnscaledFrame, false, true, 120, 0, 0, CK_KeenDreamThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenDream2};
CK_action CK_ACT_keenDream2 = {KEENSLEEP4SPR, KEENSLEEP4SPR, AT_UnscaledFrame, false, true, 120, 0, 0, CK_KeenDreamThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenDream};
CK_action CK_ACT_keenGetUp = {KEENSLEEP3SPR, KEENSLEEP3SPR, AT_UnscaledOnce, false, true, 15, 0, 0, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenGetUp2};
CK_action CK_ACT_keenGetUp2 = {KEENSLEEP2SPR, KEENSLEEP2SPR, AT_UnscaledOnce, false, true, 15, 0, 0, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenGetUp3};
CK_action CK_ACT_keenGetUp3 = {KEENGETUPSPR, KEENGETUPSPR, AT_UnscaledOnce, false, true, 15, 0, 0, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenStanding};

CK_action CK_ACT_dreamBubble = {0, 0, AT_Frame, false, 0, 8, 0, 0, CK_DreamThink, NULL, CK_DreamReact, &CK_ACT_dreamBubble};

/*
CK_action CK_ACT_keenOpenBook1 = {KEENBOOK1SPR, KEENBOOK1SPR, AT_UnscaledOnce, 0, 1, 16, 0, 0, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenOpenBook2};
CK_action CK_ACT_keenOpenBook2 = {KEENBOOK2SPR, KEENBOOK2SPR, AT_UnscaledOnce, 0, 1, 16, 0, 0, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenOpenBook3};
CK_action CK_ACT_keenOpenBook3 = {KEENBOOK3SPR, KEENBOOK3SPR, AT_UnscaledOnce, 0, 1, 16, 0, 0, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenOpenBook4};
CK_action CK_ACT_keenOpenBook4 = {KEENBOOK4SPR, KEENBOOK4SPR, AT_UnscaledOnce, 0, 1, 16, 0, 0, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenReadBook1};

CK_action CK_ACT_keenReadBook1 = {KEENBOOKREADSPR, KEENBOOKREADSPR, AT_UnscaledFrame, 0, 1, 300, 0, 0, CK_KeenReadThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenReadBook2};
CK_action CK_ACT_keenReadBook2 = {KEENBOOKFLIP1SPR, KEENBOOKFLIP1SPR, AT_UnscaledFrame, 0, 1, 16, 0, 0, CK_KeenReadThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenReadBook3};
CK_action CK_ACT_keenReadBook3 = {KEENBOOKFLIP2SPR, KEENBOOKFLIP2SPR, AT_UnscaledFrame, 0, 1, 16, 0, 0, CK_KeenReadThink, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenReadBook1};

CK_action CK_ACT_keenStowBook1 = {KEENPUTBOOKAWAY1SPR, KEENPUTBOOKAWAY1SPR, AT_UnscaledOnce, 0, 1, 12, 0, 0, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenStowBook2};
CK_action CK_ACT_keenStowBook2 = {KEENPUTBOOKAWAY2SPR, KEENPUTBOOKAWAY2SPR, AT_UnscaledOnce, 0, 1, 12, 0, 0, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenStanding};
#if 0
CK_action CK_ACT_keenStowBook3 = {KEENLOOKATWATCHSPR, KEENLOOKATWATCHSPR, AT_UnscaledOnce, 0, 1, 12, 0, 0, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenStanding};
#endif
*/

CK_action CK_ACT_keenNot = {-1, -1, AT_Frame, 0, 0, 10, 0, 0, NULL, NULL, CK_BasicDrawFunc1, NULL};
CK_action CK_ACT_keenDie0 = {KEENDIE1SPR, KEENDIE1SPR, AT_Frame, 0, 0, 0x64, 0, 0, CK_KeenDeathThink, NULL, CK_BasicDrawFunc1, &CK_ACT_keenDie0};
CK_action CK_ACT_keenDie1 = {KEENDIE2SPR, KEENDIE2SPR, AT_Frame, 0, 0, 0x64, 0, 0, CK_KeenDeathThink, NULL, CK_BasicDrawFunc1, &CK_ACT_keenDie1};

CK_action CK_ACT_keenRun1 = {KEENRUNL1SPR, KEENRUNR1SPR, AT_ScaledFrame, 1, 1, 6, 24, 0, CK_KeenRunningThink, CK_KeenColFunc, CK_KeenRunDrawFunc, &CK_ACT_keenRun2};
CK_action CK_ACT_keenRun2 = {KEENRUNL2SPR, KEENRUNR2SPR, AT_ScaledFrame, 1, 1, 6, 24, 0, CK_KeenRunningThink, CK_KeenColFunc, CK_KeenRunDrawFunc, &CK_ACT_keenRun3};
CK_action CK_ACT_keenRun3 = {KEENRUNL3SPR, KEENRUNR3SPR, AT_ScaledFrame, 1, 1, 6, 24, 0, CK_KeenRunningThink, CK_KeenColFunc, CK_KeenRunDrawFunc, &CK_ACT_keenRun4};
CK_action CK_ACT_keenRun4 = {KEENRUNL4SPR, KEENRUNR4SPR, AT_ScaledFrame, 1, 1, 6, 24, 0, CK_KeenRunningThink, CK_KeenColFunc, CK_KeenRunDrawFunc, &CK_ACT_keenRun1};

CK_action CK_ACT_keenShoot1 = {KEENSHOOTLEFTSPR, KEENSHOOTRIGHTSPR, AT_UnscaledOnce, 0, 1, 9, 0, 0x20, CK_KeenSpawnShot, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenShoot2};
CK_action CK_ACT_keenShoot2 = {KEENSHOOTLEFTSPR, KEENSHOOTRIGHTSPR, AT_UnscaledOnce, 0, 1, 6, 0, 0x20, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenStanding};

CK_action CK_ACT_keenShootUp1 = {KEENSHOOTUPSPR, KEENSHOOTUPSPR, AT_UnscaledOnce, 0, 1, 9, 0, 0x20, CK_KeenSpawnShot, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenShootUp2};
CK_action CK_ACT_keenShootUp2 = {KEENSHOOTUPSPR, KEENSHOOTUPSPR, AT_UnscaledOnce, 0, 1, 6, 0, 0x20, NULL, CK_KeenColFunc, CK_KeenDrawFunc, &CK_ACT_keenStanding};

CK_action CK_ACT_keenJump1 = {KEENJUMPL1SPR, KEENJUMPR1SPR, AT_Frame, 0, 0, 0, 0, 0, CK_KeenJumpThink, CK_KeenColFunc, CK_KeenJumpDrawFunc, &CK_ACT_keenJump2};
CK_action CK_ACT_keenJump2 = {KEENJUMPL2SPR, KEENJUMPR2SPR, AT_Frame, 0, 0, 0, 0, 0, CK_KeenJumpThink, CK_KeenColFunc, CK_KeenJumpDrawFunc, &CK_ACT_keenFall1};

CK_action CK_ACT_keenFall1 = {KEENJUMPL3SPR, KEENJUMPR3SPR, AT_UnscaledFrame, 0, 0, 0x32, 0, 0, CK_KeenJumpThink, CK_KeenColFunc, CK_KeenJumpDrawFunc, &CK_ACT_keenFall2};
CK_action CK_ACT_keenFall2 = {KEENJUMPL2SPR, KEENJUMPR2SPR, AT_UnscaledFrame, 0, 0, 0x28, 0, 0, CK_KeenJumpThink, CK_KeenColFunc, CK_KeenJumpDrawFunc, &CK_ACT_keenFall1};

CK_action CK_ACT_keenJumpShoot1 = {KEENJUMPSHOOTLEFTSPR, KEENJUMPSHOOTRIGHTSPR, AT_UnscaledFrame, 0, 0, 9, 0, 0, CK_KeenFall, CK_KeenColFunc, CK_KeenJumpDrawFunc, &CK_ACT_keenJumpShoot2};
CK_action CK_ACT_keenJumpShoot2 = {KEENJUMPSHOOTLEFTSPR, KEENJUMPSHOOTRIGHTSPR, AT_UnscaledFrame, 1, 0, 1, 0, 0, CK_KeenSpawnShot, CK_KeenColFunc, CK_KeenJumpDrawFunc, &CK_ACT_keenJumpShoot3};
CK_action CK_ACT_keenJumpShoot3 = {KEENJUMPSHOOTLEFTSPR, KEENJUMPSHOOTRIGHTSPR, AT_UnscaledFrame, 0, 0, 6, 0, 0, CK_KeenFall, CK_KeenColFunc, CK_KeenJumpDrawFunc, &CK_ACT_keenFall1};

CK_action CK_ACT_keenJumpShootUp1 = {KEENJUMPSHOOTUPSPR, KEENJUMPSHOOTUPSPR, AT_UnscaledFrame, 0, 0, 9, 0, 0, CK_KeenFall, CK_KeenColFunc, CK_KeenJumpDrawFunc, &CK_ACT_keenJumpShootUp2};
CK_action CK_ACT_keenJumpShootUp2 = {KEENJUMPSHOOTUPSPR, KEENJUMPSHOOTUPSPR, AT_UnscaledFrame, 1, 0, 1, 0, 0, CK_KeenSpawnShot, CK_KeenColFunc, CK_KeenJumpDrawFunc, &CK_ACT_keenJumpShootUp3};
CK_action CK_ACT_keenJumpShootUp3 = {KEENJUMPSHOOTUPSPR, KEENJUMPSHOOTUPSPR, AT_UnscaledFrame, 0, 0, 6, 0, 0, CK_KeenFall, CK_KeenColFunc, CK_KeenJumpDrawFunc, &CK_ACT_keenFall1};

CK_action CK_ACT_keenJumpShootDown1 = {KEENJUMPSHOOTDOWNSPR, KEENJUMPSHOOTDOWNSPR, AT_UnscaledFrame, 0, 0, 9, 0, 0, CK_KeenFall, CK_KeenColFunc, CK_KeenJumpDrawFunc, &CK_ACT_keenJumpShootDown2};
CK_action CK_ACT_keenJumpShootDown2 = {KEENJUMPSHOOTDOWNSPR, KEENJUMPSHOOTDOWNSPR, AT_UnscaledFrame, 1, 0, 1, 0, 0, CK_KeenSpawnShot, CK_KeenColFunc, CK_KeenJumpDrawFunc, &CK_ACT_keenJumpShootDown3};
CK_action CK_ACT_keenJumpShootDown3 = {KEENJUMPSHOOTDOWNSPR, KEENJUMPSHOOTDOWNSPR, AT_UnscaledFrame, 0, 0, 6, 0, 0, CK_KeenFall, CK_KeenColFunc, CK_KeenJumpDrawFunc, &CK_ACT_keenFall1};

CK_action CK_ACT_keenPogo1 = {KEENPOGOBOUNCELSPR, KEENPOGOBOUNCERSPR, AT_UnscaledOnce, 1, 0, 1, 0, 0, CK_KeenPogoBounceThink, CK_KeenColFunc, CK_KeenPogoDrawFunc, &CK_ACT_keenPogo2};
CK_action CK_ACT_keenPogo2 = {KEENPOGOBOUNCELSPR, KEENPOGOBOUNCERSPR, AT_Frame, 1, 0, 0, 0, 0, CK_KeenPogoThink, CK_KeenColFunc, CK_KeenPogoDrawFunc, &CK_ACT_keenPogo3};
CK_action CK_ACT_keenPogo3 = {KEENPOGOLSPR, KEENPOGORSPR, AT_Frame, 1, 0, 0, 0, 0, CK_KeenPogoThink, CK_KeenColFunc, CK_KeenPogoDrawFunc, NULL};

CK_action CK_ACT_keenHang1 = {KEENHANGLSPR, KEENHANGRSPR, AT_UnscaledOnce, 0, 0, 12, 0, 0, NULL, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenHang2};
CK_action CK_ACT_keenHang2 = {KEENHANGLSPR, KEENHANGRSPR, AT_Frame, 0, 0, 0, 0, 0, CK_KeenHangThink, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, NULL};

CK_action CK_ACT_keenPull1 = {KEENCLIMBL1SPR, KEENCLIMBR1SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, CK_KeenPullThink1, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPull2};
CK_action CK_ACT_keenPull2 = {KEENCLIMBL2SPR, KEENCLIMBR2SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, CK_KeenPullThink2, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPull3};
CK_action CK_ACT_keenPull3 = {KEENCLIMBL3SPR, KEENCLIMBR3SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, CK_KeenPullThink3, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPull4};
CK_action CK_ACT_keenPull4 = {KEENCLIMBL4SPR, KEENCLIMBR4SPR, AT_UnscaledOnce, 0, 0, 10, 0, 0, CK_KeenPullThink4, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPull5};
CK_action CK_ACT_keenPull5 = {KEENSTANDLSPR, KEENSTANDRSPR, AT_UnscaledOnce, 0, 0, 6, 0, 0, NULL, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenStanding};

CK_action CK_ACT_keenPoleSit = {KEENPOLECLIMBL1SPR, KEENPOLECLIMBR1SPR, AT_Frame, 0, 0, 0, 0, 0, CK_KeenPoleSitThink, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPoleSit};
CK_action CK_ACT_keenPoleUp1 = {KEENPOLECLIMBL1SPR, KEENPOLECLIMBR1SPR, AT_ScaledFrame, 0, 0, 8, 0, 8, CK_KeenPoleUpThink, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPoleUp2};
CK_action CK_ACT_keenPoleUp2 = {KEENPOLECLIMBL2SPR, KEENPOLECLIMBR2SPR, AT_ScaledFrame, 0, 0, 8, 0, 8, CK_KeenPoleUpThink, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPoleUp3};
CK_action CK_ACT_keenPoleUp3 = {KEENPOLECLIMBL3SPR, KEENPOLECLIMBR3SPR, AT_ScaledFrame, 0, 0, 8, 0, 8, CK_KeenPoleUpThink, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPoleUp1};
CK_action CK_ACT_keenPoleDown1 = {KEENPOLESLIDE1SPR, KEENPOLESLIDE1SPR, AT_ScaledOnce, 0, 0, 8, 0, 24, CK_KeenPoleDownThink, CK_KeenSpecialColFunc, CK_KeenPoleDownDrawFunc, &CK_ACT_keenPoleDown2};
CK_action CK_ACT_keenPoleDown2 = {KEENPOLESLIDE2SPR, KEENPOLESLIDE2SPR, AT_ScaledOnce, 0, 0, 8, 0, 24, CK_KeenPoleDownThink, CK_KeenSpecialColFunc, CK_KeenPoleDownDrawFunc, &CK_ACT_keenPoleDown3};
CK_action CK_ACT_keenPoleDown3 = {KEENPOLESLIDE3SPR, KEENPOLESLIDE3SPR, AT_ScaledOnce, 0, 0, 8, 0, 24, CK_KeenPoleDownThink, CK_KeenSpecialColFunc, CK_KeenPoleDownDrawFunc, &CK_ACT_keenPoleDown4};
CK_action CK_ACT_keenPoleDown4 = {KEENPOLESLIDE4SPR, KEENPOLESLIDE4SPR, AT_ScaledOnce, 0, 0, 8, 0, 24, CK_KeenPoleDownThink, CK_KeenSpecialColFunc, CK_KeenPoleDownDrawFunc, &CK_ACT_keenPoleDown1};

CK_action CK_ACT_keenPoleShoot0 = {KEENPOLESHOOTLEFTSPR, KEENPOLESHOOTRIGHTSPR, AT_UnscaledFrame, 0, 0, 9, 0, 0, CK_KeenPoleShootThink, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPoleShoot1};
CK_action CK_ACT_keenPoleShoot1 = {KEENPOLESHOOTLEFTSPR, KEENPOLESHOOTRIGHTSPR, AT_UnscaledFrame, 1, 0, 1, 0, 0, CK_KeenSpawnShot, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPoleShoot2};
CK_action CK_ACT_keenPoleShoot2 = {KEENPOLESHOOTLEFTSPR, KEENPOLESHOOTRIGHTSPR, AT_UnscaledFrame, 0, 0, 6, 0, 0, CK_KeenPoleShootThink, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPoleSit};
CK_action CK_ACT_keenPoleShootUp0 = {KEENPOLESHOOTUPLSPR, KEENPOLESHOOTUPRSPR, AT_UnscaledFrame, 0, 0, 9, 0, 0, CK_KeenPoleShootThink, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPoleShootUp1};
CK_action CK_ACT_keenPoleShootUp1 = {KEENPOLESHOOTUPLSPR, KEENPOLESHOOTUPRSPR, AT_UnscaledFrame, 1, 0, 1, 0, 0, CK_KeenSpawnShot, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPoleShootUp2};
CK_action CK_ACT_keenPoleShootUp2 = {KEENPOLESHOOTUPLSPR, KEENPOLESHOOTUPRSPR, AT_UnscaledFrame, 0, 0, 6, 0, 0, CK_KeenPoleShootThink, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPoleSit};
CK_action CK_ACT_keenPoleShootDown0 = {KEENPOLESHOOTDOWNLSPR, KEENPOLESHOOTDOWNRSPR, AT_UnscaledOnce, 0, 0, 9, 0, 0, CK_KeenPoleShootThink, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPoleShootDown1};
CK_action CK_ACT_keenPoleShootDown1 = {KEENPOLESHOOTDOWNLSPR, KEENPOLESHOOTDOWNRSPR, AT_UnscaledOnce, 1, 0, 1, 0, 0, CK_KeenSpawnShot, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPoleShootDown2};
CK_action CK_ACT_keenPoleShootDown2 = {KEENPOLESHOOTDOWNLSPR, KEENPOLESHOOTDOWNRSPR, AT_UnscaledOnce, 0, 0, 6, 0, 0, CK_KeenPoleShootThink, CK_KeenSpecialColFunc, CK_KeenSpecialDrawFunc, &CK_ACT_keenPoleSit};


// Keen Shots
CK_action CK_ACT_keenShot1 = {STUNNER1SPR, STUNNER1SPR, AT_ScaledOnce, 0, 0, 6, 0x40, 0x40, CK_ShotThink, NULL, CK_ShotDrawFunc, &CK_ACT_keenShot2};
CK_action CK_ACT_keenShot2 = {STUNNER2SPR, STUNNER2SPR, AT_ScaledOnce, 0, 0, 6, 0x40, 0x40, CK_ShotThink, NULL, CK_ShotDrawFunc, &CK_ACT_keenShot3};
CK_action CK_ACT_keenShot3 = {STUNNER3SPR, STUNNER3SPR, AT_ScaledOnce, 0, 0, 6, 0x40, 0x40, CK_ShotThink, NULL, CK_ShotDrawFunc, &CK_ACT_keenShot4};
CK_action CK_ACT_keenShot4 = {STUNNER4SPR, STUNNER4SPR, AT_ScaledOnce, 0, 0, 6, 0x40, 0x40, CK_ShotThink, NULL, CK_ShotDrawFunc, &CK_ACT_keenShot1};

CK_action CK_ACT_keenShotHit1 = {STUNNERDIE1SPR, STUNNERDIE1SPR, AT_UnscaledOnce, 0, 0, 0xC, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_keenShotHit2};
CK_action CK_ACT_keenShotHit2 = {STUNNERDIE2SPR, STUNNERDIE2SPR, AT_UnscaledOnce, 0, 0, 0xC, 0, 0, NULL, NULL, CK_BasicDrawFunc1, NULL};

CK_action CK_ACT_keenSlide = {KEENWALKAWAY1SPR, KEENWALKAWAY1SPR, AT_Frame, 0, 0, 0, 0, 0, CK_KeenSlide, NULL, CK_BasicDrawFunc1, NULL};

CK_action CK_ACT_keenPlaceGem = {KEENWALKAWAY1SPR, KEENWALKAWAY1SPR, AT_UnscaledOnce, 0, 1, 6, 0, 0, CK_KeenPlaceGem, NULL, CK_KeenDrawFunc, &CK_ACT_keenPressSwitch2};

CK_action CK_ACT_keenPressSwitch1 = {KEENWALKAWAY1SPR, KEENWALKAWAY1SPR, AT_UnscaledOnce, 0, 1, 8, 0, 0, CK_KeenPressSwitchThink, NULL, CK_KeenDrawFunc, &CK_ACT_keenPressSwitch2};
CK_action CK_ACT_keenPressSwitch2 = {KEENWALKAWAY1SPR, KEENWALKAWAY1SPR, AT_UnscaledOnce, 0, 1, 8, 0, 0, NULL, NULL, CK_KeenDrawFunc, &CK_ACT_keenStanding};

CK_action CK_ACT_keenEnterDoor1 = {KEENWALKAWAY1SPR, KEENWALKAWAY1SPR, AT_UnscaledOnce, 0, 0, 0x2D, 0, -64, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_keenEnterDoor2};
CK_action CK_ACT_keenEnterDoor2 = {KEENWALKAWAY1SPR, KEENWALKAWAY1SPR, AT_UnscaledOnce, 0, 0, 9, 0, -64, CK_KeenEnterDoor0, NULL, CK_BasicDrawFunc1, &CK_ACT_keenEnterDoor3};
CK_action CK_ACT_keenEnterDoor3 = {KEENWALKAWAY2SPR, KEENWALKAWAY2SPR, AT_UnscaledOnce, 0, 0, 9, 0, -64, CK_KeenEnterDoor1, NULL, CK_BasicDrawFunc1, &CK_ACT_keenEnterDoor4};
CK_action CK_ACT_keenEnterDoor4 = {KEENWALKAWAY3SPR, KEENWALKAWAY3SPR, AT_UnscaledOnce, 0, 0, 9, 0, -64, CK_KeenEnterDoor0, NULL, CK_BasicDrawFunc1, &CK_ACT_keenEnterDoor5};
CK_action CK_ACT_keenEnterDoor5 = {KEENWALKAWAY4SPR, KEENWALKAWAY4SPR, AT_UnscaledOnce, 0, 0, 9, 0, -64, CK_KeenEnterDoor1, NULL, CK_BasicDrawFunc1, &CK_ACT_keenEnterDoor6};
CK_action CK_ACT_keenEnterDoor6 = {KEENWALKAWAY5SPR, KEENWALKAWAY5SPR, AT_UnscaledOnce, 0, 0, 9, 0, -64, CK_KeenEnterDoor, NULL, CK_BasicDrawFunc1, &CK_ACT_keenStanding};
CK_action CK_ACT_keenEnteredDoor = {-1, -1, AT_UnscaledOnce, 0, 0, 9, 0, -64, &CK_KeenEnterDoor, NULL, CK_BasicDrawFunc1, &CK_ACT_keenStanding};

CK_action CK_ACT_keenSink = {KEENQUICKSANDSPR, KEENQUICKSANDSPR, AT_Frame, false, push_none, 0, 0, 0, CK_KeenSink, NULL, CK_DreamReact, NULL};

// New Stuff
#if 1
//CK_action CK_ACT_KeenKnockback1 = {KEENKNOCKBACK1SPR, KEENKNOCKBACK1SPR, AT_UnscaledFrame, 0, 0, 8, 0, 0, CK_KeenFall, NULL, CK_KeenKnockbackReact, &CK_ACT_KeenKnockback2};
//CK_action CK_ACT_KeenKnockback2 = {KEENKNOCKBACK2SPR, KEENKNOCKBACK2SPR, AT_UnscaledFrame, 0, 0, 8, 0, 0, CK_KeenFall, NULL, CK_KeenKnockbackReact, &CK_ACT_KeenKnockback1};
//CK_action CK_ACT_KeenKnockbackRecover1 = {KEENKNOCKDOWN1SPR, KEENKNOCKDOWN1SPR, AT_UnscaledOnce, 0, 0, 70, 0, 0, NULL, NULL, CK_BasicDrawFunc1, &CK_ACT_keenStanding};

CK_action CK_ACT_keenStunned1;
CK_action CK_ACT_keenStunned2;
CK_action CK_ACT_keenStunned3;
//CK_action SV_ACT_KeenNull;

CK_action CK_ACT_keenStunned1 = {KEENKNOCKBACK1SPR, KEENKNOCKBACK1SPR, AT_Frame, 0, 0, 0, 0, 0, CK_KeenFall, CK_KeenColFunc, CK_KeenStunnedReact, &CK_ACT_keenStunned3};
CK_action CK_ACT_keenStunned2 = {KEENKNOCKBACK2SPR, KEENKNOCKBACK2SPR, AT_Frame, 0, 0, 0, 0, 0, CK_KeenFall, CK_KeenColFunc, CK_KeenStunnedReact, &CK_ACT_keenStunned3};
CK_action CK_ACT_keenStunned3 = {KEENKNOCKDOWN1SPR, KEENKNOCKDOWN1SPR, AT_UnscaledFrame, 0, 0, 70, 0, 0, CK_KeenStunnedRecover, CK_KeenColFunc, CK_BasicDrawFunc1, &CK_ACT_keenStanding};

//CK_action SV_ACT_KeenNull = {-1, -1, AT_Frame, 0, 0, 0, 0, 0, NULL, NULL, CK_BasicDrawFunc1, NULL};
#endif

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/



typedef enum CK_miscFlag {

	MISCFLAG_POLE = 1,
	MISCFLAG_DOOR = 2,
	MISCFLAG_SWITCHPLATON = 5,
	MISCFLAG_SWITCHPLATOFF = 6,
	MISCFLAG_GEMHOLDER0 = 7,
	MISCFLAG_GEMHOLDER1 = 8,
	MISCFLAG_GEMHOLDER2 = 9,
	MISCFLAG_GEMHOLDER3 = 10,
	MISCFLAG_SWITCHBRIDGE = 15,
	MISCFLAG_SECURITYDOOR = 32,

} CK_miscFlag;



extern CK_object *ck_keenObj;

CK_keenState ck_keenState;

void CK_BasicDrawFunc1(CK_object *obj);

CK_action CK_ACT_itemNotify = {0, 0, AT_ScaledOnce, 0, 0, 40, 0, 8, 0, 0, CK_BasicDrawFunc1, 0};

soundnames CK5_ItemSounds[]  = { GOTGEMSND, GOTGEMSND, GOTGEMSND, GOTGEMSND,
                               GOTITEMSND,GOTITEMSND,GOTITEMSND,GOTITEMSND,GOTITEMSND,GOTITEMSND,
                               GOTEXTRALIFESND, GOTSTUNNERSND,GOTITEMSND 
};

unsigned CK5_ItemPoints[]  = {  0,   0,   0,   0, 100, 200, 500, 1000, 2000, 5000,   0,   0,   10000,   0};
unsigned CK5_ItemShadows[] = {	GEMSHADOWSPR, GEMSHADOWSPR, GEMSHADOWSPR, GEMSHADOWSPR, 
								POINTS100SHADOWSPR, POINTS200SHADOWSPR, POINTS500SHADOWSPR,  POINTS1000SHADOWSPR,  POINTS2000SHADOWSPR,  POINTS5000SHADOWSPR, 
								POINTS1UPSHADOWSPR, STUNNERSHADOWSPR, POINTS10000SHADOWSPR, POINTS2UPSHADOWSPR
							 };


void CK_IncreaseScore(int score);

void CK_KeenColFunc(CK_object *a, CK_object *b)
{
	// K1n9_Duk3 mod: use switch instead of if/else if
	switch (b->type)
	{
	case CT_Item:
		
		if (ck_mapState.nisasi)
			break;	
		if (b->user1 > 12)
			break;
		if (b->user1 == 11 && ck_gameState.numShots >= MAXAMMO)
			break;

		SD_PlaySound(CK5_ItemSounds[b->user1]);

		b->type = CT_Friendly;
		b->zLayer = FOREGROUNDPRIORITY;

		if (b->user1 != 10)
		{
			b->gfxChunk = CK5_ItemShadows[b->user1];
		}

		CK_IncreaseScore(CK5_ItemPoints[b->user1]);

		if (CK5_ItemPoints[b->user1])
		{
			ck_mapState.itemsCollected++;	// K1n9_Duk3 addition
			if (ck_mapState.itemsCollected == ck_mapState.itemsTotal)
			{
				SD_PlaySound(COLLECTEDALLPOINTSSND);
				ck_gameState.numLives++;
			}
		}

		b->yDirection = motion_Up;	// just in case
		
		if (b->user1 < 4)
		{
			ck_mapState.keyGems[b->user1]++;
		}
		else if (b->user1 == 10)
		{
			if (ck_gameState.difficulty == D_Normal)
			{
				ck_gameState.numLives += 2;
				b->gfxChunk = CK5_ItemShadows[b->user1+3];
			}
			else
			{ 
				ck_gameState.numLives++;
				b->gfxChunk = CK5_ItemShadows[b->user1];
			}
			//ck_mapState.livesCollected++;	// K1n9_Duk3 addition
		}
		else if (b->user1 == 11)
		{
			ck_gameState.numShots += (ck_gameState.difficulty == D_Easy)?8:5;
			if (ck_gameState.numShots > MAXAMMO)
				ck_gameState.numShots = MAXAMMO;

			//ck_mapState.ammoCollected++;	// K1n9_Duk3 addition
		}
		/* //ONLY IN KEEN 5!
		else if (b->user1 == 12)
		{
			ck_gameState.securityCard = 1;
		}
		*/
		CK_SetAction2(b, &CK_ACT_itemNotify);
		break;

	case CT_SneakyTree:	// K1n9_Duk3 mod
		if (b->user4)
			break;	// can't land on it while it's grabbed by a Nisasi
	case CT_Platform:
	case CT_LogPlatform:	// K1n9_Duk3 mod
	case CT_Cloud:
		if (!ck_keenState.platform)
			CK_PhysPushY(a,b);
		break;

	case CT_FallBlock:
		CK_ClipToSprite(a, b, true);
		break;

	case CT_Collectible:
		SD_PlaySound(GOODIESSND);
		ck_mapState.collectiblesNew |= b->user1;
		b->type = CT_Friendly;
		b->zLayer = FOREGROUNDPRIORITY;
		CK_SetAction2(b, &CK_ACT_itemNotify);
		break;

	case CT_Lindsey:
		CK_LindseyDialog(b->user4);
		// turn Lindsey into a smoke cloud:
		b->type = CT_LindseyVanish;	// no further interaction with this object
		b->gfxChunk = SMIRKYPOOF1SPR;
		b->user1 = SMIRKYPOOF4SPR;
		b->posX += -8*PIXGLOBAL;
		CK_SetAction2(b, &CK_ACT_Smoke);
		SD_PlaySound(YETICLOUDSND);

		//
		// find the next Lindsey (if it exists) and activate it:
		//
		for (a=ck_keenObj; a; a=a->next)
		{
			if (a->type == CT_LindseyHidden && a->user4 == b->user4+1)
			{
				a->type = CT_Lindsey;
				a->user3 = lasttimecount;
				break;
			}
		}
		break;
			
	case CT_CouncilMemberDone:
	case CT_CouncilMember:
		// show dialog
		if (!b->user4)	// but only if janitor is Nisasi-free
		{
			if (a->gfxChunk == KEENSLEEP3SPR || a->gfxChunk == KEENSLEEP4SPR)
			{
				// Keen is sleeping
				if (!b->user3)
				{
					CK_JanitorSleepMonolog();	// let Janitor mumble something
				}
				b->user3 = 42;	// restart cooldown, so we don't see the message in an infinite loop
			}
			else if (a->gfxChunk >= KEENKNOCKBACK1SPR && a->gfxChunk <= KEENKNOCKDOWN1SPR)
			{
				// Keen is stunned
				if (!b->user3)
				{
					CK_JanitorStunnedMonolog();	// let Janitor mumble something
				}
				b->user3 = 42;	// restart cooldown, so we don't see the message in an infinite loop
			}
			else if (ck_mapState.nisasi)	// Nisaba patch
				{
					// Keen is grabbed by a Nisasi
				if (!b->user3)
					{
						CK_JanitorNisasiDialog(0);
					}
					b->user3 = 42;	// restart cooldown, so we don't see the message in an infinite loop
				}
			else if (b->type == CT_CouncilMember)
			{
			//	if (!b->user3)
				CK_JanitorDialog(b->user2);
				b->type = CT_CouncilMemberDone;	// avoid showing the dialog a second time
			}
		}
		break;
		
	case CT_Efree:
		CK_EfreeGrabKeen(b);
		break;

	case CT_Checkpoint:
		if (ck_mapState.nisasi)
			break;
		if (!b->user1)
		{
			ck_mapState.checkpointTouched = b;	// checkpoint saving is done in CK_PlayLoop()
		}
		break;
	}
}

int ck_KeenRunXVels[8] = {0, 0, 4, 4, 8, -4, -4, -8};

int ck_KeenPoleOffs[3] = {-8, 0, 8};


void CK_IncreaseScore(int score)
{
	ck_gameState.keenScore += score;
//	ck_mapState.pointsCollected += score;	// K1n9_Duk3 addition
	if (DemoMode != demo_Off) return;
	if (ck_gameState.keenScore >= ck_gameState.nextKeenAt)
	{
		SD_PlaySound(GOTEXTRALIFESND);
		if (ck_gameState.difficulty == D_Normal)
			ck_gameState.numLives += 2;
		else 
			ck_gameState.numLives++;
	//	ck_gameState.nextKeenAt *= 2;	

		if (ck_gameState.nextKeenAt == 160000)
		{
			ck_gameState.nextKeenAt = 314000;
		}
		else if (ck_gameState.nextKeenAt == 314000)
		{
			ck_gameState.nextKeenAt = 500000;
		}
		else
		{
			ck_gameState.nextKeenAt = ck_gameState.nextKeenAt * 2;
		}
	}	
}

void CK_SpawnKeen(int tileX, int tileY, int direction)
{
	ck_keenObj->type = CT_Player;
	ck_keenObj->active = OBJ_ALWAYS_ACTIVE; 
	ck_keenObj->visible = true;
	ck_keenObj->zLayer = KEENPRIORITY;
	ck_keenObj->clipped = CLIP_normal;
	ck_keenObj->posX = (tileX << G_T_SHIFT);
	ck_keenObj->posY = (tileY << G_T_SHIFT) - 241;
	ck_keenObj->xDirection = direction;
	ck_keenObj->yDirection = 1;
	CK_SetAction(ck_keenObj, &CK_ACT_keenStanding);
}

static unsigned emptyTile = 0;

void CK_SpawnItemNotify(int tileX, int tileY, int spriteNum)
{
	CK_object *notify;

	notify = CK_GetNewObj(true);
	notify->type = 1;
	notify->zLayer = FOREGROUNDPRIORITY;
	notify->posX = tileX << G_T_SHIFT;
	notify->posY = tileY << G_T_SHIFT;
	notify->yDirection = -1;
	notify->user2 = spriteNum;
	notify->gfxChunk = notify->user2;
	CK_SetAction(notify, &CK_ACT_itemNotify);
	notify->clipped = CLIP_not;
}

void CK_KeenGetTileItem(int tileX, int tileY, int itemNumber)
{
	if (itemNumber == 10)
	{
		if (ck_gameState.difficulty == D_Normal)
		{
			ck_gameState.numLives += 2;
		}
		else 
			ck_gameState.numLives++;
		//ck_mapState.livesCollected++;	// K1n9_Duk3 addition
	}
	else if (itemNumber == 11)
	{
		if (ck_gameState.numShots >= MAXAMMO)
			return;

		ck_gameState.numShots += (ck_gameState.difficulty == D_Easy)?8:5;
		if (ck_gameState.numShots >= MAXAMMO)
			ck_gameState.numShots = MAXAMMO;

		//ck_mapState.ammoCollected++;	// K1n9_Duk3 addition
	}

	RF_MemToMap(&emptyTile, 1, tileX, tileY, 1, 1);
	SD_PlaySound(CK5_ItemSounds[itemNumber]);

	CK_IncreaseScore(CK5_ItemPoints[itemNumber]);

	if (CK5_ItemPoints[itemNumber])
	{
		ck_mapState.itemsCollected++;	// K1n9_Duk3 addition
		if (ck_mapState.itemsCollected == ck_mapState.itemsTotal)
		{
			SD_PlaySound(COLLECTEDALLPOINTSSND);
			ck_gameState.numLives++;
		}
	}
	
	// TODO: Handle more kinds of pick-ups
	
	if (itemNumber == 10 && ck_gameState.difficulty == D_Normal)
		CK_SpawnItemNotify(tileX, tileY, CK5_ItemShadows[itemNumber+3]);
	else
		CK_SpawnItemNotify(tileX, tileY, CK5_ItemShadows[itemNumber]);
}
		
void CK_GetVitalin(int tileX, int tileY)
{
	CK_object *notify = CK_GetNewObj(true);
	notify->type = 1;
	notify->clipped = CLIP_not;
	notify->zLayer = FOREGROUNDPRIORITY;
	notify->posX = tileX << G_T_SHIFT;
	notify->posY = tileY << G_T_SHIFT;
	
	CK_SetAction(notify, &CK_ACT_VitalinNotify1);
}

void CK_KeenGetTileVitalin(int tileX, int tileY)
{
	RF_MemToMap(&emptyTile, 1, tileX, tileY, 1, 1);
	SD_PlaySound(GOTVITALINSND);
	CK_GetVitalin(tileX, tileY);

	//ck_mapState.dropsCollected++;	// K1n9_Duk3 addition

	// Award extra life at 100 vitalins
	if (++ck_gameState.numVitalin == 100)
	{
		CK_object *newobj;

		ck_gameState.numVitalin = 0;
		SD_PlaySound(GOTEXTRALIFEK6SND);
		ck_gameState.numLives++;
		newobj = CK_GetNewObj(true);
		newobj->type = CT_Friendly;
		newobj->zLayer = FOREGROUNDPRIORITY;
		newobj->posX = tileX << G_T_SHIFT;
		newobj->posY = tileY << G_T_SHIFT;
		newobj->yDirection = motion_Up;
		newobj->user2 = newobj->gfxChunk = VITALIN1UPSHADOWSPR;
		CK_SetAction(newobj, &CK_ACT_itemNotify);
		newobj->clipped = CLIP_not;
	}
}

void CK_KeenCheckSpecialTileInfo(CK_object *obj)
{
	int x,y, specialTileInfo, targetXUnit;
	unsigned warpinfo=0, warp=0, quicksand=0, quicksandy;

	// don't collect tiles during special death anmations:
	if (obj->currentAction == &CK_ACT_keenDrown
		|| obj->currentAction == &CK_ACT_keenBurn
		|| obj->currentAction == &CK_ACT_Swallowed
		|| obj->currentAction == &CK_ACT_keenSink)
	{
		goto check_quicksand;
	}

	if (ck_mapState.moontile == 1)
		ck_mapState.moontile = 0;

	ck_mapState.inwater = 0;

	for (y = obj->clipRects.tileY1; y <= obj->clipRects.tileY2; ++y)
	{
		for (x = obj->clipRects.tileX1; x <= obj->clipRects.tileX2; ++x)
		{
			int tilenum = CA_TileAtPos(x,y,1);
			specialTileInfo =  (TI_ForeMisc(tilenum) & 0x7F);
			switch (specialTileInfo)
			{
			case MF_FireHazard:
				if (!CK_KeenIsDead())
					CK_BurnKeen(true);
				else
					CK_KillKeenEx(true);
				break;

			case MF_DeadlyWater:
				if (!CK_KeenIsDead())
				{
					CK_DrownKeen(y);
					return;
				}
				break;
		/*
			case MF_Deadly:
				CK_KillKeenEx(true);
				break;
		*/
			// Nisaba patch: Make deadly tiles slightly less deadly by giving
			// a tolerance margin of 4 pixels to each side.
			case MF_Deadly:
				if (x >= RF_UnitToTile(obj->clipRects.unitX1 + 4*PIXGLOBAL) && x <= RF_UnitToTile(obj->clipRects.unitX2 - 4*PIXGLOBAL))
					CK_KillKeenEx(true);
				break;
		
			case MF_Centilife:
				if (ck_mapState.nisasi)
					break;
				CK_KeenGetTileVitalin(x,y);
				break;

			case MISCFLAG_GEMHOLDER0:
			case MISCFLAG_GEMHOLDER1:
			case MISCFLAG_GEMHOLDER2:
			case MISCFLAG_GEMHOLDER3:
				if (y == obj->clipRects.tileY2 && obj->topTI
					&& obj->currentAction != &CK_ACT_keenPlaceGem
					&& !ck_mapState.nisasi
					&& ck_mapState.keyGems[specialTileInfo - MISCFLAG_GEMHOLDER0])
				{
					targetXUnit = (x << G_T_SHIFT) - 64;
					if (obj->posX == targetXUnit)
					{
						ck_mapState.keyGems[specialTileInfo - MISCFLAG_GEMHOLDER0]--;
						CK_SetAction2(obj, &CK_ACT_keenPlaceGem);
					}
					else
					{
						obj->user1 = targetXUnit;
						obj->currentAction = &CK_ACT_keenSlide;
					}
				}
				return;

			case MF_Moon:
				if (!ck_mapState.moontile)
					ck_mapState.moontile = 1;
				break;

			case MF_Points100:
			case MF_Points200:
			case MF_Points500:
			case MF_Points1000:
			case MF_Points2000:
			case MF_Points5000:
			case MF_1UP:
			case MF_Stunner:
				// check for Slug Slime cans:
				if (ck_mapState.nisasi)
					break;	
				if (tilenum == 0x5C || tilenum == 0x6E)
					ck_mapState.slugcans++;

				CK_KeenGetTileItem(x,y,specialTileInfo - (MF_Points100-4));
				break;
/*
			case MF_3UP:
				ck_gameState.numLives += 3;
				RF_MemToMap(&emptyTile, 1, x, y, 1, 1);
				SD_PlaySound(GOTEXTRALIFESND);
				CK_SpawnItemNotify(x, y, WORLDNESSIE);
				break;
*/
			case MF_Warp:
				if (warpinfo)
				{
					unsigned info = CA_TileAtPos(x, y, 2);
					if (info)
					{
						if (info != warpinfo)
						{
							// Keen is touching two warp tiles with differing destinations
							// so don't warp at all.
							warp = 0;
						}
						warpinfo = info;
					}
				}
				else
				{
					warpinfo = CA_TileAtPos(x, y, 2);
					warp = warpinfo;
				}
				break;

			case MF_MusicTrigger:
				if (!CK_KeenIsDead())
				{
					unsigned info = CA_TileAtPos(x, y, 2);
					if (info >= INFONUMBER_0 && info <= INFONUMBER_MAX)
					{
						info -= INFONUMBER_0;
						if (ck_mapState.music != info && info < LASTMUSIC)
						{
							ck_mapState.music = info;
							StartMusic(info, true);
						}
					}
				}
				break;

			case MF_Quicksand:
				if (!quicksand)
					quicksandy = y;
				quicksand = 1;
				ck_mapState.inwater = 1;	// quicksand also counts as water
				break;

			case MF_Water:
				// Keen must be at least 8 pixels into the water tile 
				// for him to be treated as being in the water:
				if (obj->clipRects.unitY2 >= RF_TileToUnit(y) + 8*PIXGLOBAL)
					ck_mapState.inwater = 1;
				break;
			}
		}
	}

	if (CK_KeenIsDead())
		quicksand = 0;

	// handle warping down here, so Keen can still collect tile items
	// near the warp spot.
	if (warp)
	{
		ck_mapState.warpInfo = warpinfo;
		// The actual warping must be handled between the refresh and
		// updating the objects, otherwise the objects around the new
		// position might be invisible for one frame.
	}

check_quicksand:
	// spawn or remove quicksand platform:
	if (quicksand && !ck_gameState.godMode)
	{
		if (quicksandy < obj->clipRects.tileY2)
		{
			// Keen's feet have sunken below the topmost quicksand tile
			// so Keen Must Die!

			RemoveDreamBubbles();	// just in case...
			CK_DropNisasi();

			CK_SetAction2(obj, &CK_ACT_keenSink);
		// 	Nisaba patch:
		//	obj->user1 = 0;
			obj->user2 = 0;
			obj->clipped = CLIP_not;
			ck_scrollDisabled = true;
		}
		else if (ck_mapState.quicksand)
		{
			// move quicksand object along with Keen:
			CK_PhysUpdateX(ck_mapState.quicksand, obj->posX - ck_mapState.quicksand->posX);
			CK_PhysUpdateY(ck_mapState.quicksand, obj->clipRects.unitY2+PIXGLOBAL - ck_mapState.quicksand->clipRects.unitY1);
		}
		else
		{
			unsigned sandtop = RF_TileToUnit(quicksandy)+PIXGLOBAL;
			// turn off pogo:
			if (obj->currentAction->draw == CK_KeenPogoDrawFunc)
			{
				sandtop += 8*PIXGLOBAL;	// pogo landing makes Keen sink 8 pixels into the quicksand
				obj->posY += sandtop - obj->clipRects.unitY2;
				CK_SetAction2(obj, &CK_ACT_keenFall2);
			}

			// spawn quicksand:
			CK_SpawnQuicksand(obj->posX, sandtop);
		}
	}
	else if (ck_mapState.quicksand)
	{
		// remove the quicksand object:
		if (ck_keenState.platform == ck_mapState.quicksand)
			ck_keenState.platform = NULL;
		CK_RemoveObj(ck_mapState.quicksand);
		ck_mapState.quicksand = NULL;
	}
}

// K1n9_Duk3 addition (so doors can also be opened by switches):
void CK_OpenGemDoor(int targetX, int targetY)
{
	CK_object *newObj;
	int doorHeight, doorY;
	unsigned doorTile;

	newObj = CK_GetNewObj(false);

	newObj->posX = targetX;
	newObj->posY = targetY;

	if (targetX > mapwidth || targetX < 2 || targetY > mapheight || targetY < 2)
	{
		//Quit("Keyholder points to a bad spot!");
		SM_Quit(S_BADKEYHOLDERSPOT);
	}

	// Calculate the height of the door.
	doorHeight = 0;
	doorY = targetY;
	doorTile = CA_TileAtPos(targetX, doorY, 1);
	do
	{
		doorHeight++;
		doorY++;
	} while ((TI_ForeMisc(CA_TileAtPos(targetX, doorY, 1)) & 0x7F) == MF_GemDoor);

	newObj->user1 = doorHeight;
	newObj->user2 = TI_ForeAnimTile(doorTile)? 1 : -1;	// 1 = open door, -1 = close door
	newObj->clipped = CLIP_not;
	newObj->type = CT_Friendly;
	newObj->active = OBJ_ALWAYS_ACTIVE;
	CK_SetAction(newObj, &CK_ACT_DoorOpen1);
}

// K1n9_Duk3 addition:
void CK_OpenDoubleDoor(int targetX, int targetY)
{
	CK_object *newObj;
	int doorHeight, doorY;
	unsigned doorType;

	if (targetX > mapwidth || targetX < 2 || targetY > mapheight || targetY < 2)
	{
		//Quit("Keyholder points to a bad spot!");
		SM_Quit(S_BADKEYHOLDERSPOT);
	}

	// Calculate the height of the door.
	doorHeight = 0;
	doorY = targetY;
	doorType = TI_ForeMisc(CA_TileAtPos(targetX, doorY, 1)) & 0x7F;
	do
	{
		doorHeight++;
		doorY++;
	} while ((TI_ForeMisc(CA_TileAtPos(targetX, doorY, 1)) & 0x7F) == doorType);

	for (doorY = 0; doorY < 2; doorY++)
	{
		newObj = CK_GetNewObj(false);
		newObj->posX = targetX;
		newObj->posY = targetY;
		newObj->user1 = doorHeight;
		newObj->user2 = 1;	// 1 = open door, -1 = close door
		newObj->clipped = CLIP_not;
		newObj->type = CT_Friendly;
		newObj->active = OBJ_ALWAYS_ACTIVE;
		CK_SetAction(newObj, (doorType == MF_DoubleDoor1)? &CK_ACT_DoorOpen3 : &CK_ACT_DoorOpen2);

		targetX++;
	}
}

void CK_ParseToggleStack(int tileX, int tileY)
{
	static int oppositearrow[8] = {2,3,0,1,6,7,4,5};

	unsigned targetX, targetY, x, y;
	unsigned info, offset, targetOff, tile;

	//unsigned top, mid, bot, far *map;	// zapper variables

	offset = mapbwidthtable[tileY]/2 + tileX;

	while ((info = mapsegs[2][offset]) != 0 && (unsigned)tileY < mapheight)
	{
		offset += mapwidth;	// += means move down, -= means move up
		tileY++;	// ++ means move down, -- means move up

		targetX = (info >> 8);
		targetY = (info & 0xFF);

		// safety check (could be omitted):
		if (targetX < 2 || targetX > mapwidth || targetY < 2 || targetY > mapheight)
			break;

		targetOff = mapbwidthtable[targetY]/2 + targetX;

		switch (TI_ForeMisc(mapsegs[1][targetOff]) & 0x7F)
		{
		case MF_GemDoor:
			CK_OpenGemDoor(targetX, targetY);
			break;

		case MF_DoubleDoor1:
		case MF_DoubleDoor2:
			CK_OpenDoubleDoor(targetX, targetY);
			break;

		case MF_CloneMachine:
			CK_ToggleCloneMachine(targetX, targetY);
			break;
/*
		case MF_ZapperOn:
			map = mapsegs[1];
			top = *map;
			mid = *++map;
			bot = *++map;
			map = mapsegs[1] + targetOff + mapwidth;

			RF_MemToMap(&top, 1, targetX, targetY++, 1, 1);
			while (TI_ForeMisc(*map) == MF_Deadly)
			{
				RF_MemToMap(&mid, 1, targetX, targetY++, 1, 1);
				map += mapwidth;
			}
			RF_MemToMap(&bot, 1, targetX, targetY, 1, 1);
			break;

		case MF_ZapperOff:
			map = mapsegs[1] + 3;
			top = *map;
			mid = *++map;
			bot = *++map;
			map = mapsegs[1] + targetOff + mapwidth;

			RF_MemToMap(&top, 1, targetX, targetY++, 1, 1);
			while (TI_ForeMisc(*map) != MF_ZapperOff)
			{
				RF_MemToMap(&mid, 1, targetX, targetY++, 1, 1);
				map += mapwidth;
			}
			RF_MemToMap(&bot, 1, targetX, targetY, 1, 1);
			break;
*/

		default:
			if (!TI_ForeAnimTile(mapsegs[1][targetOff]))
			{
				info = mapsegs[2][targetOff];
				if (!info)
				{
					mapsegs[2][targetOff] = PLATFORMBLOCK;
				}
				else if (info >= DIRARROWSTART && info < DIRARROWEND)
				{
					mapsegs[2][targetOff] = oppositearrow[info-DIRARROWSTART] + DIRARROWSTART;
				}
				else
				{
					mapsegs[2][targetOff] = 0;
				}
				break;
			}
			// no break here! dropping through to bridge behavior is intended
		case MF_Bridge:
			for (y = 0; y < 2; y++,targetY++)
			{
				for (x = targetX - y,targetOff = mapbwidthtable[targetY]/2 + x; x < mapwidth; x++, targetOff++)
				{
					{
						register int anim;

						tile = mapsegs[1][targetOff];
						anim = TI_ForeAnimTile(tile);
						if (!anim)
							break;
						tile += anim;
					}
					RF_MemToMap(&tile, 1, x, targetY, 1, 1);
				}
			}
		}
	}
}

void CK_ToggleSwitch(int switchTileX, int switchTileY)
{
	/*
	int tileY, tileX;
	unsigned currentTile, newTile;
	static int infoPlaneInverses[8] = {2,3,0,1,6,7,4,5};	// K1n9_Duk3 mod: added 'static'
	unsigned infoPlaneValue;
	unsigned switchTarget = CA_TileAtPos(switchTileX, switchTileY, 2);
	int switchTargetX = (switchTarget >> 8);
	int switchTargetY = (switchTarget & 0xFF);
	*/
	unsigned switchTile = CA_TileAtPos(switchTileX, switchTileY, 1);
	//char switchMisc = TI_ForeMisc(switchTile);

	// Toggle the switch.
	unsigned switchNextTile = switchTile + TI_ForeAnimTile(switchTile);
	RF_MemToMap(&switchNextTile, 1, switchTileX, switchTileY, 1, 1);
	SD_PlaySound(CLICKSND);

	CK_ParseToggleStack(switchTileX, switchTileY);

	/*
	switch (switchMisc)
	{
	case MF_BridgeSwitch:
		for (tileY = switchTargetY; tileY < switchTargetY + 2; ++tileY)
		{
			for (tileX = switchTargetX - ((tileY == switchTargetY)? 0 : 1); tileX < mapwidth; ++tileX)
			{
				currentTile = CA_TileAtPos(tileX, tileY, 1);
				if (!TI_ForeAnimTile(currentTile)) break;
				newTile = currentTile + TI_ForeAnimTile(currentTile);
				RF_MemToMap(&newTile, 1, tileX, tileY, 1, 1);
			} 
		}
		break;
	case MF_SwitchOff:
	case MF_SwitchOn:
		infoPlaneValue = CA_TileAtPos(switchTargetX, switchTargetY, 2);
		if (infoPlaneValue >= DIRARROWSTART && infoPlaneValue < DIRARROWEND)
		{
			// Invert the direction of the goplat arrow.
			infoPlaneValue = infoPlaneInverses[infoPlaneValue - DIRARROWSTART] + DIRARROWSTART;
		}
		else
		{
			// Insert or remove a [B] block.
			infoPlaneValue ^= PLATFORMBLOCK;
		}
		
		*(mapsegs[2] + (switchTargetY*mapwidth + switchTargetX)) = infoPlaneValue;
		break;
	}
	*/
}

void CK_KeenPressSwitchThink(CK_object *obj)
{
	CK_ToggleSwitch(obj->clipRects.tileXmid, obj->clipRects.tileY1);
}

void CK_KeenUseAmmoStation(CK_object *obj)
{
	unsigned limit = CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY1, 2);
	if (limit >= INFONUMBER_0 && limit <= INFONUMBER_MAX)
	{
		limit -= INFONUMBER_0;
		if (limit > MAXAMMO)
			limit = MAXAMMO;
	}
	else
	{
		limit = DEFAULT_SHOTS;
	}
	// give ammo:
	if (ck_gameState.numShots < limit)
	{
		ck_gameState.numShots++;

		if (ck_gameState.numShots == limit)
		{
			obj->currentAction = &CK_ACT_keenPressSwitch2;
			SD_PlaySound(GOTSTUNNERSND);
		}
		else
		{
			SD_PlaySound(BALLBOUNCESND);
		}
	}
	else
	{
		SD_PlaySound(REFILLUNITSND);
		obj->currentAction = &CK_ACT_keenPressSwitch2;
	}
}

CK_action CK_ACT_keenUseAmmoStation = {KEENWALKAWAY1SPR, KEENWALKAWAY1SPR, AT_UnscaledOnce, false, push_down, 8, 0, 0, CK_KeenUseAmmoStation, NULL, CK_KeenDrawFunc, &CK_ACT_keenUseAmmoStation};

boolean CK_KeenPressUp(CK_object *obj)
{
	CK_object *newObj;
	int destXunit;

	unsigned destUnitX;
	char miscFlagLeft;


	char tileMiscFlag = TI_ForeMisc(CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY1, 1)) & 0x7F;

	// Are we pressing a switch? And is the door still aktive?
	if (tileMiscFlag == MISCFLAG_SWITCHPLATON || tileMiscFlag == MISCFLAG_SWITCHPLATOFF || tileMiscFlag == MF_BridgeSwitch)
	{
		destXunit = (obj->clipRects.tileXmid << G_T_SHIFT) - 4*PIXGLOBAL;
		if (obj->posX == destXunit && !CK_DoorsActive())
		{
			// Flip that switch!
			obj->currentAction = &CK_ACT_keenPressSwitch1;
		}
		else
		{
			obj->user1 = destXunit;
			obj->currentAction = &CK_ACT_keenSlide;
		}
		ck_keenState.keenSliding = true;
		return true;
	}

	// K1n9_Duk3 addition:
	if (tileMiscFlag == MF_MindMachine)
	{
		CK_object *page;

		page = CK_FindPageObj(CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY1, 2));
		if (page)
		{
			destXunit = RF_TileToUnit(obj->clipRects.tileXmid) - 12*PIXGLOBAL;
			if (tileMiscFlag != (TI_ForeMisc(CA_TileAtPos(obj->clipRects.tileXmid - 1, obj->clipRects.tileY1, 1)) & 0x7F))
			{
				destXunit += TILEGLOBAL;
			}
			if (obj->posX == destXunit)
			{
				if (tileMiscFlag == (TI_ForeMisc(CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY2, 1)) & 0x7F))
				{
					// Keen's feet are inside the machine, don't walk up
					obj->currentAction = &CK_ACT_keenMindcontrol;
					obj->user2 = (int)CK_SpawnSlideDoor(obj->clipRects.tileX1, obj->clipRects.tileY2, 1);
				}
				else
				{
					// Keen's feet are NOT in the machine, so walk up
					obj->currentAction = &CK_ACT_keenEnterMachine;
				}
				obj->user1 = 0;
				obj->user3 = (int)page;
			}
			else
			{
				obj->user1 = destXunit;
				obj->currentAction = &CK_ACT_keenSlide;
			}
			ck_keenState.keenSliding = true;
			return true;
		}
	}

	if (tileMiscFlag == MF_AmmoStation)
	{
		destXunit = (obj->clipRects.tileXmid << G_T_SHIFT) - 4*PIXGLOBAL;
		if (obj->posX == destXunit)
		{
			obj->currentAction = &CK_ACT_keenUseAmmoStation;
		}
		else
		{
			obj->user1 = destXunit;
			obj->currentAction = &CK_ACT_keenSlide;
		}
		ck_keenState.keenSliding = true;
		return true;
	}
	// end of addition
	
	if (tileMiscFlag == MF_HAL)		// Nisaba patch
	{
		destXunit = (obj->clipRects.tileXmid << G_T_SHIFT) - 4*PIXGLOBAL;
		
		if (obj->posX == destXunit && obj->gfxChunk == KEENWALKAWAY1SPR)
		{
			obj->currentAction = &CK_ACT_keenPressSwitch2;
#if 0
			if (tileMiscFlag == MF_HAL)
			{
				CK_HALDialog();
			}
			else
			{
				RunSnailas();
			}
#else
			CK_MiscDialog(CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY1, 2)-INFONUMBER_0);
#endif
		}
		else
		{
			obj->user1 = destXunit;
			obj->currentAction = &CK_ACT_keenSlide;
		}
		ck_keenState.keenSliding = true;
		return true;
	}
	
/*	if (tileMiscFlag == MF_HAL)		// Nisaba patch
	{
		destXunit = (obj->clipRects.tileXmid << G_T_SHIFT) - 4*PIXGLOBAL;
		
		if (obj->posX == destXunit)
		{
			obj->currentAction = &CK_ACT_keenPressSwitch2;
			CK_HALDialog();
		}
		else
		{
			obj->user1 = destXunit;
			obj->currentAction = &CK_ACT_keenSlide;
		}
		ck_keenState.keenSliding = true;
		return true;
	}
		
	if (tileMiscFlag == MF_Snailas)		// Nisaba patch
	{
		destXunit = (obj->clipRects.tileXmid << G_T_SHIFT) - 4*PIXGLOBAL;
		
		if (obj->posX == destXunit)
		{
			obj->currentAction = &CK_ACT_keenPressSwitch2;
			RunSnailas();
		}
		else
		{
			obj->user1 = destXunit;
			obj->currentAction = &CK_ACT_keenSlide;
		}
		ck_keenState.keenSliding = true;
		return true;
	}	
		
	if (tileMiscFlag == MF_Foobs)		// Nisaba patch
	{
		destXunit = (obj->clipRects.tileXmid << G_T_SHIFT) - 4*PIXGLOBAL;
		
		if (obj->posX == destXunit)
		{
			obj->currentAction = &CK_ACT_keenPressSwitch2;
			//RunFoobs();
		}
		else
		{
			obj->user1 = destXunit;
			obj->currentAction = &CK_ACT_keenSlide;
		}
		ck_keenState.keenSliding = true;
		return true;
	}
	*/
	
	// Are we enterting a door?
	if (tileMiscFlag == MISCFLAG_DOOR || tileMiscFlag == MISCFLAG_SECURITYDOOR || tileMiscFlag == MF_QuickDoor)
	{
		destUnitX = (obj->clipRects.tileXmid << G_T_SHIFT) + 4*PIXGLOBAL;

		// If the door is two tiles wide, we want to be in the centre.
		miscFlagLeft = TI_ForeMisc(CA_TileAtPos(obj->clipRects.tileXmid - 1, obj->clipRects.tileY1, 1)) & 0x7F;
		//if (miscFlagLeft == MISCFLAG_DOOR || miscFlagLeft == MISCFLAG_SECURITYDOOR)
		if (miscFlagLeft == tileMiscFlag)	// K1n9_Duk3 mod (smaller and faster)
		{
			destUnitX -= 256;
		}

		if (obj->posX == destUnitX)
		{
			//We're at the door.
			
			// Is it a special door?
			if (tileMiscFlag == MF_QuickDoor)
			{
				// instant teleport to destination:
				CK_SetLoadingTransition();	// fade out (and automatically fade in later)
				obj->currentAction = &CK_ACT_keenEnterDoor6;
				CK_KeenTeleport(obj, CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY1, 2));
			}
			else if (tileMiscFlag == MISCFLAG_SECURITYDOOR)
			{
				/*
				if (ck_gameState.securityCard)
				{
					ck_gameState.securityCard = 0;
					SD_PlaySound(OPENSECURITYDOORSND);
					newObj = CK_GetNewObj(false);
					newObj->posX = obj->clipRects.tileXmid - 2;
					newObj->posY = obj->clipRects.tileY2 - 4;
					newObj->active = OBJ_ALWAYS_ACTIVE;
					newObj->type = 1;
					CK_SetAction(newObj, &CK_ACT_SecurityDoorOpen);
					obj->currentAction = &CK_ACT_keenEnterDoor1;
					obj->yDirection = motion_Down;	// K1n9_Duk3 fix: avoid door glitch
					obj->zLayer = 0;
					ck_keenState.keenSliding = true;
					return true;
				}
				else
				*/
				{
					SD_PlaySound(NOWAYSND);
					obj->currentAction = &CK_ACT_keenStanding;
					ck_keenState.keenSliding = true;
					return false;
				}
			}
			else
			{
				ck_mapState.invincibilityTimer = 110;
				obj->yDirection = motion_Down;	// K1n9_Duk3 fix: avoid door glitch
				obj->currentAction = &CK_ACT_keenEnterDoor2;
				obj->zLayer = 0;

#if 0
				if (!CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY1, 2))
				{
					CK5_SpawnLightning();
				}
#endif
			}
		}
		else
		{
			obj->user1 = destUnitX;
			obj->currentAction = &CK_ACT_keenSlide;
		}

		ck_keenState.keenSliding = true;

		return true;
	}


	// No? Return to our caller, who will handle poles/looking up.
	return false;
}
	
// Think function for keen "sliding" towards a switch, keygem or door.
void CK_KeenSlide(CK_object *obj)
{
	int deltaX = obj->user1 - obj->posX;
	if (deltaX < 0)
	{
		// Move left one px per tick.
		ck_nextX -= tics * 16;
		// If we're not at our target yet, return.
		if (ck_nextX > deltaX) return;
	}
	else if (deltaX > 0)
	{
		// Move right one px per tick.
		ck_nextX += tics * 16;
		// If we're not at our target yet, return.
		if (ck_nextX < deltaX) return;
	}

	// We're at our target.
	ck_nextX = deltaX;
	obj->user1 = 0;
	if (!CK_KeenPressUp(obj))
	{
		obj->currentAction = &CK_ACT_keenStanding;
	}
}

void CK_KeenEnterDoor0(CK_object *obj)
{
	SD_PlaySound(KEENWALK1SND);
}

void CK_KeenEnterDoor1(CK_object *obj)
{
	SD_PlaySound(KEENWALK2SND);
}

void CK_KeenTeleport(CK_object *obj, unsigned destination)
{
	obj->posY = ((destination&0xFF) << G_T_SHIFT) - 256 + 15;
	obj->posX = ((destination >> 8) << G_T_SHIFT);
	obj->zLayer = KEENPRIORITY;
	obj->clipped = CLIP_not;
	CK_SetAction2(obj, obj->currentAction->next);
	obj->clipped = CLIP_normal;
	CK_CentreCamera(obj);
}

// Think function for entering a door.
void CK_KeenEnterDoor(CK_object *obj)
{
	unsigned destination = CA_TileAtPos(obj->clipRects.tileX1, obj->clipRects.tileY2, 2);

	if (destination == 0x0000)
	{
		ck_levelState = 13;	// TODO: use enum name instead of number
		obj->currentAction = &CK_ACT_keenEnteredDoor;
		return;
	}

	if (destination == 0xB1B1)
	{
		ck_levelState = LS_LevelComplete;	// K1n9_Duk3 fix: use enum name instead of number
		obj->currentAction = &CK_ACT_keenEnteredDoor;
		return;
	}

	CK_KeenTeleport(obj, destination);
}

void CK_KeenPlaceGem(CK_object *obj)
{
	unsigned oldFGTile = CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY2, 1);
	unsigned newFGTile = oldFGTile + 18;
	unsigned target = CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY2, 2);

	unsigned targetX = target >> 8;
	unsigned targetY = target & 0xFF;

	RF_MemToMap(&newFGTile, 1, obj->clipRects.tileXmid, obj->clipRects.tileY2, 1, 1);

	SD_PlaySound(OPENGEMDOORSND);
	
	switch (TI_ForeMisc(CA_TileAtPos(targetX, targetY, 1)) & 0x7F)
	{
	case MF_DoubleDoor1:
	case MF_DoubleDoor2:
		CK_OpenDoubleDoor(targetX, targetY);
		break;

	default:
		CK_OpenGemDoor(targetX, targetY);
		break;
	}
}

void CK_KeenRidePlatform(CK_object *obj)
{
	// Save the platform pointer, we might be wiping it.
	CK_object *plat = ck_keenState.platform;
	int deltax = obj->deltaPosX;	// K1n9_Duk3 addition


	if (obj->clipRects.unitX2 < plat->clipRects.unitX1 || obj->clipRects.unitX1 > plat->clipRects.unitX2)
	{
		// We've fallen off the platform horizontally.
		ck_keenState.platform = NULL;
		return;
	}

	if (obj->deltaPosY < 0)
	{
		// If we've jumped off the platform.
		ck_keenState.platform = NULL;
		if (plat->deltaPosY < 0)
		{
			ck_nextX = 0;
			ck_nextY = plat->deltaPosY;
			CK_PhysUpdateSimpleObj(obj);
			return;
		}
	}
	else
	{
		//Ride the platform
		ck_nextX = plat->deltaPosX;
		ck_nextY = plat->clipRects.unitY1 - obj->clipRects.unitY2 - 16;
		CK_PhysUpdateSimpleObj(obj);

		if (obj->clipRects.unitY2 != plat->clipRects.unitY1 - 16)
		{
			// blocking tiles prevent Keen from being moved to the top of
			// the platform, meaning we're no longer standing on it
			ck_keenState.platform = NULL;
			return;
		}

		// Keen has a "/NOPAN" parameter which disables use of the EGA panning
		// register. In order to make platforms less ugly when this is on
		// (though not much less ugly, IMHO), we would do some more processing here.
		// As is, we just do the line below, to keep keen at the same position while scrolling.	

#ifdef NEWMODS
		// K1n9_Duk3 fix:
		// Make sure Keen and the platform have the same "subpixel" position
		// so that the sprites don't move back and forth in relation to each
		// other when moving sideways. But only if the platform moved sideways
		// and Keen didn't (before the platform moved him).
		if (deltax == 0 && plat->deltaPosX != 0)
		{
			obj->posX &= ~0xF;
			obj->posX |= plat->posX & 0xF;
		}

#else
		// Keen's x position should move with 2px granularity when on a platform so that scrolling
		// looks nice. (Keen will stay at the same pixel position when scrolling, as we scroll 2px at a time).
		obj->posX |= plat->posX & 0x1F;
#endif

		// We've hit the ceiling?
		if (obj->bottomTI)
		{
			ck_keenState.platform = NULL;
			return;
		}


		// We're standing on something, don't fall down!
		obj->topTI = 0x19;
	}
}


boolean CK_KeenTryClimbPole(CK_object *obj)
{
	unsigned candidateTile;

	if (lasttimecount < ck_keenState.poleGrabTime)
		ck_keenState.poleGrabTime = 0;
	else if (lasttimecount - ck_keenState.poleGrabTime < 19)
		return false;

	candidateTile = CA_TileAtPos(obj->clipRects.tileXmid, ((ck_inputFrame.yaxis==-1)?((obj->clipRects.unitY1+96)>>G_T_SHIFT):(obj->clipRects.tileY2+1)), 1);


	if ((TI_ForeMisc(candidateTile) & 0x7F) == 1)
	{
		obj->posX = 128 + ((obj->clipRects.tileXmid - 1) << G_T_SHIFT);
		ck_nextX = 0;
		ck_nextY = (ck_inputFrame.yaxis << 5);
		obj->clipped = CLIP_not;
		obj->currentAction = &CK_ACT_keenPoleSit;
		ck_keenState.platform = NULL;	// K1n9_Duk3 addition
		return true;
	}
	return false;
}

void CK_KeenRunningThink(CK_object *obj)
{
#if 0	// K1n9_Duk3 mod: speed adjustment code has been moved into CK_RunAction()
	// K1n9_Duk3 addition:
	if (ck_mapState.nisasi || ck_mapState.inwater)
		ck_nextX >>= 1;	// HACK: run at half the speed
	// end of addition
#endif

	if (!ck_inputFrame.xaxis)
	{
		obj->currentAction = &CK_ACT_keenStanding;
		CK_HandleInputOnGround(obj);
		return;
	}

	obj->xDirection = ck_inputFrame.xaxis;
	
	if (ck_inputFrame.yaxis == -1)
	{
		if (CK_KeenTryClimbPole(obj)) return;
		if (ck_keenState.keenSliding || CK_KeenPressUp(obj))
			return;
	}
	else if (ck_inputFrame.yaxis == 1)
	{
		if (CK_KeenTryClimbPole(obj)) return;
	}

	if (ck_keenState.shootIsPressed && !ck_keenState.shootWasPressed)
	{
		ck_keenState.shootWasPressed = true;
		
		if (ck_inputFrame.yaxis == -1)
		{
			obj->currentAction = &CK_ACT_keenShootUp1;
		}
		else
		{
			obj->currentAction = &CK_ACT_keenShoot1;
		}
		return;
	}

	if (ck_keenState.jumpIsPressed && !ck_keenState.jumpWasPressed)
	{
		ck_keenState.jumpWasPressed = true;
		SD_PlaySound(KEENJUMPSND);
		obj->velX = obj->xDirection * 16;
		obj->velY = -40;
		ck_nextX = ck_nextY = 0;
		ck_keenState.jumpTimer = 18;
		obj->currentAction = &CK_ACT_keenJump1;
		// Is this the mystical 'impossible pogo'?
		// k5disasm has a 'return' here, probably a mistake?
	}

	if (ck_keenState.pogoIsPressed && !ck_keenState.pogoWasPressed)
	{
		ck_keenState.pogoWasPressed = true;
		obj->currentAction = &CK_ACT_keenPogo1;
		SD_PlaySound(KEENJUMPSND);
		obj->velX = obj->xDirection * 16;
		obj->velY = -48;
		// Should this be nextY? Andy seems to think so, but lemm thinks that X is right...
		ck_nextX = 0;
		ck_keenState.jumpTimer = ck_mapState.inwater? 0 : 24;
		return;
	}

#if 0	// K1n9_Duk3 mod: speed adjustment code has been moved into CK_RunAction()
	// Andy seems to think this is Y as well. Need to do some more disasm.
	// If this is an X vel, then surely we'd want to multiply it by the direction?
	// TODO: investigate

	// While the Keen 5 Disassembly does set ck_nextX here, it would make more sense if ck_nextY 
	// were being set.
#if 0
	ck_nextX += ck_KeenRunXVels[obj->topTI&7] * tics;
#else
	// FITF version:
	{
		register int slopeadjust;

		slopeadjust = ck_KeenRunXVels[obj->topTI & SLOPEMASK] * tics;
		if (ck_mapState.nisasi || ck_mapState.inwater)
			slopeadjust >>= 1;	// HACK: move at half the speed
		ck_nextX += slopeadjust;
	}
#endif
#else
	// K1n9_Duk3 addition: let CK_RunAction() know that ck_nextX has to be adjusted:
	ck_keenState.mustAdjustNextX = true;
#endif

	if ((obj->currentAction->chunkLeft == CK_ACT_keenRun1.chunkLeft) && !(obj->user3))
	{
		SD_PlaySound(KEENWALK1SND);
		obj->user3 = 1;
	}
	else if ((obj->currentAction->chunkLeft == CK_ACT_keenRun3.chunkLeft) && !(obj->user3))
	{
		SD_PlaySound(KEENWALK2SND);
		obj->user3 = 1;
	}
	else if ((obj->currentAction->chunkLeft == CK_ACT_keenRun2.chunkLeft) || (obj->currentAction->chunkLeft == CK_ACT_keenRun4.chunkLeft))
	{
		obj->user3 = 0;
	}
}

void CK_HandleInputOnGround(CK_object *obj)
{
#ifdef KEEN5
	// If we're riding a platform, do it surfin' style!)
	if (obj->topTI == 0x19)
	{
		// But only if such an action exists in this episode. :/
		if (&CK_ACT_keenRidePlatform)
		{
			obj->currentAction = &CK_ACT_keenRidePlatform;
		}
	}
#endif

	if (ck_inputFrame.xaxis)
	{
		obj->currentAction = &CK_ACT_keenRun1;
		CK_KeenRunningThink(obj);
		
		// K1n9_Duk3 note: I *think* this was supposed to make Keen move a little
		// slower than normal when he changes from standing still to walking. But
		// it ends up doing the opposite in some (rare) cases.
		//
		// Let's assume tics is 5 and Keen was in the standing state. The standing
		// state has a tic time of 4, so depending on how many tics have already
		// accumulated while Keen was standing, the state machine code might have
		// used only 1 to 4 out of the 5 tics. The remaining tics will be spent in
		// Keen's walking state(s), which will add additional movement (24 units
		// per tic). If Keen is walking normally (on flat ground), he will move
		// up to 5*24 = 120 units in 5 tics. But if Keen just started walking and
		// there are 4 tics left after this routine is executed, Keen will move
		// (24*5)/4 + 4*24 = 30 + 96 = 126 units! 
		ck_nextX = (int)(obj->xDirection * obj->currentAction->velX * tics)/4;
		
		// K1n9_Duk3 addition: don't modify this ck_nextX value in CK_RunAction():
		ck_keenState.mustAdjustNextX = false;
		return;
	}

	if (ck_keenState.shootIsPressed && !ck_keenState.shootWasPressed)
	{
		ck_keenState.shootWasPressed = true;
		if (ck_inputFrame.yaxis == -1)
		{
			obj->currentAction = &CK_ACT_keenShootUp1;
		}
		else
		{
			obj->currentAction = &CK_ACT_keenShoot1;
		}
		return;
	}

	if (ck_keenState.jumpIsPressed && !ck_keenState.jumpWasPressed)
	{
		ck_keenState.jumpWasPressed = true;
		SD_PlaySound(KEENJUMPSND);
		obj->velX = 0;
		obj->velY = -40;
		ck_nextY = 0;
		ck_keenState.jumpTimer = 18;
		obj->currentAction = &CK_ACT_keenJump1;
		return;
	}

	if (ck_keenState.pogoIsPressed && !ck_keenState.pogoWasPressed)
	{
		ck_keenState.pogoWasPressed = true;
		SD_PlaySound(KEENJUMPSND);
		obj->currentAction = &CK_ACT_keenPogo1;
		obj->velX = 0;
		obj->velY = -48;
		ck_nextY = 0;
		ck_keenState.jumpTimer = ck_mapState.inwater? 0 : 24;
		return;
	}

	if (ck_inputFrame.yaxis == -1)
	{
		if (CK_KeenTryClimbPole(obj)) return;
		if (!ck_keenState.keenSliding && CK_KeenPressUp(obj)) return;
		obj->currentAction = &CK_ACT_keenLookUp1;
	}	
	else if (ck_inputFrame.yaxis == 1)
	{
		// Try poles.
		if (CK_KeenTryClimbPole(obj)) return;
		// Keen looks down.
		obj->currentAction = &CK_ACT_keenLookDown1;
		return;
	}
		
}

void CK_KeenStandingThink(CK_object *obj)
{
	// If we're riding a platform, do it surfin' style!
	if (obj->topTI == 0x19)
	{
		// But only if such an action exists in this episode. :/
#ifdef KEEN5
		if (&CK_ACT_keenRidePlatform)
		{
			obj->currentAction = &CK_ACT_keenRidePlatform;
		}
#endif
	}

	if (ck_inputFrame.xaxis || ck_inputFrame.yaxis || ck_keenState.jumpIsPressed || ck_keenState.pogoIsPressed || ck_keenState.shootIsPressed )
	{
		obj->user1 = obj->user2 = 0;	//Idle Time + Idle State
		obj->currentAction = &CK_ACT_keenStanding;
		CK_HandleInputOnGround(obj);

		return;
	}

	// K1n9_Duk3 addition:
	if (ck_mapState.nisasi)
		return;	// no idle animations when Keen has a Nisasi stuck to his face

	//If not on platform
	if ((obj->topTI & ~7) != 0x18)
		obj->user1 += tics;

	if (obj->user2 == 0 && obj->user1 > 200)
	{
		obj->user2++;
		obj->currentAction = &CK_ACT_keenIdle;
		obj->user1 = 0;
	}
	else if (obj->user2 == 1 && obj->user1 > 300)
	{
		obj->user2++;
		if (ck_mapState.moontile == 1)
		{
			CK_SpawnEfree(obj->posX, originyglobal-42*PIXGLOBAL);	// K1n9_Duk3 addition
			//ck_mapState.moontile = 2;	// don't spawn it again?
		}
		obj->currentAction = &CK_ACT_keenBored1;
		obj->user1 = 0;
	}
	else if (obj->user2 == 2 && obj->user1 > 600)
	{
		obj->user2++;
		obj->user1 = 0;
		if (!ck_mapState.inwater)
			obj->currentAction = &CK_ACT_keenYawn1;
	}

}

void CK_KeenLookUpThink(CK_object *obj)
{
	if (ck_inputFrame.yaxis != -1 ||
			ck_inputFrame.xaxis != 0 ||
			(ck_keenState.jumpIsPressed && !ck_keenState.jumpWasPressed) ||
			(ck_keenState.pogoIsPressed && !ck_keenState.pogoWasPressed) ||
			(ck_keenState.shootIsPressed))
	{
		obj->currentAction = &CK_ACT_keenStanding;
		CK_HandleInputOnGround(obj);
	}
}

void CK_KeenLookDownThink(CK_object *obj)
{
	int tile1, tile2;
	unsigned deltay;

	//Try to jump down
	if (ck_keenState.jumpIsPressed && !ck_keenState.jumpWasPressed
		&& (obj->topTI&7) == 1)
	{
		ck_keenState.jumpWasPressed = true;

		// cannot drop down into falling blocks:
		if (ck_keenState.platform && ck_keenState.platform->type == CT_FallBlock)
			return;

		//If the tiles below the player are blocking on any side but the top, they cannot be jumped through
		tile1 = CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY2, 1);
		tile2 = CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY2+1, 1);
		
		if (TI_ForeLeft(tile1) || TI_ForeBottom(tile1) || TI_ForeRight(tile1))
			return;

		if (TI_ForeLeft(tile2) || TI_ForeBottom(tile2) || TI_ForeRight(tile2))
			return;
		//#define max(a,b) (((a)>(b))?(a):(b))

		deltay = max(tics,4) << 4;
	
		//Moving platforms
		if (ck_keenState.platform)
			deltay += ck_keenState.platform->deltaPosY;

		ck_keenState.platform = NULL;


		obj->clipRects.unitY2 += deltay;
		obj->posY += deltay;
		ck_nextX = 0;
		ck_nextY = 0;
		obj->currentAction = &CK_ACT_keenFall1;
		obj->velX = obj->velY = 0;
		SD_PlaySound(KEENFALLSND);
		return;
	}
	

	if (ck_inputFrame.yaxis != 1 || ck_inputFrame.xaxis != 0 || (ck_keenState.jumpIsPressed && !ck_keenState.jumpWasPressed)
		|| (ck_keenState.pogoIsPressed && !ck_keenState.pogoWasPressed))
	{
		obj->currentAction = &CK_ACT_keenLookDown4;
		return;
	}
}	

// TODO: More to modify here
void CK_KeenDrawFunc(CK_object *obj)
{
	if (!obj->topTI)
	{
		RemoveDreamBubbles();	// just in case...
		SD_PlaySound(KEENFALLSND);
		obj->velX = obj->xDirection * 8;
		obj->velY = 0;
		CK_SetAction2(obj, &CK_ACT_keenFall1);
		ck_keenState.jumpTimer = 0;
	}
	else if ((obj->topTI & 0xFFF8) == 8)
	{
		CK_KillKeen();
	}
	else if (obj->topTI == 0x29)
	{
		// Keen6 conveyor belt right
		ck_nextX = tics * 8;
		ck_nextY = 0;
		obj->user1 = 0;
		CK_PhysUpdateNormalObj(obj);
	}
	else if (obj->topTI == 0x31)
	{
		// Keen6 conveyor belt left
		ck_nextX = tics * -8;
		ck_nextY = 0;
		obj->user1 = 0;
		CK_PhysUpdateNormalObj(obj);
	}
	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}

// TODO: More to modify here
void CK_KeenRunDrawFunc(CK_object *obj)
{
	if (!obj->topTI)
	{
		SD_PlaySound(KEENFALLSND);
		obj->velX = obj->xDirection * 8;
		obj->velY = 0;
		CK_SetAction2(obj, &CK_ACT_keenFall1);
		ck_keenState.jumpTimer = 0;
	}
	else if ((obj->topTI & ~SLOPEMASK) == 8)
	{
		CK_KillKeen();
	}
	else	// K1n9_Duk3 mod: make sure the wall check is not executed when already falling or dead
	{
		if (obj->topTI == 0x29)
		{
			// Keen6 conveyor belt right
			ck_nextX = tics * 8;
			ck_nextY = 0;
			obj->user1 = 0;
			CK_PhysUpdateNormalObj(obj);
		}
		else if (obj->topTI == 0x31)
		{
			// Keen6 conveyor belt left
			ck_nextX = tics * -8;
			ck_nextY = 0;
			obj->user1 = 0;
			CK_PhysUpdateNormalObj(obj);
		}
		
		// stop running if Keen hit a wall:
		if ((obj->rightTI && obj->xDirection == -1) || (obj->leftTI && obj->xDirection == 1))
		{
			obj->timeUntillThink = 0;
			obj->currentAction = &CK_ACT_keenStanding;
			obj->gfxChunk = (obj->xDirection == -1) ? obj->currentAction->chunkLeft : obj->currentAction->chunkRight;
		}
	}

	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}

//-----------------------------------------------------------------------------
// K1n9_Duk3 addition: Keen's Dreams
//-----------------------------------------------------------------------------

static void near UncacheDreamSprite(CK_object *obj)
{
	if (obj->gfxChunk > 0)
		CA_UncacheGrChunk(obj->gfxChunk);	// chunk is no longer needed
}

void RemoveDreamBubbles(void)
{
	register CK_object *obj;

	// remove all dream bubbles from the level:
	for (obj = ck_keenObj; obj; obj = obj->next)
	{
		if (obj->type == CT_DreamBubble)
		{
			UncacheDreamSprite(obj);
			CK_RemoveObj(obj);
		}
	}
}

void SpawnDreamBubble(unsigned x, unsigned y, unsigned type)
{
	CK_object *bubble = CK_GetNewObj(true);
	bubble->active = OBJ_ALWAYS_ACTIVE;
	bubble->zLayer = FOREGROUNDPRIORITY;
	bubble->type = CT_DreamBubble;
	bubble->posX = x;
	bubble->posY = y;
	bubble->user3 = type;
	bubble->clipped = CLIP_not;
	bubble->gfxChunk = DREAM3SPR;	// just to set visibility rectangle
	CK_SetAction(bubble, &CK_ACT_dreamBubble);
	bubble->gfxChunk = 0;	// actual sprite
	if (!type)
		bubble->user1 = (1.5*TickBase);	// Note: TickBase is a constant, so the floating point multiplication is only done at compile time
}

void CK_KeenStartDream(CK_object *obj)
{
	SpawnDreamBubble(obj->posX + 16*PIXGLOBAL, obj->posY - 11*PIXGLOBAL, 0);
	CK_SetAction2(obj, obj->currentAction->next);
}

void CK_KeenDreamThink(CK_object *obj)
{
	// check if we got any player input:
	if (ck_inputFrame.dir != dir_None || ck_keenState.jumpIsPressed 
		|| ck_keenState.pogoIsPressed || ck_keenState.shootIsPressed)
	{
		// stop dreaming:
		obj->currentAction = &CK_ACT_keenGetUp;
		obj->user1 = obj->user2 = 0;

		RemoveDreamBubbles();
	}
}

void CK_DreamThink(CK_object *obj)
{
	register int randnum;

	if (!obj->user3)
	{
		// move when Keen is moved:
		ck_nextX = ck_keenObj->posX + 16*PIXGLOBAL - obj->posX;
		ck_nextY = ck_keenObj->posY - 11*PIXGLOBAL - obj->posY;
	}

	if (obj->user1 > tics)
	{
		obj->user1 -= tics;

		// if Smirky can appear and there are only 3 seconds left,
		// we have a 50% chance that it will appear
		if (obj->user2 && obj->user1 <= 3*TickBase)
		{
			if (US_RndT() <= 127)
			{
				// we're going to change the sprite, so un-cache the current sprite
				UncacheDreamSprite(obj);

				obj->gfxChunk = DREAMSMIRKYSPR;
			}
			obj->user2 = false;	// always clear the "Smirky can appear" flag
		}
	}
	else
	{
		// we're going to change the sprite, so un-cache the current sprite
		UncacheDreamSprite(obj);

		// animation:
		switch (obj->gfxChunk)
		{
		case 0:
			obj->gfxChunk = DREAM1SPR;
			obj->user1 = 7;	// 0.1 seconds
			break;
		case DREAM1SPR:
		//case DREAM2SPR:
			obj->gfxChunk++;
			obj->user1 = 7;	// 0.1 seconds
			break;
		case DREAM2SPR:
			if (obj->user3)
			{
				// council page's dream bubble:
				obj->gfxChunk = (obj->user3 & 0x7FFF) + DREAMPAGE1SPR - 1;
				obj->user3++;
			}
		
			else
			{
				// Keen's dream bubble:
				randnum = US_RndT();
retry:
				if (obj->user4 < 0)	// -1 means show Nisasi dream bubble
				{
					obj->gfxChunk++;	// use Nisasi sprite (follows directly after DREAM2SPR)
					obj->user4 = 0;
				}
/*
				else if (!randnum)
					obj->gfxChunk++;
*/
				else if (randnum <= 51)
					obj->gfxChunk = DREAMBWBSPR;
				else if (randnum <= 89)
					obj->gfxChunk = DREAMTEDDYSPR;
				else if (randnum <= 127)
					obj->gfxChunk = DREAMYORPSPR;
				else if (randnum <= 165)
					obj->gfxChunk = DREAMROBOREDSPR;
				else if (randnum <= 191)
					obj->gfxChunk = DREAMICESPR, obj->user2 = true;	// Smirky can appear after 4 seconds
				else if (randnum <= 204)
					obj->gfxChunk = DREAMDREAMSPR;
				else if (randnum <= 216)
					obj->gfxChunk = DREAMSMIRKYSPR;
				else if (randnum <= 229)
					obj->gfxChunk = DREAMBANANASPR, obj->user2 = true;	// Smirky can appear after 4 seconds
				else if (randnum <= 232)
					obj->gfxChunk = DREAMMOONSPR;
				else
					obj->gfxChunk = DREAMDOPEFISHSPR;
				//
				// never show the same dream twice in a row:
				//
				if (obj->gfxChunk == obj->user4)
				{
					obj->user2 = false;	// clear Smirky flag, just in case
					randnum = (randnum+3) & 0xFF;
					goto retry;
				}
				obj->user4 = obj->gfxChunk;
			
			// Nisaba patch: Keen dreams only of the Dopefish when in level 11
			if (ck_gameState.currentLevel == MAP_DOPEFISHLEVEL) 
				{
					obj->gfxChunk = DREAMDOPEFISHSPR;
				}
			}
			obj->user1 = 7*TickBase;	// 7 seconds
			break;
		default:
			if (obj->user4 < 0)	// -1 means show Nisasi dream bubble
			{
				obj->gfxChunk = DREAM1SPR;
				obj->user1 = 7;
			}
			else
			{
				if (obj->user3)	// council page dream
				{
					// abort after the last dream sprite (avoid caching issues)
					if (obj->gfxChunk == DREAM_LUMP_END || (obj->user3 & 0x8000))
					{
						CK_RemoveObj(obj);
						break;
					}
				}
				obj->gfxChunk = 0;
				obj->user1 = TickBase;	// 1 second
			}
			break;
		}
	}
}

void CK_DreamReact(CK_object *obj)
{
	// auto-cache the current sprite if it isn't cached already:
	if (obj->gfxChunk > 0)
	{
		if (!grsegs[obj->gfxChunk] || !(grneeded[obj->gfxChunk] & ca_levelbit))
			CA_CacheGrChunk(obj->gfxChunk);
	}

	CK_BasicDrawFunc1(obj);
}

//-----------------------------------------------------------------------------
// end of Keen's Dreams
//-----------------------------------------------------------------------------

void CK_KeenReadThink(CK_object *obj)
{
	if (ck_inputFrame.xaxis != 0 || ck_keenState.jumpIsPressed || ck_keenState.pogoIsPressed)
	{
		obj->currentAction = &CK_ACT_keenStowBook1;
		obj->user1 = obj->user2 = 0;
	}
}

void CK_KeenJumpThink(CK_object *obj)
{
	// safety check: don't apply the tics again if we already have movement speed
	if (ck_nextX || ck_nextY)
		return;

	// Handle Jump Cheat
	if (ck_gameState.jumpCheat && ck_keenState.jumpIsPressed)
	{
		obj->velY = -40;
		ck_keenState.jumpTimer = 18;
		ck_keenState.jumpWasPressed = true;
	}	 

	if (ck_keenState.jumpTimer)
	{
		if (ck_keenState.jumpTimer <= tics)
		{
			ck_nextY = obj->velY * ck_keenState.jumpTimer;
			ck_keenState.jumpTimer = 0;
		}
		else
		{
			ck_nextY = obj->velY * tics;
			if (!ck_gameState.jumpCheat)
			{
				ck_keenState.jumpTimer -= tics;
			}
		}

		// Stop moving up if we've let go of control.
		if (!ck_keenState.jumpIsPressed)
		{
			ck_keenState.jumpTimer = 0;
		}

		if (!ck_keenState.jumpTimer && obj->currentAction->next)
		{
			obj->currentAction = obj->currentAction->next;
		}
	}
	else
	{
		//TODO: Check this w/ K5Disasm and/or x-disasm
		if (ck_gameState.difficulty == D_Easy)
		{
			//Nisaba patch: increase the gravity value a little bit for the Easy difficutly mode
		//	CK_PhysGravityMid(obj);
			CK_PhysGravityEasyKeen(obj);
		}
		else	// Normal or Hard
		{
			CK_PhysGravityHigh(obj);
		}

		if (obj->velY > 0 && obj->currentAction != &CK_ACT_keenFall1 && obj->currentAction != &CK_ACT_keenFall2)
		{
			obj->currentAction = obj->currentAction->next;
		}
	}


	//Move horizontally
	if (ck_inputFrame.xaxis)
	{

		obj->xDirection = ck_inputFrame.xaxis;
		CK_PhysAccelHorz(obj, ck_inputFrame.xaxis*2, 24);

#if (MINTICS < 2)		// K1n9_Duk3 fix for 70 fps:
		if (!ck_nextX && tics < 2)
			ck_nextX = ck_inputFrame.xaxis;
		// This makes sure that Keen always moves by at least 1 unit while
		// jumping and holding down the left or right key, so that Keen can
		// grab edges reliably. CK_PhysAccelHorz only accelerates every other
		// tic and hitting a wall while jumping always sets velX to 0. The
		// original Keen games ran at 35 fps max (2 tics per frame), so the
		// ck_nextX value would almost never be 0 here. Keen can only grab
		// edges if the leftTI or rightTI values are set correctly, which
		// requires the object to move by at least 1 unit along the x axis.
#endif
	}
	else CK_PhysDampHorz(obj);

	//Pole
	if (obj->bottomTI == 17)
	{
		ck_nextX = 0;
		obj->velX = 0;
	}

	//Shooting
	if (ck_keenState.shootIsPressed && !ck_keenState.shootWasPressed)
	{
		ck_keenState.shootWasPressed = true;
		switch (ck_inputFrame.yaxis)
		{
		case -1:
			obj->currentAction = &CK_ACT_keenJumpShootUp1;
			break;
		case 0:
			obj->currentAction = &CK_ACT_keenJumpShoot1;
			break;
		case 1:
			obj->currentAction = &CK_ACT_keenJumpShootDown1;
			break;
		}
		return;
	}

	if (ck_keenState.pogoIsPressed && !ck_keenState.pogoWasPressed)
	{
		ck_keenState.pogoWasPressed = true;
		obj->currentAction = &CK_ACT_keenPogo2;
		ck_keenState.jumpTimer = 0;
		return;
	}

	if (ck_inputFrame.yaxis == -1)
	{
		CK_KeenTryClimbPole(obj);
	}
	
}

void CK_KeenJumpDrawFunc(CK_object *obj)
{
	// K1n9_Duk3 addition:
	if (obj->velY < 0)	// if moving up
	{
		if (obj->topTI != 0x19)	// if has NOT hit a platform object
			obj->topTI = 0;	// don't land
	}
	else if (obj->velY > 0)	// if moving down
	{
		obj->bottomTI = 0;	// don't hit ceiling
	}
	// end of addition
	
	if (obj->rightTI && obj->xDirection == -1)
	{
		obj->velX = 0;
	}
	else if (obj->leftTI && obj->xDirection == 1)
	{
		obj->velX = 0;
	}

	// Did we hit our head on the ceiling?
	if (obj->bottomTI)
	{
		//TODO: Something to do with poles (push keen into the centre)
		if (obj->bottomTI == 17)	//Pole
		{
#if 0
			// old code:
			obj->posY -= 32;
			obj->clipRects.unitY1 -= 32;
			obj->velX = 0;
			obj->posX = (obj->clipRects.tileXmid << G_T_SHIFT) - 32;
#else
			// new code:
			CK_PhysUpdateY(obj, -2*PIXGLOBAL);
			obj->velX = 0;
			CK_PhysUpdateX(obj, (obj->clipRects.tileXmid << G_T_SHIFT) - obj->clipRects.unitX1);
#endif
		}
		else if (!ck_gameState.jumpCheat)
		{
			SD_PlaySound(KEENHITCEILINGSND);
			if (obj->bottomTI > 1)
			{
				obj->velY += 16;
				if (obj->velY < 0)
					obj->velY = 0;
			}
			else
			{
				obj->velY = 0;
			}
			ck_keenState.jumpTimer = 0;
		}
	}

	// Have we landed?
	if (obj->topTI)
	{
		obj->yDirection = motion_Down;	// K1n9_Duk3 fix: avoid door glitch
		obj->deltaPosY = 0;
		//Check if deadly.
		if ((obj->topTI & ~7) == 8)
		{
			CK_KillKeen();
		}
		else
		{
#ifdef KEEN5
			if (obj->topTI == 0x39) // Fuse
			{
				SD_PlaySound(KEENLANDONFUSESND);
			}
#endif
			if (obj->topTI != 0x19 || !ck_keenState.jumpTimer) // Or standing on a platform.
			{
				obj->velY = obj->user1 = obj->user2 = 0;	// Being on the ground is boring. // K1n9_Duk3 mod: also set velY to 0
	
				// K1n9_Duk3 mod: use correct timing and also handle second shooting state
				if (obj->currentAction == &CK_ACT_keenJumpShoot1)
				{
					int temp = obj->actionTimer;
					CK_SetAction2(obj, &CK_ACT_keenShoot1);
					obj->actionTimer = temp;
				}
				else if (obj->currentAction == &CK_ACT_keenJumpShootUp1)
				{
					int temp = obj->actionTimer;
					CK_SetAction2(obj, &CK_ACT_keenShootUp1);
					obj->actionTimer = temp;
				}
				else if (obj->currentAction == &CK_ACT_keenJumpShootUp2)
				{
					CK_SetAction2(obj, &CK_ACT_keenShootUp1);
					obj->actionTimer = obj->currentAction->timer-1;
				}
				else if (obj->currentAction == &CK_ACT_keenJumpShoot2)
				{
					CK_SetAction2(obj, &CK_ACT_keenShoot1);
					obj->actionTimer = obj->currentAction->timer-1;
				}
				else if (ck_inputFrame.xaxis)
				{
					CK_SetAction2(obj, &CK_ACT_keenRun1);
				}
				else
				{
					CK_SetAction2(obj, &CK_ACT_keenStanding);
				}
				SD_PlaySound(KEENLANDSND);
			}
		}
	}
	
	else if (obj->deltaPosY > 0)
	{
		// temp6 = Keen's previous upper y coord
		unsigned temp6 = obj->clipRects.unitY1 - obj->deltaPosY;
		// temp8 = Keen's current upper y coord - 1.5 tiles, rounded to nearest tile, + 1.5 tiles
		unsigned temp8 = ((obj->clipRects.unitY1 - 64) & 0xFF00) + 64;
		// temp10 = temp8 in tile coords, - 1
		int temp10 = ((unsigned)temp8 >> G_T_SHIFT) - 1 ;

		// If we're moving past a tile boundary.
		if (temp6 < temp8 && obj->clipRects.unitY1 >= temp8 && !ck_mapState.nisasi)	// K1n9_Duk3 mod: don't grab with Nisasi
		{
			// Moving left...
			if (ck_inputFrame.xaxis == -1)
			{
				int tileX = obj->clipRects.tileX1 - ((obj->rightTI)?1:0);
				int tileY = temp10;
				//VL_ScreenRect((tileX << 4) - (originxglobal >> 4), (tileY << 4) - (originyglobal >> 4), 16, 16, 1);
				int upperTile = CA_TileAtPos(tileX, tileY, 1);
				int lowerTile = CA_TileAtPos(tileX, tileY+1, 1);
				if ( (!TI_ForeLeft(upperTile) && !TI_ForeRight(upperTile) && !TI_ForeTop(upperTile) && !TI_ForeBottom(upperTile)) &
					TI_ForeRight(lowerTile) && TI_ForeTop(lowerTile))
				{
					obj->xDirection = -1;
					obj->clipped = CLIP_not;
					obj->posX = (obj->posX & 0xFF00) + 128;
					obj->posY = (temp8 - 64);
					obj->velY = obj->deltaPosY = 0;
					CK_SetAction2(obj, &CK_ACT_keenHang1);
				}
			}
			else if (ck_inputFrame.xaxis == 1)
			{
				int tileX = obj->clipRects.tileX2 + ((obj->leftTI)?1:0);
				int tileY = temp10;
				int upperTile = CA_TileAtPos(tileX, tileY, 1);
				int lowerTile = CA_TileAtPos(tileX, tileY+1, 1);

				if (!TI_ForeLeft(upperTile) && !TI_ForeRight(upperTile) && !TI_ForeTop(upperTile) && !TI_ForeBottom(upperTile) &
					TI_ForeLeft(lowerTile) && TI_ForeTop(lowerTile))
				{
					obj->xDirection = 1;
					obj->clipped = CLIP_not;
					obj->posX = (obj->posX & 0xFF00) + 256;
					obj->posY = (temp8 - 64);
					obj->velY = obj->deltaPosY = 0;
					CK_SetAction2(obj, &CK_ACT_keenHang1);
				}
			}
		}
	}

	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}

void CK_KeenPogoBounceThink(CK_object *obj)
{
	obj->velY = -48;
	ck_nextY = 6 * obj->velY;
	ck_keenState.jumpTimer = ck_mapState.inwater? 0 : 24;
	SD_PlaySound(KEENPOGOSND);
}

void CK_KeenPogoThink(CK_object *obj)
{
	// safety check: don't apply the tics again if we already have movement speed
	if (ck_nextX || ck_nextY)
		return;

	if (!ck_keenState.jumpTimer)
	{
		if (ck_gameState.difficulty == D_Easy)
		{
			//Nisaba patch: increase the gravity value a little bit for the Easy difficutly mode
		//	CK_PhysGravityMid(obj);
			CK_PhysGravityEasyKeen(obj);
		}
		else
		{
			CK_PhysGravityHigh(obj);
		}
	}
	else
	{
		if (ck_keenState.jumpIsPressed || ck_keenState.jumpTimer <= 9) CK_PhysGravityLow(obj);
		//if (ck_keenState.jumpIsPressed || ck_keenState.jumpTimer <= 9) ck_nextY += obj->velY * tics;
		else CK_PhysGravityHigh(obj);

		if (ck_keenState.jumpTimer <= tics) ck_keenState.jumpTimer = 0;
		else ck_keenState.jumpTimer -= tics;

		if (!ck_keenState.jumpTimer && obj->currentAction->next) obj->currentAction = obj->currentAction->next;
	}
	if (ck_inputFrame.xaxis)
	{
		if (!obj->velX)
			obj->xDirection = ck_inputFrame.xaxis;
		CK_PhysAccelHorz(obj, ck_inputFrame.xaxis, 24);
	}
	else
	{
		ck_nextX += obj->velX * tics;
		if (obj->velX < 0) obj->xDirection = -1;
		else if (obj->velX > 0) obj->xDirection = 1;
	}

	// Stop for poles?
	if (obj->bottomTI == 17)
	{
		ck_nextX = 0;
		obj->velX = 0;
	}

	//Shooting
	if (ck_keenState.shootIsPressed && !ck_keenState.shootWasPressed)
	{
		ck_keenState.shootWasPressed = true;
		switch (ck_inputFrame.yaxis)
		{
		case -1:
			obj->currentAction = &CK_ACT_keenJumpShootUp1;
			return;
		case 0:
			obj->currentAction = &CK_ACT_keenJumpShoot1;
			return;
		case 1:
			obj->currentAction = &CK_ACT_keenJumpShootDown1;
			return;
		}
	}
	
	//Stop pogoing if Alt pressed
	if (ck_keenState.pogoIsPressed && !ck_keenState.pogoWasPressed)
	{
		ck_keenState.pogoWasPressed = true;
		obj->currentAction = &CK_ACT_keenFall1;
	}
}

void CK_KeenToggleTrapDoor(int x, int y)
{
/*
	//
	// old fuse-breaking code from Keen 5
	//
	unsigned brokenFuseTiles[] = {0, 0};

	CK5_SpawnFuseExplosion(x,y);
	if (!(--ck_gameState.fusesRemaining))
	{
		CK5_SpawnLevelEnd();
	}

	
	brokenFuseTiles[0] = CA_TileAtPos(0,0,1);
	brokenFuseTiles[1] = CA_TileAtPos(0,1,1);

	RF_MemToMap(brokenFuseTiles, 1, x, y, 1, 2);
*/

	unsigned ix, iy, i;
	unsigned trapdoorTiles[8];
	unsigned mapindex = mapbwidthtable[y]/2 + x;

	if (TI_ForeTop(mapsegs[1][mapindex+mapwidth]) == TI_TOP_BREAKABLE)
	{
		mapindex--;
		x--;
	}

	i = 0;
	for (iy=0; iy<2; iy++, mapindex+=mapwidth)
	{
		for (ix=0; ix<4; ix++)
		{
			register unsigned tile = mapsegs[1][mapindex+ix];
			trapdoorTiles[i++] = tile + TI_ForeAnimTile(tile);
		}
	}

	RF_MemToMap(trapdoorTiles, 1, x, y, 4, 2);
}

void CK_KeenPogoDrawFunc(CK_object *obj)
{
	// K1n9_Duk3 addition:
	if (obj->velY < 0)	// if moving up
	{
		if (obj->topTI != 0x19)	// if has NOT hit a platform object
			obj->topTI = 0;	// don't land
	}
	else if (obj->velY > 0)	// if moving down
	{
		obj->bottomTI = 0;	// don't hit ceiling
	}
	// end of addition
	
	if (obj->rightTI && obj->xDirection == -1)
	{
		obj->velX = 0;
	}
	else if (obj->leftTI && obj->xDirection == 1)
	{
		obj->velX = 0;
	}

	if (obj->bottomTI)
	{
		if (obj->bottomTI == 17)	//Pole
		{
#if 0
			// old code:
			obj->posY -= 32;
			obj->clipRects.unitY1 -= 32;
			obj->velX = 0;
			obj->posX = (obj->clipRects.tileXmid << G_T_SHIFT) - 32;
#else
			// new code:
			CK_PhysUpdateY(obj, -2*PIXGLOBAL);
			obj->velX = 0;
			CK_PhysUpdateX(obj, (obj->clipRects.tileXmid << G_T_SHIFT) - obj->clipRects.unitX1);
#endif
		}
		else if (!ck_gameState.jumpCheat)
		{
			SD_PlaySound(KEENHITCEILINGSND);

			if (obj->bottomTI > 1)
			{
				obj->velY += 16;
				if (obj->velY < 0) obj->velY = 0;
			}
			else obj->velY = 0;
		
			ck_keenState.jumpTimer = 0;
		}
	}

	// Houston, we've landed!
	if (obj->topTI)
	{
		obj->yDirection = motion_Down;	// K1n9_Duk3 fix: avoid door glitch
		obj->deltaPosY = 0;
		//Check if deadly.
		if ((obj->topTI & ~7) == 8)
		{
			CK_KillKeen();
		}
		else
		{
			if (obj->topTI == TI_TOP_BREAKABLE) // Trap door in FITF
			{
				if (ck_gameState.currentLevel == MAP_DOPEFISHLEVEL)	// Nisaba patch: fuse has no effect in level 11
				{
				}
				else if (obj->velY >= 0x30)
				{
					SD_PlaySound(ROOTSSND);
					CK_KeenToggleTrapDoor(obj->clipRects.tileXmid-1, obj->clipRects.tileY2);
					if (!TI_ForeTop(CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY2+1, 1)))
					{
						// ground is no longer solid, so don't bounce
						RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
						return;
					}
				}
				else
				{
#ifdef KEEN5
					SD_PlaySound(KEENLANDONFUSESND);
#endif
				}
			}
			if (obj->topTI != 0x19 || ck_keenState.jumpTimer == 0)
			{
				obj->velY = -48;
				ck_keenState.jumpTimer = ck_mapState.inwater? 0 : 24;
				SD_PlaySound(KEENPOGOSND);
				CK_SetAction2(obj, &CK_ACT_keenPogo2);
			}
		}
	}

	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}

void CK_KeenSpecialColFunc(CK_object *obj, CK_object *other)
{
	// K1n9_Duk3 mod: using switch instead of if/else if
	switch (other->type)
	{
	case CT_Item:	// K1n9_Duk3 addition: also pick up object-based items while on pole
		CK_KeenColFunc(obj, other);
		break;

	case CT_FallBlock:
		obj->zLayer = KEENPRIORITY;
		obj->clipped = CLIP_normal;
		CK_SetAction2(obj, &CK_ACT_keenFall1);
		ck_keenState.jumpTimer = 0;
		obj->velX = 0;
		obj->velY = 0;
		CK_ClipToSprite(obj, other, true);
		break;

#if 0	//touching platforms shouldn't cause Keen to fall off of poles or edges
	case CT_Platform:
	case CT_LogPlatform:	// K1n9_Duk3 mod: added log platform & cloud
	case CT_Cloud:
		obj->clipped = CLIP_normal;
		CK_SetAction2(obj, &CK_ACT_keenFall1);
		ck_keenState.jumpTimer = 0;
		obj->velX = 0;
		obj->velY = 0;
		CK_PhysPushY(obj, other);
		break;
#endif
#if 0
	case CT_Ampton:
	case CT_Korath:
		obj->zLayer = KEENPRIORITY;
		obj->clipped = CLIP_normal;
		CK_SetAction2(obj, &CK_ACT_keenFall1);
		ck_keenState.jumpTimer = 0;
		obj->velX = 0;
		obj->velY = 0;
		break;
#endif
	}

}

void CK_KeenSpecialDrawFunc(CK_object *obj)
{
	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}

void CK_KeenHangThink(CK_object *obj)
{
	if (ck_inputFrame.yaxis == -1 || ck_inputFrame.xaxis == obj->xDirection)
	{
		obj->currentAction = &CK_ACT_keenPull1;

		if(obj->xDirection == 1)
		{
			ck_nextY = -256;
		}
		else
		{
			ck_nextY = -128;
		}
		//TODO: Set keen->zlayer 3

		//if (obj->xDirection == 1)
		//{
			

	}
	else if (ck_inputFrame.yaxis == 1 || (ck_inputFrame.xaxis && ck_inputFrame.xaxis != obj->xDirection))
	{
		// Drop down.
		obj->currentAction = &CK_ACT_keenFall1;
		obj->clipped = CLIP_normal;
	}
}

void CK_KeenPullThink1(CK_object *obj)
{
	if (obj->xDirection == 1)
		ck_nextX = 128;
	else
		ck_nextY = -128;
}

void CK_KeenPullThink2(CK_object *obj)
{
	ck_nextX = obj->xDirection * 128;
	ck_nextY = -128;
}

void CK_KeenPullThink3(CK_object *obj)
{
	ck_nextY = -128;
}

void CK_KeenPullThink4(CK_object *obj)
{
	obj->clipped = CLIP_normal;
	obj->zLayer = KEENPRIORITY;
	ck_nextY = 128;
}


void CK_KeenDeathThink(CK_object *obj)
{
	CK_PhysGravityMid(obj);
	ck_nextX = obj->velX * tics;
	if (!CK_ObjectVisible(obj))
	{
		ck_levelState = LS_Died;	// K1n9_Duk3 fix: use enum name instead of number
	}
}

void CK_KillKeen()
{
	CK_KillKeenEx(false);
}

void CK_KillKeenEx(boolean force)
{
	CK_object *obj = ck_keenObj;

	// K1n9_Duk3 addition: force game to kill Keen (tiles are ALWAYS deadly)
	if (force)
		force = !CK_KeenIsDead();

	if (ck_mapState.invincibilityTimer && !force)
	{
		return;
	}

	if (ck_gameState.godMode)
	{
		return;
	}

	// K1n9_Duk3 addition: don't change state when Keen is dying a special death
	if ((obj->currentAction == &CK_ACT_keenDrown || obj->currentAction == &CK_ACT_keenBurn)
		&& obj->type != CT_Player)
	{
		return;
	}

	//TODO: ACTION_KEENNOT, KeenMoon
	ck_mapState.invincibilityTimer = 30;
	ck_scrollDisabled = true;
	obj->clipped = CLIP_not;
	obj->zLayer = FOREGROUNDPRIORITY;
	if (US_RndT() < 0x80)
	{
		CK_SetAction2(obj, &CK_ACT_keenDie0);
	}
	else
	{
		CK_SetAction2(obj, &CK_ACT_keenDie1);
	}

	SD_PlaySound(KEENDIESND);

	obj->velY = -40;
	obj->velX = 16;

	RemoveDreamBubbles();	// K1n9_Duk3 addition
	CK_DropNisasi();
}

boolean CK_KeenIsOverPole(CK_object *obj)
{
	unsigned y = obj->clipRects.tileY1;
	unsigned off = obj->clipRects.tileXmid + mapbwidthtable[y]/2;

	while (y <= obj->clipRects.tileY2)
	{
		if ((TI_ForeMisc(mapsegs[1][off]) & 0x7F) == MF_Pole)
			return true;
		y++;
		off += mapwidth;
	}
	return false;
}

void CK_KeenPoleHandleInput(CK_object *obj)
{
	if (ck_inputFrame.xaxis)
		obj->xDirection = ck_inputFrame.xaxis;

	if (!CK_KeenIsOverPole(obj))
	{
		obj->velX = obj->velY = 0;
		obj->clipped = CLIP_normal;
		obj->currentAction = &CK_ACT_keenJump1;
		obj->yDirection = 1;
		ck_keenState.poleGrabTime = lasttimecount;
		ck_keenState.jumpTimer = 0;
		SD_PlaySound(KEENFALLSND);
		return;
	}

	//Shooting things. *ZAP!*
	if (ck_keenState.shootIsPressed && !ck_keenState.shootWasPressed)
	{
		ck_keenState.shootWasPressed = true;

		switch (ck_inputFrame.yaxis)
		{
		case -1:
			obj->currentAction = &CK_ACT_keenPoleShootUp0;
			break;
		case 0:
			obj->currentAction = &CK_ACT_keenPoleShoot0;
			break;
		case 1:
			obj->currentAction = &CK_ACT_keenPoleShootDown0;
			break;
		}
	}

	
	if (ck_keenState.jumpIsPressed && !ck_keenState.jumpWasPressed)
	{
		ck_keenState.jumpWasPressed = true;
		SD_PlaySound(KEENJUMPSND);
		obj->velX = ck_KeenPoleOffs[ck_inputFrame.xaxis+1];
		obj->velY = -20;
		obj->clipped = CLIP_normal;
		ck_keenState.jumpTimer = 10;
		obj->currentAction = &CK_ACT_keenJump1;
		obj->yDirection = 1;
		ck_keenState.poleGrabTime = lasttimecount;
	}
	return;
}

void CK_KeenPoleSitThink(CK_object *obj)
{
	int groundTile;

	//TODO: Support climb up/down
	if (ck_inputFrame.yaxis == 1)
	{
		obj->currentAction = &CK_ACT_keenPoleDown1;
		obj->yDirection = 1;
		CK_KeenPoleDownThink(obj);
		return;
	}
	else if (ck_inputFrame.yaxis == -1)
	{
		obj->currentAction = &CK_ACT_keenPoleUp1;
		obj->yDirection = -1;
		//CK_KeenPoleUpThink(obj);
		return;
	}

	// When keen is at ground level, allow dismounting using left/right.
	if (ck_inputFrame.xaxis)
	{
		groundTile = CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY2+1, 1);
		if (TI_ForeTop(groundTile))
		{
			obj->velX = 0;
			obj->velY = 0;
			obj->clipped = CLIP_normal;
			ck_keenState.jumpTimer = 0;
			obj->currentAction = &CK_ACT_keenFall1;
			obj->yDirection = 1;
			SD_PlaySound(KEENFALLSND);
			return;
		}
	}

	//TODO: Pole input func
	CK_KeenPoleHandleInput(obj);
}

void CK_KeenPoleUpThink(CK_object *obj)
{
	int topTile = CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY1, 1);
	
	if ((TI_ForeMisc(topTile) & 0x7F) != MF_Pole)
	{
		ck_nextY = 0;
		obj->currentAction = &CK_ACT_keenPoleSit;
		CK_KeenPoleHandleInput(obj);
		return;
	}

	if (ck_inputFrame.yaxis == 0)
	{
		obj->currentAction = &CK_ACT_keenPoleSit;
		obj->yDirection = 0;
		//CK_KeenPoleSitThink(obj);
	}
	else if (ck_inputFrame.yaxis == 1)
	{
		obj->currentAction = &CK_ACT_keenPoleDown1;
		obj->yDirection = 1;
		CK_KeenPoleDownThink(obj);
	}

	CK_KeenPoleHandleInput(obj);
}

void CK_KeenPoleShootThink(CK_object *obj)
{
	if (!CK_KeenIsOverPole(obj))
	{
		obj->velX = obj->velY = 0;
		obj->clipped = CLIP_normal;
		obj->yDirection = 1;
		ck_keenState.poleGrabTime = lasttimecount;
		ck_keenState.jumpTimer = 0;
		if (obj->currentAction == &CK_ACT_keenPoleShoot1)
		{
			obj->currentAction = &CK_ACT_keenJumpShoot2;
		}
		else if (obj->currentAction == &CK_ACT_keenPoleShootUp1)
		{
			obj->currentAction = &CK_ACT_keenJumpShootUp2;
		}
		else if (obj->currentAction == &CK_ACT_keenPoleShootDown1)
		{
			obj->currentAction = &CK_ACT_keenJumpShootDown2;
		}
		else	// shot has already been spawned, so just make Keen fall off:
		{
			obj->currentAction = &CK_ACT_keenJump1;
		}
	}
}

void CK_KeenPoleDownThink(CK_object *obj)
{

	int tileUnderneath = CA_TileAtPos(obj->clipRects.tileXmid, (obj->clipRects.unitY2-64) >> G_T_SHIFT, 1);

	if ((TI_ForeMisc(tileUnderneath) & 0x7F) != MF_Pole)
	{
		// We're no longer on a pole.
		SD_PlaySound(KEENFALLSND);
		obj->currentAction = &CK_ACT_keenFall1;
		ck_keenState.jumpTimer = 0;
		obj->velX = ck_KeenPoleOffs[ck_inputFrame.xaxis + 1];
		obj->velY = 0;
		obj->clipped = CLIP_normal;
		obj->clipRects.tileY2 -= 1;	//WTF?
		return;
	}

	if (ck_inputFrame.yaxis == 0)
	{
		obj->currentAction = &CK_ACT_keenPoleSit;
		obj->yDirection = 0;
	}
	else if (ck_inputFrame.yaxis == -1)
	{
		obj->currentAction = &CK_ACT_keenPoleUp1;
		obj->yDirection = -1;
	}
	
	CK_KeenPoleHandleInput(obj);
}

void CK_KeenPoleDownDrawFunc(CK_object *obj)
{
	// Check if keen is trying to climb through the floor.
	// It's quite a strange clipping error if he succeeds.

	int groundTile = CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY2, 1);

	if (TI_ForeTop(groundTile) == 1)
	{
		int yReset = -(obj->clipRects.unitY2 & 255) - 1;
		obj->posY += yReset;
		obj->clipRects.unitY2 += yReset;
		obj->clipRects.tileY2 += -1;
		obj->clipped = CLIP_normal;
		CK_SetAction2(obj, &CK_ACT_keenLookDown1);
	}

	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}

// Shooting

void CK_SpawnShot(int x, int y, int direction)
{
	CK_object *shot, *hit;

	if (!ck_gameState.numShots)
	{
		SD_PlaySound(CLICKSND);
		return;
	}

	ck_gameState.numShots--;

	shot = CK_GetNewObj(true);
	shot->posX = x;
	shot->posY = y;
	shot->zLayer = 1;
	shot->type = CT_Stunner;
	shot->active = OBJ_ALWAYS_ACTIVE;

	SD_PlaySound(KEENSHOOTSND);
	
	switch(direction)
	{
	case 0:
		shot->xDirection = 0;
		shot->yDirection = -1;
		break;
	case 2:
		shot->xDirection = 1;
		shot->yDirection = 0;
		break;
	case 4:
		shot->xDirection = 0;
		shot->yDirection = 1;
		break;
	case 6:
		shot->xDirection = -1;
		shot->yDirection = 0;
		break;
	default:
		//Quit("SpawnShot: Bad dir!");
		SM_Quit(S_BADSHOTDIR);
	}

	CK_SetAction(shot, &CK_ACT_keenShot1);

#if 0
	//
	// check if the shot hits an enemy:
	// (prevents enemies from killing Keen before they get stunned)
	//
	
	// K1n9_Duk3 note:
	// This doesn't help much. The shot and the enemies haven't moved yet at this
	// point, and collisions will be checked after they have moved. But the real
	// problem is that Keen is the first object in the object list and any
	// collisions with Keen are ALWAYS handled before handling collisions with
	// the shots in the original Keen 4-6 code.
	
	for (hit=ck_keenObj->next; hit; hit = hit->next)
	{
		if (hit->currentAction->collide && CK_ObjectsCollide(shot, hit))
		{
			hit->currentAction->collide(hit, shot);

			if (shot->type != CT_Stunner)
				break;	// shot has exploded
		}
	}
#endif
}

void CK_ShotHit(CK_object *obj)
{
	//TODO: Implement obj_ classes.
	obj->type = CT_Friendly;
	CK_SetAction2(obj, &CK_ACT_keenShotHit1);
	SD_PlaySound(KEENSHOTHITSND);
}

void CK_ShotThink(CK_object *shot)
{
	CK_object *currentObj;

	// Stun things which are offscreen.
	if ((shot->clipRects.tileX2 < (originxglobal >> G_T_SHIFT)) ||
	    (shot->clipRects.tileY2 < (originyglobal >> G_T_SHIFT)) ||
	    (shot->clipRects.tileX1 > (originxglobal >> G_T_SHIFT) + (320 >> 4)) ||
	    (shot->clipRects.tileY1 > (originyglobal >> G_T_SHIFT) + (208 >> 4)))
	{
		if ((shot->clipRects.tileX2 + 10 < (originxglobal >> G_T_SHIFT)) ||
		    (shot->clipRects.tileX1 - 10 > (originxglobal >> G_T_SHIFT) + (320 >> 4)) ||
		    (shot->clipRects.tileY2 + 6 < (originyglobal >> G_T_SHIFT)) ||
		    (shot->clipRects.tileY1 - 6 > (originyglobal >> G_T_SHIFT) + (208 >> 4)))
		{
			CK_RemoveObj(shot);
			return;
		}
		for (currentObj = ck_keenObj->next; currentObj; currentObj = currentObj->next)
		{
			if (currentObj->active ||
			    (shot->clipRects.unitX2 <= currentObj->clipRects.unitX1) ||
			    (shot->clipRects.unitX1 >= currentObj->clipRects.unitX2) ||
			    (shot->clipRects.unitY1 >= currentObj->clipRects.unitY2) ||
			    (shot->clipRects.unitY2 <= currentObj->clipRects.unitY1))
				continue;
			if (currentObj->currentAction->collide)
			{
				currentObj->currentAction->collide(currentObj, shot);
				currentObj->visible = true;
				currentObj->active = OBJ_ACTIVE;
			}
			if (shot->type == CT_nothing)
				return;
		}
	}
}

void CK_ShotDrawFunc(CK_object *obj)
{
	unsigned t;

	// shoot down through a pole hole
	if (obj->topTI == 1 && obj->clipRects.tileX1 != obj->clipRects.tileX2) 
	{
		t = CA_TileAtPos(obj->clipRects.tileX2, obj->clipRects.tileY1-1, 1);
		if (TI_ForeTop(t) == 0x11) 
		{
			obj->topTI = 0x11;
			obj->posX += 0x100 - (obj->posX & 0xFF);
		}
	} 
	// move into pole hole before making contact
	else if (obj->topTI == 0x11 && obj->clipRects.tileX1 != obj->clipRects.tileX2) 
	{
		obj->posX &= 0xFF00;
	}

	// shoot through pole hole upwards
	if (obj->bottomTI == 1 && obj->clipRects.tileX1 != obj->clipRects.tileX2) 
	{
		t = CA_TileAtPos(obj->clipRects.tileX2, obj->clipRects.tileY2+1, 1);
		if (TI_ForeBottom(t) == 0x11) 
		{
			obj->bottomTI = 0x11;
			obj->posX += 0x100 - (obj->posX & 0xFF);
		}
	} 
	// move into pole hole whilst travelling upwards
	else if (obj->bottomTI == 0x11 && obj->clipRects.tileX1 != obj->clipRects.tileX2) 
	{
		obj->posX &= 0xFF00;
	}

	// if hit any other type of object, die
	if (obj->topTI != 0x11 && obj->bottomTI != 0x11) 
	{
		if (obj->topTI || obj->bottomTI || obj->rightTI || obj->leftTI) 
		{
			CK_ShotHit(obj);
		}
	} 
	else 
	// correct for pole hole passage
	{
		ck_nextY = obj->currentAction->velY * tics * obj->yDirection;
		obj->posY += ck_nextY;
		obj->clipRects.unitY1 += ck_nextY;
		obj->clipRects.unitY2 += ck_nextY;
		obj->clipRects.tileY1 = obj->clipRects.unitY1 >> G_T_SHIFT;
		obj->clipRects.tileY2 = obj->clipRects.unitY2 >> G_T_SHIFT;
	}
	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}

void CK_KeenSpawnShot(CK_object *obj)
{
	if (ck_gameState.currentLevel == MAP_MUSHROOMLEVEL1)
	{
		SD_PlaySound(KITTENEXPLODESND);
		VW_FadeUp();
		obj->gfxChunk = KEENBURNED1SPR;
		CK_DreamReact(obj);
		RF_Refresh();
		VW_FadeDown();
		ck_gameState.godMode = false;
		CK_BurnKeen(true);
		return;
	}
	
	if (obj->currentAction == &CK_ACT_keenShoot1)
	{
		if (obj->xDirection == 1)
		{
			CK_SpawnShot(obj->posX + 256, obj->posY + 64, 2);
		}
		else
		{
			CK_SpawnShot(obj->posX - 128, obj->posY + 64, 6);
		}
	}
	if (obj->currentAction == &CK_ACT_keenJumpShoot2)
	{
		if (obj->xDirection == 1)
		{
			CK_SpawnShot(obj->posX + 256, obj->posY + 32, 2);
		}
		else
		{
			CK_SpawnShot(obj->posX, obj->posY + 32, 6);
		}
	}
	if (obj->currentAction == &CK_ACT_keenJumpShootDown2)
	{
		CK_SpawnShot(obj->posX + 128, obj->posY + 288, 4);
	}
	if (obj->currentAction == &CK_ACT_keenJumpShootUp2)
	{
		CK_SpawnShot(obj->posX + 80, obj->posY - 160, 0);
	}
	if (obj->currentAction == &CK_ACT_keenShootUp1)
	{
		CK_SpawnShot(obj->posX + 80, obj->posY - 160, 0);
	}
	if (obj->currentAction == &CK_ACT_keenPoleShoot1)
	{
		if (obj->xDirection == 1)
		{
			CK_SpawnShot(obj->posX + 256, obj->posY + 64, 2);
		}
		else
		{
			CK_SpawnShot(obj->posX - 128, obj->posY + 64, 6);
		}
	}
	if (obj->currentAction == &CK_ACT_keenPoleShootUp1)
	{
		if (obj->xDirection == 1)
		{
			CK_SpawnShot(obj->posX + 96, obj->posY + 64, 0);
		}
		else
		{
			CK_SpawnShot(obj->posX + 192, obj->posY + 64, 0);
		}
	}
	if (obj->currentAction == &CK_ACT_keenPoleShootDown1)
	{
		if (obj->xDirection == 1)
		{
			CK_SpawnShot(obj->posX + 96, obj->posY + 384, 4);
		}
		else
		{
			CK_SpawnShot(obj->posX + 192, obj->posY + 384, 4);
		}
	}
}

void CK_KeenFall(CK_object *obj)
{
	CK_PhysGravityHigh(obj);
	ck_nextX = obj->velX * tics;
}

#if 1
/*
// Knocks keen away from the coordinate passed as an argument
void CK_DamageKeen(unsigned x, unsigned y)
{
	if (ck_gameState.difficulty == D_Classic)
	{
		CK_KillKeen();
	}
	else
	{
		int dx;

		if (ck_gameState.godMode || ck_mapState.invincibilityTimer > 0)
			return;

		if (--ck_gameState.health == 0)
		{
			CK_KillKeen();
			return;
		}

		dx = x - (ck_keenObj->clipRects.unitX1/2 + ck_keenObj->clipRects.unitX2/2);

		CK_SetAction2(ck_keenObj, &CK_ACT_KeenKnockback1);
		SD_PlaySound(10); // TODO: need a sound!
		ck_keenObj->velX = dx > 0 ? -20 : 20;
		ck_keenObj->velY = -40;
		ck_mapState.invincibilityTimer =  INT_MAX - 1;  // Set again when keen hits the ground
	}
	
}

void CK_KeenKnockbackReact(CK_object *obj)
{
	// Change stance when contacted the ground
	if (obj->topTI)
	{
		ck_mapState.invincibilityTimer = 70;
		CK_SetAction2(obj, &CK_ACT_KeenKnockbackRecover1);
	}

	if (obj->leftTI || obj->rightTI)
		obj->velX = 0;


	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
	
}
*/

#define STUN_COOLDOWN	(2*TickBase)
#define RESTUN_DELAY	(TickBase/5)

void CK_StunKeen(int midx)
{
	// make sure Keen isn't dead:
	// (stunning a dead Keen would revive him in a glitched state!)
	if (!CK_KeenIsDead() && !ck_mapState.invincibilityTimer && !ck_gameState.godMode)
	{
		// play a sound here (if Keen isn't already stunned)
		if ( (ck_keenObj->currentAction != &CK_ACT_keenStunned1
			&& ck_keenObj->currentAction != &CK_ACT_keenStunned2
			&& ck_keenObj->currentAction != &CK_ACT_keenStunned3)
		    || 	ck_mapState.stunCooldown <= STUN_COOLDOWN-RESTUN_DELAY )
		{
			SD_PlaySound(KEENSTUNNEDSND);
			//ck_keenObj->user2 = (int)(lasttimecount);	// remember last time Keen was stunned
			ck_mapState.stunCooldown = STUN_COOLDOWN;

			if (++ck_mapState.stunCounter > 3)
			{
				CK_KillKeen();
				return;
			}
		}

		if (US_RndT() < 128)
			CK_SetAction2(ck_keenObj, &CK_ACT_keenStunned1);
		else
			CK_SetAction2(ck_keenObj, &CK_ACT_keenStunned2);

		// make Keen hop to the side:
		ck_keenObj->velX = midx > ck_keenObj->clipRects.unitXmid? -24 : 24;
		ck_keenObj->velY = -40;
		// avoid getting stuck on a platform:
		ck_keenState.platform = NULL;
		ck_keenObj->topTI = 0;	// so CK_KeenStunnedReact won't make Keen land immediately after this, effectively stopping any movement
		ck_keenObj->clipped = CLIP_normal;	// Keen might be in noclip-mode (climbing poles, grabbing edges), so we have to disable noclip here

		RemoveDreamBubbles();	// because Keen might have been dreaming
		CK_DropNisasi();	// TODO: do we want this?
	}
}

void CK_KeenStunnedRecover(CK_object *obj)
{
	obj->user1 = obj->user2 = 0;	//Idle Time + Idle State
//	obj->user2 = 0;	// reset laststuntime
}

void CK_KeenStunnedReact(CK_object *obj)
{
	if (obj->leftTI || obj->rightTI)
	{
		obj->velX = 0;
	}
	if (obj->bottomTI)
			obj->velY = 0;

	if (obj->topTI)
	{
		obj->yDirection = motion_Down;	// K1n9_Duk3 fix: avoid door glitch
		obj->velX = 0;
		obj->velY = 0;
		CK_SetAction2(ck_keenObj, ck_keenObj->currentAction->next);
	}

	RF_PlaceSprite(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, spritedraw, obj->zLayer);
}

boolean CK_KeenIsDead(void)
{
	register CK_action *action = ck_keenObj->currentAction;

	return (action == &CK_ACT_keenDie0
		|| action == &CK_ACT_keenDie1
		|| action == &CK_ACT_keenDrown
		|| action == &CK_ACT_keenBurn
		|| action == &CK_ACT_keenSink);
	// TODO: add any other death states in here!
}


void CK_DrownKeen(int tileY)
{
	register int i;
	register CK_object *ob = ck_keenObj;
	unsigned oldx = ob->posX, oldy = ob->posY;

	if (ck_gameState.godMode/* || ck_mapState.invincibilityTimer*/)
		return;

	RemoveDreamBubbles();	// just in case...
	CK_DropNisasi();

	CK_PhysUpdateY(ob, RF_TileToUnit(tileY)-ob->clipRects.unitY2);
	CK_PhysUpdateX(ob, -TILEGLOBAL);
	ob->clipped = CLIP_not;
	ob->gfxChunk = NESSI1SPR;
	CK_SetAction2(ob, &CK_ACT_keenDrown);
	if (!CK_NotStuckInWall(ob))
	{
		CK_PhysUpdateX(ob, -(ob->clipRects.unitX1 & 0xFF));
		for (i=0; i<=2*TILEGLOBAL; i+=TILEGLOBAL)
		{
			CK_PhysUpdateX(ob, -i);
			if (CK_NotStuckInWall(ob))
				goto okay;
			CK_PhysUpdateX(ob, 2*i);
			if (CK_NotStuckInWall(ob))
				goto okay;
			CK_PhysUpdateX(ob, -i);
		}
die:
		// didn't find a good position for the Nessi sprite, so just use a normal death
		ob->posX = oldx;
		ob->posY = oldy;
		CK_ResetClipRects(ob);
		CK_KillKeen();
		return;
	}
okay:
#if 1
	// check if there's enough headroom for Nessi to rise
	CK_PhysUpdateY(ob, -3*TILEGLOBAL);
	if (!CK_NotStuckInWall(ob))
		goto die;
	CK_PhysUpdateY(ob, 3*TILEGLOBAL);
#endif
	SD_PlaySound(DEADTHNESSISND);
	ob->type = CT_Friendly;	// to make sure other objects don't interact with Keen
	ob->zLayer = 0;
	ck_scrollDisabled = true;
	ob->user1 = ob->user2 = ob->user3 = 0;
	ob->user4 = RF_TileToUnit(tileY-2);
	ob->timeUntillThink = TickBase;
}

void CK_KeenDrownThink(CK_object *ob)
{
#define NESSI_ANIM_DELAY 18
	if (!ob->user1)
	{
		// Nessi is rising
		ck_nextY = -10*tics;
		if (ob->clipRects.unitY2 + ck_nextY <= ob->user4)
		{
			ob->user1++;
			ck_nextY = ob->user4 - ob->clipRects.unitY2;
		}
	}
	if (ob->user1 || ob->clipRects.unitY2 < ob->user4 + (unsigned)(1.5*TILEGLOBAL))
	{
		// animate Nessi
		if ((ob->actionTimer += tics) >= NESSI_ANIM_DELAY)
		{
			if (ob->gfxChunk == NESSI4SPR)
			{
				if (ob->actionTimer >= (3*NESSI_ANIM_DELAY)/2 && ob->user1)
					ck_nextY = 8*tics;
				if (ob->actionTimer >= NESSI_ANIM_DELAY+TickBase/2)
					ck_levelState = LS_Died;
			}
			else
			{
				ob->actionTimer -= NESSI_ANIM_DELAY;
				UncacheDreamSprite(ob);
				ob->gfxChunk++;
			}
		}
	}
#undef NESSI_ANIM_DELAY
}

CK_action CK_ACT_keenDrown = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CK_KeenDrownThink, NULL, CK_DreamReact, NULL};

void CK_BurnKeen(boolean force)
{
	CK_object *ob = ck_keenObj;

	if (ck_gameState.godMode || (ck_mapState.invincibilityTimer && !force)
		|| ob->currentAction == &CK_ACT_keenBurn)
		return;

	if (ob->currentAction == &CK_ACT_keenDie0 || ob->currentAction == &CK_ACT_keenDie1)
	{
		CK_KillKeen();
		return;
	}

	RemoveDreamBubbles();	// just in case...
	CK_DropNisasi();

	SD_PlaySound(KEENDIESND);	//TODO? unique sound
	ob->type = CT_Friendly;	// to make sure other objects don't interact with Keen
	ck_scrollDisabled = true;
	ob->gfxChunk = KEENBURNED1SPR;
	ob->velX=ob->velY=0;
	CK_SetAction2(ob, &CK_ACT_keenBurn);
//	ob->timeUntillThink = 40;
}

void CK_KeenBurnThink(CK_object *ob)
{
	if ((ob->actionTimer += tics) >= 6)
	{
		if (ob->gfxChunk == KEENDIE2SPR)
		{
			ob->gfxChunk = KEENBURNED1SPR;
		}
		if (ob->gfxChunk == KEENBURNED8SPR)
		{
			CK_Fall(ob);
			if (ob->actionTimer >= 100)
				ck_levelState = LS_Died;
		}
		else
		{
			ob->actionTimer -= 6;
			UncacheDreamSprite(ob);
			ob->gfxChunk++;
		}
	}
}

void CK_KeenBurnReact(CK_object *ob)
{
	if (ob->topTI && ob->velY > 0)
	{
		ob->velY = -(ob->velY/2);
	}
	CK_DreamReact(ob);
}

CK_action CK_ACT_keenBurn = {0, 0, AT_Frame, false, push_none, 0, 0, 0, CK_KeenBurnThink, NULL, CK_KeenBurnReact, NULL};

void CK_KeenSink(CK_object *obj)
{
	register int speed = tics*3;

	ck_nextY = speed;
	if ((obj->user2 += speed) >= 18*PIXGLOBAL)
	{
		ck_levelState = LS_Died;
	}
	if (obj->user2 >= 10*PIXGLOBAL && obj->user2 - speed < 10*PIXGLOBAL)
	{
		SD_PlaySound(KEENDIESND);
	}
}

#endif
