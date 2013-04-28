// Wolfenstein Control Panel!  Ta Da!
#include <windows.h>
#include <stdio.h>
#include <GL\gl.h>
#include "WolfDef.h"

enum {m_none, m_main, m_newg, m_episode, m_skill, m_sound, m_controls, 
			m_load, m_save, m_video, m_scores, m_quit, m_intro, m_inter,
			m_txtpages, m_dead} m_state;

void M_Intro_f(void); // At very beginning!
void M_Demo_f(void);	// When chosen 'Back to demo!'
void M_Inter_f(void); // InterMission
void M_Dead_f(void);	// Death screen
void M_Menu_Main_f(void);
	void M_Menu_NewGame_f(void);
		void M_Menu_Episode_f(void);
			void M_Menu_Skill_f(void);
	void M_Menu_Sound_f(void);
	void M_Menu_Controls_f(void);
	void M_Menu_Load_f(void);
	void M_Menu_Save_f(void);
	void M_Menu_Video_f(void);
	void M_Menu_ReadThis_f(void); // Help Screens
	void M_Menu_Scores_f(void);
	void M_Menu_Quit_f(void);

void M_Dead_Draw(void);
void M_Inter_Draw(void);
void M_Intro_Draw(void);
void M_TxtPages_Draw(void);
void M_Main_Draw(void);
	void M_NewGame_Draw(void);
		void M_Episode_Draw(void);
			void M_Skill_Draw(void);
	void M_Sound_Draw(void);
	void M_Controls_Draw(void);
	void M_Load_Draw(void);
	void M_Save_Draw(void);
	void M_Video_Draw(void);
	void M_Scores_Draw(void);
	void M_Quit_Draw(void);

void M_Dead_Key(int key);
void M_Inter_Key(int key);
void M_Intro_Key(int key);
void M_TxtPages_Key(int key);
void M_Main_Key(int key);
	void M_NewGame_Key(int key);
		void M_Episode_Key(int key);
			void M_Skill_Key(int key);
	void M_Sound_Key(int key);
	void M_Controls_Key(int key);
	void M_Load_Key(int key);
	void M_Save_Key(int key);
	void M_Video_Key(int key);
	void M_Scores_Key(int key);
	void M_Quit_Key(int key);

bool wasInMenus; // whether user came form game via a hotkey or from menu
bool elev_snd;
int	m_prevstate; // previouse menu

//=============================================================================
/*
================
M_ToggleMenu_f
================
*/
void M_ToggleMenu_f(void)
{
	if(key_dest==key_menu)
	{
		if(m_state!=m_main)
		{
			M_Menu_Main_f();
			return;
		}
		key_dest=key_game;
		m_state =m_none;
		return;
	}
	if(key_dest==key_console)
		Con_ToggleConsole_f();
	else
		M_Menu_Main_f();
}

/*
** M_Draw3D
**
** since menu is the only thing that can prevent
** 3D world from being drawn here we ask is it should be done
*/
bool M_Draw3D(void)  // FIXME (now it's just a hack)
{
	if(m_state==m_quit && (!wasInMenus || m_prevstate==m_dead)) return true;
	if(m_state==m_dead) return true;
	if(m_state==m_inter && elev_snd) return true;
	return key_dest!=key_menu;
}

void M_DrawCursor(int x, int y)
{
	static int mode=0, tchg=70; //70&8

	tchg-=tics;
	if(tchg<=0)
	{
		mode=1-mode;
		if(mode==1)
			tchg=8;
		else
			tchg=70;
	}
	Vid_DrawPic(x, y, mode ? C_CURSOR2PIC : C_CURSOR1PIC);
}

//=============================================================================
/* InterMission MENU (SOME SORT OF) */
typedef struct
{
	float time;
	char timestr[6];
} times;
times parTimes[]=
{
	{1.5,	"01:30"},	{2,		"02:00"},	{2,		"02:00"},	{3.5,	"03:30"},	{3,		"03:00"},	{3,		"03:00"},	{2.5,	"02:30"},	{2.5,	"02:30"},	{0,		"??:??"},	{0,		"??:??"},
	{1.5,	"01:30"},	{3.5,	"03:30"},	{3,		"03:00"},	{2,		"02:00"},	{4,		"04:00"},	{6,		"06:00"},	{1,		"01:00"},	{3,		"03:00"},	{0,		"??:??"},	{0,		"??:??"},
	{1.5,	"01:30"},	{1.5,	"01:30"},	{2.5,	"02:30"},	{2.5,	"02:30"},	{3.5,	"03:30"},	{2.5,	"02:30"},	{2,		"02:00"},	{6,		"06:00"},	{0,		"??:??"},	{0,		"??:??"},
	{2,		"02:00"},	{2,		"02:00"},	{1.5,	"01:30"},	{1,		"01:00"},	{4.5,	"04:30"},	{3.5,	"03:30"},	{2,		"02:00"},	{4.5,	"04:30"},	{0,		"??:??"},	{0,		"??:??"},
	{2.5,	"02:30"},	{1.5,	"01:30"},	{2.5,	"02:30"},	{2.5,	"02:30"},	{4,		"04:00"},	{3,		"03:00"},	{4.5,	"04:30"},	{3.5,	"03:30"},	{0,		"??:??"},	{0,		"??:??"},
	{6.5,	"06:30"},	{4,		"04:00"},	{4.5,	"04:30"},	{6,		"06:00"},	{5,		"05:00"},	{5.5,	"05:30"},	{5.5,	"05:30"},	{8.5,	"08:30"},	{0,		"??:??"},	{0,		"??:??"}
},
parTimesSpear[]=
{
	{1.5,	"01:30"},	{3.5,	"03:30"},	{2.75,	"02:45"}, {3.5,	"03:30"}, {0,	"??:??"},	// Boss 1
	{4.5,	"04:30"}, {3.25,	"03:15"}, {2.75,	"02:45"}, {4.75,	"04:45"}, {0,	"??:??"},	// Boss 2
	{6.5,	"06:30"}, {4.5,	"04:30"}, {2.75,	"02:45"}, {4.5,	"04:30"}, {6,	"06:00"}, {0,	"??:??"},	// Boss 3
	{6,	"06:00"}, {0,	"??:??"},	// Boss 4
	{0,	"??:??"},	// Secret level 1
	{0,	"??:??"},	// Secret level 2
};

// ELEVATOR BACK MAPS - REMEMBER (-1)!!
unsigned char ElevatorBackTo[]={1,1,7,3,5,3};
// For SPEAR
#define FROMSECRET1	3
#define FROMSECRET2	11

#define PAR_AMOUNT	500
#define PERCENT100AMT	10000
#define SECRETLVL_AMT 15000

enum {m_in_timebonus, m_in_kills, m_in_secret, m_in_treasure, m_in_bonus, m_in_wait} m_in_state;
int counter;
int bonus, full_bonus, time_s, time_m, timeleft, kr, sr, tr;
bool speardemodone;

void M_Inter_f(void)
{
	key_dest=key_menu;
	m_state=m_inter;
	m_in_state=m_in_timebonus; counter=0;
	bonus=full_bonus=0;
	speardemodone=false;
	elev_snd=true;

	if((WolfVer.set==WSET_SOD && gamestate.map!=5 && gamestate.map!=10 && gamestate.map!=16 && gamestate.map<18) ||
		 (WolfVer.set!=WSET_SOD && gamestate.map<9))  // FIXME: secret level
	{
		time_s=gamestate.TimeCount/1000;	 // time in seconds
		if(time_s>99*60+59) time_s=99*60+59; //max 99:59
		if(!IS_SPEAR)
			timeleft=(int)(60*parTimes[(gamestate.episode-1)*10+gamestate.map-1].time)-time_s;
		else
			timeleft=(int)(60*parTimesSpear[gamestate.map-1].time)-time_s;
		time_m=time_s/60; time_s=time_s%60;
		kr=sr=tr=0;
		if(gamestate.killtotal)
			kr=gamestate.killcount*100/gamestate.killtotal;
		if(gamestate.secrettotal)
			sr=gamestate.secretcount*100/gamestate.secrettotal;
		if(gamestate.treasuretotal)
			tr=gamestate.treasurecount*100/gamestate.treasuretotal;
		if(timeleft<0) timeleft=0;
		full_bonus+=timeleft*PAR_AMOUNT;
		if(kr==100) full_bonus+=PERCENT100AMT;
		if(sr==100) full_bonus+=PERCENT100AMT;
		if(tr==100) full_bonus+=PERCENT100AMT;
	}
	else
	{
		PL_GivePoints(&Player, SECRETLVL_AMT); // sectet level finished bonus
		m_in_state=m_in_wait;
	}
}

