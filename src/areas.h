#define NUMAREAS			37				// number of areas
#define FIRSTAREA			0x6B			// first area in map data (it is by the way a way to the secret floor!)
#define AMBUSHTILE		0x6A			// def guard
#define AMBUSH				-2

unsigned char areaconnect[NUMAREAS][NUMAREAS];
extern unsigned char areabyplayer[NUMAREAS];

extern void Ar_ResetAreas(void);
extern void Ar_SaveAreas(FILE *fp);
extern void Ar_LoadAreas(FILE *fp);

extern void Ar_ConnectAreas(int area);
extern void Ar_JoinAreas(int area1, int area2);
extern void Ar_DisjoinAreas(int area1, int area2);

void Ar_RecursiveConnect(int areanumber);
