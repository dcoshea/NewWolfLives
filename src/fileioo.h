// ------------------------- * File I/O * -------------------------
/*
================================
	My own code + some code ripped from wolf's src + some from
	WolfEx by Bruce Lewis
================================
*/
// ------------------------- * Devider * -------------------------
void LoadRealLevel(int level)
{
	extern void PE_ClearParticles(void);

// reseting textures, models & sounds to cache them again
	Vid_UnCacheAllTexs(); // FIXME: Precache uncaches all!
	SD_StopAllSound();
	SD_StopMusic();
	SD_UnCacheAllSnds();
	PE_ClearParticles();
	//model_reset(); // FIXME: newgeneration

// ------------------------- * Reseting values * -------------------------
	Door_ResetDoors();
	Ar_ResetAreas();
	AM_ResetAutomap();
	PW_Reset();

// FIXME:
	memset(&gamestate,	0, sizeof(gamestate)); // Reset gamestate
	ResetGuards();
	
	gamestate.difficulty=skill;
	if(!IS_SPEAR)
	{
		gamestate.episode=level/10+1; //FIXME
		gamestate.map=level%10+1;
	}
	else
		gamestate.map=level+1;

	if(!Lvl_LoadLevel(level))
	{
		Con_Printf("Error loading level: %d\n", level);
		return;
	}

	ScanInfoPlane(level); // Spawn items/guards
	PL_Spawn(CurMapData.pSpawn); // Spawn Player
	
	Vid_PrecacheTextures();

	Con_ClearNotify();
// ! Done Loading Level ! Display Info & Start music!
//	Con_Printf("\n\35\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\37\n");
	Con_Printf("\n-===================================-\n");
	Msg_Printf("%s", CurMapData.name);
	Con_Printf("\n\n");
	SD_PlayMusic(CurMapData.music, true);
}
