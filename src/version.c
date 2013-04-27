/*************************************************************************
** NewWolf
** Copyright (C) 1999-2002 by DarkOne
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**************************************************************************
** wolfenstein version detection and remap tables construction
** TODO: move to scripted part
*************************************************************************/
#include "WolfDef.h"

// ------------------------- * local constants * -------------------------

char *file_audh="audiohed";	// audio header
char *file_audm="audiot";		// audio main
char *file_page="vswap";		// page file
char *file_vgah="vgahead";	// vga header
char *file_vgad="vgadict";	// vga dictionary
char *file_vgam="vgagraph";	// vga main
char *file_maph="maphead";	// map header
char *file_mapm="gamemaps";	// map main

typedef struct ver_def_s
{
	char *ext;
	char *desc;
} ver_def_t;

ver_def_t ver_def[5]=
{
	{"wl1", "Shareware version"},
	{"wl3", "3 episode version"},
	{"wl6", "Full Wolfenstein version"},
	{"sdm", "Spear Demo"},
	{"sod", "Spear of Destiny"}
};

// ------------------------- * globals * -------------------------
ver_info_t WolfVer;

// remap definitions between versions!
RGBcolor *WolfPal;

int *VGA_REMAP, *SPR_REMAP;
int *MUS_REMAP, *SND_REMAP;
int STARTPCSOUNDS, STARTADLIBSOUNDS, STARTDIGISOUNDS, STARTMUSIC;
int *wolfdigimap;
int *levelsongs;
statinfo_t *statinfo;
int num_statics;
int *celing_color;

// ------------------------- * constants * -------------------------

#define WL6_NUMSOUNDS					87
#define WL6_NUMSNDCHUNKS			288
#define WL6_STARTPCSOUNDS			0
#define WL6_STARTADLIBSOUNDS	87
#define WL6_STARTDIGISOUNDS		174
#define WL6_STARTMUSIC				261

#define SOD_NUMSOUNDS					81
#define SOD_NUMSNDCHUNKS			267
#define SOD_STARTPCSOUNDS			0
#define SOD_STARTADLIBSOUNDS	81
#define SOD_STARTDIGISOUNDS		162
#define SOD_STARTMUSIC				243

// ------------------------- * remap tables * -------------------------

static int vga_wl6_14[]=
{// WL6 v1.4
	 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,
	23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,
	43,44,45,46,47,48,49,50,51,52,
	53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,
	73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,
	93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,
	113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,
	133,134,    136,137,138,139,140,141,142,143,144,145,146,147,148
},
vga_wl6_11[]=
{// WL6 v1.1 /* default is 0 */
	9, 15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,
	34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51, 0, 0,
	52,53,54,55,56,57,58,59,60,61,
	62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,
	82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,
	102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,
	122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,
	142,143,    145,146,147,148,149,150,151,152,153,154,155,156,157,
	3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 14
},
vga_wl1[]=
{// WL1 v1.4
	3, 4, 11, 17, 18, 19, 20,22,
	23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,
	43,44,45,46,47,48,49,50,51,52,
	53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,
	73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,
	93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,
	113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,
	133,134,135,136,137,138,139,140,141,142,143,144,145,146,    148,149,150,151,152,
	153,154,155,  3,3,3,3,3,  5,6,7,8,9,10,11,12,13,14,15,16,21
},
vga_sdm[]=
{// SDM /default is 4/
	4,4,4,4,4,4,4,16,5,6,7,8,17,18,19,4,21,22,23,24,4,25,26,15,9,27,28,4,4,4,4,4,4,
	4,4,4,4,4,10,11,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,
	51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,
	76,4,77,78,29,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,
	99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,
	119,4,126,127,129,130,4,132,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	3,12,13,14,20,30,74,75,120,121,122,123,124,125,131
},
vga_sod[]=
{// SOD /default is 4/
	4,4,4,4,4,4,4,16,5,6,7,8,17,18,19,4,21,22,23,24,4,25,26,15,9,27,28,4,4,4,4,4,4,
	4,4,4,4,4,10,11,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,
	51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,
	90,4,91,92,29,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,
	121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,
	4,148,149,151,152,4,164,165,166,167,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	3,12,13,14,20,30,79,80,142,143,144,145,146,147,153,
	31,32,33,34,35,81,82,83,84,85,86,87,88,89,93,94,95,96,97,98,99,100,
	154,155,156,157,158,159,160,161,162,163
};

static int spr_wolf[]=
{// wl1, wl6
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,

// Spear only
 	0,0,0,0,

	50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,
	70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,
	90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,
	110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,
	130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,
	150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,
	170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,
	190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,
	210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,
	230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,
	250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,
	270,271,272,273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,

// Wolf3D only!
	288,289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,307,
	308,309,310,311,312,313,314,315,316,317,318,319,320,321,322,323,324,325,326,327,
	328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,
	348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367,
	368,369,

	370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,

// Spear only
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

// Wolf3D only
	385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,401,402,403,404,
	405,406,407,408,409,410,411,412,413,414,415,

// Spear only
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

	416,417,418,419,420,421,422,423,424,425,426,427,428,429,430,431,432,433,434,435
},
spr_spear[]=
{// sod
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,

// Spear only
 	50,51,52,53,

	54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,
	74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,
	94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,
	114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,
	134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,
	154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,
	174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,
	194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,
	214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,
	234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,
	254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,
	274,275,276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,

// Wolf3D only!
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,

	292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,

// Spear only
	307,308,309,310,311,312,313,314,315,316,317,318,319,320,321,322,323,324,325,

//	Wolf3D only
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,

// Spear only
	326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,
	346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,
	366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,385,
	386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,
	
	401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,418,419,420
};

