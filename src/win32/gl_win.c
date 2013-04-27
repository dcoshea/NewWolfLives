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
** OpenGL ref initialization /Win32 version/
*************************************************************************/
#include "../common/common.h"
#include "win32.h"
#include "../r_gl/r_gl.h"

// ------------------------- * Globals * -------------------------
HDC hDC;
HGLRC glRC;
static PIXELFORMATDESCRIPTOR pfd=
{
	sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
	1,															// version number
	PFD_DRAW_TO_WINDOW |						// support window
	PFD_SUPPORT_OPENGL |						// support OpenGL
	PFD_DOUBLEBUFFER,								// double buffered
	PFD_TYPE_RGBA,									// RGBA type
	24,															// 24-bit color depth
	0, 0, 0, 0, 0, 0,								// color bits ignored
	0,															// no alpha buffer
	0,															// shift bit ignored
	0,															// no accumulation buffer
	0, 0, 0, 0,											// accum bits ignored
	32,															// 32-bit z-buffer
	0,															// no stencil buffer
	0,															// no auxiliary buffer
	PFD_MAIN_PLANE,									// main layer
	0,															// reserved
	0, 0, 0													// layer masks ignored
};

cvar_t *r_gamma, *r_vsync;

bool (APIENTRY *wglSwapIntervalEXT)(int interval);

// ------------------------- * hardware gamma stuff * -------------------------
word original_ramp[3][256], gamma_ramp[3][256];
bool is_ramp;

/*
** Win_InitGammaRamp
*/
bool Win_InitGammaRamp(HDC hDC)
{
	return is_ramp=GetDeviceGammaRamp(hDC, original_ramp);
}

/*
** Win_SetGammaRamp
*/
void Win_SetGammaRamp(HDC hDC, float value)
{
	int n, v;

	if(!is_ramp) return;

	for(n=0; n<256; n++)
	{
		v=(int)(255*pow(n/255.0, value));
		if(v<0) v=0;
		else if(v>255) v=255;
		gamma_ramp[0][n]=gamma_ramp[1][n]=gamma_ramp[2][n]=(WORD)v<<8;
	}
	SetDeviceGammaRamp(hDC, gamma_ramp);
}

/*
** Win_ShutDownGammaRamp
*/
void Win_ShutDownGammaRamp(HDC hDC)
{
	if(!is_ramp) return;
	SetDeviceGammaRamp(hDC, original_ramp);
}

// ------------------------- * window activation * -------------------------
bool wingl_active, wingl_appactive;

/*
** WinGL_ActivateWindow
*/
void WinGL_ActivateWindow(void)
{
	if(wingl_active || !wingl_appactive) return;
	wingl_active=true;

	if(ri.fs)
	{
		DEVMODE dmScreenSettings;

		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth =ri.w;
		dmScreenSettings.dmPelsHeight=ri.h;
		dmScreenSettings.dmBitsPerPel=ri.bpp;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
	// trying to set selected resolution
		Con_Printf("Setting resolution to %dx%dx%d\n", ri.w, ri.h, ri.bpp);
		if(!ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN)==DISP_CHANGE_SUCCESSFUL)
			Sys_Error("Error setting video mode %dx%dx%d\n", ri.w, ri.h, ri.bpp);
	}
	ShowWindow(sys_hwnd, SW_SHOWNORMAL);
	BringWindowToTop(sys_hwnd);
}

/*
** WinGL_DeActivateWindow
*/
void WinGL_DeActivateWindow(void)
{
	if(!wingl_active) return;
	wingl_active=false;

	if(ri.fs)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowWindow(sys_hwnd, SW_SHOWMINNOACTIVE);
	}
}

// ------------------------- * Initialization/Deinitialization * -------------------------
void WinGL_Shutdown(void);
void WinGL_Frame(void);
void WinGL_Activate(bool active);
void WinGL_Swap(void);

