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
** misc functions
*************************************************************************/
//#include "common.h"
#include "../WolfDef.h"
#include "../scripts/scripts.h"

// ------------------------- * globals * -------------------------
cvar_t *developer;

// ------------------------- * command line params * -------------------------
#define MAX_NUM_ARGVS	64

int com_argc;
char *com_argv[MAX_NUM_ARGVS];
char *com_cmdline;

/*
** COM_CheckParm
**
** Returns the position (1 to argc-1) in the program's argument list
** where the given parameter apears, or 0 if not present
*/
int COM_CheckParm(char *parm)
{
	int n;
	
	for(n=1; n<com_argc; n++)
		if(!strcmp(parm, com_argv[n]))
			return n;
		
	return 0;
}

/*
** COM_Argc
*/
int COM_Argc(void)
{
	return com_argc;
}

/*
** COM_Argv
*/
char *COM_Argv(int arg)
{
	if(arg<0 || arg>=com_argc || !com_argv[arg])
		return "";
	return com_argv[arg];
}

/*
** COM_Argv
*/
void COM_ClearArgv(int arg)
{
	if(arg<0 || arg>=com_argc || !com_argv[arg])
		return;
	com_argv[arg]="";
}

/*
** COM_InitArgv
*/
void COM_InitArgv(int argc, char **argv)
{
	int n;
	size_t zlen;

	if(argc>MAX_NUM_ARGVS)
		Sys_Error("Too many command line arguments");

	com_argc=argc;
	zlen=0;
	for(n=0; n<argc; n++)
	{
		if(!argv[n])// FIXME || strlen(argv[n])>=MAX_TOKEN_CHARS)
			com_argv[n]="";
		else
			com_argv[n]=argv[n];
		zlen+=strlen(com_argv[n])+1;
	}

// reconstruct command line
	com_cmdline=Z_Malloc(zlen);
	for(n=0; n<argc; n++)
	{
		strcat(com_cmdline, com_argv[n]);
		if(n!=argc-1) strcat(com_cmdline, " ");
	}	
}

/*
** COM_AddParm
**
** Adds the given string at the end of the current argument list
*/
void COM_AddParm(char *parm)
{
	if(com_argc==MAX_NUM_ARGVS)
		Sys_Error("COM_AddParm: Too many arguments");
	com_argv[com_argc++]=parm;
}

// ------------------------- * Size Buffer * -------------------------

/*
** SZ_Init
*/
void SZ_Init(sizebuf_t *buf, byte *data, int length)
{
	memset(buf, 0, sizeof(sizebuf_t));
	buf->data=data;
	buf->maxsize=length;
}

/*
** SZ_Clear
*/
void SZ_Clear(sizebuf_t *buf)
{
	buf->cursize=0;
	buf->overflowed=false;
}

/*
** SZ_GetSpace
*/
void *SZ_GetSpace(sizebuf_t *buf, int length)
{
	void *data;
	
	if(buf->cursize+length>buf->maxsize)
	{
		if(!buf->allowoverflow)
			Sys_Error("SZ_GetSpace: overflow without allowoverflow set");
		
		if(length>buf->maxsize)
			Sys_Error("SZ_GetSpace: %d is > full buffer size", length);

		Con_Printf("SZ_GetSpace: overflow\n");
		SZ_Clear(buf);
		buf->overflowed=true;
	}

	data=buf->data+buf->cursize;
	buf->cursize+=length;
	
	return data;
}

/*
** SZ_Write
*/
void SZ_Write(sizebuf_t *buf, void *data, int length)
{
	memcpy(SZ_GetSpace(buf, length), data, length);
}

/*
** SZ_Print
**
** strcats onto the sizebuf
*/
void SZ_Print(sizebuf_t *buf, char *data)
{
	size_t len;
	
	len=strlen(data)+1;

	if(buf->cursize)
	{
		if(!buf->data[buf->cursize-1]) // write over trailing 0
		{
			byte *p;

			p=SZ_GetSpace(buf, (int)(len-1));
			if(!buf->overflowed)
			{
				memcpy(p-1, data, len);
				return;
			}
			else
				SZ_Clear(buf);
		}
	}
	memcpy(SZ_GetSpace(buf, len), data, len);
}

// ------------------------- * ZONE MEMORY * -------------------------
#define Z_MAGIC		0xD0D0

typedef struct zhead_s
{
	struct zhead_s *prev, *next;
	word magic;
	word tag;		// for group free
	int size;
} zhead_t;

zhead_t z_chain;
int z_count, z_bytes, z_peak;

