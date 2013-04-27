/************************************
  REVISION LOG ENTRY
  Revision By: DarkOne
  Revised on 7/11/2001 12:32:59
  Comments: Artificall intellect for WolfGL!
						Actor Specific Functions
						Common Functions are in ai_com module
 ************************************/
#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"

/*
==================
= A_DeathScream
=  do a death scream as a 3D sound
=  depending on actor type
==================
*/
void A_DeathScream(Guard_struct *self)
{
	if(!IS_SHAREWARE)
	{// Perdez sound... in secret levels! FIXME!!!
		if(IS_SPEAR && ((gamestate.map==18 || gamestate.map==19) && !US_RndT()) || 
			!IS_SPEAR && (gamestate.map==9 && !US_RndT()) )
		{
			switch(self->type)
			{
				case en_mutant:
				case en_guard:
				case en_officer:
				case en_ss:
				case en_dog:
					PlaySoundLocActor(DEATHSCREAM6SND, CHAN_VOICE, self);
					return;
			}
		}
	}

	switch(self->type)
	{
	case en_mutant:
		PlaySoundLocActor(AHHHGSND, CHAN_VOICE, self);
		break;
	case en_guard:
	{
		int sounds[9]=
		{
			DEATHSCREAM1SND,
			DEATHSCREAM2SND,
			DEATHSCREAM3SND,
			DEATHSCREAM4SND,
			DEATHSCREAM5SND,
			DEATHSCREAM7SND,
			DEATHSCREAM8SND,
			DEATHSCREAM9SND
		};
		if(IS_SHAREWARE)
			PlaySoundLocActor(sounds[US_RndT()%2], CHAN_VOICE, self);
		else
			PlaySoundLocActor(sounds[US_RndT()%8], CHAN_VOICE, self);
	}	break;
	case en_officer:
		PlaySoundLocActor(NEINSOVASSND, CHAN_VOICE, self);
		break;
	case en_ss:
		PlaySoundLocActor(LEBENSND, CHAN_VOICE, self);
		break;
	case en_dog:
		PlaySoundLocActor(DOGDEATHSND, CHAN_VOICE, self);
		break;

	case en_boss:
		SD_PlaySound(MUTTISND, CHAN_VOICE);
		break;
	case en_schabbs:
		SD_PlaySound(MEINGOTTSND, CHAN_VOICE);
		break;
	case en_fake:
		SD_PlaySound(HITLERHASND, CHAN_VOICE);
		break;
	case en_mecha:
		SD_PlaySound(SCHEISTSND, CHAN_VOICE);
		break;
	case en_hitler:
		SD_PlaySound(EVASND, CHAN_VOICE);
		break;
	case en_gretel:
		SD_PlaySound(MEINSND, CHAN_VOICE);
		break;
	case en_gift:
		SD_PlaySound(DONNERSND, CHAN_VOICE);
		break;
	case en_fat:
		SD_PlaySound(ROSESND, CHAN_VOICE);
		break;
	case en_spectre:
		SD_PlaySound(GHOSTFADESND, CHAN_VOICE);
		break;
	case en_angel:
		SD_PlaySound(ANGELDEATHSND, CHAN_VOICE);
		break;
	case en_trans:
		SD_PlaySound(TRANSDEATHSND, CHAN_VOICE);
		break;
	case en_uber:
		SD_PlaySound(UBERDEATHSND, CHAN_VOICE);
		break;
	case en_will:
		SD_PlaySound(WILHELMDEATHSND, CHAN_VOICE);
		break;
	case en_death:
		SD_PlaySound(KNIGHTDEATHSND, CHAN_VOICE);
		break;
	}
}

