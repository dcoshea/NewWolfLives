/*************************************\
* PowerUps handling code              *
*                                     *
* part of NewWolf(wolf4ever) project  *
* (C) 2000-01 by DarkOne the Hacker   *
\*************************************/
#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"
#include "sprt_def.h"

typedef struct powerup_s
{
	int x, y;
	pow_t type;
	int sprite;
	struct powerup_s *prev, *next;
} powerup_t;

powerup_t *powerups=NULL;

powerup_t *Pow_Remove(powerup_t *powerup);
powerup_t *Pow_AddNew(void);

int Pow_Texture[pow_last]=
{
	SPR_STAT_34,		//pow_gibs
	SPR_STAT_38,		//pow_gibs2
	SPR_STAT_6,		//pow_alpo
	SPR_STAT_25,		//pow_firstaid
	SPR_STAT_20,		//pow_key1
	SPR_STAT_21,		//pow_key2
// not used
	SPR_STAT_20,	//pow_key3
	SPR_STAT_20,		//pow_key4

	SPR_STAT_29,		//pow_cross
	SPR_STAT_30,		//pow_chalice
	SPR_STAT_31,		//pow_bible
	SPR_STAT_32,		//pow_crown
	SPR_STAT_26,		//pow_clip
	SPR_STAT_26,		//pow_clip2
	SPR_STAT_27,		//pow_machinegun
	SPR_STAT_28,		//pow_chaingun
	SPR_STAT_24,		//pow_food
	SPR_STAT_33,		//pow_fullheal
// spear
	SPR_STAT_49,		//pow_25clip
	SPR_STAT_51,		//pow_spear
};
// ------------------------- * Devider * -------------------------

int Pow_Init(void)
{
	Pow_Reset();
	return 1;
}

// finctions to work with lists
void Pow_Reset(void)
{
	powerup_t *powerup=powerups;

	while(powerup)
		powerup=Pow_Remove(powerup);
	powerups=NULL;
}

/* save powerups status */
void Pow_Save(FILE *fp)
{
	powerup_t *powerup;
	int n;
	
	for(n=0, powerup=powerups; powerup; n++, powerup=powerup->next);
	fwrite(&n, sizeof(n), 1, fp);
	for(powerup=powerups; powerup; powerup=powerup->next)
		fwrite(powerup, sizeof(powerup_t), 1, fp);
}

/* load powerups status */
void Pow_Load(FILE *fp)
{
	powerup_t *powerup, pow;
	int n, tmp;
	
	fread(&tmp, sizeof(tmp), 1, fp);
	for(n=0; n<tmp; n++)
	{
		fread(&pow, sizeof(powerup_t), 1, fp);
		powerup=Pow_AddNew();
		powerup->sprite=pow.sprite;
		powerup->type=pow.type;
		powerup->x=pow.x;
		powerup->y=pow.y;
	}
}

powerup_t *Pow_Remove(powerup_t *powerup)
{
	powerup_t *next;

	if(powerup==NULL) return NULL;
	if(powerup->prev) powerup->prev->next=powerup->next;
	if(powerup->next) powerup->next->prev=powerup->prev;
	next=powerup->next;
	if(powerups==powerup) powerups=next; //fuck!
	free(powerup);
	return next;
}

powerup_t *Pow_AddNew(void)
{
	powerup_t *newp;
	
	newp=malloc(sizeof(powerup_t));
	newp->prev=NULL;
	newp->next=powerups;
	if(powerups)
		powerups->prev=newp;
	powerups=newp;
	return newp;
}

// ------------------------- * Powerup Code * -------------------------
// ADDME:
// If adding respawn code add it here! Will require another sub,
// which will be called on server side every frame or to say 0.5 sec
// Just remove sprite, when object is picked, leave powerup & set flag & time...

