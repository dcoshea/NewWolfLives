#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"
#include "Act_stat.h"

Guard_struct Guards[MAX_GUARDS+1], *New;
unsigned char NumGuards=0;
unsigned char add8dir[9]	={4, 5, 6, 7, 0, 1, 2, 3, 0};
unsigned char r_add8dir[9]={4, 7, 6, 5, 0, 1, 2, 3, 0};

// changes gurad's state to that defined in NewState
void A_StateChange(Guard_struct *Guard, en_state NewState)
{
	Guard->state=NewState;
	Guard->ticks=objstate[Guard->type][Guard->state].timeout; //0;
}

int DoGuard(Guard_struct *Guard) // FIXME: rewise!
{ // returns 0 if we must remove this Guard from Guards list, otherwise 1;
	think_t think;

// ------------------------- * non transitional Guard * -------------------------
	if(!Guard->ticks)
	{
		think=objstate[Guard->type][Guard->state].think;
		if(think)
		{
			think(Guard);
			if(Guard->state==st_remove) return 0;
		}
		return 1;
	}

// ------------------------- * transitional Guard * -------------------------
	Guard->ticks-=tics;
	while(Guard->ticks<=0)
	{
		think=objstate[Guard->type][Guard->state].action;			// end of state action
		if(think)
		{
			think(Guard);
			if(Guard->state==st_remove) return 0;
		}

		Guard->state=objstate[Guard->type][Guard->state].next_state;
		if(Guard->state==st_remove) return 0;

		if(!objstate[Guard->type][Guard->state].timeout)
		{
			Guard->ticks=0;
			break;
		}

		Guard->ticks += objstate[Guard->type][Guard->state].timeout;
	}

// ------------------------- * think * -------------------------
	think=objstate[Guard->type][Guard->state].think;
	if(think)
	{
		think(Guard);
		if(Guard->state==st_remove) return 0;
	}

	return 1;
}

void ProcessGuards(void)
{
	int n, relangle, tex;

	for(n=0; n<NumGuards; n++)
	{
		if( !DoGuard(&Guards[n]) )
		{ // remove guard from the game forever!
			RemoveActor(&Guards[n--]);
			continue;
		}

		Spr_SetPos(Guards[n].sprite, Guards[n].x, Guards[n].y, Guards[n].angle);
		tex=objstate[Guards[n].type][Guards[n].state].texture;
		if(objstate[Guards[n].type][Guards[n].state].rotate)
		{
			relangle=TransformPoint(Guards[n].x, Guards[n].y);
			// A little hack by me (I think rockets are wrong located in VSwap)
			if(Guards[n].type==en_rocket || Guards[n].type==en_hrocket)
				tex+=r_add8dir[Get8dir(WiseAngDiff(Guards[n].angle, relangle))];
			else
				tex+=  add8dir[Get8dir(WiseAngDiff(Guards[n].angle, relangle))];
			//^ (alt): Player.angle-Objects[n].angle; (As in doom
			//				 sprite do'nt depend from Player angle, only position)
		}
		Spr_SetTex(Guards[n].sprite, 0, tex);
	}
}

/* reset actors status */
void ResetGuards(void)
{
	memset(Guards, 0, sizeof(Guards));
	NumGuards=0;
	New=NULL;
}

/* save actors status */
void SaveGuards(FILE *fp)
{
	int n, tmp;

	fwrite(&NumGuards, sizeof(NumGuards), 1, fp);
	for(n=0; n<NumGuards; n++)
	{
		fwrite(&Guards[n], sizeof(Guards[n]), 1, fp);
		if(Guards[n].waitfordoor)
		{
			tmp=pack2shorts(Guards[n].waitfordoor->x, Guards[n].waitfordoor->y);
			fwrite(&tmp, sizeof(tmp), 1, fp);
		}
	}
}

/* load actors status */
void LoadGuards(FILE *fp)
{
	int n, tmp;

	fread(&NumGuards, sizeof(NumGuards), 1, fp);
	for(n=0; n<NumGuards; n++)
	{
		fread(&Guards[n], sizeof(Guards[n]), 1, fp);
		if(Guards[n].waitfordoor)
		{
			fread(&tmp, sizeof(tmp), 1, fp);
			Guards[n].waitfordoor=DoorMap[unpackshort1(tmp)][unpackshort2(tmp)];
		}
	}
}

