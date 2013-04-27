/*************************************************************************
** NewWolf
** Copyright (C) 1999-2002 by DarkOne
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**************************************************************************
** console variables
*************************************************************************/
#include "common.h"

// ------------------------- * Devider * -------------------------
cvar_t *cvar_vars=NULL;

// ------------------------- * Devider * -------------------------

/*
** Cvar_InfoValidate
*/
bool Cvar_InfoValidate(char *s)
{
	if(strchr(s, '\\') || strchr(s, '"') || strchr(s, ';'))
		return false;
	else
		return true;
}

/*
** Cvar_FindVar
*/
cvar_t *Cvar_FindVar(char *var_name)
{
	cvar_t *var;

	for(var=cvar_vars; var; var=var->next)
		if(!strcmp(var_name, var->name))
			return var;

	return NULL;
}

/*
** Cvar_VariableValue
**
** returns 0 if not defined or non numeric
*/
float Cvar_VariableValue(char *var_name)
{
	cvar_t *var;

	var=Cvar_FindVar(var_name);
	if(!var) return 0;
	return (float)atof(var->string);
}

/*
** Cvar_VariableInteger
**
** returns 0 if not defined or non numeric
*/
int Cvar_VariableInteger(char *var_name)
{
	cvar_t *var;

	var=Cvar_FindVar(var_name);
	if(!var) return 0;
	return atoi(var->string);
}

/*
** Cvar_VariableString
**
** returns an empty string if not defined
*/
char *Cvar_VariableString(char *var_name)
{
	cvar_t *var;

	var=Cvar_FindVar(var_name);
	if(!var) return "";
	return var->string;
}

/*
** Cvar_Link
**
** links the variable in a sorted position
*/
void Cvar_Link(cvar_t *var)
{
	cvar_t *t, **prev;

	prev=&cvar_vars;
	for(t=cvar_vars; t; t=t->next)
	{
		if(strcmp(t->name, var->name)>=0)
		{
			*prev=var;
			var->next=t;
			return;
		}
		prev=&t->next;
	}

// last in the list
	*prev=var;
	var->next=NULL;
}

/*
** Cvar_Get
**
** here Cvars would be spawned into engine
** If the variable already exists, the value will not be set
** The flags will be or'ed in if the variable exists.
*/
cvar_t *Cvar_Get(char *var_name, char *var_value, int flags)
{
	cvar_t *var;

	var=Cvar_FindVar(var_name);
	if(var)
	{
		var->flags|=flags;
		if(!(flags&CVAR_NODEFAULT))
		{
			Z_Free(var->def);
			var->def=copystring(var_value);
		}
		return var;
	}

	if(!var_value) return NULL;

	var=Z_Malloc(sizeof(cvar_t));
	var->name=copystring(var_name);
	var->string=copystring(var_value);
	var->modified=true;
	var->value=(float)atof(var->string);
	var->flags=flags;
	var->def=(flags&CVAR_NODEFAULT)?NULL:copystring(var_value);

	Cvar_Link(var);

	return var;
}

/*
** Cvar_SetEx
*/
cvar_t *Cvar_SetEx(char *var_name, char *value, bool force)
{
	cvar_t *var;

	var=Cvar_FindVar(var_name);
	if(!var) // create it
		return Cvar_Get(var_name, value, CVAR_NODEFAULT);

	if(!force && var->flags&CVAR_NOSET)
	{
		Con_Printf("%s is read only.\n", var_name);
		return var;
	}

	if(var->flags&(CVAR_USERINFO|CVAR_SERVERINFO) && !Cvar_InfoValidate(value))
	{
		Con_Printf("invalid info cvar value\n");
		return var;
	}

	if(!strcmp(value, var->string)) return var; // not changed

	var->modified=true;
	if(var->flags&CVAR_RESTART && !force)
		Con_Printf("%s would take effect upon restarting.\n", var_name);

	Z_Free(var->string); // free the old value string

	var->string=copystring(value);
	var->value=(float)atof(var->string);

	return var;
}

/*
** Cvar_Set
**
** will create the variable if it doesn't exist
*/
cvar_t *Cvar_Set(char *var_name, char *value)
{
	return Cvar_SetEx(var_name, value, false);
}

/*
** Cvar_ForceSet
**
** will set the variable even if NOSET or LATCH flags set
*/
cvar_t *Cvar_ForceSet(char *var_name, char *value)
{
	return Cvar_SetEx(var_name, value, true);
}

/*
** Cvar_SetValue
**
** expands value to a string and calls Cvar_Set
*/
void Cvar_SetValue(char *var_name, float value)
{
	char val[32];

	if(value==(int)value)
		sprintf(val, "%d", (int)value);
	else
		sprintf(val, "%f", value);
	Cvar_SetEx(var_name, val, false);
}

/*
** Cvar_SetValue
**
** expands value to a string and calls Cvar_Set
*/
void Cvar_ForceSetValue(char *var_name, float value)
{
	char val[32];

	if(value==(int)value)
		sprintf(val, "%d", (int)value);
	else
		sprintf(val, "%f", value);
	Cvar_SetEx(var_name, val, true);
}

