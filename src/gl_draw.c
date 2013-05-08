// OpenGL things drawing code!
#include "WolfDef.h"
#include "win32/win32.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include "r_models.h"
#include "scripts/scripts.h"

// ------- * Video Routines Should be in all implementations! * ----------

/*
** Vid_DrawConBack
**
** Draws translucent console background
*/
void Vid_DrawConBack(int height, byte alpha)
{
	extern texture_t tex_conback;

	glColor4ub(0xFF, 0xFF, 0xFF, alpha);
	ri.R_DrawPicScale(0, height-HEIGHT_2D, WIDTH_2D, HEIGHT_2D, (image_t*)&tex_conback);
}

// Draws Solid Filled box
void Vid_DrawBox(int x, int y, int w, int h, RGBA_PARAM(1))
{
	ri.R_DrawBox(x, y, w, h, rgba(r1,g1,b1,a1));
}

// Draws Wolf 3D original menu background
void Vid_WMenuBack(void)
{
	Vid_DrawBox(0, 0, XRES, YRES, RGBA_BORDC);
}

// Draws Spear menu background
void Vid_SMenuBack(void)
{
	Vid_DrawPic(0, 0, C_BACKDROPPIC);
}

// Draws Wolf 3D menu Stripes!
void Vid_WDrawStripes(void)
{
	Vid_DrawBox(0, 24, XRES, 58, RGBA_BLACK);
	Vid_DrawBox(0, 77, XRES,  2, RGBA_STRPC);
}

// Draws Spear menu Stripes!
void Vid_SDrawStripes(void)
{
	Vid_DrawBox(0, 24, XRES, 53, RGBA_BLACK);
	Vid_DrawBox(0, 79, XRES,  2, RGBA_BLACK);
}

void Vid_MenuBack(void)
{
	if(IS_SPEAR)
		Vid_SMenuBack();
	else
		Vid_WMenuBack();
}

void Vid_DrawStripes(void)
{
	if(IS_SPEAR)
		Vid_SDrawStripes();
	else
		Vid_WDrawStripes();
}

// draws window with outline; Dimensions of client area are: x+2, y+2, x+w-4, y+h-4
void Vid_DrawWindow(int x, int y, int w, int h, RGBA_PARAM(1), RGBA_PARAM(2), RGBA_PARAM(3))
{
//rgba1=window color; rgba2=up,left; rgba3=right, bottom
	Vid_DrawBox(x+2, y+2, w-4, h-4, RGBA_USEPARAM(1)); // inner window
	Vid_DrawBox(x, y, w, 2, RGBA_USEPARAM(2)); // upper border
	Vid_DrawBox(x, y, 2, h, RGBA_USEPARAM(2)); // left  border
	Vid_DrawBox(x, y+h, w, -2, RGBA_USEPARAM(3)); // lower border
	Vid_DrawBox(x+w, y, -2, h, RGBA_USEPARAM(3)); // right border
}

void Vid_DrawMenuBox(int x, int y, int w, int h)
{
	if(IS_SPEAR)
		Vid_DrawWindow(x, y, w, h, RGBA_SBKGC, RGBA_SDEAC, RGBA_SBRD2);
	else
		Vid_DrawWindow(x, y, w, h, RGBA_WBKGC, RGBA_DEACT, RGBA_BRD2C);
}

// draws a text message, framed in a window
void Vid_Message(char *msg)
{
	int x, y, h, w;
	unsigned long tmp;

	FNT_SetFont(FNT_WOLF_BIG);
	FNT_SetStyle(0, 0, 0);
	FNT_SetColor(RGBA_BLACK);
	FNT_SetScale(1, 1);

	tmp=FNT_GetSize(msg);
	w=unpackshort1(tmp);
	h=unpackshort2(tmp);

	x=(XRES-w)/2;
	y=(YRES-h)/2;

	Vid_DrawWindow(x-10, y-12, w+20, h+24, RGBA_TEXTC, RGBA_HIGHL, RGBA_BLACK);
	FNT_Print(x, y, msg);
}

