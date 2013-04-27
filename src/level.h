#define TILE_IS_WALL			1
#define TILE_IS_PWALL			(1<<20)
#define TILE_IS_DOOR			2
#define TILE_IS_SECRET		4
#define TILE_IS_DRESS			8
#define TILE_IS_BLOCK			16
#define TILE_IS_ACTOR			32
#define TILE_IS_DEADACTOR 64
#define TILE_IS_POWERUP		128
#define TILE_IS_AMBUSH		256
#define TILE_IS_EXIT			512
#define TILE_IS_SLEVEL		1024
#define TILE_IS_ELEVATOR 	(1<<11)
#define TILE_IS_E_TURN		(1<<12)
#define TILE_IS_NE_TURN		(1<<13)
#define TILE_IS_N_TURN		(1<<14)
#define TILE_IS_NW_TURN		(1<<15)
#define TILE_IS_W_TURN		(1<<16)
#define TILE_IS_SW_TURN		(1<<17)
#define TILE_IS_S_TURN		(1<<18)
#define TILE_IS_SE_TURN		(1<<19)

#define TILE_IS_SOLIDTILE (TILE_IS_WALL | TILE_IS_BLOCK | TILE_IS_PWALL)
#define TILE_BLOCKS_MOVE	(TILE_IS_WALL | TILE_IS_BLOCK | TILE_IS_PWALL | TILE_IS_ACTOR)
#define TILE_IS_WAYPOINT	(TILE_IS_E_TURN|TILE_IS_NE_TURN|TILE_IS_N_TURN|TILE_IS_NW_TURN|TILE_IS_W_TURN|TILE_IS_SW_TURN|TILE_IS_S_TURN|TILE_IS_SE_TURN)

// Structure which holds ALL info about level, currently played!
// used by both client & server: shared resource!
// FIXME: add a checksumm to check versions between C-S
// or just upload from Server to Client! (cool, needs work)
typedef struct
{
	int mapid; // Map name (id used to load it, now just a map number, later: string id)
// this 3 are directly from data files (*.wl6 - original)
// they do not ment to be used by game engine
// all processing should be done in this module!
	unsigned short layer1[64*64]; // walls
	unsigned short layer2[64*64]; // objects
	unsigned short layer3[64*64]; // other

// this is an info of each level's tile: (contains a matrix of flags):
// TILE_IS_WALL			- if solid wall
// TILE_IS_DOOR			- if thre is a door
// TILE_IS_SECRET		- if it is a pushwall (not activated yet!)
// TILE_IS_DRESS		- if there is a dressing static
// TILE_IS_BLOCK		- if there is a blocking static
// TILE_IS_ACTOR		- if there is an actor in tile \
// TILE_IS_DEADACTOR- if there is an acror's body  |
// TILE_IS_POWERUP	- if there is a powerup        / this two are set by spawn sub!
// TILE_IS_WAYPOINT - if there is patrol waypoint
// TILE_IS_AMBUSH		- if a guard, spawned there is in ambush state
// TILE_IS_EXIT			- if it is an end of the game
// TILE_IS_SLEVEL		- if it is a floor which will lead to secret level
// TILE_IS_ELEVATOR - if there is an exit from level
	long tile_info[64][64];

// this is an array of references to texture descriptions
// the renderer must know what to draw by this number
	int wall_tex_x[64][64]; // x_wall
	int wall_tex_y[64][64]; // y_wall

// this is a (0-based) array of area numbers!
// must be all filled by level loading sub
// if -1 it is a wall, if -2 it is a door, if -3 it is unknown
	int areas[64][64];

	placeonplane_t pSpawn; // player spawn place
	char name[17];
// music name
	int music;
// celing & floor colors
	RGBcolor celing, floor;
} leveldef;

extern leveldef CurMapData;
extern int TotalLevels;

extern int Lvl_Init(void);

extern int Lvl_LoadLevel(int zbmn);