/*
** Cvar_SetInteger
**
** expands value to a string and calls Cvar_Set
*/
void Cvar_SetInteger(char *var_name, int value)
{
	char val[32];

	sprintf(val, "%d", (int)value);
	Cvar_SetEx(var_name, val, false);
}

/*
** Cvar_ForceSetInteger
**
** expands value to a string and calls Cvar_Set
*/
void Cvar_ForceSetInteger(char *var_name, int value)
{
	char val[32];

	sprintf(val, "%d", (int)value);
	Cvar_SetEx(var_name, val, true);
}

/*
** Cvar_FullSet
**
** overwrites flags
*/
cvar_t *Cvar_FullSet(char *var_name, char *value, int flags)
{
	cvar_t *var;
	
	var=Cvar_FindVar(var_name);
	if(!var) // create it
		return Cvar_Get(var_name, value, flags);

	var->modified=true;

	Z_Free(var->string); // free the old value string
	var->string=copystring(value);
	var->value=(float)atof(var->string);
	var->flags=flags;
	Z_Free(var->def);
	var->def=(flags&CVAR_NODEFAULT)?NULL:copystring(value);

	return var;
}

/*
** Cvar_Command
**
** called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known command.
** Returns true if the command was a variable reference that was handled (print or change)
**
** Handles variable inspection and changing from the console
*/
bool Cvar_Command(void)
{
	cvar_t *cvar;

// check variables
	cvar=Cvar_FindVar(Cmd_Argv(0));
	if(!cvar) return false;
		
// perform a variable print or set
	if(Cmd_Argc()==1)
	{
		if(cvar->def)
			Con_Printf("\"%s\" is \"%s\" default \"%s\"\n", cvar->name, cvar->string, cvar->def);
		else
			Con_Printf("\"%s\" is \"%s\"\n", cvar->name, cvar->string);
		return true;
	}

	Cvar_Set(cvar->name, Cmd_Argv(1));
	return true;
}

/*
** Cvar_Set_f
**
** Allows setting and defining of arbitrary cvars from console
*/
void Cvar_Set_f(void)
{
	if(Cmd_Argc()!=3)
	{
		Con_Printf("use: set <variable> <value>\n");
		return;
	}

	Cvar_Set(Cmd_Argv(1), Cmd_Argv(2));
}

/*
** Cvar_Reset_f
**
** Resets cvar to it's defailt value
*/
void Cvar_Reset_f(void)
{
	cvar_t *var;

	if(Cmd_Argc()!=2)
	{
		Con_Printf("use: reset <variable>\n");
		return;
	}

	var=Cvar_FindVar(Cmd_Argv(1));
	if(!var)
	{
		Con_Printf("variable \"%s\" not found\n", Cmd_Argv(1));
		return;
	}

	if(var->def) Cvar_Set(Cmd_Argv(1), var->def);
}

/*
** Cvar_List_f
**
** lists all cvars
*/
void Cvar_List_f(void)
{
	cvar_t *var;
	char *mask;
	int n, m;

	if(Cmd_Argc()==2)
		mask=Cmd_Argv(1);
	else
		mask=NULL;

	for(n=0, m=0, var=cvar_vars; var; var=var->next, n++)
	{
		if(!TestWildCard(var->name, mask)) continue;
		m++;

		Con_Printf(var->flags&CVAR_ARCHIVE?"*":" ");
		Con_Printf(var->flags&CVAR_USERINFO?"U":" ");
		Con_Printf(var->flags&CVAR_SERVERINFO?"S":" ");
		Con_Printf(var->flags&CVAR_NOSET?"-":var->flags&CVAR_LATCH?"L":" ");
		if(var->def)
			Con_Printf(" %s \"%s\" / \"%s\"\n", var->name, var->string, var->def);
		else
			Con_Printf(" %s \"%s\"\n", var->name, var->string);
	}
	if(mask)
		Con_Printf(" % 4d cvars matching \"%s\"\n", m, mask);
	Con_Printf(" % 4d cvars total\n", n);
}

/*
** Cvar_Toggle_f
**
** toggles a variable between its '1' and '0' values
*/
void Cvar_Toggle_f(void)
{
	cvar_t *var;

	if(Cmd_Argc()!=2)
	{
		Con_Printf("use: toggle <variable>\n");
		return;
	}

	var=Cvar_FindVar(Cmd_Argv(1));
	if(!var)
	{
		Con_Printf("variable \"%s\" not found\n", Cmd_Argv(1));
		return;
	}

	if(var->value)
		Cvar_Set(var->name, "0");
	else
		Cvar_Set(var->name, "1");
}

/*
** Cvar_WriteVariables
**
** Writes lines containing "set variable value" for
** all variables with the archive flag set to true.
*/
void Cvar_WriteVariables(FILE *f)
{
	cvar_t	*var;
	
	for(var=cvar_vars; var; var=var->next)
		if(var->flags&CVAR_ARCHIVE)
			fprintf(f, "set %s \"%s\"\n", var->name, var->string);
}

/*
** Cvar_Init
**
** cvar initialization
*/
void Cvar_Init(void)
{
	Cmd_AddCommand("cvarlist", Cvar_List_f);
	Cmd_AddCommand("set", Cvar_Set_f);
	Cmd_AddCommand("reset", Cvar_Reset_f);
	Cmd_AddCommand("toggle", Cvar_Toggle_f);
}
