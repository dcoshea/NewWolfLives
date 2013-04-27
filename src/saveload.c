#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"
// Save/Load game

#define SAVE_VERSION 3

char m_filenames[SAVE_SLOTS][33];
bool loadable[SAVE_SLOTS];

void ScanSaves(void)
{
	int	n, tmp;
	char title[32];
	char sig[5];
	FILE *fp;

	for(n=0; n<SAVE_SLOTS ; n++)
	{
		strcpy(m_filenames[n], "      - empty -");
		loadable[n]=false;
		fp=fopen(FS_ExpandFilename(va("savegam%d.%s", n, WolfVer.ext)), "rb");
		if(!fp)	continue;
		if(fread(sig, 1, 5, fp)!=5) {fclose(fp); continue;}
		if(sig[0]!='N' || sig[1]!='W' || sig[2]!='S' || sig[3]!='A' || sig[4]!='V') {fclose(fp); continue;}
		if(fread(&tmp, sizeof(tmp), 1, fp)!=1) {fclose(fp); continue;}
		if(tmp!=SAVE_VERSION) {fclose(fp); continue;}
		if(fread(title, 1, 32, fp)!=32) {fclose(fp); continue;}

		strncpy(m_filenames[n], title, 32);
		loadable[n]=true;
		fclose(fp);
	}
}

// ------------------------- * Saving * -------------------------
bool Save(int slot)
{
	int n;
	SYSTEMTIME time;
	char *name;
	char title[32];
	FILE *fp;

	if(Player.playstate!=ex_playing)
	{
		Msg_Printf("Cann't save now");
		return false;
	}

	name=FS_ExpandFilename(va("savegam%d.%s", slot, WolfVer.ext));
	Msg_Printf("Saving game to %s..", name);

// savegame comment
	GetLocalTime(&time);
	memset(title, 0, sizeof(title));
	sprintf(title, "%02d/%02d %02d:%02d %s", time.wMonth, time.wDay, time.wHour, time.wMinute, CurMapData.name);

	fp=fopen(name, "wb");
	if(!fp)
	{
		Msg_Printf("Error writing file");
		return false;
	}
	fwrite("NWSAV", 1, 5, fp);
	n=SAVE_VERSION; fwrite(&n, sizeof(n), 1, fp);								// version
	fwrite(title, 1, 32, fp);
// ------------------------- * Actual Save * -------------------------
	fwrite(&CurMapData.mapid, sizeof(CurMapData.mapid), 1, fp);	// Current Map ID
	fwrite(&CurMapData, sizeof(CurMapData), 1, fp);							// Current Map Data

	GM_SaveGameState(fp); // Gamestate
	PL_SavePlayer(fp);		// Player
	Door_SaveDoors(fp);		// Doors
	PW_Save(fp);					// pushwalls
	Ar_SaveAreas(fp);			// Areas
	AM_SaveAutomap(fp);		// Automap
	
	Spr_SaveSprites(fp);	// sprites
	SaveGuards(fp);				// guards
	Pow_Save(fp);					// powerups

	fclose(fp);
	
	return true;
}

// ------------------------- * Loading * -------------------------
bool Load(int slot)
{
	char *name;
	char sig[5];
	FILE *fp;
	int n;

	name=FS_ExpandFilename(va("savegam%d.%s", slot, WolfVer.ext));
	Msg_Printf("Loading from %s...", name);

	fp=fopen(name, "rb");
	if(!fp)
	{
		Msg_Printf("Error opening file");
		return false;
	}
	
	fread(sig, 1, 5, fp);
	if(sig[0]!='N' || sig[1]!='W' || sig[2]!='S' || sig[3]!='A' || sig[4]!='V')
	{
		Msg_Printf("Wrong file format");
		fclose(fp);
		return false;
	}
	fread(&n, sizeof(n), 1, fp);
	if(n!=SAVE_VERSION)
	{
		Msg_Printf("Wrong saved game version: %d (required: %d)", n, SAVE_VERSION);
		fclose(fp);
		return false;
	}
	fseek(fp, 32, SEEK_CUR);
// ------------------------- * Actual Load * -------------------------
	fread(&CurMapData.mapid, sizeof(CurMapData.mapid), 1, fp);	// Current Map ID
	fread(&CurMapData, sizeof(CurMapData), 1, fp);							// Current Map Data

	GM_Reset();					GM_LoadGameState(fp);// Gamestate
	PL_Reset();					PL_LoadPlayer(fp);	// Player
	Door_ResetDoors();	Door_LoadDoors(fp);	// Doors
	PW_Reset();					PW_Load(fp);				// pushwalls

	Ar_ResetAreas();		Ar_LoadAreas(fp);		// Areas
	AM_ResetAutomap();	AM_LoadAutomap(fp); // Automap
	
	Spr_ResetSprites();	Spr_LoadSprites(fp);// sprites
	ResetGuards();			LoadGuards(fp);			// guards
	Pow_Reset();				Pow_Load(fp);				// powerups

	Vid_UnCacheAllTexs(); Vid_PrecacheTextures();
	
	SD_StopAllSound();
	SD_StopMusic();
	SD_UnCacheAllSnds();
	SD_PlayMusic(CurMapData.music, true);
	key_dest=key_game;

	fclose(fp);
	return true;
}

void Save_f(void)
{
	if(Cmd_Argc()==1)
	{
		Con_Printf("use: save <slot number>\n");
		return;
	}
	Save(atol(Cmd_Argv(1)));
}

void Load_f(void)
{
	if(Cmd_Argc()==1)
	{
		Con_Printf("use: load <slot number>\n");
		return;
	}
	Load(atol(Cmd_Argv(1)));
}

// ------------------------- * Initializing * -------------------------
void InitSaveLoad(void)
{
	Cmd_AddCommand("save", Save_f);
	Cmd_AddCommand("load", Load_f);
}