#define FNT_WOLF_SMALL	0
#define FNT_WOLF_BIG		1
#define FNT_CONSOLE			2

void FNT_LoadWolfFont(int fnt, int page);
void FNT_LoadImgFont(char *filename, int fnt);

extern int FNT_Init(void);
extern void FNT_ShutDown(void);

extern void FNT_SetFont(int font);
extern void FNT_SetStyle(int italic, int shadow, int gradient);
extern void FNT_SetColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A);
extern void FNT_SetRGBAcolor(RGBAcolor *c);
extern void FNT_SetColor2(unsigned char R, unsigned char G, unsigned char B, unsigned char A);
extern void FNT_SetRGBAcolor2(RGBAcolor *c);
extern void FNT_SetColorSh(unsigned char R, unsigned char G, unsigned char B, unsigned char A);
extern void FNT_SetRGBAcolorSh(RGBAcolor *c);
extern void FNT_SetScale(int numerator, int divisor);
void FNT_AllowFMT(bool allow);

extern void FNT_Print(int x, int y, char *text);
extern void FNT_PrintPos(int xl, int yl, char *text);
extern void FNT_PrintPosOff(int xl, int yl, int ox, int oy, char *text);

extern void FNT_Printf(int x, int y, char *fmt, ...);
extern void FNT_PrintfPos(int xl, int yl, char *fmt, ...);
extern void FNT_PrintfPosOff(int xl, int yl, int ox, int oy, char *fmt, ...);

extern void FNT_PrintC(int y, int x_s, int x_w, char *msg);

extern unsigned long FNT_GetPos(int xl, int yl);
extern unsigned long FNT_GetSize(char *text);
