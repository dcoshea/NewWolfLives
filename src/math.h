// Angle Direction Types & LUTs (Hard Coded! Please do not mess them)
typedef enum {q_first, q_second, q_third, q_fourth} quadrant;
typedef enum {dir4_east, dir4_north, dir4_west, dir4_south,	dir4_nodir}	dir4type;
typedef enum {dir8_east, dir8_northeast, dir8_north, dir8_northwest, dir8_west,
							dir8_southwest,	dir8_south,	dir8_southeast,	dir8_nodir} dir8type;

extern char dx4dir[5], dy4dir[5], dx8dir[9], dy8dir[9];
extern dir4type opposite4[5], dir4d[3][3];
extern dir8type opposite8[9], dir4to8[5], diagonal[9][9];
extern int dir8angle[9], dir4angle[5];
// ------------------------- * Vectors * -------------------------
// Vectors & angles for 3D-Space
typedef long vec_t;
typedef struct{vec_t x, y, z;} vec3_s;
typedef struct{vec_t x, y;} vec2_s;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
/* pitch: up <-> down
	 yaw: left <-> right
	 roll: lean (fall over) */
#define	PITCH	0
#define	YAW		1
#define	ROLL	2
typedef long ang_t;
typedef struct{ang_t pitch, yaw, roll;} ang3_s;
typedef ang_t ang3_t[3];
typedef struct
{
	vec2_t origin;
	ang_t angle;
} placeonplane_t;
typedef struct
{
	vec3_t origin;
	ang3_t angle;
} palceinspace_t;
#define DotProduct(x,y) (x[0]*y[0]+x[1]*y[1]+x[2]*y[2])
#define VectorSubtract(a,b,c) {c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];}
#define VectorAdd(a,b,c) {c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];}
#define VectorCopy(a,b) {b[0]=a[0];b[1]=a[1];b[2]=a[2];}
// ------------------------- * Some Macroses * -------------------------

#define pack4bytes(b1, b2, b3, b4) ((b1)+((b2)<<8)+((b3)<<16)+((b4)<<24))
#define unpackbyte1(dw) ((dw)&0xFF)
#define unpackbyte2(dw) ((dw>>8)&0xFF)
#define unpackbyte3(dw) ((dw>>16)&0xFF)
#define unpackbyte4(dw) (dw>>24)

#define pack2shorts(s1, s2) ((s1)+((s2)<<16))
#define unpackshort1(dw)    ((dw)&0xFFFF)
#define unpackshort2(dw)    ((dw)>>16)

#define words2dword(l, h) ((l)+((h)<<16))
#define lword(dw) ((dw)&0xFFFF)
#define hword(dw) ((dw)>>16)

#define min_of_2(a, b) ((a)<(b)?(a):(b))
#define max_of_2(a, b) ((a)>(b)?(a):(b))

#define SIGN(x) ((x)>0?1:-1)
#define ABS(x) 	((int)(x)>0?(x):-(x))
#define LABS(x) ((long)(x)>0?(x):-(x))

// Transform old coord's for 2D (320x200) to new 2D (640x480)
#define VID_NEW_X(x) ((x)*2)
#define VID_NEW_Y(y) (((y)*12)/5)

