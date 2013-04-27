#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"

doorstruct Doors[MAX_DOORS];
doorstruct *DoorMap[64][64];
int totaldoors=0;

/* resets doors status */
void Door_ResetDoors(void)
{
	totaldoors=0;
	memset(Doors,	0, sizeof(Doors));
	memset(DoorMap, 0, sizeof(DoorMap));
}

/* save door status */
void Door_SaveDoors(FILE *fp)
{
	fwrite(&Doors, sizeof(Doors), 1, fp);
	fwrite(&totaldoors, sizeof(totaldoors), 1, fp);
}

/* load door status */
void Door_LoadDoors(FILE *fp)
{
	int n;

	fread(&Doors, sizeof(Doors), 1, fp);
	fread(&totaldoors, sizeof(totaldoors), 1, fp);

	for(n=0; n<totaldoors; n++)
		DoorMap[Doors[n].x][Doors[n].y]=&Doors[n];
}

/* spawns door at x, y, position */
int Door_SpawnDoor(int x, int y, int type)
{
	if(totaldoors>=MAX_DOORS)
	{
		Con_Printf("Doors: Too many Doors (%d)\n", totaldoors);
		return 0;
	}

	switch(type)
	{
	case 0x5A:
		Doors[totaldoors].type=DOOR_VERT;
		Doors[totaldoors].vertical=true;
		Doors[totaldoors].texture=TEX_DDOOR+1;
		break;
	case 0x5B:
		Doors[totaldoors].type=DOOR_HORIZ;
		Doors[totaldoors].vertical=false;
		Doors[totaldoors].texture=TEX_DDOOR;
		break;
	case 0x5C:
		Doors[totaldoors].type=DOOR_G_VERT;
		Doors[totaldoors].vertical=true;
		Doors[totaldoors].texture=TEX_DLOCK+1;
		break;
	case 0x5D:
		Doors[totaldoors].type=DOOR_G_HORIZ;
		Doors[totaldoors].vertical=false;
		Doors[totaldoors].texture=TEX_DLOCK;
		break;
	case 0x5E:
		Doors[totaldoors].type=DOOR_S_VERT;
		Doors[totaldoors].vertical=true;
		Doors[totaldoors].texture=TEX_DLOCK+1;
		break;
	case 0x5F:
		Doors[totaldoors].type=DOOR_S_HORIZ;
		Doors[totaldoors].vertical=false;
		Doors[totaldoors].texture=TEX_DLOCK;
		break;
	case 0x64:
		Doors[totaldoors].type=DOOR_E_VERT;
		Doors[totaldoors].vertical=true;
		Doors[totaldoors].texture=TEX_DELEV+1;
		break;
	case 0x65:
		Doors[totaldoors].type=DOOR_E_HORIZ;
		Doors[totaldoors].vertical=false;
		Doors[totaldoors].texture=TEX_DELEV;
		break;
	default:
		Con_Printf("SpawnDoor: Unknown door type: %d\n", type);
		return 0;
	}

	Doors[totaldoors].x=x;
	Doors[totaldoors].y=y;
	Doors[totaldoors].action=dr_closed;
	DoorMap[x][y]=&Doors[totaldoors];
	totaldoors++;

	return totaldoors-1;
}

/*
================================
Setting areas for doors!
================================
*/
void Door_SetAreas(void)
{
	int n, x, y;

	for(n=0; n<totaldoors; n++)
	{
		x=Doors[n].x;
		y=Doors[n].y;

		if(Doors[n].vertical)
		{
			Doors[n].area1=CurMapData.areas[x+1][y]>=0 ? CurMapData.areas[x+1][y] : 0;
			Doors[n].area2=CurMapData.areas[x-1][y]>=0 ? CurMapData.areas[x-1][y] : 0;
		}
		else
		{
			Doors[n].area1=CurMapData.areas[x][y+1]>=0 ? CurMapData.areas[x][y+1] : 0;
			Doors[n].area2=CurMapData.areas[x][y-1]>=0 ? CurMapData.areas[x][y-1] : 0;
		}
	}
}

/*
=====================
=
= OpenDoor
=
=====================
*/
void Door_OpenDoor(doorstruct *Door)
{
	if(Door->action==dr_open)
		Door->tickcount=0;			// reset opened time
	else
		Door->action=dr_opening;	// start opening it
}