/*
===============
=
= FirstSighting
=
= Puts an actor into attack mode and possibly reverses the direction
= if the player is behind it
=
===============
*/
void A_FirstSighting(Guard_struct *self)
{
	switch(self->type)
	{
	case en_guard:
		PlaySoundLocActor(HALTSND, CHAN_VOICE, self);
		self->speed*=3;			// go faster when chasing player
		break;
	case en_officer:
		PlaySoundLocActor(SPIONSND, CHAN_VOICE, self);
		self->speed*=5;			// go faster when chasing player
		break;
	case en_mutant:
		self->speed*=3;			// go faster when chasing player
		break;
	case en_ss:
		PlaySoundLocActor(SCHUTZADSND, CHAN_VOICE, self);
		self->speed*=4;			// go faster when chasing player
		break;
	case en_dog:
		PlaySoundLocActor(DOGBARKSND, CHAN_VOICE, self);
		self->speed*=2;			// go faster when chasing player
		break;

	case en_boss:
		SD_PlaySound(GUTENTAGSND, CHAN_VOICE);
		self->speed=SPDPATROL*3;	// go faster when chasing player
		break;
	case en_gretel:
		SD_PlaySound(KEINSND, CHAN_VOICE);
		self->speed*=3;			// go faster when chasing player
		break;
	case en_gift:
		SD_PlaySound(EINESND, CHAN_VOICE);
		self->speed*=3;			// go faster when chasing player
		break;
	case en_fat:
		SD_PlaySound(ERLAUBENSND, CHAN_VOICE);
		self->speed*=3;			// go faster when chasing player
		break;
	case en_schabbs:
		SD_PlaySound(SCHABBSHASND, CHAN_VOICE);
		self->speed*=3;			// go faster when chasing player
		break;
	case en_fake:
		SD_PlaySound(TOT_HUNDSND, CHAN_VOICE);
		self->speed*=3;			// go faster when chasing player
		break;
	case en_mecha:
		SD_PlaySound(DIESND, CHAN_VOICE);
		self->speed*=3;			// go faster when chasing player
		break;
	case en_hitler:
		SD_PlaySound(DIESND, CHAN_VOICE);
		self->speed*=5;			// go faster when chasing player
		break;
	case en_blinky:
	case en_clyde:
	case en_pinky:
	case en_inky:
		self->speed*=2;			// go faster when chasing player
		break;
/**************** SPEAR ****************/
	case en_spectre:
		SD_PlaySound(GHOSTSIGHTSND, CHAN_VOICE);
		self->speed=800;			// go faster when chasing player
		break;
	case en_angel:
		SD_PlaySound(ANGELSIGHTSND, CHAN_VOICE);
		self->speed=1536;			// go faster when chasing player
		break;
	case en_trans:
		SD_PlaySound(TRANSSIGHTSND, CHAN_VOICE);
		self->speed=1536;			// go faster when chasing player
		break;
	case en_uber:
		self->speed=3000;			// go faster when chasing player
		break;
	case en_will:
		SD_PlaySound(WILHELMSIGHTSND, CHAN_VOICE);
		self->speed=2048;			// go faster when chasing player
		break;
	case en_death:
		SD_PlaySound(KNIGHTSIGHTSND, CHAN_VOICE);
		self->speed=2048;			// go faster when chasing player
		break;
	default:
		return;
	}
	A_StateChange(self, st_chase1);
	if(self->waitfordoor) self->waitfordoor=NULL;	// ignore the door opening command
	self->dir=dir8_nodir;
	self->flags |= FL_ATTACKMODE|FL_FIRSTATTACK;
}

/*
===============
=
= A_KillActor
=
===============
*/
void A_KillActor(Guard_struct *self)
{
	int	tilex, tiley;

	tilex=self->tilex=self->x >> TILESHIFT; // drop item on center
	tiley=self->tiley=self->y >> TILESHIFT;

	switch(self->type)
	{
	case en_guard:
		PL_GivePoints(&Player, 100);
		Pow_Spawn(tilex, tiley, pow_clip2);
		break;
	case en_officer:
		PL_GivePoints(&Player, 400);
		Pow_Spawn(tilex, tiley, pow_clip2);
		break;
	case en_mutant:
		PL_GivePoints(&Player, 700);
		Pow_Spawn(tilex, tiley, pow_clip2);
		break;
	case en_ss:
		PL_GivePoints(&Player, 500);
		if(Player.items&ITEM_WEAPON_3) // have a schmeiser?
			Pow_Spawn(tilex, tiley, pow_clip2);
		else
			Pow_Spawn(tilex, tiley, pow_machinegun);
		break;
	case en_dog:
		PL_GivePoints(&Player, 200);
		break;

	case en_boss:
		PL_GivePoints(&Player, 5000);
		Pow_Spawn(tilex, tiley, pow_key1);
		break;
	case en_gretel:
		PL_GivePoints(&Player, 5000);
		Pow_Spawn(tilex, tiley, pow_key1);
		break;
	case en_gift:
		PL_GivePoints(&Player, 5000);
		//gamestate.killx = player->x;
		//gamestate.killy = player->y;
		break;
	case en_fat:
		PL_GivePoints(&Player, 5000);
		//gamestate.killx = player->x;
		//gamestate.killy = player->y;
		break;
	case en_schabbs:
		PL_GivePoints(&Player, 5000);
		//gamestate.killx = player->x;
		//gamestate.killy = player->y;
		A_DeathScream(self);
		break;
	case en_fake:
		PL_GivePoints(&Player, 2000);
		break;
	case en_mecha:
		PL_GivePoints(&Player, 5000);
		break;
	case en_hitler:
		PL_GivePoints(&Player, 5000);
		//gamestate.killx = player->x;
		//gamestate.killy = player->y;
		A_DeathScream(self);
		break;

	case en_spectre:
		PL_GivePoints(&Player, 200);
		break;
	case en_angel:
		PL_GivePoints(&Player, 5000);
		break;
	case en_trans:
		PL_GivePoints(&Player, 5000);
		Pow_Spawn(tilex, tiley, pow_key1);
		break;
	case en_uber:
		PL_GivePoints(&Player, 5000);
		Pow_Spawn(tilex, tiley, pow_key1);
		break;
	case en_will:
		PL_GivePoints(&Player, 5000);
		Pow_Spawn(tilex, tiley, pow_key1);
		break;
	case en_death:
		PL_GivePoints(&Player, 5000);
		Pow_Spawn(tilex, tiley, pow_key1);
		break;
	}
	A_StateChange(self, st_die1);
	gamestate.killcount++;
	self->flags &= ~FL_SHOOTABLE;
	self->flags |= FL_NONMARK;
}