static int wl1_digimap[]=
{
	HALTSND,                0,
	DOGBARKSND,             1,
	CLOSEDOORSND,           2,
	OPENDOORSND,            3,
	ATKMACHINEGUNSND,       4,
	ATKPISTOLSND,           5,
	ATKGATLINGSND,          6,
	SCHUTZADSND,            7,
	GUTENTAGSND,            8,
	MUTTISND,               9,
	BOSSFIRESND,            10,
	SSFIRESND,              11,
	DEATHSCREAM1SND,        12,
	DEATHSCREAM2SND,        13,
	DEATHSCREAM3SND,        13,
	TAKEDAMAGESND,          14,
	PUSHWALLSND,            15,
	LEBENSND,               20,
	NAZIFIRESND,            21,
	SLURPIESND,             22,
	YEAHSND,								32,
	LASTSOUND
},
wl6_digimap[]=
{
	HALTSND,          0,
	DOGBARKSND,       1,
	CLOSEDOORSND,     2,
	OPENDOORSND,      3,
	ATKMACHINEGUNSND, 4,
	ATKPISTOLSND,     5,
	ATKGATLINGSND,    6,
	SCHUTZADSND,      7,
	GUTENTAGSND,      8,
	MUTTISND,         9,
	BOSSFIRESND,      10,
	SSFIRESND,        11,
	DEATHSCREAM1SND,  12,
	DEATHSCREAM2SND,  13,
	DEATHSCREAM3SND,  13,
	TAKEDAMAGESND,    14,
	PUSHWALLSND,      15,
	LEBENSND,         20,
	NAZIFIRESND,      21,
	SLURPIESND,       22,
	YEAHSND,					32,
	DOGDEATHSND,      16,
	AHHHGSND,         17,
	DIESND,           18,
	EVASND,           19,
	TOT_HUNDSND,      23,
	MEINGOTTSND,      24,
	SCHABBSHASND,     25,
	HITLERHASND,      26,
	SPIONSND,         27,
	NEINSOVASSND,     28,
	DOGATTACKSND,     29,
	LEVELDONESND,     30,
	MECHSTEPSND,			31,
	SCHEISTSND,				33,
	DEATHSCREAM4SND,	34,		// AIIEEE
	DEATHSCREAM5SND,	35,		// DEE-DEE
	DONNERSND,				36,		// EPISODE 4 BOSS DIE
	EINESND,					37,		// EPISODE 4 BOSS SIGHTING
	ERLAUBENSND,			38,		// EPISODE 6 BOSS SIGHTING
	DEATHSCREAM6SND,	39,		// FART
	DEATHSCREAM7SND,	40,		// GASP
	DEATHSCREAM8SND,	41,		// GUH-BOY!
	DEATHSCREAM9SND,	42,		// AH GEEZ!
	KEINSND,					43,		// EPISODE 5 BOSS SIGHTING
	MEINSND,					44,		// EPISODE 6 BOSS DIE
	ROSESND,					45,		// EPISODE 5 BOSS DIE
	LASTSOUND
},
sdm_digimap[]=
{

	HALTSND,            0,
	CLOSEDOORSND,       2,
	OPENDOORSND,        3,
	ATKMACHINEGUNSND,   4,
	ATKPISTOLSND,       5,
	ATKGATLINGSND,      6,
	SCHUTZADSND,        7,
	BOSSFIRESND,        8,
	SSFIRESND,          9,
	DEATHSCREAM1SND,    10,
	DEATHSCREAM2SND,    11,
	TAKEDAMAGESND,      12,
	PUSHWALLSND,        13,
	AHHHGSND,           15,
	LEBENSND,           16,
	NAZIFIRESND,        17,
	SLURPIESND,         18,
	LEVELDONESND,       22,
	DEATHSCREAM4SND,		23,		// AIIEEE
	DEATHSCREAM3SND,    23,		// DOUBLY-MAPPED!!!
	DEATHSCREAM5SND,		24,		// DEE-DEE
	DEATHSCREAM6SND,		25,		// FART
	DEATHSCREAM7SND,		26,		// GASP
	DEATHSCREAM8SND,		27,		// GUH-BOY!
	DEATHSCREAM9SND,		28,		// AH GEEZ!
	GETGATLINGSND,			38,		// Got Gat replacement
LASTSOUND
},
sod_digimap[]=
{
	HALTSND,            0,
	CLOSEDOORSND,       2,
	OPENDOORSND,        3,
	ATKMACHINEGUNSND,   4,
	ATKPISTOLSND,       5,
	ATKGATLINGSND,      6,
	SCHUTZADSND,        7,
	BOSSFIRESND,        8,
	SSFIRESND,          9,
	DEATHSCREAM1SND,    10,
	DEATHSCREAM2SND,    11,
	TAKEDAMAGESND,      12,
	PUSHWALLSND,        13,
	AHHHGSND,           15,
	LEBENSND,           16,
	NAZIFIRESND,        17,
	SLURPIESND,         18,
	LEVELDONESND,       22,
	DEATHSCREAM4SND,		23,		// AIIEEE
	DEATHSCREAM3SND,    23,		// DOUBLY-MAPPED!!!
	DEATHSCREAM5SND,		24,		// DEE-DEE
	DEATHSCREAM6SND,		25,		// FART
	DEATHSCREAM7SND,		26,		// GASP
	DEATHSCREAM8SND,		27,		// GUH-BOY!
	DEATHSCREAM9SND,		28,		// AH GEEZ!
	GETGATLINGSND,			38,		// Got Gat replacement
	DOGBARKSND,         1,
	DOGDEATHSND,        14,
	SPIONSND,           19,
	NEINSOVASSND,       20,
	DOGATTACKSND,       21,
	TRANSSIGHTSND,			29,		// Trans Sight
	TRANSDEATHSND,			30,		// Trans Death
	WILHELMSIGHTSND,		31,		// Wilhelm Sight
	WILHELMDEATHSND,		32,		// Wilhelm Death
	UBERDEATHSND,				33,		// Uber Death
	KNIGHTSIGHTSND,			34,		// Death Knight Sight
	KNIGHTDEATHSND,			35,		// Death Knight Death
	ANGELSIGHTSND,			36,		// Angel Sight
	ANGELDEATHSND,			37,		// Angel Death
	GETSPEARSND,				39,		// Got Spear replacement
	LASTSOUND
};