/*
** Vid_DrawPic
**
** Draws Cached Picture,
** if not cached it will be cached :)
** FIXME: add reference by name as string.
*/
void Vid_DrawPic(int x, int y, int name)
{
	Vid_DrawPicIndirect(x, y, Vid_CachePic(name));
}

// draws indirect!
void Vid_DrawPicEx(int x, int y, int name)
{
	cache_pic *pic;

	pic=Vid_CachePicEx(name);
	Vid_DrawPicIndirect(x, y, pic);
}

// caller is responsible for pointer being correct only NULL check is done!
void Vid_DrawPicIndirect(int x, int y, cache_pic *pic)
{
	if(pic==NULL) return;

	glColor3ub(0xFF, 0xFF, 0xFF);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pic->glTex);

	glBegin(GL_QUADS);
		glTexCoord2f(1, 0); glVertex2i(x+pic->width, y);
		glTexCoord2f(0,	0); glVertex2i(x,						 y);
		glTexCoord2f(0,	1); glVertex2i(x,						 y+pic->height);
		glTexCoord2f(1, 1); glVertex2i(x+pic->width, y+pic->height);
	glEnd();
}

// ------------------------- * Drawing 3D world (OpenGL) * -------------------------

/*   Draw an wall
**
**          north (y)
**          |--------|
** west (x) |        | east (x)
**          |--------|
**           south (y)
*/
void R_Draw_Wall(float x, float y, float z1, float z2, dir4type type, int tex)
{
	float x1, x2, y1, y2;

	r_polys++;

	switch(type)
	{
// X wall
	case dir4_east:
		x1=x2=x+1;
		y1=-1-y; y2=-y;
		break;
	case dir4_west:
		x1=x2=x;
		y1=-y; y2=-1-y;
		break;
// Y wall
	case dir4_north:
		y1=y2=-y-1;
		x1=x; x2=x+1;
		break;
	case dir4_south:
		y1=y2=-y;
		x1=x+1; x2=x;
		break;
	}

	Vid_SelectTexture(tex);
	glColor4ub(RGBA_WHITE);

	glBegin(GL_QUADS);
		glTexCoord2f(1.0, 0.0); glVertex3f(x1, z2, y1);
		glTexCoord2f(0.0, 0.0); glVertex3f(x2, z2, y2);
		glTexCoord2f(0.0, 1.0); glVertex3f(x2, z1, y2);
		glTexCoord2f(1.0, 1.0); glVertex3f(x1, z1, y1);
	glEnd();
}

/*
** Draw door
*/
void R_Draw_Door(int x, int y, float z1, float z2, bool vertical, bool backside, int tex, int amount)
{
	float x1, x2, y1, y2, amt;

	if(amount==DOOR_FULLOPEN) return;
	amt=(float)amount/DOOR_FULLOPEN;

	r_polys++;

	if(vertical)
	{
		x1=x2=(float)x+0.5f;
		y1=-((float)y-amt);
		y2=-((float)y-amt); // -1
		if(backside)
			y1-=1;
		else
			y2-=1;
	}
	else
	{
		y1=y2=-(float)y-0.5f;
		x1=(float)x+amt; // +1
		x2=(float)x+amt;
		if(backside)
			x2+=1;
		else
			x1+=1;
	}

	Vid_SelectTexture(tex);
	glColor4ub(RGBA_WHITE);

	glBegin(GL_QUADS);
		glTexCoord2f(backside?0.0f:1.0f, 0.0); glVertex3f(x1, z2, y1);
		glTexCoord2f(backside?1.0f:0.0f, 0.0); glVertex3f(x2, z2, y2);
		glTexCoord2f(backside?1.0f:0.0f, 1.0); glVertex3f(x2, z1, y2);
		glTexCoord2f(backside?0.0f:1.0f, 1.0); glVertex3f(x1, z1, y1);
	glEnd();
}