void M_Write(int x, int y, char *txt)
{
	int alpha[]={L_NUM0PIC,L_NUM1PIC,L_NUM2PIC,L_NUM3PIC,L_NUM4PIC,L_NUM5PIC,
		L_NUM6PIC,L_NUM7PIC,L_NUM8PIC,L_NUM9PIC,L_COLONPIC,0,0,0,0,0,0,L_APIC,L_BPIC,
		L_CPIC,L_DPIC,L_EPIC,L_FPIC,L_GPIC,L_HPIC,L_IPIC,L_JPIC,L_KPIC,
		L_LPIC,L_MPIC,L_NPIC,L_OPIC,L_PPIC,L_QPIC,L_RPIC,L_SPIC,L_TPIC,
		L_UPIC,L_VPIC,L_WPIC,L_XPIC,L_YPIC,L_ZPIC};

	int ox, nx, ny;
	char ch;

	ox=nx=x*16;
	ny=y*19;
	while(ch=*txt++)
	{
		switch(ch)
		{
		case '\n':
			nx=ox; ny+=38;
			continue;
		case '!':
			Vid_DrawPic(nx, ny, L_EXPOINTPIC);
			nx+=16;
			continue;
		case '\'':
			Vid_DrawPic(nx, ny, L_APOSTROPHEPIC);
			nx+=16;
			continue;
		case ' ':
			break;
		case ':':
			Vid_DrawPic(nx, ny, L_COLONPIC);
			nx+=16;
			continue;
		case '%':
			Vid_DrawPic(nx, ny, L_PERCENTPIC);
			break;
		default:
			if(ch>='a')
				ch-=('a'-'A');
			ch-='0';
			if(ch>=0)
				Vid_DrawPic(nx, ny, alpha[ch]);
		}
		nx+=32;
	}
}

// Breathe Mr. BJ!!!
void M_BJ_Breathe(void)
{
	static int which=0, bj_timer=35;
	int pics[2]={L_GUYPIC, L_GUY2PIC};

	bj_timer-=tics;
	if(bj_timer<0)
	{
		which^=1;
		bj_timer=35;
	}
	Vid_DrawPic(0, 38, pics[which]);
}

void M_Inter_Draw(void)
{
	if(elev_snd)
	{
		if(SD_SoundPlaying(LEVELDONESND))
			return;
		elev_snd=0;
		SD_PlayMusic(ENDLEVEL_MUS, true);
	}
	
	Vid_DrawBox(0, 0, XRES, YRES, RGBA_INTER);
	M_BJ_Breathe();

	if((WolfVer.set==WSET_SOD && gamestate.map!=5 && gamestate.map!=10 && gamestate.map!=16 && gamestate.map<18) ||
		 (WolfVer.set!=WSET_SOD && gamestate.map<9))  // FIXME: secret level
	{
		M_Write(14,  2, va("floor %d\ncompleted", gamestate.map));
		M_Write(16, 10, va("TIME %02d:%02d", time_m, time_s));
		M_Write(18, 12, va("PAR %s", IS_SPEAR?parTimesSpear[gamestate.map-1].timestr:parTimes[(gamestate.episode-1)*10+gamestate.map-1].timestr));
		if(m_in_state==m_in_timebonus)
		{
			M_Write(14,  7, va("BONUS %5d", counter*PAR_AMOUNT));
			if(!(counter++%10)) SD_PlaySound(ENDBONUS1SND, CHAN_LOCAL);
			if(counter>timeleft)
			{
				counter=0;
				m_in_state++;
				bonus+=timeleft*PAR_AMOUNT;
				SD_PlaySound(ENDBONUS2SND, CHAN_BODY);
			}

			M_Write( 9, 14, "KILL RATIO");
			M_Write( 5, 16, "SECRET RATIO");
			M_Write( 1, 18, "TREASURE RATIO");
		}
		else if(m_in_state==m_in_kills)
		{
			M_Write(14,  7, va("BONUS %5d", bonus));
			M_Write( 9, 14, va("KILL RATIO %3d%%", counter));
			if(!(counter++%10)) SD_PlaySound(ENDBONUS1SND, CHAN_LOCAL);
			if(counter>kr)
			{
				counter=0;
				m_in_state++;
				if(kr==100)
				{
					SD_PlaySound(PERCENT100SND, CHAN_BODY);
					bonus+=PERCENT100AMT;
				}
				else if(kr==0)
					SD_PlaySound(NOBONUSSND, CHAN_BODY);
				else
					SD_PlaySound(ENDBONUS2SND, CHAN_BODY);
			}
			M_Write( 5, 16, "SECRET RATIO");
			M_Write( 1, 18, "TREASURE RATIO");
		}
		else if(m_in_state==m_in_secret)
		{
			M_Write(14,  7, va("BONUS %5d", bonus));
			M_Write( 9, 14, va("KILL RATIO %3d%%", kr));
			M_Write( 5, 16, va("SECRET RATIO %3d%%", counter));
			if(!(counter++%10)) SD_PlaySound(ENDBONUS1SND, CHAN_LOCAL);
			if(counter>sr)
			{
				counter=0;
				m_in_state++;
				if(sr==100)
				{
					SD_PlaySound(PERCENT100SND, CHAN_BODY);
					bonus+=PERCENT100AMT;
				}
				else if(sr==0)
					SD_PlaySound(NOBONUSSND, CHAN_BODY);
				else
					SD_PlaySound(ENDBONUS2SND, CHAN_BODY);
			}
			M_Write( 1, 18, "TREASURE RATIO");
		}
		else if(m_in_state==m_in_treasure)
		{
			M_Write(14,  7, va("BONUS %5d", bonus));
			M_Write( 9, 14, va("KILL RATIO %3d%%", kr));
			M_Write( 5, 16, va("SECRET RATIO %3d%%", sr));
			M_Write( 1, 18, va("TREASURE RATIO %3d%%", counter));
			if(!(counter++%10)) SD_PlaySound(ENDBONUS1SND, CHAN_LOCAL);
			if(counter>tr)
			{
				counter=0;
				m_in_state++;
				if(tr==100)
				{
					SD_PlaySound(PERCENT100SND, CHAN_BODY);
					bonus+=PERCENT100AMT;
				}
				else if(tr==0)
					SD_PlaySound(NOBONUSSND, CHAN_BODY);
				else
					SD_PlaySound(ENDBONUS2SND, CHAN_BODY);
			}
		}
		else
		{
			if(m_in_state==m_in_bonus)
			{
				PL_GivePoints(&Player, full_bonus);
				m_in_state++;
			}
			M_Write(14,  7, va("BONUS %5d", full_bonus));
			M_Write( 9, 14, va("KILL RATIO %3d%%", kr));
			M_Write( 5, 16, va("SECRET RATIO %3d%%", sr));
			M_Write( 1, 18, va("TREASURE RATIO %3d%%", tr));
		}
	}
	else
	{
		if(IS_SPEAR)
			switch(gamestate.map)
			{
			case 5:
				M_Write(14, 4, " trans\n grosse\ndefeated!");
				break;
			case 10:
				M_Write(14, 4, "barnacle\nwilhelm\ndefeated!");
				break;
			case 16:
				M_Write(14, 4, "ubermutant\ndefeated!");
				break;
			case 18:
				M_Write(14, 4, " death\n knight\ndefeated!");
				break;
			case 19:
				M_Write(13, 4, "secret tunnel\n    area\n  completed!");
				break;
			case 20:
				M_Write(13, 4, "secret castle\n    area\n  completed!");
				break;
			}
		else
			M_Write(14, 4, "secret floor\n completed!");
	  M_Write(10, 16, "15000 bonus!");
	}
	R_DrawHUD();
	if(speardemodone)
	{
		Vid_Message("This concludes your demo\n"
								"of Spear of Destiny! Now,\n"
								"go to your local software\n"
								"store and buy it!");
	}
}