/*
** WinGL_Init
**
** Sets up OpenGL under Win32. may be called multiple times
*/
void WinGL_Init(void)
{
	cvar_t *r_mode, *r_colorbits, *r_fullscreen;
	int mode, pixelFormat;

	Con_Printf("--- Starting Win32-OpenGL refresh system ---\n");

// registering cvars
	r_gamma=Cvar_Get("r_gamma", "1", CVAR_ARCHIVE);
	r_vsync=Cvar_Get("r_vsync", "1", CVAR_ARCHIVE);

// reading settings
	r_mode=Cvar_Get("r_mode", "3", CVAR_ARCHIVE|CVAR_RESTART);
	r_colorbits=Cvar_Get("r_colorbits", "16", CVAR_ARCHIVE|CVAR_RESTART);
	r_fullscreen=Cvar_Get("r_fullscreen", "1", CVAR_ARCHIVE|CVAR_RESTART);

	mode=(int)r_mode->value;
	if(mode<0 || mode>=Vid_Modes()) mode=3; // invalid mode
	ri.bpp=(int)r_colorbits->value;
	if(ri.bpp!=16 && ri.bpp!=32) ri.bpp=16;

	ri.w=vid_modes[mode].width;
	ri.h=vid_modes[mode].height;
	ri.fs=(bool)(r_fullscreen->value);
	Cvar_SetInteger("r_mode", mode);
	Cvar_SetInteger("r_colorbits", ri.bpp);

// creating window
	Con_Printf("-> Using %s %d bpp %s\n", vid_modes[mode].description, ri.bpp, ri.bpp?"FS":"W");
	if(!Win_CreateWindow(ri.fs, false, ri.w, ri.h, ri.bpp))
		Sys_Error("Failed to create window");

// initialize OpenGL
	Con_Printf("-> Initializing OpenGL...\n");

	if(!(hDC=GetDC(sys_hwnd)))
		Sys_Error("Error getting Device Context");

	pfd.cColorBits=ri.bpp;
	Con_Printf("-> Asking for PFD: (%d, %d, %d)\n", pfd.cColorBits, pfd.cDepthBits, pfd.cStencilBits);
	if(!(pixelFormat=ChoosePixelFormat(hDC, &pfd)))
		Sys_Error("Error choosing Pixel Format");
	DescribePixelFormat(hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	Con_Printf("-> Returned PFD #%d: (%d, %d, %d)\n", pixelFormat, pfd.cColorBits, pfd.cDepthBits, pfd.cStencilBits);
  if(!SetPixelFormat(hDC, pixelFormat, &pfd))
		Sys_Error("Error setting Pixel Format");
	if(!(glRC=wglCreateContext(hDC)))
		Sys_Error("Error creating OpenGL Rendering Context");
	if(!wglMakeCurrent(hDC, glRC))
		Sys_Error("Error setting current OpenGL Rendering Context");

// setting up OpenGL /portable part/
	R_GL_Init();

// windows specific extensions
	if(Win_InitGammaRamp(hDC))
		Con_Printf("-> Hardware gamma control found\n");
	if(R_GL_ExtensionSupported("WGL_EXT_swap_control"))
	{
		if(wglSwapIntervalEXT=(void *)wglGetProcAddress("wglSwapIntervalEXT"))
			Con_Printf("-> Vertical synchronization control found\n");
	}
	else
		wglSwapIntervalEXT=NULL;

// setup interface
	ri.R_ShutDown=WinGL_Shutdown;
	ri.R_Frame=WinGL_Frame;
	ri.R_Activate=WinGL_Activate;
	ri.R_Swap=WinGL_Swap;

	ri.R_Clear=R_GL_Clear;
	ri.R_DumpScreen=R_GL_DumpScreen;
	ri.R_DrawBox=R_GL_DrawBox;
	ri.R_DrawPic=R_GL_DrawPic;
	ri.R_DrawPicScale=R_GL_DrawPicScale;

	ri.R_UploadTexture=R_GL_UploadTexture;
	ri.R_UnloadTexture=R_GL_UnloadTexture;
	ri.R_SelectTexture=R_GL_SelectTexture;

	wingl_appactive=true;
	wingl_active=false;
	Cvar_ForceSet("vid_ref", "gl");
	Cvar_FullSet("r_driver", "Win32-OpenGL", CVAR_NOSET|CVAR_NODEFAULT);
	Con_Printf("--- Done ---\n");
}

/*
** WinGL_Shutdown
*/
void WinGL_Shutdown(void)
{
	Con_Printf("--- Shutting Down Win32-OpenGL refresh system ---\n");

	if(hDC)
	{
		wglMakeCurrent(hDC, NULL);
		if(glRC)
		{
			wglDeleteContext(glRC);
			glRC=NULL;
		}
		Win_ShutDownGammaRamp(hDC);
		ReleaseDC(sys_hwnd, hDC);
		hDC=NULL;
	}
	DestroyWindow(sys_hwnd);
	WinGL_DeActivateWindow();

	Con_Printf("--- Done ---\n");
}

/*
** WinGL_Frame
*/
void WinGL_Frame(void)
{
	R_GL_Frame();
	if(r_gamma->modified)
	{
		if(r_gamma->value<0 || r_gamma->value>1.75)
		{
			Con_Printf("r_gamma shoud be in range [0, 1.75]\n");
			Cvar_SetValue("r_gamma", 1);
		}
		Win_SetGammaRamp(hDC, 2-r_gamma->value);
		r_gamma->modified=false;
	}
	if(r_vsync->modified && wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT(r_vsync->value?1:0);
		r_vsync->modified=false;
	}

	if(!wingl_appactive) return;
	WinGL_ActivateWindow();
}

/*
** WinGL_Activate
*/
void WinGL_Activate(bool active)
{
	if(!active) WinGL_DeActivateWindow();
	wingl_appactive=active;
	wingl_active=!active; // force a new window check or turn off
}

/*
** WinGL_Swap
*/
void WinGL_Swap(void)
{
	glFinish(); //glFlush();
	SwapBuffers(hDC);
}

