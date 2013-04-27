#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"

byte tile_visible[64*64]; // can player see this tile?

// ------------------------- * Actual Casting * -------------------------

/*
** Main raycasting function! Will
** render view (tell what to render)
** for given pos, angle, fov.
** may be called recursively
** to create mirror & portal effects!
** --------- Takes -------------
** viewport: position of the camera (player)
** --------- Sets --------------
**
** Platform Independent. Please no
** gl- code here!
*/
void R_RayCast(placeonplane_t viewport)
{
	int n, x, y, angle, vx, vy;
	bool p_wall=false;
	r_trace_t trace;

	memset(tile_visible, 0, sizeof(tile_visible)); // clear tile visible flags

// viewport tile coordinates
	x=viewport.origin[0];
	y=viewport.origin[1];
	angle=viewport.angle;
	vx=POS2TILE(viewport.origin[0]);
	vy=POS2TILE(viewport.origin[1]);

	trace.tile_vis=tile_visible;
	trace.flags=TRACE_SIGHT|TRACE_MARK_MAP;

// ------------------------- * Raycasting * -------------------------
/*
** let's cast some rays!
**  would mark all visible tiles in tile_visible array
** FIXME: control ray count and make angle init
*/
	for(n=0; n<XRES; n++) 
	{
		trace.x=x;
		trace.y=y;
		trace.a=NormalizeAngle(angle+ColumnAngle[n]);

		R_Trace(&trace);
	}

// ------------------------- * Rendering * -------------------------
	for(x=0; x<64; x++)
		for(y=0; y<64; y++)
	if(tile_visible[x+y*64])
	{
		if(CurMapData.tile_info[x][y]&TILE_IS_DOOR)
		{
		/* door */
			if(DoorMap[x][y]->action!=dr_open)
			{
				bool backside=false;

				if(DoorMap[x][y]->vertical)
				{
					if(x<vx) backside=true;
				}
				else
				{
					if(y<vy) backside=true;
				}
				R_Draw_Door(x, y, LOWERZCOORD, UPPERZCOORD, DoorMap[x][y]->vertical, backside, DoorMap[x][y]->texture, Door_Opened(x, y));
			}
		/* door sides */
			if(DoorMap[x][y]->vertical)
			{
				if(y<=vy)
					R_Draw_Wall((float)x, (float)(y-1), LOWERZCOORD, UPPERZCOORD, dir4_north, TEX_PLATE);
				if(y>=vy)
					R_Draw_Wall((float)x, (float)(y+1), LOWERZCOORD, UPPERZCOORD, dir4_south, TEX_PLATE);

				if(x<=vx && CurMapData.tile_info[x-1][y]&TILE_IS_WALL)
					R_Draw_Wall((float)(x-1), (float)y, LOWERZCOORD, UPPERZCOORD, dir4_east, CurMapData.wall_tex_x[x-1][y]);
				if(x>=vx && CurMapData.tile_info[x+1][y]&TILE_IS_WALL)
					R_Draw_Wall((float)(x+1), (float)y, LOWERZCOORD, UPPERZCOORD, dir4_west, CurMapData.wall_tex_x[x+1][y]);
			}
			else
			{
				if(x<=vx)
					R_Draw_Wall((float)(x-1), (float)y, LOWERZCOORD, UPPERZCOORD, dir4_east, TEX_PLATE+1);
				if(x>=vx)
					R_Draw_Wall((float)(x+1), (float)y, LOWERZCOORD, UPPERZCOORD, dir4_west, TEX_PLATE+1);

				if(y<=vy && CurMapData.tile_info[x][y-1]&TILE_IS_WALL)
					R_Draw_Wall((float)x, (float)(y-1), LOWERZCOORD, UPPERZCOORD, dir4_north, CurMapData.wall_tex_y[x][y-1]);
				if(y>=vy && CurMapData.tile_info[x][y+1]&TILE_IS_WALL)
					R_Draw_Wall((float)x, (float)(y+1), LOWERZCOORD, UPPERZCOORD, dir4_south, CurMapData.wall_tex_y[x][y+1]);
			}
		}
		else
		{
			/* P-Wall */
			if(CurMapData.tile_info[x][y]&TILE_IS_PWALL && !p_wall)
			{
				float dx, dy;

				dx=PWall.dx*PWall.PWpointsmoved/128.0f;
				dy=PWall.dy*PWall.PWpointsmoved/128.0f;

				if(PWall.x<=vx)
					R_Draw_Wall((float)PWall.x+dx, (float)PWall.y+dy, LOWERZCOORD, UPPERZCOORD, dir4_east, PWall.tex_x);
				if(PWall.x>=vx)
					R_Draw_Wall((float)PWall.x+dx, (float)PWall.y+dy, LOWERZCOORD, UPPERZCOORD, dir4_west, PWall.tex_x);
				if(PWall.y<=vy)
					R_Draw_Wall((float)PWall.x+dx, (float)PWall.y+dy, LOWERZCOORD, UPPERZCOORD, dir4_north, PWall.tex_y);
				if(PWall.y>=vy)
					R_Draw_Wall((float)PWall.x+dx, (float)PWall.y+dy, LOWERZCOORD, UPPERZCOORD, dir4_south, PWall.tex_y);
				p_wall=true;
			}
			/* x-wall */
			if(x<=vx && CurMapData.tile_info[x-1][y]&TILE_IS_WALL)
				R_Draw_Wall((float)(x-1), (float)y, LOWERZCOORD, UPPERZCOORD, dir4_east, CurMapData.wall_tex_x[x-1][y]);
			if(x>=vx && CurMapData.tile_info[x+1][y]&TILE_IS_WALL)
				R_Draw_Wall((float)(x+1), (float)y, LOWERZCOORD, UPPERZCOORD, dir4_west, CurMapData.wall_tex_x[x+1][y]);
			/* y-wall */
			if(y<=vy && CurMapData.tile_info[x][y-1]&TILE_IS_WALL)
				R_Draw_Wall((float)x, (float)(y-1), LOWERZCOORD, UPPERZCOORD, dir4_north, CurMapData.wall_tex_y[x][y-1]);
			if(y>=vy && CurMapData.tile_info[x][y+1]&TILE_IS_WALL)
				R_Draw_Wall((float)x, (float)(y+1), LOWERZCOORD, UPPERZCOORD, dir4_south, CurMapData.wall_tex_y[x][y+1]);
		}
		/* floor/ceiling */
		R_Draw_FlatSolid(x, y, LOWERZCOORD, true, &CurMapData.floor);
		R_Draw_FlatSolid(x, y, UPPERZCOORD, false, &CurMapData.celing);
	}
}

