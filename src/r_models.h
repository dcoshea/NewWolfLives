// r_models.h

typedef struct bbox_s
{
	float zmin, zmax;
	float xmin, xmax;
	float ymin, ymax;
} bbox_t;

typedef struct model_s
{
	int name;
	int *glCmds;
	int numFrames;
	int frameSize;
	byte *frames;
	bbox_t bbox; // bounding box for the whole model

	texture_t skin;
} model_t;

typedef enum mdl_allign_e {ALLIGN_NO, ALLIGN_CEIL, ALLIGN_FLOOR} mdl_allign_t;

bool MD2_Load(char *filename, char *skin, model_t *mdl);
void MD2_UnLoad(model_t *mdl);
void MD2_Render(model_t *mdl, int frame, int align, float base);
void MD2_RenderLerp(model_t *mdl, int frame1, int frame2, float t, int align, float base);

extern model_t mdl_flag;