/*
** Z_Free
*/
void Z_Free(void *ptr)
{
	zhead_t	*z;

	if(!ptr) return;
	z=((zhead_t *)ptr)-1;

	if(z->magic!=Z_MAGIC)
		Sys_Error("Z_Free: bad magic");

	z->prev->next=z->next;
	z->next->prev=z->prev;

	z_count--;
	z_bytes-=z->size;
	free(z);
}

/*
** Z_Stats_f
*/
void Z_Stats_f(void)
{
	Con_Printf("Zone memory: %d bytes in %d blocks; peak: %d bytes\n", z_bytes, z_count, z_peak);
}

/*
** Z_FreeTags
*/
void Z_FreeTags(int tag)
{
	zhead_t	*z, *next;

	for(z=z_chain.next; z!=&z_chain; z=next)
	{
		next=z->next;
		if(z->tag==tag) Z_Free((void *)(z+1));
	}
}

/*
** Z_TagMalloc
**
** filles memory with zeroes
*/
void *Z_TagMalloc(int size, int tag)
{
	zhead_t	*z;
	
	size=size+sizeof(zhead_t);
	z=malloc(size);
	if(!z)
		Sys_Error("Z_Malloc: failed on allocation of %d bytes (current zone is %d)", size, z_bytes);
	memset(z, 0, size);
	z_count++;
	z_bytes+=size;
	if(z_bytes>z_peak) z_peak=z_bytes;
	z->magic=Z_MAGIC;
	z->tag=tag;
	z->size=size;

	z->next=z_chain.next;
	z->prev=&z_chain;
	z_chain.next->prev=z;
	z_chain.next=z;

	return (void *)(z+1);
}

/*
** Z_Malloc
**
** filles memory with zeroes
*/
void *Z_Malloc(int size)
{
	return Z_TagMalloc(size, 0);
}

// ------------------------- * misc functions * -------------------------

/*
** copystring
*/
char *copystring(const char *s)
{
	char *b;

	b=Z_Malloc(strlen(s)+1);
	strcpy(b, s);
	return b;
}

/*
** Q_strncasecmp
*/
int Q_strncasecmp(char *s1, char *s2, int n)
{
	int c1, c2;
	
	do
	{
		c1=*s1++; c2=*s2++;

		if(!n--) return 0; // strings are equal until end point
		if(c1!=c2)
		{
			if(c1>='a' && c1<='z')
				c1-=('a'-'A');
			if(c2>='a' && c2<='z')
				c2-=('a'-'A');
			if(c1!=c2)
				return -1; // strings not equal
		}
	} while(c1);
	
	return 0; // strings are equal
}

/*
** Q_strcasecmp
*/
int Q_strcasecmp(char *s1, char *s2)
{
	return Q_strncasecmp(s1, s2, 0xFFFFFF);
}

/*
** testWildCard
**
** tests wildcards with a string
** supports:
** * - any number (including zero) of any characters
** ? - any character
** if str  is NULL returns false
** if mask is NULL returns true
*/
bool TestWildCard(const char *str, const char *mask)
{
	if(!str) return false;
	if(!mask) return true;

	for(; *str; str++, mask++)
		switch(*mask)
		{
		case 0:
			return false;
		case '*':
			while(*mask=='*') *mask++;
			if(!*mask) return true;
			for(; *str; str++)
				if((*mask=='?' || *mask==*str) && TestWildCard(str, mask))
					return true;
			return false;
		case '?':
			break;
		default:
			if(*mask!=*str) return false;
		}

	while(*mask=='*') mask++;
	if(!*mask)
		return true;
	else
		return false;
}

/*
** va
**
** does a varargs printf into a temp buffer, so I don't need to have
** varargs versions of all text functions.
** FIXME: make this buffer size safe someday
*/
char *va(char *format, ...)
{
	static char string[1024];
	va_list argptr;

	va_start(argptr, format);
	vsprintf(string, format, argptr);
	va_end(argptr);

	return string;  
}

// ------------------------- * Common Functions * -------------------------
bool com_initialized=false;

/*
** Com_Error_f
**
** Just throw a fatal error to
** test error shutdown procedures
*/
void Com_Error_f(void)
{
	Sys_Error("%s", Cmd_Argv(1));
}