void M_Inter_Key(int key)
{
	if(elev_snd) return;
	
	if(key==K_SPACE || key==K_ENTER || key==K_MOUSE1)
	{
		if(m_in_state<=m_in_bonus)
		{
			PL_GivePoints(&Player, full_bonus);
			m_in_state=m_in_wait;
			return;
		}
		if(!IS_SPEAR)
		{
			if(gamestate.map==10)
				gamestate.map=ElevatorBackTo[gamestate.episode-1]+1;	// back from secret
			else if(Player.playstate==ex_secretlevel)
				gamestate.map=10;
			else
				gamestate.map++;
			PL_NextLevel(&Player);
			LoadRealLevel((gamestate.episode-1)*10+(gamestate.map-1));
		}
		else
		{
			if(WolfVer.set==WSET_SDM && gamestate.map==2)
			{
				if(speardemodone)
				{
					Player.playstate=ex_notingame;
					M_Menu_Main_f();
				}
				else
					speardemodone=true;
				return;
			}
			else if(gamestate.map==19)
				gamestate.map=FROMSECRET1+2;
			else if(gamestate.map==20)
				gamestate.map=FROMSECRET2+2;
			else if(Player.playstate==ex_secretlevel)
				switch(gamestate.map)
				{
				case FROMSECRET1+1:
					gamestate.map=19;
					break;
				case FROMSECRET2+1:
					gamestate.map=20;
					break;
				}
			else
				gamestate.map++;
			PL_NextLevel(&Player);
			LoadRealLevel(gamestate.map-1);
		}
		Player.playstate=ex_playing;
		m_state=m_none;
		key_dest=key_game;
	}
}

//=============================================================================
/* INTRO MENU (SOME SORT OF) + SPEAR END SCREENS */
int i_timer;
enum
{
	m_i_pg13, m_i_title, m_i_credits, m_i_scores,
	m_i_bjc1, m_i_bjc2, m_i_bjc3, m_i_bjc4, m_i_idguys
} m_i_state;

void M_Intro_f(void)
{
	if(IS_SPEAR)
		SD_PlayMusic(XTOWER2_MUS, true);
	else
		SD_PlayMusic(NAZI_NOR_MUS, true);
	key_dest=key_menu;
	m_state=m_intro;
	m_i_state=m_i_pg13;
	i_timer=7*70; // 7 seconds for PG13
}

void M_Demo_f(void)
{
	if(IS_SPEAR)
		SD_PlayMusic(XTOWER2_MUS, false);
	else
		SD_PlayMusic(NAZI_NOR_MUS, false);
	key_dest=key_menu;
	m_state=m_intro;
	m_i_state=m_i_title;
	i_timer=15*70;
}

// spear of destiny victory screens
void M_Victory_f(void)
{
	key_dest=key_menu;
	m_state=m_intro;
	m_i_state=m_i_bjc1;
	i_timer=2*70;
}

// spear Easter Egg (Id without pants)
void M_Menu_IdGuys_f(void)
{
	SD_PlayMusic(XJAZNAZI_MUS, false);
	key_dest=key_menu;
	m_state=m_intro;
	m_i_state=m_i_idguys;
}

static bool pic_cached=false;
void M_Intro_Draw(void)
{
	i_timer-=tics;
	switch(m_i_state)
	{
	case m_i_pg13:
		Vid_DrawBox(0, 0, XRES, YRES, RGBA_PG13C);
		Vid_DrawPic(432, 264, PG13PIC);
		if(i_timer<0)
		{
			m_i_state=m_i_title;
			i_timer=15*70;
			Vid_UnCachePic(PG13PIC);
		}
		break;
	case m_i_title:
		if(IS_SPEAR)
		{
			if(!pic_cached)
			{ // we must load custom palette, before caching pics!
				void *oldpal;
				int n;

				pic_cached=true;
				oldpal=WolfPal;		// save
				WolfPal=malloc(256*3);
				File_VGA_ReadChunk(VGA_REMAP[TITLEPALETTE], (unsigned char *)WolfPal); // read palette
				for(n=0; n<256; n++)
				{ // convert to 32-bit
					WolfPal[n].r<<=2;
					WolfPal[n].g<<=2;
					WolfPal[n].b<<=2;
				}
				Vid_CachePic(TITLE1PIC);
				Vid_CachePic(TITLE2PIC);
				free(WolfPal);
				WolfPal=oldpal;		// restore
			}
			Vid_DrawPic(0, 0, TITLE1PIC);
			Vid_DrawPic(0, 192, TITLE2PIC);
		}
		else
			Vid_DrawPic(0, 0, TITLEPIC);
		if(i_timer<0)
		{
			m_i_state=m_i_credits;
			i_timer=10*70;
			if(IS_SPEAR)
			{
				pic_cached=false;
				Vid_UnCachePic(TITLE1PIC);
				Vid_UnCachePic(TITLE2PIC);
			}
			else
				Vid_UnCachePic(TITLEPIC);
		}
		break;
	case m_i_credits:
		Vid_DrawPic(0, 0, CREDITSPIC);
		if(i_timer<0)
		{
			m_i_state=m_i_scores;
			i_timer=10*70;
			Vid_UnCachePic(CREDITSPIC);
		}
		break;
	case m_i_scores:
		M_Scores_Draw();
		if(i_timer<0)
			M_Menu_Main_f();
		break;
	case m_i_idguys:
		if(!pic_cached)
		{ // we must load custom palette, before caching pics!
			void *oldpal;
			int n;

			pic_cached=true;
			oldpal=WolfPal;		// save
			WolfPal=malloc(256*3);
			File_VGA_ReadChunk(VGA_REMAP[IDGUYSPALETTE], (unsigned char *)WolfPal); // read palette
			for(n=0; n<256; n++)
			{ // convert to 32-bit
				WolfPal[n].r<<=2;
				WolfPal[n].g<<=2;
				WolfPal[n].b<<=2;
			}
			Vid_CachePic(IDGUYS1PIC);
			Vid_CachePic(IDGUYS2PIC);
			free(WolfPal);
			WolfPal=oldpal;		// restore
		}
		Vid_DrawPic(0, 0, IDGUYS1PIC);
		Vid_DrawPic(0, 192, IDGUYS2PIC);
		break;
	}
}

void M_Intro_Key(int key)
{
	switch(m_i_state)
	{
	case m_i_pg13:
		Vid_UnCachePic(PG13PIC);
		M_Demo_f();
		return;
	case m_i_credits:
		Vid_UnCachePic(CREDITSPIC);
		break;
	case m_i_idguys:
		pic_cached=false;
		Vid_UnCachePic(IDGUYS1PIC);
		Vid_UnCachePic(IDGUYS2PIC);
		break;
	case m_i_title:
		if(IS_SPEAR)
		{
			pic_cached=false;
			Vid_UnCachePic(TITLE1PIC);
			Vid_UnCachePic(TITLE2PIC);
		}
		else
			Vid_UnCachePic(TITLEPIC);
		break;
	}
	M_Menu_Main_f();
}

//=============================================================================
/* MAIN MENU */
int	m_main_cursor=0;
#define	MAIN_ITEMS 10
char *m_main_items[MAIN_ITEMS+1]=
{
	"New Game",
	"Sound",
	"Control",
	"Load Game",
	"Save Game",
	"Video Options",
	"Read This!",
	"View Scores",
	"Back to Demo",
	"Quit",
	"Back to Game"
};

