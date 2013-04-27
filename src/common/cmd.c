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
** script commands processing module
*************************************************************************/
#include "../WolfDef.h"

// ------------------------- * Devider * -------------------------
#define	MAX_ARGS					80
#define	MAX_ALIAS_NAME		32
#define	ALIAS_LOOP_COUNT	16

typedef struct cmdalias_s
{
	struct cmdalias_s	*next;
	char	name[MAX_ALIAS_NAME];
	char	*value;
} cmdalias_t;

cmdalias_t *cmd_alias;
int alias_count; // for detecting runaway loops
bool cmd_wait;

// ------------------------- * COMMAND BUFFER * -------------------------
sizebuf_t	cmd_text;
byte cmd_text_buf[8192];

/*
** Cbuf_Init
*/
void Cbuf_Init(void)
{
// space for commands and script files
	SZ_Init(&cmd_text, cmd_text_buf, sizeof(cmd_text_buf));
}

/*
** Cbuf_AddText
**
** Adds command text at the end of the buffer
*/
void Cbuf_AddText(char *text)
{
	int	l;
	
	l=strlen(text);

	if(cmd_text.cursize+l>=cmd_text.maxsize)
	{
		Con_Printf("Cbuf_AddText: overflow\n");
		return;
	}
	SZ_Write(&cmd_text, text, l);
}

/*
** Cbuf_InsertText
**
** Adds command text immediately after the current command
** Adds a \n to the text
** FIXME: actually change the command buffer to do less copying
*/
void Cbuf_InsertText(char *text)
{
	char *temp;
	int templen;

// copy off any commands still remaining in the exec buffer
	templen=cmd_text.cursize;
	if(templen)
	{
		temp=Z_Malloc(templen);
		memcpy(temp, cmd_text.data, templen);
		SZ_Clear(&cmd_text);
	}

// add the entire text of the file
	Cbuf_AddText(text);
	Cbuf_AddText("\n");
	
// add the copied off data
	if(templen)
	{
		SZ_Write(&cmd_text, temp, templen);
		Z_Free(temp);
	}
}

/*
** Cbuf_ExecuteText
*/
void Cbuf_ExecuteText(int exec_when, char *text)
{
	switch(exec_when)
	{
	case EXEC_NOW:
		Cmd_ExecuteString(text);
		break;
	case EXEC_INSERT:
		Cbuf_InsertText(text);
		break;
	case EXEC_APPEND:
		Cbuf_AddText(text);
		break;
	default:
		Sys_Error("Cbuf_ExecuteText: bad exec_when\n");
	}
}

/*
** Cbuf_Execute
**
** executes command buffer line after line
** untill wait command or it is empty
*/
void Cbuf_Execute(void)
{
	char *text, line[1024];
	bool inquote;
	int n;

	alias_count=0;

	while(cmd_text.cursize)
	{
// scans till command separator '\n' or ';'
		text=cmd_text.data;
		inquote=false;
		for(n=0; n<cmd_text.cursize; n++)
		{
			if(text[n]=='"')
				inquote^=1;
			else if(text[n]=='\n')
				break;
			else if(!inquote && text[n]==';')
				break; // don't break if inside a quoted string
		}

		memcpy(line, text, n);
		line[n]=0;

// delete the text from the command buffer and move remaining commands down
// this is necessary because commands (exec, alias) can insert data at the
// beginning of the text buffer

		if(n==cmd_text.cursize)
			cmd_text.cursize=0;
		else
		{
			n++;
			cmd_text.cursize-=n;
			memmove(text, text+n, cmd_text.cursize);
		}

		Cmd_ExecuteString(line); // execute the command line

		if(cmd_wait)
		{
			// skip out while text still remains in buffer, leaving it for next frame
			cmd_wait=false;
			break;
		}
	}
}

