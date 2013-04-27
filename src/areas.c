#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"

unsigned char areabyplayer[NUMAREAS]; // if player is in area N areabyplayer[N] is true!
unsigned char areaconnect[NUMAREAS][NUMAREAS]; 
// increment value if areas connect, decrement if disconnect. 
// if 0 not connected if >0 connected. Pretty simple, no?

/* resets areas status */
void Ar_ResetAreas(void)
{
	memset(areabyplayer, 0, sizeof(areabyplayer)); // all areas do not contain player, so far :)
	memset(areaconnect,	 0, sizeof(areaconnect));	 // all areas are disconnected
}

/* save areas status */
void Ar_SaveAreas(FILE *fp)
{
	fwrite(&areabyplayer, sizeof(areabyplayer), 1, fp);
	fwrite(&areaconnect,  sizeof(areaconnect),  1, fp);
}

/* load areas status */
void Ar_LoadAreas(FILE *fp)
{
	fread(&areabyplayer, sizeof(areabyplayer), 1, fp);
	fread(&areaconnect,  sizeof(areaconnect),  1, fp);
}

/*
==============
= ConnectAreas: Scans outward from areanumber, marking all connected areas
==============
*/
void Ar_RecursiveConnect(int areanumber)
{
	int	i;

	for(i=0; i<NUMAREAS; i++)
	{
		if(areaconnect[areanumber][i] && !areabyplayer[i])
		{
			areabyplayer[i]=1;
			Ar_RecursiveConnect(i);
		}
	}
}

// sets all areabyplayer elems to 1 
// if it is connected with [area] FIXME: check bounds! of area
void Ar_ConnectAreas(int area)
{
	memset(areabyplayer, 0, sizeof(areabyplayer));
	areabyplayer[area]=1;
	Ar_RecursiveConnect(area);
}

void Ar_JoinAreas(int area1, int area2)
{// FIXME: check for overflow!
	areaconnect[area1][area2]++;
	areaconnect[area2][area1]++;
}

void Ar_DisjoinAreas(int area1, int area2)
{// FIXME: check for underflow!
	areaconnect[area1][area2]--;
	areaconnect[area2][area1]--;
}
