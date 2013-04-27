// Font Manager
#include <windows.h>
#include <stdio.h>
#include <GL\gl.h>
#include "WolfDef.h"

#define MAX_FONTS 3

typedef struct font_s
{
	char height;
	char width[256];
	float left[256], right[256];
	float top[256], bottom[256];
	texture_t tex;
} font_t;

typedef struct fstyle_s
{
	int italic;
	int shadow;
	int gradient;
	RGBAcolor color1, color2, cshadow; // normal, gradient, shaddow
	int numerator, divisor;
	bool allow_fmt;
} fstyle_t;

typedef struct
{
	short height;
	short location[256];
	char width[256];
} fontstruct;

fstyle_t sFNT;
font_t fonts[MAX_FONTS], *curFNT;

// ------------------------- * Init & ShutDown * -------------------------

/*
** FNT_Init
**
** font subsystem initialization
** called after GLinit, Con_Init, File_Init & CMD, cvar init
*/
int FNT_Init(void)
{
	Con_Printf("FNT_Init: Initailizing font subsystem:\n");
	Con_Printf("-> Total Fonts: %d\n", MAX_FONTS);
	FNT_LoadWolfFont(FNT_WOLF_SMALL, 1);
	FNT_LoadWolfFont(FNT_WOLF_BIG, 2);
	Con_Printf("-> Total WolfFonts: 2\n");
	FNT_LoadImgFont("textures/ui/font.tga", FNT_CONSOLE);

	sFNT.color1.r=sFNT.color1.g=sFNT.color1.b=sFNT.color1.a=0xFF;
	sFNT.color2.r=sFNT.color2.g=sFNT.color2.b=sFNT.color2.a=0xFF;
	sFNT.cshadow.r=sFNT.cshadow.g=sFNT.cshadow.b=0x00; sFNT.cshadow.a=0xFF;
	sFNT.italic=sFNT.shadow=sFNT.gradient=0;
	sFNT.numerator=sFNT.divisor=1;
	sFNT.allow_fmt=false;

	curFNT=&fonts[0];
	return 1;
}

/*
** FNT_ShutDown
**
** font subsystem ShutDown
*/
void FNT_ShutDown(void)
{
	int n;

	for(n=0; n<MAX_FONTS; n++)
		Vid_UnLoadTexture(&fonts[n].tex);
}

// ------------------------- * Devider * -------------------------

/*
** FNT_LoadWolfFont
**
** Loads Wolfenstein font
*/
void FNT_LoadWolfFont(int fnt, int page)
{
	unsigned char *databuff, *data;
	long psize;
	fontstruct *header;
	font_t *font;
	int n;
	int curbyte;
	int outx, outy, x, y;

	if(fnt>MAX_FONTS || fnt <0) return;
	font=&fonts[fnt];
	memset(font, 0, sizeof(font_t)); // clear font data

	psize=File_VGA_GetChunkSize(page);
	if(!psize) return;
	databuff=malloc(psize);
	header=(fontstruct *)databuff;
	File_VGA_ReadChunk(page, databuff);

	font->tex.width=256;
	font->tex.bpp=4;
	font->tex.data=malloc(256*256*4);

	data=font->tex.data;
	for(x=0; x<256; x++)
		for(y=0; y<256; y++, data+=4)
	{
		data[0]=data[1]=data[2]=0xFF;
		data[3]=0x00;
	}

	font->height=VID_NEW_Y(header->height);

	data=font->tex.data;

// make first letter's symbol (square, like in windows)
// to map all letters, which do not have a symbol
	for(y=1; y<header->height-1; y++)
		data[(y*256+header->height/6)*4+3]=
		data[(y*256+header->height/3)*4+3]=0xFF;
	for(x=header->height/6; x<header->height/3; x++)
		data[(256+x)*4+3]=data[(256*(header->height-1)+x)*4+3]=0xFF;

	outx=header->height/2+1;
	outy=0;

	for(n=0; n<256; n++)
	{
		if(header->width[n] && n!=0)
		{ // have entry for this letter
			if(outx+header->width[n]>255)
			{
				outy+=header->height+1;
				outx=0;
			}
			curbyte=header->location[n];

			data=font->tex.data+(outy*256+outx)*4;
			for(y=0; y<header->height; y++, data+=256*4)
				for(x=0; x<header->width[n]; x++)
					if(databuff[curbyte++])	data[x*4+3]=0xFF;

			font->width[n] =VID_NEW_X(header->width[n]);
			font->left[n]  =outx/256.0f;
			font->right[n] =(outx+header->width[n])/256.0f;
			font->top[n]	 =(float)outy; // we'll devide it later by height!
			font->bottom[n]=(float)(outy+header->height);
			outx+=header->width[n]+1;
		}
		else
		{
			font->width[n] =font->height/2;
			font->left[n]  =0;
			font->right[n] =(header->height/2)/256.0f;
			font->top[n]   =0;
			font->bottom[n]=(float)(header->height);
		}
	}
	
	outy+=header->height;
	free(databuff);
	for(font->tex.height=1; font->tex.height<outy; font->tex.height<<=1);
	for(n=0; n<256; n++)
	{
		font->top[n]   /=font->tex.height;
		font->bottom[n]/=font->tex.height;
	}

	Vid_UploadTexture(&font->tex, false, false);
}

