#define RGBA_WHITE 0xFF, 0xFF, 0xFF, 0xFF
#define RGBA_BLACK 0x00, 0x00, 0x00, 0xFF
#define RGBA_RED   0xFF, 0x00, 0x00, 0xFF
#define RGBA_GREEN 0x00, 0xFF, 0x00, 0xFF
#define RGBA_BLUE  0x00, 0x00, 0xFF, 0xFF

#define RGBA_WLMNU 0x8C, 0x8C, 0x8C, 0xFF // menu item
#define RGBA_WLMSL 0xC0, 0xC0, 0xC0, 0xFF // selected menu item
#define RGBA_WLMDS 0x70, 0x00, 0x00, 0xFF // disabled menu item
#define RGBA_WLMCU 0xB4, 0xAC, 0x00, 0xFF // current (yellow menu item)
#define RGBA_WLMCS 0xFC, 0xF4, 0x00, 0xFF // current + selected

// Read This! colors
#define RGBA_PGNUM 0x40, 0x40, 0x00, 0xFF // page number in Read This!
#define RGBA_READB 0xDC, 0xDC, 0xDC, 0xFF // background in Read This!

// 16 base colors!
#define RGBA_C15 0xFC, 0xFC, 0xFC, 0xFF

#define RGBA_WBKGC 0x58, 0x00, 0x00, 0xFF
#define RGBA_BORDC 0x88, 0x00, 0x00, 0xFF
#define RGBA_BRD2C 0xD4, 0x00, 0x00, 0xFF
#define RGBA_DEACT 0x70, 0x00, 0x00, 0xFF
#define RGBA_STRPC 0x64, 0x00, 0x00, 0xFF

#define RGBA_SBKGC 0x00, 0x00, 0x58, 0xFF
#define RGBA_SBORD 0x00, 0x00, 0x88, 0xFF
#define RGBA_SBRD2 0x00, 0x00, 0xD4, 0xFF
#define RGBA_SDEAC 0x00, 0x00, 0x70, 0xFF
#define RGBA_SSTRP 0x00, 0x00, 0x64, 0xFF

#define RGBA_READC 0xB4, 0xAC, 0x00, 0xFF
#define RGBA_READH 0xFC, 0xF4, 0x00, 0xFF
#define RGBA_NOEPC 0x04, 0x70, 0x00, 0xFF
#define RGBA_NOEPH 0x04, 0xA4, 0x00, 0xFF
#define RGBA_VIEWC 0x00, 0x40, 0x40, 0xFF
#define RGBA_TEXTC 0x8C, 0x8C, 0x8C, 0xFF
#define RGBA_HIGHL 0xC0, 0xC0, 0xC0, 0xFF

#define RGBA_PG13C 0x20, 0xA8, 0xFC, 0xFF
#define RGBA_INTER 0x00, 0x40, 0x40, 0xFF
#define RGBA_VIEWC 0x00, 0x40, 0x40, 0xFF

// Wolf Palette
#define RGB_WOLFCOLOR(index)	WolfPal[index].r, WolfPal[index].g, WolfPal[index].b
#define RGBA_WOLFCOLOR(index) WolfPal[index].r, WolfPal[index].g, WolfPal[index].b, 0xFF

#define RGBA_PARAM(n) unsigned char r##n, unsigned char g##n, unsigned char b##n, unsigned char a##n
#define RGBA_USEPARAM(n) r##n, g##n, b##n, a##n

typedef struct
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} RGBcolor;
typedef struct
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} RGBAcolor;

int Vid1_Init(void);

void Vid_DrawConBack(int height, byte alpha);
void Vid_DrawBox(int x, int y, int w, int h, RGBA_PARAM(1));
void Vid_WMenuBack(void);
void Vid_SMenuBack(void);
void Vid_WDrawStripes(void);
void Vid_SDrawStripes(void);
void Vid_MenuBack(void);
void Vid_DrawStripes(void);
void Vid_DrawWindow(int x, int y, int w, int h, RGBA_PARAM(1), RGBA_PARAM(2), RGBA_PARAM(3));
void Vid_DrawMenuBox(int x, int y, int w, int h);
void Vid_Message(char *msg);
void Vid_DrawPic(int x, int y, int name);
void Vid_DrawPicEx(int x, int y, int name);
void Vid_DrawPicIndirect(int x, int y, cache_pic *pic);

void Vid_ScreenShot_f(void);

// ------------------------- * Drawing 3D world (OpenGL) * -------------------------
// Not very portable, but let it be!

void R_Draw_Door(int x, int y, float z1, float z2, bool vertical, bool backside, int tex, int amount);
void R_Draw_Wall(float x, float y, float z1, float z2, dir4type type, int tex);
void R_Draw_Flat(int x, int y, float z, bool floor, int tex);
void R_Draw_FlatSolid(int x, int y, float z, bool floor, RGBcolor *color);

void R_DrawSprites(void);
void R_DrawParticles(void);
void R_DrawCelFloorHack(void);
void R_DrawGun(void);

