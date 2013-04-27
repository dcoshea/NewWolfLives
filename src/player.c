#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"

player_t Player; // player struct (pos, health etc...)

// ------------------------- * local * -------------------------

struct atkinf
{
	char tics, attack, frame; // attack is 1 for gun, 2 for knife
} attackinfo[4][14]= // 4 guns, 14 frames max for every gun!
{
{ {6,0,1},{6,2,2},{6,0,3},{6,-1,4} },
{ {6,0,1},{6,1,2},{6,0,3},{6,-1,4} },
{ {6,0,1},{6,1,2},{6,3,3},{6,-1,4} },
{ {6,0,1},{6,1,2},{6,4,3},{6,-1,4} },
};

// ------------------------- * CONTROLs * -------------------------

/*
** PL_ChangeWeapon
**
** changes weapon /returns false if impossible/
*/
bool PL_ChangeWeapon(player_t *self, int weapon)
{
	unsigned itemflag;
	
	itemflag=ITEM_WEAPON_1<<weapon;

	if(self->ammo[AMMO_BULLETS]==0 && weapon!=WEAPON_KNIFE)
	{
		Msg_Printf("Not enough ammo.");
		return false;
	}
	if(!(self->items&itemflag)) 
	{
		Msg_Printf("No weapon.");
		return false;
	}

	self->weapon=self->pendingweapon=weapon;
	self->attackframe=self->attackcount=self->weaponframe=0;

	return true;
}

/*
** PL_Use
**
** Called if player pressed USE button
** returns true if player used something
*/
bool PL_Use(player_t *self)
{
	int x, y, dir;

	dir=Get4dir(self->position.angle);
	x=self->tilex+dx4dir[dir];
	y=self->tiley+dy4dir[dir];

	if(CurMapData.tile_info[x][y]&TILE_IS_DOOR)
		return Door_TryUse(DoorMap[x][y], Player.items);
	if(CurMapData.tile_info[x][y]&TILE_IS_SECRET)
		return PW_Push(x, y, dir);
	if(CurMapData.tile_info[x][y]&TILE_IS_ELEVATOR)
	{
		int newtex;
		
		switch(dir)
		{
		case dir4_east:
		case dir4_west:
			newtex=CurMapData.wall_tex_x[x][y]+=2;
			break;
		case dir4_north:
		case dir4_south:
			return false; // don't allow to press elevator rails
		}
		Vid_CacheTex(newtex); // cache switch texture
		
		if(CurMapData.tile_info[self->tilex][self->tiley]&TILE_IS_SLEVEL)
		{
			Msg_Printf("You have found a secret level!");
			self->playstate=ex_secretlevel;
		}
		else
		{
			Msg_Printf("Level complete!");
			self->playstate=ex_complete;
		}
		SD_PlaySound(LEVELDONESND, CHAN_BODY);

// if paying or recording demo
		if(gamestate.demorecord)
			DEMO_FinishRecord();// FIXME
		else if(gamestate.demoplayback)
		{
			DEMO_FinishPlay();
			M_ToggleMenu_f(); // FIXME
		}
		M_Inter_f();
		return true;
	}

	//SD_PlaySound(DONOTHINGSND, CHAN_BODY); //FIXME: annoying sound
	return false;
}

/*
** PL_KnifeAttack
*/
void PL_KnifeAttack(player_t *self)
{
	Guard_struct *closest;
	int dist, d1, n, shot_dist, damage;

	SD_PlaySound(ATKKNIFESND, CHAN_PWEAPON);

// actually fire
	dist=0x7fffffff;
	closest=NULL;

	for(n=0; n<NumGuards; n++)
	{
		if(Guards[n].flags&FL_SHOOTABLE) // && Guards[n].flags&FL_VISABLE
		{
			shot_dist=Point2LineDist(Guards[n].x-self->position.origin[0], Guards[n].y-self->position.origin[1], self->position.angle);
			if(shot_dist>(2*TILEGLOBAL/3)) continue; // miss
			d1=LineLen2Point(Guards[n].x-self->position.origin[0], Guards[n].y-self->position.origin[1], self->position.angle);
			if(d1<0 || d1>dist) continue;
			if(!CheckLine(&Guards[n])) continue; // obscured

			dist=d1;
			closest=&Guards[n];
		}
	}

	if(!closest || dist>TILE2POS(1)) return; // missed if futhurer than 1.5 tiles

	damage=US_RndT()>>4;
	A_DamageActor(closest, damage); // hit something
	PE_fxBlood(POS2TILEf(closest->x), POS2TILEf(closest->y), self->position.angle, damage);
}

