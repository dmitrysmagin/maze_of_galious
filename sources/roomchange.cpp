#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "sound.h"
#include "MOGtypes.h"

//#include "drawing.h"
#include "bitmaps.h"
#include "sprites.h"
#include "tiles.h"

#include "mog.h"


/* Gráficos: */ 
extern Bitmap *konami_bmp,*menu_bmp,*tiles_bmp,*enemy_bmp;
extern int n_tiles;
extern CTile **tiles;

extern BYTE *col_buffer;
extern BYTE *buffer_screen;

/* Estado: */ 
extern char keyboard[256];
extern int STATE,SUBSTATE;
extern int cycle;

/* Juego: */ 
extern int inter_screen;
extern int map;			/* En que mundo está el personaje			*/ 
extern int map_x,map_y;	/* En que habitación dentro del mundo		*/ 
extern int pers_x,pers_y;	/* Posición del personaje en la habitación	*/ 
extern int pers_pos;
extern bool pers_right;	/* Hacia donde mira el personaje			*/ 
extern int pers_state;		/* Estado en el que se encuentra el personaje	*/ 
extern int pers_substate;	/* Variable de estado secundaria.			*/ 
extern int pers_walk_state;	/* Variable que controla el caminar.	*/ 
extern int jump_inertia;
extern bool sword;				/* Hay que dibujar la espada?	*/ 
extern bool old_sword;
extern int sword_x,sword_y;	/* Coordenadas de la espada.	*/ 
extern int sword_time;
extern int in_ladder;			/* En qué escalera está el personaje	*/ 
extern bool previous_x_collision;	/* Hubo una colisión en X en el frame anterior? */ 
extern bool previous_y_collision;	/* Hubo una colisión en Y en el frame anterior? */ 
extern int hit_time;	/* Contador para saber cuando pueden dañarnos de nuevo	*/ 
extern int character;
extern int freezed;
extern bool live_character[2];
extern int current_weapon;
extern int n_fired_arrows,n_fired_mines;
extern int world3_room06_counter;
extern int world5_room44_state,world5_room44_state2;
extern int room26_state,room56_state;
extern int world10_door_x,world10_door_y;
extern int stone_hit_counter;
extern bool jumping_from_elevator;

extern int in_door;
extern int to_enter_cut;

extern bool room_demonroom,old_room_demonroom;
extern int fighting_demon;
extern int room_appearwall,room_appearwall_x,room_appearwall_y;
extern int appearwall_record_x[MAX_AW_RECORD];
extern int appearwall_record_y[MAX_AW_RECORD];

extern int n_arrows,n_coins,n_keys;
extern int player_energy[2],player_experience[2],player_max[2];
extern int player_denergy[2],player_dexperience[2];
extern bool item[38];
extern bool world_item[10][4];
extern bool cross,world_key;
extern int next_world;
extern bool world_doors_open[10];
extern bool in_water;
extern int water_counter,lava_counter;
extern int lava_state;

extern int room_size_x,room_size_y;
extern int *room_tiles;

extern int wall_tile;
extern int stone_tile,ladder_tile,wdoor_tile,keydoor_tile,stats_tile;
extern int coin_tile,arrow_tile,key_tile,door_tile;
extern int character_tile[2],character_tile_inv[2];
extern int colision_tile;
extern int sword_tile[2],explosion_tile;
extern int worm_tile,bat_tile,skeleton_tile,smoke_tile,slime_tile;
extern int items_tile,armas_tile;

extern int worm_state_tile[28];
extern int worm_state_tile_i[28];
extern int worm_state_y[28];


/* Efectos de sonido: */ 
extern SOUNDT S_gamestart,S_jump,S_sword,S_swordhit;
extern SOUNDT S_item,S_ah,S_powerup,S_ladderdisapear;
extern SOUNDT S_death,S_entering;
extern SOUNDT S_enterwater;
extern SOUNDT S_waterhit,S_fall,S_shield,S_bell,S_lavahit;


/* Control de pantalla: */ 
extern int logic_dx[4],logic_dy[4],logic_x[4],logic_y[4];
extern int physic_dx[4],physic_dy[4],physic_x[4],physic_y[4];

extern int zoom;


/* Teclas: */ 
extern int UP_KEY,DOWN_KEY,LEFT_KEY,RIGHT_KEY;
extern int SWORD_KEY,WEAPON_KEY,ITEM_KEY,PAUSE_KEY;



