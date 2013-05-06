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
** non-portable system services /Win32 version/
*************************************************************************/
#include "../common/common.h"
#include "win32.h"
#include <direct.h>

// ------------------------- * globals * -------------------------
HINSTANCE hInst;
bool ActiveApp, Minimized;

// ------------------------- * exit * -------------------------

/*
** Sys_Error
**
** drops out with an error message
*/
void Sys_Error(char *error, ...)
{
	va_list argptr;
	char text[1024];

	va_start(argptr, error);
	vsprintf(text, error, argptr);
	va_end(argptr);

	Con_Printf("Sys_Error: %s\n", text);
	COM_Shutdown();
	MessageBox(NULL, text, NEWWOLF_NAME" Error", MB_ICONEXCLAMATION|MB_OK);

// shut down any system specific stuff
	timeEndPeriod(1);

	exit(1);
}

/*
** Sys_Quit
**
** clear exit after deinit
*/
void Sys_Quit(void)
{
	COM_Shutdown();

// shut down any system specific stuff
	timeEndPeriod(1);

	exit(0);
}

// ------------------------- * starting * -------------------------

/*
** Sys_Init
**
** initialize system specific stuff
*/
void Sys_Init(void)
{
	timeBeginPeriod(1);
}

// ------------------------- * splash * -------------------------
HWND hWndSplash;

/*
** Sys_BeginSplash
**
** displays splash screen
*/
void Sys_BeginSplash(void)
{
	RECT rect;

	hWndSplash=CreateDialog(hInst, MAKEINTRESOURCE(IDD_SPLASH), NULL, NULL);
	if(!hWndSplash) return;

	if(GetWindowRect(hWndSplash, &rect))
	{
		if(rect.left>rect.top*2)
			SetWindowPos(hWndSplash, NULL, (rect.left/2)-((rect.right-rect.left)/2), rect.top, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
	}

	ShowWindow(hWndSplash, SW_SHOWDEFAULT);
	UpdateWindow(hWndSplash);
	SetForegroundWindow(hWndSplash);
}

/*
** Sys_EndSplash
*/
void Sys_EndSplash(void)
{
	DestroyWindow(hWndSplash);
}

// ------------------------- * system functions * -------------------------
int	curtime;

/*
** Sys_Milliseconds
*/
int Sys_Milliseconds(void)
{
	static int base;
	static bool initialized=false;

	if(!initialized)
	{
	// let base retain 16 bits of effectively random data
		base=timeGetTime()&0xffff0000;
		initialized=true;
	}
	curtime=timeGetTime()-base;

	return curtime;
}

/*
** Sys_Mkdir
*/
void Sys_Mkdir(char *path)
{
	_mkdir(path);
}

// ------------------------- * Alt-Tab enable/disable * -------------------------
bool alttab_disabled=false;

/*
** Win_DisableAltTab
*/
void Win_DisableAltTab(void)
{
	if(alttab_disabled) return;

	if(sys_win9x)
	{
		BOOL old;
		SystemParametersInfo(SPI_SCREENSAVERRUNNING, 1, &old, 0);
	}
	else // WinNT
	{
		RegisterHotKey(0, 0, MOD_ALT, VK_TAB);
		RegisterHotKey(0, 1, MOD_ALT, VK_RETURN);
		RegisterHotKey(0, 2, MOD_ALT, VK_ESCAPE);
	}
	alttab_disabled=true;
}

/*
** Win_EnableAltTab
*/
void Win_EnableAltTab(void)
{
	if(!alttab_disabled) return;

	if(sys_win9x)
	{
		BOOL old;
		SystemParametersInfo(SPI_SCREENSAVERRUNNING, 0, &old, 0);
	}
	else // WinNT
	{
		UnregisterHotKey(0, 0);
		UnregisterHotKey(0, 1);
		UnregisterHotKey(0, 2);
	}

	alttab_disabled=false;
}

// ------------------------- * Win32 main * -------------------------
#define MAX_NUM_ARGVS 128

int argc;
char *argv[MAX_NUM_ARGVS];

/*
** ParseCommandLine
**
** parses Win32 command line to (almost) ANSI compatible argc/argv[]
*/
void ParseCommandLine(LPSTR lpCmdLine)
{
	bool inquote;

	argc=1;
	argv[0]="exe";

	while(argc<MAX_NUM_ARGVS)
	{
	// skip whitespace
		while(*lpCmdLine && *lpCmdLine<=32)
			lpCmdLine++;

		if(!*lpCmdLine) break; // end of argument string

		argv[argc++]=lpCmdLine;

		inquote=false;
		while(*lpCmdLine && (inquote || *lpCmdLine>32))
			if(*lpCmdLine++=='"') inquote^=1;

		if(*lpCmdLine) *lpCmdLine++=0;
	}
}

/*
** WinMain
*/
int WINAPI WinMain(HINSTANCE hCurInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  MSG msg;
	int time, oldtime, newtime;
	cvar_t *min_ms_between_frames;
	int min_ms_between_frames_value;

// previous instances do not exist in Win32, but just in case
	if(hPrevInstance) return 0;

	hInst=hCurInstance;
	ParseCommandLine(lpCmdLine);

	COM_Init(argc, argv); // initialization

	min_ms_between_frames=Cvar_Get("min_ms_between_frames", "1", CVAR_ARCHIVE); // Not saved.
	min_ms_between_frames_value=(int) min_ms_between_frames->value;

	oldtime=Sys_Milliseconds();

// main window message loop
	while(1)
	{
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message==WM_QUIT) Sys_Quit(); // quit message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(Minimized || !ActiveApp) continue; // FIXME

		if(min_ms_between_frames->modified) {
			min_ms_between_frames_value=(int) min_ms_between_frames->value;
			// Originally, the code below always waited until time was >= 1,
			// so ensure that we don't allow the loop to exit when time == 0.
			// I'm not sure if that would cause problems or not, but limiting
			// players to <1000 FPS seems reasonable anyway!
			if(min_ms_between_frames_value<1) {
				min_ms_between_frames_value=1;
			}
		}

		do
		{
			newtime=Sys_Milliseconds();
			time=newtime-oldtime;
		} while(time<min_ms_between_frames_value);
//		Msg_Printf("time: %d - %d = %d", newtime, oldtime, time);
		oldtime=newtime;

		COM_Frame(time);
	}

// never gets here
	return 1;
}