Guard_struct *GetNewActor(void)
{
	if(NumGuards>MAX_GUARDS) return NULL;
	memset(&Guards[NumGuards], 0, sizeof(Guards[0]));
	return &Guards[NumGuards++];
}

void RemoveActor(Guard_struct *actor)
{
	Spr_RemoveSprite(actor->sprite);
	memmove(actor, actor+1, (int)(&Guards[NumGuards])-(int)(actor+1) );
	NumGuards--;
}

Guard_struct *SpawnActor(enemy_t which, int x, int y, dir4type dir)
{
	Guard_struct *new_actor;

	new_actor=GetNewActor();
	if(!new_actor) return NULL;

	new_actor->x=TILE2POS(x);
	new_actor->y=TILE2POS(y);
	new_actor->tilex=x;
	new_actor->tiley=y;
	new_actor->angle=dir4angle[dir];
	new_actor->dir=dir4to8[dir];
	new_actor->areanumber=CurMapData.areas[x][y];
	new_actor->type=which;
	new_actor->health=starthitpoints[gamestate.difficulty][which];
	new_actor->sprite=Spr_GetNewSprite();
	return new_actor;
}

/*
===============
= SpawnStand
===============
*/
void SpawnStand(enemy_t which, int x, int y, int dir)
{
	Guard_struct *self;

	self=SpawnActor(which, x, y, dir);
	if(!self) return;

	self->state=st_stand;
	self->speed=SPDPATROL;
	self->ticks=objstate[which][st_stand].timeout ? US_RndT()%objstate[which][st_stand].timeout + 1 : 0;
	self->flags|=FL_SHOOTABLE;
	if(CurMapData.tile_info[x][y] & TILE_IS_AMBUSH) self->flags|=FL_AMBUSH;

	gamestate.killtotal++;
}

/*
===============
= SpawnPatrol
===============
*/
void SpawnPatrol(enemy_t which, int x, int y, int dir)
{
	Guard_struct *self;

	self=SpawnActor(which, x, y, dir);
	if(!self) return;
	
	self->state=st_path1;
	self->speed= (which==en_dog) ? SPDDOG : SPDPATROL;
	self->distance=TILEGLOBAL;
	self->ticks=objstate[which][st_path1].timeout ? US_RndT()%objstate[which][st_path1].timeout + 1 : 0;
	self->flags|=FL_SHOOTABLE;

  gamestate.killtotal++;
}

void SpawnDeadGuard(enemy_t which, int x, int y)
{
	Guard_struct *self;

	self=SpawnActor(which, x, y, dir4_nodir);
	if(!self) return;
	
	self->state=st_dead;
	self->speed=0;
	self->health=0;
	self->ticks=objstate[which][st_dead].timeout ? US_RndT()%objstate[which][st_dead].timeout + 1 : 0;

  //gamestate.killtotal++; //<- a statue to my foolness (I debugged my programm all night, before noticed this ;(
}

void SpawnBoss(enemy_t which, int x, int y)
{
	Guard_struct *self;
	dir4type face;

	switch(which)
	{
	case en_boss:
	case en_schabbs:
	case en_fat:
	case en_hitler:
		face=dir4_south;
		break;
	case en_fake:
	case en_gretel:
	case en_gift:
		face=dir4_north;
		break;
	case en_trans:
	case en_uber:
	case en_will:
	case en_death:
	case en_angel:
	case en_spectre:
		face=dir4_nodir;
		break;
	default:
		face=dir4_nodir;
		break;
	}

	self=SpawnActor(which, x, y, face);
	if(!self) return;
	
	self->state=which==en_spectre?st_path1:st_stand;
	self->speed=SPDPATROL;
	self->health=starthitpoints[gamestate.difficulty][which];
 	self->ticks=objstate[which][st_stand].timeout ? US_RndT()%objstate[which][st_stand].timeout + 1 : 0;
	self->flags|=FL_SHOOTABLE|FL_AMBUSH;

	gamestate.killtotal++;
}

void SpawnGhosts(enemy_t which, int x, int y)
{
	Guard_struct *self;

	self=SpawnActor(which, x, y, dir4_nodir);
	if(!self) return;
	
	self->state=st_chase1;
	self->speed=SPDPATROL*3;
	self->health=starthitpoints[gamestate.difficulty][which];
	self->ticks=objstate[which][st_chase1].timeout ? US_RndT()%objstate[which][st_chase1].timeout + 1: 0;
	self->flags|=FL_AMBUSH;

  gamestate.killtotal++;
}