void RoomChange(void) 
{
	int old_x=pers_x,old_y=pers_y;
	bool room_change=false;
	bool room_loaded=true;

	if (pers_x<TILE_SIZE_X) {
		if (fighting_demon==0 || fighting_demon==432) {
			if (map==4 && map_x==0 && map_y==4) {
				/* Salir del mundo 4: */ 
				map=0;
				map_x=0;
				map_y=8;
				pers_x=TILE_SIZE_X*9;
				pers_y=TILE_SIZE_Y*9;
				STATE=9;
				SUBSTATE=0;
				in_door=-1;
			} else {
				if (map==9 && map_x==0 && map_y==7) {
					/* Salir del mundo 9: */ 
					map=0;
					map_x=7;
					map_y=0;
					pers_x=TILE_SIZE_X*13;
					pers_y=TILE_SIZE_Y*5;
					STATE=9;
					SUBSTATE=0;
					in_door=-1;
				} else {
					if (map==10 && map_x==0 && map_y==0) {
						/* Salir del mundo 10: */ 
						map=0;
						map_x=world10_door_x;
						map_y=world10_door_y;
						if (map_x==1) pers_x=TILE_SIZE_X*13;
						if (map_x==5) pers_x=TILE_SIZE_X*21;
						if (map_x==9) pers_x=TILE_SIZE_X*9;
						if (map_x==14) pers_x=TILE_SIZE_X*21;
						if (map_x==1) pers_y=TILE_SIZE_X*17;
						if (map_x==5) pers_y=TILE_SIZE_X*9;
						if (map_x==9) pers_y=TILE_SIZE_X*17;
						if (map_x==14) pers_y=TILE_SIZE_X*1;
						STATE=9;
						SUBSTATE=0;
						in_door=-1;
					} else {
						if (map==3 && map_y==0) {
							pers_x=TILE_SIZE_X;
						} else {
							map_x--;
							HP_filter(map_x,map_y);
							room_loaded=loadroom(map,map_x,map_y);
							HP_restore(map_x,map_y);
							room_change=true;
							n_fired_arrows=0;
							n_fired_mines=0;
							pers_x=(room_size_x-2)*TILE_SIZE_X;
						} /* if */ 
					} /* if */ 
				} /* if */ 
			} /* if */ 
		} else {
			pers_x=TILE_SIZE_X;
		} /* if */ 
	} /* if */ 
	if (pers_x>(room_size_x-2)*TILE_SIZE_X) {
		if (fighting_demon==0 || fighting_demon==432) {
			if (map==10 && map_x==4 && next_world<11) {
				pers_x=(room_size_x-2)*TILE_SIZE_X;
			} else {
				if (map==5 && map_x==3 && map_y==0) {
					/* Salir del mundo 5: */ 
					map=0;
					map_x=8;
					map_y=8;
					pers_x=TILE_SIZE_X*9;
					pers_y=TILE_SIZE_Y*5;
					STATE=9;
					SUBSTATE=0;
					in_door=-1;
				} else {
					if (map==3 && map_y==0) {
						pers_x=(room_size_x-2)*TILE_SIZE_X;
					} else {
						map_x++;
						HP_filter(map_x,map_y);
						room_loaded=loadroom(map,map_x,map_y);
						HP_restore(map_x,map_y);
						room_change=true;
						n_fired_arrows=0;
						n_fired_mines=0;
						pers_x=TILE_SIZE_X;
					} /* if */ 
				} /* if */ 
			} /* if */ 
		} else {
			pers_x=(room_size_x-2)*TILE_SIZE_X;
		} /* if */ 
	} /* if */ 
	if (pers_y<-TILE_SIZE_Y) {
		if (fighting_demon==0 || fighting_demon==432) {
			if (pers_state==S_UP_LADDER || pers_state==S_QUIET ||
 				 pers_state==S_WALKING_LEFT || pers_state==S_WALKING_RIGHT ||
				 (pers_state==S_JUMPING && jumping_from_elevator)) {
				bool shrine=false;
				map_y--;
				if (map==1 && map_y==6 && map_x==0) {
					/* DEMETER en el mundo 1: */ 
					STATE=7;
					SUBSTATE=0;

					Sound_release_music();
					Sound_create_music("entering","shrine1","shrine2");
					shrine=true;
				} /* if */ 
				if (map==2 && map_y==1 && map_x==3) {
					/* ATHENA en el mundo 2: */ 
					STATE=7;
					SUBSTATE=0;

					Sound_release_music();
					Sound_create_music("entering","shrine1","shrine2");
					shrine=true;
				} /* if */ 
				if (map==3 && map_y==3 && map_x==3) {
					STATE=7;
					SUBSTATE=0;

					Sound_release_music();
					Sound_create_music("entering","shrine1","shrine2");
					shrine=true;
					/* ATHENA en el mundo 3: */ 
				} /* if */ 
				if (map==4 && map_y==1 && map_x==4) {
					STATE=7;
					SUBSTATE=0;

					Sound_release_music();
					Sound_create_music("entering","shrine1","shrine2");
					shrine=true;
					/* DEMETER en el mundo 4: */ 
				} /* if */ 
				if (map==3 && map_y==5 && map_x==0) {
					world3_room06_counter++;
					map_y=6;
				} /* if */ 
				if (map==5 && map_y==1 && map_x==5) {
					STATE=7;
					SUBSTATE=0;

					Sound_release_music();
					Sound_create_music("entering","shrine1","shrine2");
					shrine=true;
					/* DEMETER en el munedo 5 */ 
				} /* if */ 
				if (map==6 && map_y==3 && map_x==5) {
					STATE=7;
					SUBSTATE=0;

					Sound_release_music();
					Sound_create_music("entering","shrine1","shrine2");
					shrine=true;
					/* ATHENA en el munedo 6 */ 
				} /* if */ 
				if (map==7 && map_y==1 && map_x==6) {
					STATE=7;
					SUBSTATE=0;

					Sound_release_music();
					Sound_create_music("entering","shrine1","shrine2");
					shrine=true;
					/* ATHENA en el munedo 7 */ 
				} /* if */ 
				if (map==8 && map_y==1 && map_x==1) {
					STATE=7;
					SUBSTATE=0;

					Sound_release_music();
					Sound_create_music("entering","shrine1","shrine2");
					shrine=true;
					/* DEMETER en el munedo 8 */ 
				} /* if */ 
				if (map==9 && map_y==0 && map_x==2) {
					STATE=7;
					SUBSTATE=0;

					Sound_release_music();
					Sound_create_music("entering","shrine1","shrine2");
					shrine=true;
					/* DEMETER en el munedo 9 */ 
				} /* if */ 
				if (!shrine) {
					if (map_y>=0) {
						HP_filter(map_x,map_y);
						room_loaded=loadroom(map,map_x,map_y);
						HP_restore(map_x,map_y);
						room_change=true;
						n_fired_arrows=0;
						n_fired_mines=0;
						pers_y=(room_size_y-3)*TILE_SIZE_Y;
					} else {
						/* Se ha subido a una escalera que no tiene pantalla: */ 
						if (map==1 && map_x==2 && map_y==-1) {
							/* Salir del mundo 1: */ 
							map=0;
							map_x=3;
							map_y=12;
							pers_x=TILE_SIZE_X*9;
							pers_y=TILE_SIZE_Y*5;
							STATE=9;
							SUBSTATE=0;
							in_door=-1;
						} /* if */ 
						if (map==6 && map_x==3 && map_y==-1) {
							/* Salir del mundo 6: */ 
							map=0;
							map_x=14;
							map_y=12;
							pers_x=TILE_SIZE_X*21;
							pers_y=TILE_SIZE_Y*5;
							STATE=9;
							SUBSTATE=0;
							in_door=-1;
						} /* if */ 
						if (map==7 && map_x==3 && map_y==-1) {
							/* Salir del mundo 7: */ 
							map=0;
							map_x=5;
							map_y=2;
							pers_x=TILE_SIZE_X*5;
							pers_y=TILE_SIZE_Y*17;
							STATE=9;
							SUBSTATE=0;
							in_door=-1;
						} /* if */ 
						if (map==8 && map_x==3 && map_y==-1) {
							/* Salir del mundo 8: */ 
							map=0;
							map_x=4;
							map_y=4;
							pers_x=TILE_SIZE_X*13;
							pers_y=TILE_SIZE_Y*17;
							STATE=9;
							SUBSTATE=0;
							in_door=-1;
						} /* if */ 
					} /* if */ 
				} /* if */ 
			} else {
				previous_y_collision=true;
				pers_y=-TILE_SIZE_Y;
			} /* if */ 
		} else {
			pers_y=-TILE_SIZE_Y;
		} /* if */ 
	} /* if */ 
	if (pers_y>(room_size_y-2)*TILE_SIZE_Y) {
		map_y++;
		if (map!=2 || map_y<5) {
			if (map!=3 || map_y<9) {
				HP_filter(map_x,map_y);
				room_loaded=loadroom(map,map_x,map_y);
				HP_restore(map_x,map_y);
				room_change=true;
				n_fired_arrows=0;
				n_fired_mines=0;
				pers_y=-TILE_SIZE_Y;
				in_ladder=search_ladder(pers_x,pers_y);
			} else {
				/* Salir del mundo 3: */ 
				map=0;
				map_x=10;
				map_y=8;
				pers_x=TILE_SIZE_X*21;
				pers_y=TILE_SIZE_Y*5;
				STATE=9;
				SUBSTATE=0;
				in_door=-1;
			} /* if */ 
		} else {
			/* Salir del mundo 2: */ 
			map=0;
			map_x=10;
			map_y=11;
			pers_x=TILE_SIZE_X*25;
			pers_y=TILE_SIZE_Y*5;
			STATE=9;
			SUBSTATE=0;
			in_door=-1;
		} /* if */ 
	} /* if */ 

	if (room_change) {
		int i;

		stone_hit_counter=0;
		STATE=16;
		inter_screen=6;
		world5_room44_state=world5_room44_state2;
		room26_state=0;
		room56_state=0;
		clear_typed_word();

		for(i=0;i<MAX_AW_RECORD;i++) {
			if (appearwall_record_x[i]==map_x &&
				appearwall_record_y[i]==map_y) {

				room_tiles[room_appearwall_y*room_size_x+room_appearwall_x]=wall_tile;
				room_tiles[(room_appearwall_y+1)*room_size_x+room_appearwall_x]=wall_tile;
				room_tiles[(room_appearwall_y+2)*room_size_x+room_appearwall_x]=wall_tile;
				room_appearwall=-1;
			} /* if */ 
		} /* for */ 

		if (item[3] && map==5 && map_x>=5) {
			for(i=0;i<room_size_x*room_size_y;i++) {
				if (room_tiles[i]==142) room_tiles[i]=181;
			} /* for */ 
		} /* if */ 

		/* Campanas: */ 
		if (item[7] && world_key) {
			int wx=-10,wy=-10;
			switch(next_world) {
			case 1:wx=3;wy=12;
					break;
			case 2:wx=10;wy=11;
					break;
			case 3:wx=10;wy=8;
					break;
			case 4:wx=0;wy=8;
					break;
			case 5:wx=8;wy=8;
					break;
			case 6:wx=14;wy=12;
					break;
			case 7:wx=5;wy=2;
					break;
			case 8:wx=4;wy=4;
					break;
			case 9:wx=7;wy=0;
					break;
			case 10:wx=world10_door_x;
				    wy=world10_door_y;
					break;
			} /* switch */ 
			
			if ((abs(map_x-wx)+abs(map_y-wy))<=2) {
				Sound_play(S_bell);
			} /* if */ 
		} /* if */ 
		
		if (!old_room_demonroom && room_demonroom) {
			Sound_release_music();
			Sound_create_music("demonroom2","demonroom1",0);
		} /* if */ 
		if (old_room_demonroom && !room_demonroom && fighting_demon!=432) {
			char tmp[80];
			Sound_release_music();
			if (map==0) sprintf(tmp,"castle");
				   else sprintf(tmp,"world");
			Sound_create_music(tmp,0,0);
		} /* if */ 

		lava_state=0;

		if (!room_loaded) {
			if (map!=0) {
				STATE=9;
				SUBSTATE=0;
			} else {
				STATE=6;
			} /* if */ 
			map=0;
			map_x=5;
			map_y=9;
			pers_x=TILE_SIZE_X*16;
			pers_y=TILE_SIZE_Y*5;
			pers_state=0;
			pers_pos=0;
			HP_objs_reset();
			loadroom(map,map_x,map_y);
		} else {
			GameRoomEvents();
			if (pers_state==S_UP_LADDER) in_ladder=search_ladder(pers_x,pers_y+TILE_SIZE_Y);
			if (pers_state==S_DOWN_LADDER) in_ladder=search_ladder(pers_x,pers_y);
		} /* if */ 
	} /* if */ 


	/* Corregir la posición de la espada: */ 
	if (sword) {
		sword_x+=pers_x-old_x;
		sword_y+=pers_y-old_y;
	} /* if */ 
} /* Roomchange */ 

