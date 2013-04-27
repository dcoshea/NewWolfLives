#include <windows.h>
#include <stdio.h>
#include "wolfdef.h"
#include "sprt_def.h"

leveldef CurMapData;
int TotalLevels=0;

int Lvl_Init(void)
{
	memset(&CurMapData, 0, sizeof(CurMapData));
	
	TotalLevels=File_MAP_OpenMapFiles(WolfVer.file_maph, WolfVer.file_mapm);
	if(!TotalLevels) return 0;

	Con_Printf("Lvl_Init: Done.\n");
	return 1;
}

void Lvl_SpawnStatic(int type, int x, int y)
{
	int spr_id;

	if(statinfo[type].powerup==-1)
	{
		if(statinfo[type].block)	// blocking static
			CurMapData.tile_info[x][y]|=TILE_IS_BLOCK;
		else											// dressing static
			CurMapData.tile_info[x][y]|=TILE_IS_DRESS;
		spr_id=Spr_GetNewSprite();
		if(spr_id==-1) return;
		Spr_SetPos(spr_id, TILE2POS(x), TILE2POS(y), 0);
		Spr_SetTex(spr_id, 0, SPR_STAT_0+type);
	}
	else
	{
		Pow_Spawn(x, y, statinfo[type].powerup);
		if(statinfo[type].powerup==pow_cross || 
			 statinfo[type].powerup==pow_chalice ||
			 statinfo[type].powerup==pow_bible ||
			 statinfo[type].powerup==pow_crown ||
			 statinfo[type].powerup==pow_fullheal)
				gamestate.treasuretotal++; // FIXME: move this to Pow_Spawn Function!
	}
}

void Lvl_SpawnObj(int type, int x, int y)
{
	if(type>=23 && type<23+num_statics)
	{// static object
		Lvl_SpawnStatic(type-23, x, y);
		return;
	}

	switch(type)
	{
	case 0x13: // start N
		CurMapData.pSpawn.origin[0]=TILE2POS(x);
		CurMapData.pSpawn.origin[1]=TILE2POS(y);
		CurMapData.pSpawn.angle=ANG_90;
		break;
	case 0x14: // start E
		CurMapData.pSpawn.origin[0]=TILE2POS(x);
		CurMapData.pSpawn.origin[1]=TILE2POS(y);
		CurMapData.pSpawn.angle=ANG_0;
		break;
	case 0x15: // start S
		CurMapData.pSpawn.origin[0]=TILE2POS(x);
		CurMapData.pSpawn.origin[1]=TILE2POS(y);
		CurMapData.pSpawn.angle=ANG_270;
		break;
	case 0x16: // start W
		CurMapData.pSpawn.origin[0]=TILE2POS(x);
		CurMapData.pSpawn.origin[1]=TILE2POS(y);
		CurMapData.pSpawn.angle=ANG_180;
		break;
	case 0x5a: // turn E
		CurMapData.tile_info[x][y]|=TILE_IS_E_TURN;//FIXME!
		break;
	case 0x5b: // turn NE
		CurMapData.tile_info[x][y]|=TILE_IS_NE_TURN;//FIXME!
		break;
	case 0x5c: // turn N
		CurMapData.tile_info[x][y]|=TILE_IS_N_TURN;//FIXME!
		break;
	case 0x5d: // turn NW
		CurMapData.tile_info[x][y]|=TILE_IS_NW_TURN;//FIXME!
		break;
	case 0x5e: // turn W
		CurMapData.tile_info[x][y]|=TILE_IS_W_TURN;//FIXME!
		break;
	case 0x5f: // turn SW
		CurMapData.tile_info[x][y]|=TILE_IS_SW_TURN;//FIXME!
		break;
	case 0x60: // turn S
		CurMapData.tile_info[x][y]|=TILE_IS_S_TURN;//FIXME!
		break;
	case 0x61: // turn SE
		CurMapData.tile_info[x][y]|=TILE_IS_SE_TURN;//FIXME!
		break;
	case 0x62: // pushwall modifier
		CurMapData.tile_info[x][y]|=TILE_IS_SECRET;
		gamestate.secrettotal++;
		break;
	case 0x63: // End Game triger
		CurMapData.tile_info[x][y]|=TILE_IS_EXIT;
		break;
	// spawn guards
	}
}

// loads a map with a zero based index n (e1m1=0, e2m5=14)
// it just fills in the 'leveldef' struct
int Lvl_LoadLevel(int zbmn)
{
	int x, y0, y, l1, l2, l3;

// FIXME: move to {client|server} init
	Pow_Reset();
	Spr_ResetSprites();

	memset(&CurMapData, 0, sizeof(CurMapData));
	CurMapData.mapid=zbmn;
	if(!File_MAP_ReadData(zbmn, CurMapData.layer1, CurMapData.layer2, CurMapData.layer3, CurMapData.name)) return 0;

// ------------------------- * Parsing Layers * -------------------------
	for(y0=0; y0<64; y0++)
		for(x=0; x<64; x++)
	{
		y=63-y0;
		l1=CurMapData.layer1[y0*64+x];
		l2=CurMapData.layer2[y0*64+x];
		l3=CurMapData.layer3[y0*64+x];

// if server, process obj layer!
		if(l2) Lvl_SpawnObj(l2, x, y);

// Map data layer
		if(l1==0)
		{
			CurMapData.areas[x][y]=-3; // unknown area
		}
		else if(l1<0x6a) // solid map object
		{
			if((l1>=0x5A && l1<=0x5F) || l1==0x64 || l1==0x65) // door
			{
				CurMapData.tile_info[x][y]|=TILE_IS_DOOR;
				Door_SpawnDoor(x, y, l1);
				CurMapData.areas[x][y]=-2; // door area
			}
			else
			{
				CurMapData.tile_info[x][y]|=TILE_IS_WALL;

				CurMapData.wall_tex_x[x][y]=(l1-1)*2+1;
				CurMapData.wall_tex_y[x][y]=(l1-1)*2;
				CurMapData.areas[x][y]=-1; // wall area
				if(l1==0x15) // elevator
					CurMapData.tile_info[x][y]|=TILE_IS_ELEVATOR;
			}
		}
		else if(l1==0x6a) // Ambush floor tile
		{
			CurMapData.tile_info[x][y]|=TILE_IS_AMBUSH;
			CurMapData.areas[x][y]=-3; // unknown area
		}
		else if(l1>=FIRSTAREA && l1<(FIRSTAREA+NUMAREAS)) // area
		{
			if(l1==FIRSTAREA) // secret level
				CurMapData.tile_info[x][y]|=TILE_IS_SLEVEL;
			CurMapData.areas[x][y]=l1-FIRSTAREA;// spawn area
		}
		else
			CurMapData.areas[x][y]=-3; // unknown area
// End of the map data layer
	}
	Door_SetAreas();

	CurMapData.music=levelsongs[zbmn];
	CurMapData.celing=WolfPal[celing_color[zbmn]];
	CurMapData.floor=WolfPal[0x19]; // standart floor color

	return 1;
}