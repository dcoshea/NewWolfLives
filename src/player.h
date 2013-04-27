// ------------------------- * constants * -------------------------

#define ITEM_KEY_1 1
#define ITEM_KEY_2 2
#define ITEM_KEY_3 4
#define ITEM_KEY_4 8
#define ITEM_WEAPON_1 16
#define ITEM_WEAPON_2 32
#define ITEM_WEAPON_3 64
#define ITEM_WEAPON_4 128
#define ITEM_WEAPON_5 256
#define ITEM_WEAPON_6 512
#define ITEM_WEAPON_7 1024
#define ITEM_WEAPON_8 2048
#define ITEM_BACKPACK (1<<12) // doubles carrying capacity
#define ITEM_AUGMENT	(1<<13) // adds 50 to maximum health
#define ITEM_UNIFORM  (1<<14) // allows you to pass guards
#define ITEM_AUTOMAP  (1<<15)	// shows unknown map ares in other color (as in DooM)
#define ITEM_FREE			(1<<16)	// - unused -

#define ARMOR_2_FLAG	(1<<16)
#define ARMOR_MASK		0x7FFF

enum weapon_e
{
	WEAPON_KNIFE,
	WEAPON_PISTOL,
	WEAPON_AUTO,
	WEAPON_CHAIN,

	WEAPON_TYPES
};

enum key_e
{
	KEY_GOLD,
	KEY_SILVER,
	KEY_FREE1,
	KEY_FREE2,

	KEY_TYPES
};

enum ammo_e
{
	AMMO_BULLETS,

	AMMO_TYPES
};

enum artif_e
{
	ARTIF_INVUL,
	ARTIF_INVIS,
	ARTIF_VISION,

	ARTIF_TYPES
};

#define PL_FLAG_REUSE		1 // use button pressed
#define PL_FLAG_ATTCK		2 // attacking
// debug (cheat codes) flags
#define PL_FLAG_GOD			(1<<4)
#define PL_FLAG_CLIP		(1<<5)
#define PL_FLAG_TARGET	(1<<6)

// ------------------------- * old and not so good * -------------------------

typedef	enum state_e
{
	ex_notingame,
	ex_playing,
	ex_dead,
	ex_secretlevel,
	ex_victory,
	ex_complete
/*
	ex_stillplaying,
	ex_completed,
	ex_died,
	ex_warped,
	ex_resetgame,
	ex_loadedgame,
	ex_victorious,
	ex_abort,
	ex_demodone,
	ex_secretlevel
*/
} state_t;

// ------------------------- * types * -------------------------

// Player structure: Holds all info about player
typedef struct player_s
{
	placeonplane_t position; // player position
	int movx, movy, speed;
	int tilex, tiley;

// stats
	int health, lives, frags;
	int armor;	// there are 2 types. The better one is indicated by high bit set
	int ammo[AMMO_TYPES];
	int artifacts[ARTIF_TYPES];
	int old_score, score, next_extra;
	unsigned items; // (keys, weapon)
	int weapon, pendingweapon;
// additional info
	int attackframe, attackcount, weaponframe; // attack info
	unsigned flags;
	int areanumber;

	bool madenoise;	// FIXME: move to flags?
	Guard_struct *LastAttacker;
	int faceframe, facecount;	// bj's face in the HUD // FIXME decide something!
	bool face_gotgun, face_ouch;
	state_t playstate; // fixme: move to gamestate
} player_t;

// ------------------------- * Devider * -------------------------

extern player_t Player;

// ------------------------- * Devider * -------------------------
void PL_Spawn(placeonplane_t location);

void PL_Process(player_t *self, usercmd_t *cmd);

void PL_Damage(player_t *self, Guard_struct *attacker, int points);
bool PL_GiveHealth(player_t *self, int points, int max);
bool PL_GiveAmmo(player_t *self, int type, int ammo);
void PL_GiveWeapon(player_t *self, int weapon);
void PL_GiveLife(player_t *self);
bool PL_GiveArmor(player_t *self, bool mega, int points);
void PL_GiveArmorBonus(player_t *self, int points, int max);
void PL_GivePoints(player_t *self, long points);
void PL_GiveKey(player_t *self, int key);

void PL_NewGame(player_t *self);
void PL_NextLevel(player_t *self);
bool PL_Reborn(player_t *self);

void PL_Init(void);
void PL_Reset(void);
void PL_SavePlayer(FILE *fp);
void PL_LoadPlayer(FILE *fp);
