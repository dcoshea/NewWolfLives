/************************************
  REVISION LOG ENTRY
  Revision By: DarkOne
  Revised on 7/31/00 14:48:39
  Comments: Artificall intellect for WolfGL!
 ************************************/

#define BJRUNSPEED	2048
#define BJJUMPSPEED	680
/*
===============
=
= SpawnBJVictory
=
===============
*/
void SpawnBJVictory(void)
{
	Guard_struct *bj;

	bj=SpawnActor(en_bj, POS2TILE(Player.position.origin[0]), POS2TILE(Player.position.origin[1]), dir4_north);
	if(!bj) return;

	bj->x=Player.position.origin[0];
	bj->y=Player.position.origin[1];
	bj->state=st_path1;
	bj->speed=BJRUNSPEED;
	bj->flags=FL_NONMARK; // FL_NEVERMARK;
	bj->temp2=6;
}

/*
===============
=
= T_BJRun
=
===============
*/
void T_BJRun(Guard_struct *Guard)
{
//	MoveObj(Guard, Guard->speed);
	
	if(!Guard->distance)
	{
		Guard->distance=TILEGLOBAL;
		if ( !(-- Guard->temp2) )
		{
			A_StateChange(Guard, st_shoot1);
			Guard->speed=BJJUMPSPEED;
			return;
		}
	}
}

/*
===============
=
= T_BJJump
=
===============
*/
void T_BJJump(Guard_struct *Guard)
{
//	MoveObj(Guard, Guard->speed);
}

/*
===============
=
= T_BJYell
=
===============
*/
void T_BJYell(Guard_struct *Guard)
{
	PlaySoundLocActor(YEAHSND, CHAN_VOICE, Guard);	// JAB
}

/*
===============
=
= T_BJDone
=
===============
*/
void T_BJDone(Guard_struct *Guard)
{
	Player.playstate = ex_victory;				// exit castle tile
}

// ------------------------- * Additional Functions * -------------------------

/*
=====================
= CheckLine
=
= Returns true if a straight line between the player and ob is unobstructed
=====================
*/
bool CheckLine(Guard_struct *Guard)
{
	return Map_CheckLine(Guard->x, Guard->y, Player.position.origin[0], Player.position.origin[1]);
}

char CanCloseDoor(int x, int y, char vert)
{
	int n;

	if(POS2TILE(Player.position.origin[0])==x && POS2TILE(Player.position.origin[1])==y) return 0;

	if(vert)
	{
		if(POS2TILE(Player.position.origin[1])==y)
		{
			if(POS2TILE(Player.position.origin[0]+CLOSEWALL)==x) return 0;
			if(POS2TILE(Player.position.origin[0]-CLOSEWALL)==x)	return 0;
		}
		for(n=0; n<NumGuards; n++)
		{
			if(Guards[n].tilex==x && Guards[n].tiley==y) return 0; // guard in door
			if(Guards[n].tilex==x-1 && Guards[n].tiley==y && POS2TILE(Guards[n].x+CLOSEWALL)==x) return 0; // guard in door
			if(Guards[n].tilex==x+1 && Guards[n].tiley==y && POS2TILE(Guards[n].x-CLOSEWALL)==x) return 0; // guard in door
		}
	}
	else
	{
		if(POS2TILE(Player.position.origin[0])==x)
		{
			if(POS2TILE(Player.position.origin[1]+CLOSEWALL)==y)	return 0;
			if(POS2TILE(Player.position.origin[1]-CLOSEWALL)==y)	return 0;
		}
		for(n=0; n<NumGuards; n++)
		{
			if(Guards[n].tilex==x && Guards[n].tiley==y) return 0; // guard in door
			if(Guards[n].tilex==x && Guards[n].tiley==y-1 && POS2TILE(Guards[n].y+CLOSEWALL)==y) return 0; // guard in door
			if(Guards[n].tilex==x && Guards[n].tiley==y+1 && POS2TILE(Guards[n].y-CLOSEWALL)==y) return 0; // guard in door
		}
	}

	return 1;
}