/*
** PL_GunAttack
*/
void PL_GunAttack(player_t *self)
{
	Guard_struct *closest;
	int damage;
	int dx, dy, dist;
	int d1, shot_dist, n;

	switch(self->weapon)
	{
	case WEAPON_PISTOL:
		SD_PlaySound(ATKPISTOLSND, CHAN_PWEAPON);
		break;
	case WEAPON_AUTO:
		SD_PlaySound(ATKMACHINEGUNSND, CHAN_PWEAPON);
		break;
	case WEAPON_CHAIN:
		SD_PlaySound(ATKGATLINGSND, CHAN_PWEAPON);
		break;
	}
	self->madenoise=true;

	dist=0x7fffffffl;
	closest=NULL;

	for(n=0; n<NumGuards; n++)
	{
		if(Guards[n].flags&FL_SHOOTABLE) // && Guards[n].flags&FL_VISABLE
		{
			shot_dist=Point2LineDist(Guards[n].x-self->position.origin[0], Guards[n].y-self->position.origin[1], self->position.angle);
			if(shot_dist>(2*TILEGLOBAL/3)) continue; // miss
			d1=LineLen2Point(Guards[n].x-self->position.origin[0], Guards[n].y-self->position.origin[1], self->position.angle);
			if(d1<0 || d1>dist) continue;
			if(!CheckLine(&Guards[n])) continue; // obscured

			dist=d1;
			closest=&Guards[n];
		}
	}

	if(!closest) // missed
	{
		r_trace_t trace;

		trace.a=NormalizeAngle(self->position.angle-DEG2FINE(2)+rand()%(DEG2FINE(4)));
		trace.x=self->position.origin[0];
		trace.y=self->position.origin[1];
		trace.flags=TRACE_BULLET;
		trace.tile_vis=NULL;
		R_Trace(&trace);
		PE_fxWallShot(POS2TILEf(trace.x), POS2TILEf(trace.y), trace.a, trace.flags&TRACE_HIT_VERT);
		if(trace.flags&TRACE_HIT_DOOR) SD_PlaySound(SHOOTDOORSND, CHAN_AUTO);
		return;
	}

// hit something
	dx=abs(closest->tilex-self->tilex);
	dy=abs(closest->tiley-self->tiley);
	dist=max_of_2(dx, dy);

	if(dist<2)
		damage=US_RndT()/4;
	else if(dist<4)
		damage=US_RndT()/6;
	else
	{
		if(US_RndT()/12<dist) return; // missed
		damage=US_RndT()/6;
	}

	A_DamageActor(closest, damage);
	PE_fxBlood(POS2TILEf(closest->x), POS2TILEf(closest->y), self->position.angle, damage);
}

// ------------------------- * player movement * -------------------------
#define STOPSPEED 0x0D00
#define FRICTION  0.25f
#define MAXMOVE 	(MINDIST*2-1)

/*
** TryMove
**
** returns true if move ok
** debug: use pointers to optimize
*/
bool PL_TryMove(player_t *self)
{
	int xl, yl, xh, yh, x, y;
	int d, n;

	xl=POS2TILE(Player.position.origin[0]-PLAYERSIZE);
	yl=POS2TILE(Player.position.origin[1]-PLAYERSIZE);
	xh=POS2TILE(Player.position.origin[0]+PLAYERSIZE);
	yh=POS2TILE(Player.position.origin[1]+PLAYERSIZE);

	// Cheching for solid walls:
	for(y=yl; y<=yh; y++)
		for(x=xl; x<=xh; x++)
	{
		if(CurMapData.tile_info[x][y]&TILE_IS_SOLIDTILE) return 0;
		if(CurMapData.tile_info[x][y]&TILE_IS_DOOR && Door_Opened(x, y)!=DOOR_FULLOPEN) return 0;
	}

// check for actors
	for(n=0; n<NumGuards; n++)
	{
		if(Guards[n].state>=st_die1) continue;
		d=self->position.origin[0]-Guards[n].x;
		if(d<-MINACTORDIST || d>MINACTORDIST) continue;
		d=self->position.origin[1]-Guards[n].y;
		if(d<-MINACTORDIST || d>MINACTORDIST) continue;

		return false;
	}

	return true;
}

