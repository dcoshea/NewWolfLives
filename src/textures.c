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
** texture caching module
*************************************************************************/
#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"

// ------------------------- * Devider * -------------------------
image_t *tex_cache=NULL;

// ------------------------- * Devider * -------------------------

/*
** Tex_Find
**
** finds cached texture, returns NULL if not found
*/
image_t *Tex_Find(char *name)
{
	image_t *tex;

	for(tex=tex_cache; tex; tex=tex->next)
		if(!strcmp(name, tex->name)) return tex;
	return NULL;
}

/*
** Tex_Cache
**
** caches texture, if not cached
*/
image_t *Tex_Cache(char *name)
{
	image_t *tex;
	int index;

	if(tex=Tex_Find(name)) return tex; // already cached

	tex=Z_Malloc(sizeof(image_t));
	strcpy(tex->name, name);

	if(*name=='$') // texture from wolf data file
	{
		if(!strncmp(name, "$tex/", 5)) // wall texuture from VSWAP
		{
			index=atoi(name+5);
			
		// check for substitution first
			if(Img_Read(va("textures/wolf/wall%04d", index), tex))
				goto loading_ok;

			tex->data=Z_Malloc(64*64*3);
			tex->w=tex->h=64;
			tex->bpp=3;
			if(File_PML_LoadWall(index, tex->data))
				goto loading_ok;
			else
				Z_Free(tex->data);
		}
		if(!strncmp(name, "$dor/", 5)) // door texuture from VSWAP
		{
			index=atoi(name+5);
			
		// check for substitution first
			if(Img_Read(va("textures/wolf/door%04d", index), tex))
				goto loading_ok;

			tex->data=Z_Malloc(64*64*3);
			tex->w=tex->h=64;
			tex->bpp=3;
			if(File_PML_LoadDoor(index, tex->data))
				goto loading_ok;
			else
				Z_Free(tex->data);
		}
		else if(!strncmp(name, "$spr/", 5)) // sprite from VSWAP
		{
			index=atoi(name+5);

		// check for substitution first
			if(Img_Read(va("textures/wolf/sprt%04d", index), tex))
				goto loading_ok;

			tex->data=Z_Malloc(64*64*4);
			tex->w=tex->h=64;
			tex->bpp=4;
			if(!File_PML_LoadSprite(SPR_REMAP[index], tex->data))
				goto loading_ok;
			else
				Z_Free(tex->data);
		}
		else if(!strncmp(name, "$pic/", 5)) // picture from VGAGRAPH
		{
			wl_pic pic;

			index=atoi(name+5);

			// check for substitution first
			if(Img_Read(va("gfx/wolf/pict%04d", index), tex))
				goto loading_ok;

			if(File_VGA_ReadPic(index, &pic))
			{
				tex->bpp=3;
				tex->data=pic.data;
				tex->w=pic.width;
				tex->h=pic.height;
			}
		}

		Z_Free(tex);
		return NULL; // unknown source
	}
	else if(!Img_Read(name, tex))
	{
		Z_Free(tex);
		return NULL; // unknown source
	}		

loading_ok:
// link texture
	tex->next=tex_cache;
	tex_cache=tex;

	return tex;
}

/*
** Tex_Uncache
*/
void Tex_Uncache(image_t *tex)
{
	image_t *t, **prev;

	prev=&tex_cache;

	for(t=tex_cache; t; t=t->next)
	{
		if(t==tex)
		{
			*prev=t->next;
			if(t->data) Z_Free(t->data);
			Z_Free(t);
			return;
		}
		prev=&t->next;
	}
}

/*
** Tex_UncacheAll
*/
void Tex_UncacheAll(void)
{
	image_t *tex, *t;

	for(tex=tex_cache; tex;)
	{
		t=tex->next;
		if(tex->data) Z_Free(tex->data);
		Z_Free(tex);
		tex=t;
	}

	tex_cache=NULL;
}
