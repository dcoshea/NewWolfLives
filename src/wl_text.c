/*************************************************************************
** NewWolf
** Copyright (C) 1999-2002 by DarkOne
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**************************************************************************
** Wolf 'Rich Text'
*************************************************************************/
#include <windows.h>
#include "WolfDef.h"

/*
** TEXT FORMATTING COMMANDS
** ------------------------
** ;													Comment
** ^P[enter]									Start new page, must be the first chars in a layout
** ^E[enter]									End of layout (all pages)
** ^C<hex digit>							Change text color (index in Wolf Palette)
** ^B<y>,<x>,<w>,<h>[enter]		Draw a box (background color) [x, y]-[x+w, y+h]
** ^G<y>,<x>,<pic>[enter]			Draw a graphic on x, y and push margins (pic is index in vgagraph)
** ^T<y>,<x>,<pic>,<t>[enter]	Same as above, but after <t> tics
** ^L<x>,<y>[ENTER]						Locate to a specific spot, x in pixels, y in lines
** ^>													Draw text on right half on screen
*/

// ------------------------- * constants * -------------------------
#define BACKCOLOR				0x11

#define WORDLIMIT				80 // max word len
#define FONTHEIGHT			24 // font height ...
#define TOPMARGIN				38
#define BOTTOMMARGIN		77
#define LEFTMARGIN			32
#define RIGHTMARGIN			32
#define PICMARGIN				16 // ???
#define TEXTROWS				((480-TOPMARGIN-BOTTOMMARGIN)/FONTHEIGHT)
#define	SPACEWIDTH			14
#define SCREENPIXWIDTH	640
#define SCREENMID				(SCREENPIXWIDTH/2)

// ------------------------- * variables * -------------------------
char *txtsrc=NULL, *text; // pointer to the start of the text
int txtsize;							// size of article
int numpages;							// number of pages
// variables in use
int	pagenum, rowon;
int leftmargin[TEXTROWS], rightmargin[TEXTROWS];
int px, py, fontcolor;		// print position & color!
bool layoutdone;

// ------------------------- * !DarkOne's Edition! * -------------------------

// ------------------------- * AUX * -------------------------

/*
** Text_RipToEOL
**
** scan to end of line
*/
void Text_RipToEOL(void)
{
	while(*text++!='\n');
}

/*
** Text_ParseNumber
**
** get number from stream /will catch 0x## hex also/
*/
int	Text_ParseNumber(void)
{
	int number;

// scan until a number is found
	while((text[0]<'0' || text[0]>'9') && !(text[0]=='0' && (text[1]=='x' || text[1]=='X')))
		text++;

// get number
	number=Q_atoi(text);

// scan past number
	if(text[0]=='0' && (text[1]=='x' || text[1]=='X'))
	{
		text+=2;
		while((*text>='0' && *text<='9') ||
					(*text>='a' && *text<='f') ||
					(*text>='A' && *text<='F')) text++;
	}
	else
		while(*text>='0' && *text<='9') text++;
	
	return number;
}

/*
** Text_ParseHexNumber
**
** get hex number ## from the stream
*/
int Text_ParseHexNumber(void)
{
	int c, num=0;

	c=toupper(*text++);
	if(c>='0' && c<='9')
		num=c-'0';
	else if(c>='A' && c<='F')
		num=c-'A'+10;
	
	num*=16;
	c=toupper(*text++);
	if(c>='0' && c<='9')
		num+=c-'0';
	else if(c>='A' && c<='F')
		num+=c-'A'+10;

	return num;
}

/*
** Text_BackPage
**
** Scans for a previous ^P
*/
void Text_BackPage(void)
{
	pagenum--;
	while(1)
	{
		text--;
		if(text[0]=='^' && toupper(text[1])=='P') break;
	}
}

/*
** NextPage
**
** Scans for a next ^P
*/
void Text_NextPage(void)
{
	pagenum++;
	while(1)
	{
		text++;
		if(text[0]=='^' && toupper(text[1])=='P') return;
	}
}

/*
** Text_NewLine
*/
void Text_NewLine(void)
{
	char ch;

	if(++rowon==TEXTROWS)
	{
	// overflowed the page, so skip until next page break
		layoutdone=true;
		while(1)
		{
			if(text[0]=='^')
			{
				ch=toupper(text[1]);
				if(ch=='E' || ch=='P')
				{
					layoutdone=true;
					return;
				}
			}
			text++;
		}
	}
	px =leftmargin[rowon];
	py+=FONTHEIGHT;
}

/*
** Text_HandleCtrls
*/
void Text_HandleCtrls(void)
{
	if(*text++=='\n')
	{
		Text_NewLine();
		return;
	}
}