/*
** FNT_LoadImgFont
**
** Loads a font from image
*/
void FNT_LoadImgFont(char *filename, int fnt)
{
	font_t *font;
	int n;

	if(fnt>MAX_FONTS || fnt <0) return;
	font=&fonts[fnt];
	memset(font, 0, sizeof(font_t)); // clear font data

	font->height=14;
	for(n=0; n<256; n++)
	{
		font->width[n]=10;
		font->left[n]	 =(n%16)	*0.0625f+0.002f;
		font->right[n] =(n%16+1)*0.0625f-0.002f;
		font->top[n]	 =(n/16)	*0.0625f+0.002f;
		font->bottom[n]=(n/16+1)*0.0625f-0.002f;
	}

	Img_Read(filename, (image_t *)&font->tex);
	Vid_UploadTexture(&font->tex, true, false);
}

// ------------------------- * Devider * -------------------------

void FNT_SetFont(int font)
{
	if(font>=MAX_FONTS || font<0) return;
	curFNT=&fonts[font];
}

void FNT_SetStyle(int italic, int shadow, int gradient)
{
	sFNT.italic=italic;
	sFNT.shadow=shadow;
	sFNT.gradient=gradient;
}

void FNT_SetColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A)
{
	sFNT.color1.r=R;
	sFNT.color1.g=G;
	sFNT.color1.b=B;
	sFNT.color1.a=A;
}

void FNT_SetRGBAcolor(RGBAcolor *c)
{
	sFNT.color1=*c;
}

void FNT_SetColor2(unsigned char R, unsigned char G, unsigned char B, unsigned char A)
{
	sFNT.color2.r=R;
	sFNT.color2.g=G;
	sFNT.color2.b=B;
	sFNT.color2.a=A;
}

void FNT_SetRGBAcolor2(RGBAcolor *c)
{
	sFNT.color2=*c;
}

void FNT_SetColorSh(unsigned char R, unsigned char G, unsigned char B, unsigned char A)
{
	sFNT.cshadow.r=R;
	sFNT.cshadow.g=G;
	sFNT.cshadow.b=B;
	sFNT.cshadow.a=A;
}

void FNT_SetRGBAcolorSh(RGBAcolor *c)
{
	sFNT.cshadow=*c;
}

void FNT_SetScale(int numerator, int divisor)
{
	sFNT.numerator=numerator;
	sFNT.divisor=divisor;
}

void FNT_AllowFMT(bool allow)
{
	sFNT.allow_fmt=allow;
}

// ------------------------- * Printing * -------------------------
RGBcolor primecolors[8]=
{
	{0x00, 0x00, 0x00}, // 0. black
	{0xFF, 0x00, 0x00}, // 1. red
	{0x00, 0xFF, 0x00}, // 2. light green
	{0xFF, 0xFF, 0x00}, // 3. yellow
	{0x00, 0x00, 0xFF}, // 4. blue
	{0x00, 0xFF, 0xFF}, // 5. cyan
	{0xFF, 0x00, 0xFF}, // 6. light magenta
	{0xFF, 0xFF, 0xFF}  // 7. bright white
};

