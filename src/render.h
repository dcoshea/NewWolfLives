void R_Init(void);

void R_DrawHUD(void);
void R_UpdateScreen(void);

void R_ResetFlash(void);
void R_BonusFlash(void);
void R_DamageFlash(int damage);

extern int r_fps;
extern int r_polys;

// if true, a crosshair is drawn
extern cvar_t *crosshair_enabled;
// outer radius of the crosshair
extern cvar_t *crosshair_outer_radius;
// inner radius of the crosshair (i.e. radius of the part where no line is
// drawn).  Note that if this is 0, and crosshair_alpha is less than 1.0, the
// crosshair will be less transparent in the middle due to overlapping.
extern cvar_t *crosshair_inner_radius;
// thickness of the crosshair lines, in pixels
extern cvar_t *crosshair_thickness;
// alpha (transparency) of the crosshair lines, in range 0.0-1.0
extern cvar_t *crosshair_alpha;