// ------------------------- * TRACE * -------------------------
int x_tile_step[4]={1, -1, -1,  1};
int y_tile_step[4]={1,  1, -1, -1};

/*
** R_TraceCheck
**
** x, y are in tiles
*/
bool R_TraceCheck(int x, int y, int frac, int dfrac, bool vert, bool flip, r_trace_t *trace)
{
	if(trace->flags&TRACE_MARK_MAP)
		AM_AutoMap.vis[x][y]=true; // mark automap if required

	if(CurMapData.tile_info[x][y]&TILE_IS_WALL)
	{
		if(vert)
		{
			trace->x=(x<<TILESHIFT)+(flip?TILEGLOBAL:0);
			trace->y=(y<<TILESHIFT)+frac;
			trace->flags|=TRACE_HIT_VERT;
		}
		else
		{
			trace->x=(x<<TILESHIFT)+frac;
			trace->y=(y<<TILESHIFT)+(flip?TILEGLOBAL:0);
			trace->flags&=~TRACE_HIT_VERT;
		}
		return true; // wall, stop tracing
	}

	if(trace->tile_vis)
		trace->tile_vis[x+y*64]=true; // this tile is visible
/*
	if(CurMapData.tile_info[x][y]&TILE_IS_PWALL)
	{
		if(vert)
		{
			switch(PWall.dir)
			{
			case dir4_north:
				if((frac>>7)<PWall.PWpointsmoved)
					return false;
				break;
			case dir4_south:
				if(128-(frac>>7)>PWall.PWpointsmoved)
					return false;
				break;
			case dir4_east:
			case dir4_west:
				
			}
		}
		else
		{
			switch(PWall.dir)
			{
			
			
			}
		}
	}*/

	if(CurMapData.tile_info[x][y]&TILE_IS_DOOR && DoorMap[x][y]->action!=dr_open)
	{
		frac+=dfrac>>1;
		if(POS2TILE(frac)) return false;

		if(vert)
		{
			if(DoorMap[x][y]->action!=dr_closed && (frac>>10)>DOOR_FULLOPEN-Door_Opened(x, y))
				return false; // opened enough
			trace->x=TILE2POS(x);
			trace->y=(y<<TILESHIFT)+frac;
			trace->flags|=TRACE_HIT_VERT;
		}
		else
		{
			if(DoorMap[x][y]->action!=dr_closed && (frac>>10)<Door_Opened(x, y))
				return false; // opened enough
			trace->y=TILE2POS(y);
			trace->x=(x<<TILESHIFT)+frac;
			trace->flags&=~TRACE_HIT_VERT;
		}
		trace->flags|=TRACE_HIT_DOOR;
		return true; // closed door, stop tracing
	}

	return false; // no intersection, go on!
}

