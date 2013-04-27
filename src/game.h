typedef enum difficulty_e
{
	gd_baby,
	gd_easy,
	gd_medium,
	gd_hard
} difficulty_t;

typedef struct game_s
{
	byte episode, map;													// current map & episode
	long TimeCount;															// time since level start in ms
	int	secretcount, treasurecount, killcount;	// this level data found by player
	int secrettotal, treasuretotal, killtotal;	// this level 100%
	difficulty_t difficulty;										// game difficulty
	bool demorecord, demoplayback;							// demo playing, recording
	FILE *demofile;															// demo file
} game_t;

extern game_t gamestate;

bool DEMO_ReadCmd(usercmd_t *cmd);
void DEMO_WriteCmd(usercmd_t *cmd);
void DEMO_StartRecord(char *demoname, int levelnumber);
void DEMO_FinishRecord(void);
void DEMO_StartPlay(char *demoname);
void DEMO_FinishPlay(void);

void GM_Init(void);
void GM_Reset(void);
void GM_SaveGameState(FILE *fp);
void GM_LoadGameState(FILE *fp);
