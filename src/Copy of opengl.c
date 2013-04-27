// OpenGL low level Video!
#include "WolfDef.h"
#include "win32/win32.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "r_gl/glext.h"

// ------------------------- * Devider * -------------------------
#define DEFAULT_FOV		75		// degrees
#define DEFAULT_FOVs "75"		// the same as above, a string

typedef struct fog_s
{
	int Mode;
	float Color[4];
	float Density;		// for Exp & Exp2
	float Start, End;	// for Linear (it suck, IMHO)
} fog_t;

int fogModes[3]={GL_LINEAR, GL_EXP, GL_EXP2};
fog_t FogStat;
float cur_x_fov, cur_y_fov; // x & y field of view (in degrees)
float ratio; // viewport width/height

int maxTexSize=256; // maximum texture size 
float maxAniso=0;		// maximum maximum degree of anisotropy [should be >=2.0] 0 if not supported

static GLuint texture[NUM_TEXTURES];

cvar_t *vid_fov;	// 1..179 degrees
cvar_t *vid_fog;	// {0:"OFF" | 1:"GL_LINEAR" | 2:"GL_EXP" | 3:"GL_EXP2"}
cvar_t *vid_fogd;	// 0.0....1.0
cvar_t *vid_fogs;	// 1.0...64.0
cvar_t *vid_foge;	// 1.0...64.0
cvar_t *vid_fogr;	// 0.0....1.0
cvar_t *vid_fogg;	// 0.0....1.0
cvar_t *vid_fogb;	// 0.0....1.0
cvar_t *vid_foga;	// 0.0....1.0
cvar_t *mdl_s, *mdl_x, *mdl_y, *mdl_z;

void GL_SetDefaults(void);

// ------------------------- * OpenGL video init & shut down * -------------------------

int GL_Init(void)
{
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
	if(maxAniso<2) maxAniso=0;

	vid_fov=Cvar_Get("fov", DEFAULT_FOVs, CVAR_ARCHIVE);
	vid_fog=Cvar_Get("fog", "0", CVAR_ARCHIVE);
	vid_fogd=Cvar_Get("fog_density", "0.1", CVAR_ARCHIVE);
	vid_fogs=Cvar_Get("fog_start", "1.0", CVAR_ARCHIVE);
	vid_foge=Cvar_Get("fog_end", "0.1", CVAR_ARCHIVE);
	vid_fogr=Cvar_Get("fog_r", "0.1", CVAR_ARCHIVE);
	vid_fogg=Cvar_Get("fog_g", "0.1", CVAR_ARCHIVE);
	vid_fogb=Cvar_Get("fog_b", "0.1", CVAR_ARCHIVE);
	vid_foga=Cvar_Get("fog_a", "0.1", CVAR_ARCHIVE);
	mdl_x=Cvar_Get("r_mdl_x", "0", 0);
	mdl_y=Cvar_Get("r_mdl_y", "0", 0);
	mdl_z=Cvar_Get("r_mdl_z", "0", 0);
	mdl_s=Cvar_Get("r_mdl_scale", "0", 0);

	Con_Printf("OpenGL Initialized!\n");
	return 1;
}

void GL_ShutDown(void)
{
}

// ------------------------- * OpenGL Aux Functions * -------------------------

/*
** GL_CheckFOV
**
** Recalculate fov (cvar) change & updates screen if nescesary
*/
void GL_CheckFOV(void)
{
	if(!vid_fov->modified) return;
	vid_fov->modified=false;

	if(vid_fov->value<1 || vid_fov->value>179)
	{
		Con_Printf("Wrong FOV: %f\n", vid_fov->value);
		Cvar_SetValue(vid_fov->name, (cur_x_fov>=1 && cur_x_fov<=179)?cur_x_fov:DEFAULT_FOV);
	}

	ratio=(float)ri.w/(float)ri.h; // FIXME: move somewhere
	cur_x_fov=vid_fov->value;
	cur_y_fov=CalcFov(cur_x_fov, (float)ri.w, (float)ri.h);
	Con_Printf("Setting FOV: x=%f, y=%f\n", cur_x_fov, cur_y_fov);
}

void GL_CheckFog(void)
{
// fog mode...
	if(vid_fog->value!=FogStat.Mode)
	{
		if(vid_fog->value<0 || vid_fog->value>2)
			FogStat.Mode=(FogStat.Mode>=0 && FogStat.Mode<=2)?FogStat.Mode:0;
		else
			FogStat.Mode=(int)vid_fog->value-1;
		Cvar_SetInteger(vid_fog->name, FogStat.Mode);
		glFogi(GL_FOG_MODE, fogModes[FogStat.Mode]);
	}
	if(vid_fog->value) glEnable(GL_FOG);

// fog density
	if(vid_fogd->value!=FogStat.Density)
		glFogf(GL_FOG_DENSITY, FogStat.Density=vid_fogd->value);
// fog start, end
	if(vid_fogs->value!=FogStat.Start)
		glFogf(GL_FOG_START, FogStat.Start=vid_fogs->value);
	if(vid_foge->value!=FogStat.End)
		glFogf(GL_FOG_END, FogStat.End=vid_foge->value);
// fog color
	if(vid_fogr->value!=FogStat.Color[0] || vid_fogg->value!=FogStat.Color[1] || vid_fogb->value!=FogStat.Color[2] || vid_foga->value!=FogStat.Color[3])
	{
		FogStat.Color[0]=vid_fogr->value;
		FogStat.Color[1]=vid_fogg->value;
		FogStat.Color[2]=vid_fogb->value;
		FogStat.Color[3]=vid_foga->value;
		glFogfv(GL_FOG_COLOR, FogStat.Color);
	}
}