/*
** COM_Init
**
** initialize common subsystems
*/
void COM_Init(int argc, char **argv)
{
	z_chain.next=z_chain.prev=&z_chain;
	z_count=z_bytes=z_peak=0;

	Sys_BeginSplash(); // show splash screen

// prepare enough of the subsystems to handle
// cvar and command buffer management
	COM_InitArgv(argc, argv);

	Cmd_Init();			// allows command adding
	Cvar_Init();		// allows cvar adding
	Con_Init();			// console printing to the console [CLIENT]

	Sys_Info();			// sets system dependend flags /CPU and OS types/
	Sys_Init();			// initializes system dependent stuff
	Math_Init();		// math /depends on system to allow MMX optimizations/

	Key_Init();			// allows bindings to be set

// read settings from command line and configuration files
	Cbuf_AddSetCmds(false); // set dirs
	Cbuf_Execute();

	FS_InitFilesystem();		// allows config file to be read

	Cbuf_AddText("exec default.cfg\n");
	Cbuf_AddText("exec config.cfg\n");
	Cbuf_AddSetCmds(true);	// override default and config params
	Cbuf_Execute();

// init common commands and vars
	Cmd_AddCommand("z_stats", Z_Stats_f);
	Cmd_AddCommand("error", Com_Error_f);
	Cmd_AddCommand("quit", Sys_Quit);

	developer=Cvar_Get("developer", "0", 0);
	Cvar_FullSet("version", va("%4.2f %s [%s]", VERSION, BUILDSTRING, __DATE__), CVAR_SERVERINFO|CVAR_NOSET|CVAR_NODEFAULT);
	Cvar_FullSet("cmdline", com_cmdline, CVAR_NOSET|CVAR_NODEFAULT);

// system initialization
	Vid_Init();
	In_Init();

//////////// mess down here; clean sometime ///////////////////
//	Win_Init();

	Ver_Init();	// version control
	File_Init();// file system /before video & sound/
	R_Init();		// init renderer
	SD_Init();	// init sound

	Lvl_Init();
	AM_Init();	// automap
	Pow_Init();	// powerups
	Spr_Init();	// sprites
	PE_Init();	// particles

	InitSaveLoad();
	
	CL_Init();

	GM_Init();	// game and player init
	M_Init();		// menu
	FNT_Init();	// fonts
	
	Scr_ModelsParse(va("scripts/models.%s.script", WolfVer.ext)); ////////////////

	Sys_EndSplash();	// end splash screen
//	Win_Show(true);		// show main window /switch video mode is necessary/

	Con_Printf("\n======= NewWolf Initialized ========\n\n");

// --- * initializaton done * ---

// add + commands from command line
	if(!Cbuf_AddCmds())
	{
		Cbuf_AddText("intro\n");	// start with an intro
		Cbuf_AddText("exec autoexec.cfg\n");
		Cbuf_Execute();
	}
	com_initialized=true;
}

/*
** COM_Shutdown
*/
void COM_Shutdown(void)
{
	FILE *fp;

	Con_Printf("Terminating NewWolf...\n");

	fp=fopen(FS_ExpandFilename("config.cfg"), "wt");
	if(fp)
	{
		fprintf(fp, "// NewWolf Version %4.2f config file\n", VERSION);
		Key_WriteBindings(fp);
		Cvar_WriteVariables(fp);
		fclose(fp);
	}
	else
		Con_Printf("-> error saving config file\n");


	SD_ShutDown();
	File_ShutDown();
	FNT_ShutDown();
	Vid_UnCacheAllPics();
	Vid_ShutDownTexMan();
	GL_ShutDown();

	In_Shutdown();
	Vid_Shutdown();

	com_initialized=false;
}

// ------------------------- * frame * -------------------------
int tics;
float ftime;							// [s]
float dftime;							// [s]
unsigned int gametime=0;	// [tics]
unsigned int realtime=0;	// [ms]
unsigned int frameon=0;		// [frame]
#define	MAXTICS	10

/*
** COM_Frame
*/
void COM_Frame(int msec)
{
	static int frac_time=0; // [ms] tics are counted using this
	static usercmd_t cmd;

	if(!com_initialized) return;

// calculate Wolfenstein tics [1 tick=1/70 sec] or [14 ms~1 tick]
	frac_time+=msec;
	tics=frac_time/14;
	frac_time%=14;
	if(tics>MAXTICS) tics=MAXTICS;

// time
	gametime+=tics;
	realtime+=msec;
	ftime=realtime*(1/1000.0f);
	dftime=msec*(1/1000.0f);
	frameon++;

//	Msg_Printf("%d[frame] %d[ms] %5.2f[s]", frameon, realtime, ftime);

// process system
	Cbuf_Execute();

	Vid_Frame();
	In_Frame();

// process game
	if(tics && key_dest==key_game)
	{
		CL_SendCmd(&cmd);
		tics=cmd.tics;

		PL_Process(&Player, &cmd);	// Player processing
		ProcessGuards();						// if single
		PW_Process();
		Door_ProcessDoors_e(tics, msec);
		PE_ProcessParticles();
		gamestate.TimeCount+=msec;
		SD_UpdateSounds3D();
	}

	R_UpdateScreen();
}

