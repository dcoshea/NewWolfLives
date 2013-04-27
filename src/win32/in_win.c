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
** input /Win32 version/
*************************************************************************/
#include "../WolfDef.h"
//#include "../common/common.h"
#include "win32.h"

// ------------------------- * key mapping * -------------------------
byte scantokey[128]=
{
	0, K_ESCAPE, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', K_BACKSPACE, // 0..14
	K_TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', K_ENTER, // 14..28
	K_CTRL, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', // 29..41
	K_SHIFT, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', K_SHIFT, // 42..54
	K_KP_STAR, K_ALT, K_SPACE, K_CAPSLOCK, // 55..58
	K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_PAUSE, K_SCROLLLOCK, // 59..70
	K_HOME, K_UPARROW, K_PGUP, K_KP_MINUS, // 71..74
	K_LEFTARROW, K_KP_5, K_RIGHTARROW, K_KP_PLUS, // 75..78
	K_END, K_DOWNARROW, K_PGDN, K_INS, K_DEL, // 79..83
	0, 0, 0, K_F11, K_F12, 0, 0, K_WIN, K_WIN, K_MENU, 0, 0, // 84..95
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
}; 

/*
** In_MapKey
**
** maps from windows keynums
*/
int In_MapKey(int key)
{
	int result;
	bool is_extended;

	result=(key>>16)&255;
	if(result>127) return 0;
	is_extended=key&(1<<24)?true:false;

	result=scantokey[result];

	if(!is_extended) switch(result)
	{
	case K_HOME:
		return K_KP_HOME;
	case K_UPARROW:
		return K_KP_UPARROW;
	case K_PGUP:
		return K_KP_PGUP;
	case K_LEFTARROW:
		return K_KP_LEFTARROW;
	case K_RIGHTARROW:
		return K_KP_RIGHTARROW;
	case K_END:
		return K_KP_END;
	case K_DOWNARROW:
		return K_KP_DOWNARROW;
	case K_PGDN:
		return K_KP_PGDN;
	case K_INS:
		return K_KP_INS;
	case K_DEL:
		return K_KP_DEL;
	default:
		return result;
	}

	switch(result)
	{
	case K_ENTER:
		return K_KP_ENTER;
	case '/':
		return K_KP_SLASH;
	case K_PAUSE:
		return K_NUMLOCK;
	}
	return result;
}

// ------------------------- * Mouse * -------------------------
cvar_t *m_filter, *m_forward, *m_side, *m_pitch, *m_yaw;
cvar_t *in_mouse;
cvar_t *sensitivity, *mousemove;

int mouse_buttons, mouse_oldbuttonstate;
int mouse_x, mouse_y, old_mouse_x, old_mouse_y, wnd_cx, wnd_cy;
int origmouse[3], newmouse[3]={0, 0, 1};
bool mouseinitialized, mouseactive, mouseparmsvalid, restore_spi;

/*
** In_ActivateMouse
*/
void In_ActivateMouse(void)
{
	int w, h;
	RECT r, r1;

	if(!mouseinitialized || !sys_hwnd || mouseactive) return;
	if(!in_mouse->value)
	{
		mouseactive=false;
		return;
	}
	mouseactive=true;

// set mouse acceleration params
	if(mouseparmsvalid)
		restore_spi=SystemParametersInfo(SPI_SETMOUSE, 0, newmouse, 0);
//set clipping region
	w=GetSystemMetrics(SM_CXSCREEN);
	h=GetSystemMetrics(SM_CYSCREEN);
	GetClientRect(sys_hwnd, &r);
	GetWindowRect(sys_hwnd, &r1);
	r.left+=r1.left;
	r.right+=r1.left;
	r.top+=r1.top;
	r.bottom+=r1.top;

	if(r.left<0) r.left=0;
	if(r.top<0) r.top=0;
	if(r.right>=w) r.right=w-1;
	if(r.bottom>=h) r.bottom=h-1;
// set cursor starting position
	wnd_cx=(r.right+r.left)/2;
	wnd_cy=(r.top+r.bottom)/2;
	SetCursorPos(wnd_cx, wnd_cy);
	old_mouse_x=old_mouse_y=0;
	mouse_oldbuttonstate=0;

	SetCapture(sys_hwnd);
	ClipCursor(&r);
	while(ShowCursor(FALSE)>=0);
}

