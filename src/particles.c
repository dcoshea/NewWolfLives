// particle engine handling [PE]
#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"

// ------------------------- * Devider * -------------------------
particle_t particles[MAX_PARTICLES];
particle_t *active_particles, *free_particles;

// ------------------------- * Devider * -------------------------

/*
** PE_ClearParticles
**
** clears all particles
*/
void PE_ClearParticles(void)
{
	int n;

	free_particles=&particles[0];
	active_particles=NULL;

	for(n=0; n<MAX_PARTICLES; n++)
		particles[n].next=&particles[n+1];

	particles[MAX_PARTICLES-1].next=NULL;
}

/*
** PE_GetParticle
**
** returns free particle; it is made active
*/
particle_t *PE_GetParticle(void)
{
	particle_t *p;

	if(!free_particles) return NULL; // no free particles

	p=free_particles;
	free_particles=p->next;
	p->next=active_particles;
	active_particles=p;
	
	return p;
}

/*
** PE_GetParticle
**
** returns free particle; it is made active
** if there are no free particles, takes one active, with the smallest ttl
*/
particle_t *PE_ForceGetParticle(void)
{
	particle_t *p, *best;

	p=PE_GetParticle();
	if(p) return p;

	for(best=p=active_particles; p; p=p->next)
		if(p->ttl<best->ttl)
			best=p;

	return best;
}

/*
** PE_KillParticle
**
** removes particle from active particles list
** and adds it to free particles list
** returns next particle in the list
*/
particle_t *PE_KillParticle(particle_t *p, particle_t *prev)
{
	if(!p) return NULL; // no particle to kill

// relink list
	if(!prev) // first particle in the list
		active_particles=p->next;
	else
		prev->next=p->next;

// add current particle to free list
	p->next=free_particles;
	free_particles=p;

	return prev?prev->next:active_particles;
}

/*
** PE_ProcessParticles
**
** handles all active particles
*/
void PE_ProcessParticles(void)
{
	particle_t *p, *prev;

	for(p=active_particles, prev=NULL; p; prev=p, p=p->next)
	{
		while(p)
		{
			p->ttl-=tics;
			if(p->ttl<=0)
				p=PE_KillParticle(p, prev);
			else
				break;
		}
		if(!p) break;

		p->org[0]+=p->vel[0]*tics;
		p->org[1]+=p->vel[1]*tics;
		p->org[2]+=p->vel[2]*tics;
		p->vel[2]-=p->mass*tics/10000.0f; // gravity, lowers vertical speed

		// TODO: add collision detection
		if(p->org[2]<LOWERZCOORD)
		{
			p->org[2]=LOWERZCOORD+(LOWERZCOORD-p->org[2])*0.3f;
			p->vel[2]=-p->vel[2]*0.3f;
		}
	}
}

/*
** PE_Emmit_f
*/
void PE_Emmit_f(void)
{
	particle_t *p;

	p=PE_GetParticle();
	p->org[0]=Player.position.origin[0]/FLOATTILE;
	p->org[1]=Player.position.origin[1]/FLOATTILE;
	p->org[2]=0.1f;
	p->vel[0]=p->vel[1]=p->vel[2]=0;
	p->color.r=0xF8;
	p->color.g=0xF7;
	p->color.b=0x4E;
	p->color.a=100;
	p->ttl=350;
	p->mass=0.1f;

	PE_fxWallShot(Player.position.origin[0]/FLOATTILE, Player.position.origin[1]/FLOATTILE, Player.position.angle, true);
}

/*
** PE_fxWallShot
**
** particle effect: bullet hits a wall
*/
void PE_fxWallShot(float x, float y, int angle, bool vertical)
{
	particle_t *p;
	int n, a;

	if(vertical)
		angle=ANG_180-angle;
	else
		angle=-angle;

// ricochet
	for(n=0; n<20; n++)
	{
		a=NormalizeAngle(angle-DEG2FINE(30)+rand()%DEG2FINE(60));

		p=PE_GetParticle();
		if(!p) return; // not enough particles
		p->org[0]=x; p->org[1]=y; p->org[2]=0;
		p->vel[0]=(float)(CosTable[a]/300.0); p->vel[1]=(float)(SinTable[a]/300.0); p->vel[2]=0;
		p->mass=5.0f+rnd()*5.0f;
		p->color.r=p->color.g=p->color.b=140+(rand()&63); p->color.a=0xFF;
		p->ttl=70;
		p->type=PART_BRICK;
	}
}

/*
** PE_fxBlood
**
** particle effect: enemy erects blood
*/
void PE_fxBlood(float x, float y, int angle, int damage)
{
	particle_t *p;
	int n, a;

	angle=angle-ANG_180;

	for(n=0; n<damage*2; n++)
	{
		a=NormalizeAngle(angle-DEG2FINE(30)+rand()%DEG2FINE(60));

		p=PE_GetParticle();
		if(!p) return; // not enough particles
		p->org[0]=x+rnd()/3.0f; p->org[1]=y+rnd()/3.0f; p->org[2]=-0.1f+0.3f*rnd();
		p->vel[0]=(float)(CosTable[a]/200.0); p->vel[1]=(float)(SinTable[a]/200.0); p->vel[2]=0.005f-rnd()/100.0f;
		p->mass=1.0f+rnd();
		p->color.r=224+(rand()&31); p->color.g=p->color.b=0; p->color.a=0xFF;
		p->ttl=70;
		p->type=PART_BLOOD;
	}
}

/*
** PE_Init
*/
void PE_Init(void)
{
	PE_ClearParticles();
	Cmd_AddCommand("emmit", PE_Emmit_f);
}