void M_Menu_Main_f(void)
{
	SD_PlayMusic(WONDERIN_MUS, false);
	key_dest=key_menu;
	m_state =m_main;
}

void M_Main_Draw(void)
{
	int n;

	Vid_MenuBack();
	Vid_DrawStripes();
	Vid_DrawPic(224, 442, C_MOUSELBACKPIC);
	Vid_DrawPic(160, 0, C_OPTIONSPIC);
	if(IS_SPEAR)
		Vid_DrawMenuBox(136, 125, 356, 295);
	else
		Vid_DrawMenuBox(136, 125, 356, 326);
	FNT_SetFont(FNT_WOLF_BIG);
	FNT_SetStyle(0, 0, 0);
	FNT_SetScale(1, 1);
	FNT_SetColor(RGBA_WLMNU);
	for(n=0; n<MAIN_ITEMS; n++)
	{
		if(n==4 && Player.playstate==ex_notingame)
			FNT_SetColor(RGBA_WLMDS);
		else if(m_main_cursor==n)
			FNT_SetColor(RGBA_WLMSL);
		if(WolfVer.set==WSET_WL1 || (WolfVer.set==WSET_WL6 && WolfVer.ver==WVER_11) || n<6)
		{
			if(n==8 && Player.playstate!=ex_notingame)
			{
				if(m_main_cursor==n)
					FNT_SetColor(RGBA_WLMCS);
				else
					FNT_SetColor(RGBA_WLMCU);
				FNT_Print(200, 132+n*31, m_main_items[10]);
			}
			else
				FNT_Print(200, 132+n*31, m_main_items[n]);
		}
		else if(n==6)
			continue;
		else
		{
			if(n==8 && Player.playstate!=ex_notingame)
			{
				if(m_main_cursor==n)
					FNT_SetColor(RGBA_WLMCS);
				else
					FNT_SetColor(RGBA_WLMCU);
				FNT_Print(200, 132+(n-1)*31, m_main_items[10]);
			}
			else
				FNT_Print(200, 132+(n-1)*31, m_main_items[n]);
		}
		if(m_main_cursor==n || (n==4 && Player.playstate==ex_notingame) || (n==8 && Player.playstate!=ex_notingame))
			FNT_SetColor(RGBA_WLMNU);
	}

	if((WolfVer.set==WSET_WL1 || (WolfVer.set==WSET_WL6 && WolfVer.ver==WVER_11)) || m_main_cursor<=6)
		M_DrawCursor(144, 127+31*m_main_cursor);
	else
		M_DrawCursor(144, 127+31*(m_main_cursor-1));
}

void M_Main_Key(int key)
{
	switch(key)
	{
	case K_ESCAPE:
		if(Player.playstate!=ex_notingame)
		{
			key_dest=key_game;
			m_state =m_none;
			SD_PlayMusic(CurMapData.music, true);
		}
		else
			M_Demo_f();
		break;
	case K_DOWNARROW:
		if(++m_main_cursor>=MAIN_ITEMS) m_main_cursor=0;
		if(WolfVer.set!=WSET_WL1 && (WolfVer.set!=WSET_WL6 || WolfVer.ver!=WVER_11) && m_main_cursor==6)
			m_main_cursor=7;
		if(m_main_cursor==4 && Player.playstate==ex_notingame)
			m_main_cursor=5;
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case K_UPARROW:
		if(--m_main_cursor<0)	m_main_cursor=MAIN_ITEMS-1;
		if(WolfVer.set!=WSET_WL1 && (WolfVer.set!=WSET_WL6 || WolfVer.ver!=WVER_11) && m_main_cursor==6)
			m_main_cursor=5;
		if(m_main_cursor==4 && Player.playstate==ex_notingame)
			m_main_cursor=3;
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case K_ENTER:
		SD_PlaySound(SHOOTSND, CHAN_LOCAL);
		switch(m_main_cursor)
		{
		case 0:
			M_Menu_NewGame_f();
			break;
		case 1:
			M_Menu_Sound_f();
			break;
		case 2:
			M_Menu_Controls_f();
			break;
		case 3:
			M_Menu_Load_f();
			break;
		case 4:
			M_Menu_Save_f();
			break;
		case 5:
			M_Menu_Video_f();
			break;
		case 6:
			M_Menu_ReadThis_f();
			break;
		case 7:
			M_Menu_Scores_f();
			break;
		case 8:
			if(Player.playstate!=ex_notingame)
			{
				key_dest=key_game;
				m_state =m_none;
				SD_PlayMusic(CurMapData.music, true);
			}
			else
				M_Demo_f();
			break;
		case 9:
			M_Menu_Quit_f();
			break;
		}
		break;
	}
}

//=============================================================================
/* KEYS MENU */
char *bindnames[][2] =
{
	{"+attack",			"attack"},
	{"impulse 10",	"change weapon"},
	{"+forward",		"walk forward"},
	{"+back",				"backpedal"},
	{"+left",				"turn left"},
	{"+right",			"turn right"},
	{"+speed",			"run"},
	{"+moveleft",		"step left"},
	{"+moveright",	"step right"},
	{"+strafe",			"sidestep"},
	{"+use",				"use"}
};
#define	NUMCOMMANDS	(sizeof(bindnames)/sizeof(bindnames[0]))

int	keys_cursor=0, bind_grab=0;

void M_Menu_Controls_f(void)
{
	key_dest=key_menu;
	m_state =m_controls;
}

void M_Controls_Draw(void)
{
	int	n;
	int	keys[2];
	int	y, h;

	Vid_MenuBack();
	Vid_DrawPic(224, 442, C_MOUSELBACKPIC);
	Vid_DrawStripes();
	Vid_DrawPic(160, 0, C_CUSTOMIZEPIC);
	Vid_DrawMenuBox(60, 150, 520, 285);

	FNT_SetFont(FNT_WOLF_SMALL);
	FNT_SetStyle(0, 0, 0);
	FNT_SetScale(1, 1);
	FNT_SetColor(RGBA_READH);

	if(bind_grab)
		FNT_PrintC(118, 0, 640, "Press a key for this action");
	else
		FNT_PrintC(118, 0, 640, "Enter to change, backspace to clear");

	FNT_SetColor(RGBA_WLMSL);

	h=unpackshort2(FNT_GetSize("A"));

// search for known bindings
	for(n=0; n<NUMCOMMANDS; n++)
	{
		y=115+h*(n+2);

		FNT_Print(75, y, bindnames[n][1]);

		Key_FindKeysForCommand(bindnames[n][0], keys);
		FNT_Printf(280, y, "%s %s", keys[0]==-1 ? "???" : Key_KeynumToString(keys[0]),
																keys[1]==-1 ? "" : va("and %s", Key_KeynumToString(keys[1]) ));
	}

	if(bind_grab)
		FNT_Print(263, 115+(keys_cursor+2)*h, "=");
	else
		FNT_Print(263, 115+(keys_cursor+2)*h, ">");
}

void M_Controls_Key(int key)
{
	char *cmd;
	int	keys[2];

	if(bind_grab)
	{	// defining a key
		if(key==K_ESCAPE)
		{
			SD_PlaySound(ESCPRESSEDSND, CHAN_LOCAL);
			bind_grab=false;
		}
		else if(key!='`')
		{
			SD_PlaySound(SHOOTSND, CHAN_LOCAL);
			cmd=va("bind \"%s\" \"%s\"\n", Key_KeynumToString(key), bindnames[keys_cursor][0]);
			Cbuf_InsertText(cmd);
		}

		bind_grab=false;
		return;
	}

	switch(key)
	{
	case K_ESCAPE:
		SD_PlaySound(ESCPRESSEDSND, CHAN_LOCAL);
		M_Menu_Main_f();
		break;
	case K_LEFTARROW:
	case K_UPARROW:
		keys_cursor--;
		if(keys_cursor<0) keys_cursor=NUMCOMMANDS-1;
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case K_DOWNARROW:
	case K_RIGHTARROW:
		keys_cursor++;
		if(keys_cursor>=NUMCOMMANDS) keys_cursor=0;
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case K_ENTER:			// go into bind mode
		Key_FindKeysForCommand(bindnames[keys_cursor][0], keys);
		if(keys[1]!=-1)
			Key_UnbindCommand(bindnames[keys_cursor][0]);
		bind_grab = true;
		break;
	case K_BACKSPACE:	// delete bindings
	case K_DEL:				// delete bindings
		Key_UnbindCommand(bindnames[keys_cursor][0]);
		break;
	}
}

