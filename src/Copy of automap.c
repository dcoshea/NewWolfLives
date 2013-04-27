/*************************************\
* Automap Code                        *
* Ideas: Try to implement line-style  *
* map (like in doom!)                 *
* Show opened doors!                  *
* part of NewWolf(wolf4ever) project  *
* (C) 2000-01 by DarkOne the Hacker   *
\*************************************/
#include <windows.h>
#include <stdio.h>
#include <GL\gl.h>
#include "WolfDef.h"

AutoMap_t AM_AutoMap;

// ** Automap Init **
int AM_Init(void)
{
	Cmd_AddCommand("revealmap", AM_Reveal_f);
	Cmd_AddCommand("hidemap", AM_Hide_f);
	AM_ResetAutomap();
	return 1;
}

/* reset automap status */
void AM_ResetAutomap(void)
{
	memset(&AM_AutoMap, 0, sizeof(AM_AutoMap));
}

/* save automap status */
void AM_SaveAutomap(FILE *fp)
{
	fwrite(&AM_AutoMap, sizeof(AM_AutoMap), 1, fp);
}

/* load automap status */
void AM_LoadAutomap(FILE *fp)
{
	fread(&AM_AutoMap, sizeof(AM_AutoMap), 1, fp);
}

// will hide all automap
void AM_Hide_f(void)
{
	memset(AM_AutoMap.vis, 0, sizeof(AM_AutoMap.vis));
}

// will open all map [!!cheat!!]
void AM_Reveal_f(void)
{
	memset(AM_AutoMap.vis, 1, sizeof(AM_AutoMap.vis));
}

