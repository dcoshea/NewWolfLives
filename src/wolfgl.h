/*****************************************\
* Ray Tracer Core                         *
*                                         *
* (C) 2000 by DarkOne the Hacker          *
\*****************************************/
char skill=2;

// ------------------------- * End of the Header * -------------------------
void DrawDebug(void)
{
	FNT_SetFont(FNT_CONSOLE);
	FNT_SetStyle(0, 0, 0);
	FNT_SetColor(0, 180, 180, 255);
	FNT_SetScale(1, 1);

	FNT_PrintfPos(1, 1, "Player: [%2d; %2d @ %3d] || {%d; %d @ %d}", Player.tilex, Player.tiley, FINE2DEG(Player.position.angle), Player.position.origin[0], Player.position.origin[1], Player.position.angle);
	FNT_PrintfPos(54, 1, "%3d fps", r_fps);
	FNT_PrintfPos(53, 2, "%4d polys", r_polys);
	FNT_PrintfPos(1, 2, "Time: %02d%c%02d Kills:%d/%d Treasure:%d/%d Secrets:%d/%d",
										gamestate.TimeCount/60000, (gamestate.TimeCount/500)%2?':':' ', (gamestate.TimeCount/1000)%60,
										gamestate.killcount,			gamestate.killtotal,
										gamestate.treasurecount,	gamestate.treasuretotal,
										gamestate.secretcount,		gamestate.secrettotal);
	FNT_PrintfPos(1, 3, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
										areabyplayer[0],
										areabyplayer[1],
										areabyplayer[2],
										areabyplayer[3],
										areabyplayer[4],
										areabyplayer[5],
										areabyplayer[6],
										areabyplayer[7],
										areabyplayer[8],
										areabyplayer[9],
										areabyplayer[10],
										areabyplayer[11],
										areabyplayer[12],
										areabyplayer[13],
										areabyplayer[14],
										areabyplayer[15],
										areabyplayer[16],
										areabyplayer[17],
										areabyplayer[18],
										areabyplayer[19],
										areabyplayer[20],
										areabyplayer[21],
										areabyplayer[22],
										areabyplayer[23],
										areabyplayer[24],
										areabyplayer[25],
										areabyplayer[25],
										areabyplayer[27],
										areabyplayer[28],
										areabyplayer[29],
										areabyplayer[30],
										areabyplayer[31],
										areabyplayer[32],
										areabyplayer[33],
										areabyplayer[34],
										areabyplayer[35],
										areabyplayer[36]);
	FNT_PrintfPos(1+Player.areanumber, 4, "%d", Player.areanumber);
}
