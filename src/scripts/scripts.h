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
** script parsers
*************************************************************************/

// ------------------------- * models * -------------------------
#define MDL_ALLIGN_FLOOR	1		// floor alligned model
#define MDL_ALLIGN_CEIL		2		// ceiling alligned model
#define MDL_FACE_PLAYER		4		// model faces player all the time (same as aprites)
#define MDL_ROTATE				8		// model rotates (for weapons and bonuses)
#define MDL_BOBBING				16	// vertical bobbing

typedef struct mdl_static_s
{
	char filename[MAX_WLPATH];	// model file name
	char skin[MAX_WLPATH];			// skin file name
	int frame;									// model's frame index
	int animlen;								// animation length
	float fps;									// animation speed
	float scale;								// model scale
	float z_coord;							// z-coord
	unsigned flags;							// flags
	void *model;								// model itself
} mdl_static_t;

typedef struct mdl_weapon_s
{
	char filename[MAX_WLPATH];	// model file name
	char skin[MAX_WLPATH];			// skin file name
	int frames[5];							// model's frame index
	void *model;							// model itself
} mdl_weapon_t;

extern mdl_static_t *mdl_statics;
extern mdl_weapon_t *mdl_weapons;
extern int mdl_nstatics, mdl_nweapons;

void Scr_ModelsFree(void);
void Scr_ModelsParse(char *filename);
