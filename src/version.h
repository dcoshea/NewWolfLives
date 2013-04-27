// Wolfenstein version control system!
// FIXME: this is where all script processing will occur

// Wolf Versions:
/*
Info from Wolf3D FAQ

Wolf3D Versions:

v1.0 - First release (05/05/92)
v1.1 - Fixed some secret doors map bugs (on level 7 & 8 in episode 1 there
       were secret doors left out) and also some video problems (06/10/92)
v1.2 - Secret level elevator bug was fixed (introduced in 1.1 :( ) (06/22/92)
v1.3 - Was never released.
v1.4 - Fixed something but I forgot what. It also changed slightly the graphic
       backdrop on the opening screen and other menus, changed the interior
       elevator graphic and added a calibration routine in the joystick setup
       menu. (12/03/92)

Spear Versions:

v1.0 - First release (09/28/92)
v1.1 - No information available (yet? call on info)
v1.2 - "	"  "	" "	"
v1.3 - "	"  "	" "	"
v1.4 - Final release, changes from previous versions unknown
       (01/05/93)
*/
// in order of preference:

typedef enum wolf_set_e
{
	WSET_WL1,	// shareware version
	WSET_WL3,	// 3 episodes /never seen this one/
	WSET_WL6,	// full registered version
	WSET_SDM,	// spear demo
	WSET_SOD,	// separ of sestiny

	WSET_LAST
} wolf_set_t;

typedef enum wolf_ver_e
{
	WVER_10,
	WVER_11,
	WVER_12,
	WVER_13,
	WVER_14
} wolf_ver_t;

#define IS_SPEAR (WolfVer.set==WSET_SDM || WolfVer.set==WSET_SOD)
#define IS_SHAREWARE (WolfVer.set==WSET_WL1)

typedef struct ver_info_s
{
	wolf_set_t set; //WSET_{WL1|WL3|WL6|SDM|SOD}
	wolf_ver_t ver; //WVER_{10|11|12|13|14}
	char file_audh[MAX_OSPATH]; // audio header
	char file_audm[MAX_OSPATH]; // audio main
	char file_page[MAX_OSPATH]; // page file
	char file_vgah[MAX_OSPATH]; // vga header
	char file_vgad[MAX_OSPATH]; // vga dictionary
	char file_vgam[MAX_OSPATH]; // vga main
	char file_maph[MAX_OSPATH]; // map header
	char file_mapm[MAX_OSPATH]; // map main
	char ext[4]; // set's default extension
} ver_info_t;

typedef struct statinfo_t
{
	bool block;
	int powerup;
} statinfo_t;

extern ver_info_t WolfVer;
extern RGBcolor *WolfPal;

extern int *VGA_REMAP, *SPR_REMAP;
extern int *MUS_REMAP, *SND_REMAP;
extern int STARTPCSOUNDS, STARTADLIBSOUNDS, STARTDIGISOUNDS, STARTMUSIC;
extern int *wolfdigimap;
extern int *levelsongs;
extern statinfo_t *statinfo;
extern int num_statics;
extern int *celing_color;

int Ver_Init(void);