/*
===================
=
= DamageActor
=
= Called when the player succesfully hits an enemy.
=
= Does damage points to enemy ob, either putting it into a stun frame or
= killing it.
=
= FIXME: make: Guard_struct *attacker
===================
*/
void A_DamageActor(Guard_struct *self, int damage)
{
	Player.madenoise=1;

// do double damage if shooting a non attack mode actor
	if(!(self->flags&FL_ATTACKMODE)) damage<<=1;

	self->health-=damage;

	if(self->health<=0)
		A_KillActor(self);
	else
	{
		if(!(self->flags&FL_ATTACKMODE))
			A_FirstSighting(self);		// put into combat mode

		switch(self->type)		// dogs only have one hit point
		{
		case en_guard:
		case en_officer:
		case en_mutant:
		case en_ss:
			if(self->health&1)
				A_StateChange(self, st_pain);
			else
				A_StateChange(self, st_pain1);
			break;
		}
	}
}

// ------------------------- * Devider * -------------------------

// Hitler:

// plays Mecha Sound in 3D
void A_MechaSound(Guard_struct *self)
{
	if(areabyplayer[self->areanumber])
		PlaySoundLocActor(MECHSTEPSND, CHAN_VOICE, self);
}

// plays Slurpie Sound
void A_Slurpie(Guard_struct *self)
{
	SD_PlaySound(SLURPIESND, CHAN_VOICE);
}

/*
===============
= A_HitlerMorph
=  Spawns new actor, where Mecha Hitler is dead
===============
*/
void A_HitlerMorph(Guard_struct *self)
{
	int hitpoints[4]={500, 700, 800, 900};
	Guard_struct *hitler;

	hitler=GetNewActor();
	if(!hitler) return;

	hitler->x=self->x;//
	hitler->y=self->y;//
	hitler->distance=self->distance;
	hitler->tilex=self->tilex;//
	hitler->tiley=self->tiley;//
	hitler->angle=self->angle;//
	hitler->dir=self->dir;//
	hitler->health=hitpoints[gamestate.difficulty];
	hitler->areanumber=self->areanumber;
	hitler->state=st_chase1;//
	hitler->type=en_hitler; //
	hitler->speed=SPDPATROL*5;//
	hitler->ticks=0;//
	hitler->flags=self->flags | FL_SHOOTABLE; //
	hitler->sprite=Spr_GetNewSprite();

	gamestate.killtotal++;
}

// ------------------------- * Angel of Death * -------------------------
/* Angel cann't shoot more then 3 sparks in a row.
** It will get tired!
*/
static int angel_temp=0;

// plays Angel Breathing Sound
void A_Breathing(Guard_struct *self)
{
	SD_PlaySound(ANGELTIREDSND, CHAN_VOICE);
}

/*
=================
= A_StartAttack
=================
*/
void A_StartAttack(Guard_struct *self)
{
	angel_temp=0;
}

/*
=================
= A_Relaunch
=================
*/
void A_Relaunch(Guard_struct *self)
{
	if(++angel_temp==3)
	{
		A_StateChange(self, st_pain);
		return;
	}

	if(US_RndT()&1)
	{
		A_StateChange(self, st_chase1);
		return;
	}
}

/*
=================
= A_Victory
=================
*/
void A_Victory(Guard_struct *self)
{
//	playstate=ex_victory;
//	M_Victory_f();
}

