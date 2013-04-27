
#define MAX_PARTICLES 4096

typedef enum ptype_e
{
	PART_BARE,		// simple /untextured/ particle
	PART_BLOOD,		// blood
	PART_BRICK		// brick piece
} ptype_t;

typedef struct particle_s
{
	struct particle_s *next;

	int ttl;				// TimeToLive in tics

	float org[3];		// Position in tiles /also may be fractional/
	float vel[3];		// Velocity in tiles per tick
	float mass;			// vertical spd is incremented by this value each tick
									// if negative particle would float in air
	RGBAcolor color;// particle color, alpha would fade out if ttl<0.5s
	ptype_t type;		// particle type
} particle_t;

extern particle_t *active_particles;

void PE_ProcessParticles(void);
void PE_Init(void);

void PE_fxWallShot(float x, float y, int angle, bool vertical);
void PE_fxBlood(float x, float y, int angle, int damage);
