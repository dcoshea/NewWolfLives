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
** system functions and definitions /Win32 version/
*************************************************************************/
#include <windows.h>
#include "resource.h"

#define WND_CLASS_NAME	"NewWolf"
#define NUM_MODES				14

extern bool sys_win9x;	// true - Win9x; false - WinNT
extern HINSTANCE hInst; // application instance
extern HWND sys_hwnd;		// main window handle
extern bool ActiveApp, Minimized;

int In_MapKey(int key);
void In_MouseEvent(int mstate);

void Win_DisableAltTab(void);
void Win_EnableAltTab(void);
bool Win_CreateWindow(bool FullScreen, bool Center, int width, int height, int bpp);

void WinGL_Init(void);
