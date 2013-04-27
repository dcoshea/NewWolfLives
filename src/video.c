// high level video
#include <Windows.h>
#include <stdio.h>
#include "WolfDef.h"

/*
** Vid1_Init
*/
int Vid1_Init(void)
{
	Cmd_AddCommand("screenshot", Vid_ScreenShot_f);
	return 1;
}

/*
** Vid_ScreenShot_f
**
** takes screenshot and saves it to 'nWolfXX.tga' file
*/
void Vid_ScreenShot_f(void)
{
	texture_t dump;
	char checkname[MAX_OSPATH];
	int n;

	for(n=0; n<=99; n++)
	{
		sprintf(checkname, "%s/nWolf%02d.tga", FS_Gamedir(), n);
		if(!File_COM_Exist(checkname)) break; // file doesn't exist
	}

	if(n==100)
	{
		Con_Printf("Too many screenshots.\n");
		return;
	}

	ri.R_DumpScreen((image_t*)&dump);
	File_TGA_Write(checkname, &dump);
	Z_Free(dump.data);
	Msg_Printf("Wrote %s", checkname);
}

