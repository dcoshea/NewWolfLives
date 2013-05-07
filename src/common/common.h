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
** common functions and definitions
*************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <setjmp.h>

#include "keys.h"
// ------------------------- * constants * -------------------------

#define NEWWOLF_NAME	"NewWolfLives"
#define VERSION			0.75
#define BASEDIRNAME "basewgl"

// width and height in 2D
#define WIDTH_2D		640
#define HEIGHT_2D		480

#ifdef WIN32
	#ifdef NDEBUG
		#define BUILDSTRING "Win32 RELEASE"
	#else
		#define BUILDSTRING "Win32 DEBUG"
	#endif
#elif defined __linux__
	#define BUILDSTRING "Linux"
#else
	#define BUILDSTRING "Unknown"
#endif

#define MAX_OSPATH 128
#define MAX_WLPATH 64

// ------------------------- * basic types * -------------------------
typedef	enum {false, true} bool;
typedef	unsigned char byte;
typedef	unsigned short word;
typedef	unsigned long dword;

// ------------------------- * additional types * -------------------------
typedef dword rgb;
typedef dword rgba;

#define rgb(r, g, b) ((r)+((g)<<8)+((b)<<16))
#define _rgb(r, g, b) ((r)+((g)<<8)+((b)<<16)+((255)<<24))
#define rgba(r, g, b, a) ((r)+((g)<<8)+((b)<<16)+((a)<<24))

// ------------------------- * Cvars * -------------------------
typedef struct cvar_s
{
	char *name;
	char *string;
	char *def;
	float value;
	int flags;
	bool modified; // set each time the cvar is changed
	struct cvar_s *next;
} cvar_t;

#define CVAR_ARCHIVE		1		// set to cause it to be saved
#define CVAR_USERINFO		2		// added to userinfo  when changed
#define CVAR_SERVERINFO	4		// added to serverinfo when changed
#define CVAR_NOSET			8		// don't allow change from console at all, but can be set from the command line
#define CVAR_LATCH			16	// save changes until server restart
#define CVAR_RESTART		32	// takes effect upon restarting
#define CVAR_NODEFAULT	64	// cvar has no default value (usually for NOSET cvars)

extern cvar_t	*cvar_vars;

void Cvar_Init(void);

float Cvar_VariableValue(char *var_name);
int Cvar_VariableInteger(char *var_name);
char *Cvar_VariableString(char *var_name);

cvar_t *Cvar_Get(char *var_name, char *var_value, int flags);
cvar_t *Cvar_Set(char *var_name, char *value);
cvar_t *Cvar_ForceSet(char *var_name, char *value);
void Cvar_SetValue(char *var_name, float value);
void Cvar_ForceSetValue(char *var_name, float value);
void Cvar_SetInteger(char *var_name, int value);
void Cvar_ForceSetInteger(char *var_name, int value);
cvar_t *Cvar_FullSet(char *var_name, char *value, int flags);

bool Cvar_Command(void);
void Cvar_WriteVariables(FILE *f);

// ------------------------- * Commands * -------------------------
typedef void (*xcommand_t) (void);

#define	EXEC_NOW		0 // don't return until completed
#define	EXEC_INSERT	1 // insert at current position, but don't run yet
#define	EXEC_APPEND	2 // add to end of the command buffer

int Cmd_Init(void);

void Cmd_AddCommand(char *cmd_name, xcommand_t function);
void Cmd_RemoveCommand(char *cmd_name);

bool Cmd_Exists(char *cmd_name);
char *Cmd_CompleteCommand(char *partial);

int	Cmd_Argc(void);
char *Cmd_Argv(int arg);
char *Cmd_Args(void);

void Cmd_TokenizeString(char *text, bool macroExpand);

void Cbuf_ExecuteText(int exec_when, char *text);
void Cbuf_Execute(void);
void Cbuf_InsertText(char *text);
void Cbuf_AddSetCmds(bool clear);
bool Cbuf_AddCmds(void);
void Cbuf_AddText(char *text);
void Cmd_ExecuteString(char *cmd);