void Door_ChangeDoorState(doorstruct *Door)
{
	if(Door->action<dr_opening)
		Door_OpenDoor(Door);
	else if(Door->action==dr_open && CanCloseDoor(Door->x, Door->y, Door->vertical))
	{
		Door->action=dr_closing;
		Door->tickcount=DOOR_FULLOPEN;
	}
}

void Door_ProcessDoors_e(int t_tk, int t_ms)
{
	int n;

	for(n=0; n<totaldoors; n++)
	{
		switch(Doors[n].action)
		{
		case dr_closed: // this door is closed!
			continue;
		case dr_opening:
			if(Doors[n].tickcount>=DOOR_FULLOPEN) // door fully opened!
			{
				Doors[n].action=dr_open;
				Doors[n].tickcount=0;
			}
			else // opening!
			{
				if(Doors[n].tickcount==0)
				{ // door is just starting to open, so connect the areas
					Ar_JoinAreas(Doors[n].area1, Doors[n].area2);
					Ar_ConnectAreas(Player.areanumber);
					if(areabyplayer[Doors[n].area1]) // Door Opening sound!
						PlaySoundLocTile(OPENDOORSND, CHAN_VOICE, Doors[n].x, Doors[n].y);
				}
				Doors[n].tickcount+=t_tk;
				if(Doors[n].tickcount>DOOR_FULLOPEN) Doors[n].tickcount=DOOR_FULLOPEN;
			}
			break;
		case dr_closing:
			if(Doors[n].tickcount<=0) // door fully closed! disconnect areas!
			{
				Ar_DisjoinAreas(Doors[n].area1, Doors[n].area2);
				Ar_ConnectAreas(Player.areanumber);
				Doors[n].tickcount=0;
				Doors[n].action=dr_closed;
			}
			else // closing!
			{
				if(Doors[n].tickcount==DOOR_FULLOPEN)
				if(areabyplayer[Doors[n].area1]) // Door Closing sound!
					PlaySoundLocTile(CLOSEDOORSND, CHAN_VOICE, Doors[n].x, Doors[n].y);
				Doors[n].tickcount-=t_tk;
				if(Doors[n].tickcount<0) Doors[n].tickcount=0;
			}
			break;
		case dr_open:
			if(Doors[n].tickcount>DOOR_MINOPEN)
			{ // If player or something is in door do not close it!
				if(!CanCloseDoor(Doors[n].x, Doors[n].y, Doors[n].vertical)) Doors[n].tickcount=DOOR_MINOPEN; // do not close door immediately!
			}
			if(Doors[n].tickcount>=DOOR_TIMEOUT)
			{ // Door timeout, time to close it!
				Doors[n].action=dr_closing;
				Doors[n].tickcount=DOOR_FULLOPEN;
			}
			else
			{ // Increase timeout!
				Doors[n].tickcount+=t_tk;
			}
			break;
		}
	}
}

// returns DOOR_FULLOPEN is door is opened, 0 if closed or progress [0-DOOR_FULLOPEN]
// if there are no doors in tile assume a closed door!
int Door_Opened(int x, int y)
{
	if(!DoorMap[x][y]) return 0;
	return DoorMap[x][y]->action==dr_open ? DOOR_FULLOPEN : DoorMap[x][y]->tickcount;
}

/*
** Door_TryUse
**
** Try to use door with keys player has
** FIXME: [Script]
*/
bool Door_TryUse(doorstruct *Door, int keys)
{
	switch(Door->type)
	{
	case DOOR_VERT:
	case DOOR_HORIZ:
	case DOOR_E_VERT:
	case DOOR_E_HORIZ:
		Door_ChangeDoorState(Door); // does not require key!
		break;
	case DOOR_G_VERT:
	case DOOR_G_HORIZ:
		if(keys&ITEM_KEY_1)
			Door_ChangeDoorState(Door);
		else
			Msg_Printf("You need a gold key to open this door");
		break;
	case DOOR_S_VERT:
	case DOOR_S_HORIZ:
		if(keys&ITEM_KEY_2)
			Door_ChangeDoorState(Door);
		else
			Msg_Printf("You need a silver key to open this door");
		break;
	}
	return true; // FIXME
}