/*
** Cbuf_AddSetCmds
**
** adds all the +set commands from the command line
*/
void Cbuf_AddSetCmds(bool clear)
{
	int n;
	char *s;

	for(n=0; n<COM_Argc(); n++)
	{
		s=COM_Argv(n);
		if(strcmp(s, "+set")) continue;

		Cbuf_AddText(va("set %s %s\n", COM_Argv(n+1), COM_Argv(n+2)));
		if(clear)
		{
			COM_ClearArgv(n);
			COM_ClearArgv(n+1);
			COM_ClearArgv(n+2);
		}
		n+=2;
	}
}

/*
** Cbuf_AddCmds
**
** Adds command line parameters as script statements
** Commands lead with a + and continue until another + or -
** quake +vid_ref gl +map amlev1
**
** Returns true if any late commands were added, which
** will keep the demoloop from immediately starting
*/
bool Cbuf_AddCmds(void)
{
	int n, m, s;
	char *text, *build, c;
	int argc;
	bool ret;

// build the combined string to parse from
	s=0;
	argc=COM_Argc();
	for(n=1; n<argc; n++)
		s+=strlen(COM_Argv(n))+1;

	if(!s) return false;

	text=Z_Malloc(s+1);
	text[0]=0;
	for(n=1; n<argc; n++)
	{
		strcat(text, COM_Argv(n));
		if(n!=argc-1)
			strcat(text, " ");
	}
	
// pull out the commands
	build=Z_Malloc(s+1);
	build[0]=0;
	
	for(n=0; n<s-1; n++)
	{
		if(text[n]=='+')
		{
			n++;

			for(m=n; text[m]!='+' && text[m]!='-' && text[m]!=0; m++);

			c=text[m];
			text[m]=0;
			
			strcat(build, text+n);
			strcat(build, "\n");
			text[m]=c;
			n=m-1;
		}
	}

	ret=(build[0]!=0);
	if(ret) Cbuf_AddText(build);

	Z_Free(text);
	Z_Free(build);

	return ret;
}

// ------------------------- * SCRIPT COMMANDS * -------------------------

/*
** Cmd_Wait_f
**
** Causes execution of the remainder of the command buffer to be delayed until
** next frame.  This allows commands like:
** bind g "impulse 5 ; +attack ; wait ; -attack ; impulse 2"
*/
void Cmd_Wait_f(void)
{
	cmd_wait=true;
}

/*
** Cmd_Exec_f
*/
void Cmd_Exec_f(void)
{
	char *f;

	if(Cmd_Argc()!=2)
	{
		Con_Printf("exec <filename> : execute a script file\n");
		return;
	}

	FS_LoadFile(Cmd_Argv(1), &f, true);
	if(!f)
	{
		Con_Printf("couldn't exec %s\n", Cmd_Argv(1));
		return;
	}
	Con_Printf("execing %s\n", Cmd_Argv(1));
	
	Cbuf_InsertText(f);
	FS_FreeFile(f);
}

/*
** Cvar_Vstr_f
**
** executes the contents of a variable
*/
void Cmd_Vstr_f(void)
{
	if(Cmd_Argc()!=2)
	{
		Con_Printf("vstr <variablename> : execute a variable command\n");
		return;
	}

	Cbuf_InsertText(Cvar_VariableString(Cmd_Argv(1)));
}

/*
** Cmd_Echo_f
**
** Just prints the rest of the line as a message
*/
void Cmd_Echo_f(void)
{
	int n;

	for(n=1; n<Cmd_Argc(); n++)
		Con_Printf("%s ", Cmd_Argv(n));
	Con_Printf("\n");
}

/*
** Cmd_Print_f
**
** prints arguments to the console
*/
void Cmd_Print_f(void)
{
	Msg_Printf("%s", Cmd_Args());
}

/*
** Cmd_LinkAlias
**
** links alias in sorted position
*/
void Cmd_LinkAlias(cmdalias_t *a)
{
	cmdalias_t *t, **prev;

	prev=&cmd_alias;
	for(t=cmd_alias; t; t=t->next)
	{
		if(strcmp(t->name, a->name)>=0)
		{
			*prev=a;
			a->next=t;
			return;
		}
		prev=&t->next;
	}

// last in the list
	*prev=a;
	a->next=NULL;
}

