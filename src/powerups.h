typedef enum
{
//please provide description
//FIXME: script
	pow_gibs,				// 	 1% if <=10%; SLURPIESND
	pow_gibs2,			// 	 1% if <=10%; SLURPIESND
	pow_alpo,				//   4% if <100%; HEALTH1SND
	pow_firstaid,		//  25% if <100%; HEALTH2SND
	pow_key1,				// gold key;		GETKEYSND
	pow_key2,				// silver key;	GETKEYSND
	pow_key3,				// not used
	pow_key4,				// not used
	pow_cross,				//  100pts; BONUS1SND
	pow_chalice,			//  500pts; BONUS2SND
	pow_bible,				// 1000pts; BONUS3SND
	pow_crown,				// 5000pts; BONUS4SND
	pow_clip,				// 8bul if <99bul; GETAMMOSND
	pow_clip2,				// 4bul if <99bul; GETAMMOSND
	pow_machinegun,  // machine gun; GETMACHINESND
	pow_chaingun,		// gatling gun; GETGATLINGSND
	pow_food,				//  10% if <100%; HEALTH1SND
	pow_fullheal,		// 99%, 25bul; BONUS1UPSND
	pow_25clip,			// 25bul if <99bul; GETAMMOBOXSND
	pow_spear,			// spear of destiny!

	pow_last
// add new types <!only!> here (after last)
} pow_t;
	
int Pow_Init(void);
void Pow_Reset(void);
void Pow_Save(FILE *fp);
void Pow_Load(FILE *fp);
void Pow_Spawn(int x, int y, int type);
void Pow_PickUp(int x, int y);
