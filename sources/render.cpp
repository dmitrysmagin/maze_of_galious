#include "string.h"
#include "math.h"
#include "MOGtypes.h"
#include "SDL.h"

#include "drawing.h"
#include "bitmaps.h"
#include "sprites.h"
#include "tiles.h"

#include "mog.h"

#define MAX_VIEWS	4

/* Vistas: */ 
int logic_dx[MAX_VIEWS] = {640, 0, 0, 0};
int logic_dy[MAX_VIEWS] = {400, 0, 0, 0};
int logic_x[MAX_VIEWS] = {0, 0, 0, 0};
int logic_y[MAX_VIEWS] = {0, 0, 0, 0};

int physic_dx[MAX_VIEWS] = {640, 0, 0, 0};
int physic_dy[MAX_VIEWS] = {400, 0, 0, 0};
int physic_x[MAX_VIEWS] = {0, 0, 0, 0};
int physic_y[MAX_VIEWS] = {0, 0, 0, 0};

/* Variables globales: */ 

extern int SCREEN_X;
extern int SCREEN_Y;
BYTE *buffer_screen = NULL;

extern int pers_x, pers_y;

extern int n_fired_arrows;

void Render(SDL_Surface *surface)
{
	int i;

	/* Reservar buffers: */ 
	if (buffer_screen == NULL) {
		buffer_screen = new BYTE[SCREEN_X * SCREEN_Y];
		if (buffer_screen == NULL) {
			return;
		}
		memset(buffer_screen, 0, SCREEN_X * SCREEN_Y);
	} /* if */ 

	/* Ejecutar un ciclo de juego: */ 
	GameCycle(buffer_screen, SCREEN_X, SCREEN_Y);

    // Bloquear la superficie primaria
	if (SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}
	
	// Transferir la imagen de 'buffer_screen' a la memoria de vÌdeo:
	for (i = 0;i < MAX_VIEWS; i++) {
	
		if (logic_dx[i] != 0) {
		
			BYTE *orig, *dest;
		    int y, x, inc_x, inc_y;
			int offsx, offsy;

			/* Vista activa: */ 
			if (logic_dx[i] == physic_dx[i] &&
				logic_dy[i] == physic_dy[i]) {

				for (y = 0;y < physic_dy[i]; y++) {
					orig = buffer_screen + logic_x[i] + (logic_y[i] + y) * SCREEN_X;
					dest = (BYTE *)surface->pixels + physic_x[i] + (physic_y[i] + y) * surface->pitch;
					memcpy(dest, orig, physic_dx[i]);
				} /* for */ 
			} else {
			
				inc_x = ((logic_dx[i] << 8) / physic_dx[i]);
				inc_y = ((logic_dy[i] << 8) / physic_dy[i]);
				offsy = logic_y[i] << 8;
				
				for (y = 0;y < physic_dy[i]; y++, offsy += inc_y) {
									
					orig = buffer_screen + (offsy >> 8) * SCREEN_X;
					dest = (BYTE *)surface->pixels + physic_x[i] + (physic_y[i] + y) * surface->pitch;
					offsx = logic_x[i] << 8;
					
					for (x = 0; x < physic_dx[i]; x++, offsx += inc_x) {
						dest[x] = orig[(offsx >> 8)];
					} /* for */ 
				} /* for */ 
			} /* if */ 
		} /* if */ 
	} /* if */ 
		
	// Desbloquear la superficie primaria
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}

} /* Render */ 



