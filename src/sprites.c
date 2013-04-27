/*************************************\
* Sprites Handling Code (server)      *
*                                     *
* part of NewWolf(wolf4ever) project  *
* (C) 2000-02 by DarkOne the Hacker   *
\*************************************/
#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"

// ------------------------- * Devider * -------------------------

// total sprites on level in a moment
#define MAX_SPRITES 1024

// nobody should see this array!
sprite_t Spr_Sprites[MAX_SPRITES];
int n_of_sprt;

// ------------------------- * Devider * -------------------------

/*
** Spr_Init
*/
int Spr_Init(void)
{
	Spr_ResetSprites();
	return 1;
}

// ------------------------- * Devider * -------------------------

/* reset sprites status
**
** called only when client must reconnect
** will not set remove flag!
*/
void Spr_ResetSprites(void)
{
	n_of_sprt=0;
	memset(Spr_Sprites, 0, sizeof(Spr_Sprites));
}

/* save sprites status */
void Spr_SaveSprites(FILE *fp)
{
	fwrite(&n_of_sprt, sizeof(n_of_sprt), 1, fp);
	fwrite(&Spr_Sprites, sizeof(Spr_Sprites), 1, fp);
}

/* load sprites status */
void Spr_LoadSprites(FILE *fp)
{
	fread(&n_of_sprt, sizeof(n_of_sprt), 1, fp);
	fread(&Spr_Sprites, sizeof(Spr_Sprites), 1, fp);
}

void Spr_RemoveSprite(int spr_id)
{
	if(spr_id==-1) return;
	Spr_Sprites[spr_id].flags|=SPRT_REMOVE;
}

// returns "sprite id" which is just index
int Spr_GetNewSprite(void)
{
	int n;
	sprite_t* sprt;

	for(n=0, sprt=Spr_Sprites; n<n_of_sprt; n++, sprt++)
	{
		if(sprt->flags&SPRT_REMOVE)
		{ // free spot: clear it first
			memset(sprt, 0, sizeof(sprite_t));
			return n;
		}
	}
	if(n_of_sprt>=MAX_SPRITES)
	{
		Con_Printf("Warning n_of_sprt==MAX_SPRITES\n");
		return -1;
	}
	return n_of_sprt++;
}

// ------------------------- * Changing Sprite Data * -------------------------

void Spr_SetPos(int spr_id, int x, int y, int angle)
{
	if(spr_id==-1) return;
	Spr_Sprites[spr_id].x=x;
	Spr_Sprites[spr_id].y=y;
	Spr_Sprites[spr_id].ang=angle;
	Spr_Sprites[spr_id].tilex=POS2TILE(x);
	Spr_Sprites[spr_id].tiley=POS2TILE(y);
	Spr_Sprites[spr_id].flags|=SPRT_CHG_POS;

	if(!(x&HALFTILE)) // (x%TILEGLOBAL>=HALFTILE)
		Spr_Sprites[spr_id].tilex--;
	if(!(y&HALFTILE))
		Spr_Sprites[spr_id].tiley--;
}

void Spr_SetTex(int spr_id, int index, int tex)
{
	if(spr_id==-1) return;
	if(index==-1) // one texture for each phase
	{
		Spr_Sprites[spr_id].tex[0]=tex;
		Spr_Sprites[spr_id].flags|=SPRT_ONE_TEX;
	}
	else
		Spr_Sprites[spr_id].tex[index]=tex;
	Spr_Sprites[spr_id].flags|=SPRT_CHG_TEX;
}

// ------------------------- * Visibility List * -------------------------
#define MAXVISABLE 128
visobj_t vislist[MAXVISABLE];
// macro to get distance from a void pointer to visobj_t
#define vis_dist(vis) (((visobj_t *)vis)->dist)

/*
** Spr_cmpVis
**
** compare function for vislist sorting
*/
int Spr_cmpVis(const void *vis1, const void *vis2)
{
	if(vis_dist(vis1)==vis_dist(vis2))
		return 0; // process equal distance
	else
		return vis_dist(vis1)<vis_dist(vis2)?1:-1; // if dist > sprite must be first
}

/*
** Will build & sort visibility list of sprites
**
** it is a ready-to-draw list. No platform specific code here!
** list is sorited from far to near
** built based on tile visibility array, made by raycaster
** called only by client, so no server data is available
*/
int Spr_CreateVisList(void)
{
	int tx, ty, n, num_visable;
	visobj_t *visptr;
	sprite_t* sprt;

	visptr=vislist;
	num_visable=0;

	for(n=0, sprt=Spr_Sprites; n<n_of_sprt; n++, sprt++)
	{
		if(sprt->flags&SPRT_REMOVE) continue;

		tx=sprt->tilex; ty=sprt->tiley;
	// can be in any of 4 surrounding tiles; not 9 - see definition of tilex & tiley
		if(tile_visible[tx+ty*64]		|| tile_visible[tx+1+ty*64] ||
			 tile_visible[tx+(ty+1)*64] || tile_visible[tx+1+(ty+1)*64])
		{ // player spoted it
			visptr->dist=LineLen2Point(sprt->x-Player.position.origin[0],
																 sprt->y-Player.position.origin[1],
																 Player.position.angle); //FIXME viewport
			visptr->x=sprt->x;
			visptr->y=sprt->y;
			visptr->ang=sprt->ang;
			visptr->tex=sprt->tex[0]; //FIXME!
			if(++num_visable>MAXVISABLE) break; // vislist full
			visptr++;
		}
	}

// sorting list
	if(num_visable) // do not sort if no entries
		qsort(vislist, num_visable, sizeof(visobj_t), Spr_cmpVis);

	return num_visable;
}