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
** common video functions /Win32 version/
*************************************************************************/
//#include "../common/common.h"
#include "../WolfDef.h"
#include "win32.h"

// ------------------------- * Globals * -------------------------
HWND sys_hwnd; // main window handle
cvar_t *win_noalttab;

vidmode_t vid_modes[NUM_MODES]=
{
	{"mode 0: 320x240",   320,	240},
	{"mode 1: 400x300",   400,	300},
	{"mode 2: 512x384",   512,	384},
	{"mode 3: 640x480",   640,	480},
	{"mode 4: 800x600",   800,	600},
	{"mode 5: 960x720",   960,	720},
	{"mode 6: 1024x768",  1024, 768},
	{"mode 7: 1152x864",  1152, 864},
	{"mode 8: 1280x960",  1280, 960},
	{"mode 9: 1600x1200",	1600, 1200},
	{"mode 10: 2048x1536",2048, 1536},
	{"mode 11: 320x200",	320,	200},
	{"mode 12: 640x400",	640,	400},
	{"mode 13: 1280x1024",1280,	1024}
};

// ------------------------- * Devider * -------------------------

/*
** Win_CreateWindow
*/
bool Win_CreateWindow(bool fullscreen, bool center, int width, int height, int bpp)
{
	DWORD style;
	RECT adjust;
	int x, y;

	style=fullscreen?WS_POPUP:WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;

	if(fullscreen || !center)
		x=y=0;
	else
	{
		x=(GetSystemMetrics(SM_CXSCREEN)-width)/2;
		y=(GetSystemMetrics(SM_CYSCREEN)-height)/2;
		if(x>y*2) x>>=1; // dual screens
		if(x<0) x=0;
		if(y<0) y=0;
	}
	adjust.left=x;
	adjust.top =y;
	adjust.right =x+width;
	adjust.bottom=y+height;

	AdjustWindowRect(&adjust, style, 0);

// make header visible if windowed
  if(!fullscreen && (adjust.left<0 || adjust.top<0))
	{
		int dx, dy;

		dx=-adjust.left;
		dy=-adjust.top;
		x+=dx; y+=dy;
		adjust.left=adjust.top=0;
		adjust.right+=dx;
		adjust.bottom+=dy;
	}

	sys_hwnd=CreateWindowEx(fullscreen?WS_EX_TOPMOST:0, WND_CLASS_NAME, NEWWOLF_NAME": Wolfenstein3D for OpenGL <"__DATE__">",
												style, adjust.left, adjust.top, 
												adjust.right-adjust.left, adjust.bottom-adjust.top, 
												NULL, NULL, hInst, NULL);

  if(!sys_hwnd) return false;


	return true;
}

/*
** AppActivate
**
** activates/deactivates whole app
*/
void AppActivate(bool fActive, bool minimize)
{
	Minimized=minimize;

	Key_ClearStates();

// we don't want to act like we're active if we're minimized
	if(fActive && !Minimized)
		ActiveApp=true;
	else
		ActiveApp=false;

	if(!ActiveApp)
	{
	// deactivation
		In_Activate(false);
		Vid_Activate(false);
		Win_EnableAltTab();
	}
	else
	{
	// activation
		Vid_Activate(true);
		In_Activate(true);
		if(win_noalttab->value) Win_DisableAltTab();
	}
}

// ------------------------- * Main Window Callback * -------------------------
static UINT MSH_MOUSEWHEEL;

// mouse wheel
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL (WM_MOUSELAST+1)  // message that will be supported by the OS 
#endif

// additional mouse buttons
#ifndef MK_XBUTTON1
#define MK_XBUTTON1			0x0020
#define MK_XBUTTON2			0x0040
#define WM_XBUTTONDOWN	0x020B
#define WM_XBUTTONUP		0x020C
#endif

/*
** MainWndProc
**
** return 0 if handled message, 1 if not
*/
static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
// mouse wheel /Win95 code/
	if(uMsg==MSH_MOUSEWHEEL)
	{
		if((int)wParam>0)
		{
			Key_Event(K_MWHEELUP, true);
			Key_Event(K_MWHEELUP, false);
		}
		else
		{
			Key_Event(K_MWHEELDOWN, true);
			Key_Event(K_MWHEELDOWN, false);
		}
		return 1;
	}

