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
** OpenGL based renderer
*************************************************************************/
#include "../common/common.h"
#include "r_gl.h"

// ------------------------- * Devider * -------------------------
char *glExtStr;
int glMaxTexSize; // maximum texture size 
float glMaxAniso; // maximum maximum degree of anisotropy [should be >=2.0] 0 if not supported

int glFilterMin=GL_LINEAR_MIPMAP_NEAREST;
int glFilterMax=GL_LINEAR;
int glFilterMode=5;

cvar_t *glRoundDown, *glPicMip;
cvar_t *glTextureMode;

// ------------------------- * aux * -------------------------

/*
** R_GL_ExtensionSupported
*/
bool R_GL_ExtensionSupported(const char *ext)
{
	char *start, *where, *terminator;

	if(!glExtStr) return false;

	start=glExtStr;
	while(1)
	{
		where=strstr(start, ext);
		if(!where) break;
		terminator=where+strlen(ext);
		if((where==start || *(where-1)==' ') && (*terminator==' ' || *terminator=='\0'))
			return true;
		start=terminator;
	}
	return false;
}

typedef struct glmode_s
{
	char *name;
	int	minimize, maximize;
} glmode_t;

glmode_t modes[]=
{
	{"GL_NEAREST", GL_NEAREST, GL_NEAREST},
	{"GL_LINEAR", GL_LINEAR, GL_LINEAR},
	{"GL_NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST},
	{"GL_LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR},
	{"GL_NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST},
	{"GL_LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR}
};

#define NUM_GL_MODES (sizeof(modes)/sizeof(glmode_t))

/*
** R_GL_TextureMode
*/
void R_GL_TextureMode(char *mode)
{
	cvar_t *glMode;
	int n;

	for(n=0; n<NUM_GL_MODES; n++)
	{
		if(!Q_strcasecmp(modes[n].name, mode))
			break;
	}

	if(n==NUM_GL_MODES)
		Con_Printf("bad filter name\n");
	else
		glFilterMode=n;

	glMode=Cvar_Set("r_texturemode", modes[glFilterMode].name);
	glMode->modified=false;

	glFilterMin=modes[glFilterMode].minimize;
	glFilterMax=modes[glFilterMode].maximize;

// update existing textures

	Con_Printf("texture filter set to %s\n", modes[glFilterMode].name);
}

// ------------------------- * Devider * -------------------------

/*
** R_GL_Init
*/
void R_GL_Init(void)
{
	char *p, *curExt;

	/* our cvars */
	glRoundDown=Cvar_Get("r_rounddown", "0", CVAR_ARCHIVE|CVAR_RESTART);
	glPicMip=Cvar_Get("r_picmip", "0", CVAR_ARCHIVE|CVAR_RESTART);
	glTextureMode=Cvar_Get("r_texturemode", "GL_LINEAR_MIPMAP_LINEAR", CVAR_ARCHIVE);

	/* OpenGL implementation info */
	Con_Printf("GL_Init: Initializing OpenGL...\n");
	Con_Printf("-> GL_VENDOR:   %s\n", glGetString(GL_VENDOR));
	Con_Printf("-> GL_RENDERER: %s\n", glGetString(GL_RENDERER));
	Con_Printf("-> GL_VERSION:  %s\n", glGetString(GL_VERSION));
	Con_Printf("-> GL_EXTENSIONS:\n");
	curExt=p=glExtStr=copystring(glGetString(GL_EXTENSIONS));

	while(1)
	{
		if(*p==' ')
		{
			*p=0;
			Con_Printf("--> %s\n", curExt);
			*p++=' ';
			if(!*p) break;
			curExt=p;
			continue;
		}
		if(*p=='\0')
		{
			Con_Printf("--> %s\n", curExt);
			break;
		}
		p++;
	}
	Con_Printf("-> GLU_VERSION: %s\n", gluGetString(GLU_VERSION));
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexSize);
	Con_Printf("-> Maximum texture size: %d\n", glMaxTexSize);

// checking for anisotropic filtering
	if(R_GL_ExtensionSupported("GL_EXT_texture_filter_anisotropic"))
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &glMaxAniso);
	else
		glMaxAniso=0;
	if(glMaxAniso<2.0f)
	{
		Con_Printf("-> Anisotropic filtering not supported\n");
		glMaxAniso=0;
	}
	else
		Con_Printf("-> Maximum degree of anisotropy: %4.2f\n", glMaxAniso);

// ---- * setting OpenGL defaults * ----

	/* viewport */
	glViewport(0, 0, ri.w, ri.h);
	glScissor(0, 0, ri.w, ri.h);

	/* set matrixes for 2D display */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WIDTH_2D, HEIGHT_2D, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0.375, 0.375, 0); // render all primitives at integer positions

	/* Misc Init */
	glClearColor(0, 0, 0, 0);
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DITHER);
	glEnable(GL_ALPHA_TEST);	// for models
	glEnable(GL_BLEND);

	/* Funcs Init */
	glDepthFunc(GL_LEQUAL);		// for multitexturing (kinda)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/* Hints */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_FOG_HINT, GL_NICEST);

	/* Clear */
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	/* changeable defaults */
	R_GL_TextureMode(glTextureMode->string);

	Con_Printf("OpenGL Initialized.\n");
}

/*
** R_GL_Frame
*/
void R_GL_Frame(void)
{
	if(glTextureMode->modified)
		R_GL_TextureMode(glTextureMode->string);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}

