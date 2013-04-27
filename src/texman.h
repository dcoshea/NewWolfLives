// TexMan.h

/*
** texture info
*/
typedef struct texture_s
{
	int width, height;
	int bpp;
	unsigned char *data;	// image data in RGBA (if alpha) of RGB format
	int name;
	unsigned int glTex;		// for OpenGL
} texture_t;

int Vid_InitTexMan(void);
void Vid_ShutDownTexMan(void);

texture_t *Vid_FindCachedTex(int name);
texture_t *Vid_CacheTex(int name);
void Vid_UnCacheTex(int name);
void Vid_UnCacheAllTexs(void);

void Vid_PrecacheTextures(void);

void Vid_UploadTexture(texture_t *tex, bool mipmap, bool aniso);
void Vid_UnLoadTexture(texture_t *tex);

void Vid_SelectTextureIndirect(texture_t *tex);
void Vid_SelectTexture(int name);
void Vid_SelectTextureEx(int name);

#define TEX_SPRITE_BIT	4096
#define TEX_DOOR_BIT		2048

#define TEX_SPRITE_MASK	4095
#define TEX_DOOR_MASK		2047

// texture IDs used by cache routines
#define TEX_DDOOR	(0|TEX_DOOR_BIT)	// simple Door
#define TEX_PLATE	(2|TEX_DOOR_BIT)	// door Plate
#define TEX_DELEV	(4|TEX_DOOR_BIT)	// elevator Door
#define TEX_DLOCK	(6|TEX_DOOR_BIT)	// locked Door