/*
** Text_HandleCommand
*/
void Text_HandleCommand(void)
{
	int	n, margin, top, bottom;
	int	x, y, w, h, picmid;
	int picnum, delay;
	cache_pic *pdata;

	switch(toupper(*text++))
	{
	case 'B':		// box
		y=VID_NEW_X(Text_ParseNumber());
		x=VID_NEW_Y(Text_ParseNumber());
		w=VID_NEW_X(Text_ParseNumber());
		h=VID_NEW_Y(Text_ParseNumber());
		Vid_DrawBox(x, y, w, h, RGBA_READB);
		Text_RipToEOL();
		break;
	case ';':		// comment
		Text_RipToEOL();
		break;
	case 'P':		// ^P is start of next page, ^E is end of file
	case 'E':
		layoutdone=true;
		text-=2; // back up to the '^'
		break;
	case 'C':		// ^c<hex digit> changes text color
		fontcolor=Text_ParseHexNumber();
		FNT_SetColor(RGBA_WOLFCOLOR(fontcolor));
		break;
	case '>':
		px=320;
		text++;
		break;
	case 'L':
		py=Text_ParseNumber();
		rowon=(py-TOPMARGIN)/FONTHEIGHT;
		py=TOPMARGIN+rowon*FONTHEIGHT;
		px=VID_NEW_X(Text_ParseNumber());
		Text_RipToEOL();
		break;
	case 'T':		// ^Tyyy,xxx,ppp,ttt waits ttt tics, then draws pic
		y=VID_NEW_Y(Text_ParseNumber());
		x=VID_NEW_X(Text_ParseNumber());
		picnum=Text_ParseNumber();
		delay=Text_ParseNumber();
		Text_RipToEOL();

	// FIXME: wait specified amount of time
		Vid_DrawPicEx(x&~15, y, picnum);
		break;
	case 'G':		// ^Gyyy,xxx,ppp draws graphic
		y=VID_NEW_Y(Text_ParseNumber());
		x=VID_NEW_X(Text_ParseNumber());
		picnum=Text_ParseNumber();
		Text_RipToEOL();

		Vid_DrawPicEx(x&~15, y, picnum);
		pdata=Vid_CachePicEx(picnum);
		w=pdata->width;
		h=pdata->height;

	// adjust margins
		picmid=x+w/2;
		if(picmid>SCREENMID)
			margin=x-PICMARGIN;		// new right margin
		else
			margin=x+w+PICMARGIN;	// new left margin

		top=(y-TOPMARGIN)/FONTHEIGHT;
		if(top<0) top=0;
		bottom=(y+h-TOPMARGIN)/FONTHEIGHT;
		if(bottom>=TEXTROWS) bottom=TEXTROWS-1;

		for(n=top; n<=bottom; n++)
			if(picmid>SCREENMID)
				rightmargin[n]=margin;
			else
				leftmargin[n]=margin;

		if(px<leftmargin[rowon]) px=leftmargin[rowon]; // adjust this line if needed
		break;
	}
}

/*
** HandleWord
*/
void Text_HandleWord(void)
{
	char curword[WORDLIMIT];
	int wwidth, wheight, n;
	unsigned long tmp;

// copy the next word into [word]
	n=0;
	while(*text>32)
	{
		curword[n]=*text++;
		if(++n==WORDLIMIT)
		{
			Con_Printf("PageLayout: Word limit exceeded\n");
			break;
		}
	}
	curword[n]=0; // stick a null at end for C

	// see if it fits on this line
	tmp=FNT_GetSize(curword);
	wwidth =unpackshort1(tmp);
	wheight=unpackshort2(tmp);

	while(px+wwidth>rightmargin[rowon])
	{
		Text_NewLine();
		if(layoutdone) return; // overflowed page
	}

// print it
	FNT_SetColor(RGBA_WOLFCOLOR(fontcolor));
	FNT_Print(px, py, curword);
	px+=wwidth;

	// suck up any extra spaces
	while(*text==' ')
	{
		px+=SPACEWIDTH;
		text++;
	}
}

// ------------------------- * Main * -------------------------

/*
** Text_ScanArticle
**
** Scans an entire layout file (until a ^E) marking all graphics used, and
** counting pages, then caches the graphics in
*/
void Text_ScanArticle(bool cache)
{
	int picnum;

	numpages=0;
	if(cache)
	{
		Vid_CachePic(H_TOPWINDOWPIC);
		Vid_CachePic(H_LEFTWINDOWPIC);
		Vid_CachePic(H_RIGHTWINDOWPIC);
		Vid_CachePic(H_BOTTOMINFOPIC);
	}
	else
	{
		Vid_UnCachePic(H_TOPWINDOWPIC);
		Vid_UnCachePic(H_LEFTWINDOWPIC);
		Vid_UnCachePic(H_RIGHTWINDOWPIC);
		Vid_UnCachePic(H_BOTTOMINFOPIC);
	}

	do
	{
		if(*text++=='^')
			switch(toupper(*text++))
			{
			case 'P':		// start of a page
				numpages++;
				break;
			case 'E':		// end of file
				text=txtsrc;
				return;
		// Graphics commands: Cache picture
			case 'G':		// draw graphic command
			case 'T':		// timed draw graphic command
				Text_ParseNumber();
				Text_ParseNumber();
				picnum=Text_ParseNumber();
				Text_RipToEOL();
				if(cache)
					Vid_CachePicEx(picnum);
				else
					Vid_UnCachePic(picnum);
				break;
			}
	} while((text-txtsrc)<txtsize);

	text=txtsrc;
	Con_Printf("Text_ScanArticle: No ^E to terminate file!\n");
}

