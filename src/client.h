void CL_SendCmd(usercmd_t *cmd);

// cl_input
typedef struct kbutton_s
{
	int down[2];	// key nums holding it down
	int state;		// low bit is down state
} kbutton_t;

// cvars
extern cvar_t *cl_name, *cl_color;
extern cvar_t *lookstrafe, *allwaysrun;
extern cvar_t *m_yaw, *m_side;

extern kbutton_t in_left, in_right, in_forward, in_back;
extern kbutton_t in_moveleft, in_moveright;
extern kbutton_t in_strafe, in_speed, in_use, in_attack;

extern void CL_InitInput(void);
extern float CL_KeyState(kbutton_t *key);
void CL_BaseMove(usercmd_t *cmd);

void CL_Init(void);

