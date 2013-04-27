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
** OpenGL renderer definitions
*************************************************************************/
#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glext.h"

void R_GL_Init(void);
void R_GL_Frame(void);
bool R_GL_ExtensionSupported(const char *ext);

// primitives
void R_GL_Clear(void);
void R_GL_DumpScreen(image_t *dump);
void R_GL_DrawBox(int x, int y, int w, int h, rgba c);
void R_GL_DrawPic(int x, int y, image_t *pic);
void R_GL_DrawPicScale(int x, int y, int w, int h, image_t *pic);
// textures
void R_GL_UploadTexture(image_t *tex, bool mipmap, bool aniso, bool clamp);
void R_GL_UnloadTexture(image_t *tex);
void R_GL_SelectTexture(image_t *pic);