/*
** Cmd_Alias_f
**
** Creates a new command that executes a command string (possibly ; seperated)
*/
void Cmd_Alias_f(void)
{
	cmdalias_t *a;
	char cmd[1024], *s;
	int	n, c;

	if(Cmd_Argc()==1)
	{
		Con_Printf("Current alias commands:\n");
		for(a=cmd_alias; a; a=a->next)
			Con_Printf("%s : \"%s\"\n", a->name, a->value);
		return;
	}

	s=Cmd_Argv(1);
	if(strlen(s)>=MAX_ALIAS_NAME)
	{
		Con_Printf("Alias name is too long\n");
		return;
	}

// if the alias already exists, reuse it
	for(a=cmd_alias; a; a=a->next)
		if(!strcmp(s, a->name))
		{
			Z_Free(a->value);
			break;
		}

// else create new alias
	if(!a)
	{
		a=Z_Malloc(sizeof(cmdalias_t));
		strcpy(a->name, s);
		Cmd_LinkAlias(a);
	}

// copy the rest of the command line
	cmd[0]=0;	// start out with a null string
	c=Cmd_Argc();
	for(n=2; n<c; n++)
	{
		strcat(cmd, Cmd_Argv(n));
		if(n!=c-1) strcat(cmd, " ");
	}

	a->value=copystring(cmd);
}

// ------------------------- * COMMAND EXECUTION * -------------------------
typedef struct cmd_function_s
{
	struct cmd_function_s *next;
	char *name;
	xcommand_t function;
} cmd_function_t;

static cmd_function_t *cmd_functions;		// possible commands to execute

static int   cmd_argc=0;
static char *cmd_argv[MAX_ARGS];
static char  cmd_args[1024];

// ------------------------- * Devider * -------------------------

/*
** Cmd_Argc
*/
int Cmd_Argc(void)
{
	return cmd_argc;
}

/*
** Cmd_Argv
*/
char *Cmd_Argv(int arg)
{
	if(arg>=cmd_argc || arg<0) return "";
	return cmd_argv[arg];
}

/*
** Cmd_Args
**
** Returns a single string containing argv(1) to argv(argc()-1)
*/
char *Cmd_Args(void)
{
	return cmd_args;
}

/*
** Cmd_MacroExpandString
*/
char *Cmd_MacroExpandString(char *text)
{
	static char expanded[1024];
	char temporary[1024];
	char *scan, *start, *exvalue;
	int len, exlen, n, count;
	bool inquote;

	inquote=false;
	scan=text;

	len=strlen(scan);
	if(len>=1024)
	{
		Con_Printf("Line exceeded 1024 chars, discarded.\n");
		return NULL;
	}

	count=0;

	for(n=0; n<len; n++)
	{
		if(scan[n]=='"')
		{
			inquote^=1;
			continue;
		}
		if(inquote || scan[n]!='$') continue; // don't expand inside quotes

	// scan out the complete macro
		start=scan+n+1;
		start=COM_Parse(start);
		if(!start) continue;
	
		exvalue=Cvar_VariableString(com_token);

		exlen=strlen(exvalue);
		len+=exlen;
		if(len>=1024)
		{
			Con_Printf("Expanded line exceeded 1024 chars, discarded.\n");
			return NULL;
		}

		strncpy(temporary, scan, n);
		strcpy(temporary+n, exvalue);
		strcpy(temporary+n+exlen, start);

		strcpy(expanded, temporary);
		scan=expanded;
		n--;

		if(++count==100)
		{
			Con_Printf("Macro expansion loop, discarded.\n");
			return NULL;
		}
	}

	if(inquote)
	{
		Con_Printf("Line has unmatched quote, discarded.\n");
		return NULL;
	}

	return scan;
}

