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
** models caching module
*************************************************************************/
#include "common.h"

// ------------------------- * types * -------------------------
typedef struct bbox_s
{
	float zmin, zmax;
	float xmin, xmax;
	float ymin, ymax;
} bbox_t;

typedef struct md2_model_s
{
	char name[MAX_WLPATH];
	int numFrames;
	int frameSize;
	byte *frames;
	int *glCmds;
	bbox_t bbox; // bounding box for the whole model
	
	struct md2_model_s *next;
} md2_model_t;

// ------------------------- * globals * -------------------------
md2_model_t *mdl_cache=NULL;

// ------------------------- * Devider * -------------------------

/*
** Mdl_Find
**
** finds cached model, returns NULL if not found
*/
md2_model_t *Mdl_Find(char *name)
{
	md2_model_t *mdl;

	for(mdl=mdl_cache; mdl; mdl=mdl->next)
		if(!strcmp(name, mdl->name)) return mdl;
	return NULL;
}

/*
** Mdl_Cache
**
** caches modle, if not already cached
*/
md2_model_t *Mdl_Cache(char *name)
{
	md2_model_t *mdl;

	if(mdl=Mdl_Find(name)) return mdl; // already cached

	mdl=malloc(sizeof(md2_model_t));
	if(!mdl) return NULL; // memory error
	strcpy(mdl->name, name);

// load model here

loading_ok:
// link model
	mdl->next=mdl_cache;
	mdl_cache=mdl;

	return mdl;
}

/*
** Mdl_Uncache
*/
void Mdl_Uncache(md2_model_t *mdl)
{
	md2_model_t *t, **prev;

	prev=&mdl_cache;

	for(t=mdl_cache; t; t=t->next)
	{
		if(t==mdl)
		{
			*prev=t->next;

		// unload model here

			free(t);
			return;
		}
		prev=&t->next;
	}
}

/*
** Mdl_UncacheAll
*/
void Mdl_UncacheAll(void)
{
	md2_model_t *mdl, *t;

	for(mdl=mdl_cache; mdl;)
	{
		t=mdl->next;

		// unload model here

		free(mdl);
		mdl=t;
	}

	mdl_cache=NULL;
}
