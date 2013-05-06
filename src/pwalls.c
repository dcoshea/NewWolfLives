/*************************************\
* PushWalls Code                      *
* Note: as in wolf only one Pwall can *
*       be active at a time           *
\*************************************/
#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"

Pwall_t PWall;

/* reset pushwall status */
void PW_Reset(void)
{
	PWall.active=false;
}

/* save pushwall status */
void PW_Save(FILE *fp)
{
	fwrite(&PWall, sizeof(PWall), 1, fp);
}

/* load pushwall status */
void PW_Load(FILE *fp)
{
	fread(&PWall, sizeof(PWall), 1, fp);
}

/*
** PW_Push
**
** called by server, when someone tries to push a secret wall
** dir is direction in which pushwall is intended to move
** returns true if push successfull
*/
bool PW_Push(int x, int y, dir4type dir)
{
	int dx, dy;

	if(PWall.active) return false; // another PWall is moving [only one at a time!]

	dx=dx4dir[dir]; dy=dy4dir[dir];

	if(CurMapData.tile_info[x+dx][y+dy]&(TILE_IS_SOLIDTILE | TILE_IS_DOOR))
	{ // noway (smth is blocking)
		Msg_Printf("No way!");
		SD_PlaySound(NOWAYSND, CHAN_VOICE);
		return true;
	}
	
// remove secret flag & make everything needed when pushwall used!
	CurMapData.tile_info[x][y]&=(~TILE_IS_SECRET);
	CurMapData.tile_info[x][y]&=(~TILE_IS_WALL);
	CurMapData.tile_info[x][y]|=TILE_IS_PWALL;
	Msg_Printf("You have found a secret!");
	gamestate.secretcount++;
	SD_PlaySound(PUSHWALLSND, CHAN_VOICE);

// good way to avoid stuckness; [un]coment one more down!
// it makes a tile behind pushwall unpassable
	CurMapData.tile_info[x+dx][y+dy]|=TILE_IS_PWALL;
	CurMapData.wall_tex_x[x+dx][y+dy]=CurMapData.wall_tex_x[x][y];
	CurMapData.wall_tex_y[x+dx][y+dy]=CurMapData.wall_tex_y[x][y];

// write down PWall info
	PWall.active=true;
	PWall.PWtilesmoved=PWall.PWpointsmoved=0;
	PWall.dir=dir;
	PWall.x=x; PWall.y=y;
	PWall.dx=dx; PWall.dy=dy;
	PWall.tex_x=CurMapData.wall_tex_x[x][y];
	PWall.tex_y=CurMapData.wall_tex_y[x][y];

	return true;
}

/* Called by Server each frame to make changes to PWall */
void PW_Process(void)
{
	if(!PWall.active) return; // no active PWall to work with

	PWall.PWpointsmoved+=tics;
	
	if(PWall.PWpointsmoved<128) return;

	PWall.PWpointsmoved-=128;
	PWall.PWtilesmoved++;
// Free tile
	CurMapData.tile_info[PWall.x][PWall.y]&=(~TILE_IS_PWALL);
// Occupy new tile
	PWall.x+=PWall.dx;
	PWall.y+=PWall.dy;

// Shall we move futher?
	if(CurMapData.tile_info[PWall.x+PWall.dx][PWall.y+PWall.dy]&(TILE_IS_SOLIDTILE | TILE_IS_DOOR | TILE_IS_ACTOR | TILE_IS_POWERUP) ||
	   PWall.PWtilesmoved==2)
	{
		CurMapData.tile_info[PWall.x][PWall.y]&=(~TILE_IS_PWALL); // wall now
		CurMapData.tile_info[PWall.x][PWall.y]|=TILE_IS_WALL; // wall now
		CurMapData.wall_tex_x[PWall.x][PWall.y]=PWall.tex_x;
		CurMapData.wall_tex_y[PWall.x][PWall.y]=PWall.tex_y;
		PWall.active=false; // Free Push Wall
	}
	else
		CurMapData.tile_info[PWall.x+PWall.dx][PWall.y+PWall.dy]|=TILE_IS_PWALL;
}
