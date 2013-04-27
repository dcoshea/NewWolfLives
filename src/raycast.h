
// marks
#define TRACE_MARK_MAP	1		// makrs traced area in 'AM_AutoMap.vis' array
// obstacle levels
#define TRACE_SIGHT			2		// player sight
#define TRACE_SIGHT_AI	4		// enemy sight
#define TRACE_BULLET		8		// bullet
#define TRACE_OBJECT		16	// object

#define TRACE_HIT_VERT	32	// vertical wall was hit
#define TRACE_HIT_DOOR	64	// door was hit
#define TRACE_HIT_PWALL	128	// pushwall was hit

typedef struct r_trace_s
{
	int x, y; // origin
	int a;		// trace angle
	int flags;
	byte *tile_vis; // should point to [64*64] array
} r_trace_t;

#define UPPERZCOORD	 0.6f
#define	LOWERZCOORD -0.6f

extern byte tile_visible[64*64]; // can player see this tile?

void R_RayCast(placeonplane_t viewport);
int R_CastRay(int x, int y, int angle);
void R_Trace(r_trace_t *trace);
