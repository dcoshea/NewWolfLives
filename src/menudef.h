// Used only by menu.c! Types, defines, etc!
// ------------------------- * Menu Related defines! * -------------------------
#define MNU_SIMPLE 1
#define MNU_SELECT 2
#define MNU_NUMBER 3

// ------------------------- * Types: * -------------------------
typedef struct
{
	int min;
	int max;
	int step;
	int value;
} mnuNumData;

typedef struct
{
	char num;
	char item[10][20]; // 10 items, 19 chars each
	char selected;
} mnuSelData;

typedef struct
{
	char type;
	char name[50];
	void *data;
	int (*fn_notify) (void);
	unsigned char enabled;
	RGBAcolor color1, color2;
} menuEntry;

typedef struct
{
	short x, y, w, h;
} menuWindow;

typedef struct
{
	unsigned char num;
	menuEntry *item;
	char current; // when (--)may go below zero!, so signed
	int cursor_x;
	int left, left2, top;
	unsigned char Width, Height;
	char font;
	menuWindow window;
	int (*fn_draw) (void);
	int (*fn_exit) (void); // called when Esc is pressed
} Wolf_Menu;
