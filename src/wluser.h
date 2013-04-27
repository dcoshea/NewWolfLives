/*
void KeyDown(int KeyCode, HWND hWnd)
{
	KeyPressed=1;
	LastKey=KeyCode;

	if(Con_KeyInput(KeyCode)) return;
	switch(KeyCode)
	{
	case VK_UP:  // Up
		key_press[KBD_FWD]=1;
		break;
	case VK_DOWN:  // Down
		key_press[KBD_BCK]=1;
		break;
	case VK_LEFT:  // Left
		key_press[KBD_LFT]=1;
		break;
	case VK_RIGHT:  // Right
		key_press[KBD_RGT]=1;
		break;
	case VK_SPACE:
		key_press[KBD_USE]=1;
		break;
	case 0x53:		// S
		key_press[KBD_STL]=1;
		break;
	case 0x46:		// F
		key_press[KBD_STR]=1;
		break;
	case 0x45:		// E
		key_press[KBD_FWD_ALT]=1;
		break;
	case 0x44:		// D
		key_press[KBD_BCK_ALT]=1;
		break;
	case 0x31:		// 1
		if (Player.weapons & WEAPONS_KNIFE)
		{
			Player.curweapon=Player.chsweapon=WEAPON_KNIFE;
			Player.weapframe=0;
		}
		else
		{
			Msg_Printf("No weapon");
		}				
		break;
	case 0x32:		// 2
		if (Player.weapons & WEAPONS_PISTOL)
		{
			Player.curweapon=Player.chsweapon=WEAPON_PISTOL;
			Player.weapframe=0;
		}
		else
		{
			Msg_Printf("No weapon");
		}				
		break;
	case 0x33:		// 3
		if (Player.weapons & WEAPONS_AUTO)
		{
			Player.curweapon=Player.chsweapon=WEAPON_AUTO;
			Player.weapframe=0;
		}
		else
		{
			Msg_Printf("No weapon");
		}				
		break;
	case 0x34:		// 4
		if (Player.weapons & WEAPONS_CHAIN)
		{
			Player.curweapon=Player.chsweapon=WEAPON_CHAIN;
			Player.weapframe=0;
		}
		else
		{
			Msg_Printf("No weapon");
		}				
		break;
	case 0x11:	//	Ctrl
		key_press[KBD_ATTACK]=1;
		break;
	case 0xC0:  //	tilde
		Con_ToggleConsole_f();
		break;
	case VK_ESCAPE:
		if(!menu)
		{
			RGBAcolor c0=RGBA_WLMNU, c1=RGBA_WLMCU, c2=RGBA_WLMCS;
			LastKey=0;
			mnu_current=&mnu_main;
			mnu_current->current=0;
			mnu_main.item[4].color1=c0;
			mnu_main.item[4].enabled=1;
			strcpy(mnu_main.item[6].name, "End Game");
			mnu_main.item[7].enabled=1;
			mnu_main.item[7].color1=c1;
			mnu_main.item[7].color2=c2;
			strcpy(mnu_main.item[7].name, "Back to Game");
			mnu_main.item[7].fn_notify=mnuBackGFN;
			menu=1;
			action=0;
			virtualreality=0;
			menufull=1;
		}
		break;
	case 0x71: // F2 Save Menu
		if(!menu)
		{
			LastKey=0;
			GetSaveGamesInfo();
			mnu_current=&mnu_file;
			fileio=1;
			menu=1;
			action=0;
			virtualreality=0;
			menufull=0;
		}
		break;
	case 0x72: // F3 Load Menu
		if(!menu)
		{
			LastKey=0;
			GetSaveGamesInfo();
			mnu_current=&mnu_file;
			fileio=2;
			menu=1;
			action=0;
			virtualreality=0;
			menufull=0;
		}
		break;
	case 0x73: // F4 Video Oprions Menu
		if(!menu)
		{
			LastKey=0;
			mnu_current=&mnu_vopt;
			menu=1;
			action=0;
			virtualreality=0;
			menufull=0;
		}
		break;
	case 0x77: // F8 (quick save)
		SaveGame(0, "quicksave");
		break;
	case 0x78: // F9 (quick load)
		LoadGame(0);
		break;
	case 0x7B: // F12 (screenshot)
		Vid_ScreenShot_f();
		break;
	case 0x09:	// Tab
		map=!map;
		break;
	default:
		Msg_Printf("Undefined Key (ScanCode=%d (0x%02X))", KeyCode, KeyCode);
		break;
	}
}

void KeyUp(int KeyCode)
{
	switch(KeyCode)
	{
	case VK_UP:  // Up
		key_press[KBD_FWD]=0;
		break;
	case VK_DOWN:  // Down
		key_press[KBD_BCK]=0;
		break;
	case VK_LEFT:  // Left
		key_press[KBD_LFT]=0;
		break;
	case VK_RIGHT:  // Right
		key_press[KBD_RGT]=0;
		break;
	case VK_SPACE:
		key_press[KBD_USE]=0;
		break;
	case 0x53:		// S
		key_press[KBD_STL]=0;
		break;
	case 0x46:		// F
		key_press[KBD_STR]=0;
		break;
	case 0x45:		// E
		key_press[KBD_FWD_ALT]=0;
		break;
	case 0x44:		// D
		key_press[KBD_BCK_ALT]=0;
		break;
	case 0x11:	//	Ctrl
		key_press[KBD_ATTACK]=0;
		break;
	}
}
*/