/*
** Text_NewArticle
**
** this function will be called to indicate that new help screen
** need to be displayed, so zero all old data & cache all data
*/
void Text_NewArticle(char *src, int size)
{
/* DEBUG
	FILE *fp;

	fp=fopen("e:\\rrr.txt", "wb");
	fwrite(src, 1, size, fp);
	fclose(fp);
*/

// malloc & copy text data
	if(txtsrc) free(txtsrc);
	txtsrc=malloc(size);
	memcpy(txtsrc, src, size);
	text=txtsrc; txtsize=size;
// Scan Article to cache graphics and count pages
	Text_ScanArticle(true); // cache
// Init vars
	pagenum=1; fontcolor=0;
// Setup Font
	FNT_SetFont(FNT_WOLF_SMALL);
	FNT_SetStyle(0, 0, 0);
	FNT_SetScale(1, 1);
	FNT_SetColor(RGBA_BLACK);
}

/*
** Text_FreeArticle
**
** called there when user presses Esc
*/
void Text_FreeArticle(void)
{
	if(!txtsrc) return;

	Text_ScanArticle(false); // uncache
	free(txtsrc);
	text=txtsrc=NULL;
}

/*
** Text_DisplayPage
**
** Clears the screen, draws the pics on the page, and word wraps the text.
** Returns a pointer to the terminating command
*/
void Text_DisplayPage(void)
{
	char *pagestart, ch;
	int n;

	if(text==NULL) return;
	pagestart=text;	// save

// clear the screen
	Vid_DrawBox(0, 0, 640, 480, RGBA_READB);
	Vid_DrawPic(0, 0, H_TOPWINDOWPIC);
	Vid_DrawPic(0, 19, H_LEFTWINDOWPIC);
	Vid_DrawPic(624, 19, H_RIGHTWINDOWPIC);
	Vid_DrawPic(16, 422, H_BOTTOMINFOPIC);

// select font
	FNT_SetFont(FNT_WOLF_SMALL);
	FNT_SetStyle(0, 0, 0);
	FNT_SetScale(1, 1);
	FNT_SetColor(RGBA_WOLFCOLOR(fontcolor));

// init
	for(n=0; n<TEXTROWS; n++)
	{
		leftmargin[n]=LEFTMARGIN;
		rightmargin[n]=SCREENPIXWIDTH-RIGHTMARGIN;
	}
	rowon=0; layoutdone=false;
	px=LEFTMARGIN; py=TOPMARGIN;

// make sure we are starting layout text (^P first command)
	while(*text<=32) text++;

	if(text[0]!='^' || toupper(text[1])!='P')
	{
		// Exit Text Display!
		Con_Printf("Text_DisplayPage: Text not headed with ^P\n");
		return;
	}
	Text_RipToEOL();

// process text stream
	do
	{
		ch=*text;

		if(ch=='^')
		{
			text++;
			Text_HandleCommand();
		}
		else if(ch==9)
		{
		 px=(px+16)&~15;
		 text++;
		}
		else if(ch<=32)
			Text_HandleCtrls();
		else
			Text_HandleWord();
	} while(!layoutdone);

// Page Numbers!
	FNT_SetFont(FNT_WOLF_SMALL);
	FNT_SetStyle(0, 0, 0);
	FNT_SetScale(1, 1);

	FNT_SetColor(0x40, 0x40, 0x00, 0xFF); // 0x4F
	FNT_Printf(426, 439, "pg %d of %d", pagenum, numpages);
	FNT_SetColor(RGBA_BLACK);
// End Drawing

	text=pagestart; // restore
}

/*
** Text_Input
**
** recieves keycode, returns 1 if we are finished!
*/
int Text_Input(int key)
{
	switch(key)
	{
	case K_ESCAPE:	// Exit
		Text_FreeArticle();
		return 1;
		break;
	case K_UPARROW:	// Prev Page
	case K_PGUP:
	case K_LEFTARROW:
		if(pagenum>1)
			Text_BackPage();
		break;
	case K_ENTER:		// Next Page
	case K_DOWNARROW:
	case K_PGDN:
	case K_RIGHTARROW:
		if(pagenum<numpages)
			Text_NextPage();
		break;
	}
	return 0;
}
