//
// Generated by MakeStr - DO NOT MODIFY!
//
#ifndef __GAMESTR_H__
#define __GAMESTR_H__

typedef enum {
	S_nil,
	S_LOADGAME_NOMEM,
	S_YOUDIDNTMAKEITPAST,
	S_TRYAGAIN,
	S_RESTARTFROMCHECKPOINT,
	S_EXITTOWORLDMAP,
	S_CANTLOADCKECKPOINT,
	S_GAMEOVER,
	S_RANOUTOFLIVES,
	S_OUTOFLIVES1,
	S_OUTOFLIVES2,
	S_OUTOFLIVES3,
	S_OUTOFLIVES4,
	S_RANOUTOFJOKES,
	S_CONTINUE,
	S_QUIT,
	S_CONTINUEJOKE1,
	S_CONTINUEJOKE2,
	S_CONTINUEJOKE0,
	S_CONTINUEJOKE2A,
	S_CONTINUEJOKE2B,
	S_CONTINUEJOKE2C,
	S_CONTINUEJOKE3A,
	S_CONTINUEJOKE3B,
	S_CONTINUEJOKE4A,
	S_CONTINUEJOKE4B,
	S_CONTINUEJOKE5A,
	S_CONTINUEJOKE5B,
	S_CONTINUEJOKE5C,
	S_CONTINUEJOKE6A,
	S_CONTINUEJOKE6B,
	S_CONTINUEJOKE6C,
	S_CONTINUEJOKE7A,
	S_CONTINUEJOKE7B,
	S_CONTINUEJOKE7C,
	S_CONTINUEJOKE8A,
	S_CONTINUEJOKE8B,
	S_CONTINUEJOKE8C,
	S_CONTINUEJOKE9A,
	S_CONTINUEJOKE9B,
	S_CONTINUEJOKE9C,
	S_CONTINUEJOKE10A,
	S_CONTINUEJOKE10B,
	S_CONTINUEJOKE10C,
	S_CONTINUEJOKE10D,
	S_CONTINUEJOKE10E,
	S_CONTINUEJOKE10F,
	S_CONTINUEJOKE10G,
	S_CONTINUEJOKE10H,
	S_CONTINUEJOKE10I,
	S_CONTINUEJOKE10Q,
	S_CONTINUEJOKE11A,
	S_CONTINUEJOKE11B,
	S_FINALCONTINUEJOKE,
	S_DEMO,
	S_RECORDDEMO,
	S_DEMOFILE,
	S_SAVEDEMO,
	S_CANNOTWRITEDEMO,
	S_LOADLEVEL_NOMEM,
	S_LOADWORDLMAP_NOMEM,
	S_ONEMOMENT,
	S_LOCATION,
	S_SCORE,
	S_EXTRA,
	S_LEVEL,
	S_CHEATER,
	S_EASY,
	S_NORMAL,
	S_HARD,
	S_KEYS,
	S_AMMO,
	S_KEENS,
	S_DROPS,
	S_GOODIES,
	S_ITEMSDETECTED,
	S_POINTS,
	S_LIVES,
	S_DEBUGKEYSACTIVE,
	S_ACTIVEOBJECTS,
	S_INACTIVEOBJECTS,
	S_OBJECTSTOTAL,
	S_SPRITESUSED,
	S_SPRITESFREE,
	S_MEMUSAGE,
	S_MEMTOTAL,
	S_MEMFREE,
	S_WITHPURGE,
	S_MEMUSED,
	S_BLOCKSFREE,
	S_FREEITEMS,
	S_WORKING,
	S_WALLDEBUGEASTWALL,
	S_WALLDEBUGWESTWALL,
	S_MAPHEADFILE,
	S_SPRITETEST,
	S_SPRITETESTSTUFF,
	S_SHIFT,
	S_NEWLINE,
	S_SOUNDTEST,
	S_SOUNDNUM,
	S_BORDERCOLOR,
	S_GODMODE_OFF,
	S_GODMODE_ON,
	S_JUMPCHEAT_ON,
	S_JUMPCHEAT_OFF,
	S_NOCLIP_ON,
	S_NOCLIP_OFF,
	S_SLOMO_ON,
	S_SLOMO_OFF,
	S_EXTRAVBLS,
	S_LEVELWARP,
	S_PAUSED,
	S_OBJARRAYFULL,
	S_REMOVEPLAYER,
	S_BADMUSIC,
	S_MUSIC_NOMEM,
	S_BADKEYHOLDERSPOT,
	S_BADSHOTDIR,
	S_HELP1,
	S_HELP2,
	S_HELP3,
	S_HELPTEXT1,
	S_HELPTEXT2,
	S_HELPTEXT3,
	S_HELPTEXT4,
	S_HELPTEXT5,
	S_HELPTEXT6,
	S_HELPTEXT7,
	S_HELPTEXT8,
	S_HELPTEXT9,
	S_HELPTEXT9A,
	S_HELPTEXT10,
	S_HELPTEXT11,
	S_HELPTEXT12,
	S_HELPTEXT13,
	S_HELPTEXT14,
	S_HELPTEXT15,
	S_HELPTEXT16,
	S_HELPTEXTEND,
	S_NOTENOUGHMEMORY1,
	S_NOTENOUGHMEMORY2,
	S_NOTENOUGHMEMORY3,
	S_NOTENOUGHMEMORY4,
	S_NOTENOUGHMEMORY5,
	S_NOTENOUGHMEMORYEND,
	S_DEMOLOOPEXITED,
	S_BADCLOUDSTART,
	S_BADCLOUDEND,
	S_LEVELNAME0,
	S_LEVELNAME1,
	S_LEVELNAME2,
	S_LEVELNAME3,
	S_LEVELNAME4,
	S_LEVELNAME5,
	S_LEVELNAME6,
	S_LEVELNAME7,
	S_LEVELNAME8,
	S_LEVELNAME9,
	S_LEVELNAME10,
	S_LEVELNAME11,
	S_LEVELNAME12,
	S_LEVELNAME13,
	S_LEVELNAME14,
	S_LEVELNAME15,
	S_LEVELNAME16,
	S_LEVELNAME17,
	S_LEVELNAME18,
	S_LEVELNAME19,
	S_LEVELNAME20,
	S_LEVELNAME21,
	S_LEVELNAME22,
	S_LEVELNAME23,
	S_LEVELNAME24,
	S_LEVELNAME25,
	S_LEVELTEXT0,
	S_LEVELTEXT1,
	S_LEVELTEXT2,
	S_LEVELTEXT3,
	S_LEVELTEXT4,
	S_LEVELTEXT5,
	S_LEVELTEXT6,
	S_LEVELTEXT7,
	S_LEVELTEXT8,
	S_LEVELTEXT9,
	S_LEVELTEXT10,
	S_LEVELTEXT11,
	S_LEVELTEXT12,
	S_LEVELTEXT13,
	S_LEVELTEXT14,
	S_LEVELTEXT15,
	S_LEVELTEXT16,
	S_LEVELTEXT17,
	S_LEVELTEXT18,
	S_LEVELTEXT19,
	S_LEVELTEXT20,
	S_LEVELTEXT21,
	S_LEVELTEXT22,
	S_LEVELTEXT23,
	S_LEVELTEXT24,
	S_LEVELTEXT25,
	S_LEVELTEXTAPPROACH,
	S_LEVELTEXTLAND,
	S_LEVELTEXTBONUSDONE,
	S_JANITORMESSAGE0,
	S_JANITORMESSAGE1,
	S_JANITORMESSAGE2,
	S_JANITORMESSAGE3,
	S_JANITORMESSAGE4,
	S_JANITORMESSAGE5,
	S_JANITORMESSAGE6,
	S_JANITORMESSAGE7,
	S_JANITORMESSAGE8,
	S_JANITORMESSAGE9,
	S_JANITORMESSAGE10,
	S_JANITORMESSAGE11,
	S_JANITORMESSAGE12,
	S_JANITORMESSAGE18,
	S_JANITORMESSAGE19,
	S_JANITORMESSAGE13,
	S_JANITORMESSAGE14,
	S_JANITORMESSAGE20,
	S_JANITORMESSAGE15,
	S_JANITORMESSAGE16,
	S_JANITORMESSAGE17,
	S_JANITORREPLY0,
	S_JANITORREPLY1,
	S_JANITORREPLY2,
	S_JANITORREPLY3,
	S_JANITORREPLY4,
	S_JANITORREPLY5,
	S_JANITORREPLY6,
	S_JANITORREPLY7,
	S_JANITORREPLY8,
	S_JANITORREPLY9,
	S_JANITORREPLY10,
	S_JANITORREPLY15,
	S_JANITORREPLY11,
	S_JANITORREPLY12,
	S_JANITORREPLY13,
	S_JANITORREPLY14,
	S_JANITORJOKE0,
	S_JANITORJOKE1,
	S_JANITORJOKE2,
	S_JANITORJOKE3,
	S_JANITORJOKE4,
	S_JANITORJOKE5,
	S_JANITORJOKE6,
	S_JANITORJOKE7,
	S_JANITORJOKE8,
	S_JANITORJOKE9,
	S_JANITORJOKE10,
	S_JANITORMONOLOG0,
	S_JANITORMONOLOG1,
	S_JANITORMONOLOG2,
	S_JANITORMONOLOG3,
	S_JANITORMONOLOG4,
	S_JANITORMONOLOG5,
	S_JANITORMONOLOGEND,
	S_JANITORSTUNNEDMSG0,
	S_JANITORNISASIMESSAGE0,
	S_JANITORNISASIREPLY0,
	S_LINDSEYMESSAGE0,
	S_LINDSEYMESSAGE1,
	S_LINDSEYMESSAGE2,
	S_LINDSEYMESSAGE3,
	S_LINDSEYREPLY0,
	S_LINDSEYREPLY1,
	S_LINDSEYREPLY2,
	S_HALMESSAGE1,
	S_HALREPLY1,
	S_HALMESSAGE2,
	S_HALREPLY2,
	S_SPACEMANSPIFFMESSAGE1,
	S_SPACEMANSPIFFMESSAGE2,
	S_SPACEMANSPIFFMESSAGE3,
	S_INSPACE,
	S_INSERTCOIN,
	S_MISCMESSAGE4,
	S_MISCMESSAGE5,
	S_MISCMESSAGE6,
	S_MISCMESSAGE7,
	S_MISCMESSAGE8,
	S_MISCMESSAGE9,
	S_MISCMESSAGE10,
	S_MISCMESSAGE11,
	S_MISCMESSAGE12,
	S_MISCMESSAGE13,
	S_MISCMESSAGE14,
	S_MISCMESSAGE15,
	S_MISCMESSAGE16,
	S_MISCMESSAGE17,
	S_MISCMESSAGE18,
	S_MISCMESSAGE19,
	S_MISCMESSAGE20,
	S_MISCMESSAGE21,
	S_MISCMESSAGE22,
	S_MISCMESSAGE23,
	S_MISCMESSAGE24,
	S_MISCMESSAGE25,
	S_MISCMESSAGE26,
	S_MISCMESSAGE27,
	S_MISCMESSAGE28,
	S_MISCMESSAGE29,
	S_MISCMESSAGE30,
	S_MISCMESSAGEEND,
	S_RADIOMESSAGE1,
	S_JUKEBOXTOP,
	S_JUKEBOXBOTTOM,
	S_MUSICNAME0,
	S_MUSICNAME1,
	S_MUSICNAME2,
	S_MUSICNAME3,
	S_MUSICNAME4,
	S_MUSICNAME5,
	S_MUSICNAME6,
	S_MUSICNAME7,
	S_MUSICNAME8,
	S_MUSICNAME9,
	S_MUSICNAME10,
	S_MUSICNAME11,
	S_MUSICNAME12,
	S_MUSICNAME13,
	S_MUSICNAME14,
	S_MUSICNAME15,
	S_MUSICNAME16,
	S_MUSICNAME17,
	S_MUSICNAME18,
	S_MUSICNAME19,
	S_MUSICNAME20,
	S_MUSICNAME21,
	S_MUSICNAME22,
	S_MUSICNAME23,
	S_MUSICNAME24,
	S_MUSICNAME25,
	S_MUSICNAME26,
	S_MUSICNAME27,
	S_MUSICNAME28,
	S_MUSICNAME29,
	S_MUSICNAME30,
	S_MUSICNAME31,
	S_MUSICNAME32,
	S_MUSICNAME33,
	S_MUSICNAME34,
	S_MUSICNAME35,
	S_MUSICNAME36,
	S_MUSICNAME37,
	S_MUSICNAME38,
	S_BONUSLEVELINFO1,
	S_BONUSLEVELINFO2,
	S_BADSTATE,
	S_DOORTOOTALL,
	S_BADTURNTYPE,
	S_BADOBCLASS_CUSTOM,
	S_WORDLIMITEXCEEDED,
	S_BADTEXTSTART,
	S_BADTEXTEND,
	S_PAGE,
	S_OF,
	S_BADSTARWARSLINE,
	S_BADSTARWARSEND,
	S_CANNOTOPEN,
	S_DEBUGFILE,
	S_CA_FARREAD,
	S_CA_FARWRITE,
	S_BADSHIFTS,
	S_BADMAP,
	S_CA_UPLEVEL,
	S_CA_DOWNLEVEL,
	S_NOJOYS,
	S_NOMOUSE,
	S_DEMOPLAYBACKEXCEEDED,
	S_DEMOBUFFEROVERFLOW,
	S_KEYNAME0,
	S_KEYNAME1,
	S_KEYNAME2,
	S_KEYNAME3,
	S_KEYNAME4,
	S_KEYNAME5,
	S_KEYNAME6,
	S_KEYNAME7,
	S_KEYNAME8,
	S_KEYNAME9,
	S_KEYNAME10,
	S_KEYNAME11,
	S_KEYNAME12,
	S_KEYNAME13,
	S_KEYNAME14,
	S_KEYNAME15,
	S_KEYNAME16,
	S_KEYNAME17,
	S_KEYNAME18,
	S_KEYNAME19,
	S_KEYNAME20,
	S_KEYNAME21,
	S_KEYNAME22,
	S_KEYNAME23,
	S_KEYNAME24,
	S_KEYNAME25,
	S_KEYNAME26,
	S_KEYNAME27,
	S_KEYNAME28,
	S_KEYNAME29,
	S_KEYNAME30,
	S_KEYNAME31,
	S_KEYNAME32,
	S_KEYNAME33,
	S_KEYNAME34,
	S_KEYNAME35,
	S_KEYNAME36,
	S_KEYNAME37,
	S_KEYNAME38,
	S_KEYNAME39,
	S_KEYNAME40,
	S_KEYNAME41,
	S_KEYNAME42,
	S_KEYNAME43,
	S_KEYNAME44,
	S_KEYNAME45,
	S_KEYNAME46,
	S_KEYNAME47,
	S_KEYNAME48,
	S_KEYNAME49,
	S_KEYNAME50,
	S_KEYNAME51,
	S_KEYNAME52,
	S_KEYNAME53,
	S_KEYNAME54,
	S_KEYNAME55,
	S_KEYNAME56,
	S_KEYNAME57,
	S_KEYNAME58,
	S_KEYNAME59,
	S_KEYNAME60,
	S_KEYNAME61,
	S_KEYNAME62,
	S_KEYNAME63,
	S_KEYNAME64,
	S_KEYNAME65,
	S_KEYNAME66,
	S_KEYNAME67,
	S_KEYNAME68,
	S_KEYNAME69,
	S_KEYNAME70,
	S_KEYNAME71,
	S_KEYNAME72,
	S_KEYNAME73,
	S_KEYNAME74,
	S_KEYNAME75,
	S_KEYNAME76,
	S_KEYNAME77,
	S_KEYNAME78,
	S_KEYNAME79,
	S_KEYNAME80,
	S_KEYNAME81,
	S_KEYNAME82,
	S_KEYNAME83,
	S_KEYNAME84,
	S_KEYNAME85,
	S_KEYNAME86,
	S_KEYNAME87,
	S_KEYNAME88,
	S_LASTKEYNAME,
	S_NOEMS,
	S_NOXMS,
	S_NOUMB,
	S_SETUPEMSERROR,
	S_FREEEMSERROR,
	S_MAPEMSERROR,
	S_SEGMENTSPANSBLOCKS,
	S_NOPURGABLEBLOCKS,
	S_OUTOFMEMORY,
	S_FREEPTR_BLOCKNOTFOUND,
	S_GETSIZE_BLOCKNOTFOUND,
	S_SETPURGE_BLOCKNOTFOUND,
	S_SETLOCK_BLOCKNOTFOUND,
	S_SETLOCK_BADTYPE,
	S_BLOCKORDERCORRUPTED,
	S_MAPTOOTALL,
	S_BADBACKANIM,
	S_TOOMANYANIMS,
	S_UNENDINGBACKANIM,
	S_BADFOREANIM,
	S_UNENDINGFOREANIM,
	S_NOFREETILESPOTS,
	S_TILEOUTOFBOUNDS,
	S_TOOMANYHSCROLLBLOCKS,
	S_TOOMANYVSCROLLBLOCKS,
	S_BADNEWROWDIR,
	S_NOFREESPRITESPOTS,
	S_UNCACHEDSPRITE,
	S_NOAL,
	S_ADLIB,
	S_OPLPORT,
	S_BADINSTRUMENT,
	S_UNCACHEDSOUND,
	S_ZEROLENGTHSOUND,
	S_COMP,
	S_NOCOMP,
	S_JERK,
	S_TEDLEVEL,
	S_NOWAIT,
	S_BADTEDLEVEL,
	S_EMPTY,
	S_SVGACOMPENABLED,
	S_STARTUP_LOADING,
	S_STARTUP_READY,
	S_STRINGEXCEEDSWIDTH,
	S_DEBUG,
	S_NOSOUNDEFFECTS,
	S_PCSPEAKER,
	S_ADLIBSOUNDBLASTER,
	S_NOMUSIC,
	S_BEGINEASYGAME,
	S_BEGINNORMALGAME,
	S_BEGINHARDGAME,
	S_SCOREBOX,
	S_SVGACOMPATIBILITY,
	S_FIXJERKYMOTION,
	S_TWOBUTTONFIRING,
	S_MAXIMUMFRAMERATE,
	S_PAYTOSAVE,
	S_QUICKLOAD,
	S_QUICKSAVE,
	S_UPLEFT,
	S_UP,
	S_UPRIGHT,
	S_RIGHT,
	S_DOWNRIGHT,
	S_DOWN,
	S_DOWNLEFT,
	S_LEFT,
	S_JUMP,
	S_POGO,
	S_FIRE,
	S_STATUS,
	S_DETECTOR,
	S_TOGGLESCOREBOX,
	S_SAVEMENU,
	S_LODAMENU,
	S_MOVEMENT,
	S_ACTIONBUTTONS,
	S_FUNCTIONBUTTONS,
	S_SOUND,
	S_MUSIC,
	S_OPTIONS,
	S_USEKEYBOARD,
	S_USEJOYSTICK1,
	S_USEJOYSTICK2,
	S_NEWGAME,
	S_LOADGAME,
	S_SAVEGAME,
	S_PADDLEWAR,
	S_HELPGAMEINFO,
	S_CONFIGURE,
	S_QUIT1,
	S_EMPTYCOMM,
	S_ESCTOBACKOUT,
	S_REALLYENDCURRENTGAME,
	S_PRESSYTOENDIT,
	S_REALLYQUIT,
	S_PRESSYTOQUIT,
	S_YOUREINAGAME,
	S_PRESSYTOLOADGAME,
	S_SAVINGWILLCOSTALIFE,
	S_PRESSYTOSAVE,
	S_PRESSYFORNEWGAME,
	S_ON,
	S_OFF,
	S_AUTO,
	S_CONFIRM,
	S_JOKEKEYNAME1,
	S_JOKEKEYNAME2,
	S_JOKEKEYNAME3,
	S_JOKEKEYNAME4,
	S_JOKEKEYNAME5,
	S_JOKEKEYNAME6,
	S_JOKEKEYNAME7,
	S_JOKEKEYNAME8,
	S_JOKEKEYNAME9,
	S_JOKEKEYNAME10,
	S_JOKEKEYNAME11,
	S_JOKEKEYNAME12,
	S_JOKEKEYNAME13,
	S_JOKEKEYNAME14,
	S_JOKEKEYNAMEEND,
	S_FRAMERATE1,
	S_FRAMERATE2,
	S_FRAMERATE3,
	S_FRAMERATE4,
	S_FRAMERATE5,
	S_CONTROL,
	S_CONTROLNAME0,
	S_KEYBOARD,
	S_JOYSTICK1,
	S_JOYSTICK2,
	S_MOUSE,
	S_KEYALREADYUSED,
	S_PRESSAKEY,
	S_CALIBMIN1,
	S_CALIBMIN2,
	S_CALIBMAX1,
	S_CALIBMAX2,
	S_CALIBFAILED,
	S_USINGJOY1,
	S_USINGJOY2,
	S_PRESSANYKEY,
	S_LOADING,
	S_SAVING,
	S_TYPENAME,
	S_ENTERACCEPTS,
	S_ARROWSMOVE,
	S_ENTERSELECTS,
	S_ESCBACK,
	S_ESCQUIT,
	S_QUICKLOADTOP,
	S_QUICKSAVETOP,
	S_QUICKLOADBOT,
	S_QUICKSAVEBOT,
	S_HINTDISABLECONFIRM,
	S_PWKEEN,
	S_PWCOMP,
	S_PWWON,
	S_PWLOST,
	S_NNSCORE,
	S_NNBEST,
	S_ERROR0,
	S_ERROR1,
	S_ERROR2,
	S_ERROR3,
	S_ERROR4,
	S_ERROR5,
	S_ERROR6,
	S_ERROR7,
	S_ERROR8,
	S_ERROR9,
	S_ERROR10,
	S_ERROR11,
	S_ERROR12,
	S_ERROR13,
	S_ERROR14,
	S_ERROR15,
	S_ERROR16,
	S_ERROR17,
	S_ERROR18,
	S_ERROR19,
	S_ERROR20,
	S_ERROR21,
	S_ERROR22,
	S_ERROR23,
	S_ERROR24,
	S_ERROR25,
	S_ERROR26,
	S_ERROR27,
	S_ERROR28,
	S_ERROR29,
	S_ERROR30,
	S_ERROR31,
	S_ERROR32,
	S_ERROR33,
	S_ERROR34,
	S_ERROR35,
	S_ERROR36,
	S_ERRORDISKFULL,
	S_ERRORINCOMPLETE,
	S_NILCARD,
	S_RETURNTOGAME,
	S_RETURNTODEMO,
	S_VIEWSCORES,
	S_ENDGAME,
	S_CONTROLPANEL,
	S_UNKNOWNCOMM,
	S_QUITTING,
	S_HIDDENCARD,
	S_NOBORDER,
	S_VGA,
	S_IMPROPERCARD,
	NUMGAMESTRINGS
} stringnames;

#define MAXGAMESTRINGLENGTH 561

#endif