void R_Trace(r_trace_t *trace)
{
	int xtilestep, ytilestep;
	int xstep, ystep;
	int xtile, ytile;
	int xintercept, yintercept;
	int YmapPos, XmapPos;
	quadrant q;

// Setup for raycasting
	q=GetQuadrant(trace->a);
	xtilestep=x_tile_step[q];
	ytilestep=y_tile_step[q];
	xtile=POS2TILE(trace->x)+xtilestep;
	ytile=POS2TILE(trace->y)+ytilestep;
	xstep=ytilestep*XnextTable[trace->a];
	ystep=xtilestep*YnextTable[trace->a];
	xintercept=(int)( ( ((ytilestep==-1 ? ytile+1 : ytile)<<TILESHIFT)-trace->y )/TanTable[trace->a])+trace->x;
	yintercept=(int)( ( ((xtilestep==-1 ? xtile+1 : xtile)<<TILESHIFT)-trace->x )*TanTable[trace->a])+trace->y;
	YmapPos=yintercept>>TILESHIFT; // toXray
	XmapPos=xintercept>>TILESHIFT; // toYray

	if(trace->tile_vis) trace->tile_vis[POS2TILE(trace->x)+POS2TILE(trace->y)*64]=true; // this tile is visible
	if(trace->flags&TRACE_MARK_MAP) AM_AutoMap.vis[POS2TILE(trace->x)][POS2TILE(trace->y)]=true;

// ------------------------- * Start of raycasting * -------------------------

// ------------- * CORE LOOP * ------------------
// weird, but better then Carmack's self modifying stuff
	while(1)
	{
// --- vertical loop // an anologue for X-Ray
		while(!(ytilestep==-1 && YmapPos<=ytile) && !(ytilestep==1 && YmapPos>=ytile))
		{
			if(xtile<0 || xtile>=64 || YmapPos<0 || YmapPos>=64) return; // FIXME: do we need this?

		// call check fuction!
			if(R_TraceCheck(xtile, YmapPos, yintercept%TILEGLOBAL, ystep, true, xtilestep==-1, trace))
				return;

		// prepare for next step
			xtile+=xtilestep;
			yintercept+=ystep;
			YmapPos=yintercept>>TILESHIFT;
		}

// --- horizontal loop // an anologue for Y-Ray
		while(!(xtilestep==-1 && XmapPos<=xtile) && !(xtilestep==1 && XmapPos>=xtile))
		{
			if(ytile<0 || ytile>=64 || XmapPos<0 || XmapPos>=64) return; // FIXME: do we need this?

		// call check fuction!
			if(R_TraceCheck(XmapPos, ytile, xintercept%TILEGLOBAL, xstep, false, ytilestep==-1, trace))
				return;

		// prepare for next step
			ytile+=ytilestep;
			xintercept+=xstep;
			XmapPos=xintercept>>TILESHIFT;
		}
	}
}