/*
** ClipMove
*/
void PL_ClipMove(player_t *self, int xmove, int ymove)
{
	int basex, basey;

	basex=self->position.origin[0];
	basey=self->position.origin[1];

	self->position.origin[0]+=xmove;
	self->position.origin[1]+=ymove;
	if(PL_TryMove(self)) return; // we moved as we wanted

	if(	self->flags&PL_FLAG_CLIP &&
			self->position.origin[0]>TILE2POS(0) && self->position.origin[0]<TILE2POS(63) &&
			self->position.origin[1]>TILE2POS(0) && self->position.origin[1]<TILE2POS(63))
		return; // we can move through walls

//	SD_PlaySound(HITWALLSND, CHAN_BODY); // another anoying sound

	if(xmove)	// don't bother eating shit if we don't move x!
	{
		self->position.origin[0]=basex+xmove;
		self->position.origin[1]=basey;
		if(PL_TryMove(self)) return; // May be we'll move only X direction?
	}
	if(ymove)	// don't bother eating shit if we don't move y!
	{
		self->position.origin[0]=basex;
		self->position.origin[1]=basey+ymove;
		if(PL_TryMove(self)) return; // May be we'll move only Y direction?
	}

// movement blocked; we must stay on one place... :(
	self->position.origin[0]=basex;
	self->position.origin[1]=basey;
}

/*
** ControlMovement
**
** Changes player's angle and position
*/
void PL_ControlMovement(player_t *self, usercmd_t *cmd)
{
	int angle, spd;

// rotation
	angle=self->position.angle=cmd->angle;

//	if(cmd->forwardmove || cmd->sidemove)
		self->movx=self->movy=0; // clear accumulated movement

	if(cmd->forwardmove)
	{
		spd=cmd->tics*cmd->forwardmove;
		self->movx+=(int)(spd*CosTable[angle]);
		self->movy+=(int)(spd*SinTable[angle]);
	}
	if(cmd->sidemove)
	{
		spd=cmd->tics*cmd->sidemove;
		self->movx+=(int)(spd*SinTable[angle]);
		self->movy-=(int)(spd*CosTable[angle]);
	}

	if(!self->movx && !self->movy) return;
	
#ifdef SPEAR
	funnyticount=0; // ZERO FUNNY COUNTER IF MOVED! // FIXME!
#endif
	self->speed=self->movx+self->movy;

// bound movement
	if(self->movx>MAXMOVE) self->movx=MAXMOVE;
	else if(self->movx<-MAXMOVE) self->movx=-MAXMOVE;
	if(self->movy>MAXMOVE) self->movy=MAXMOVE;
	else if(self->movy<-MAXMOVE) self->movy=-MAXMOVE;

// move player and clip movement to walls (check for no-clip mode here)
	PL_ClipMove(self, self->movx, self->movy);
	self->tilex=POS2TILE(self->position.origin[0]);
	self->tiley=POS2TILE(self->position.origin[1]);

	Pow_PickUp(self->tilex, self->tiley);

// Checking for area change
	if(CurMapData.areas[self->tilex][self->tiley]>=0 &&
		 CurMapData.areas[self->tilex][self->tiley]!=Player.areanumber)
	{
		Player.areanumber=CurMapData.areas[self->tilex][self->tiley];
		Ar_ConnectAreas(Player.areanumber);
	}

	//if(*(mapsegs[1]+offset)==EXITTILE) VictoryTile(); FIXME

// slow down
//	if(abs(self->movx)<STOPSPEED && abs(self->movy)<STOPSPEED)
//		self->movx=self->movy=0; // stop moving
//	else
//	{
//		self->movx=(int)(self->movx*FRICTION);
//		self->movy=(int)(self->movy*FRICTION);
//	}

}

