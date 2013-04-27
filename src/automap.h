typedef struct
{
	bool vis[64][64]; // player saw this tile?
} AutoMap_t;

extern AutoMap_t AM_AutoMap;

int AM_Init(void);
void AM_ResetAutomap(void);
void AM_SaveAutomap(FILE *fp);
void AM_LoadAutomap(FILE *fp);

void AM_DrawAutomap(bool transparent);
void AM_Hide_f(void);
void AM_Reveal_f(void);