//=============================================================================
/* Load/Save MENU */
int	m_load_cursor=0;

void M_Menu_Load_f(void)
{
	if(key_dest==key_menu && m_state==m_load) return;
	SD_PlayMusic(WONDERIN_MUS, false);
	wasInMenus=(key_dest==key_menu);
	key_dest=key_menu;
	m_prevstate=m_state;
	m_state=m_load;
	ScanSaves();
}

void M_Menu_Save_f (void)
{
	if(key_dest==key_menu && (m_state==m_dead || m_state==m_save)) return;
	SD_PlayMusic(WONDERIN_MUS, false);
	wasInMenus=(key_dest==key_menu);
	key_dest=key_menu;
	m_prevstate=m_state;
	m_state=m_save;
	ScanSaves();
}

void M_Load_Draw(void)
{
	int n;

	Vid_MenuBack();
	Vid_DrawPic(224, 442, C_MOUSELBACKPIC);
	Vid_DrawMenuBox(130, 120, 390, 336);
	Vid_DrawStripes();
	Vid_DrawPic(120, 0, C_LOADGAMEPIC);

	FNT_SetFont(FNT_WOLF_SMALL);
	FNT_SetStyle(0, 0, 0);
	FNT_SetScale(1, 1);
	FNT_SetColor(RGBA_WLMNU);
	for(n=0; n<SAVE_SLOTS; n++)
	{
		if(m_load_cursor==n)
		{
			Vid_DrawWindow(198, 132+n*31, 312, 26, RGBA_WBKGC, RGBA_WLMSL, RGBA_WLMSL);
			FNT_SetColor(RGBA_WLMSL);
		}
		else
			Vid_DrawWindow(198, 132+n*31, 312, 26, RGBA_WBKGC, RGBA_WLMNU, RGBA_WLMNU);

		FNT_Print(202, 134+n*31, m_filenames[n]);
		if(m_load_cursor==n) FNT_SetColor(RGBA_WLMNU);
	}

	M_DrawCursor(140, 127+31*m_load_cursor);
}

void M_Save_Draw(void)
{
	int n;

	Vid_MenuBack();
	Vid_DrawPic(224, 442, C_MOUSELBACKPIC);
	Vid_DrawMenuBox(130, 120, 390, 336);
	Vid_DrawStripes();
	Vid_DrawPic(120, 0, C_SAVEGAMEPIC);

	FNT_SetFont(FNT_WOLF_SMALL);
	FNT_SetStyle(0, 0, 0);
	FNT_SetScale(1, 1);
	FNT_SetColor(RGBA_WLMNU);
	for(n=0; n<SAVE_SLOTS; n++)
	{
		if(m_load_cursor==n)
		{
			Vid_DrawWindow(198, 132+n*31, 312, 26, RGBA_WBKGC, RGBA_WLMSL, RGBA_WLMSL);
			FNT_SetColor(RGBA_WLMSL);
		}
		else
			Vid_DrawWindow(198, 132+n*31, 312, 26, RGBA_WBKGC, RGBA_WLMNU, RGBA_WLMNU);

		FNT_Print(202, 134+n*31, m_filenames[n]);
		if(m_load_cursor==n) FNT_SetColor(RGBA_WLMNU);
	}

	M_DrawCursor(140, 127+31*m_load_cursor);
}

void M_Load_Key(int key)
{
	switch(key)
	{
	case K_DOWNARROW:
		if(++m_load_cursor>=SAVE_SLOTS) m_load_cursor=0;
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case K_UPARROW:
		if(--m_load_cursor<0) m_load_cursor=SAVE_SLOTS-1;
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case K_ESCAPE:
		SD_PlaySound(ESCPRESSEDSND, CHAN_LOCAL);
		if(wasInMenus)
		{
			key_dest=key_menu;
			m_state=m_prevstate;
		}
		else
		{
			key_dest=key_game;
			m_state =m_none;
			SD_PlayMusic(CurMapData.music, true);
		}
		break;
	case K_ENTER:
		if(!loadable[m_load_cursor]) return;
		SD_PlaySound(SHOOTSND, CHAN_LOCAL);
		if(Load(m_load_cursor))
		{
			key_dest=key_game;
			m_state=m_none;
		}
		break;
	}
}

void M_Save_Key(int key)
{
	switch(key)
	{
	case K_DOWNARROW:
		if(++m_load_cursor>=SAVE_SLOTS) m_load_cursor=0;
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case K_UPARROW:
		if(--m_load_cursor<0) m_load_cursor=SAVE_SLOTS-1;
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case K_ESCAPE:
		SD_PlaySound(ESCPRESSEDSND, CHAN_LOCAL);
		if(wasInMenus)
		{
			key_dest=key_menu;
			m_state=m_prevstate;
		}
		else
		{
			key_dest=key_game;
			m_state=m_none;
			SD_PlayMusic(CurMapData.music, true);
		}
		break;
	case K_ENTER:
		SD_PlaySound(SHOOTSND, CHAN_LOCAL);
		Save(m_load_cursor);
		key_dest=key_game;
		m_state=m_none;
		SD_PlayMusic(CurMapData.music, false);
		break;
	}
}

//=============================================================================
/* QUIT MENU */
int	msgNumber;

char *quitMessage[]=
{
	"Dost thou wish to\nleave with such hasty\nabandon?",
	"Chickening out...\nalready?",
	"Press N for more carnage.\nPress Y to be a weenie.",
	"So, you think you can\nquit this easily, huh?",
	"Press N to save the world.\nPress Y to abandon it in\nits hour of need.",
	"Press N if you are brave.\nPress Y to cower in shame.",
	"Heroes, press N.\nWimps, press Y.",
	"You are at an intersection.\nA sign says, 'Press Y to quit.'\n>",
	"For guns and glory, press N.\nFor work and worry, press Y."
};
char *spearQuit[]=
{
	"Heroes don't quit, but\ngo ahead and press Y\nif you aren't one.",
	"Press Y to quit,\nor press N to enjoy\nmore violent diversion.",
	"Depressing the Y key means\nyou must return to the\nhumdrum workday world.",
	"Hey, quit or play,\nY or N:\nit's your choice.",
	"Sure you don't want to\nwaste a few more\nproductive hours?",
	"I think you had better\nplay some more. Please\npress N...please?",
	"If you are tough, press N.\nIf not, press Y daintily.",
	"I'm thinkin' that\nyou might wanna press N\nto play more. You do it.",
	"Sure. Fine. Quit.\nSee if we care.\nGet it over with.\nPress Y."
};

void M_Menu_Quit_f(void)
{
	if(key_dest==key_menu && m_state==m_quit) return;
	wasInMenus=(key_dest==key_menu);
	key_dest=key_menu;
	m_prevstate=m_state;
	m_state=m_quit;
	msgNumber=US_Rnd()%9;
}

void M_Quit_Key(int key)
{
	switch(key)
	{
	case K_ESCAPE:
	case 'n':
	case 'N':
		if(wasInMenus)
			m_state=m_prevstate;
		else
		{
			key_dest=key_game;
			m_state=m_none;
		}
		break;
	case 'Y':
	case 'y':
		Sys_Quit();
		break;
	default:
		break;
	}
}

void M_Quit_Draw(void)
{
	if(wasInMenus)
	{
		m_state=m_prevstate;
		M_Draw();
		m_state=m_quit;
	}
	if(IS_SPEAR)
		Vid_Message(spearQuit[msgNumber]);
	else
		Vid_Message(quitMessage[msgNumber]);
}

