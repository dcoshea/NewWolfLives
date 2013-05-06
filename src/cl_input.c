#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"

/*
===============================================================================
KEY BUTTONS

Continuous button event tracking is complicated by the fact that two different
input sources (say, mouse button 1 and the control key) can both press the
same button, but the button should only be released when both of the
pressing key have been released.

When a key event issues a button command (+forward, +attack, etc), it appends
its key number as a parameter to the command so it can be matched up with
the release.

state bit 0 is the current state of the key
state bit 1 is edge triggered on the up to down transition
state bit 2 is edge triggered on the down to up transition
===============================================================================
*/

kbutton_t	in_left, in_right, in_forward, in_back;
kbutton_t	in_moveleft, in_moveright;
kbutton_t	in_strafe, in_speed, in_use, in_attack;

int in_impulse;

void KeyDown(kbutton_t *b)
{
	int k;
	char *c;

	c=Cmd_Argv(1);
	if(c[0])
		k=atoi(c);
	else
		k=-1;		// typed manually at the console for continuous down

	if(k==b->down[0] || k==b->down[1])
		return;		// repeating key
	
	if(!b->down[0])
		b->down[0]=k;
	else if(!b->down[1])
		b->down[1]=k;
	else
	{
		Con_Printf("Three keys down for a button!\n");
		return;
	}
	
	if(b->state & 1)
		return; // still down
	b->state|=1+2; // down + impulse down
}

void KeyUp(kbutton_t *b)
{
	int k;
	char *c;
	
	c=Cmd_Argv(1);
	if(c[0])
		k=atoi(c);
	else
	{// typed manually at the console, assume for unsticking, so clear all
		b->down[0]=b->down[1]=0;
		b->state=4;	// impulse up
		return;
	}

	if(b->down[0]==k)
		b->down[0]=0;
	else if(b->down[1]==k)
		b->down[1]=0;
	else
		return;		// key up without coresponding down (menu pass through)
	if(b->down[0] || b->down[1])
		return;		// some other key is still holding it down

	if(!(b->state & 1))
		return;		// still up (this should not happen)
	b->state&=~1;		// now up
	b->state|=4; 		// impulse up
}

void IN_LeftDown(void) {KeyDown(&in_left);}
void IN_LeftUp(void) {KeyUp(&in_left);}
void IN_RightDown(void) {KeyDown(&in_right);}
void IN_RightUp(void) {KeyUp(&in_right);}
void IN_ForwardDown(void) {KeyDown(&in_forward);}
void IN_ForwardUp(void) {KeyUp(&in_forward);}
void IN_BackDown(void) {KeyDown(&in_back);}
void IN_BackUp(void) {KeyUp(&in_back);}
void IN_MoveleftDown(void) {KeyDown(&in_moveleft);}
void IN_MoveleftUp(void) {KeyUp(&in_moveleft);}
void IN_MoverightDown(void) {KeyDown(&in_moveright);}
void IN_MoverightUp(void) {KeyUp(&in_moveright);}

void IN_SpeedDown(void) {KeyDown(&in_speed);}
void IN_SpeedUp(void) {KeyUp(&in_speed);}
void IN_StrafeDown(void) {KeyDown(&in_strafe);}
void IN_StrafeUp(void) {KeyUp(&in_strafe);}

void IN_AttackDown(void) {KeyDown(&in_attack);}
void IN_AttackUp(void) {KeyUp(&in_attack);}

void IN_UseDown (void) {KeyDown(&in_use);}
void IN_UseUp (void) {KeyUp(&in_use);}

void IN_Impulse (void) {in_impulse=atoi(Cmd_Argv(1));}

/*
===============
CL_KeyState

Returns:
0.25 if a key was pressed and released during the frame,
0.5  if it was pressed and held
0.75 if released and re-pressed this frame
0 if held then released, and
1.0 if held for the entire time
===============
*/
float CL_KeyState(kbutton_t *key)
{
	float val=0;
	bool impulsedown, impulseup, down;
	
	impulsedown=key->state & 2;
	impulseup=key->state & 4;
	down=key->state & 1;
	
	if(impulsedown && !impulseup)
		if(down)
			val=0.5; // pressed and held this frame
		else
			val=0; // I_Error ();
	if(impulseup && !impulsedown)
		if(down)
			val=0;	// I_Error ();
		else
			val=0;	// released this frame
	if(!impulsedown && !impulseup)
		if(down)
			val=1.0;	// held the entire frame
		else
			val=0;	  // up the entire frame
	if(impulsedown && impulseup)
		if(down)
			val=0.75;	// released and re-pressed this frame
		else
			val=0.25;	// pressed and released this frame

	key->state&=1; // clear impulses

	return val;
}