static int wl6_songs[]=
{
// Episode One
	GETTHEM_MUS,SEARCHN_MUS,POW_MUS,SUSPENSE_MUS,GETTHEM_MUS,
	SEARCHN_MUS,POW_MUS,SUSPENSE_MUS,WARMARCH_MUS,CORNER_MUS,
// Episode Two
	NAZI_OMI_MUS,PREGNANT_MUS,GOINGAFT_MUS,HEADACHE_MUS,NAZI_OMI_MUS,
	PREGNANT_MUS,HEADACHE_MUS,GOINGAFT_MUS,WARMARCH_MUS,DUNGEON_MUS,
// Episode Three
	INTROCW3_MUS,NAZI_RAP_MUS,TWELFTH_MUS,ZEROHOUR_MUS,INTROCW3_MUS,
	NAZI_RAP_MUS,TWELFTH_MUS,ZEROHOUR_MUS,ULTIMATE_MUS,PACMAN_MUS,
// Episode Four
	GETTHEM_MUS,SEARCHN_MUS,POW_MUS,SUSPENSE_MUS,GETTHEM_MUS,SEARCHN_MUS,
	POW_MUS,SUSPENSE_MUS,WARMARCH_MUS,CORNER_MUS,
	// Episode Five
	NAZI_OMI_MUS,PREGNANT_MUS,GOINGAFT_MUS,HEADACHE_MUS,NAZI_OMI_MUS,
	PREGNANT_MUS,HEADACHE_MUS,GOINGAFT_MUS,WARMARCH_MUS,DUNGEON_MUS,
	// Episode Six
	INTROCW3_MUS,NAZI_RAP_MUS,TWELFTH_MUS,ZEROHOUR_MUS,INTROCW3_MUS,
	NAZI_RAP_MUS,TWELFTH_MUS,ZEROHOUR_MUS,ULTIMATE_MUS,FUNKYOU_MUS
},
sod_songs[]=
{
	XTIPTOE_MUS,XFUNKIE_MUS,XDEATH_MUS,XGETYOU_MUS,ULTIMATE_MUS,
	DUNGEON_MUS,GOINGAFT_MUS,POW_MUS,TWELFTH_MUS,ULTIMATE_MUS,
	NAZI_OMI_MUS,GETTHEM_MUS,SUSPENSE_MUS,SEARCHN_MUS,ZEROHOUR_MUS,ULTIMATE_MUS,
	XPUTIT_MUS,ULTIMATE_MUS,XJAZNAZI_MUS,XFUNKIE_MUS,XEVIL_MUS
};

// Celing Colors:
static int celing_wl6[]=
{
	0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0xbf,
	0x4e, 0x4e, 0x4e, 0x1d, 0x8d, 0x4e, 0x1d, 0x2d, 0x1d, 0x8d,
	0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x2d, 0xdd, 0x1d, 0x1d, 0x98,

	0x1d,	0x9d,	0x2d,	0xdd,	0xdd,	0x9d,	0x2d,	0x4d,	0x1d,	0xdd,
	0x7d,	0x1d,	0x2d,	0x2d,	0xdd,	0xd7,	0x1d,	0x1d,	0x1d,	0x2d,
	0x1d,	0x1d,	0x1d,	0x1d,	0xdd,	0xdd,	0x7d,	0xdd,	0xdd,	0xdd
},
celing_sod[]=
{
	0x6f,	0x4f,	0x1d,	0xde,	0xdf,	0x2e,	0x7f,	0x9e,	0xae,	0x7f,
	0x1d,	0xde,	0xdf,	0xde,	0xdf,	0xde,	0xe1,	0xdc,	0x2e,	0x1d,	0xdc
};

static int mus_wl6[]=
{
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,0,0,0,0,0,0,0,0,0,0,0
},
mus_sod[]=
{
0,1,0,3,0,0,0,0,8,9,0,11,12,0,14,0,16,5,0,15,0,13,10,0,6,0,0,0,2,4,7,17,18,19,20,21,22,23
};

static int snd_wl6[]=
{
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,
60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
80,81,82,83,84,85,86,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
},
snd_sod[]=
{
0,0,2,0,4,5,6,7,0,9,10,11,12,13,14,15,16,17,18,19,20,21,
22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,
42,43,44,45,46,47,48,49,0,51,52,0,0,0,56,0,58,59,60,61,
0,0,0,0,66,67,68,0,0,0,0,50,53,57,54,55,63,0,0,0,0,0,0,
8,1,3,62,64,65,69,70,71,72,73,74,75,76,77,78,79,80
};

