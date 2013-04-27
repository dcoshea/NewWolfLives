#define MAX_DOORS 256
#define DOOR_TIMEOUT 300
#define DOOR_MINOPEN 50

#define DOOR_FULLOPEN 63

typedef enum
{
	dr_closing=-1,
	dr_closed,
	dr_opening,
	dr_open
} dr_state;

#define DOOR_VERT			255
#define DOOR_HORIZ		254
#define DOOR_E_VERT		253
#define DOOR_E_HORIZ	252

#define DOOR_G_VERT		251
#define DOOR_G_HORIZ	250
#define DOOR_S_VERT		249
#define DOOR_S_HORIZ	248

#define FIRST_DOOR		248
#define LAST_LOCK			251

typedef struct
{
	int tickcount;
// door state
//	dr_closing=-1,
//	dr_closed,
//	dr_opening,
//	dr_open
	dr_state action;
	int x, y;
	int area1, area2;
/*DOOR_VERT			255
	DOOR_HORIZ		254
	DOOR_E_VERT		253
	DOOR_E_HORIZ	252
	DOOR_G_VERT		251
	DOOR_G_HORIZ	250
	DOOR_S_VERT		249
	DOOR_S_HORIZ	248*/
	int type;
	bool vertical;
	int texture;
}  doorstruct;

extern doorstruct Doors[MAX_DOORS];
extern doorstruct *DoorMap[64][64];

void Door_ResetDoors(void);
void Door_SaveDoors(FILE *fp);
void Door_LoadDoors(FILE *fp);

int Door_SpawnDoor(int x, int y, int type);
void Door_SetAreas(void);
void Door_OpenDoor(doorstruct *Door);
void Door_ChangeDoorState(doorstruct *Door);
void Door_ProcessDoors_e(int t_tk, int t_ms);
int Door_Opened(int x, int y);
bool Door_TryUse(doorstruct *Door, int keys);
