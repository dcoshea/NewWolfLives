#define NUM_TEXTURES 32

// this type must be in all video implementations
// not used outside of video (only as ref), so may contain anything
typedef struct
{
	int name;
	short width, height; // scaled /ready to draw/
	unsigned int glTex; // for OpenGL
} cache_pic;

typedef unsigned int GLuint;

extern float maxAniso;

extern int  GL_Init(void);
extern void GL_ShutDown(void);

extern void GL_SwitchTo3DState(placeonplane_t viewport);
extern void GL_Restore2DState(void);
extern void GL_CheckFOV(void);

extern cache_pic *Vid_CachePic(int name);
extern cache_pic *Vid_CachePicEx(int name);
extern void Vid_UnCachePic(int name);
extern void Vid_UnCacheAllPics(void);