#define TILE2POS(a) (((a)<<TILESHIFT)+HALFTILE)
#define POS2TILE(a)  ((a)>>TILESHIFT)
#define POS2TILEf(a)  ((a)/FLOATTILE)
// ------------------------- * vvv FINE angles vvv * -------------------------
#define ASTEP			0.0078125f		// 1 FINE=x DEGREES
#define ASTEPRAD	0.000136354f	// 1 FINE=x RADIANS
#define ANG_1RAD	7333.8598			// 1 RADIAN=x FINES
#define ANG_0			0       //(int)((float)0/ASTEP)
#define ANG_1			128			//(int)((float)1/ASTEP)
#define ANG_6			768     //(int)((float)6/ASTEP)
#define ANG_15		1920		//(int)((float)15/ASTEP)
#define ANG_22_5	2880		//(int)((float)22.5/ASTEP)
#define ANG_30		3840    //(int)((float)30/ASTEP)
#define ANG_45		5760    //(int)((float)45/ASTEP)
#define ANG_67_5	8640		//(int)((float)67.5/ASTEP)
#define ANG_90		11520   //(int)((float)90/ASTEP)
#define ANG_112_5	14400		//(int)((float)112.5/ASTEP)
#define ANG_135		17280   //(int)((float)135/ASTEP)
#define ANG_157_5	20160		//(int)((float)157.5/ASTEP)
#define ANG_180		23040   //(int)((float)180/ASTEP)
#define ANG_202_5	25920		//(int)((float)202.5/ASTEP)
#define ANG_225		28800   //(int)((float)225/ASTEP)
#define ANG_247_5	31680		//(int)((float)247.5/ASTEP)
#define ANG_270		34560	  //(int)((float)270/ASTEP)
#define ANG_292_5	37440		//(int)((float)292.5/ASTEP)
#define ANG_315		40320	  //(int)((float)225/ASTEP)
#define ANG_337_5 43200   //(int)((float)337.5/ASTEP)
#define ANG_360		46080   //(int)((float)360/ASTEP)
// ------------------------- * ^^^ FINE angles ^^^ * -------------------------

// Borland Style math constants rounded for 21 decimals
#ifndef M_PI
#define M_E					2.71828182845904523536
#define M_LOG2E			1.44269504088896340736
#define M_LOG10E		0.434294481903251827651
#define M_LN2				0.693147180559945309417
#define M_LN10			2.30258509299404568402
#define M_PI				3.14159265358979323846
#define M_PI_2			1.57079632679489661923
#define M_PI_4			0.785398163397448309116
#define M_1_PI			0.318309886183790671538
#define M_2_PI			0.636619772367581343076
#define M_1_SQRTPI	0.564189583547756286948
#define M_2_SQRTPI	1.12837916709551257390
#define M_SQRT2			1.41421356237309504880
#define M_SQRT_2		0.707106781186547524401
#endif

#define DEG2RAD(a)  (((a)*M_PI )/180.0)
#define RAD2DEG(a)  (((a)*180.0)/M_PI)
#define FINE2RAD(a) (((a)*M_PI )/ANG_180)
#define RAD2FINE(a) (((a)*ANG_180)/M_PI)
#define FINE2DEG(a) ((a)/ANG_1)
#define FINE2DEGf(a) ((a)/(float)ANG_1)
#define DEG2FINE(a) ((a)*ANG_1)

extern double SinTable[], *CosTable, TanTable[ANG_360+1];
extern int XnextTable[ANG_360+1], YnextTable[ANG_360+1];
extern int ColumnAngle[XRES];

int Math_Init(void);
double TanDgr(double x);
double SinDgr(double x);
double CosDgr(double x);
double ArcTanDgr(double x);
double ArcSinDgr(double x);
double ArcCosDgr(double x);

int AngDiff(int alpha, int beta);
int WiseAngDiff(int alpha, int beta);
int NormalizeAngle(int angle);
float CalcFov(float fov_x, float width, float height);
quadrant GetQuadrant(int angle);
dir4type Get4dir(int angle);
dir8type Get8dir(int angle);

int Point2LineDist(int x, int y, int a);
int LineLen2Point(int x, int y, int a);
int Point2PointDist(int x1, int y1, int x2, int y2);
int advAng(vec2_t point1, vec2_t point2);

int TransformPoint(int x, int y);

int GreatestCommonDivisor(int i1, int i2);

__inline float lerp(float t, float a, float b)
{return (1-t)*a+t*b;}

// Texture Resizing
void ResampleTexture(unsigned char *in, int inwidth, int inheight, unsigned char *out,  int outwidth, int outheight, int bits);

// --- CRC ---
void CRC_Init(unsigned short *crcvalue);
void CRC_ProcessByte(unsigned short *crcvalue, byte data);
unsigned short CRC_Value(unsigned short crcvalue);
unsigned short CRC_Block(byte *start, int count);

// --- Random Numbers ---
void US_InitRndT(bool randomize);
void US_InitRnd(void);
int US_RndT(void);
int US_Rnd(void);
float rnd(void);
