// Texture Manager
#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "r_gl/glext.h"
#include "WolfDef.h"

#define Vid_CacheTexPair(name) {Vid_CacheTex(name); Vid_CacheTex((name)+1);}
#define Vid_CacheTexPairEx(name) Vid_CacheTexPair((name)<<1)

// ------------------------- * Picture Loding & Caching * -------------------------
#define MAX_CACHED_TEXS 1024
texture_t tex_cache[MAX_CACHED_TEXS], tex_default, tex_conback, tex_blood, tex_brick;
int numcachetexs=0;

// ------------------------- * Init & ShutDown * -------------------------

/*
** Vid_InitTexMan
**
** Texture manager Initialization
*/
int Vid_InitTexMan(void)
{
	unsigned char *dest;
	int x, y;

	Con_Printf("Initializing TexMan...");

// create a simple checkerboard texture (used as a default)
	tex_default.data=malloc(16*16*3);
	for(y=0; y<16; y++)
		for(x=0; x<16; x++)
	{
		dest=&tex_default.data[(y*16+x)*3];
		if((y<8)^(x<8))
			{dest[0]=0x00; dest[1]=0x00; dest[2]=0x00;}
		else
			{dest[0]=0x9F; dest[1]=0x5B; dest[2]=0x53;}
	}
	tex_default.width=tex_default.height=16;
	tex_default.bpp=3;
	tex_default.name=-1;
	Vid_UploadTexture(&tex_default, true, false);

// load console background texture
	Img_Read("textures/ui/conback", (image_t *)&tex_conback);
	Vid_UploadTexture(&tex_conback, false, false);

	Img_Read("textures/particles/blood.tga", (image_t *)&tex_blood);
	Vid_UploadTexture(&tex_blood, true, false);
	Img_Read("textures/particles/brick.tga", (image_t *)&tex_brick);
	Vid_UploadTexture(&tex_brick, true, false);

	Con_Printf("done\n");
	return 1;
}

/*
** Vid_ShutDownTexMan
**
** Texture manager ShutDown
*/
void Vid_ShutDownTexMan(void)
{
	Vid_UnCacheAllTexs();
	Vid_UnLoadTexture(&tex_default);	// remove default texture
	Vid_UnLoadTexture(&tex_conback);	// remove conback textures
}

// ------------------------- * OpenGL texture uploading * -------------------------

/*
** Vid_UploadTexture
**
** uploads texture to the renderer so that it could be selected
*/
void Vid_UploadTexture(texture_t *tex, bool mipmap, bool aniso)
{
	ri.R_UploadTexture((image_t *)tex, mipmap, aniso, true);
}

/*
** Vid_UnLoadTexture
**
** unloads texture from the renderer, frees all the associated memory
*/
void Vid_UnLoadTexture(texture_t *tex)
{
	ri.R_UnloadTexture((image_t *)tex);
}

// ------------------------- * Devider * -------------------------

texture_t *Vid_FindCachedTex(int name)
{
	texture_t *tex;
	int n;

	for(tex=tex_cache, n=0; n<numcachetexs; tex++, n++)
		if(name==tex->name) return tex;
	return NULL;
}

texture_t *Vid_CacheTex(int name)
{
	texture_t *tex;
	int n;

	for(tex=tex_cache, n=0; n<numcachetexs; tex++, n++)
		if(name==tex->name) return tex;

	if(numcachetexs==MAX_CACHED_TEXS)
	{
		Con_Printf("Warning! numcachetexs == MAX_CACHED_TEXS\n");
		return NULL;
	}

	tex->name=name;
	if(!File_LoadTexture(tex)) return NULL;
	numcachetexs++;

	Vid_UploadTexture(tex, true, true);
	return tex;
}

void Vid_UnCacheTex(int name)
{
	texture_t *tex;

	tex=Vid_FindCachedTex(name);
	if(!tex) return;

	Vid_UnLoadTexture(tex);
	memmove(tex, tex+1, (byte*)&tex_cache[--numcachetexs]-(byte*)tex);
}

void Vid_UnCacheAllTexs(void)
{
	texture_t *tex;
	int n;

	for(tex=tex_cache, n=0; n<numcachetexs; tex++, n++)
		Vid_UnLoadTexture(tex);
	numcachetexs=0;
}

// precaches all needed textures for current level
void Vid_PrecacheTextures(void)
{
	int n=0;
	int x, y;

	Con_Printf("Precaching...");

	Vid_UnCacheAllTexs();
	for(x=0; x<64; x++)
		for(y=0; y<64; y++)
		{
			if(CurMapData.tile_info[x][y]&TILE_IS_WALL)
			{
				Vid_CacheTex(CurMapData.wall_tex_x[x][y]);
				Vid_CacheTex(CurMapData.wall_tex_y[x][y]);
			}
		}

// Any Level needs doors (allmost any)
	Vid_CacheTexPair(TEX_DDOOR);
	Vid_CacheTexPair(TEX_PLATE);
	Vid_CacheTexPair(TEX_DELEV);
	Vid_CacheTexPair(TEX_DLOCK);
	Con_Printf(" done [%d textures]\n", numcachetexs);
}

// ------------------------- * Texture Selection * -------------------------

/*
** Vid_SelectTextureIndirect
**
** Selects texutre tex,
** if tex==NULL default texture is selected
*/
void Vid_SelectTextureIndirect(texture_t *tex)
{
	if(!tex) tex=&tex_default;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex->glTex);
}

/*
** Vid_SelectTexture
**
** Select texture if it is cached
**  if not default will be used!
*/
void Vid_SelectTexture(int name)
{
	Vid_SelectTextureIndirect(Vid_FindCachedTex(name));
}

/*
** Vid_SelectTextureEx
**
** Same as Vid_SelectTexture; but will cache if texture is not cached
*/
void Vid_SelectTextureEx(int name)
{
	Vid_SelectTextureIndirect(Vid_CacheTex(name));
}