statinfo_t static_wl6[]=
{
	{false, -1},	// puddle          spr1v
	{ true, -1},	// Green Barrel    "
	{ true, -1},	// Table/chairs    "
	{ true, -1},	// Floor lamp      "
	{false, -1},	// Chandelier      "
	{ true, -1},	// Hanged man      "
	{false, pow_alpo},			// Bad food        "
	{ true, -1},	// Red pillar      "
	{ true, -1},	// Tree            spr2v
	{false, -1},	// Skeleton flat   "
	{ true, -1},	// Sink            " (SOD:gibs)
	{ true, -1},	// Potted plant    "
	{ true, -1},	// Urn             "
	{ true, -1},	// Bare table      "
	{false, -1},	// Ceiling light   "
	{false, -1},	// Kitchen stuff   "
	{ true, -1},	// suit of armor   spr3v
	{ true, -1},	// Hanging cage    "
	{ true, -1},	// SkeletoninCage  "
	{false, -1},	// Skeleton relax  "
	{false, pow_key1},// Key 1           "
	{false, pow_key2},// Key 2           "
	{ true, -1},	// stuff				(SOD:gibs)
	{false, -1},	// stuff
	{false, pow_food},			// Good food       spr4v
	{false, pow_firstaid},	// First aid       "
	{false, pow_clip},			// Clip            "
	{false, pow_machinegun},// Machine gun     "
	{false, pow_chaingun},	// Gatling gun     "
	{false, pow_cross},		// Cross           "
	{false, pow_chalice},	// Chalice         "
	{false, pow_bible},		// Bible           "
	{false, pow_crown},		// crown           spr5v
	{false, pow_fullheal},	// one up          "
	{false, pow_gibs},			// gibs            "
	{ true, -1},	// barrel          "
	{ true, -1},	// well            "
	{ true, -1},	// Empty well      "
	{false, pow_gibs},	// Gibs 2          "
	{ true, -1},	// flag				"
	{ true, -1},	// Call Apogee		spr7v
	{false, -1},	// junk            "
	{false, -1},	// junk 		   "
	{false, -1},	// junk            "
	{false, -1},	// pots            "
	{ true, -1},	// stove           " (SOD:gibs)
	{ true, -1},	// spears          " (SOD:gibs)
	{false, -1},	// vines			"
},
static_sod[]=
{
	{false, -1},	// puddle          spr1v
	{ true, -1},	// Green Barrel    "
	{ true, -1},	// Table/chairs    "
	{ true, -1},	// Floor lamp      "
	{false, -1},	// Chandelier      "
	{ true, -1},	// Hanged man      "
	{false, pow_alpo},			// Bad food        "
	{ true, -1},	// Red pillar      "
	{ true, -1},	// Tree            spr2v
	{false, -1},	// Skeleton flat   "
	{ true, -1},	// Sink            " (SOD:gibs)
	{ true, -1},	// Potted plant    "
	{ true, -1},	// Urn             "
	{ true, -1},	// Bare table      "
	{false, -1},	// Ceiling light   "
	{ true, -1},	// Gibs!
	{ true, -1},	// suit of armor   spr3v
	{ true, -1},	// Hanging cage    "
	{ true, -1},	// SkeletoninCage  "
	{false, -1},	// Skeleton relax  "
	{false, pow_key1},// Key 1           "
	{false, pow_key2},// Key 2           "
	{ true, -1},	// stuff				(SOD:gibs)
	{false, -1},	// stuff
	{false, pow_food},			// Good food       spr4v
	{false, pow_firstaid},	// First aid       "
	{false, pow_clip},			// Clip            "
	{false, pow_machinegun},// Machine gun     "
	{false, pow_chaingun},	// Gatling gun     "
	{false, pow_cross},		// Cross           "
	{false, pow_chalice},	// Chalice         "
	{false, pow_bible},		// Bible           "
	{false, pow_crown},		// crown           spr5v
	{false, pow_fullheal},	// one up          "
	{false, pow_gibs},			// gibs            "
	{ true, -1},	// barrel          "
	{ true, -1},	// well            "
	{ true, -1},	// Empty well      "
	{false, pow_gibs},	// Gibs 2          "
	{ true, -1},	// flag				"
	{false, -1},	// Red light
	{false, -1},	// junk            "
	{false, -1},	// junk 		   "
	{false, -1},	// junk            "
	{ true, -1},	// Gibs!
	{ true, -1},	// stove           " (SOD:gibs)
	{ true, -1},	// spears          " (SOD:gibs)
	{false, -1},	// vines			"
	{ true, -1},	// marble pillar
	{false, pow_25clip},	// bonus 25 clip
	{ true, -1},	// truck
	{false, pow_spear}, // SPEAR OF DESTINY!
};