// ------------------------- * ghosts * -------------------------
/*
===============
= A_Dormant
===============
*/
void A_Dormant(Guard_struct *self)
{
	int deltax, deltay;
	int xl, xh, yl, yh, x, y, n;

	deltax=self->x-Player.position.origin[0];
	if(deltax<-MINACTORDIST || deltax>MINACTORDIST) goto moveok;
	deltay=self->y-Player.position.origin[1];
	if(deltay<-MINACTORDIST || deltay>MINACTORDIST) goto moveok;
	return;

moveok:
	xl=(self->x-MINDIST)>>TILESHIFT;
	xh=(self->x+MINDIST)>>TILESHIFT;
	yl=(self->y-MINDIST)>>TILESHIFT;
	yh=(self->y+MINDIST)>>TILESHIFT;

	for(y=yl; y<=yh; y++)
		for(x=xl; x<=xh; x++)
		{
			if(CurMapData.tile_info[x][y] & TILE_IS_SOLIDTILE) return;
			for(n=0; n<NumGuards; n++)
			{
				if(Guards[n].state>=st_die1) continue;
				if(Guards[n].tilex==x && Guards[n].tiley==y) return; // another guard in path
			}
		}

	self->flags|=FL_AMBUSH|FL_SHOOTABLE;
	self->flags&=~FL_ATTACKMODE;
	self->dir=dir8_nodir;
	A_StateChange(self, st_path1);
}

/*
===============
= A_StartDeathCam
=  Must start Death Cam
===============
*/
void A_StartDeathCam(Guard_struct *self)
{
}

// ------------------------- * Missiles * -------------------------

/*
=================
= A_Smoke
=  rockets emmit smoke
=================
*/
void A_Smoke(Guard_struct *self)
{
	Guard_struct *smoke;

	smoke=GetNewActor();
	if(!smoke) return;
	smoke->x=self->x;//
	smoke->y=self->y;//
	smoke->tilex=self->tilex;//
	smoke->tiley=self->tiley;//
	smoke->state=st_die1;//
	smoke->type=(self->type==en_hrocket) ? en_hsmoke : en_smoke;
	smoke->ticks=6;//
	smoke->flags=FL_NEVERMARK; //
	smoke->sprite=Spr_GetNewSprite();
}

/*
===================
=
= ProjectileTryMove
=
= returns true if move ok
===================
*/
#define PROJSIZE	0x2000

bool ProjectileTryMove(Guard_struct *self)
{
	int xl, yl, xh, yh, x, y;

	xl=(self->x-PROJSIZE)>>TILESHIFT;
	yl=(self->y-PROJSIZE)>>TILESHIFT;

	xh=(self->x+PROJSIZE)>>TILESHIFT;
	yh=(self->y+PROJSIZE)>>TILESHIFT;

	// Checking for solid walls:
	for(y=yl; y<=yh; y++)
	{
		for(x=xl; x<=xh; x++)
		{
// FIXME: decide what to do with statics & Doors!
			if(CurMapData.tile_info[x][y]&(TILE_IS_WALL|TILE_IS_BLOCK)) return false;
			if(CurMapData.tile_info[x][y]&TILE_IS_DOOR)
			{
				if(Door_Opened(x, y)!=DOOR_FULLOPEN) return false;
			}
		}
	}
// FIXME: Projectile will fly through objects (even guards & columns) - must fix to create rocket launcher!
	return true;
}

/*
=================
=
= T_Projectile
=
=================
*/
#define PROJECTILESIZE	0xC000

void T_Projectile(Guard_struct *self)
{
	int deltax, deltay, speed, damage;

	speed=self->speed*tics;

	deltax=(int)(speed*CosTable[self->angle]);
	deltay=(int)(speed*SinTable[self->angle]);

	if(deltax> TILEGLOBAL) deltax= TILEGLOBAL;
	if(deltax<-TILEGLOBAL) deltax=-TILEGLOBAL; // my
	if(deltay> TILEGLOBAL) deltay= TILEGLOBAL;
	if(deltay<-TILEGLOBAL) deltay=-TILEGLOBAL; // my

	self->x+=deltax;
	self->y+=deltay;

	deltax=abs(self->x-Player.position.origin[0]);
	deltay=abs(self->y-Player.position.origin[1]);

	if(!ProjectileTryMove(self))
	{
		if(self->type==en_rocket || self->type==en_hrocket)
		{ // rocket run into obstacle, draw explosion!
			PlaySoundLocActor(MISSILEHITSND, CHAN_WEAPON, self);
			A_StateChange(self, st_die1);
		}
		else
		{
			A_StateChange(self, st_remove); // mark for removal
		}
		return;
	}

	if(deltax<PROJECTILESIZE && deltay<PROJECTILESIZE)
	{	// hit the player
		switch(self->type)
		{
		case en_needle:
			damage=(US_RndT()>>3)+20;
			break;
		case en_rocket:
		case en_hrocket:
		case en_spark:
			damage=(US_RndT()>>3)+30;
			break;
		case en_fire:
			damage=(US_RndT()>>3);
			break;
		default:
			damage=0;
			break;
		}

		PL_Damage(&Player, self, damage);
		A_StateChange(self, st_remove); // mark for removal
		return;
	}

	self->tilex=self->x>>TILESHIFT;
	self->tiley=self->y>>TILESHIFT;
}