/*
** PL_PlayerAttack
*/
void PL_PlayerAttack(player_t *self, bool re_attack)
{
	struct atkinf *cur;

	self->attackcount-=tics;
	while(self->attackcount<=0)
	{
		cur=&attackinfo[self->weapon][self->attackframe];
		switch(cur->attack)
		{
		case -1:
			self->flags&= ~PL_FLAG_ATTCK;
			if(!self->ammo[AMMO_BULLETS])
				self->weapon=WEAPON_KNIFE;
			else if(self->weapon!=self->pendingweapon)
				self->weapon=self->pendingweapon;
			self->attackframe=self->weaponframe=0;
			return;
		case 4:
			if(!self->ammo[AMMO_BULLETS]) break;
			if(re_attack) self->attackframe-=2;
		case 1:
			if(!self->ammo[AMMO_BULLETS]) // can only happen with chain gun
			{
				self->attackframe++;
				break;
			}
			PL_GunAttack(self);
			self->ammo[AMMO_BULLETS]--;
			break;
		case 2:
			PL_KnifeAttack(self);
			break;
		case 3:
			if(self->ammo[AMMO_BULLETS] && re_attack)
				self->attackframe-=2;
			break;
		}

		self->attackcount+=cur->tics;
		self->attackframe++;
		self->weaponframe=attackinfo[self->weapon][self->attackframe].frame;
	}
}

/*
** PL_Process
**
** player think function
*/
void PL_Process(player_t *self, usercmd_t *cmd)
{
	int n;

	self->madenoise=false;

	PL_ControlMovement(self, cmd);

	if(self->flags&PL_FLAG_ATTCK)
		PL_PlayerAttack(self, cmd->buttons&BT_ATTACK);
	else
	{
		if(cmd->buttons&BT_USE)
		{
			if(!(self->flags&PL_FLAG_REUSE) && PL_Use(self))
				self->flags|=PL_FLAG_REUSE;
		}
		else
			self->flags&= ~PL_FLAG_REUSE;

		if(cmd->buttons&BT_ATTACK)
		{
			self->flags|=PL_FLAG_ATTCK;

			self->attackframe=0;
			self->attackcount=attackinfo[self->weapon][0].tics;
			self->weaponframe=attackinfo[self->weapon][0].frame;
		}
	}

// process impulses
	switch(cmd->impulse)
	{
	case 0:
		break; // no impulse
	case 1:
	case 2:
	case 3:
	case 4:
		PL_ChangeWeapon(self, cmd->impulse-1);
		break;
	case 10: // next weapon /like in Quake/ FIXME: weapprev, weapnext
		self->pendingweapon=self->weapon;
		for(n=0; n<4; n++)
		{
			if(++self->weapon>WEAPON_CHAIN) self->weapon=WEAPON_KNIFE;
			if(PL_ChangeWeapon(self, self->weapon)) break;
		}
		self->weapon=self->pendingweapon;
		break;
	default:
		Con_Printf("Unknown Impulse: %d\n", cmd->impulse);
		break;
	}

// process artifacts' timed bonuses
	if(self->artifacts[ARTIF_INVUL])
	{
		self->artifacts[ARTIF_INVUL]-=tics;
		if(self->artifacts[ARTIF_INVUL]<0) self->artifacts[ARTIF_INVUL]=0;
	}
	if(self->artifacts[ARTIF_INVIS])
	{
		self->artifacts[ARTIF_INVIS]-=tics;
		if(self->artifacts[ARTIF_INVIS]<0) self->artifacts[ARTIF_INVIS]=0;
	}
	if(self->artifacts[ARTIF_VISION])
	{
		self->artifacts[ARTIF_VISION]-=tics;
		if(self->artifacts[ARTIF_VISION]<0) self->artifacts[ARTIF_VISION]=0;
	}
}

// ------------------------- * misc stuff * -------------------------

/*
** PL_Reset
**
** reset player status
*/
void PL_Reset(void)
{
	memset(&Player, 0, sizeof(Player));
	Player.playstate=ex_notingame;
}

/*
** PL_SavePlayer
**
** save player status
*/
void PL_SavePlayer(FILE *fp)
{
	fwrite(&Player, sizeof(Player), 1, fp);
}

/*
** PL_LoadPlayer
**
** load player status
*/
void PL_LoadPlayer(FILE *fp)
{
	fread(&Player, sizeof(Player), 1, fp);
	Player.LastAttacker=NULL;
}

/*
** PL_Spawn
**
** Spawn Player
*/
void PL_Spawn(placeonplane_t location)
{
	Player.position=location;
	Player.tilex=POS2TILE(location.origin[0]);
	Player.tiley=POS2TILE(location.origin[1]);
	Player.areanumber=CurMapData.areas[Player.tilex][Player.tiley];
	if(Player.areanumber<0) Player.areanumber=36;
	Ar_ConnectAreas(Player.areanumber);
}

// ------------------------- * debug commands /cheat codes/ * -------------------------