/*
** Cmd_TokenizeString
**
** Parses the given string into command line tokens.
** $Cvars will be expanded unless they are in a quoted token
*/
void Cmd_TokenizeString(char *text, bool macroExpand)
{
	int n;

// clear the args from the last string
	for(n=0; n<cmd_argc; n++)
		Z_Free(cmd_argv[n]);

	cmd_argc=0;
	cmd_args[0]=0;

	// macro expand the text
	if(macroExpand) text=Cmd_MacroExpandString(text);
	if(!text) return;

	while(1)
	{
		while(*text && *text<=' ' && *text!='\n')
			text++; // skip whitespace up to a '/n'
		
		if(*text=='\n') // a newline separates commands in the buffer
		{
			text++;
			break;
		}

		if(!*text) return;

	// set cmd_args to everything after the first arg
		if(cmd_argc==1)
		{
			int l;

			strcpy(cmd_args, text);

		// strip off any trailing whitespace
			l=strlen(cmd_args)-1;
			for(; l>=0; l--)
				if(cmd_args[l]<=' ')
					cmd_args[l]=0;
				else
					break;
		}

		text=COM_Parse(text);
		if(!text) return;

		if(cmd_argc<MAX_ARGS)
		{
			cmd_argv[cmd_argc]=Z_Malloc(strlen(com_token)+1);
			strcpy(cmd_argv[cmd_argc], com_token);
			cmd_argc++;
		}
	}
}

/*
** Cmd_Link
**
** links command in sorted position
*/
void Cmd_Link(cmd_function_t *cmd)
{
	cmd_function_t *t, **prev;

	prev=&cmd_functions;
	for(t=cmd_functions; t; t=t->next)
	{
		if(strcmp(t->name, cmd->name)>=0)
		{
			*prev=cmd;
			cmd->next=t;
			return;
		}
		prev=&t->next;
	}

// last in the list
	*prev=cmd;
	cmd->next=NULL;
}

/*
** Cmd_AddCommand
*/
void Cmd_AddCommand(char *cmd_name, xcommand_t function)
{
	cmd_function_t	*cmd;
		
	// fail if the command is a variable name
	if(Cvar_VariableString(cmd_name)[0])
	{
		Con_Printf("Cmd_AddCommand: %s already defined as a var\n", cmd_name);
		return;
	}
	
	// fail if the command already exists
	for(cmd=cmd_functions; cmd; cmd=cmd->next)
		if(!strcmp(cmd_name, cmd->name))
		{
			Con_Printf("Cmd_AddCommand: %s already defined\n", cmd_name);
			return;
		}

	cmd=Z_Malloc(sizeof(cmd_function_t));
	cmd->name=cmd_name;
	cmd->function=function;

	Cmd_Link(cmd);
}

/*
** Cmd_RemoveCommand
*/
void Cmd_RemoveCommand(char *cmd_name)
{
	cmd_function_t *cmd, **back;

	back=&cmd_functions;
	while(1)
	{
		cmd=*back;
		if(!cmd)
		{
			Con_Printf("Cmd_RemoveCommand: %s not added\n", cmd_name);
			return;
		}
		if(!strcmp(cmd_name, cmd->name))
		{
			*back=cmd->next;
			free(cmd);
			return;
		}
		back=&cmd->next;
	}
}

/*
** Cmd_Exists
*/
bool Cmd_Exists(char *cmd_name)
{
	cmd_function_t *cmd;

	for(cmd=cmd_functions; cmd; cmd=cmd->next)
	{
		if(!strcmp(cmd_name, cmd->name)) return true;
	}
	return false;
}

