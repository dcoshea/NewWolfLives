typedef struct
{
	bool active;
	int PWtilesmoved;
	int PWpointsmoved;
	dir4type dir;
	int x, y;
	int dx, dy;
	int tex_x, tex_y;
} Pwall_t;

extern Pwall_t PWall;

void PW_Reset(void);
void PW_Save(FILE *fp);
void PW_Load(FILE *fp);

bool PW_Push(int x, int y, dir4type dir);
void PW_Process(void);
