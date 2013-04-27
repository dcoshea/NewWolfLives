#define SPRT_ONE_TEX  1
#define SPRT_NO_ROT   2
#define SPRT_CHG_POS	4
#define SPRT_CHG_TEX	8
#define SPRT_REMOVE		16

typedef struct sprite_s
{
	int x, y, ang;
// very clever to make it not just (x>>TILESHIFT)
// but also (x>>TILESHIFT)-1 if (x%TILEWIDTH)<HALFTILE
// so we will check only 4 files instead of 9 as Carmack did!
	int tilex, tiley;
// controls appearence of this sprite:
// SPRT_ONE_TEX: use one texture for each rotation
// SPRT_NO_ROT: do not rotate sprite (fence)
// SPRT_CHG_POS
// SPRT_CHG_TEX
// SPRT_REMOVE
	int flags;
// 8 textures: one for each rotation phase!
// if SPRT_ONE_TEX flag use tex with index 0!
	int tex[8];
} sprite_t;

typedef struct visobj_s
{
	int	viewx, viewheight; // obsolete, but will do in software rendering!
	int dist; // how we will sort them
	int x, y; // position in game space! (for OpenGL)
	int ang;	// for md2 rotation
	int tex;  // texture to draw (renderer must know what to draw by this number!
} visobj_t;

extern visobj_t vislist[];

int Spr_Init(void);

void Spr_ResetSprites(void);
void Spr_SaveSprites(FILE *fp);
void Spr_LoadSprites(FILE *fp);

void Spr_RemoveSprite(int spr_id);
int Spr_GetNewSprite(void);
void Spr_SetPos(int spr_id, int x, int y, int angle);
void Spr_SetTex(int spr_id, int index, int tex);
int Spr_CreateVisList(void);
