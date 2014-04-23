#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "sound.h"
#include "MOGtypes.h"

#include "drawing.h"
#include "bitmaps.h"
#include "sprites.h"
#include "tiles.h"

#include "mog.h"
#include "object.h"


// FILE *fp2;

/* Gráficos: */ 
extern Bitmap *konami_bmp,*menu_bmp,*tiles_bmp,*enemy_bmp;
extern int n_tiles;
extern CTile **tiles;

extern BYTE *col_buffer;
extern BYTE *buffer_screen;

/* Estado: */ 
extern int STATE,SUBSTATE;
extern int cycle;

/* Juego: */ 
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
extern unsigned char old_keyboard[SDLK_LAST];
extern int in_ladder;			/* En qué escalera está el personaje	*/ 
extern bool previous_x_collision;	/* Hubo una colisión en X en el frame anterior? */ 
extern bool previous_y_collision;	/* Hubo una colisión en Y en el frame anterior? */ 
extern int hit_time;	/* Contador para saber cuando pueden dañarnos de nuevo	*/ 
extern int character;
extern int freezed;
extern bool live_character[2];
extern int current_weapon;
extern int n_fired_arrows;
extern int world3_room06_counter;
extern bool mine_stepped;
extern int stone_hit_counter;
extern int bible_counter,bible_subcounter;

extern int world10_door_x,world10_door_y;

extern int in_door;
extern int to_enter_cut;

extern bool room_demonroom,old_room_demonroom;
extern int fighting_demon;

extern int n_arrows,n_coins,n_keys;
extern int player_energy[2],player_experience[2],player_max[2];
extern int player_denergy[2],player_dexperience[2];
extern bool item[38];
extern bool world_item[10][4];
extern bool cross,world_key;
extern int next_world;
extern bool world_doors_open[10];
extern bool in_water,in_lava;
extern int water_counter,lava_counter;

extern int room_size_x,room_size_y;
extern int *room_tiles;
extern bool jumping_from_elevator,character_over_vertical_lever;


/* Objetos: (escaleras, piedras, etc...) */ 
extern MOGObject object[MAX_OBJECTS];
extern int n_objects;

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

/* Transportadores: */ 
extern int lever_room_x[MAX_LEVERS];
extern int lever_room_y[MAX_LEVERS];
extern int lever_x[MAX_LEVERS];
extern int lever_y[MAX_LEVERS];
extern int lever_drx[MAX_LEVERS][2];
extern int lever_dry[MAX_LEVERS][2];
extern int lever_dx[MAX_LEVERS][2];
extern int lever_dy[MAX_LEVERS][2];
extern int lever_going_to[MAX_LEVERS];

/* Efectos de sonido: */ 
extern SOUNDT S_gamestart,S_jump,S_sword,S_swordhit;
extern SOUNDT S_item,S_ah,S_ah2,S_powerup,S_ladderdisapear;
extern SOUNDT S_death,S_entering;
extern SOUNDT S_enterwater;
extern SOUNDT S_waterhit,S_fall,S_shield,S_bell,S_lavahit;
extern SOUNDT S_bible;


/* Control de pantalla: */ 
extern int logic_dx[4],logic_dy[4],logic_x[4],logic_y[4];
extern int physic_dx[4],physic_dy[4],physic_x[4],physic_y[4];

extern int zoom;


/* Teclas: */ 
extern SDLKey UP_KEY,DOWN_KEY,LEFT_KEY,RIGHT_KEY;
extern SDLKey SWORD_KEY,WEAPON_KEY,ITEM_KEY,PAUSE_KEY;




