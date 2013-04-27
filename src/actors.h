#define MAX_GUARDS	255
#define NUMENEMIES 31
#define NUMSTATES	 34

typedef enum
{
	en_guard,
	en_officer,
	en_ss,
	en_dog,
	en_boss,
	en_schabbs,
	en_fake,
	en_mecha,
	en_hitler,
	en_mutant,
	en_blinky,
	en_clyde,
	en_pinky,
	en_inky,
	en_gretel,
	en_gift,
	en_fat,
// --- Projectiles
	en_needle,
	en_fire,
	en_rocket,
	en_smoke,
	en_bj,
// --- Spear of destiny!
	en_spark,
	en_hrocket,
	en_hsmoke,

	en_spectre,
	en_angel,
	en_trans,
	en_uber,
	en_will,
	en_death
} enemy_t;

typedef enum
{
	st_stand,
	st_path1,	st_path1s,	st_path2,	st_path3,	st_path3s,	st_path4,
	st_pain, st_pain1,
	st_shoot1, st_shoot2, st_shoot3, st_shoot4, st_shoot5, st_shoot6, st_shoot7, st_shoot8, st_shoot9,
	st_chase1, st_chase1s, st_chase2, st_chase3, st_chase3s, st_chase4,
	st_die1, st_die2, st_die3, st_die4, st_die5, st_die6, st_die7, st_die8, st_die9,
	st_dead,
	st_remove
} en_state;

typedef struct
{
	int x, y, angle;
	int type;
	int health;
	int speed;
	int ticks;
	int temp2;
	int distance;
	char tilex, tiley;
	char areanumber;
	doorstruct *waitfordoor; // doorstruct
	unsigned char flags;
	en_state state;
	dir8type dir;
	int sprite;
} Guard_struct;

typedef void (*think_t) (Guard_struct *self);

typedef struct
{
	char rotate; // 1-if object can be rotated, 0 if one sprite for every direction
	int texture; // base object's state texture if rotation is on facing player
	int timeout; // after how man ticks change state to .next_state
	think_t think; // what to do every frame
	think_t action; // what to do once per state
	en_state next_state; // next state
} stateinfo;


extern int starthitpoints[4][NUMENEMIES]; // FIXME: remove it!

extern Guard_struct Guards[MAX_GUARDS+1];
extern Guard_struct *New;
extern unsigned char NumGuards;
extern stateinfo objstate[NUMENEMIES][NUMSTATES];

void Actors_Init(void);
void ResetGuards(void);
void SaveGuards(FILE *fp);
void LoadGuards(FILE *fp);

Guard_struct *GetNewActor(void);
void RemoveActor(Guard_struct *actor);
Guard_struct *SpawnActor(enemy_t which, int x, int y, dir4type dir);
void A_StateChange(Guard_struct *Guard, en_state NewState);