void PL_goobers_f(void)
{
	Msg_Printf("You've got everything!");
	PL_GiveHealth(&Player, 999, 0);
	PL_GiveAmmo(&Player, AMMO_BULLETS, 99);
	PL_GiveWeapon(&Player, WEAPON_AUTO);
	PL_GiveWeapon(&Player, WEAPON_CHAIN);
	PL_GiveKey(&Player, KEY_GOLD);
	PL_GiveKey(&Player, KEY_SILVER);
}

void PL_hurt_f(void)
{
	Msg_Printf("You are a sadomasochist!");
	Player.health=1;
	Player.armor=0;
	memset(Player.ammo, 0, sizeof(Player.ammo));
}

void PL_god_f(void)
{
	Player.flags^=PL_FLAG_GOD;
	Msg_Printf("God mode %s", Player.flags&PL_FLAG_GOD?"ON":"OFF");
}

void PL_noclip_f(void)
{
	Player.flags^=PL_FLAG_CLIP;
	Msg_Printf("No clipping %s", Player.flags&PL_FLAG_CLIP?"ON":"OFF");
}

void PL_notarget_f(void)
{
	Player.flags^=PL_FLAG_TARGET;
	Msg_Printf("No target %s", Player.flags&PL_FLAG_TARGET?"ON":"OFF");
}

// ------------------------- * Devider * -------------------------

void PL_Init(void)
{
	PL_Reset();

	Cmd_AddCommand("god", PL_god_f);
	Cmd_AddCommand("noclip", PL_noclip_f);
	Cmd_AddCommand("notarget", PL_notarget_f);

	Cmd_AddCommand("goobers", PL_goobers_f);
	Cmd_AddCommand("hurt", PL_hurt_f);
}

// ------------------------- * environment interraction * -------------------------
#define EXTRAPOINTS 40000	// points for an extra life

/*
** PL_Damage
*/
void PL_Damage(player_t *self, Guard_struct *attacker, int points)
{
	int saved;

	self->LastAttacker=attacker;

	if(self->artifacts[ARTIF_INVUL] || self->flags&PL_FLAG_GOD/* || gamestate.victoryflag FIXME*/) return;
	if(gamestate.difficulty==gd_baby) points>>=2;
	
	if(self->armor) // armor
	{
		if(self->armor&ARMOR_2_FLAG)
			saved=points/2; // better armor
		else
			saved=points/3;
		
		if((self->armor&ARMOR_MASK)<=saved) // armor is used up
		{
			saved=self->armor&ARMOR_MASK;
			self->armor=0;
		}
		else
			self->armor=(self->armor&ARMOR_2_FLAG)|((self->armor&ARMOR_MASK)-saved);
		points-=saved;
	}

	self->health-=points;

	if(self->health<=0)
	{
		Msg_Printf("You are dead..");
		self->health=0;
		self->playstate=ex_dead;

		if(gamestate.demorecord)
		{
			DEMO_FinishRecord();
			M_ToggleMenu_f(); // FIXME
		}
		else if(gamestate.demoplayback)
		{
			DEMO_FinishPlay();
			M_ToggleMenu_f(); // FIXME
		}
		
		M_Dead_f();
	}

	R_DamageFlash(points);
	Player.face_gotgun=false;

// MAKE BJ'S EYES BUG IF MAJOR DAMAGE!
// P.S. It's Spear's feature!
	if(points>30 && Player.health!=0)
	{
		Player.face_ouch=true;
		Player.facecount=0;
	}
}

/*
** PL_GiveHealth
**
** gives player some HP
** max can be:
**   0 - natural player's health limit (100 or 150 with augment)
**  >0 - indicates the limit
**
** returns true if player needs this health
*/
bool PL_GiveHealth(player_t *self, int points, int max)
{
	if(max==0) max=self->items&ITEM_AUGMENT?150:100;

	if(self->health>=max) return false; // doesn't need this health
	self->health+=points;
	if(self->health>max) self->health=max;

	Player.face_gotgun=false;

	return true; // took it
}

/*
** PL_GiveAmmo
**
** gives player some ammo
** returns true if player needs this ammo
** FIXME: it's broken a little (with ammo types)
*/
bool PL_GiveAmmo(player_t *self, int type, int ammo)
{
	int max_ammo[AMMO_TYPES]={99};
	int max;

	max=max_ammo[type];
	if(self->items&ITEM_BACKPACK) max*=2;

	if(self->ammo[type]>=max) return false; // don't need

	if(!self->ammo[type] && !self->attackframe) // knife was out
		self->weapon=self->pendingweapon;

	self->ammo[type]+=ammo;
	if(self->ammo[type]>max) self->ammo[type]=max;

	return true;
}