/*
** Draw flat textured surface (floor or celing)
*/
void R_Draw_Flat(int x, int y, float z, bool floor, int tex)
{
	r_polys++;

	Vid_SelectTexture(tex);
	glColor4ub(RGBA_WHITE);

	glBegin(GL_QUADS);
		if(floor)
		{
			glTexCoord2f(0.0, 1.0); glVertex3f((float)x,		 z, (float)(-y));
			glTexCoord2f(1.0, 1.0); glVertex3f((float)(x+1), z, (float)(-y));
			glTexCoord2f(1.0, 0.0); glVertex3f((float)(x+1), z, (float)(-y-1));
			glTexCoord2f(0.0, 0.0); glVertex3f((float)x,		 z, (float)(-y-1));
		}
		else
		{
			glTexCoord2f(0.0, 1.0); glVertex3f((float)x,		 z, (float)(-y));
			glTexCoord2f(0.0, 0.0); glVertex3f((float)x,		 z, (float)(-y-1));
			glTexCoord2f(1.0, 0.0); glVertex3f((float)(x+1), z, (float)(-y-1));
			glTexCoord2f(1.0, 1.0); glVertex3f((float)(x+1), z, (float)(-y));
		}
	glEnd();
}

/*
** Draw flat solid surface (floor or celing)
*/
void R_Draw_FlatSolid(int x, int y, float z, bool floor, RGBcolor *color)
{
	r_polys++;

	glColor3ubv((char *)color);
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
		if(floor)
		{

			glVertex3f((float)x,		 z, (float)(-y));
			glVertex3f((float)(x+1), z, (float)(-y));
			glVertex3f((float)(x+1), z, (float)(-y-1));
			glVertex3f((float)x,		 z, (float)(-y-1));
		}
		else
		{
			glVertex3f((float)x,		 z, (float)(-y));
			glVertex3f((float)x,		 z, (float)(-y-1));
			glVertex3f((float)(x+1), z, (float)(-y-1));
			glVertex3f((float)(x+1), z, (float)(-y));
		}
	glEnd();
	glColor4ub(RGBA_WHITE); // FIXME
}

// ------------------------- * models * -------------------------
extern cvar_t *mdl_x, *mdl_y, *mdl_z, *mdl_s;

/*
** R_DrawModel
**
** Draws a model replacement if one exists
*/
bool R_DrawModel(int sprite, int x, int y, int angle)
{
	mdl_static_t *mdl;
	float base, ang;

	if(sprite>=mdl_nstatics) return false;

	mdl=&mdl_statics[sprite];
	if(!*mdl->filename) return false;
	if(!mdl->model) // model not loaded
	{
		mdl->model=malloc(sizeof(model_t));
		if(!mdl->model || !MD2_Load(mdl->filename, mdl->skin, mdl->model))
		{
			*mdl->filename=0; // unable to load
			return false;
		}
	}

// rotation
	if(mdl->flags&MDL_FACE_PLAYER)
		ang=90+FINE2DEGf(Player.position.angle);
	else if(mdl->flags&MDL_ROTATE)
		ang=ftime*180;
	else
		ang=FINE2DEGf(angle);

// alligning
	if(mdl->flags&MDL_ALLIGN_FLOOR)
		base=LOWERZCOORD;
	else if(mdl->flags&MDL_ALLIGN_CEIL)
		base=UPPERZCOORD;
	else
		base=0;

// bobbing
	if(mdl->flags&MDL_BOBBING)
		base+=0.1f*(float)sin(ftime*M_PI);


// OpenGL
	glPushMatrix();
	glPushAttrib(GL_ENABLE_BIT|GL_POLYGON_BIT);
	// setup
		glCullFace(GL_FRONT);
		glRotatef(-90, 1, 0, 0);	// put Z going up
    glRotatef(90, 0, 0, 1);		// put Z going up
	// position
		glTranslatef(POS2TILEf(y), -POS2TILEf(x), mdl->z_coord+mdl_z->value);
		glRotatef(ang, 0, 0, 1);
		glScalef(mdl->scale+mdl_s->value, mdl->scale+mdl_s->value, mdl->scale+mdl_s->value);

	// rendering
		if(mdl->animlen) // animation sequence
		{
			int frame1, frame2;
			float t;

			t=ftime*mdl->fps;
			frame1=(int)t;
			t-=(float)frame1;

			frame1%=mdl->animlen;
			frame2=frame1+1;
			if(frame2==mdl->animlen) frame2=0;

			frame1+=mdl->frame; frame2+=mdl->frame;
			MD2_RenderLerp(mdl->model, frame1, frame2, t, mdl->flags&3, base/(mdl->scale+mdl_s->value));
		}
		else // single frame
			MD2_Render(mdl->model, mdl->frame, mdl->flags&3, base/(mdl->scale+mdl_s->value));
	glPopAttrib();
	glPopMatrix();
	
	return true;
}

