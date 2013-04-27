#include "Audiowl6.h"

// sound channels
// channel 0 never willingly overrides
// other channels (1-7) allways override a playing sound on that channel
#define CHAN_AUTO			-1
#define CHAN_PWEAPON	 0
#define CHAN_WEAPON		 1
#define CHAN_VOICE		 2
#define CHAN_ITEM			 3
#define CHAN_BODY			 4
#define CHAN_LOCAL		 5
// number first avail channel for CHAN_AUTO mode
#define CHAN_FIRST		 6

typedef struct
{
	int length; // in bytes
	//char name[MAX_WLPATH];
	int name;
	byte *data;
} cache_snd;

typedef enum {sfx_none, sfx_pc, sfx_adlib} sfx_t;
typedef enum {digi_none, digi_sb, digi_ss} digi_t;
typedef enum {mus_none, mus_adlib} mus_t;
typedef struct
{
	bool avail[3][3];
	sfx_t sfx;
	digi_t digi;
	mus_t mus;
} snd_mode;
extern snd_mode SD_mode;

void SD_SetSoundLoc(int gx, int gy);
int SD_LoadIdSound(int n, int chn);
int SD_ChooseChannel(soundnames SoundNum, int channel);
cache_snd *SD_CacheSND(int name);
void SD_UnCacheSnd(int name);
void SD_UnCacheAllSnds(void);
void SD_InitDigiMap(void);

void SD_PlayImf_f(void);

extern void SD_StopMusic(void);
extern void SD_PlayMusic(int song, bool force_restart);
extern bool SD_SoundPlaying(soundnames SoundNum);
extern void SD_PlaySound(soundnames SoundNum, int channel);
extern void SD_PlaySound3D(soundnames SoundNum, int channel, int x, int y);
extern void SD_StopAllSound(void);
extern void SD_UpdateSounds3D(void);
extern int SD_Init(void);
extern void SD_Config(int option, int setting);
extern void SD_ShutDown(void);


#define	PlaySoundLocTile(s, chn, tx, ty)	SD_PlaySound3D(s, chn, TILE2POS(tx), TILE2POS(ty))
#define	PlaySoundLocActor(s, chn, ob)			SD_PlaySound3D(s, chn, (ob)->x, (ob)->y)