/*
** PL_GiveWeapon
*/
void PL_GiveWeapon(player_t *self, int weapon)
{
	unsigned itemflag;

	PL_GiveAmmo(self, AMMO_BULLETS, 6); // give some ammo with a weapon

	itemflag=ITEM_WEAPON_1<<weapon;
	if(self->items&itemflag)
		return; // player owns this weapon
	else
	{
		self->items|=itemflag;
		self->weapon=self->pendingweapon=weapon;
	}
}

/*
** PL_GiveLife
*/
void PL_GiveLife(player_t *self)
{
	if(self->lives<9) self->lives++;

	SD_PlaySound(BONUS1UPSND, CHAN_ITEM);
}

/*
** PL_GiveArmor
**
** returns true if player picked up armor
*/
bool PL_GiveArmor(player_t *self, bool mega, int points)
{
	if(points<(self->armor&ARMOR_MASK)) return false;
	
	if(mega)
		self->armor=ARMOR_2_FLAG|points;
	else
		self->armor=points;

	return true;
}

/*
** PL_GiveArmorBonus
*/
void PL_GiveArmorBonus(player_t *self, int points, int max)
{
	self->armor+=points;
	if((self->armor&ARMOR_MASK)>max)
		self->armor=(self->armor&ARMOR_2_FLAG)|max;
}

/*
** PL_GivePoints
*/
void PL_GivePoints(player_t *self, long points)
{
	self->score+=points;
	while(self->score>=self->next_extra)
	{
		self->next_extra+=EXTRAPOINTS;
		Msg_Printf("You have scored for extra life!");
		PL_GiveLife(self);
	}
}

/*
** PL_GiveKey
*/
void PL_GiveKey(player_t *self, int key)
{
	self->items|=ITEM_KEY_1<<key;
}

/*
** PL_GiveArtifact
*/
void PL_GiveArtifact(player_t *self, int type)
{
	switch(type)
	{
	case ARTIF_INVUL:
		self->artifacts[ARTIF_INVUL]=30*70;		// invul lasts 30 seconds
		return;
	case ARTIF_VISION:
		self->artifacts[ARTIF_VISION]=60*70;	// vison lasts 60 seconds
		return;
	case ARTIF_INVIS:
		self->artifacts[ARTIF_INVIS]=45*70;		// invisibility lasts 45 seconds
		return;
	}
}

// ------------------------- * Spawning/reseting * -------------------------

/*
** PL_NewGame
**
** Set up player for the new game
*/
void PL_NewGame(player_t *self)
{
	memset(self, 0, sizeof(player_t));

	self->health=100;
	self->ammo[AMMO_BULLETS]=8;
	self->lives=3;

	self->weapon=self->pendingweapon=WEAPON_PISTOL;
	self->items=ITEM_WEAPON_1|ITEM_WEAPON_2;
	self->next_extra=EXTRAPOINTS;
}

/*
** PL_NextLevel
**
** Set up player for level transition
*/
void PL_NextLevel(player_t *self)
{
	memset(self->artifacts, 0, sizeof(self->artifacts));
	self->old_score=self->score;
	self->attackcount=self->attackframe=self->weaponframe=0;
	self->flags=0;

	self->items&= ~(ITEM_KEY_1|ITEM_KEY_2|ITEM_KEY_3|ITEM_KEY_4);
}

/*
** PL_Reborn
**
** reborn dead player - returns false if no lives left
*/
bool PL_Reborn(player_t *self)
{
	if(--self->lives<0) return false;

	memset(self->artifacts, 0, sizeof(self->artifacts));
	self->health=100;
	self->ammo[AMMO_BULLETS]=8;
	self->score=self->old_score;
	self->attackcount=self->attackframe=self->weaponframe=0;
	self->flags=0;

	self->weapon=self->pendingweapon=WEAPON_PISTOL;
	self->items=ITEM_WEAPON_1|ITEM_WEAPON_2;
	if(IS_SPEAR)
	{
		// On the en_angel level, need to give the player the gold key so that
		// they can get out of the room they picked up pow_spear from.
		if(gamestate.map==21)
		{
			self->items|=ITEM_KEY_1;
		}
	}

	return true;
}