// palettes:
static RGBcolor Wl6Pal[256]=
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0xA8, 0x00, 0xA8, 0x00, 0x00, 0xA8, 0xA8, 0xA8, 0x00, 0x00, 0xA8,
	0x00, 0xA8, 0xA8, 0x54, 0x00, 0xA8, 0xA8, 0xA8, 0x54, 0x54, 0x54, 0x54, 0x54, 0xFC, 0x54, 0xFC,
	0x54, 0x54, 0xFC, 0xFC, 0xFC, 0x54, 0x54, 0xFC, 0x54, 0xFC, 0xFC, 0xFC, 0x54, 0xFC, 0xFC, 0xFC,
	0xEC, 0xEC, 0xEC, 0xDC, 0xDC, 0xDC, 0xD0, 0xD0, 0xD0, 0xC0, 0xC0, 0xC0, 0xB4, 0xB4, 0xB4, 0xA8,
	0xA8, 0xA8, 0x98, 0x98, 0x98, 0x8C, 0x8C, 0x8C, 0x7C, 0x7C, 0x7C, 0x70, 0x70, 0x70, 0x64, 0x64,
	0x64, 0x54, 0x54, 0x54, 0x48, 0x48, 0x48, 0x38, 0x38, 0x38, 0x2C, 0x2C, 0x2C, 0x20, 0x20, 0x20,
	0xFC, 0x00, 0x00, 0xEC, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xD4, 0x00, 0x00, 0xC8, 0x00, 0x00, 0xBC,
	0x00, 0x00, 0xB0, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x98, 0x00, 0x00, 0x88, 0x00, 0x00, 0x7C, 0x00,
	0x00, 0x70, 0x00, 0x00, 0x64, 0x00, 0x00, 0x58, 0x00, 0x00, 0x4C, 0x00, 0x00, 0x40, 0x00, 0x00,
	0xFC, 0xD8, 0xD8, 0xFC, 0xB8, 0xB8, 0xFC, 0x9C, 0x9C, 0xFC, 0x7C, 0x7C, 0xFC, 0x5C, 0x5C, 0xFC,
	0x40, 0x40, 0xFC, 0x20, 0x20, 0xFC, 0x00, 0x00, 0xFC, 0xA8, 0x5C, 0xFC, 0x98, 0x40, 0xFC, 0x88,
	0x20, 0xFC, 0x78, 0x00, 0xE4, 0x6C, 0x00, 0xCC, 0x60, 0x00, 0xB4, 0x54, 0x00, 0x9C, 0x4C, 0x00,
	0xFC, 0xFC, 0xD8, 0xFC, 0xFC, 0xB8, 0xFC, 0xFC, 0x9C, 0xFC, 0xFC, 0x7C, 0xFC, 0xF8, 0x5C, 0xFC,
	0xF4, 0x40, 0xFC, 0xF4, 0x20, 0xFC, 0xF4, 0x00, 0xE4, 0xD8, 0x00, 0xCC, 0xC4, 0x00, 0xB4, 0xAC,
	0x00, 0x9C, 0x9C, 0x00, 0x84, 0x84, 0x00, 0x70, 0x6C, 0x00, 0x58, 0x54, 0x00, 0x40, 0x40, 0x00,
	0xD0, 0xFC, 0x5C, 0xC4, 0xFC, 0x40, 0xB4, 0xFC, 0x20, 0xA0, 0xFC, 0x00, 0x90, 0xE4, 0x00, 0x80,
	0xCC, 0x00, 0x74, 0xB4, 0x00, 0x60, 0x9C, 0x00, 0xD8, 0xFC, 0xD8, 0xBC, 0xFC, 0xB8, 0x9C, 0xFC,
	0x9C, 0x80, 0xFC, 0x7C, 0x60, 0xFC, 0x5C, 0x40, 0xFC, 0x40, 0x20, 0xFC, 0x20, 0x00, 0xFC, 0x00,
	0x00, 0xFC, 0x00, 0x00, 0xEC, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xD4, 0x00, 0x04, 0xC8, 0x00, 0x04,
	0xBC, 0x00, 0x04, 0xB0, 0x00, 0x04, 0xA4, 0x00, 0x04, 0x98, 0x00, 0x04, 0x88, 0x00, 0x04, 0x7C,
	0x00, 0x04, 0x70, 0x00, 0x04, 0x64, 0x00, 0x04, 0x58, 0x00, 0x04, 0x4C, 0x00, 0x04, 0x40, 0x00,
	0xD8, 0xFC, 0xFC, 0xB8, 0xFC, 0xFC, 0x9C, 0xFC, 0xFC, 0x7C, 0xFC, 0xF8, 0x5C, 0xFC, 0xFC, 0x40,
	0xFC, 0xFC, 0x20, 0xFC, 0xFC, 0x00, 0xFC, 0xFC, 0x00, 0xE4, 0xE4, 0x00, 0xCC, 0xCC, 0x00, 0xB4,
	0xB4, 0x00, 0x9C, 0x9C, 0x00, 0x84, 0x84, 0x00, 0x70, 0x70, 0x00, 0x58, 0x58, 0x00, 0x40, 0x40,
	0x5C, 0xBC, 0xFC, 0x40, 0xB0, 0xFC, 0x20, 0xA8, 0xFC, 0x00, 0x9C, 0xFC, 0x00, 0x8C, 0xE4, 0x00,
	0x7C, 0xCC, 0x00, 0x6C, 0xB4, 0x00, 0x5C, 0x9C, 0xD8, 0xD8, 0xFC, 0xB8, 0xBC, 0xFC, 0x9C, 0x9C,
	0xFC, 0x7C, 0x80, 0xFC, 0x5C, 0x60, 0xFC, 0x40, 0x40, 0xFC, 0x20, 0x24, 0xFC, 0x00, 0x04, 0xFC,
	0x00, 0x00, 0xFC, 0x00, 0x00, 0xEC, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xD4, 0x00, 0x00, 0xC8, 0x00,
	0x00, 0xBC, 0x00, 0x00, 0xB0, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x98, 0x00, 0x00, 0x88, 0x00, 0x00,
	0x7C, 0x00, 0x00, 0x70, 0x00, 0x00, 0x64, 0x00, 0x00, 0x58, 0x00, 0x00, 0x4C, 0x00, 0x00, 0x40,
	0x28, 0x28, 0x28, 0xFC, 0xE0, 0x34, 0xFC, 0xD4, 0x24, 0xFC, 0xCC, 0x18, 0xFC, 0xC0, 0x08, 0xFC,
	0xB4, 0x00, 0xB4, 0x20, 0xFC, 0xA8, 0x00, 0xFC, 0x98, 0x00, 0xE4, 0x80, 0x00, 0xCC, 0x74, 0x00,
	0xB4, 0x60, 0x00, 0x9C, 0x50, 0x00, 0x84, 0x44, 0x00, 0x70, 0x34, 0x00, 0x58, 0x28, 0x00, 0x40,
	0xFC, 0xD8, 0xFC, 0xFC, 0xB8, 0xFC, 0xFC, 0x9C, 0xFC, 0xFC, 0x7C, 0xFC, 0xFC, 0x5C, 0xFC, 0xFC,
	0x40, 0xFC, 0xFC, 0x20, 0xFC, 0xFC, 0x00, 0xFC, 0xE0, 0x00, 0xE4, 0xC8, 0x00, 0xCC, 0xB4, 0x00,
	0xB4, 0x9C, 0x00, 0x9C, 0x84, 0x00, 0x84, 0x6C, 0x00, 0x70, 0x58, 0x00, 0x58, 0x40, 0x00, 0x40,
	0xFC, 0xE8, 0xDC, 0xFC, 0xE0, 0xD0, 0xFC, 0xD8, 0xC4, 0xFC, 0xD4, 0xBC, 0xFC, 0xCC, 0xB0, 0xFC,
	0xC4, 0xA4, 0xFC, 0xBC, 0x9C, 0xFC, 0xB8, 0x90, 0xFC, 0xB0, 0x80, 0xFC, 0xA4, 0x70, 0xFC, 0x9C,
	0x60, 0xF0, 0x94, 0x5C, 0xE8, 0x8C, 0x58, 0xDC, 0x88, 0x54, 0xD0, 0x80, 0x50, 0xC8, 0x7C, 0x4C,
	0xBC, 0x78, 0x48, 0xB4, 0x70, 0x44, 0xA8, 0x68, 0x40, 0xA0, 0x64, 0x3C, 0x9C, 0x60, 0x38, 0x90,
	0x5C, 0x34, 0x88, 0x58, 0x30, 0x80, 0x50, 0x2C, 0x74, 0x4C, 0x28, 0x6C, 0x48, 0x24, 0x5C, 0x40,
	0x20, 0x54, 0x3C, 0x1C, 0x48, 0x38, 0x18, 0x40, 0x30, 0x18, 0x38, 0x2C, 0x14, 0x28, 0x20, 0x0C,
	0x60, 0x00, 0x64, 0x00, 0x64, 0x64, 0x00, 0x60, 0x60, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x2C, 0x30,
	0x24, 0x10, 0x48, 0x00, 0x48, 0x50, 0x00, 0x50, 0x00, 0x00, 0x34, 0x1C, 0x1C, 0x1C, 0x4C, 0x4C,
	0x4C, 0x5C, 0x5C, 0x5C, 0x40, 0x40, 0x40, 0x30, 0x30, 0x30, 0x34, 0x34, 0x34, 0xD8, 0xF4, 0xF4,
	0xB8, 0xE8, 0xE8, 0x9C, 0xDC, 0xDC, 0x74, 0xC8, 0xC8, 0x48, 0xC0, 0xC0, 0x20, 0xB4, 0xB4, 0x20,
	0xB0, 0xB0, 0x00, 0xA4, 0xA4, 0x00, 0x98, 0x98, 0x00, 0x8C, 0x8C, 0x00, 0x84, 0x84, 0x00, 0x7C,
	0x7C, 0x00, 0x78, 0x78, 0x00, 0x74, 0x74, 0x00, 0x70, 0x70, 0x00, 0x6C, 0x6C, 0x98, 0x00, 0x88
},
SodPal[256]=
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0xA8, 0x00, 0xA8, 0x00, 0x00, 0xA8, 0xA8, 0xA8, 0x00, 0x00, 0xA8,
	0x00, 0xA8, 0xA8, 0x54, 0x00, 0xA8, 0xA8, 0xA8, 0x54, 0x54, 0x54, 0x54, 0x54, 0xFC, 0x54, 0xFC,
	0x54, 0x54, 0xFC, 0xFC, 0xFC, 0x54, 0x54, 0xFC, 0x54, 0xFC, 0xFC, 0xFC, 0x54, 0xFC, 0xFC, 0xFC,
	0xEC, 0xEC, 0xEC, 0xDC, 0xDC, 0xDC, 0xD0, 0xD0, 0xD0, 0xC0, 0xC0, 0xC0, 0xB4, 0xB4, 0xB4, 0xA8,
	0xA8, 0xA8, 0x98, 0x98, 0x98, 0x8C, 0x8C, 0x8C, 0x7C, 0x7C, 0x7C, 0x70, 0x70, 0x70, 0x64, 0x64,
	0x64, 0x54, 0x54, 0x54, 0x48, 0x48, 0x48, 0x38, 0x38, 0x38, 0x2C, 0x2C, 0x2C, 0x20, 0x20, 0x20,
	0xFC, 0x00, 0x00, 0xEC, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xD4, 0x00, 0x00, 0xC8, 0x00, 0x00, 0xBC,
	0x00, 0x00, 0xB0, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x98, 0x00, 0x00, 0x88, 0x00, 0x00, 0x7C, 0x00,
	0x00, 0x70, 0x00, 0x00, 0x64, 0x00, 0x00, 0x58, 0x00, 0x00, 0x4C, 0x00, 0x00, 0x40, 0x00, 0x00,
	0xFC, 0xD8, 0xD8, 0xFC, 0xB8, 0xB8, 0xFC, 0x9C, 0x9C, 0xFC, 0x7C, 0x7C, 0xFC, 0x5C, 0x5C, 0xFC,
	0x40, 0x40, 0xFC, 0x20, 0x20, 0xFC, 0x00, 0x00, 0xFC, 0xA8, 0x5C, 0xFC, 0x98, 0x40, 0xFC, 0x88,
	0x20, 0xFC, 0x78, 0x00, 0xE4, 0x6C, 0x00, 0xCC, 0x60, 0x00, 0xB4, 0x54, 0x00, 0x9C, 0x4C, 0x00,
	0xFC, 0xFC, 0xD8, 0xFC, 0xFC, 0xB8, 0xFC, 0xFC, 0x9C, 0xFC, 0xFC, 0x7C, 0xFC, 0xF8, 0x5C, 0xFC,
	0xF4, 0x40, 0xFC, 0xF4, 0x20, 0xFC, 0xF4, 0x00, 0xE4, 0xD8, 0x00, 0xCC, 0xC4, 0x00, 0xB4, 0xAC,
	0x00, 0x9C, 0x9C, 0x00, 0x84, 0x84, 0x00, 0x70, 0x6C, 0x00, 0x58, 0x54, 0x00, 0x40, 0x40, 0x00,
	0xD0, 0xFC, 0x5C, 0xC4, 0xFC, 0x40, 0xB4, 0xFC, 0x20, 0xA0, 0xFC, 0x00, 0x90, 0xE4, 0x00, 0x80,
	0xCC, 0x00, 0x74, 0xB4, 0x00, 0x60, 0x9C, 0x00, 0xD8, 0xFC, 0xD8, 0xBC, 0xFC, 0xB8, 0x9C, 0xFC,
	0x9C, 0x80, 0xFC, 0x7C, 0x60, 0xFC, 0x5C, 0x40, 0xFC, 0x40, 0x20, 0xFC, 0x20, 0x00, 0xFC, 0x00,
	0x00, 0xFC, 0x00, 0x00, 0xEC, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xD4, 0x00, 0x04, 0xC8, 0x00, 0x04,
	0xBC, 0x00, 0x04, 0xB0, 0x00, 0x04, 0xA4, 0x00, 0x04, 0x98, 0x00, 0x04, 0x88, 0x00, 0x04, 0x7C,
	0x00, 0x04, 0x70, 0x00, 0x04, 0x64, 0x00, 0x04, 0x58, 0x00, 0x04, 0x4C, 0x00, 0x04, 0x40, 0x00,
	0xD8, 0xFC, 0xFC, 0xB8, 0xFC, 0xFC, 0x9C, 0xFC, 0xFC, 0x7C, 0xFC, 0xF8, 0x5C, 0xFC, 0xFC, 0x40,
	0xFC, 0xFC, 0x20, 0xFC, 0xFC, 0x00, 0xFC, 0xFC, 0x00, 0xE4, 0xE4, 0x00, 0xCC, 0xCC, 0x00, 0xB4,
	0xB4, 0x00, 0x9C, 0x9C, 0x00, 0x84, 0x84, 0x00, 0x70, 0x70, 0x00, 0x58, 0x58, 0x00, 0x40, 0x40,
	0x5C, 0xBC, 0xFC, 0x40, 0xB0, 0xFC, 0x20, 0xA8, 0xFC, 0x00, 0x9C, 0xFC, 0x00, 0x8C, 0xE4, 0x00,
	0x7C, 0xCC, 0x00, 0x6C, 0xB4, 0x00, 0x5C, 0x9C, 0xD8, 0xD8, 0xFC, 0xB8, 0xBC, 0xFC, 0x9C, 0x9C,
	0xFC, 0x7C, 0x80, 0xFC, 0x5C, 0x60, 0xFC, 0x40, 0x40, 0xFC, 0x20, 0x24, 0xFC, 0x00, 0x04, 0xFC,
	0x00, 0x00, 0xFC, 0x00, 0x00, 0xEC, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xD4, 0x00, 0x00, 0xC8, 0x00,
	0x00, 0xBC, 0x00, 0x00, 0xB0, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x98, 0x00, 0x00, 0x88, 0x00, 0x00,
	0x7C, 0x00, 0x00, 0x70, 0x00, 0x00, 0x64, 0x00, 0x00, 0x58, 0x00, 0x00, 0x4C, 0x00, 0x00, 0x40,
	0x28, 0x28, 0x28, 0xFC, 0xE0, 0x34, 0xFC, 0xD4, 0x24, 0xFC, 0xCC, 0x18, 0xFC, 0xC0, 0x08, 0xFC,
	0xB4, 0x00, 0x00, 0x38, 0x00, 0x00, 0x28, 0x00, 0x98, 0x00, 0xE4, 0x80, 0x00, 0xCC, 0x74, 0x00,
	0xB4, 0x60, 0x00, 0x9C, 0x50, 0x00, 0x84, 0x44, 0x00, 0x70, 0x34, 0x00, 0x58, 0x28, 0x00, 0x40,
	0xFC, 0xD8, 0xFC, 0xFC, 0xB8, 0xFC, 0xFC, 0x9C, 0xFC, 0xFC, 0x7C, 0xFC, 0xFC, 0x5C, 0xFC, 0xFC,
	0x40, 0xFC, 0xFC, 0x20, 0xFC, 0xFC, 0x00, 0xFC, 0xE0, 0x00, 0xE4, 0xC8, 0x00, 0xCC, 0xB4, 0x00,
	0xB4, 0x9C, 0x00, 0x9C, 0x84, 0x00, 0x84, 0x6C, 0x00, 0x70, 0x58, 0x00, 0x58, 0x40, 0x00, 0x40,
	0xFC, 0xE8, 0xDC, 0xFC, 0xE0, 0xD0, 0xFC, 0xD8, 0xC4, 0xFC, 0xD4, 0xBC, 0xFC, 0xCC, 0xB0, 0xFC,
	0xC4, 0xA4, 0xFC, 0xBC, 0x9C, 0xFC, 0xB8, 0x90, 0xFC, 0xB0, 0x80, 0xFC, 0xA4, 0x70, 0xFC, 0x9C,
	0x60, 0xF0, 0x94, 0x5C, 0xE8, 0x8C, 0x58, 0xDC, 0x88, 0x54, 0xD0, 0x80, 0x50, 0xC8, 0x7C, 0x4C,
	0xBC, 0x78, 0x48, 0xB4, 0x70, 0x44, 0xA8, 0x68, 0x40, 0xA0, 0x64, 0x3C, 0x9C, 0x60, 0x38, 0x90,
	0x5C, 0x34, 0x88, 0x58, 0x30, 0x80, 0x50, 0x2C, 0x74, 0x4C, 0x28, 0x6C, 0x48, 0x24, 0x5C, 0x40,
	0x20, 0x54, 0x3C, 0x1C, 0x48, 0x38, 0x18, 0x40, 0x30, 0x18, 0x38, 0x2C, 0x14, 0x28, 0x20, 0x0C,
	0x60, 0x00, 0x64, 0x00, 0x64, 0x64, 0x00, 0x60, 0x60, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x2C, 0x30,
	0x24, 0x10, 0x48, 0x00, 0x48, 0x50, 0x00, 0x50, 0x00, 0x00, 0x34, 0x1C, 0x1C, 0x1C, 0x4C, 0x4C,
	0x4C, 0x5C, 0x5C, 0x5C, 0x40, 0x40, 0x40, 0x30, 0x30, 0x30, 0x34, 0x34, 0x34, 0xD8, 0xF4, 0xF4,
	0xB8, 0xE8, 0xE8, 0x9C, 0xDC, 0xDC, 0x74, 0xC8, 0xC8, 0x48, 0xC0, 0xC0, 0x20, 0xB4, 0xB4, 0x20,
	0xB0, 0xB0, 0x00, 0xA4, 0xA4, 0x00, 0x98, 0x98, 0x00, 0x8C, 0x8C, 0x00, 0x84, 0x84, 0x00, 0x7C,
	0x7C, 0x00, 0x78, 0x78, 0x00, 0x74, 0x74, 0x00, 0x70, 0x70, 0x00, 0x6C, 0x6C, 0x98, 0x00, 0x88
};

