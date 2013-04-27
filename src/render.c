/*
	Includes all
	Rendering of 2D & 3D screens
	Including HUD, 3D viewport & other
	/do not include menu, console & messages rendering/
*/
#include <windows.h>
#include <stdio.h>
#include <Mmsystem.h>
#include "WolfDef.h"

bool r_automap;

// ------------------------- * Devider * -------------------------

void PL_togglemap_f(void)
{
	r_automap=!r_automap;
}

void R_Init(void)
{
	r_automap=false;
	Cmd_AddCommand("togglemap", PL_togglemap_f);

// init all the rendering stuff
	GL_Init();
	Vid1_Init();
	Vid_InitTexMan();
}

// ------------------------- * Status Bar /HUD/ * -------------------------

void R_DrawFace(void)
{
	int face;

// update face picture FIXME: move to player
	if(!SD_SoundPlaying(GETGATLINGSND))
	{
		Player.facecount+=tics;
		if(Player.facecount>US_Rnd())
		{
			Player.face_gotgun=Player.face_ouch=false;
			Player.faceframe=US_Rnd()>>6;
			if(Player.faceframe==3) Player.faceframe=0;
			Player.facecount=0;
		}
	}

	if(Player.health)
	{
		if(Player.face_gotgun)
			face=GOTGATLINGPIC;
		else if(Player.face_ouch && IS_SPEAR)
			face=BJOUCHPIC;
		else if((Player.artifacts[ARTIF_INVUL] || Player.flags&PL_FLAG_GOD) && IS_SPEAR)
			face=GODMODEFACE1PIC+Player.faceframe;
		else
			face=FACE1APIC+3*((100-Player.health)>>4)+Player.faceframe;
	}
	else
	{
		if(Player.LastAttacker && Player.LastAttacker->type==en_needle)
			face=MUTANTBJPIC;
		else
		  face=FACE8APIC;
	}

	Vid_DrawPic(272, 394, face);
}

/*
** R_LatchNumber
**
** right justifies and pads with blanks
*/
void R_LatchNumber(int x, int y, int width, long number)
{
	char str[16], *c;
	int len;

	len=sprintf(str, "%*d", width, number);
	if(len>width)
		c=&str[len-width];
	else
		c=str;

	for(; *c; c++, x+=16)
	{
		if(*c!=' ')
			Vid_DrawPic(x, y, N_0PIC+(*c-'0'));
		else
			Vid_DrawPic(x, y, N_BLANKPIC);
	}
}

/*
** R_DrawHUD
**
** expected to be called every frame (except for: when in menu)
*/
void R_DrawHUD(void)
{
	Vid_DrawPic(0, 384, STATUSBARPIC);
	R_DrawFace();

	Vid_DrawPic(480, 394, Player.items&ITEM_KEY_1?GOLDKEYPIC:NOKEYPIC);
	Vid_DrawPic(480, 432, Player.items&ITEM_KEY_2?SILVERKEYPIC:NOKEYPIC);
	Vid_DrawPic(512, 403, KNIFEPIC+Player.weapon);
	R_LatchNumber( 32, 422, 2, gamestate.map);
	R_LatchNumber( 96, 422, 6, Player.score);
	R_LatchNumber(224, 422, 1, Player.lives);
	R_LatchNumber(336, 422, 3, Player.health);
	R_LatchNumber(432, 422, 2, Player.ammo[AMMO_BULLETS]); // FIXME!
}

// ------------------------- * Flashes: damage, bonus, etc * -------------------------
int r_bonusflash=0, r_damageflash=0;

void R_ResetFlash()
{
	r_bonusflash=r_damageflash=0;
}

void R_BonusFlash(void)
{
	r_bonusflash=64;	// white shift palette
}

void R_DamageFlash(int damage)
{
	r_damageflash+=damage;
	if(r_damageflash>256) r_damageflash=256;
}

void R_DrawFlash(void)
{
	if(r_bonusflash)
	{
// OpenGL only, use Palette for software rendering
		Vid_DrawBox(0, 0, 640, 480, 0xFF, 0xFF, 0xFF, (byte)(r_bonusflash<=255 ? r_bonusflash : 255));

		if((r_bonusflash-=16)<0) r_bonusflash=0;
	}

	if(r_damageflash)
	{
// OpenGL only, use Palette for software renrering
		Vid_DrawBox(0, 0, 640, 480, 0xFF, 0x00, 0x00, (byte)(r_damageflash<<3>200 ? 200 : r_damageflash<<3));

		if((r_damageflash-=4)<0) r_damageflash=0;
	}
}

// ------------------------- * Draw 3D world here (high level!) * -------------------------
int r_polys;

void R_Draw3D(void)
{
	placeonplane_t viewport;
	extern float cur_x_fov;

// initializing
	r_polys=0;
	viewport=Player.position;

//	R_DrawCelFloorHack();				// draw celing & floor [if a 2D hack is used]

	GL_SwitchTo3DState(viewport);	// switch renderer to 3D
	R_RayCast(viewport);					// raycaster (draws walls)
	R_DrawSprites();							// draw sprites
	R_DrawParticles();
	GL_Restore2DState();					// restore 2D back
}

// ------------------------- * Draw 2D world here (high level!) * -------------------------
void R_Draw2D(void)
{
	if(M_Draw3D())
	{ // Draw 2D overlay
		extern void DrawDebug(void);

		R_DrawGun();
		R_DrawHUD();

		if(r_automap) AM_DrawAutomap();
		if(developer->value) DrawDebug();
	}

	Con_Draw();	// draw console always (and before menu)
	if(key_dest==key_menu) M_Draw(); // Draw menu
	R_DrawFlash();			// bouns/damage flash
/*
	Vid_DrawPicture(304-(bonuscount>>2), 224-(bonuscount>>2),
									336+(bonuscount>>2), 256+(bonuscount>>2),
									TEX_VIDEO, 0, 0, 1, 1, 1); // crosshair
*/	
}

// ------------------------- * Screen Refresh (new frame) * -------------------------
#define FPS_UPDATE 500 // clock ticks (1000=1second)
int r_fps=0;

/*
** Frame rate counter
*/
void R_FrameCounter(void)
{
	static unsigned long fps_timer=0, fps_timer_old=0;
	static int frames=0;

	if(!fps_timer_old) fps_timer_old=timeGetTime();

	frames++;
  fps_timer=timeGetTime(); //clock();
  if(fps_timer-fps_timer_old>=FPS_UPDATE)  // Time to calculate fps and reset frame counter
	{
    r_fps=1000*frames/(fps_timer-fps_timer_old);
    frames=0;
    fps_timer_old=fps_timer;
  }
}

/* R_UpdateScreen
**
** Draws all the stuff
**  caled every frame
*/
void R_UpdateScreen(void)
{
// 	if(something) return; FIXME!

	R_FrameCounter();	// update frame rate

	if(M_Draw3D()) R_Draw3D();	// Draw 3D world
	R_Draw2D();		// 2D Overlay

	ri.R_Swap();	// swap buffers
}