// ------------------------- * FileSystem * -------------------------
void FS_InitFilesystem(void);
void FS_SetGamedir(char *dir);
char *FS_Gamedir(void);
char *FS_ExpandFilename(char *name);

void FS_CreatePath(char *path);

int FS_FOpenFile(char *filename,  FILE **file);
void FS_FCloseFile(FILE *f);

void FS_Read(void *buffer, int len, FILE *f);
int FS_LoadFile(char *path, void **buffer, bool text);
void FS_FreeFile(void *buffer);

// ------------------------- * File I/O * -------------------------
typedef struct image_s
{
	int w, h;		// dimensions of an image
	int bpp;		// cound be 1 - grayscale, 3 - RGB or 4 - RGBA
	byte *data;	// raw image data [w*h*bpp]
// host application may add additional fields here
//	char name[MAX_WLPATH];
	int name_foo;
	int glTex;
//	struct image_s *next;
} image_t;

bool Img_Read(char *path, image_t *img);
bool Img_ReadTGA(char *name, image_t *img);
bool Img_ReadPCX(char *name, image_t *img);
bool Img_ReadJPG(char *name, image_t *img);

void Img_ExpandPalette(byte *dst, byte *src, int w, int h, byte *pal, bool transparent);
bool Img_HasTransprency(byte *data, int size);
void Img_Resize(byte *dst, int outwidth, int outheight, byte *data, int width, int height, int bpp);
void Img_Resample(byte *dst, int outwidth, int outheight, byte *data, int width, int height, int bpp);
bool Img_MipMap(byte *src, int *width, int *height, int bpp);
void Img_Free(image_t *img);

// ------------------------- * command line params * -------------------------
int	COM_Argc(void);
char *COM_Argv(int arg);
void COM_ClearArgv(int arg);
int COM_CheckParm(char *parm);
void COM_AddParm(char *parm);
void COM_InitArgv(int argc, char **argv);

// ------------------------- * size buffer * -------------------------
typedef struct sizebuf_s
{
	bool allowoverflow;	// if false, Sys_Error occurs
	bool overflowed;		// set to true if the buffer size failed
	byte *data;
	int maxsize;
	int cursize;
} sizebuf_t;

void SZ_Init(sizebuf_t *buf, byte *data, int length);
void SZ_Clear(sizebuf_t *buf);
void *SZ_GetSpace(sizebuf_t *buf, int length);
void SZ_Write(sizebuf_t *buf, void *data, int length);
void SZ_Print(sizebuf_t *buf, char *data);

// ------------------------- * ZONE MEMORY * -------------------------
void Z_Free(void *ptr);
void Z_FreeTags(int tag);
void *Z_TagMalloc(int size, int tag);
void *Z_Malloc(int size);

// ------------------------- * misc functions * -------------------------
char *copystring(const char *s);
int Q_strncasecmp(char *s1, char *s2, int n);
int Q_strcasecmp(char *s1, char *s2);
bool TestWildCard(const char *str, const char *mask);
char *va(char *format, ...);

// ------------------------- * Common Functions * -------------------------
extern cvar_t *developer;
extern int tics;
extern float ftime;						// [s]
extern float dftime;					// [s]
extern unsigned int realtime;	// [ms]
extern unsigned int gametime;	// [tics]
extern unsigned int frameon;

void COM_Init(int argc, char **argv);
void COM_Shutdown(void);
void COM_Frame(int msec);

// ------------------------- * Console * -------------------------
void Con_Init(void);
void Con_Draw(void);
void Con_Key(int key);
void Con_ClearNotify(void);
void Con_Printf(char *fmt, ...);

void Msg_Printf(char *fmt, ...);
void Con_ToggleConsole_f(void);

// ------------------------- * Debugger Output * -----------------
// Sends a message to the debugger (Windows-specific).
void Debug_Printf(char *fmt, ...);

// ------------------------- * Client/Server interaction * -------------------------
#define BT_ATTACK			1		// Press "Fire"
#define BT_USE				2		// Use button, to open doors, activate switches.
#define	BUTTON_ANY		128	// any key whatsoever