// message handling
	switch(uMsg)
	{
	case WM_KILLFOCUS:
		AppActivate(false, Minimized);
		break;
	case WM_ACTIVATE:
		AppActivate(LOWORD(wParam)!=WA_INACTIVE, Minimized=(BOOL)HIWORD(wParam));
		break;
	case WM_MOVE:
		if(ActiveApp) In_Activate(true);
		break;
	case WM_MOUSEWHEEL:
	// Mouse wheel (Microsoft Intellimouse) /NT 4.0, Win98 and later code/
		if((short)HIWORD(wParam)>0)
		{
			Key_Event(K_MWHEELUP, true);
			Key_Event(K_MWHEELUP, false);
		}
		else
		{
			Key_Event(K_MWHEELDOWN, true);
			Key_Event(K_MWHEELDOWN, false);
		}
		return 0;
	case WM_HOTKEY:
		return 0;
	case WM_CREATE:
		sys_hwnd=hWnd;
		MSH_MOUSEWHEEL=RegisterWindowMessage("MSWHEEL_ROLLMSG");
		break;
	case WM_DESTROY:
	// let sound and input know about this?
		sys_hwnd=NULL;
    break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	{
		int temp=0;
		if(wParam&MK_LBUTTON) temp|=1;
		if(wParam&MK_RBUTTON) temp|=2;
		if(wParam&MK_MBUTTON) temp|=4;
		if(wParam&MK_XBUTTON1) temp|=8;
		if(wParam&MK_XBUTTON2) temp|=16;
		In_MouseEvent(temp);
		return 0;
	}
	case WM_SYSCOMMAND:
		if(wParam==SC_SCREENSAVE) return 0;
		break;
	case WM_SYSKEYDOWN:
		if(wParam==13) // [Alt]+[Enter] toggles fullscreen
		{
			Cbuf_AddText("toggle r_fullscreen");
			return 0;
		}
	case WM_KEYDOWN:
		Key_Event(In_MapKey((int)lParam), true);
		return 0;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		Key_Event(In_MapKey((int)lParam), false);
		return 0;
	case WM_SYSCHAR:
		return 0; // keep Alt-Space from happening
	}

// default handling
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// ------------------------- * video initialization/deinitialization * -------------------------
cvar_t *vid_ref;
r_interface_t ri;

/*
** VID_Restart_f
**
** Console command to re-start video subsystem
*/
void VID_Restart_f(void)
{
	Con_Printf("Command not implemented\n");
//	vid_ref->modified=true;
}

/*
** Vid_Frame
**
** called every frame /generally checks for changes/
*/
void Vid_Frame(void)
{

// no Alt-Tab
	if(win_noalttab->modified)
	{
		if(win_noalttab->value)
			Win_DisableAltTab();
		else
			Win_EnableAltTab();
		win_noalttab->modified=false;
	}

// video refresh
	if(vid_ref->modified)
	{
		if(ri.R_ShutDown) ri.R_ShutDown();
		memset(&ri, 0, sizeof(ri));

//		if(strcmp(vid_ref->value, "gl"))
//		else // default renderer
		{
			WinGL_Init();
		}

		vid_ref->modified=false;
	}

	if(ri.R_Frame) ri.R_Frame();
}

/*
** Vid_Init
**
** video common initialization. called only once
*/
void Vid_Init(void)
{
	WNDCLASS wc;

	Con_Printf("Vid_Init: initializing video subsystem...\n");

// setting up window class
	memset(&wc, 0, sizeof(wc));
	wc.style        =CS_OWNDC;
	wc.lpfnWndProc  =(WNDPROC)MainWndProc;
	wc.hInstance    =hInst;
	wc.hIcon        =LoadIcon(hInst, MAKEINTRESOURCE(IDI_NEWWOLF));
	wc.hCursor      =LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName=WND_CLASS_NAME;

	if(!RegisterClass(&wc))
		Sys_Error("Vid_Init: Failed to register window class");

	vid_ref=Cvar_Get("vid_ref", "gl", CVAR_ARCHIVE);
	win_noalttab=Cvar_Get("win_noalttab", "0", CVAR_ARCHIVE);

	Cmd_AddCommand("vid_restart", VID_Restart_f);

	Vid_Frame();
}

/*
** Vid_Shutdown
*/
void Vid_Shutdown(void)
{
	if(ri.R_ShutDown) ri.R_ShutDown();
	memset(&ri, 0, sizeof(ri));
}

/*
** Vid_Activate
*/
void Vid_Activate(bool active)
{
	ActiveApp=active;
	if(ri.R_Activate) ri.R_Activate(active);
}

/*
** Vid_Modes
*/
int Vid_Modes(void)
{
	return NUM_MODES;
}

/*
** Vid_DescribeMode
*/
const char *Vid_DescribeMode(int mode)
{
	if(mode<0 || mode>=NUM_MODES) return "";
	return vid_modes[mode].description;
}