// ------------------------- * Devider * -------------------------
const int forwardmove[2]={20<<8, 40<<8}; 
const int sidemove[2]={19<<8, 35<<8}; 
const int angleturn[3]={DEG2FINE(2), DEG2FINE(4), DEG2FINE(1)};	// + slow turn 

/*
** CL_AdjustAngles
**
** Moves the local angle positions
*/
void CL_AdjustAngles(usercmd_t *cmd)
{
	float	speed;
	
	if((!allwaysrun->value && in_speed.state&1) || (allwaysrun->value && !(in_speed.state&1)))
		speed=(float)(tics*1.5/70.0); // frametime FIXME!
	else
		speed=(float)(tics/70.0); // frametime

	if(!(in_strafe.state&1))
		cmd->angle+=(int)(speed*23040*(CL_KeyState(&in_left)-CL_KeyState(&in_right)));
}

/*
** CL_BaseMove
**
** Send the intended movement message to the server
*/
void CL_BaseMove(usercmd_t *cmd)
{
	int speed;
	int forward, side;

	memset(cmd, 0, sizeof(usercmd_t));
	CL_AdjustAngles(cmd);

	speed=in_speed.state&1; // 0 for walking, 1 for running
	if(allwaysrun->value) speed=1-speed; // invert if allwaysrun

	forward=side=0;

	if(in_strafe.state&1)
	{
		cmd->sidemove+=(int)(sidemove[speed]*CL_KeyState(&in_right));
		cmd->sidemove-=(int)(sidemove[speed]*CL_KeyState(&in_left));
	}

	cmd->sidemove+=(int)(sidemove[speed]*CL_KeyState(&in_moveright));
	cmd->sidemove-=(int)(sidemove[speed]*CL_KeyState(&in_moveleft));

	cmd->forwardmove+=(int)(forwardmove[speed]*CL_KeyState(&in_forward));
	cmd->forwardmove-=(int)(forwardmove[speed]*CL_KeyState(&in_back));

	cmd->impulse=in_impulse;
	in_impulse=0;

	if(in_use.state&1) cmd->buttons|=BT_USE;
	if(in_attack.state&1) cmd->buttons|=BT_ATTACK;
}

// ------------------------- * Devider * -------------------------

/*
============
CL_InitInput
============
*/
void CL_InitInput(void)
{
	Cmd_AddCommand("+left", IN_LeftDown);
	Cmd_AddCommand("-left", IN_LeftUp);
	Cmd_AddCommand("+right", IN_RightDown);
	Cmd_AddCommand("-right", IN_RightUp);
	Cmd_AddCommand("+forward", IN_ForwardDown);
	Cmd_AddCommand("-forward", IN_ForwardUp);
	Cmd_AddCommand("+back", IN_BackDown);
	Cmd_AddCommand("-back", IN_BackUp);
	Cmd_AddCommand("+strafe", IN_StrafeDown);
	Cmd_AddCommand("-strafe", IN_StrafeUp);
	Cmd_AddCommand("+moveleft", IN_MoveleftDown);
	Cmd_AddCommand("-moveleft", IN_MoveleftUp);
	Cmd_AddCommand("+moveright", IN_MoverightDown);
	Cmd_AddCommand("-moveright", IN_MoverightUp);
	Cmd_AddCommand("+speed", IN_SpeedDown);
	Cmd_AddCommand("-speed", IN_SpeedUp);
	Cmd_AddCommand("+attack", IN_AttackDown);
	Cmd_AddCommand("-attack", IN_AttackUp);
	Cmd_AddCommand("+use", IN_UseDown);
	Cmd_AddCommand("-use", IN_UseUp);

	Cmd_AddCommand("impulse", IN_Impulse);
}