void FNT_Print(int x, int y, char *text)
{
	int cx, cy, sw, sh, c;
	RGBAcolor cur_color; // current color (since ^X can change it)

	cx=x; cy=y;
	sh=curFNT->height*sFNT.numerator/sFNT.divisor; // scaled height

	cur_color=sFNT.color1;
	glColor4ubv((byte *)&cur_color);
	glEnable(GL_BLEND);
	Vid_SelectTextureIndirect(&curFNT->tex);

	glBegin(GL_QUADS);
	while(c=*text++)
	{
		if(c=='\n')
		{
			cx=x;
			cy+=sh;
			continue;
		}
		if(c=='^' && sFNT.allow_fmt && *text>='0' && *text<='7')
		{
			if(sFNT.allow_fmt!=2) // just stip it out
			{
				memcpy(&cur_color, &primecolors[*text-'0'], sizeof(RGBcolor));
				glColor4ubv((byte *)&cur_color);
			}
			text++;
			continue;
		}
		
		// scaled width
		sw=(sFNT.numerator!=1 || sFNT.divisor!=1) ? curFNT->width[c]*sFNT.numerator/sFNT.divisor : curFNT->width[c];
		if(sFNT.shadow)
		{
			int shd;
			shd=sFNT.shadow;
			glColor4ubv((byte *)&sFNT.cshadow);
			glTexCoord2f(curFNT->right[c], curFNT->top[c]);		 glVertex2i(cx+sw+sFNT.italic+shd, cy+shd);
			glTexCoord2f(curFNT->left[c],	curFNT->top[c]);		 glVertex2i(cx+sFNT.italic+shd, cy+shd);
			glTexCoord2f(curFNT->left[c],	curFNT->bottom[c]);	 glVertex2i(cx+shd, cy+sh+shd);
			glTexCoord2f(curFNT->right[c], curFNT->bottom[c]); glVertex2i(cx+sw+shd, cy+sh+shd);
			glColor4ubv((byte *)&cur_color);
		}
		glTexCoord2f(curFNT->right[c], curFNT->top[c]);		 glVertex2i(cx+sw+sFNT.italic, cy);
		glTexCoord2f(curFNT->left[c],	curFNT->top[c]);		 glVertex2i(cx+sFNT.italic, cy);
		if(sFNT.gradient) glColor4ubv((byte *)&sFNT.color2);
		glTexCoord2f(curFNT->left[c],	curFNT->bottom[c]);	 glVertex2i(cx,	cy+sh);
		glTexCoord2f(curFNT->right[c], curFNT->bottom[c]); glVertex2i(cx+sw, cy+sh);
		if(sFNT.gradient && !sFNT.shadow) glColor4ubv((byte *)&cur_color);
		cx+=sw;
	}
	glEnd();
}

void FNT_PrintPos(int xl, int yl, char *text)
{
	FNT_Print(xl*curFNT->width[' ']*sFNT.numerator/sFNT.divisor, yl*curFNT->height*sFNT.numerator/sFNT.divisor, text);
}

void FNT_PrintPosOff(int xl, int yl, int ox, int oy, char *text)
{
	FNT_Print(ox+xl*curFNT->width[' ']*sFNT.numerator/sFNT.divisor, oy+yl*curFNT->height*sFNT.numerator/sFNT.divisor, text);
}

char msg[1024];

void FNT_Printf(int x, int y, char *fmt, ...)
{
	va_list	argptr;

	va_start(argptr,fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	FNT_Print(x, y, msg);
}

void FNT_PrintfPos(int xl, int yl, char *fmt, ...)
{
	va_list	argptr;

	va_start(argptr,fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	FNT_PrintPos(xl, yl, msg);
}

void FNT_PrintfPosOff(int xl, int yl, int ox, int oy, char *fmt, ...)
{
	va_list	argptr;

	va_start(argptr,fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	FNT_PrintPosOff(xl, yl, ox, oy, msg);
}

// y in pixels, x_s, x_w in pixels!
void FNT_PrintC(int y, int x_s, int x_w, char *msg)
{
	int w;

	w=unpackshort1(FNT_GetSize(msg));
	FNT_Print(x_s+(x_w-w)/2, y, msg);
}

// ------------------------- * Size Calculation * -------------------------
// returns width (x) in lword & height (y) in hword
unsigned long FNT_GetPos(int xl, int yl)
{
	return pack2shorts(xl*curFNT->width[' ']*sFNT.numerator/sFNT.divisor, yl*curFNT->height*sFNT.numerator/sFNT.divisor);
}

unsigned long FNT_GetSize(char *text)
{
	int h=0, w=0, mw=0, sh, sw;
	unsigned char c;

	sh=curFNT->height*sFNT.numerator/sFNT.divisor; // scaled height

	h=sh;
	while(c=*text++)
	{
		if(c=='\n')
		{
			if(w>mw) mw=w;
			w=0;
			h+=sh;
		}
		else
		{
			sw=(sFNT.numerator!=1 || sFNT.divisor!=1) ? curFNT->width[c]*sFNT.numerator/sFNT.divisor : curFNT->width[c];
			w+=sw;
		}
	}
	if(w>mw) mw=w;
	return pack2shorts(mw, h);
}
