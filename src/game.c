#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"

game_t gamestate;

// ------------------------- * demo Playing/Recording * -------------------------

/*
** DEMO_ReadCmd
*/
bool DEMO_ReadCmd(usercmd_t *cmd)
{
	if(gamestate.demofile && fread(cmd, sizeof(usercmd_t), 1, gamestate.demofile))
		return true;
	else
		return false;

//	if(!cmd->tics) playstate=ex_completed; // demo is done // FIXME
}

/*
** DEMO_WriteCmd
*/
void DEMO_WriteCmd(usercmd_t *cmd)
{
	if(gamestate.demofile) fwrite(cmd, sizeof(usercmd_t), 1, gamestate.demofile);
}

/*
** DEMO_StartRecord
**
** allocs memory and sets up stuff
*/
void DEMO_StartRecord(char *demoname, int levelnumber)
{
	FILE *fp;

	Con_Printf("recording to %s.\n", demoname);
	fp=fopen(demoname, "wb");
	if(!fp)
	{
		Con_Printf("ERROR: couldn't open.\n");
		return;
	}

	fwrite(&levelnumber, sizeof(levelnumber), 1, fp);

	Cmd_ExecuteString(va("map %d", levelnumber));
	US_InitRndT(false);

	gamestate.demorecord=true;
	gamestate.demofile=fp;
}

/*
** DEMO_FinishRecord
*/
void DEMO_FinishRecord(void)
{
	usercmd_t end_cmd;

	Con_Printf("demo recording stopped.\n");
	memset(&end_cmd, 0, sizeof(usercmd_t));	// end marker
	DEMO_WriteCmd(&end_cmd);

	fclose(gamestate.demofile);
	gamestate.demofile=NULL;
	gamestate.demorecord=false;
}

/*
** DEMO_StartPlay
**
** returns level number
*/
void DEMO_StartPlay(char *demoname)
{
	int levelnumber;
	FILE *fp;

	Con_Printf("playing from %s.\n", demoname);
	fp=fopen(demoname, "rb");
	if(!fp)
	{
		Con_Printf("ERROR: couldn't open.\n");
		return;
	}

	fread(&levelnumber, sizeof(levelnumber), 1, fp);

	Cmd_ExecuteString(va("map %d", levelnumber));
	US_InitRndT(false);

	gamestate.demoplayback=true;
	gamestate.demofile=fp;
}

/*
** DEMO_FinishPlay
*/
void DEMO_FinishPlay(void)
{
	Con_Printf("demo playback stopped.\n");

	fclose(gamestate.demofile);
	gamestate.demofile=NULL;
	gamestate.demoplayback=false;
}

// ------------------------- * Devider * -------------------------

/*
** Map_f
**
** "map <levelnumber>" spawns player to map levelnumber
*/
void Map_f(void)
{
	int level;

	if(Cmd_Argc()==1)
	{
		Con_Printf("use: map <levelnumber>\n");
		return;
	}
	level=atoi(Cmd_Argv(1));
	if(level<=0 && level>TotalLevels)
	{
		Con_Printf("Wrong Level\n");
		return;
	}

	Con_Printf("Changing Level...\n");
	PL_NewGame(&Player);
	LoadRealLevel(level-1);
}

/*
** Record_f
**
** "record <demoname> <levelnumber>" records a demo
*/
void Record_f(void)
{
	char *demoname;
	int level;
	
	if(Cmd_Argc()!=3)
	{
		Con_Printf("use: record <demoname> <levelnumber>\n");
		return;
	}

	if(strstr(Cmd_Argv(1), ".."))
	{
		Con_Printf("Relative pathnames are not allowed.\n");
		return;
	}
	demoname=FS_ExpandFilename(Cmd_Argv(1));
	COM_DefaultExtension(demoname, ".nwd");

	level=atoi(Cmd_Argv(2));
	if(level<=0 && level>TotalLevels)
	{
		Con_Printf("Wrong Level\n");
		return;
	}

	DEMO_StartRecord(demoname, level);
}

/*
** PlayDemo_f
**
** "playdemo <demoname>" plays a demo
*/
void PlayDemo_f(void)
{
	char *demoname;

	if(Cmd_Argc()!=2)
	{
		Con_Printf("use: play <demoname>\n");
		return;
	}
	demoname=FS_ExpandFilename(Cmd_Argv(1));
	COM_DefaultExtension(demoname, ".nwd");

	DEMO_StartPlay(demoname);
}

/*
** Stop_f
**
** "stop" stops recording or playing a demo
*/
void Stop_f(void)
{
	if(gamestate.demorecord)
		DEMO_FinishRecord();
	else if(gamestate.demoplayback)
		DEMO_FinishPlay();
	else
		Con_Printf("nothing to stop.\n");
}

// ------------------------- * Devider * -------------------------

/*
** GM_Init
*/
void GM_Init(void)
{
	GM_Reset();
	PL_Init();

	Cmd_AddCommand("map", Map_f);
	Cmd_AddCommand("record", Record_f);
	Cmd_AddCommand("playdemo", PlayDemo_f);
	Cmd_AddCommand("stop", Stop_f);
}

/*
** GM_Reset
**
** reset gamestete
*/
void GM_Reset(void)
{
	memset(&gamestate, 0, sizeof(gamestate));
}

/*
** GM_SaveGameState
**
** save gamestate
*/
void GM_SaveGameState(FILE *fp)
{
	fwrite(&gamestate, sizeof(gamestate), 1, fp);
}

/*
** GM_LoadGameState
**
** load gamestate
*/
void GM_LoadGameState(FILE *fp)
{
	fread(&gamestate, sizeof(gamestate), 1, fp);
}