// ------------------------- * Devider * -------------------------

/*
** Draws all visible sprites
*/
void R_DrawSprites(void)
{
	float sina, cosa;
	float Ex, Ey, Dx, Dy;
	int n_sprt, n, ang;

// build visible sprites list
	n_sprt=Spr_CreateVisList();
	if(!n_sprt) return; // nothing to draw

// prepare values for bilboarding
	ang=NormalizeAngle(Player.position.angle+ANG_90); // FIXME: take viewport
	sina=(float)(0.5*SinTable[ang]);
	cosa=(float)(0.5*CosTable[ang]);

/* You can enable this & disable sorting, but...
	glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
*/
	glColor4ub(RGBA_WHITE);
	for(n=0; n<n_sprt; n++)
	{
		if(vislist[n].dist<MINDIST/2) continue; // little hack to save speed & z-buffer

		if(R_DrawModel(vislist[n].tex, vislist[n].x, vislist[n].y, vislist[n].ang))
			continue; // draw a model instead

		r_polys++;
		Vid_SelectTextureEx(vislist[n].tex|TEX_SPRITE_BIT);
		glBegin(GL_QUADS);
			Ex=Dx=vislist[n].x/FLOATTILE;
			Ey=Dy=vislist[n].y/FLOATTILE;
			Ex+=cosa; Ey+=sina;
			Dx-=cosa; Dy-=sina;

			glTexCoord2f(0.0, 0.0);	glVertex3f(Ex, UPPERZCOORD, -Ey);
			glTexCoord2f(0.0, 1.0);	glVertex3f(Ex, LOWERZCOORD, -Ey);
			glTexCoord2f(1.0, 1.0);	glVertex3f(Dx, LOWERZCOORD, -Dy);
			glTexCoord2f(1.0, 0.0);	glVertex3f(Dx, UPPERZCOORD, -Dy);
		glEnd();
	}
/*
	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
*/
}

#define PART_SIZE 0.025f
/*
** R_DrawParticles
*/
void R_DrawParticles(void)
{
	float sina, cosa;
	float Ex, Ey, Dx, Dy;
	int ang, type;
	particle_t *p;
	extern texture_t tex_blood, tex_brick;

	if(!active_particles) return; // no active particles

// prepare values for bilboarding
	ang=NormalizeAngle(Player.position.angle+ANG_90); // FIXME: take viewport
	sina=(float)(PART_SIZE*SinTable[ang]);
	cosa=(float)(PART_SIZE*CosTable[ang]);

	glDepthMask(GL_FALSE); // no z buffering
	for(type=0; type<3; type++)
	{
		switch(type)
		{
		case PART_BARE:
			glDisable(GL_TEXTURE_2D);
			break;
		case PART_BLOOD:
			Vid_SelectTextureIndirect(&tex_blood);
			break;
		case PART_BRICK:
			Vid_SelectTextureIndirect(&tex_brick);
			break;
		}
		glBegin(GL_QUADS);
		for(p=active_particles; p; p=p->next)
		{
			if(p->type!=type) continue;
			glColor4ubv((byte *)(&p->color));
			Ex=Dx=p->org[0];
			Ey=Dy=p->org[1];
			Ex+=cosa; Ey+=sina;
			Dx-=cosa; Dy-=sina;

			glTexCoord2f(0.0, 0.0); glVertex3f(Ex, p->org[2]+PART_SIZE, -Ey);
			glTexCoord2f(0.0, 1.0); glVertex3f(Ex, p->org[2]-PART_SIZE, -Ey);
			glTexCoord2f(1.0, 1.0); glVertex3f(Dx, p->org[2]-PART_SIZE, -Dy);
			glTexCoord2f(1.0, 0.0); glVertex3f(Dx, p->org[2]+PART_SIZE, -Dy);
		}
		glEnd();
	}
	glDepthMask(GL_TRUE); // back to normal Z buffering
}