//=============================================================================
/* New Game MENU */

void M_Menu_NewGame_f(void)
{
	// check if game in progress!
	// if not go directly to {episode | skill} (also choice is here)
	key_dest=key_menu;
	m_state=m_newg;
	if(IS_SPEAR)
		M_Menu_Skill_f();
	else
		M_Menu_Episode_f();
}

void M_NewGame_Key(int key)
{
}

void M_NewGame_Draw(void)
{
	Vid_Message("You are currently in\na game. Continuing will\nerase old game. Ok?");
}

//=============================================================================
/* Episode MENU */
bool warning_show=false;
int	m_episode_cursor=0;
#define	EPISODES 6
char *m_episodes[EPISODES]=
{
	"Escape from Wolfenstein",
	"Operation: Eisenfaust",
	"Die, Fuhrer, Die!",
  "A Dark Secret",
  "Trail of the Madman",
  "Confrontation"
};

void M_Menu_Episode_f(void)
{
	key_dest=key_menu;
	m_state=m_episode;
}

void M_Episode_Key(int key)
{
	if(warning_show) {warning_show=false; return;}
	switch(key)
	{
	case K_ESCAPE:
		SD_PlaySound(ESCPRESSEDSND, CHAN_LOCAL);
		M_Menu_Main_f();
		break;
	case K_DOWNARROW:
		if(++m_episode_cursor>=EPISODES) m_episode_cursor=0;
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case K_UPARROW:
		if(--m_episode_cursor<0)	m_episode_cursor=EPISODES-1;
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case K_ENTER:
		if( (WolfVer.set==WSET_WL1 && m_episode_cursor>0) || 
				(WolfVer.set==WSET_WL3 && m_episode_cursor>2) )
		{
			SD_PlaySound(NOWAYSND, CHAN_LOCAL);
			warning_show=true;
		}
		else
		{
			SD_PlaySound(SHOOTSND, CHAN_LOCAL);
			Cvar_SetInteger("episode", m_episode_cursor);
			M_Menu_Skill_f();
		}
		break;
	}
}

void M_Episode_Draw(void)
{
	int n;

	Vid_MenuBack();
	Vid_DrawPic(224, 442, C_MOUSELBACKPIC);
	Vid_DrawMenuBox(12, 46, 616, 388);
	FNT_SetFont(FNT_WOLF_BIG);
	FNT_SetStyle(0, 0, 0);
	FNT_SetScale(1, 1);
	FNT_SetColor(RGBA_READH);
	FNT_PrintC(4, 12, 616, "Which episode to play?");
	FNT_SetColor(RGBA_WLMNU);

	for(n=0; n<EPISODES; n++)
	{
		Vid_DrawPic(84, 55+n*62, C_EPISODE1PIC+n);

		if( (WolfVer.set==WSET_WL1 && n>0) || 
				(WolfVer.set==WSET_WL3 && n>2) )
		{
			if(m_episode_cursor==n)
				FNT_SetColor(RGBA_NOEPH);
			else
				FNT_SetColor(RGBA_NOEPC);
		}
		else
		{
			if(m_episode_cursor==n)
				FNT_SetColor(RGBA_WLMSL);
			else
				FNT_SetColor(RGBA_WLMNU);
		}
		FNT_Printf(196, 55+(n*2)*31, "Episode %d\n%s", n+1, m_episodes[n]);
	}

	M_DrawCursor(16, 50+31*(m_episode_cursor*2));

	if(warning_show)
	{
		Vid_Message("Please select \"Read This!\"\n"
								"from the Options menu to\n"
								"find out how to order this\n"
								"episode from Apogee.");
	}
}

//=============================================================================
/* Skill MENU */
int	m_skill_cursor=2; // default is normal mode
#define	SKILLS 4
char *m_skills[SKILLS]=
{
	"Can I play, Daddy?",
	"Don't hurt me.",
	"Bring 'em on!",
	"I am Death incarnate!"
};

void M_Menu_Skill_f(void)
{
	key_dest=key_menu;
	m_state=m_skill;
}
extern char skill; //FIXME!
void M_Skill_Key(int key)
{
	switch(key)
	{
	case K_ESCAPE:
		SD_PlaySound(ESCPRESSEDSND, CHAN_LOCAL);
		if(IS_SPEAR)
			M_Menu_Main_f();
		else
			M_Menu_Episode_f();
		break;
	case K_DOWNARROW:
		if(++m_skill_cursor>=SKILLS) m_skill_cursor=0;
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case K_UPARROW:
		if(--m_skill_cursor<0)	m_skill_cursor=SKILLS-1;
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case K_ENTER:
		SD_PlaySound(SHOOTSND, CHAN_LOCAL);
		Cvar_SetInteger("skill", m_skill_cursor);
		skill=m_skill_cursor;
		PL_NewGame(&Player);
		LoadRealLevel(m_episode_cursor*10);
		key_dest=key_game;
		
		Player.playstate=ex_playing; // FIXME: start game
		break;
	}
}

void M_Skill_Draw(void)
{
	int n;

	Vid_MenuBack();
	Vid_DrawPic(224, 442, C_MOUSELBACKPIC);
	Vid_DrawMenuBox(90, 216, 450, 161);
	FNT_SetFont(FNT_WOLF_BIG);
	FNT_SetStyle(0, 0, 0);
	FNT_SetScale(1, 1);
	FNT_SetColor(RGBA_READH);
	if(IS_SPEAR)
		Vid_DrawPic(140, 163, C_HOWTOUGHPIC);
	else
		FNT_Print(140, 163, "How tough are you?");
	FNT_SetColor(RGBA_WLMNU);
	Vid_DrawPic(470, 257, m_skill_cursor+C_BABYMODEPIC);
	
	for(n=0; n<SKILLS; n++)
	{
		if(m_skill_cursor==n) FNT_SetColor(RGBA_WLMSL);
		FNT_Print(148, 240+n*31, m_skills[n]);
		if(m_skill_cursor==n) FNT_SetColor(RGBA_WLMNU);
	}

	M_DrawCursor(96, 235+31*m_skill_cursor);
}

//=============================================================================
/* Video Options MENU */
#define VOPT_COUNT 8
int m_vopt_cursor;

void M_Menu_Video_f(void)
{
	key_dest=key_menu;
	m_state=m_video;
}

void M_Video_Draw(void)
{
	Vid_MenuBack();
	Vid_DrawStripes();
	Vid_DrawPic(224, 442, C_MOUSELBACKPIC);
	Vid_DrawPic(160, 0, C_OPTIONSPIC);
	Vid_DrawMenuBox(50, 195, 540, 168);

	FNT_SetFont(FNT_WOLF_SMALL);
	FNT_SetStyle(0, 1, 0);
	FNT_SetColorSh(0, 0, 0, 164);
	FNT_SetScale(1, 1);
	FNT_SetColor(RGBA_READH);

	FNT_PrintC(125, 0, 640, "NOTE: you must restart the game");
	FNT_PrintC(150, 0, 640, "for changes to take effect");

	FNT_SetFont(FNT_WOLF_BIG);
	FNT_SetColor(RGBA_WLMSL);
	FNT_SetStyle(0, 0, 0);
	FNT_AllowFMT(true); // colored text

	FNT_Printf(112, 202+0*31, "Video ^4%s", Vid_DescribeMode(Cvar_VariableInteger("r_mode")));
	FNT_Printf(112, 202+1*31, "Color depth: ^3%d bits", Cvar_VariableInteger("r_colorbits"));
	FNT_Printf(112, 202+2*31, "Full screen: %s", Cvar_VariableInteger("r_fullscreen")?"^2on":"^1off");
	FNT_Printf(112, 202+3*31, "Brightness: ^3%4.2f", Cvar_VariableValue("r_gamma"));
	FNT_Printf(112, 202+4*31, "Sync frames: %s", Cvar_VariableInteger("r_vsync")?"^2on":"^1off");
	FNT_AllowFMT(false);
	M_DrawCursor(58, 197+31*m_vopt_cursor);
}