typedef struct usercmd_s
{
	int angle;										// in fines
	long /*FIXME: short*/ forwardmove, sidemove;	// *2048 for move
	byte tics;
	byte buttons;
	byte impulse;
} usercmd_t;

// ------------------------- * parse lib * -------------------------
#define MAX_TOKEN_LEN 512

typedef enum ttype_e
{
	TOKEN_NULL,		// NULL
	TOKEN_SYMBOL,	// , : ; {} [] etc.
	TOKEN_NUM,		// number
	TOKEN_QUOTE,	// quoted string "aaa sd e"
	TOKEN_ID			// unquoted string (identifier)
} ttype_t;

typedef struct parse_s
{
	jmp_buf error;
	char *src;								// stream poiter
	char *lineStart;					// current line start pointer
	char *tokenStart;					// curretn token start pointer
	unsigned int lineNumber;	// line number
	ttype_t tokenType;				// last token type
	bool tokenReturned;				// token already parsed (after UnGet)
	char token[MAX_TOKEN_LEN];// token itself
} parse_t;

#define P_BeginParseEx(p, data) (P_BeginParse((p), (data)), setjmp((p)->error))

void P_BeginParse(parse_t *p, char *data);
char *P_StreamPosition(parse_t *p);
char *P_LastError(void);
char *P_TokenType(ttype_t n);
void P_UnGetToken(parse_t *p);
ttype_t P_GetToken(parse_t *p);
void P_Expect(parse_t *p, ttype_t type, char *val);
void P_ExpectString(parse_t *p, char *dest, unsigned maxlen);
char *P_ExpectStringEx(parse_t *p);
void P_ExpectId(parse_t *p, char *dest, unsigned maxlen);
char *P_ExpectIdEx(parse_t *p);
void P_ExpectSymbol(parse_t *p, int c);
void P_ExpectInt(parse_t *p, int *dest);
void P_ExpectFloat(parse_t *p, float *dest);
bool P_CheckSymbol(parse_t *p, int c);

// ------------------------- * non-portable system functions * -------------------------
extern int curtime; // time returned by last Sys_Milliseconds
extern bool sys_mmx, sys_xmm; // cpu features

void Sys_Error(char *error, ...);
void Sys_Quit(void);
void Sys_Init(void);
void Sys_Info(void);

void Sys_BeginSplash(void);
void Sys_EndSplash(void);

int Sys_Milliseconds(void);
void Sys_Mkdir(char *path);

// input
void In_Init(void);
void In_Shutdown(void);
void In_Activate(bool active);
void In_Frame(void);
void In_Move(usercmd_t *cmd);
// video
void Vid_Init(void);
void Vid_Shutdown(void);
void Vid_Activate(bool active);
void Vid_Frame(void);
int Vid_Modes(void);
const char *Vid_DescribeMode(int mode);

typedef struct vidmode_s
{
	const char *description;
	int width, height;
} vidmode_t;
extern vidmode_t vid_modes[];

// rendering library interface
typedef struct r_interface_s
{
	int w, h, bpp;						// screen resolution
	bool fs;									// fullscreen?
	void (*R_ShutDown)(void);					// called to free ri
	void (*R_Activate)(bool active);	// called when app is about to (de)activate
	void (*R_Frame)(void);						// called just before each frame
	void (*R_Swap)(void);							// swaps visible and offscreen buffers

	void (*R_Clear)(void);
	void (*R_DumpScreen)(image_t *dump);
	void (*R_DrawBox)(int x, int y, int w, int h, rgba c);
	void (*R_DrawPic)(int x, int y, image_t *pic);
	void (*R_DrawPicScale)(int x, int y, int w, int h, image_t *pic);

	void (*R_UploadTexture)(image_t *tex, bool mipmap, bool aniso, bool clamp);
	void (*R_UnloadTexture)(image_t *tex);
	void (*R_SelectTexture)(image_t *pic);

} r_interface_t;

extern r_interface_t ri;