// will draw automap!
// Better NOT to do it platform independent!
void AM_DrawAutomap(void)
{
	int x, y, ymap;

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

// map background! think about picture (old manuscript (as in Hexen))!
// ask mod authors!
	glColor3ub(128, 128, 128);
	glVertex2i(MAPXSTART, MAPYSTART+(64<<MAPSHIFT));
	glVertex2i(MAPXSTART, MAPYSTART);
	glVertex2i(MAPXSTART+(64<<MAPSHIFT), MAPYSTART);
	glVertex2i(MAPXSTART+(64<<MAPSHIFT), MAPYSTART+(64<<MAPSHIFT));
// end background

	for(x=0; x<64; x++)
	for(y=0, ymap=63; y<64; y++, ymap--)
	{
		if(!AM_AutoMap.vis[x][ymap]) continue;
		if(CurMapData.tile_info[x][ymap]&TILE_IS_WALL)
		{// wall!
			if(CurMapData.tile_info[x][ymap]&TILE_IS_SECRET)
				glColor3ub(255, 255, 128);
			else if(CurMapData.tile_info[x][ymap]&TILE_IS_ELEVATOR)
				glColor3ub(255, 255, 0);
			else
				glColor3ub(255, 128, 128);
			glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT) );
			glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+(	y			<< MAPSHIFT) );
			glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT), MAPYSTART+(	y			<< MAPSHIFT) );
			glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT) );
		}
		else
		{// floor
			glColor3ub(0, 0, 0);
			glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT) );
			glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+(	y			<< MAPSHIFT) );
			glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT), MAPYSTART+(	y			<< MAPSHIFT) );
			glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT) );
		}
		if(CurMapData.tile_info[x][ymap]&TILE_IS_DOOR)
		{
			switch(DoorMap[x][ymap]->type)
			{
			case DOOR_VERT:
			case DOOR_HORIZ:
				glColor3ub(128, 128, 128);
				break;
			case DOOR_E_VERT:
			case DOOR_E_HORIZ:
				glColor3ub(255, 128, 0);
				break;
			case DOOR_S_VERT:
			case DOOR_S_HORIZ:
				glColor3ub(32, 176, 176);
				break;
			case DOOR_G_VERT:
			case DOOR_G_HORIZ:
				glColor3ub(228, 216, 0);
				break;
			}
			if(DoorMap[x][ymap]->vertical)
			{
				glVertex2i( MAPXSTART+(	x		<< MAPSHIFT)+(1<<(MAPSHIFT-2)), MAPYSTART+((y+1)	<< MAPSHIFT) );
				glVertex2i( MAPXSTART+(	x		<< MAPSHIFT)+(1<<(MAPSHIFT-2)), MAPYSTART+(	y		<< MAPSHIFT) );
				glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT)-(1<<(MAPSHIFT-2)), MAPYSTART+(	y		<< MAPSHIFT) );
				glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT)-(1<<(MAPSHIFT-2)), MAPYSTART+((y+1)	<< MAPSHIFT) );
			}
			else
			{
				glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT)-(1<<(MAPSHIFT-2)) );
				glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+(	y		<< MAPSHIFT)+(1<<(MAPSHIFT-2)) );
				glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT), MAPYSTART+(	y		<< MAPSHIFT)+(1<<(MAPSHIFT-2)) );
				glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT)-(1<<(MAPSHIFT-2)) );
			}
		}
		else if(CurMapData.tile_info[x][ymap]&(TILE_IS_POWERUP|TILE_IS_DRESS|TILE_IS_BLOCK|TILE_IS_ACTOR))
		{
			if(CurMapData.tile_info[x][ymap]&TILE_IS_BLOCK)
				glColor3ub(255, 128, 128);
			else if(CurMapData.tile_info[x][ymap]&TILE_IS_DRESS)
				glColor3ub(0, 255, 0);
/* DEBUG!
			else if(CurMapData.tile_info[x][ymap]&TILE_IS_ACTOR)
				glColor3ub(255, 0, 255);*/
			else if(CurMapData.tile_info[x][ymap]&TILE_IS_POWERUP)
				glColor3ub(255, 0, 0);
			glVertex2i( MAPXSTART+(	x		<< MAPSHIFT)+(1<<(MAPSHIFT-2)), MAPYSTART+((y+1)	<< MAPSHIFT)-(1<<(MAPSHIFT-2)) );
			glVertex2i( MAPXSTART+(	x		<< MAPSHIFT)+(1<<(MAPSHIFT-2)), MAPYSTART+(	y		<< MAPSHIFT)+(1<<(MAPSHIFT-2)) );
			glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT)-(1<<(MAPSHIFT-2)), MAPYSTART+(	y		<< MAPSHIFT)+(1<<(MAPSHIFT-2)) );
			glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT)-(1<<(MAPSHIFT-2)), MAPYSTART+((y+1)	<< MAPSHIFT)-(1<<(MAPSHIFT-2)) );
		}
		if(developer->value && tile_visible[x+ymap*64])
		{	
			glColor4ub(0, 255, 0, 0x80);
			glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT) );
			glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+(	y		<< MAPSHIFT) );
			glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT), MAPYSTART+(	y		<< MAPSHIFT) );
			glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT) );
			glColor3ub(255, 255, 255);
		}
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	x=POS2TILE(Player.position.origin[0]);
	y=63-POS2TILE(Player.position.origin[1]);
	glColor3ub(128, 255, 255);
	switch(Get8dir(Player.position.angle))
	{
	case dir8_east:
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT) );
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+(	y		<< MAPSHIFT) );
		glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT), MAPYSTART+(	y		<< MAPSHIFT)+(1<<(MAPSHIFT-1)) );
		break;
	case dir8_northeast:
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT)+(1<<(MAPSHIFT-1)), MAPYSTART+((y+1)	<< MAPSHIFT) );
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+(	y		<< MAPSHIFT)+(1<<(MAPSHIFT-1)) );
		glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT), MAPYSTART+(	y		<< MAPSHIFT) );
		break;
	case dir8_north:
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT)+(1<<(MAPSHIFT-1)), MAPYSTART+(y	<< MAPSHIFT) );
		glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT) );
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT) );
		break;
	case dir8_northwest:
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+(	y		<< MAPSHIFT) );
		glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT), MAPYSTART+(	y 	<< MAPSHIFT)+(1<<(MAPSHIFT-1)) );
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT)+(1<<(MAPSHIFT-1)), MAPYSTART+((y+1)	<< MAPSHIFT) );
		break;
	case dir8_west:
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+(	y		<< MAPSHIFT)+(1<<(MAPSHIFT-1)) );
		glVertex2i( MAPXSTART+((x+1)	<< MAPSHIFT), MAPYSTART+(	y		<< MAPSHIFT) );
		glVertex2i( MAPXSTART+((x+1)	<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT) );
		break;
	case dir8_southwest:
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT)+(1<<(MAPSHIFT-1)), MAPYSTART+(y	<< MAPSHIFT) );
		glVertex2i( MAPXSTART+((x+1)	<< MAPSHIFT), MAPYSTART+(	y 	<< MAPSHIFT)+(1<<(MAPSHIFT-1)) );
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT) );
		break;
	case dir8_south:
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+(	y		<< MAPSHIFT) );
		glVertex2i( MAPXSTART+((x+1)	<< MAPSHIFT), MAPYSTART+(	y		<< MAPSHIFT) );
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT)+(1<<(MAPSHIFT-1)), MAPYSTART+((y+1)	<< MAPSHIFT) );
		break;
	case dir8_southeast:
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT)+(1<<(MAPSHIFT-1)), MAPYSTART+(	y		<< MAPSHIFT) );
		glVertex2i( MAPXSTART+((x+1)	<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT) );
		glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+(	y		<< MAPSHIFT)+(1<<(MAPSHIFT-1)) );
		break;
	}
	glEnd();

// map name
	FNT_SetFont(FNT_CONSOLE);
	FNT_SetStyle(0, 1, 0);
	FNT_SetColor(192, 192, 192, 255);
	FNT_SetColorSh(0, 0, 0, 100);
	FNT_SetScale(1, 1);
	FNT_PrintPos(1, 26, CurMapData.name);
}
