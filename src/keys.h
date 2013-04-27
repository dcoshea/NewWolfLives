#include "common/keys.h"

typedef enum {key_game, key_console, key_message, key_menu} keydest_t;

extern keydest_t	key_dest;
extern char *keybindings[256];
extern	int	 key_repeats[256];
extern	int	 key_count;			// incremented every key event
extern	int	 key_lastpress;

extern void Key_Event(int key, bool down);
extern void Key_Init(void);
extern void Key_WriteBindings(FILE *f);
extern void Key_SetBinding(int keynum, char *binding);
extern void Key_ClearStates(void);
extern char *Key_KeynumToString(int keynum);
extern void Key_FindKeysForCommand(char *command, int *twokeys);
extern void Key_UnbindCommand(char *command);
extern void Key_ClearStates(void);
