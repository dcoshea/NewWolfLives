/*
// ------------------------- * Devider * -------------------------
	Constants & Defines for WolfGL Project
*/

#include "common/common.h"


#define XRES			640
#define YRES			480

// this 2 values describe 2d drawing size
// and are not dependant from resolution/window size
#define SIZE_2D_X 640
#define SIZE_2D_Y 480


#include "build.h"

#include "math.h"
#include "OpenGL.h"
#include "video.h"
#include "sound.h"
#include "keys.h"
#include "Q_fns.h"
#include "TexMan.h"
#include "fileio.h"

#include "client.h"
//#include "server.h"

#include "game.h"
#include "level.h"
#include "areas.h"
#include "doors.h"
#include "sprites.h"
#include "Actors.h"
#include "Player.h"
#include "ai_com.h"
#include "actor_ai.h"
#include "FontMan.h"
#include "Menu.h"
#include "Render.h"
#include "raycast.h"
#include "map.h"
#include "automap.h"
#include "version.h"

#include "vgapics.h"
#include "powerups.h"
#include "Pwalls.h"
#include "wl_text.h"
#include "SaveLoad.h"
#include "particles.h"
// ------------------------- * My Functions * -------------------------
void LoadRealLevel(int level);
char CanCloseDoor(int x, int y, char vert);

void ScanInfoPlane(int level);

void SpawnStand (enemy_t which, int tilex, int tiley, int dir);
void SpawnPatrol (enemy_t which, int tilex, int tiley, int dir);
void SpawnDeadGuard(enemy_t which, int x, int y);
void SpawnBoss(enemy_t which, int x, int y);
void SpawnGhosts(enemy_t which, int x, int y);

void ProcessGuards(void);

void T_BJRun(Guard_struct *Guard);
void T_BJJump(Guard_struct *Guard);
void T_BJYell(Guard_struct *Guard);
void T_BJDone(Guard_struct *Guard);

void SpawnBJVictory(void);

bool CheckLine(Guard_struct *Guard);

// ------------------------- * End Functions * -------------------------

// ------------------------- * Tile Sizes * -------------------------
#define MINACTORDIST 0x10000 // minimum dist from player center to any actor center

#define TILEGLOBAL 0x10000
#define HALFTILE   0x8000
#define FLOATTILE  65536.0f
#define TILESHIFT  16				// 1<<TILESHIFT=0x10000 (TILEGLOBAL)
// ------------------------- * Devider * -------------------------

#define HORIZON		240     // YRES/2 
#define VIEWSIZE	412			// where to start Control Panel
// ------------------------- * Devider * -------------------------
#define MINDIST				(0x5800)
#define PLAYERSIZE		MINDIST			// player radius
#define CLOSEWALL			MINDIST //22			// Space between wall & player

#define PLAYERSPEED		3000
#define RUNSPEED   		6000

#define	TEX_CON		0
#define TEX_FLOOR 1
#define TEX_CEL   2
#define TEX_WALL_BEG	3


#define SECRET_BLOCK 0x62

#define ELEVATOR_SWITCH		21
#define ELEVATOR_PRESSED	22

#define ALTELEVATORTILE		107

#define MAPSHIFT	2
#define MAPXSTART 192
#define MAPYSTART 100

#define SIZEOF_SPRITES 24

#define MAXACTORS			150				// max number of nazis, etc / map
#define MAXSTATS			400				// max number of lamps, bonus, etc
#define MAXDOORS			64				// max number of sliding doors
#define MAXWALLTILES	64				// max number of wall tiles

#define TURNTICS	10
#define SPDPATROL	512
#define SPDDOG		1500

#define FL_SHOOTABLE		1
#define FL_BONUS				2
#define FL_NEVERMARK		4
#define FL_VISABLE			8
#define FL_ATTACKMODE		16
#define FL_FIRSTATTACK	32
#define FL_AMBUSH				64
#define FL_NONMARK			128