/*
** Cmd_CompleteCommand
*/
char *Cmd_CompleteCommand(char *partial)
{
	static char part[128];
	int len, exact, t, n;
	char *match;
	cmd_function_t *cmd;
	cmdalias_t *a;
	cvar_t *cvar;

	len=strlen(partial);
	match=NULL;
	exact=-1;
	n=0;
	for(cmd=cmd_functions; cmd; cmd=cmd->next)
	{
		if(strncmp(partial, cmd->name, len)) continue;
		t=strlen(cmd->name);
		if(exact<0 || exact>t) exact=t;
		if(match)
			while(exact>0 && match[exact-1]!=cmd->name[exact-1]) exact--;
		if(n==1)
		{
			Con_Printf("]%s\n", partial);
			Con_Printf("  %s\n", match);
		}
		if(n++)
			Con_Printf("  %s\n", cmd->name);
		if(!match || exact<t) match=cmd->name;
	}
	for(a=cmd_alias; a; a=a->next)
	{
		if(strncmp(partial, a->name, len)) continue;
		t=strlen(a->name);
		if(!exact || exact>t) exact=t;
		if(match)
			while(exact>0 && match[exact-1]!=a->name[exact-1]) exact--;
		if(n==1)
		{
			Con_Printf("]%s\n", partial);
			Con_Printf("  %s\n", match);
		}
		if(n++)
			Con_Printf("  %s\n", a->name);
		if(!match || exact<t) match=a->name;
	}
	for(cvar=cvar_vars; cvar; cvar=cvar->next)
	{
		if(strncmp(partial, cvar->name, len)) continue;
		t=strlen(cvar->name);
		if(exact<0 || exact>t) exact=t;
		if(match)
			while(exact>0 && match[exact-1]!=cvar->name[exact-1]) exact--;
		if(n==1)
		{
			Con_Printf("]%s\n", partial);
			Con_Printf("  %s\n", match);
		}
		if(n++)
			Con_Printf("  %s\n", cvar->name);
		if(!match || exact<t) match=cvar->name;
	}

	if(!match || !exact) return NULL; // nothing matched
	if(exact>=sizeof(part)-1)
		exact=sizeof(part)-1; // matched commands too long /unlikely though/
	strncpy(part, match, exact);
	part[exact]=0;
	if(!match[exact]) strcat(part, " "); // exact match

	return part;
}

/*
** Cmd_ExecuteString
**
** A complete command line has been parsed, so try to execute it
*/
void Cmd_ExecuteString(char *text)
{
	cmd_function_t *cmd;
	cmdalias_t *a;

	Cmd_TokenizeString(text, true);

// execute the command line
	if(!Cmd_Argc()) return; // no tokens

// check functions
	for(cmd=cmd_functions; cmd; cmd=cmd->next)
		if(!Q_strcasecmp(cmd_argv[0], cmd->name))
		{
			cmd->function();
			return;
		}

// check alias
	for(a=cmd_alias; a; a=a->next)
		if(!Q_strcasecmp(cmd_argv[0], a->name))
		{
			if(++alias_count==ALIAS_LOOP_COUNT)
			{
				Con_Printf("Warning: possible alias loop detected\n");
				return;
			}
			Cbuf_InsertText(a->value);
			return;
		}
	
// check cvars
	if(Cvar_Command()) return;

	Con_Printf("Unknown command \"%s\"\n", Cmd_Argv(0));
}

// ------------------------- * Devider * -------------------------

/*
** Cmd_CmdList_f
**
** displays a list of console commands
*/
void Cmd_List_f(void)
{
	cmd_function_t *cmd;
	char *mask;
	int n, m;

	if(Cmd_Argc()==2)
		mask=Cmd_Argv(1);
	else
		mask=NULL;

	for(n=0, m=0, cmd=cmd_functions; cmd; cmd=cmd->next, n++)
	{
		if(!TestWildCard(cmd->name, mask)) continue;
		m++;

		Con_Printf(" %s\n", cmd->name);
	}

	if(mask)
		Con_Printf(" % 4d commands matching \"%s\"\n", m, mask);
	Con_Printf(" % 4d commands total\n", n);
}

/*
** Cmd_Init
**
** registers our commands
*/
int Cmd_Init(void)
{
	Cbuf_Init();

	Cmd_AddCommand("exec", Cmd_Exec_f);
	Cmd_AddCommand("vstr", Cmd_Vstr_f);
	Cmd_AddCommand("echo", Cmd_Echo_f);
	Cmd_AddCommand("print", Cmd_Print_f);
	Cmd_AddCommand("alias", Cmd_Alias_f);
	Cmd_AddCommand("wait", Cmd_Wait_f);
	Cmd_AddCommand("cmdlist", Cmd_List_f);
	return 1;
}