// ------------------------- * version detection and initialization * -------------------------
cvar_t *datadir; // wolf data directory /can be located outside the game tree/
cvar_t *gametype;// wolf game type (extension, which will be checked)

/*
** Ver_CheckGameType
*/
bool Ver_CheckGameType(char *ext)
{
	if(File_COM_Exist(va("%s/%s.%s", datadir->string, file_audh, ext)) &&
		 File_COM_Exist(va("%s/%s.%s", datadir->string, file_audm, ext)) &&
		 File_COM_Exist(va("%s/%s.%s", datadir->string, file_page, ext)) &&
		 File_COM_Exist(va("%s/%s.%s", datadir->string, file_vgah, ext)) &&
		 File_COM_Exist(va("%s/%s.%s", datadir->string, file_vgad, ext)) &&
		 File_COM_Exist(va("%s/%s.%s", datadir->string, file_vgam, ext)) &&
		 File_COM_Exist(va("%s/%s.%s", datadir->string, file_maph, ext)) &&
		 File_COM_Exist(va("%s/%s.%s", datadir->string, file_mapm, ext)))
			return true;
	else
		return false;
}

/*
** Ver_Init
**
** fills WolfVer stucture and sets all data remap tables
*/
int Ver_Init(void)
{
	int n;

	Con_Printf("Ver_Init: Detecting Wolf version\n");

// those can be set from command line only
	datadir=Cvar_Get("datadir", ".", CVAR_NOSET);
	gametype=Cvar_Get("gametype", "", CVAR_NOSET|CVAR_SERVERINFO);
	Con_Printf("-> Using datadir=\"%s\"\n", datadir->string);

	if(*gametype->string)
	{
	// gametype defined
		Con_Printf("-> Using custom gametype=\"%s\"\n", gametype->string);

	// select definition
		for(n=0; n<WSET_LAST; n++)
			if(!strcmp(ver_def[n].ext, gametype->string)) break;
		if(n==WSET_LAST) n=WSET_WL6;

	// check that files really exist
		if(!Ver_CheckGameType(gametype->string))
			n=WSET_LAST;
	}
	else for(n=0; n<WSET_LAST; n++)
	{
	// autodetect gametype
		if(Ver_CheckGameType(ver_def[n].ext)) // we've found version!
		{
			Cvar_ForceSet("gametype", ver_def[n].ext);
			break;
		}
	}

	if(n==WSET_LAST)
	{
		Con_Printf("-> No Wolf data files found!\n");
		Sys_Error("No Wolf data files found.\nYou need original Wolf3D data files in order to play NewWolf.");
	}
	Con_Printf("-> Wolf version: %s", ver_def[n].desc);
	if(n==WSET_WL3)
	{
		Con_Printf("\n");
		Con_Printf("-> Sorry, but it is currenty unsupported\n");
		Con_Printf("-> Wait for an update...\n");
		Sys_Error("NewWolf doesn't support '3 episode version' yet.");
	}

	sprintf(WolfVer.file_audh, "%s/%s.%s", datadir->string, file_audh, gametype->string); 
	sprintf(WolfVer.file_audm, "%s/%s.%s", datadir->string, file_audm, gametype->string);
	sprintf(WolfVer.file_page, "%s/%s.%s", datadir->string, file_page, gametype->string);
	sprintf(WolfVer.file_vgah, "%s/%s.%s", datadir->string, file_vgah, gametype->string);
	sprintf(WolfVer.file_vgad, "%s/%s.%s", datadir->string, file_vgad, gametype->string);
	sprintf(WolfVer.file_vgam, "%s/%s.%s", datadir->string, file_vgam, gametype->string);
	sprintf(WolfVer.file_maph, "%s/%s.%s", datadir->string, file_maph, gametype->string);
	sprintf(WolfVer.file_mapm, "%s/%s.%s", datadir->string, file_mapm, gametype->string);
	strcpy(WolfVer.ext, ver_def[n].ext);
	WolfVer.set=n;
	if(n==WSET_WL6 && File_COM_Size(WolfVer.file_vgah)/3==159)
	{
		WolfVer.ver=WVER_11; // version 1.1
		Con_Printf(" v1.1\n");
	}
	else
	{
		WolfVer.ver=WVER_14; // assume v1.4 (it will crash on other) FIXME!
		Con_Printf("\n");
	}

// set all remap data
 	switch(n)
	{
	case WSET_WL6:
		WolfPal=Wl6Pal;
		if(File_COM_Size(WolfVer.file_vgah)/3==159)
		{
			WolfVer.ver=WVER_11;
			VGA_REMAP=vga_wl6_11;
		}
		else
			VGA_REMAP=vga_wl6_14;
		SPR_REMAP=spr_wolf;
		MUS_REMAP=mus_wl6;
		SND_REMAP=snd_wl6;
		wolfdigimap=wl6_digimap;
		STARTPCSOUNDS=WL6_STARTPCSOUNDS;
		STARTADLIBSOUNDS=WL6_STARTADLIBSOUNDS;
		STARTDIGISOUNDS=WL6_STARTDIGISOUNDS;
		STARTMUSIC=WL6_STARTMUSIC;
		levelsongs=wl6_songs;
		statinfo=static_wl6;
		num_statics=sizeof(static_wl6)/sizeof(static_wl6[0]);
		celing_color=celing_wl6;
		break;
	case WSET_WL1:
		WolfPal=Wl6Pal;
		VGA_REMAP=vga_wl1;
		SPR_REMAP=spr_wolf;
		MUS_REMAP=mus_wl6;
		SND_REMAP=snd_wl6;
		wolfdigimap=wl1_digimap;
		STARTPCSOUNDS=WL6_STARTPCSOUNDS;
		STARTADLIBSOUNDS=WL6_STARTADLIBSOUNDS;
		STARTDIGISOUNDS=WL6_STARTDIGISOUNDS;
		STARTMUSIC=WL6_STARTMUSIC;
		levelsongs=wl6_songs;
		statinfo=static_wl6;
		num_statics=sizeof(static_wl6)/sizeof(static_wl6[0]);
		celing_color=celing_wl6;
		break;
	case WSET_SDM:
		WolfPal=SodPal;
		VGA_REMAP=vga_sdm;
		SPR_REMAP=spr_spear;
		MUS_REMAP=mus_sod;
		SND_REMAP=snd_sod;
		wolfdigimap=sdm_digimap;
		STARTPCSOUNDS=SOD_STARTPCSOUNDS;
		STARTADLIBSOUNDS=SOD_STARTADLIBSOUNDS;
		STARTDIGISOUNDS=SOD_STARTDIGISOUNDS;
		STARTMUSIC=SOD_STARTMUSIC;
		levelsongs=sod_songs;
		statinfo=static_sod;
		num_statics=sizeof(static_sod)/sizeof(static_sod[0]);
		celing_color=celing_sod;
		break;
	case WSET_SOD:
		WolfPal=SodPal;
		VGA_REMAP=vga_sod;
		SPR_REMAP=spr_spear;
		MUS_REMAP=mus_sod;
		SND_REMAP=snd_sod;
		wolfdigimap=sod_digimap;
		STARTPCSOUNDS=SOD_STARTPCSOUNDS;
		STARTADLIBSOUNDS=SOD_STARTADLIBSOUNDS;
		STARTDIGISOUNDS=SOD_STARTDIGISOUNDS;
		STARTMUSIC=SOD_STARTMUSIC;
		levelsongs=sod_songs;
		statinfo=static_sod;
		num_statics=sizeof(static_sod)/sizeof(static_sod[0]);
		celing_color=celing_sod;
		break;
	case WSET_WL3:
		break;
	}
	return 1;
}