// ------------------------- * primitives * -------------------------

/*
** R_GL_Clear
*/
void R_GL_Clear(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}

/*
** R_GL_DumpScreen
**
** reads pixels from screen in RGB format
*/
void R_GL_DumpScreen(image_t *dump)
{
	dump->w=ri.w;
	dump->h=ri.h;
	dump->bpp=3;
	dump->data=Z_Malloc(ri.w*ri.h*3);

	glReadPixels(0, 0, ri.w, ri.h, GL_RGB, GL_UNSIGNED_BYTE, dump->data);
}

/*
** R_GL_DrawBox
*/
void R_GL_DrawBox(int x, int y, int w, int h, rgba c)
{
	glDisable(GL_TEXTURE_2D);
	glColor4ubv((GLubyte *)&c);

	glBegin(GL_QUADS);
		glVertex2i(x, y);
		glVertex2i(x, y+h);
		glVertex2i(x+w, y+h);
		glVertex2i(x+w, y);
	glEnd();
}

/*
** R_GL_DrawPic
**
** draws picture at its original size
*/
void R_GL_DrawPic(int x, int y, image_t *pic)
{
	R_GL_DrawPicScale(x, y, pic->w, pic->h, pic);
}

/*
** R_GL_DrawPicScale
**
** draw picture scaling it to specified size
*/
void R_GL_DrawPicScale(int x, int y, int w, int h, image_t *pic)
{
	R_GL_SelectTexture(pic);

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2i(x, y);
		glTexCoord2f(0, 1); glVertex2i(x, y+h);
		glTexCoord2f(1, 1); glVertex2i(x+w, y+h);
		glTexCoord2f(1, 0); glVertex2i(x+w, y);
	glEnd();
}

// ------------------------- * textures * -------------------------

/*
** R_GL_UploadTexture
**
** uploads texture to the renderer so that it could be selected
*/
void R_GL_UploadTexture(image_t *tex, bool mipmap, bool aniso, bool clamp)
{
	int sw, sh;
	char *upData;

	glGenTextures(1, &tex->glTex);
	glBindTexture(GL_TEXTURE_2D, tex->glTex);

// set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap?glFilterMin:glFilterMax);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilterMax);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp?GL_CLAMP_TO_EDGE:GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp?GL_CLAMP_TO_EDGE:GL_REPEAT);
	if(aniso && glMaxAniso)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2);

// scaled size, should be a power of 2, not greater then glMaxTexSize
	for(sw=1; sw<tex->w; sw<<=1);
	for(sh=1; sh<tex->h; sh<<=1);
	if(sw>glMaxTexSize) sw=glMaxTexSize;
	if(sh>glMaxTexSize) sh=glMaxTexSize;

	if(glRoundDown->value)
	{
		if(sw>tex->w) sw>>=1;
		if(sh>tex->h) sh>>=1;
	}
	if(mipmap && glPicMip->value>0)
	{
		sw>>=(int)glPicMip->value;
		if(!sw) sw=1;
		sh>>=(int)glPicMip->value;
		if(!sh) sh=1;
	}

// 3dfx board has a limit on max aspect ratio (1:8)
	if(glMaxTexSize==256)
	{
		if((sw<<3)<sh)
			sw=sh>>3;
		else if((sh<<3)<sw)
			sh=sw>>3;
	}

	upData=Z_Malloc(sw*sh*tex->bpp);
	if(sw!=tex->w || sh!=tex->h)
		Img_Resample(upData, sw, sh, tex->data, tex->w, tex->h, tex->bpp);
	else
		memcpy(upData, tex->data, sw*sh*tex->bpp);

// uploading base image
	glTexImage2D(GL_TEXTURE_2D, 0, tex->bpp, sw, sh, 0, tex->bpp==4?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, upData);
// uploading mipmaps if required
	if(mipmap)
	{
		int level=0;

		while(Img_MipMap(upData, &sw, &sh, tex->bpp))
			glTexImage2D(GL_TEXTURE_2D, ++level, tex->bpp, sw, sh, 0, tex->bpp==4?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, upData);
	}

// free data
	Z_Free(upData);
	free(tex->data);
	tex->data=NULL;
}

/*
** R_GL_UnloadTexture
**
** unloads texture from the renderer, frees all the associated memory
*/
void R_GL_UnloadTexture(image_t *tex)
{
	glDeleteTextures(1, &tex->glTex);
}

/*
** R_GL_SelectTexture
**
** selects target texture
** if tex==NULL default texture is selected
*/
void R_GL_SelectTexture(image_t *tex)
{
	if(!tex) return; // FIXME: select default???
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex->glTex);
}

// ------------------------- * 3D view refresh * -------------------------
bool tile_vis[64][64];

/*
** R_GL_ViewRefresh
**
** refreshes level geometry
*//*
void R_GL_ViewRefresh(int x, int y, int angle, bool recursive)
{
	int n, x, y, vx, vy;
	r_trace_t trace;
	bool p_wall=false;

// clear tile visible flags
	memset(tile_vis, 0, sizeof(tile_vis));

// viewport tile coordinates
	vx=POS2TILE(x);
	vy=POS2TILE(y);

// little hack to make sure tile, where player is, is visible
// raycaster may occasionally miss it
	tile_visible[vx][vy]=true;
	AM_AutoMap.vis[vx][vy]=true;


	for(n=0; n<XRES; n++)

// setup raycaster
	trace.x=x;
	trace.y=y;
}
*/