/*
** Pow_Give
**
** returns 1 powerup is picked up
*/
int Pow_Give(pow_t type)
{
	char *keynames[]={"gold", "silver", "?", "?"};

	switch(type)
	{
// ------------------------- * keys * -------------------------
	case pow_key1:
	case pow_key2:
	case pow_key3:
	case pow_key4:
		type-=pow_key1;
		PL_GiveKey(&Player, type);
		SD_PlaySound(GETKEYSND, CHAN_ITEM);
		Msg_Printf("Picked up a %s key.", keynames[type]);
		break;

// ------------------------- * treasure * -------------------------
	case pow_cross:
		PL_GivePoints(&Player, 100);
		SD_PlaySound(BONUS1SND, CHAN_ITEM);
		gamestate.treasurecount++;
		Msg_Printf("Picked up a cross.");
		break;
	case pow_chalice:
		PL_GivePoints(&Player, 500);
		SD_PlaySound(BONUS2SND, CHAN_ITEM);
		gamestate.treasurecount++;
		Msg_Printf("Picked up a chalice.");
		break;
	case pow_bible:
		PL_GivePoints(&Player, 1000);
		SD_PlaySound(BONUS3SND, CHAN_ITEM);
		gamestate.treasurecount++;
		Msg_Printf("Picked up a chest.");
		break;
	case pow_crown:
		PL_GivePoints(&Player, 5000);
		SD_PlaySound(BONUS4SND, CHAN_ITEM);
		gamestate.treasurecount++;
		Msg_Printf("Picked up a crown.");
		break;

// ------------------------- * health * -------------------------
	case pow_gibs:
		if(!PL_GiveHealth(&Player, 1, 11)) return 0;
		SD_PlaySound(SLURPIESND, CHAN_ITEM);
		Msg_Printf("BLOOD!");
		break;
	case pow_alpo:
		if(!PL_GiveHealth(&Player, 4, 0)) return 0;
		SD_PlaySound(HEALTH1SND, CHAN_ITEM);
		Msg_Printf("Picked up dog's food.");
		break;
	case pow_food:
		if(!PL_GiveHealth(&Player, 10, 0)) return 0;
		SD_PlaySound(HEALTH1SND, CHAN_ITEM);
		Msg_Printf("Picked up cold food.");
		break;
	case pow_firstaid:
		if(!PL_GiveHealth(&Player, 25, 0)) return 0;
		SD_PlaySound(HEALTH2SND, CHAN_ITEM);
		Msg_Printf("Picked up a medikit.");
		break;

// ------------------------- * weapon & ammo * -------------------------
	case pow_clip:
		if(!PL_GiveAmmo(&Player, AMMO_BULLETS, 8)) return 0;
		SD_PlaySound(GETAMMOSND, CHAN_ITEM);
		Msg_Printf("Picked up a clip.");
		break;
	case pow_clip2:
		if(!PL_GiveAmmo(&Player, AMMO_BULLETS, 4)) return 0;
		SD_PlaySound(GETAMMOSND, CHAN_ITEM);
		Msg_Printf("Picked up a used clip.");
		break;
	case pow_25clip:
		if(!PL_GiveAmmo(&Player, AMMO_BULLETS, 25)) return 0;
		SD_PlaySound(GETAMMOBOXSND, CHAN_ITEM);
		Msg_Printf("Picked up an ammo box.");
		break;

	case pow_machinegun:
		PL_GiveWeapon(&Player, WEAPON_AUTO);
		SD_PlaySound(GETMACHINESND, CHAN_ITEM);
		Msg_Printf("You got the machine gun!");
		break;
	case pow_chaingun:
		PL_GiveWeapon(&Player, WEAPON_CHAIN);
		SD_PlaySound(GETGATLINGSND, CHAN_ITEM);
		Msg_Printf("You got the chain gun!");

		Player.facecount=0;
		Player.face_gotgun=true;
		break;

// ------------------------- * artifacts * -------------------------
	case pow_fullheal:
		PL_GiveHealth(&Player, 999, 0);
		PL_GiveAmmo(&Player, AMMO_BULLETS, 25);
		PL_GiveLife(&Player);
		gamestate.treasurecount++;
		Msg_Printf("Extra life!");
		break;

// ------------------------- * Devider * -------------------------
	default:
		Con_Printf("Warning: Unknown item type: %d\n", type);
		break;
	}
	return 1;
}

/*
** Pow_Spawn
**
** x and y are in TILES
*/
void Pow_Spawn(int x, int y, int type)
{
	powerup_t *newp;

	CurMapData.tile_info[x][y]|=TILE_IS_POWERUP;
	newp=Pow_AddNew();
	newp->sprite=Spr_GetNewSprite();
	Spr_SetPos(newp->sprite, TILE2POS(newp->x=x), TILE2POS(newp->y=y), 0);
	newp->type=type;
	Spr_SetTex(newp->sprite, -1, Pow_Texture[type]);
	CurMapData.tile_info[x][y]|=TILE_IS_POWERUP;
// good place to update total treasure count!
}

/*
** Pow_PickUp
**
** x, y, is in TILES
** we will get here only if tile powerup flag is set!
** FIXME add an object ref, who wants to pick it up!
*/
void Pow_PickUp(int x, int y)
{
	powerup_t *pow;
	bool p_left=false, p_pick=false;

	for(pow=powerups; pow; pow=pow->next)
	{
check_again:
		if(pow->x==x && pow->y==y)
		{// got a powerup here
			if(Pow_Give(pow->type)) //FIXME script
			{// picked up this stuff, remove it!
				p_pick=true;
				Spr_RemoveSprite(pow->sprite);
				pow=Pow_Remove(pow);
				if(pow)
					goto check_again;
				else
					break;
			}
			else
			{// player do not need it, so may be next time!
				p_left=true;
			}
		}
	}
	if(p_pick) R_BonusFlash();
	if(p_left)
		CurMapData.tile_info[x][y]|= TILE_IS_POWERUP;
	else
		CurMapData.tile_info[x][y]&=~TILE_IS_POWERUP;
}