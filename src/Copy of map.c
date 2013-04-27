/*
Raycasting, tile checks, door checks, etc
2001 by DarkOne
*/
#include <windows.h>
#include <stdio.h>
#include "wolfdef.h"

/*
=====================
= Map_CheckLine
=
= Returns true if a straight line
= between 2 points is unobstructed,
= false otherwise
=====================
*/
bool Map_CheckLine(int x1, int y1, int x2, int y2)
{
	int	xt1, yt1, xt2, yt2; // tile positions
	int	x, y; // current point in !tiles!
	int	xdist, ydist, xstep, ystep; // destancees & steps in !tiles!
	int	xfrac, yfrac, deltafrac; // current point in !1/256 of tile!
	int	partial; // how much to move in our direction to border
	int intercept;

// get start & end tiles
	xt1=x1>>TILESHIFT; yt1=y1>>TILESHIFT;
	xt2=x2>>TILESHIFT; yt2=y2>>TILESHIFT;

	xdist=abs(xt2-xt1); // X distance in tiles
	ydist=abs(yt2-yt1); // Y distance in tiles

// 1/256 tile precision (TILESHIST is 16)
	x1>>=8; y1>>=8;
	x2>>=8; y2>>=8;


	if(xdist) // allways positive check only for 0
	{
		if(xt2>xt1)
		{
			partial=256-(x1&0xff);
			xstep=1;
		}
		else
		{
			partial=x1&0xff;
			xstep=-1;
		}

		deltafrac=abs(x2-x1);
		ystep=((y2-y1)<<8)/deltafrac; // P.S.: ltemp only on 16-bit compilers!
		yfrac=y1+((ystep*partial)>>8);

		x=xt1+xstep;
		xt2+=xstep;
		do
		{
			y=yfrac>>8;
			yfrac+=ystep;

			if(CurMapData.tile_info[x][y]&TILE_IS_WALL)	return false; // Wall is in path quitting!
			if(CurMapData.tile_info[x][y]&TILE_IS_DOOR)
			{// door, see if the door is open enough
				if(DoorMap[x][y]->action!=dr_open)
				{
					if(DoorMap[x][y]->action==dr_closed) return false;
					// checking vertical doors in action: ->_I_
					intercept=((yfrac-ystep/2)&0xFF)>>4; // 1/64 of tile
					if(intercept < (63 - DoorMap[x][y]->tickcount) ) return false;
				}
			}
			x+=xstep;
		} while(x!=xt2);
	}

	if(ydist) // allways positive check only for 0
	{
		if(yt2>yt1)
		{
			partial=256-(y1&0xff);
			ystep=1;
		}
		else
		{
			partial=y1&0xff;
			ystep=-1;
		}

		deltafrac=abs(y2-y1);
		xstep=((x2-x1)<<8)/deltafrac;
		xfrac=x1+((xstep*partial)>>8);

		y=yt1+ystep;
		yt2+=ystep;
		do
		{
			x=xfrac>>8;
			xfrac+=xstep;

			if(CurMapData.tile_info[x][y]&TILE_IS_WALL)	return false; // Wall is in path quitting!
			if(CurMapData.tile_info[x][y]&TILE_IS_DOOR)
			{// door, see if the door is open enough
				if(DoorMap[x][y]->action!=dr_open)
				{
					if(DoorMap[x][y]->action==dr_closed) return false;
					// checking vertical doors in action: ->_I_
					intercept=((xfrac-xstep/2)&0xFF)>>4; // 1/64 of tile
					if(intercept < DoorMap[x][y]->tickcount ) return false;
				}
			}
			y+=ystep;
		} while(y!=yt2);
	}
	return true;
}