bool ee_i=false, ee_d=false;
void M_Video_Key(int key)
{
	switch(key)
	{
	case K_LEFTARROW:
	case K_RIGHTARROW:
		switch(m_vopt_cursor)
		{
		case 0:
		{
			int tmp, modes;

			tmp=Cvar_VariableInteger("r_mode");
			if(key==K_LEFTARROW)
				tmp--;
			else
				tmp++;
			if(tmp<0) tmp=0;
			else if(tmp>=(modes=Vid_Modes())) tmp=modes-1;
			Cvar_ForceSetInteger("r_mode", tmp);
			break;
		}
		case 1:
			Cvar_ForceSet("r_colorbits", Cvar_VariableInteger("r_colorbits")==32?"16":"32");
			break;
		case 2:
			Cvar_ForceSet("r_fullscreen", Cvar_VariableInteger("r_fullscreen")?"0":"1");
			break;
		case 3:
		{
			float tmp;

			tmp=Cvar_VariableValue("r_gamma");
			if(key==K_LEFTARROW)
				tmp-=0.05f;
			else
				tmp+=0.05f;
			if(tmp<0) tmp=0;
			else if(tmp>1.75f) tmp=1.75f;
			Cvar_SetValue("r_gamma", tmp);
			break;
		}
		case 4:
			Cvar_Set("r_vsync", Cvar_VariableInteger("r_vsync")?"0":"1");
			break;
		}
		break;
	case K_UPARROW:
		if(--m_vopt_cursor<0) m_vopt_cursor=4;
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case K_DOWNARROW:
		if(++m_vopt_cursor>4) m_vopt_cursor=0;
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case 'i':
		ee_i=true;
		break;
	case 'd':
		ee_d=true;
		break;
	case K_ESCAPE:
		SD_PlaySound(ESCPRESSEDSND, CHAN_LOCAL);
		if(ee_i && ee_d && WolfVer.set==WSET_SOD)
			M_Menu_IdGuys_f(); // Spear of Destiny Easter Egg
		else
			M_Menu_Main_f();
		ee_i=ee_d=false;
		break;
	}
}

//=============================================================================
/* Sound Options MENU */
#define SOPT_COUNT 8
int m_sopt_cursor;

typedef struct
{
	int y_pos;
	char *txt;
	int enabeld, on;
} snd_t;

snd_t m_snd_items[SOPT_COUNT]=
{
	{48+0*31, "None", 1, 0},
	{48+1*31, "PC Speaker", 0, 0},
	{48+2*31, "AdLib/Sound Blaster", 1, 1},

	{48+5*31, "None", 1, 0},
	{48+6*31, "Sound Blaster", 1, 1},
	{48+7*31, "Disney Sound Source", 0, 0},

	{48+10*31, "None", 1, 0},
	{48+11*31, "AdLib/Sound Blaster", 1, 1},
};

void M_Sound_Update(void)
{
	int n, m;

	for(n=0; n<3; n++)
		for(m=0; m<(n==2?2:3); m++)
		{
			m_snd_items[n*3+m].enabeld=SD_mode.avail[n][m];
			switch(n)
			{
			case 0:
				m_snd_items[n*3+m].on=SD_mode.sfx==m;
				break;
			case 1:
				m_snd_items[n*3+m].on=SD_mode.digi==m;
				break;
			case 2:
				m_snd_items[n*3+m].on=SD_mode.mus==m;
				break;
			}
		}
}

void M_Menu_Sound_f(void)
{
	M_Sound_Update();

	key_dest=key_menu;
	m_state=m_sound;
}

void M_Sound_Draw(void)
{
	int n;

	Vid_MenuBack();
	Vid_DrawPic(224, 442, C_MOUSELBACKPIC);

	Vid_DrawMenuBox(80,  41, 500, 108);
	Vid_DrawMenuBox(80, 196, 500, 108);
	Vid_DrawMenuBox(80, 352, 500,  77);

	Vid_DrawPic(200,   0, C_FXTITLEPIC);
	Vid_DrawPic(200, 156, C_DIGITITLEPIC);
	Vid_DrawPic(200, 312, C_MUSICTITLEPIC);

	FNT_SetFont(FNT_WOLF_BIG);
	FNT_SetStyle(0, 0, 0);
	FNT_SetScale(1, 1);
	FNT_SetColor(RGBA_WLMNU);
	
	for(n=0; n<SOPT_COUNT; n++)
	{
		if(!m_snd_items[n].enabeld) FNT_SetColor(RGBA_WLMDS);
		else if(n==m_sopt_cursor)   FNT_SetColor(RGBA_WLMSL);
		FNT_Printf (200, m_snd_items[n].y_pos, m_snd_items[n].txt);
		Vid_DrawPic(144, m_snd_items[n].y_pos+4, m_snd_items[n].on ? C_SELECTEDPIC : C_NOTSELECTEDPIC);
		if(!m_snd_items[n].enabeld || n==m_sopt_cursor) FNT_SetColor(RGBA_WLMNU);
	}

	M_DrawCursor(96, m_snd_items[m_sopt_cursor].y_pos-5);
}

void M_Sound_Key(int key)
{
	switch(key)
	{
	case K_ENTER:
	case K_SPACE:
		SD_Config(m_sopt_cursor/3, m_sopt_cursor%3);
		M_Sound_Update();
		break;
	case K_ESCAPE:
		SD_PlaySound(ESCPRESSEDSND, CHAN_LOCAL);
		M_Menu_Main_f();
		break;
	case K_DOWNARROW:
		do
		{
			if(++m_sopt_cursor>=SOPT_COUNT) m_sopt_cursor=0;
		} while(!m_snd_items[m_sopt_cursor].enabeld);
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	case K_UPARROW:
		do
		{
			if(--m_sopt_cursor<0)	m_sopt_cursor=SOPT_COUNT-1;
		} while(!m_snd_items[m_sopt_cursor].enabeld);
		SD_PlaySound(MOVEGUN1SND, CHAN_LOCAL);
		break;
	}
}
//=============================================================================
/* High Scores MENU */
#define	SCORES 7
char *m_score[SCORES]=
{
	"id software-'92",
	"Adrian Carmack",
	"John Carmack",
	"Kevin Cloud",
	"Tom Hall",
	"John Romero",
	"Jay Wilbur"
};

void M_Menu_Scores_f(void)
{
	if(IS_SPEAR)
		SD_PlayMusic(XAWARD_MUS, true);
	else
		SD_PlayMusic(ROSTER_MUS, true);
	key_dest=key_menu;
	m_state=m_scores;
}

void M_Scores_Key(int key)
{
	M_Menu_Main_f();
}

void M_Scores_Draw(void)
{
	char buffer[16], *str;
	int w;
	int n;

	Vid_MenuBack();
	Vid_DrawStripes();
	if(IS_SPEAR)
	{
		Vid_DrawPic(0, 0, HIGHSCORESPIC);
		FNT_SetFont(FNT_WOLF_BIG);
	}
	else
	{
		Vid_DrawPic(96, 0, HIGHSCORESPIC);
		Vid_DrawPic(64, 163, C_NAMEPIC);
		Vid_DrawPic(320, 163, C_LEVELPIC);
		Vid_DrawPic(448, 163, C_SCOREPIC);
		FNT_SetFont(FNT_WOLF_SMALL);
	}

	FNT_SetStyle(0, 0, 0);
	FNT_SetScale(1, 1);
	FNT_SetColor(RGBA_C15);
	
	for(n=0; n<SCORES; n++)
	{
		if(IS_SPEAR)
		{ // spear
			// name
			FNT_Print(32, 182+38*n, m_score[n]);

			// level
			ultoa(1/*FIXME-Completed Level*/, buffer, 10);
			w=unpackshort1(FNT_GetSize(buffer));
			FNT_Print(352-w, 182+38*n, buffer);
		
			// score
			ultoa(1000/*FIXME-Score*/, buffer, 10);
			w=unpackshort1(FNT_GetSize(buffer));
			FNT_Print(584-w, 182+38*n, buffer);
		}
		else
		{
			// name
			FNT_Print(64, 182+38*n, m_score[n]);
			
			// level
			ultoa(1/*FIXME-Completed Level*/, buffer, 10);
			for(str=buffer; *str; str++)
				*str=*str+(129-'0');	// Used fixed-width numbers (129...)
			w=unpackshort1(FNT_GetSize(buffer));
			FNT_Printf(340-w, 182+38*n, "E%d/L%s", 0/*FIXME-Completed Episode*/+1, buffer);
		
			// score
			ultoa(1000/*FIXME-Score*/, buffer, 10);
			for(str=buffer; *str; str++)
				*str=*str+(129-'0');	// Used fixed-width numbers (129...)
			w=unpackshort1(FNT_GetSize(buffer));
			FNT_Print(528-w, 182+38*n, buffer);
		}
	}
}

