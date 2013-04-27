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
// true if player position should be centered on the screen
static cvar_t *center_player;
// true if map should be rotated so that the direction the player is facing is
// always at the top of the screen; ignored if center_player is false
static cvar_t *rotate_map;
static cvar_t *alpha_normal; // float in range 0.0-1.0 for alpha
static cvar_t *alpha_transparent; // float in range 0.0-1.0 for alpha
static cvar_t *draw_background; // true if grey background drawn
static cvar_t *show_secrets; // true if secrets should be highlighted
static cvar_t *show_actors; // true if actors should be shown


// ** Automap Init **
int AM_Init(void)
{
	Cmd_AddCommand("revealmap", AM_Reveal_f);
	Cmd_AddCommand("hidemap", AM_Hide_f);
	center_player=Cvar_Get("automap_center_player", "1", CVAR_ARCHIVE);
	rotate_map=Cvar_Get("automap_rotate", "1", CVAR_ARCHIVE);
	alpha_normal=Cvar_Get("automap_alpha_normal", "1.0", CVAR_ARCHIVE);
	alpha_transparent=Cvar_Get("automap_alpha_transparent", "0.4", CVAR_ARCHIVE);
	draw_background=Cvar_Get("automap_draw_background", "0", CVAR_ARCHIVE);
	show_secrets=Cvar_Get("automap_secrets", "0", CVAR_ARCHIVE);
	show_actors=Cvar_Get("automap_actors", "0", CVAR_ARCHIVE);
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
void AM_DrawAutomap(bool transparent)
{
	int x, y, ymap;
	GLubyte map_alpha;
	int player_x=POS2TILE(Player.position.origin[0]);
	int player_y=63-POS2TILE(Player.position.origin[1]);

	if(transparent)
	{
		map_alpha=(GLubyte)(0xFF*alpha_transparent->value);
	}
	else
	{
		map_alpha=(GLubyte)(0xFF*alpha_normal->value);
	}

	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	if(center_player->value)
	{
		glTranslatef(XRES/2, YRES/2, 0);
		if(rotate_map->value)
		{
			// adjust angle so that direction player is facing is at the top
			glRotatef(NormalizeAngle(Player.position.angle + ANG_270)*ASTEP, 0, 0, 1);
		}
		glTranslatef(-1.0f*(MAPXSTART+(player_x<<MAPSHIFT)), -1.0f*(MAPYSTART+(player_y<<MAPSHIFT)), 0);
	}
	glBegin(GL_QUADS);

// map background! think about picture (old manuscript (as in Hexen))!
// ask mod authors!
	if(draw_background->value)
	{
		glColor4ub(128, 128, 128, map_alpha);
		glVertex2i(MAPXSTART, MAPYSTART+(64<<MAPSHIFT));
		glVertex2i(MAPXSTART, MAPYSTART);
		glVertex2i(MAPXSTART+(64<<MAPSHIFT), MAPYSTART);
		glVertex2i(MAPXSTART+(64<<MAPSHIFT), MAPYSTART+(64<<MAPSHIFT));
	}
// end background

	for(x=0; x<64; x++)
	for(y=0, ymap=63; y<64; y++, ymap--)
	{
		if(!AM_AutoMap.vis[x][ymap]) continue;
		if(CurMapData.tile_info[x][ymap]&TILE_IS_WALL)
		{// wall!
			if(CurMapData.tile_info[x][ymap]&TILE_IS_SECRET && show_secrets->value!=0)
				glColor4ub(255, 255, 128, map_alpha);
			else if(CurMapData.tile_info[x][ymap]&TILE_IS_ELEVATOR)
				glColor4ub(255, 255, 0, map_alpha);
			else // includes case of TILE_IS_SECRET but the "show_secrets" cvar is 0
				glColor4ub(255, 128, 128, map_alpha);
			glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT) );
			glVertex2i( MAPXSTART+(	x		<< MAPSHIFT), MAPYSTART+(	y			<< MAPSHIFT) );
			glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT), MAPYSTART+(	y			<< MAPSHIFT) );
			glVertex2i( MAPXSTART+((x+1)<< MAPSHIFT), MAPYSTART+((y+1)	<< MAPSHIFT) );
		}
		else
		{// floor
			glColor4ub(0, 0, 0, map_alpha);
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
				glColor4ub(128, 128, 128, map_alpha);
				break;
			case DOOR_E_VERT:
			case DOOR_E_HORIZ:
				glColor4ub(255, 128, 0, map_alpha);
				break;
			case DOOR_S_VERT:
			case DOOR_S_HORIZ:
				glColor4ub(32, 176, 176, map_alpha);
				break;
			case DOOR_G_VERT:
			case DOOR_G_HORIZ:
				glColor4ub(228, 216, 0, map_alpha);
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
		// note that we'll draw this on top of a door if applicable - this
		// should only be the case for actors
		if(CurMapData.tile_info[x][ymap]&(TILE_IS_POWERUP|TILE_IS_DRESS|TILE_IS_BLOCK|TILE_IS_ACTOR))
		{
			if(CurMapData.tile_info[x][ymap]&TILE_IS_BLOCK)
				glColor4ub(255, 128, 128, map_alpha);
			else if(CurMapData.tile_info[x][ymap]&TILE_IS_DRESS)
				glColor4ub(0, 255, 0, map_alpha);
			else if(CurMapData.tile_info[x][ymap]&TILE_IS_ACTOR)
			{
				if(show_actors->value!=0)
				{
					glColor4ub(255, 0, 255, map_alpha);
				}
				else
				{
					glColor4ub(0, 0, 0, 0);
				}
			}
			else if(CurMapData.tile_info[x][ymap]&TILE_IS_POWERUP)
				glColor4ub(255, 0, 0, map_alpha);
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
	x=player_x;
	y=player_y;
	glColor4ub(128, 255, 255, map_alpha);
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
	glPopMatrix();

// map name
	FNT_SetFont(FNT_CONSOLE);
	FNT_SetStyle(0, 1, 0);
	FNT_SetColor(192, 192, 192, 255);
	FNT_SetColorSh(0, 0, 0, 100);
	FNT_SetScale(1, 1);
	FNT_PrintPos(1, 26, CurMapData.name);
}