/*
** GL_SetDefaults
**
** Setting OpenGL default state
** /renderer is 2D based (I think it is much clever).../
*/
void GL_SetDefaults(void)
{
	glViewport(0, 0, ri.w, ri.h);
	glScissor(0, 0, ri.w, ri.h);

	/* set matrixes for 2D display */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, SIZE_2D_X, SIZE_2D_Y, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.375, 0.375, 0); // render all primitives at integer positions

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
}

/*
** GL_SwitchTo3DState
**
** Sets up renderer for 3D & sets fov and viewport position
*/
void GL_SwitchTo3DState(placeonplane_t viewport)
{
	/* Save State */
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT|GL_TRANSFORM_BIT);

	/* Enable/Disable */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	/* Viewport, Fov, fog, etc */
	//glViewport(0, 96, 640, 384); // and (float)(640.0/384.0) in gluPerspective & fov 60 == original look
	GL_CheckFOV();
	GL_CheckFog();

	/* matrixes */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(cur_y_fov, ratio, 0.2f, 64.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* options */
	glCullFace(GL_BACK);

	/* setup viewport position */
	glRotatef		((float)(90-FINE2DEG(viewport.angle)), 0, 1, 0);
	glTranslatef(-viewport.origin[0]/FLOATTILE, 0, viewport.origin[1]/FLOATTILE);
}

/*
Return renderer to 2D state...
*/
void GL_Restore2DState(void)
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();
}

// ------------------------- * Picture Loding & Caching * -------------------------
#define MAX_CACHED_PICS 128
cache_pic cachepics[MAX_CACHED_PICS];
int numcachepics=0;

#define MAX_SIZE maxTexSize

cache_pic *Vid_CachePic(int name)
{
	return Vid_CachePicEx(VGA_REMAP[name]);
}

cache_pic *Vid_CachePicEx(int name)
{
	cache_pic *pic;
	int i;
	wl_pic WlPic;
	int	scaled_width, scaled_height;
	static unsigned char scaled[512*512*3];
	float border[4]={1, 1, 1, 0};

	for(pic=cachepics, i=0; i<numcachepics; pic++, i++)
		if(name==pic->name) return pic;

	if(numcachepics==MAX_CACHED_PICS)
	{
		Con_Printf("Warning! numcachepics == MAX_CACHED_PICS\n");
		return NULL;
	}
	if(!File_VGA_ReadPic(name, &WlPic)) return NULL;

	numcachepics++;
	pic->name=name;
	pic->width =VID_NEW_X(WlPic.width);
	pic->height=VID_NEW_Y(WlPic.height);
	
	for(scaled_width =1; scaled_width <WlPic.width ; scaled_width <<=1);
	for(scaled_height=1; scaled_height<WlPic.height; scaled_height<<=1);

	if(scaled_width >MAX_SIZE) scaled_width =MAX_SIZE;
	if(scaled_height>MAX_SIZE) scaled_height=MAX_SIZE;

	gluScaleImage(GL_RGB, WlPic.width, WlPic.height, GL_UNSIGNED_BYTE, WlPic.data, scaled_width, scaled_height, GL_UNSIGNED_BYTE, scaled);
//ResampleTexture(WlPic.data, WlPic.width, WlPic.height, scaled,  scaled_width, scaled_height, 3);
	free(WlPic.data);

	glGenTextures(1, &pic->glTex);
	glBindTexture(GL_TEXTURE_2D, pic->glTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);//GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);//GL_REPEAT);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, scaled_width, scaled_height, 0, GL_RGB, GL_UNSIGNED_BYTE, scaled);
	return pic;
}

void Vid_UnCachePic(int name)
{
	cache_pic *pic;
	int i;

	for(pic=cachepics, i=0; i<numcachepics; pic++, i++)
		if(name==pic->name) break;

	if(name!=pic->name) return; // no pictures to uncache
	
	glDeleteTextures(1, &pic->glTex);
	memmove(pic, pic+1, (int)&cachepics[--numcachepics]-(int)pic);
}

void Vid_UnCacheAllPics(void)
{
	cache_pic *pic;
	int i;

	for(pic=cachepics, i=0; i<numcachepics; pic++, i++)
		glDeleteTextures(1, &pic->glTex);
	numcachepics=0;
}