//=============================================================================
/* Help Screens */

void M_Menu_Help_f(void)
{
	char *data;
	int psize;

	if(Cmd_Argc()==1)
	{ // no params! just show 'Read This!'
		M_Menu_ReadThis_f();
		return;
	}
	psize=FS_LoadFile(va("text/%s", Cmd_Argv(1)), &data, true);
	if(!data)
	{
		Con_Printf("File %s not found\n", va("text/%s", Cmd_Argv(1)));
		return;
	}
	Text_NewArticle(data, psize);
	FS_FreeFile(data);

	SD_PlayMusic(CORNER_MUS, true);
	key_dest=key_menu;
	m_state=m_txtpages;
}

void M_Menu_ReadThis_f(void)
{
	long psize;
	unsigned char *databuff;
	int chunk;

	chunk=VGA_REMAP[T_HELPART];

	psize=File_VGA_GetChunkSize(chunk);
	if(psize<=0) return;
	databuff=malloc(psize);
	File_VGA_ReadChunk(chunk, databuff);
	Text_NewArticle(databuff, psize);
	free(databuff);

	SD_PlayMusic(CORNER_MUS, true);
	key_dest=key_menu;
	m_state=m_txtpages;
}

/*
** Show debriefing text. episode is zero based
*/
void M_Menu_EndText_f(int episode)
{
	long psize;
	unsigned char *databuff;
	int chunk;

	chunk=VGA_REMAP[T_ENDART1+episode];

	psize=File_VGA_GetChunkSize(chunk);
	if(psize<=0) return;
	databuff=malloc(psize);
	File_VGA_ReadChunk(chunk, databuff);
	Text_NewArticle(databuff, psize);
	free(databuff);

	SD_PlayMusic(URAHERO_MUS, false);
	key_dest=key_menu;
	m_state=m_txtpages;
}

/*
** Show help text (Read this!)
*/
void M_TxtPages_Key(int key)
{
	if(Text_Input(key)) M_Menu_Main_f();
}

void M_TxtPages_Draw(void)
{
	Text_DisplayPage();
}

//=============================================================================
/* Dead Screen Menu (sort of) */
int intensity;
int killerangle; // slowly face last attacker
int anglediff, anglestep;

void M_Dead_f(void)
{
	vec2_t p;

	intensity=0;
	SD_PlaySound(PLAYERDEATHSND, CHAN_BODY);
	Msg_Printf("You are dead..");
	key_dest=key_menu;
	m_state=m_dead;
	
	p[0]=Player.LastAttacker->x; p[1]=Player.LastAttacker->y;
	killerangle=advAng(Player.position.origin, p);
	anglediff=WiseAngDiff(Player.position.angle, killerangle);
	if(abs(anglediff)>ANG_180) anglediff-=ANG_360;
	if(anglediff!=0)
		anglestep=anglediff/abs(anglediff);
	else
		anglestep=0;
}

void M_Dead_Draw(void)
{
	if(anglestep)
	{
		if(AngDiff(killerangle, Player.position.angle)<AngDiff(killerangle, Player.position.angle+tics*ANG_1*2*anglestep))
		{
			Player.position.angle=killerangle;
			anglestep=0;
		}
		else
			Player.position.angle+=tics*ANG_1*2*anglestep;

		Player.position.angle=NormalizeAngle(Player.position.angle);
	}
	else
	{
		Vid_DrawBox(0, 0, 640, 384, 0xFF, 0x00, 0x00, (byte)intensity);
		if(++intensity>=200) intensity=200;
	}
}

void M_Dead_Key(int key)
{
	switch(key)
	{
	case K_SPACE:
		if(!PL_Reborn(&Player))
		{
			Msg_Printf("Game Over!");
			M_Demo_f();
			return;
		}
		if(!IS_SPEAR)
			LoadRealLevel((gamestate.episode-1)*10+(gamestate.map-1));
		else
			LoadRealLevel(gamestate.map-1);
		Player.playstate=ex_playing;
		key_dest=key_game;
		m_state=m_none;
		break;
	}
}

//=============================================================================
/* Menu Subsystem */
int M_Init(void)
{
	Cmd_AddCommand("togglemenu", M_ToggleMenu_f);

	Cmd_AddCommand("menu_main", M_Menu_Main_f);
	Cmd_AddCommand("menu_newgame", M_Menu_NewGame_f);
	Cmd_AddCommand("menu_load", M_Menu_Load_f);
	Cmd_AddCommand("menu_save", M_Menu_Save_f);
	Cmd_AddCommand("menu_keys", M_Menu_Controls_f);
	Cmd_AddCommand("help", M_Menu_Help_f);
	Cmd_AddCommand("intro", M_Intro_f);
	Cmd_AddCommand("menu_demo", M_Demo_f);
	Cmd_AddCommand("menu_quit", M_Menu_Quit_f);

	return 1;
}

void M_Draw(void)
{
	if(m_state==m_none || key_dest!=key_menu)	return;

	switch(m_state)
	{
	case m_txtpages:
		M_TxtPages_Draw();
		return;
	case m_inter:
		M_Inter_Draw();
		break;
	case m_intro:
		M_Intro_Draw();
		break;
	case m_main:
		M_Main_Draw();
		break;
	case m_newg:
		M_NewGame_Draw();
		break;
	case m_episode:
		M_Episode_Draw();
		break;
	case m_skill:
		M_Skill_Draw();
		break;
	case m_controls:
		M_Controls_Draw();
		break;
	case m_load:
		M_Load_Draw();
		break;
	case m_save:
		M_Save_Draw();
		break;
	case m_scores:
		M_Scores_Draw();
		break;
	case m_video:
		M_Video_Draw();
		break;
	case m_sound:
		M_Sound_Draw();
		break;
	case m_quit:
		M_Quit_Draw();
		break;
	case m_dead:
		M_Dead_Draw();
		break;
	}
}

void M_Keydown(int key)
{
	switch(m_state)
	{
	case m_none:
		return;
	case m_txtpages:
		M_TxtPages_Key(key);
		return;
	case m_inter:
		M_Inter_Key(key);
		return;
	case m_intro:
		M_Intro_Key(key);
		return;
	case m_main:
		M_Main_Key(key);
		return;
	case m_newg:
		M_NewGame_Key(key);
		return;
	case m_episode:
		M_Episode_Key(key);
		return;
	case m_skill:
		M_Skill_Key(key);
		return;
	case m_controls:
		M_Controls_Key(key);
		return;
	case m_load:
		M_Load_Key(key);
		return;
	case m_save:
		M_Save_Key(key);
		return;
	case m_scores:
		M_Scores_Key(key);
		return;
	case m_video:
		M_Video_Key(key);
		return;
	case m_sound:
		M_Sound_Key(key);
		return;
	case m_quit:
		M_Quit_Key(key);
		return;
	case m_dead:
		M_Dead_Key(key);
		return;
	}
}
