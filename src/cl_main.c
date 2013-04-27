#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"

// ------------------------- * Globals * -------------------------
cvar_t *cl_name, *cl_color;

cvar_t *lookstrafe, *allwaysrun;

// ------------------------- * Devider * -------------------------

/*
** CL_SendCmd
*/
void CL_SendCmd(usercmd_t *cmd)
{
	if(gamestate.demoplayback)
		DEMO_ReadCmd(cmd);
	else
	{
//		cmd->angle=Player.position.angle;
		CL_BaseMove(cmd);	// get basic movement from keyboard
		In_Move(cmd);			// allow mice or other external controllers to add to the move
		cmd->tics=tics;
		cmd->angle+=Player.position.angle;
		Player.position.angle=cmd->angle=NormalizeAngle(cmd->angle);

		if(gamestate.demorecord) DEMO_WriteCmd(cmd);
	}
}

void CL_Init(void)
{	
	CL_InitInput();
	
// register our commands
	cl_name=Cvar_Get("name", "player", CVAR_ARCHIVE);

	lookstrafe=Cvar_Get("lookstrafe", "0", CVAR_ARCHIVE);
	allwaysrun=Cvar_Get("allwaysrun", "0", CVAR_ARCHIVE);
}