/*
** Hack version of R_DrawCelFloor()
** Draws Celing & Floor
**
** Small Hack to make faster! By the way. Can implement DooM like sky
** OpenGL Clear Color must be set to floor color!
** must be called first, when in 2D mode
*/
void R_DrawCelFloorHack(void)
{
	glDisable(GL_TEXTURE_2D);
	glColor3ubv((unsigned char *)&CurMapData.celing);
	glBegin(GL_QUADS);
		glVertex2i(0, 0); glVertex2i(SIZE_2D_X, 0);
		glVertex2i(SIZE_2D_X, HORIZON); glVertex2i(0, HORIZON);
	glEnd();
	glColor4ub(RGBA_WHITE); // restore colors
}

// ------------------------- * HUD weapons * -------------------------

/*
** R_DrawModel
**
** Draws a model replacement if one exists
*/
bool R_DrawHUDModel(int weapon, int frame)
{
	mdl_weapon_t *mdl;

	if(weapon>=mdl_nweapons) return false;

	mdl=&mdl_weapons[weapon];
	if(!*mdl->filename) return false;
	if(!mdl->model) // model not loaded
	{
		mdl->model=malloc(sizeof(model_t));
		if(!mdl->model || !MD2_Load(mdl->filename, mdl->skin, mdl->model))
		{
			*mdl->filename=0; // unable to load
			return false;
		}
	}

	GL_SwitchTo3DState(Player.position);

// OpenGL
	glPushMatrix();
	glPushAttrib(GL_ENABLE_BIT|GL_POLYGON_BIT);
		glLoadIdentity();
		glCullFace(GL_FRONT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glRotatef(-90, 1, 0, 0); // z axis points up
		glRotatef(90, 0, 0, 1); // x axis points forward

		glTranslatef(0.25f, -0.1f, 0.015f);
		glScalef(0.01f, 0.01f, 0.01f); // scale

		MD2_Render(mdl->model, mdl->frames[frame], 0, 0);

	glPopAttrib();
	glPopMatrix();

	GL_Restore2DState();
	
	return true;
}

#include "sprt_def.h" // FIXME!

/*
** R_DrawGun
**
** Draw HUD weapons
*/
void R_DrawGun(void)
{
	if(R_DrawHUDModel(Player.weapon, Player.weaponframe)) return;
	glColor3ub(255, 255, 255);
	Vid_SelectTextureEx((SPR_KNIFEREADY+Player.weapon*5+Player.weaponframe)|TEX_SPRITE_BIT);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);	glVertex2i(480, 0);
		glTexCoord2f(0.0f, 0.0f);	glVertex2i(160, 0);
		glTexCoord2f(0.0f, 1.0f);	glVertex2i(160, 384);
		glTexCoord2f(1.0f, 1.0f);	glVertex2i(480, 384);
	glEnd();
}

void R_DrawCrosshair(int bonusflash)
{
	// If crosshair_bonus_scale > 0, then whenever the player picks up a
	// bonus, the crosshair will briefly increase in size.
	float scale_factor = 1.0 + crosshair_bonus_scale->value*bonusflash/BONUS_FLASH_MAX;

	GLint outer_radius=(GLint)crosshair_outer_radius->value * scale_factor;
	GLint inner_radius=(GLint)crosshair_inner_radius->value * scale_factor;

	if(!crosshair_enabled->value)
	{
		return;
	}

	glDisable(GL_TEXTURE_2D);
	glLineWidth(crosshair_thickness->value);
	glColor4f(1.0, 1.0, 0.0, crosshair_alpha->value);
	glBegin(GL_LINES);

	glVertex2i(XRES/2, YRES/2-inner_radius);
	glVertex2i(XRES/2, YRES/2-outer_radius);

	glVertex2i(XRES/2+inner_radius, YRES/2);
	glVertex2i(XRES/2+outer_radius, YRES/2);

	glVertex2i(XRES/2, YRES/2+inner_radius);
	glVertex2i(XRES/2, YRES/2+outer_radius);

	glVertex2i(XRES/2-inner_radius, YRES/2);
	glVertex2i(XRES/2-outer_radius, YRES/2);

	glEnd();
}