void GameInGameCycle(int dx,int dy)
{
	int i,j;
	int dist;
	int t_x;
	int val,inc;
	int next_x=pers_x;
	int next_y=pers_y;
	int old_x=pers_x;
	int old_y=pers_y;
	bool found;
	bool der_col,izq_col;
	bool escalera_up=false,escalera_down=false;
	int escalera_up_i=0,escalera_down_i=0;
	int colision;
	unsigned char *keyboard;
	bool invert_move=false;

	if (map==8 && !item[6]) {
		SDLKey tmp;
		invert_move=true;

		tmp=LEFT_KEY;
		LEFT_KEY=RIGHT_KEY;
		RIGHT_KEY=tmp;
	} /* if */ 

	SDL_PumpEvents();
	keyboard = (unsigned char *)SDL_GetKeyState(NULL);

	/* Mascara de colisión: */ 
	if (in_ladder==-1) colision=T_WALL|T_LADDER_WALL|T_DOOR_WALL;
				  else colision=T_WALL|T_DOOR_WALL;

	if (in_ladder==-1 && (pers_state==S_UP_LADDER || pers_state==S_DOWN_LADDER ||
						  pers_state==S_QUIET_LADDER)) {
		pers_state=S_QUIET;
		pers_pos=0;
	} /* if */ 

	/* Determinar si se está en posición de subir escalera: */ 
	if (pers_state==S_QUIET || pers_state==S_WALKING_RIGHT || pers_state==S_WALKING_LEFT) {
		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_LADDER) {
				t_x=object[i].x;
				if (pers_x>=t_x-4 && pers_x<=t_x+8) {
					if (pers_y>=((object[i].y-1*TILE_SIZE_Y)) && pers_y<=(object[i].y+(object[i].size-2)*TILE_SIZE_Y)) {
						escalera_up=true;
						escalera_up_i=i;
					} /* if */ 
					if (pers_y>=(object[i].y-3*TILE_SIZE_Y) && pers_y<=(object[i].y-2*TILE_SIZE_Y)) {
						escalera_down=true;
						escalera_down_i=i;
					} /* if */ 
				} /* if */ 
			} /* if */ 
		} /* for */ 
	} /* if */ 

	/* Determinar si se ha chocado con algún enemigo: */ 
	if (pers_state!=S_ENTERING_PASSAGE &&
		pers_state!=S_ENTERING_WORLD) {
		if (hit_time>0) hit_time--;
		if (pers_right) val=character_tile[character];
				   else val=character_tile_inv[character];
		if (pers_state!=S_HIT && 
			pers_state!=S_HIT_COLISION &&
			pers_state!=S_HIT_RECOVERY &&
			pers_state!=S_DEATH &&
			hit_time==0 &&
			(((tiles[val+pers_pos]->
				coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_ENEMY)!=0 &&
			(tiles[val+pers_pos]->
				coltest(GAME_VIEW_X+pers_x+TILE_UNIT,GAME_VIEW_Y+pers_y,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_ENEMY)!=0 &&
			(tiles[val+pers_pos]->
				coltest(GAME_VIEW_X+pers_x-TILE_UNIT,GAME_VIEW_Y+pers_y,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_ENEMY)!=0 &&
			(tiles[val+pers_pos]->
				coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y+TILE_UNIT,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_ENEMY)!=0 &&
			(tiles[val+pers_pos]->
				coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y-TILE_UNIT,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_ENEMY)!=0) ||
			mine_stepped
			)) {
			int bullet=0;
			int obj,oldobj;
			int xoffs=0;
			bool world_demon=false;

			if (!pers_right) xoffs=-2*TILE_UNIT;

			/* Colisión con enemigo!!!! */ 
			izq_col=((tiles[colision_tile]->
						coltest(GAME_VIEW_X+pers_x+xoffs,GAME_VIEW_Y+pers_y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_ENEMY)!=0);
			der_col=((tiles[colision_tile+1]->
						coltest(GAME_VIEW_X+pers_x+xoffs,GAME_VIEW_Y+pers_y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_ENEMY)!=0);
			if (!izq_col && !der_col) {
				izq_col=((tiles[colision_tile]->
							coltest(GAME_VIEW_X+pers_x+xoffs-2*TILE_UNIT,GAME_VIEW_Y+pers_y,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_ENEMY)!=0);
				der_col=((tiles[colision_tile+1]->
							coltest(GAME_VIEW_X+pers_x+xoffs+2*TILE_UNIT,GAME_VIEW_Y+pers_y,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_ENEMY)!=0);
			} /* if */ 

			val=2;
			dist=-1;
			obj=-1;
			bullet=0;
			for(i=0;i<n_objects;i++) {
				if (object[i].type==T_ENEMY) {
					j=abs(object[i].x-pers_x)+abs(object[i].y-pers_y);
						if (dist==-1 || j<dist) {
							oldobj=obj;
							bullet=0;
							val=2;
							obj=i;
							switch(object[i].enemy) {
							case T_BAT:val=2;
									   break;
							case T_WORM:val=2;
										break;
							case T_SKELETON:val=6;
										break;
							case T_SMOKE:val=2;
//										if (item[21]) obj=oldobj;
										break;
							case T_BOUNCINGBALL:val=2;
										break;
							case T_WATERMONSTER_ARM:val=4;
										break;
							case T_BUBBLE:val=2;
										bullet=2;
										break;
							case T_JUMPINGBUSH:val=2;
										break;
							case T_BLUESPIDER:val=2;
										break;
							case T_WHITEFIREBALL:val=2;
//										if (item[2]) obj=oldobj;
										break;
							case T_FSTONE:
							case T_RSTONE:
										val=6;
										break;
							case T_BLOB:
										val=2;
										break;
							case T_BAMBU:
										val=4;
//										if (item[10]) obj=oldobj;
										break;
							case T_PORCUPINE:
										val=4;
										break;
							case T_PORCUPINE_BULLET:
										bullet=1;
										val=4;
										break;
							case T_KNIGHT:val=6;
										break;
							case T_BLACK:val=4;
										break;
							case T_WITCH:val=2;
//										if (item[1]) obj=oldobj;
										break;
							case T_WHITEBEAR_BULLET:
										bullet=1;
										val=-1;
										break;
							case T_FEET:val=4;
										break;
							case T_REDJUMPER:val=4;
										break;
							case T_VENT2:val=2;
										break;
							case T_MEGABAT:val=6;
										break;
							case T_MEGABAT2:
							case T_MEGABAT3:
										val=2;
										break;
							case T_DEMON1:if (world_item[0][1]) val=4;
														   else val=8;
										world_demon=true;
										break;
							case T_DEMON1_BONES:val=2;
										bullet=3;
										world_demon=true;
										break;
							case T_DEMON1_BONES2:val=2;
										bullet=3;
										world_demon=true;
										break;
							case T_DEMON2:
							case T_DEMON2_HEAD:
										if (world_item[1][1]) val=12;
														   else val=24;
										world_demon=true;
										break;
							case T_DEMON2_BALLS:val=2;
										bullet=3;
										world_demon=true;
										break;
							case T_DEMON3:
										if (world_item[2][1]) val=24;
														 else val=48;
										world_demon=true;
										break;
							case T_DEMON3_FIRE:
										if (world_item[2][1]) val=12;
														 else val=24;
										world_demon=true;
										break;
							case T_LAVA1:
							case T_LAVA2:
							case T_LAVA3:val=2;
//										if (item[23]) obj=oldobj;	
										break;
							case T_PIRANHA:val=2;
										break;
							case T_KNIGHTHEAD:val=2;
										break;
							case T_CHICKEN:val=6;
										break;
							case T_CHICKEN_EGG:
										val=4;
										bullet=1;
										break;
							case T_EGG_EXPLOSION:val=4;
										break;
							case T_ROCKMAN:val=8;
										break;
							case T_CLOUD:val=2;
										break;
							case T_BFLY_SMALL:val=1;
										break;
							case T_BFLY_MEDIUM:val=2;
										break;
							case T_BFLY_LARGE:val=4;
										break;
							case T_BFLY_GIANT:val=8;
										break;
							case T_BFLY_BULLET:val=2;
										bullet=1;
										break;
							case T_DEMON4:
										if (world_item[3][1]) val=12;
														 else val=24;
										world_demon=true;
										break;
							case T_GHOST:val=4;
										break;
							case T_GHOST_BULLET:val=2;
										bullet=1;
										break;
							case T_WORM2:val=4;
										break;
							case T_OCTOPUS:val=6;
										break;
							case T_HAIRY:val=8;
										break;
							case T_HAIRYBULLET:
										val=2;
										bullet=2;
										break;
							case T_DEMON5:
										if (world_item[4][1]) val=16;
														 else val=32;
										world_demon=true;
										break;
							case T_DEMON5_FIRE:
										val=8;
										world_demon=true;
										break;
							case T_BIRD:val=4;
										break;
							case T_PACMAN:val=8;
										break;
							case T_STONEMAN:
										val=8;
										break;
							case T_STONEMAN_BULLET:
										val=4;
										bullet=1;
										break;
							case T_REDDRAGON:
										val=16;
										break;
							case T_REDDRAGON_BULLET:
										val=4;
										bullet=2;
										break;
							case T_DEMON6:
										if (world_item[5][1]) val=32;
														 else val=64;
										world_demon=true;
										break;
							case T_DEMON6_BULLET:
										val=8;
										bullet=3;
										world_demon=true;
										break;
							case T_OWL:	val=16;
										break;
							case T_GREENMONKEY:
										val=8;
										break;
							case T_PLANT:val=16;
										break;
							case T_PLANT_BULLET:
										val=4;
										bullet=2;
										break;
							case T_DEMON7:
										if (world_item[6][1]) val=24;
														 else val=48;
										world_demon=true;
										break;
							case T_DEMON7_BULLET:
										val=8;
										bullet=3;
										world_demon=true;
										break;
							case T_FLAME:
										val=12;
										break;
							case T_FLAME_BULLET:
										bullet=1;
										val=4;
										break;
							case T_TRANSFORMER:
										val=12;
										break;
							case T_TRANSF_BULLET:
										bullet=1;
										val=4;
										break;
							case T_WITCH2:
										val=20;
										break;
							case T_CYCLOPS:
										val=12;
										break;
							case T_CYCLOPS_BULLET:
										val=4;
										bullet=2;
										break;
							case T_DEMON8:
										if (world_item[7][1]) val=32;
														 else val=64;
										world_demon=true;
										break;
							case T_DEMON8_BULLET:
										val=10;
										bullet=3;
										world_demon=true;
										break;
							case T_SNAKE:
										val=12;
										break;
							case T_SNAKE_BULLET:
										val=4;
										bullet=1;
										break;
							case T_GORILLA:
										val=8;
										break;
							case T_GLIZARD:
										val=16;
										break;
							case T_GLIZARD_TONGUE:
										val=16;
										break;
							case T_BDEMON:
										val=20;
										break;
							case T_BDEMON_BULLET:
										val=8;
										bullet=2;
										break;
							case T_LCLOUD:
										val=6;
										break;
							case T_LIGHTNING:
										val=40;
										break;
							case T_LIGHTNING_FIRE:
										val=40;
										break;
							case T_DEMON9:
										if (world_item[8][1]) val=32;
														 else val=64;
										world_demon=true;
										break;
							case T_DEMON9_FIRE:
										if (world_item[8][1]) val=6;
														 else val=12;
										world_demon=true;
										break;
							case T_DEMON10:
										if (world_item[9][1]) val=44;
														 else val=88;
										world_demon=true;
										break;
							case T_DEMON10_BALL:
										if (world_item[9][1]) val=12;
														 else val=24;
										world_demon=true;
										break;
							case T_PAMPERSE_BALL:
										val=20;
										break;
							} /* switch */ 
							if (obj!=-1) dist=j;
						} /* if */ 
				} /* if */ 
			} /* for */ 

			if (!world_demon && item[12]) val/=2;

			if (bullet!=0 && !sword) {
				if (item[30] && bullet>1) bullet=0;
				if (item[31] && bullet>2) bullet=0;
				if (!item[27] && !item[30] && !item[31]) bullet=0;
				if (bullet!=0) {
					if (pers_right) {
						if (!der_col || izq_col) bullet=0;
					} else {
						if (der_col || !izq_col) bullet=0;
					} /* if */ 
				} /* if */ 
			} else {
				bullet=0;
			} /* if */ 

			if (obj!=-1 || mine_stepped) {
				if (bullet==0) {
					/* Tocado: */ 
					if (val==-1) {
						freezed=48;
						object[obj].type=T_NADA;
					} else {
						if (mine_stepped) {
							val=4;
							mine_stepped=false;
						} /* if */ 
						if (player_energy[character]<8) Sound_play(S_ah2);
												   else Sound_play(S_ah);
						player_energy[character]-=val;

						sword=false;
						// in_ladder=-1;
						if (pers_state==S_UP_LADDER ||
							pers_state==S_DOWN_LADDER ||
							pers_state==S_QUIET_LADDER ||
							pers_state==S_SWORD_LADDER) {
							pers_state=S_HIT_COLISION;
							pers_substate=0;
						} else {
							if (pers_state!=S_DOORPASS &&
								pers_state!=S_ENTERING_PASSAGE &&
								pers_state!=S_ENTERING_WORLD &&
								pers_state!=S_SLOWDOORPASS) {
							if (izq_col && !der_col) pers_right=false;
							if (!izq_col && der_col) pers_right=true;
							pers_state=S_HIT;
							pers_substate=0;
							} else {
								hit_time=HIT_TIME;
							} /* if */ 
						} /* if */ 
						freezed=0;
					} /* if */ 
				} else {
					/* Bala parada por el escudo: */ 
					Sound_play(S_shield);
					object[obj].type=T_NADA;
				} /* if */ 
			} /* if */ 
		} /* if */ 

		if (player_energy[character]<=0 && pers_state!=S_DEATH) {
			pers_state=S_DEATH;
			pers_substate=0;
		} /* if */ 
	} /* if */ 


	/* Determinar si está en el agua: */ 
	if (pers_right) val=character_tile[character];
			   else val=character_tile_inv[character];
	if ((tiles[val+pers_pos]->
			coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_WATER)!=0) {
		if (!in_water) {
			Sound_play(S_enterwater); 
		} /* if */ 
		in_water=true;
		water_counter++;
		if (pers_state!=S_HIT) {
			if (character==0) {
				if (water_counter>=8) {
					Sound_play(S_waterhit);
					player_energy[0]-=2;
					water_counter=0;
				} /* if */ 
			} /* if */ 
			if (character==1) {
				if (water_counter>=192) {
					Sound_play(S_waterhit);
					player_energy[1]-=2;
					water_counter=0;
				} /* if */ 
			} /* if */ 
		} /* if */ 
	} else {
		in_water=false;
//		water_counter=0;
	} /* if */ 

	/* Determinar si está en la lava: */ 
	in_lava=false;
	if (pers_right) val=character_tile[character];
			   else val=character_tile_inv[character];
	if ((tiles[val+pers_pos]->
			coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_LAVA)!=0 &&
		pers_state!=S_DEATH) {
		if (map!=0 || map_x!=8 || map_y!=11) in_lava=true;
		lava_counter++;
		in_water=true; /* Para frenar el movimiento! */ 
/*		if (pers_state==S_WALKING_RIGHT ||
			pers_state==S_WALKING_LEFT ||
			pers_state==S_JUMPING ||
			pers_state==S_STOPPED_JUMPING ||
			pers_state==S_QUIET_SWORD ||
			pers_state==S_UC_FALL_SWORD	|| 
			pers_state==S_C_FALL_SWORD) {
			pers_state=S_QUIET;
		} /* if */ 
		pers_pos=0;
		if (lava_counter>=4) {
			Sound_play(S_lavahit);
			player_energy[character]-=2;
			lava_counter=0;
		} /* if */ 
	} /* if */ 


	if (item[19] &&
		((keyboard[SDLK_LCTRL] && !old_keyboard[SDLK_LCTRL]) ||
		 (keyboard[SDLK_RCTRL] && !old_keyboard[SDLK_RCTRL])) &&
		bible_counter>0 && bible_subcounter==0) {
		bible_counter--;
		bible_subcounter=128;
		Sound_pause_music();
	} /* if */ 

	if (bible_subcounter>0) {
		if ((bible_subcounter&0xf)==0) {
			Sound_play(S_bible);
		} /* if */ 
		bible_subcounter--;
		if (bible_subcounter==0) {
			Sound_unpause_music();
		} /* if */ 
	} /* if */ 

	if (pers_state!=S_JUMPING) jumping_from_elevator=false;

	if (freezed>0) {
		freezed--;
		if ((freezed&0x01)==0) {
			if ((freezed&0x02)==0) {
				next_x=pers_x+TILE_UNIT;
			} else {
				next_x=pers_x-TILE_UNIT;
			} /* if */ 
		} /* if */ 
	} else {
		switch(pers_state) {
		case S_QUIET:
				if (keyboard[LEFT_KEY]) {
					if (!pers_right) {
						if (!in_lava) pers_state=S_WALKING_LEFT;
						pers_right=false;
						pers_walk_state=0;
					} else {
						pers_right=false;
					} /* if */ 
				} /* if */ 
				if (keyboard[RIGHT_KEY]) {
					if (pers_right) {
						if (!in_lava) pers_state=S_WALKING_RIGHT;
						pers_right=true;
						pers_walk_state=1;
					} else {
						pers_right=true;
					} /* if */ 
				} /* if */ 
				if (keyboard[UP_KEY] && !old_keyboard[UP_KEY] && !escalera_up && !in_lava) {
					pers_state=S_JUMPING;
					if (character_over_vertical_lever) jumping_from_elevator=true;
					pers_substate=0;
					jump_inertia=0;
					if (keyboard[LEFT_KEY] && !old_keyboard[RIGHT_KEY]) jump_inertia=-1;
					if (keyboard[RIGHT_KEY] && !old_keyboard[LEFT_KEY]) jump_inertia=1;
					Sound_play(S_jump); 
				} /* if */ 
				if (keyboard[UP_KEY] && !old_keyboard[UP_KEY] && escalera_up) {
					pers_state=S_UP_LADDER;
					pers_substate=0;
					next_x=object[escalera_up_i].x;
					in_ladder=escalera_up_i;
				} /* if */ 
				if (keyboard[DOWN_KEY] && !old_keyboard[DOWN_KEY] && escalera_down && pers_state!=S_JUMPING) {
					pers_state=S_DOWN_LADDER;
					pers_substate=0;
					next_x=object[escalera_down_i].x;
					in_ladder=escalera_down_i;
				} /* if */ 
				if (pers_right) val=character_tile[character];
						   else val=character_tile_inv[character];
				if ((tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y+1,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
					pers_state=S_UNCONTROLED_FALL;
					pers_substate=0;
					pers_pos=0;
				} /* if */ 
				if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY] && !in_lava) {
					Sound_play(S_sword);
					if (pers_state==S_UNCONTROLED_FALL) {
						pers_state=S_UC_FALL_SWORD;
						sword_time=0;
						pers_substate=0;
						pers_pos=3;
					} /* if */ 
					if (pers_state==S_JUMPING) {
						sword_time=0;
						pers_state=S_JUMPING_SWORD;
						pers_pos=4;
					} /* if */ 
					if (pers_state==S_QUIET || pers_state==S_WALKING_LEFT || pers_state==S_WALKING_RIGHT) {
						pers_state=S_QUIET_SWORD;
						sword_time=0;
						pers_substate=0;
						pers_pos=3;
					} /* if */ 
				} /* if */ 
				if (keyboard[WEAPON_KEY] && !old_keyboard[WEAPON_KEY]) fire_arrow(); 
				break;
		case S_WALKING_RIGHT:
				if (in_water) {
					if ((pers_substate&0x01)==0) next_x=pers_x+TILE_UNIT;
				} else {
					/* Efecto de las botas */ 
					next_x=pers_x+TILE_UNIT*2;
					if (item[4]) next_x+=TILE_UNIT/2;
				} /* if */ 
				pers_right=true;
				pers_substate++;
				if (pers_substate>7) pers_substate=0;
				if (pers_substate<4) pers_pos=1;
				if (pers_substate>=4) pers_pos=0;

				if (keyboard[RIGHT_KEY] && !old_keyboard[LEFT_KEY]) pers_walk_state=0;

				if (keyboard[LEFT_KEY] && pers_walk_state==0) pers_state=S_WALKING_LEFT;

				if (!keyboard[RIGHT_KEY]) {
					if (!keyboard[LEFT_KEY]) pers_state=S_QUIET;
										else pers_state=S_WALKING_LEFT;
				} /* if */ 
				if (keyboard[UP_KEY] && !old_keyboard[UP_KEY] && !escalera_up && !in_lava) {
					Sound_play(S_jump);
					pers_state=S_JUMPING;
					if (character_over_vertical_lever) jumping_from_elevator=true;
					pers_substate=0;
					jump_inertia=1;
				} /* if */ 
				if (keyboard[UP_KEY] && !old_keyboard[UP_KEY] && escalera_up) {
					pers_state=S_UP_LADDER;
					pers_substate=0;
					next_x=object[escalera_up_i].x;
					in_ladder=escalera_up_i;
				} /* if */ 
				if (keyboard[DOWN_KEY] && !old_keyboard[DOWN_KEY] && escalera_down && pers_state!=S_JUMPING) {
					pers_state=S_DOWN_LADDER;
					pers_substate=0;
					next_x=object[escalera_down_i].x;
					in_ladder=escalera_down_i;
				} /* if */ 
				if (pers_right) val=character_tile[character];
						   else val=character_tile_inv[character];
				if ((tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y+1,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
					pers_state=S_UNCONTROLED_FALL;
					pers_substate=0;
					pers_pos=0;
					next_x=pers_x;	/* If the character must fall, he cannot walk */ 
				} /* if */ 
				if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY] && !in_lava) {
					Sound_play(S_sword);
					if (pers_state==S_UNCONTROLED_FALL) {
						pers_state=S_UC_FALL_SWORD;
						pers_substate=0;
						pers_pos=3;
						sword_time=0;
					} /* if */ 
					if (pers_state==S_JUMPING) {
						sword_time=0;
						pers_state=S_JUMPING_SWORD;
						pers_pos=4;
					} /* if */ 
					if (pers_state==S_QUIET || pers_state==S_WALKING_LEFT || pers_state==S_WALKING_RIGHT) {
						pers_state=S_QUIET_SWORD;
						pers_substate=0;
						pers_pos=3;
						sword_time=0;
					} /* if */ 
				} /* if */ 
				if (keyboard[WEAPON_KEY] && !old_keyboard[WEAPON_KEY]) fire_arrow(); 
				break;
		case S_WALKING_LEFT:
				if (in_water) {
					if ((pers_substate&0x01)==0) next_x=pers_x-TILE_UNIT;
				} else {
					/* Efecto de las botas */ 
					next_x=pers_x-TILE_UNIT*2;
					if (item[4]) next_x-=TILE_UNIT/2;
				} /* if */ 
				pers_right=false;
				pers_substate++;
				if (pers_substate>7) pers_substate=0;
				if (pers_substate<4) pers_pos=1;
				if (pers_substate>=4) pers_pos=0;

				if (keyboard[LEFT_KEY] && !old_keyboard[RIGHT_KEY]) pers_walk_state=1;

				if (keyboard[RIGHT_KEY] && pers_walk_state==1) pers_state=S_WALKING_RIGHT;

				if (!keyboard[LEFT_KEY]) {
					if (!keyboard[RIGHT_KEY]) pers_state=S_QUIET;
						 			     else pers_state=S_WALKING_RIGHT;
				} /* if */ 
				if (keyboard[UP_KEY] && !old_keyboard[UP_KEY] && !escalera_up && !in_lava) {
					Sound_play(S_jump);
					pers_state=S_JUMPING;
					if (character_over_vertical_lever) jumping_from_elevator=true;
					pers_substate=0;
					jump_inertia=-1;
				} /* if */ 
				if (keyboard[UP_KEY] && !old_keyboard[UP_KEY] && escalera_up) {
					pers_state=S_UP_LADDER;
					pers_substate=0;
					next_x=object[escalera_up_i].x;
					in_ladder=escalera_up_i;
				} /* if */ 
				if (keyboard[DOWN_KEY] && !old_keyboard[DOWN_KEY] && escalera_down && pers_state!=S_JUMPING) {
					pers_state=S_DOWN_LADDER;
					pers_substate=0;
					next_x=object[escalera_down_i].x;
					in_ladder=escalera_down_i;
				} /* if */ 
				if (pers_right) val=character_tile[character];
						   else val=character_tile_inv[character];
				if ((tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y+TILE_UNIT,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
					pers_state=S_UNCONTROLED_FALL;
					pers_substate=0;
					pers_pos=0;
					next_x=pers_x;	/* If the character must fall, he cannot walk */ 
				} /* if */ 
				if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY] && !in_lava) {
					Sound_play(S_sword);
					if (pers_state==S_UNCONTROLED_FALL) {
						pers_state=S_UC_FALL_SWORD;
						pers_substate=0;
						pers_pos=3;
						sword_time=0;
					} /* if */ 
					if (pers_state==S_JUMPING) {
						sword_time=0;
						pers_state=S_JUMPING_SWORD;
						pers_pos=4;
					} /* if */ 
					if (pers_state==S_QUIET || pers_state==S_WALKING_LEFT || pers_state==S_WALKING_RIGHT) {
						pers_state=S_QUIET_SWORD;
						pers_substate=0;
						pers_pos=3;
						sword_time=0;
					} /* if */ 
				} /* if */ 
				if (keyboard[WEAPON_KEY] && !old_keyboard[WEAPON_KEY]) fire_arrow(); 
				break;
		case S_UNCONTROLED_FALL:
				colision=T_WALL|T_LADDER_WALL|T_DOOR_WALL;
				pers_pos=0;
				if (pers_right) val=character_tile[character];
						   else val=character_tile_inv[character];
				if (in_water) {
					if (pers_substate== 0) if ((cycle&0x01)==0) next_y=pers_y+(TILE_UNIT/2);
					if (pers_substate== 1) next_y=pers_y+(TILE_UNIT/2);
					if (pers_substate== 2) next_y=pers_y+(TILE_UNIT/2)*2;
					if (pers_substate== 3) next_y=pers_y+(TILE_UNIT/2)*2;
					if (pers_substate== 4) next_y=pers_y+(TILE_UNIT/2)*2;
					if (pers_substate== 5) next_y=pers_y+(TILE_UNIT/2)*2;
					if (pers_substate== 6) next_y=pers_y+(TILE_UNIT/2)*3;
					if (pers_substate== 7) next_y=pers_y+(TILE_UNIT/2)*3;
					if (pers_substate>= 8) next_y=pers_y+(TILE_UNIT/2)*3;
					if ((cycle&0x03)==0) pers_substate++;
				} else {
					if (pers_substate== 0) next_y=pers_y+TILE_UNIT;
					if (pers_substate== 1) next_y=pers_y+TILE_UNIT*2;
					if (pers_substate== 2) next_y=pers_y+TILE_UNIT*3;
					if (pers_substate== 3) next_y=pers_y+TILE_UNIT*4;
					if (pers_substate== 4) next_y=pers_y+TILE_UNIT*4;
					if (pers_substate== 5) next_y=pers_y+TILE_UNIT*5;
					if (pers_substate== 6) next_y=pers_y+TILE_UNIT*5;
					if (pers_substate== 7) next_y=pers_y+TILE_UNIT*5;
					if (pers_substate>= 8) next_y=pers_y+TILE_UNIT*6;
					pers_substate++;
				} /* if */ 
				if ((tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+next_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
					if (pers_substate>=2) Sound_play(S_fall);
					pers_state=S_QUIET;
					pers_substate=0;
					pers_pos=0;
				} /* if */ 
				if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
					Sound_play(S_sword);
					if (pers_state==S_UNCONTROLED_FALL) {
						pers_state=S_UC_FALL_SWORD;
						sword_time=0;
						pers_pos=3;
					} /* if */ 
					if (pers_state==S_JUMPING) {
						sword_time=0;
						pers_state=S_JUMPING_SWORD;
						pers_pos=4;
					} /* if */ 
					if (pers_state==S_QUIET || pers_state==S_WALKING_LEFT || pers_state==S_WALKING_RIGHT) {
						pers_state=S_QUIET_SWORD;
						sword_time=0;
						pers_substate=0;
						pers_pos=3;
					} /* if */ 
				} /* if */ 
				if (keyboard[WEAPON_KEY] && !old_keyboard[WEAPON_KEY]) fire_arrow(); 
				break;

		case S_CONTROLED_FALL:
				colision=T_WALL|T_LADDER_WALL|T_DOOR_WALL;
				pers_pos=2;
				if (pers_right) val=character_tile[character];
						   else val=character_tile_inv[character];

				if (previous_x_collision) jump_inertia=0;
				if ((!keyboard[RIGHT_KEY] && !keyboard[LEFT_KEY]) ||
					(keyboard[RIGHT_KEY] && keyboard[LEFT_KEY])) {
					if (in_water) {
						next_x=pers_x+jump_inertia*(TILE_UNIT/2);
					} else {
						next_x=pers_x+jump_inertia*TILE_UNIT*2;
					} /* if */ 
				} /* if */ 
				if (keyboard[RIGHT_KEY] && !keyboard[LEFT_KEY]) {
					pers_right=true;
					if (in_water) {
						if (jump_inertia==1) next_x=pers_x+TILE_UNIT;
						if (jump_inertia==0) next_x=pers_x+TILE_UNIT/2;
						if (jump_inertia==-1) if ((cycle&0x01)==0) next_x=pers_x-TILE_UNIT/2;
					} else {
						if (jump_inertia==1) next_x=pers_x+TILE_UNIT*3;
						if (jump_inertia==0) next_x=pers_x+TILE_UNIT*2;
						if (jump_inertia==-1) next_x=pers_x-TILE_UNIT;
					} /* if */ 
				} /* if */ 
				if (keyboard[LEFT_KEY] && !keyboard[RIGHT_KEY]) {
					pers_right=false;
					if (in_water) {
						if (jump_inertia==-1) next_x=pers_x-TILE_UNIT;
						if (jump_inertia==0) next_x=pers_x-TILE_UNIT/2;
						if (jump_inertia==1) if ((cycle&0x01)==0) next_x=pers_x+TILE_UNIT/2;
					} else {
						if (jump_inertia==-1) next_x=pers_x-TILE_UNIT*3;
						if (jump_inertia==0) next_x=pers_x-TILE_UNIT*2;
						if (jump_inertia==1) next_x=pers_x+TILE_UNIT;
					} /* if */ 
				} /* if */ 

				next_y=pers_y;

				if (in_water) {
					if (pers_substate== 3) if ((cycle&0x01)==0) next_y=pers_y+TILE_UNIT/2;
					if (pers_substate== 4) next_y=pers_y+TILE_UNIT/2;
					if (pers_substate== 5) next_y=pers_y+TILE_UNIT;
					if (pers_substate== 6) next_y=pers_y+TILE_UNIT;
					if (pers_substate== 7) next_y=pers_y+TILE_UNIT;
					if (pers_substate== 8) next_y=pers_y+TILE_UNIT;
					if (pers_substate== 9) next_y=pers_y+(TILE_UNIT/2)*3;
					if (pers_substate==10) next_y=pers_y+(TILE_UNIT/2)*3;
					if (pers_substate>=11) next_y=pers_y+(TILE_UNIT/2)*3;
					if ((cycle&0x03)==0) pers_substate++;
				} else { 
					if (pers_substate== 3) next_y=pers_y+TILE_UNIT;
					if (pers_substate== 4) next_y=pers_y+TILE_UNIT*2;
					if (pers_substate== 5) next_y=pers_y+TILE_UNIT*3;
					if (pers_substate== 6) next_y=pers_y+TILE_UNIT*4;
					if (pers_substate== 7) next_y=pers_y+TILE_UNIT*4;
					if (pers_substate== 8) next_y=pers_y+TILE_UNIT*5;
					if (pers_substate== 9) next_y=pers_y+TILE_UNIT*5;
					if (pers_substate==10) next_y=pers_y+TILE_UNIT*5;
					if (pers_substate>=11) next_y=pers_y+TILE_UNIT*6;
					pers_substate++;
				} /* if */ 
				if ((tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					(tiles[val+pers_pos+MASK_OFFSET]->
							coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+next_y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
					pers_state=S_QUIET;
					pers_substate=0;
					pers_pos=0;
				} /* if */ 

				if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
					Sound_play(S_sword);
					if (pers_state==S_CONTROLED_FALL) {
						pers_state=S_C_FALL_SWORD;
						sword_time=0;
						pers_pos=4;
					} /* if */ 
					if (pers_state==S_JUMPING) {
						sword_time=0;
						pers_state=S_JUMPING_SWORD;
						pers_pos=4;
					} /* if */ 
					if (pers_state==S_QUIET || pers_state==S_WALKING_LEFT || pers_state==S_WALKING_RIGHT) {
						pers_state=S_QUIET_SWORD;
						sword_time=0;
						pers_substate=0;
						pers_pos=3;
					} /* if */ 
				} /* if */ 
				if (keyboard[WEAPON_KEY] && !old_keyboard[WEAPON_KEY]) fire_arrow(); 
				break;

		case S_JUMPING:
				pers_pos=2;
				if (!previous_y_collision) {
					if (character==0) {
						if (!keyboard[UP_KEY] && pers_substate<11) pers_substate=11;
						if (in_water) {
							if (pers_substate== 0) next_y=pers_y-TILE_UNIT*2;
							if (pers_substate== 1) next_y=pers_y-(TILE_UNIT/2)*3;
							if (pers_substate== 2) next_y=pers_y-(TILE_UNIT/2)*3;
							if (pers_substate== 3) next_y=pers_y-(TILE_UNIT/2)*3;
							if (pers_substate== 4) next_y=pers_y-(TILE_UNIT/2)*3;
							if (pers_substate== 5) next_y=pers_y-(TILE_UNIT/2)*3;
							if (pers_substate== 6) next_y=pers_y-(TILE_UNIT/2)*2;
							if (pers_substate== 7) next_y=pers_y-(TILE_UNIT/2)*2;
							if (pers_substate== 8) next_y=pers_y-(TILE_UNIT/2)*2;
							if (pers_substate== 9) if ((cycle&0x01)==0) next_y=pers_y-(TILE_UNIT/2)*3;
							if (pers_substate==10) next_y=pers_y-(TILE_UNIT/2);
							if (pers_substate==11) if ((cycle&0x01)==0) next_y=pers_y-(TILE_UNIT/2);
						} else {
							if (pers_substate== 0) next_y=pers_y-TILE_UNIT*7;
							if (pers_substate== 1) next_y=pers_y-TILE_UNIT*6;
							if (pers_substate== 2) next_y=pers_y-TILE_UNIT*6;
							if (pers_substate== 3) next_y=pers_y-TILE_UNIT*6;
							if (pers_substate== 4) next_y=pers_y-TILE_UNIT*5;
							if (pers_substate== 5) next_y=pers_y-TILE_UNIT*5;
							if (pers_substate== 6) next_y=pers_y-TILE_UNIT*5;
							if (pers_substate== 7) next_y=pers_y-TILE_UNIT*4;
							if (pers_substate== 8) next_y=pers_y-TILE_UNIT*4;
							if (pers_substate== 9) next_y=pers_y-TILE_UNIT*3;
							if (pers_substate==10) next_y=pers_y-TILE_UNIT*2;
							if (pers_substate==11) next_y=pers_y-TILE_UNIT*1;
						} /* if */ 
					} /* if */ 
					if (character==1) {
						if (pers_substate==8) pers_substate=11;
						if (in_water) {
							if (pers_substate== 0) next_y=pers_y-(TILE_UNIT/2)*4;
							if (pers_substate== 1) next_y=pers_y-(TILE_UNIT/2)*3;
							if (pers_substate== 2) next_y=pers_y-(TILE_UNIT/2)*3;
							if (pers_substate== 3) next_y=pers_y-(TILE_UNIT/2)*3;
							if (pers_substate== 4) next_y=pers_y-(TILE_UNIT/2)*3;
							if (pers_substate== 5) next_y=pers_y-(TILE_UNIT/2)*2;
							if (pers_substate== 6) next_y=pers_y-(TILE_UNIT/2)*2;
							if (pers_substate== 7) next_y=pers_y-(TILE_UNIT/2);
							if (pers_substate==11) if ((cycle&0x01)==0) next_y=pers_y-(TILE_UNIT/2);		
						} else {
							if (pers_substate== 0) next_y=pers_y-TILE_UNIT*7;
							if (pers_substate== 1) next_y=pers_y-TILE_UNIT*6;
							if (pers_substate== 2) next_y=pers_y-TILE_UNIT*6;
							if (pers_substate== 3) next_y=pers_y-TILE_UNIT*6;
							if (pers_substate== 4) next_y=pers_y-TILE_UNIT*5;
							if (pers_substate== 5) next_y=pers_y-TILE_UNIT*5;
							if (pers_substate== 6) next_y=pers_y-TILE_UNIT*4;
							if (pers_substate== 7) next_y=pers_y-TILE_UNIT*3;
							if (pers_substate==11) next_y=pers_y-TILE_UNIT*1;		
						} /* if */ 
					} /* if */ 
				} /* if */ 

				if (pers_right) val=character_tile[character];
						   else val=character_tile_inv[character];
				if ((
					(tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x-TILE_UNIT*4,GAME_VIEW_Y+next_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0 &&
					(tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x-TILE_UNIT*3,GAME_VIEW_Y+next_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0 &&
					(tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x-TILE_UNIT*2,GAME_VIEW_Y+next_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0 &&
					(tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x-TILE_UNIT,GAME_VIEW_Y+next_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0 &&
					(tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+next_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0 &&
					(tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x+TILE_UNIT,GAME_VIEW_Y+next_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0 &&
					(tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x+TILE_UNIT*2,GAME_VIEW_Y+next_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0 &&
					(tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x+TILE_UNIT*3,GAME_VIEW_Y+next_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0 &&
					(tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x+TILE_UNIT*4,GAME_VIEW_Y+next_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0
					)
							 ||
					previous_y_collision
					) {
					pers_substate=11;
					pers_state=S_STOPPED_JUMPING;
				} /* if */ 

				if (previous_x_collision) jump_inertia=0;
				if (in_water) {
					if (jump_inertia==1) next_x=pers_x+TILE_UNIT/2;
					if (jump_inertia==-1) next_x=pers_x-TILE_UNIT/2;
					if ((cycle&0x03)==0) pers_substate++;
				} else {
					if (jump_inertia==1) next_x=pers_x+TILE_UNIT*2;
					if (jump_inertia==-1) next_x=pers_x-TILE_UNIT*2;
					pers_substate++;
				} /* if */ 

				if (pers_substate>=13) {
					pers_state=S_CONTROLED_FALL;
					pers_substate=0;
				} /* if */ 
				if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
					Sound_play(S_sword);
					if (pers_state==S_CONTROLED_FALL) {
						pers_state=S_C_FALL_SWORD;
						sword_time=0;
						pers_pos=4;
					} /* if */ 
					if (pers_state==S_STOPPED_JUMPING) {
						pers_state=S_SJ_SWORD;
						sword_time=0;
						pers_pos=4;
					} /* if */ 
					if (pers_state==S_JUMPING) {
						sword_time=0;
						pers_state=S_JUMPING_SWORD;
						pers_pos=4;
					} /* if */ 
					if (pers_state==S_QUIET || pers_state==S_WALKING_LEFT || pers_state==S_WALKING_RIGHT) {
						pers_state=S_QUIET_SWORD;
						sword_time=0;
						pers_substate=0;
						pers_pos=3;
					} /* if */ 
				} /* if */ 
				if (keyboard[WEAPON_KEY] && !old_keyboard[WEAPON_KEY]) fire_arrow(); 
				break;
		case S_STOPPED_JUMPING:
				pers_pos=2;
				if (character==0) 
					if (!keyboard[UP_KEY] && pers_substate<11) pers_substate=11;
				if (character==1) 
					if (pers_substate==8) pers_substate=11;

				if (previous_x_collision) jump_inertia=0;
				if (in_water) {
					if (jump_inertia==1) next_x=pers_x+TILE_UNIT/2;
					if (jump_inertia==-1) next_x=pers_x-TILE_UNIT/2;
					if ((cycle&0x03)==0) pers_substate++;
				} else {
					if (jump_inertia==1) next_x=pers_x+TILE_UNIT*2;
					if (jump_inertia==-1) next_x=pers_x-TILE_UNIT*2;
					pers_substate++;
				} /* if */ 

				if (pers_substate>=13) {
					pers_state=S_CONTROLED_FALL;
					pers_substate=0;
				} /* if */ 
				if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
					Sound_play(S_sword);
					if (pers_state==S_CONTROLED_FALL) {
						pers_state=S_C_FALL_SWORD;
						sword_time=0;
						pers_pos=4;
					} /* if */ 
					if (pers_state==S_STOPPED_JUMPING) {
						pers_state=S_SJ_SWORD;
						sword_time=0;
						pers_pos=4;
					} /* if */ 
					if (pers_state==S_JUMPING) {
						sword_time=0;
						pers_state=S_JUMPING_SWORD;
						pers_pos=4;
					} /* if */ 
					if (pers_state==S_QUIET || pers_state==S_WALKING_LEFT || pers_state==S_WALKING_RIGHT) {
						pers_state=S_QUIET_SWORD;
						sword_time=0;
						pers_substate=0;
						pers_pos=3;
					} /* if */ 
				} /* if */ 
				if (keyboard[WEAPON_KEY] && !old_keyboard[WEAPON_KEY]) fire_arrow(); 
				break;
		case S_QUIET_SWORD:
				{
					int sword_delay=4;

					if (stone_hit_counter>=6 && character==1) sword_delay=2+(rand()%2);

					pers_pos=3;
					sword=true;
					if (pers_right)	sword_x=pers_x+2*TILE_SIZE_X+3*TILE_UNIT;
							   else sword_x=pers_x-2*TILE_SIZE_X-5*TILE_UNIT;
					sword_y=pers_y;
					if (sword_time>=(in_water ? sword_delay*2 : sword_delay)) {
						pers_state=S_QUIET;
						pers_substate=0;
						pers_pos=0;
						sword=false;
					} /* if */ 

					if (pers_right) val=character_tile[character];
							   else val=character_tile_inv[character];
					if ((tiles[val+pers_pos+MASK_OFFSET]->
							coltest(GAME_VIEW_X+next_x,GAME_VIEW_Y+pers_y+1,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
						if (pers_state==S_QUIET) {
							pers_state=S_UNCONTROLED_FALL;
							pers_substate=0;
						} /* if */ 
						if (pers_state==S_QUIET_SWORD) {
							pers_state=S_UC_FALL_SWORD;
							pers_substate=0;
						} /* if */ 
					} /* if */ 
					pers_substate++;
					sword_time++;
					if (keyboard[WEAPON_KEY] && !old_keyboard[WEAPON_KEY]) fire_arrow(); 
				}
				break;
		case S_UC_FALL_SWORD:
				pers_pos=4;
				sword=true;
				if (pers_right)	sword_x=pers_x+2*TILE_SIZE_X;
						   else sword_x=pers_x-2*TILE_SIZE_X-2*TILE_UNIT;
				sword_y=pers_y+TILE_UNIT;
				if (sword_time>=(in_water ? 8 : 4)) {
					pers_state=S_UNCONTROLED_FALL;
					pers_pos=0;
					sword=false;
				} /* if */ 

				if (pers_right) val=character_tile[character];
						   else val=character_tile_inv[character];
				if (in_water) {
					if (pers_substate== 0) if ((cycle&0x01)==0) next_y=pers_y+(TILE_UNIT/2);
					if (pers_substate== 1) next_y=pers_y+(TILE_UNIT/2);
					if (pers_substate== 2) next_y=pers_y+(TILE_UNIT/2)*2;
					if (pers_substate== 3) next_y=pers_y+(TILE_UNIT/2)*2;
					if (pers_substate== 4) next_y=pers_y+(TILE_UNIT/2)*2;
					if (pers_substate== 5) next_y=pers_y+(TILE_UNIT/2)*2;
					if (pers_substate== 6) next_y=pers_y+(TILE_UNIT/2)*3;
					if (pers_substate== 7) next_y=pers_y+(TILE_UNIT/2)*3;
					if (pers_substate>= 8) next_y=pers_y+(TILE_UNIT/2)*3;
					if ((cycle&0x03)==0) pers_substate++;
				} else {
					if (pers_substate== 0) next_y=pers_y+TILE_UNIT;
					if (pers_substate== 1) next_y=pers_y+TILE_UNIT*2;
					if (pers_substate== 2) next_y=pers_y+TILE_UNIT*3;
					if (pers_substate== 3) next_y=pers_y+TILE_UNIT*4;
					if (pers_substate== 4) next_y=pers_y+TILE_UNIT*4;
					if (pers_substate== 5) next_y=pers_y+TILE_UNIT*5;
					if (pers_substate== 6) next_y=pers_y+TILE_UNIT*5;
					if (pers_substate== 7) next_y=pers_y+TILE_UNIT*5;
					if (pers_substate>= 8) next_y=pers_y+TILE_UNIT*6;
					pers_substate++;
				} /* if */ 
				if ((tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+next_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
					Sound_play(S_fall);
					if (pers_state==S_UNCONTROLED_FALL) {
						pers_state=S_QUIET;
						pers_substate=0;
						pers_pos=0;
					} /* if */ 
					if (pers_state==S_UC_FALL_SWORD) {
						pers_state=S_QUIET_SWORD;
						pers_substate=0;
					} /* if */ 
				} /* if */ 
				sword_time++;
				if (keyboard[WEAPON_KEY] && !old_keyboard[WEAPON_KEY]) fire_arrow(); 
				break;
		case S_C_FALL_SWORD:
				pers_pos=4;

				if (pers_right) val=character_tile[character];
						   else val=character_tile_inv[character];

				if ((!keyboard[RIGHT_KEY] && !keyboard[LEFT_KEY]) ||
					(keyboard[RIGHT_KEY] && keyboard[LEFT_KEY])) {
					if (in_water) {
						next_x=pers_x+jump_inertia*TILE_UNIT/2;
					} else {
						next_x=pers_x+jump_inertia*TILE_UNIT*2;
					} /* if */ 
				} /* if */ 
				if (keyboard[RIGHT_KEY] && !keyboard[LEFT_KEY]) {
					pers_right=true;
					if (in_water) {
						if (jump_inertia==1) next_x=pers_x+TILE_UNIT;
						if (jump_inertia==0) next_x=pers_x+(TILE_UNIT/2);
						if (jump_inertia==-1) if ((cycle&0x01)==0) next_x=pers_x-(TILE_UNIT/2);
					} else {
						if (jump_inertia==1) next_x=pers_x+TILE_UNIT*3;
						if (jump_inertia==0) next_x=pers_x+TILE_UNIT*2;
						if (jump_inertia==-1) next_x=pers_x-TILE_UNIT;
					} /* if */ 
				} /* if */ 
				if (keyboard[LEFT_KEY] && !keyboard[RIGHT_KEY]) {
					pers_right=false;
					if (in_water) {
						if (jump_inertia==-1) next_x=pers_x-TILE_UNIT;
						if (jump_inertia==0) next_x=pers_x-(TILE_UNIT/2);
						if (jump_inertia==1) if ((cycle&0x01)==0) next_x=pers_x+(TILE_UNIT/2);
					} else {
						if (jump_inertia==-1) next_x=pers_x-TILE_UNIT*3;
						if (jump_inertia==0) next_x=pers_x-TILE_UNIT*2;
						if (jump_inertia==1) next_x=pers_x+TILE_UNIT;
					} /* if */ 
				} /* if */ 

				sword=true;
				if (pers_right)	sword_x=pers_x+2*TILE_SIZE_X;
						   else sword_x=pers_x-2*TILE_SIZE_X-2*TILE_UNIT;
				sword_y=pers_y+TILE_UNIT;
				if (sword_time>=4) {
					pers_state=S_CONTROLED_FALL;
					pers_pos=2;
					if (pers_right) {
						pers_x-=2;
					} else {
						pers_x+=2;
					} /* if */ 
					sword=false;
				} /* if */ 

				next_y=pers_y;
				if (in_water) {
					if (pers_substate== 3) if ((cycle&0x01)==0) next_y=pers_y+(TILE_UNIT/2);
					if (pers_substate== 4) next_y=pers_y+(TILE_UNIT/2);
					if (pers_substate== 5) next_y=pers_y+(TILE_UNIT/2)*2;
					if (pers_substate== 6) next_y=pers_y+(TILE_UNIT/2)*2;
					if (pers_substate== 7) next_y=pers_y+(TILE_UNIT/2)*2;
					if (pers_substate== 8) next_y=pers_y+(TILE_UNIT/2)*2;
					if (pers_substate== 9) next_y=pers_y+(TILE_UNIT/2)*3;
					if (pers_substate==10) next_y=pers_y+(TILE_UNIT/2)*3;
					if (pers_substate>=11) next_y=pers_y+(TILE_UNIT/2)*3;
					if ((cycle&0x03)==0) pers_substate++;
				} else { 
					if (pers_substate== 3) next_y=pers_y+TILE_UNIT;
					if (pers_substate== 4) next_y=pers_y+TILE_UNIT*2;
					if (pers_substate== 5) next_y=pers_y+TILE_UNIT*3;
					if (pers_substate== 6) next_y=pers_y+TILE_UNIT*4;
					if (pers_substate== 7) next_y=pers_y+TILE_UNIT*4;
					if (pers_substate== 8) next_y=pers_y+TILE_UNIT*5;
					if (pers_substate== 9) next_y=pers_y+TILE_UNIT*5;
					if (pers_substate==10) next_y=pers_y+TILE_UNIT*5;
					if (pers_substate>=11) next_y=pers_y+TILE_UNIT*6;
					pers_substate++;
				} /* if */ 

				if ((tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					(tiles[val+pers_pos+MASK_OFFSET]->
							coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+next_y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
					if (pers_state==S_CONTROLED_FALL) {
						pers_state=S_QUIET;
						pers_substate=0;
						pers_pos=0;
					} /* if */ 
					if (pers_state==S_C_FALL_SWORD) {
						pers_state=S_QUIET_SWORD;
						pers_substate=0;
					} /* if */ 
				} /* if */ 

				sword_time++;
				if (keyboard[WEAPON_KEY] && !old_keyboard[WEAPON_KEY]) fire_arrow(); 
				break;
		case S_JUMPING_SWORD:
				pers_pos=4;
				sword=true;
				if (pers_right)	sword_x=pers_x+2*TILE_SIZE_X;
						   else sword_x=pers_x-2*TILE_SIZE_X-2*TILE_UNIT;
				sword_y=pers_y+TILE_UNIT;
				if (sword_time>=(in_water ? 8 : 4)) {
					pers_state=S_JUMPING;
					pers_pos=2;
					sword=false;
				} /* if */ 

				if (character==0) {
					if (!keyboard[UP_KEY] && pers_substate<11) pers_substate=11;
					if (in_water) {
						if (pers_substate== 0) next_y=pers_y-(TILE_UNIT/2)*4;
						if (pers_substate== 1) next_y=pers_y-(TILE_UNIT/2)*3;
						if (pers_substate== 2) next_y=pers_y-(TILE_UNIT/2)*3;
						if (pers_substate== 3) next_y=pers_y-(TILE_UNIT/2)*3;
						if (pers_substate== 4) next_y=pers_y-(TILE_UNIT/2)*3;
						if (pers_substate== 5) next_y=pers_y-(TILE_UNIT/2)*3;
						if (pers_substate== 6) next_y=pers_y-(TILE_UNIT/2)*2;
						if (pers_substate== 7) next_y=pers_y-(TILE_UNIT/2)*2;
						if (pers_substate== 8) next_y=pers_y-(TILE_UNIT/2)*2;
						if (pers_substate== 9) if ((cycle&0x01)==0) next_y=pers_y-(TILE_UNIT/2)*3;
						if (pers_substate==10) next_y=pers_y-(TILE_UNIT/2);
						if (pers_substate==11) if ((cycle&0x01)==0) next_y=pers_y-(TILE_UNIT/2);
					} else {
						if (pers_substate== 0) next_y=pers_y-TILE_UNIT*7;
						if (pers_substate== 1) next_y=pers_y-TILE_UNIT*6;
						if (pers_substate== 2) next_y=pers_y-TILE_UNIT*6;
						if (pers_substate== 3) next_y=pers_y-TILE_UNIT*6;
						if (pers_substate== 4) next_y=pers_y-TILE_UNIT*5;
						if (pers_substate== 5) next_y=pers_y-TILE_UNIT*5;
						if (pers_substate== 6) next_y=pers_y-TILE_UNIT*5;
						if (pers_substate== 7) next_y=pers_y-TILE_UNIT*4;
						if (pers_substate== 8) next_y=pers_y-TILE_UNIT*4;
						if (pers_substate== 9) next_y=pers_y-TILE_UNIT*3;
						if (pers_substate==10) next_y=pers_y-TILE_UNIT*2;
						if (pers_substate==11) next_y=pers_y-TILE_UNIT*1;
					} /* if */ 
				} /* if */ 
				if (character==1) {
					if (pers_substate==8) pers_substate=11;
					if (in_water) {
						if (pers_substate== 0) next_y=pers_y-(TILE_UNIT/2)*4;
						if (pers_substate== 1) next_y=pers_y-(TILE_UNIT/2)*3;
						if (pers_substate== 2) next_y=pers_y-(TILE_UNIT/2)*3;
						if (pers_substate== 3) next_y=pers_y-(TILE_UNIT/2)*3;
						if (pers_substate== 4) next_y=pers_y-(TILE_UNIT/2)*3;
						if (pers_substate== 5) next_y=pers_y-(TILE_UNIT/2)*2;
						if (pers_substate== 6) next_y=pers_y-(TILE_UNIT/2)*2;
						if (pers_substate== 7) next_y=pers_y-(TILE_UNIT/2);
						if (pers_substate==11) if ((cycle&0x01)==0) next_y=pers_y-(TILE_UNIT/2);		
					} else {
						if (pers_substate== 0) next_y=pers_y-TILE_UNIT*7;
						if (pers_substate== 1) next_y=pers_y-TILE_UNIT*6;
						if (pers_substate== 2) next_y=pers_y-TILE_UNIT*6;
						if (pers_substate== 3) next_y=pers_y-TILE_UNIT*6;
						if (pers_substate== 4) next_y=pers_y-TILE_UNIT*5;
						if (pers_substate== 5) next_y=pers_y-TILE_UNIT*5;
						if (pers_substate== 6) next_y=pers_y-TILE_UNIT*4;
						if (pers_substate== 7) next_y=pers_y-TILE_UNIT*3;
						if (pers_substate==11) next_y=pers_y-TILE_UNIT*1;		
					} /* if */ 
				} /* if */ 

				if (pers_right) val=character_tile[character];
						   else val=character_tile_inv[character];
				if ((tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+next_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0 &&
					(tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x+TILE_SIZE_X/2,GAME_VIEW_Y+next_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0 &&
					(tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x-TILE_SIZE_X/2,GAME_VIEW_Y+next_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0
							 ) {
					pers_substate=11;		
					if (pers_state==S_JUMPING_SWORD) {
						pers_state=S_SJ_SWORD;
					} /* if */ 
					if (pers_state==S_JUMPING) {
						pers_state=S_STOPPED_JUMPING;
					} /* if */ 
				} /* if */ 

				if (in_water) {
					if (jump_inertia==1) next_x=pers_x+(TILE_UNIT/2);
					if (jump_inertia==-1) next_x=pers_x-(TILE_UNIT/2);
					if ((cycle&0x03)==0) pers_substate++;
				} else {
					if (jump_inertia==1) next_x=pers_x+TILE_UNIT*2;
					if (jump_inertia==-1) next_x=pers_x-TILE_UNIT*2;
					pers_substate++;
				} /* if */ 

				if (pers_substate>=13) {
					if (pers_state==S_JUMPING_SWORD) {
						pers_state=S_C_FALL_SWORD;
						pers_substate=0;
					} /* if */ 
					if (pers_state==S_JUMPING) {
						pers_state=S_CONTROLED_FALL;
						pers_substate=0;
					} /* if */ 
				} /* if */ 

				sword_time++;
				if (keyboard[WEAPON_KEY] && !old_keyboard[WEAPON_KEY]) fire_arrow(); 
				break;
		case S_SJ_SWORD:
				pers_pos=4;
				sword=true;
				if (pers_right)	sword_x=pers_x+2*TILE_SIZE_X;
						   else sword_x=pers_x-2*TILE_SIZE_X-2*TILE_UNIT;
				sword_y=pers_y+TILE_UNIT;
				if (sword_time>=(in_water ? 8 : 4)) {
					pers_state=S_STOPPED_JUMPING;
					pers_pos=0;
					sword=false;
				} /* if */ 

				if (character==0) 
					if (!keyboard[UP_KEY] && pers_substate<11) pers_substate=11;
				if (character==1) 
					if (pers_substate==8) pers_substate=11;

				if (in_water) {
					if (jump_inertia==1) next_x=pers_x+(TILE_UNIT/2);
					if (jump_inertia==-1) next_x=pers_x-(TILE_UNIT/2);
					if ((cycle&0x03)==0) pers_substate++;
				} else {
					if (jump_inertia==1) next_x=pers_x+TILE_UNIT*2;
					if (jump_inertia==-1) next_x=pers_x-TILE_UNIT*2;
					pers_substate++;
				} /* if */ 

				if (pers_substate>=13) {
					if (pers_state==S_SJ_SWORD) {
						pers_state=S_C_FALL_SWORD;
						pers_substate=0;
					} /* if */ 
					if (pers_state==S_STOPPED_JUMPING) {
						pers_state=S_CONTROLED_FALL;
						pers_substate=0;
					} /* if */ 
				} /* if */ 
				sword_time++;
				if (keyboard[WEAPON_KEY] && !old_keyboard[WEAPON_KEY]) fire_arrow(); 
				break;
		case S_UP_LADDER:
				next_x=object[in_ladder].x;
				if (pers_substate>7) pers_substate=0;
				if (pers_substate<4) pers_pos=7;
				if (pers_substate>=4) pers_pos=8;
				if (keyboard[UP_KEY] && !keyboard[DOWN_KEY]) {
					pers_substate++;
					if (pers_y>(object[in_ladder].y-3*TILE_SIZE_Y)) {
						next_y=pers_y-TILE_UNIT;
					} /* if */ 
					if (next_y<=(object[in_ladder].y-3*TILE_SIZE_Y)) {
						if (object[in_ladder].state==1) {
							object[in_ladder].type=T_NADA;
							Sound_play(S_ladderdisapear);
							HP_add_record(map_x,map_y,in_ladder,T_LADDER);
						} /* if */ 
						pers_state=S_QUIET;
						pers_substate=0;
						in_ladder=-1;
						pers_pos=0;
					} /* if */ 
				} /* if */ 
				if (keyboard[DOWN_KEY]) {
					pers_state=S_DOWN_LADDER;
					if (pers_y<(object[in_ladder].y+(object[in_ladder].size-2)*TILE_SIZE_Y)) {
						next_y=pers_y+TILE_UNIT;
					} else {
						if (object[in_ladder].state==1) {
							object[in_ladder].type=T_NADA;
							Sound_play(S_ladderdisapear);
							HP_add_record(map_x,map_y,in_ladder,T_LADDER);
						} /* if */ 
						pers_state=S_QUIET;
						pers_substate=0;
						in_ladder=-1;
						pers_pos=0;
					} /* if */ 
				} /* if */ 
				if (keyboard[LEFT_KEY] && !keyboard[UP_KEY] && !keyboard[DOWN_KEY]) {
					pers_state=S_QUIET_LADDER;
					pers_right=false;
				} /* if */ 
				if (keyboard[RIGHT_KEY] && !keyboard[UP_KEY] && !keyboard[DOWN_KEY]) {
					pers_state=S_QUIET_LADDER;
					pers_right=true;
				} /* if */ 
				break;
		case S_DOWN_LADDER:
				next_x=object[in_ladder].x;
				if (pers_substate>7) pers_substate=0;
				if (pers_substate<4) pers_pos=5;
				if (pers_substate>=4) pers_pos=6;
				if (keyboard[DOWN_KEY] && !keyboard[UP_KEY]) {
					pers_substate++;
					if (pers_y<(object[in_ladder].y+(object[in_ladder].size-2)*TILE_SIZE_Y)) {
						next_y=pers_y+TILE_UNIT;
					} /* if */ 
					if (next_y>=(object[in_ladder].y+(object[in_ladder].size-2)*TILE_SIZE_Y)) {
						if (object[in_ladder].state==1) {
							object[in_ladder].type=T_NADA;
							Sound_play(S_ladderdisapear);
							HP_add_record(map_x,map_y,in_ladder,T_LADDER);
						} /* if */ 
						pers_state=S_QUIET;
						pers_substate=0;
						in_ladder=-1;
						pers_pos=0;
					} /* if */ 
				} /* if */ 
				if (keyboard[UP_KEY]) {
					pers_state=S_UP_LADDER;
					if (pers_y>(object[in_ladder].y-3*TILE_SIZE_Y)) {
						next_y=pers_y-TILE_UNIT;
					} else {
						if (object[in_ladder].state==1) {
							object[in_ladder].type=T_NADA;
							Sound_play(S_ladderdisapear);
							HP_add_record(map_x,map_y,in_ladder,T_LADDER);
						} /* if */ 
						pers_state=S_QUIET;
						pers_substate=0;
						in_ladder=-1;
						pers_pos=0;
					} /* if */ 
				} /* if */ 
				if (keyboard[LEFT_KEY] && !keyboard[UP_KEY] && !keyboard[DOWN_KEY]) {
					pers_state=S_QUIET_LADDER;
					pers_right=false;
				} /* if */ 
				if (keyboard[RIGHT_KEY] && !keyboard[UP_KEY] && !keyboard[DOWN_KEY]) {
					pers_state=S_QUIET_LADDER;
					pers_right=true;
				} /* if */ 
				break;

		case S_QUIET_LADDER:
				next_x=object[in_ladder].x;
				if (pers_substate>7) pers_substate=0;
				pers_pos=0;
				if (keyboard[UP_KEY]) {
					pers_state=S_UP_LADDER;
					if (pers_y>(object[in_ladder].y-3*TILE_SIZE_Y)) {
						next_y=pers_y-TILE_UNIT;
					} else {
						if (object[in_ladder].state==1) {
							object[in_ladder].type=T_NADA;
							Sound_play(S_ladderdisapear);
							HP_add_record(map_x,map_y,in_ladder,T_LADDER);
						} /* if */ 
						pers_state=S_QUIET;
						pers_substate=0;
						in_ladder=-1;
						pers_pos=0;
					} /* if */ 
				} /* if */ 
				if (keyboard[DOWN_KEY]) {
					pers_state=S_DOWN_LADDER;
					if (pers_y<(object[in_ladder].y+(object[in_ladder].size-2)*TILE_SIZE_Y)) {
						next_y=pers_y+TILE_UNIT;
					} else {
						if (object[in_ladder].state==1) {
							object[in_ladder].type=T_NADA;
							Sound_play(S_ladderdisapear);
							HP_add_record(map_x,map_y,in_ladder,T_LADDER);
						} /* if */ 
						pers_state=S_QUIET;
						pers_substate=0;
						in_ladder=-1;
						pers_pos=0;
					} /* if */ 
					next_y=pers_y+2;
				} /* if */ 
				if (keyboard[LEFT_KEY] && !keyboard[UP_KEY] && !keyboard[DOWN_KEY]) {
					int ty,tx,dty;

					tx=pers_x/TILE_SIZE_X;
					ty=pers_y/TILE_SIZE_Y;
					dty=pers_y-ty*TILE_SIZE_Y;
//					if (pers_y>=(object[in_ladder].y+(object[in_ladder].size-2)*TILE_SIZE_Y)-TILE_UNIT*2 &&
					if (dty>TILE_SIZE_Y/2 &&
						iswall(tx-1,ty+3) &&
						(tiles[character_tile_inv[character]+MASK_OFFSET]->
							coltest(GAME_VIEW_X+pers_x-8*TILE_UNIT,GAME_VIEW_Y+pers_y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
						next_x=pers_x-TILE_UNIT*6;
						pers_state=S_QUIET;
						pers_substate=0;
						in_ladder=-1;
						pers_pos=0;
						pers_right=false;
					} else {
						if (pers_y<((object[in_ladder].y-2*TILE_SIZE_Y)-TILE_SIZE_Y/2)) {
							if (object[in_ladder].state==1) {
								object[in_ladder].type=T_NADA;
								Sound_play(S_ladderdisapear);
								HP_add_record(map_x,map_y,in_ladder,T_LADDER);
							} /* if */ 
							next_x=pers_x-TILE_UNIT*4;
							next_y=ty*TILE_SIZE_Y;
							pers_state=S_QUIET;
							pers_substate=0;
							in_ladder=-1;
							pers_pos=0;
							pers_right=false;
						} else {
							pers_state=S_QUIET_LADDER;
							pers_right=false;
						} /* if */ 
					} /* if */ 
				} /* if */ 
				if (keyboard[RIGHT_KEY] && !keyboard[UP_KEY] && !keyboard[DOWN_KEY]) {
					int ty,tx,dty;

					tx=pers_x/TILE_SIZE_X;
					ty=pers_y/TILE_SIZE_Y;
					dty=pers_y-ty*TILE_SIZE_Y;
//					if (pers_y>=(object[in_ladder].y+(object[in_ladder].size-2)*TILE_SIZE_Y)-TILE_UNIT*2 &&
					if (dty>TILE_SIZE_Y/2 &&
						iswall(tx+2,ty+3) &&
						(tiles[character_tile[character]+MASK_OFFSET]->
							coltest(GAME_VIEW_X+pers_x+8*TILE_UNIT,GAME_VIEW_Y+pers_y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
						next_x=pers_x+TILE_UNIT*6;
						pers_state=S_QUIET;
						pers_substate=0;
						in_ladder=-1;
						pers_pos=0;
						pers_right=true;
					} else {
						if (pers_y<((object[in_ladder].y-2*TILE_SIZE_Y)-TILE_SIZE_Y/2)) {
							if (object[in_ladder].state==1) {
								object[in_ladder].type=T_NADA;
								Sound_play(S_ladderdisapear);
								HP_add_record(map_x,map_y,in_ladder,T_LADDER);
							} /* if */ 
							next_x=pers_x+TILE_UNIT*4;
							next_y=ty*TILE_SIZE_Y;
							pers_state=S_QUIET;
							pers_substate=0;
							in_ladder=-1;
							pers_pos=0;
							pers_right=true;
						} else {
							pers_state=S_QUIET_LADDER;
							pers_right=true;
						} /* if */ 
					} /* if */ 
				} /* if */ 
				if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY] && pers_state==S_QUIET_LADDER) {
					Sound_play(S_sword);
					pers_state=S_SWORD_LADDER;
					sword_time=0;
					pers_substate=0;
					pers_pos=3;
				} /* if */ 
				if (keyboard[WEAPON_KEY] && !old_keyboard[WEAPON_KEY]) fire_arrow(); 
				break;

		case S_SWORD_LADDER:
				next_x=object[in_ladder].x;
				pers_pos=3;
				sword=true;
				if (pers_right)	sword_x=pers_x+2*TILE_SIZE_X+3*TILE_UNIT;
						   else sword_x=pers_x-2*TILE_SIZE_X-5*TILE_UNIT;
				sword_y=pers_y;
				if (sword_time>=4) {
					pers_state=S_QUIET_LADDER;
					pers_substate=0;
					pers_pos=0;
					sword=false;
				} /* if */ 

				sword_time++;
				if (keyboard[WEAPON_KEY] && !old_keyboard[WEAPON_KEY]) fire_arrow(); 
				break;

		case S_HIT:
				if (pers_substate<4) {
					pers_pos=((pers_substate%4)>>1)+9;
					if (pers_right) {
						next_x=pers_x-TILE_UNIT*4;
					} else {
						next_x=pers_x+TILE_UNIT*4;
					} /* if */ 
					if ((tiles[val+MASK_OFFSET+pers_pos]->
						coltest(GAME_VIEW_X+next_x,GAME_VIEW_Y+pers_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0 &&
						(tiles[val+MASK_OFFSET+pers_pos]->
						coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {			
						next_x=pers_x;
						pers_state=S_HIT_COLISION;
					} /* if */ 
				} /* if */ 
				if (pers_substate>=4) {
					pers_pos=((pers_substate%4)>>1)+9;
					if (pers_right) {
						next_x=pers_x-TILE_UNIT*4;
					} else {
						next_x=pers_x+TILE_UNIT*4;
					} /* if */ 
					if ((tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+next_x,GAME_VIEW_Y+pers_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0 &&
						(tiles[val+pers_pos+MASK_OFFSET]->
						coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
						next_x=pers_x;
						pers_state=S_HIT_COLISION;
					} /* if */ 
					val=pers_substate-4;
					if (val>TILE_UNIT*6) val=TILE_UNIT*6;
					next_y=pers_y+val;
					if (pers_right) val=character_tile[character];
							   else val=character_tile_inv[character];
					if ((tiles[val+MASK_OFFSET+pers_pos]->
							coltest(GAME_VIEW_X+next_x,GAME_VIEW_Y+pers_y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
						&&
						(tiles[val+MASK_OFFSET+pers_pos]->
							coltest(GAME_VIEW_X+next_x,GAME_VIEW_Y+next_y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
						pers_state=S_HIT_RECOVERY;
						pers_substate=0;
						pers_pos=0;

					} /* if */ 

				} /* if */ 

	//			if (pers_substate>=24) {
	//				pers_state=S_QUIET;
	//				pers_substate=0;
	//				pers_pos=0;
	//			} else {
					pers_substate++;
	//			} /* if */ 
				break;

		case S_HIT_COLISION:
				if (pers_substate<4) {
					pers_pos=((pers_substate%4)>>1)+9;
				} /* if */ 
				if (pers_substate>=4) {
					pers_pos=((pers_substate%4)>>1)+9;
					val=pers_substate-4;
					if (val>TILE_UNIT*6) val=TILE_UNIT*6;
					next_y=pers_y+val;
					if (pers_right) val=character_tile[character];
							   else val=character_tile_inv[character];
					if ((tiles[val+MASK_OFFSET+pers_pos]->
							coltest(GAME_VIEW_X+next_x,GAME_VIEW_Y+pers_y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
						&&
						(tiles[val+MASK_OFFSET+pers_pos]->
							coltest(GAME_VIEW_X+next_x,GAME_VIEW_Y+next_y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
						pers_state=S_HIT_RECOVERY;
						pers_substate=0;
						pers_pos=0;
					} /* if */ 
				} /* if */ 

	//			if (pers_substate>=24) {
	//				pers_state=S_QUIET;
	//				pers_substate=0;
	//				pers_pos=0;
	//			} else {
					pers_substate++;
	//			} /* if */ 
				break;

		case S_HIT_RECOVERY:
				pers_pos=((pers_substate%4)>>1)+11;
				if (pers_substate>=8) {
					pers_state=S_QUIET;
					pers_substate=0;
					pers_pos=0;
					hit_time=HIT_TIME;
				} else {
					pers_substate++;
				} /* if */ 
				break;
		case S_DEATH:
				sword=false;
				if (pers_substate==0) {
					Sound_pause_music();
					Sound_play(S_death);
				} /* if */ 
				if (pers_substate<6) pers_pos=13;
				if (pers_substate>=6 && pers_substate<12) pers_pos=14;
				if (pers_substate>=12 && pers_substate<18) pers_pos=15;
				if (pers_substate>=18 && pers_substate<24) pers_pos=16;
				if (pers_substate>=24) pers_pos=17;

				if (pers_substate>=40) {
					Sound_release_music();
					live_character[character]=false;
					STATE=11;
					SUBSTATE=0;
				} /* if */ 
				pers_substate++;
				break;

		case S_DOORPASS:
				pers_substate++;
				if (pers_substate>=20) {
					if (pers_substate<=26) {
						if (pers_right)	next_x=pers_x+TILE_UNIT*4;
								   else next_x=pers_x-TILE_UNIT*4;
						if ((pers_substate&0x01)==0) pers_pos=1;
						if ((pers_substate&0x01)==1) pers_pos=0;
					} /* if */ 

 					if (pers_substate>=30) {
						pers_state=S_QUIET;
						pers_substate=0;
						pers_pos=0;
					} /* if */ 
				} else {
					pers_pos=0;
				} /* if */ 
				break;

		case S_SLOWDOORPASS:
				if (pers_substate<=24) {
					if (pers_right)	next_x=pers_x+TILE_UNIT;
							   else next_x=pers_x-TILE_UNIT;
					if ((pers_substate&0x04)==0) pers_pos=1;
					if ((pers_substate&0x04)==4) pers_pos=0;
				} /* if */ 

 				if (pers_substate>=24) {
					pers_state=S_QUIET;
					pers_substate=0;
					pers_pos=0;
				} /* if */ 
				pers_substate++;
				break;

		case S_ENTERING_PASSAGE:
				if (pers_substate==0) {
					char tmp[80],tmp2[80],tmp3[80];
					Sound_release_music();
					sprintf(tmp,"entering");
					sprintf(tmp2,"shrine1");
					sprintf(tmp3,"shrine2");
					Sound_create_music(tmp,tmp2,tmp3);
				} /* if */ 

				pers_substate++;
				if (in_door>=0) {
					next_y=object[in_door].y;
					next_x=object[in_door].x;
				} /* if */ 
				if (pers_substate<=32) {
					if ((pers_substate&0x07)<4) pers_pos=7;
					if ((pers_substate&0x07)>=4) pers_pos=8;
					to_enter_cut--;
				} else {
					if ((pers_substate&0x07)<4) pers_pos=5;
					if ((pers_substate&0x07)>=4) pers_pos=6;
					to_enter_cut++;
				} /* if */ 
				if (pers_substate==32) {
					/* Ha entrado en la puerta: */ 
					STATE=7;
					SUBSTATE=0;
//					HP_filter(map_x,map_y);
//					loadroom(map,map_x,map_y);
//					HP_restore(map_x,map_y);
				} /* if */ 
				if (pers_substate>64) {
					/* Recién salido de la puerta: */ 

					pers_state=S_QUIET;
					pers_substate=0;
					pers_pos=0;
					to_enter_cut=0;
				} /* if */ 
				break;

		case S_ENTERING_WORLD:
				if (pers_substate==0) {
					Sound_release_music();
					Sound_play(S_entering);
				} /* if */ 
				pers_substate++;
				if (in_door>=0) {
					next_y=object[in_door].y+TILE_SIZE_X;
					next_x=object[in_door].x+TILE_SIZE_Y;
				} /* if */ 
				if (pers_substate<=32) {
					if ((pers_substate&0x07)<4) pers_pos=7;
					if ((pers_substate&0x07)>=4) pers_pos=8;
					to_enter_cut++;
				} else {
					if ((pers_substate&0x07)<4) pers_pos=5;
					if ((pers_substate&0x07)>=4) pers_pos=6;
					to_enter_cut--;
				} /* if */ 
				if (pers_substate==32) {
					/* Ha entrado en la puerta: */ 
					STATE=9;
					SUBSTATE=0;
					if (map_x==3 && map_y==12) {
						map=1;
						map_x=2;
						map_y=0;
					} /* if */ 
					if (map_x==10 && map_y==11) {
						map=2;
						map_x=1;
						map_y=4;
					} /* if */ 
					if (map_x==10 && map_y==8) {
						map=3;
						map_x=2;
						map_y=8;
					} /* if */ 
					if (map_x==0 && map_y==8) {
						map=4;
						map_x=0;
						map_y=4;
					} /* if */ 
					if (map_x==8 && map_y==8) {
						map=5;
						map_x=3;
						map_y=0;
					} /* if */ 
					if (map_x==14 && map_y==12) {
						map=6;
						map_x=3;
						map_y=0;
					} /* if */ 
					if (map_x==5 && map_y==2) {
						map=7;
						map_x=3;
						map_y=0;
					} /* if */ 
					if (map_x==4 && map_y==4) {
						map=8;
						map_x=3;
						map_y=0;
					} /* if */ 
					if (map_x==7 && map_y==0) {
						map=9;
						map_x=0;
						map_y=7;
					} /* if */ 
					if (map_x==world10_door_x && map_y==world10_door_y) {
						map=10;
						map_x=0;
						map_y=0;
					} /* if */ 
				} /* if */ 
				if (pers_substate>64) {
					/* Recién salido de la puerta: */ 
					
					pers_state=S_QUIET;
					pers_substate=0;
					pers_pos=0;
					to_enter_cut=0;
				} /* if */ 
				break;

		} /* switch */ 
	} /* if */ 


	/* Comprobar colisiones: */ 
	{
		bool test_x_col=false;
		if (pers_right) val=character_tile[character];
				   else val=character_tile_inv[character];

		found=false;
		previous_y_collision=false;
		if ((tiles[val+pers_pos+MASK_OFFSET]->
				coltest(GAME_VIEW_X+next_x,GAME_VIEW_Y+next_y,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) { 
			if (next_y>pers_y) inc=-1;
						  else inc=1;
			found=false;
			if (next_y>pers_y) {
				if ((tiles[val+pers_pos+MASK_OFFSET]->
					coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
						 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
					for(i=0;!found && i<=TILE_SIZE_X/2;i++) {
						if ((tiles[val+pers_pos+MASK_OFFSET]->
								coltest(GAME_VIEW_X+next_x+i,GAME_VIEW_Y+next_y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
							pers_x=next_x+i;
							pers_y=next_y;
							found=true;
						} else {
							if ((tiles[val+pers_pos+MASK_OFFSET]->
									coltest(GAME_VIEW_X+next_x-i,GAME_VIEW_Y+next_y,
										 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
							pers_x=next_x-i;
							pers_y=next_y;
							found=true;
							} /* if */ 
						} /* if */ 
					} /* for */ 

				} /* if */ 

 				for(next_y+=inc;!found && next_y!=pers_y;next_y+=inc) {
					if ((tiles[val+pers_pos+MASK_OFFSET]->
							coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+next_y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
						pers_y=next_y;
						test_x_col=true;
						found=true;
					} /* if */ 
				} /* for */ 
			} else {
				if (next_y<pers_y) {
 					for(;!found && next_y!=pers_y;next_y+=inc) {
						if ((tiles[val+pers_pos+MASK_OFFSET]->
									coltest(GAME_VIEW_X+next_x,GAME_VIEW_Y+next_y,
										 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
							pers_x=next_x;
							pers_y=next_y;
							found=true;
						} else {
  							for(i=0;!found && i<=TILE_SIZE_X/2;i++) {
								if ((tiles[val+pers_pos+MASK_OFFSET]->
										coltest(GAME_VIEW_X+next_x+i,GAME_VIEW_Y+next_y,
											 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
									pers_x=next_x+i;
									pers_y=next_y;
									found=true;
								} else {
									if ((tiles[val+pers_pos+MASK_OFFSET]->
											coltest(GAME_VIEW_X+next_x-i,GAME_VIEW_Y+next_y,
												 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
									pers_x=next_x-i;
									pers_y=next_y;
									found=true;
									} /* if */ 
								} /* if */ 
							} /* for */ 
						} /* if */ 
					} /* for */ 
					if (!found) previous_y_collision=true;
				} /* if */ 
			} /* if */ 
		} else {
			if (next_y>=pers_y) {
				bool found=false;
				/* Comprobar si ha caido encima de un ransportador: */ 
				for(i=0;!found && i<MAX_LEVERS;i++) {
					if (map==0 && map_x==lever_room_x[i] && map_y==lever_room_y[i] &&		
						pers_y>=lever_y[i]-TILE_SIZE_Y*2 && pers_y<lever_y[i] &&
						pers_x>(lever_x[i]-(TILE_SIZE_X*2-4*TILE_UNIT)) && pers_x<(lever_x[i]+4*TILE_SIZE_X-3*TILE_UNIT)) {
						pers_y=lever_y[i]-TILE_SIZE_Y*2;
						pers_x=next_x;
						found=true;
					} /* if */ 
				} /* for */ 
				if (!found) {
					pers_y=next_y; 
					test_x_col=true;
				} /* if */ 
			} else {
				pers_y=next_y; 
				test_x_col=true;
			} /* if */ 
		} /* if */ 
		
		if (test_x_col || !found) {
			if ((tiles[val+pers_pos+MASK_OFFSET]->
					coltest(GAME_VIEW_X+next_x,GAME_VIEW_Y+pers_y,
						 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
				found=false;
  				for(i=0;!found && i<=TILE_SIZE_X/2;i++) {
					if ((tiles[val+pers_pos+MASK_OFFSET]->
							coltest(GAME_VIEW_X+next_x+i,GAME_VIEW_Y+pers_y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
						pers_x=next_x+i;
						found=true;
					} else {
						if ((tiles[val+pers_pos+MASK_OFFSET]->
								coltest(GAME_VIEW_X+next_x-i,GAME_VIEW_Y+pers_y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
						pers_x=next_x-i;
						found=true;
						} /* if */ 
					} /* if */ 
				} /* for */ 
				previous_x_collision=true;
			} else {
				previous_x_collision=false;
				pers_x=next_x;
			} /* if */ 
		} /* if */ 
	} 

	/* Corregir la posición del personaje: */ 
	if ((pers_state==S_QUIET || pers_state==S_HIT ||
		 pers_state==S_HIT_RECOVERY || pers_state==S_HIT_COLISION) &&
		(tiles[val+pers_pos+MASK_OFFSET]->
			coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
				 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
		bool left_col,right_col,up_col,down_col;
		left_col=(tiles[colision_tile]->
					coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0;
		right_col=(tiles[colision_tile]->
					coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0;
		up_col=(tiles[colision_tile]->
					coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0;
		down_col=(tiles[colision_tile]->
					coltest(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0;

		
		if (left_col && !right_col) pers_x+=TILE_UNIT;
		if (!left_col && right_col) pers_x-=TILE_UNIT;
		if (up_col && !down_col) pers_y+=TILE_UNIT;
		if (!up_col && down_col) pers_y-=TILE_UNIT;
	} /* if */ 

	// Cuando se estaba en una escalera, y un enemigo te ha tocado, no debes salir de la escalera, hasta que no has dejado
	// de colisionar con los bloques atravesables que hay encima de la escalera:
	if (in_ladder!=-1 &&
		(pers_state==S_HIT || pers_state==S_HIT_COLISION)) {
		if ((tiles[val+pers_pos+MASK_OFFSET]->
				coltest(GAME_VIEW_X+next_x,GAME_VIEW_Y+next_y,
					    TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_LADDER_WALL)==0) { 
			in_ladder=-1;
		} /* if */ 
	} /* if */ 

	/* Corregir la posición de la espada: */ 
	if (sword) {
		sword_x+=pers_x-old_x;
		sword_y+=pers_y-old_y;
	} /* if */ 

	if (player_experience[0]>=96) {
		Sound_play(S_powerup);
		player_energy[0]=player_max[0];
		player_experience[0]=0;
	} /* if */ 
	if (player_experience[1]>=96) {
		Sound_play(S_powerup);
		player_energy[1]=player_max[1];
		player_experience[1]=0;
	} /* if */ 

	if (invert_move) {
		SDLKey tmp;
		tmp=LEFT_KEY;
		LEFT_KEY=RIGHT_KEY;
		RIGHT_KEY=tmp;
	} /* if */ 

//	fprintf(fp2,"resulting: %.3i, %.3i\n",pers_x,pers_y);

} /* GameInGameCycle */ 

