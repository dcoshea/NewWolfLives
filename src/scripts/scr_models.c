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
** model definition script parser
*************************************************************************/
#include <windows.h>
#include "../WolfDef.h"
#include "scripts.h"

// ------------------------- * globals * -------------------------
mdl_static_t *mdl_statics=NULL; // statics
mdl_weapon_t *mdl_weapons=NULL;	// weapons
int mdl_nstatics=0;	// total statics
int mdl_nweapons=0;	// total weapons

// ------------------------- * Devider * -------------------------

/*
** Scr_ModelsParseObjects
*/
void Scr_ModelsParseObjects(parse_t *p)
{
	mdl_static_t *cur;
	char flags[16], *t;

	Con_Printf("-> object models:\n");
	P_ExpectSymbol(p, '{');

	while(P_CheckSymbol(p, '{'))
	{
	// allocate memory
		mdl_nstatics++;
		mdl_statics=realloc(mdl_statics, sizeof(mdl_static_t)*mdl_nstatics);
		cur=&mdl_statics[mdl_nstatics-1];
		memset(cur, 0, sizeof(mdl_static_t));
		
		if(P_CheckSymbol(p, '}')) // empty definition
		{
			P_CheckSymbol(p, ','); // optional comma
			continue; 
		}
		
		// file name
		P_ExpectString(p, cur->filename, sizeof(cur->filename));
		P_ExpectSymbol(p, ',');
		// skin name
		P_ExpectString(p, cur->skin, sizeof(cur->skin));
		P_ExpectSymbol(p, ',');
		// frame
		P_ExpectInt(p, &cur->frame);
		P_ExpectSymbol(p, ',');
		// animation length
		P_ExpectInt(p, &cur->animlen);
		P_ExpectSymbol(p, ',');
		// animation speed
		P_ExpectFloat(p, &cur->fps);
		P_ExpectSymbol(p, ',');
		// scale
		P_ExpectFloat(p, &cur->scale);
		P_ExpectSymbol(p, ',');
		// z
		P_ExpectFloat(p, &cur->z_coord);
		P_ExpectSymbol(p, ',');
		// flags
		P_ExpectId(p, flags, sizeof(flags));
		t=flags;
		while(*t) switch(*t++)
		{
		case 'f':
			cur->flags|=MDL_ALLIGN_FLOOR;
			break;
		case 'c':
			cur->flags|=MDL_ALLIGN_CEIL;
			break;
		case 'b':
			cur->flags|=MDL_FACE_PLAYER;
			break;
		case 'r':
			cur->flags|=MDL_ROTATE;
			break;
		case 'v':
			cur->flags|=MDL_BOBBING;
			break;
		}

		P_ExpectSymbol(p, '}');
		P_CheckSymbol(p, ','); // optional comma

//		Con_Printf("--> %d: {\"%s\", \"%s\", %d, %d, %g, %g, %g, %d}\n", mdl_nstatics-1, cur->filename, cur->skin, cur->frame, cur->animlen, cur->fps, cur->scale, cur->z_coord, cur->flags);
	}
	P_ExpectSymbol(p, '}');
	Con_Printf("--> total %d object models(s)\n", mdl_nstatics);
}

void Scr_ModelsParseWeapons(parse_t *p)
{
	mdl_weapon_t *cur;
	int n;

	Con_Printf("-> weapon models:\n");
	P_ExpectSymbol(p, '{');

	while(P_CheckSymbol(p, '{'))
	{
	// allocate memory
		mdl_nweapons++;
		mdl_weapons=realloc(mdl_weapons, sizeof(mdl_weapon_t)*mdl_nweapons);
		cur=&mdl_weapons[mdl_nweapons-1];
		memset(cur, 0, sizeof(mdl_weapon_t));
		
		if(P_CheckSymbol(p, '}')) // empty definition
		{
			P_CheckSymbol(p, ','); // optional comma
			continue; 
		}
		
		// file name
		P_ExpectString(p, cur->filename, sizeof(cur->filename));
		P_ExpectSymbol(p, ',');
		// skin name
		P_ExpectString(p, cur->skin, sizeof(cur->skin));
		P_ExpectSymbol(p, ',');
		// frames
		P_ExpectSymbol(p, '[');
		for(n=0; n<5; n++)
		{
			P_ExpectInt(p, &cur->frames[n]);
			P_CheckSymbol(p, ',');
		}
		P_ExpectSymbol(p, ']');

		P_ExpectSymbol(p, '}');
		P_CheckSymbol(p, ','); // optional comma
//		Con_Printf("--> %d: {\"%s\", \"%s\", [%i %i %i %i %i]}\n", mdl_nweapons-1, cur->filename, cur->skin, cur->frames[0], cur->frames[1], cur->frames[2], cur->frames[3], cur->frames[4], cur->frames[5]);
	}
	P_ExpectSymbol(p, '}');
	Con_Printf("--> total %d weapon model(s)\n", mdl_nweapons);
}

/*
** Scr_ModelsParse
*/
void Scr_ModelsParse(char *filename)
{
	parse_t p;
	char *data;

	Scr_ModelsFree(); // free any previouse data
	Con_Printf("Reading model definitions from %s\n", filename);

	FS_LoadFile(filename, &data, true);
	if(!data)
	{
		Con_Printf("-> file read error\n");
		return;
	}

	if(P_BeginParseEx(&p, data))
	{
		Con_Printf("-> error: %s @ %s\n", P_LastError(), P_StreamPosition(&p));
		Scr_ModelsFree(); // free partially parsed data
		FS_FreeFile(data);
		return;
	}

	P_Expect(&p, TOKEN_ID, "objects");
	Scr_ModelsParseObjects(&p);

	P_Expect(&p, TOKEN_ID, "weapons");
	Scr_ModelsParseWeapons(&p);

	FS_FreeFile(data);
}

/*
** Scr_ModelsFree
**
** frees resourced used by model definitions
*/
void Scr_ModelsFree(void)
{
	if(mdl_statics)
	{
		free(mdl_statics);
		mdl_statics=NULL;
	}
	if(mdl_weapons)
	{
		free(mdl_weapons);
		mdl_weapons=NULL;
	}
	mdl_nstatics=0;
	mdl_nweapons=0;
}