/*
** In_DeactivateMouse
*/
void In_DeactivateMouse(void)
{
	if(!mouseinitialized || !mouseactive) return;
	mouseactive=false;

	if(restore_spi)
		SystemParametersInfo(SPI_SETMOUSE, 0, origmouse, 0);

	ClipCursor(NULL);
	ReleaseCapture();
	while(ShowCursor(TRUE)<0);
}

/*
** In_StartupMouse
*/
void In_StartupMouse(void)
{
	cvar_t *cv;

	cv=Cvar_Get("in_initmouse", "1", CVAR_NOSET);
	if(!cv->value) return;

	mouseinitialized=true;
	mouseparmsvalid=SystemParametersInfo(SPI_GETMOUSE, 0, origmouse, 0);
	mouse_buttons=5;
}

/*
** In_MouseEvent
*/
void In_MouseEvent(int mstate)
{
	int n;

	if(!mouseinitialized) return;

// perform button actions
	for(n=0; n<mouse_buttons; n++)
	{
		if(mstate&(1<<n) && !(mouse_oldbuttonstate&(1<<n)))
			Key_Event(K_MOUSE1+n, true);

		if(!(mstate&(1<<n)) && mouse_oldbuttonstate&(1<<n))
			Key_Event(K_MOUSE1+n, false);
	}

	mouse_oldbuttonstate=mstate;
}

/*
** In_MouseMove
*/
void In_MouseMove(usercmd_t *cmd)
{
	POINT	current_pos;
	int mx, my;

	if(!mouseactive) return;

// find mouse movement
	if(!GetCursorPos(&current_pos)) return;
	mx=current_pos.x-wnd_cx;
	my=current_pos.y-wnd_cy;

	if(m_filter->value)
	{
		mouse_x=(mx+old_mouse_x)/2;
		mouse_y=(my+old_mouse_y)/2;
	}
	else
	{
		mouse_x=mx;
		mouse_y=my;
	}

	old_mouse_x=mx;
	old_mouse_y=my;

	mouse_x=(int)(mouse_x*sensitivity->value);
	mouse_y=(int)(mouse_y*sensitivity->value);

// add mouse X/Y movement to cmd
	if(in_strafe.state&1 || lookstrafe->value)
		cmd->sidemove+=(int)(m_side->value*mouse_x);
	else
		cmd->angle-=(int)(m_yaw->value*mouse_x);

	if(mousemove->value)
		cmd->forwardmove-=(int)(m_forward->value*mouse_y);

	// force the mouse to the center, so there's room to move
	if(mx || my)
		SetCursorPos(wnd_cx, wnd_cy);
}

// ------------------------- * common init founctions * -------------------------
bool in_appactive;

/*
** In_Init
*/
void In_Init(void)
{
// mouse variables
	in_mouse=Cvar_Get("in_mouse", "1", CVAR_ARCHIVE);
	m_filter=Cvar_Get("m_filter", "1", CVAR_ARCHIVE);
	m_forward=Cvar_Get("m_forward", "50", CVAR_ARCHIVE);
	m_side=Cvar_Get("m_side", "50", CVAR_ARCHIVE);
	m_pitch=Cvar_Get("m_pitch", "1", CVAR_ARCHIVE);
	m_yaw=Cvar_Get("m_yaw", "1", CVAR_ARCHIVE);
	mousemove=Cvar_Get("mousemove", "0", CVAR_ARCHIVE);
	sensitivity=Cvar_Get("sensitivity", "15", CVAR_ARCHIVE);

//	Cmd_AddCommand("+mlook", In_MLookDown);
//	Cmd_AddCommand("-mlook", In_MLookUp);

	In_StartupMouse();
}

/*
** In_Shutdown
*/
void In_Shutdown(void)
{
	In_DeactivateMouse();
}

/*
** In_Activate
**
** Called when the main window gains or loses focus.
** The window may have been destroyed and recreated
** between a deactivate and an activate.
*/
void In_Activate(bool active)
{
	in_appactive=active;
	mouseactive=!active; // force a new window check or turn off

// FIXME: this one wouldn't be required
	if(!active) In_DeactivateMouse();
}

/*
** In_Frame
**
** Called every frame, even if not generating commands
*/
void In_Frame(void)
{
	if(!mouseinitialized) return;

// temporarily deactivate mouse if inactive or in console and windowed
	if(!in_appactive || (key_dest==key_console && !ri.fs))
	{
		In_DeactivateMouse();
		return;
	}

	In_ActivateMouse();
}

/*
** In_Move
*/
void In_Move(usercmd_t *cmd)
{
	In_MouseMove(cmd);
}
