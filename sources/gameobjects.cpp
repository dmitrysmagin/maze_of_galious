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


/* Gráficos: */ 
extern Bitmap *konami_bmp,*menu_bmp,*tiles_bmp,*enemy_bmp;
extern int n_tiles;
extern CTile **tiles;

extern BYTE *col_buffer;

/* Estado: */ 
extern int STATE,SUBSTATE;

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
extern int in_ladder;			/* En qué escalera está el personaje	*/ 
extern bool previous_x_collision;	/* Hubo una colisión en X en el frame anterior? */ 
extern bool previous_y_collision;	/* Hubo una colisión en Y en el frame anterior? */ 
extern int hit_time;	/* Contador para saber cuando pueden dañarnos de nuevo	*/ 
extern int character;
extern int current_weapon;
extern int n_fired_arrows,n_fired_mines;
extern int freezed;

extern int in_door;
extern int to_enter_cut;

extern int watermonster_state;

extern int n_arrows,n_coins,n_keys;
extern int player_energy[2],player_experience[2],player_max[2];
extern int player_denergy[2],player_dexperience[2];
extern bool item[38];
extern bool world_item[10][4];
extern bool cross,world_key;
extern int next_world;
extern bool world_doors_open[10];
extern int bible_counter,bible_subcounter;

extern int room_size_x,room_size_y;
extern int *room_tiles;

/* Objetos: (escaleras, piedras, etc...) */ 
extern MOGObject object[MAX_OBJECTS];
extern int n_objects;

extern int wall_tile;
extern int stone_tile,ladder_tile,wdoor_tile,keydoor_tile,stats_tile;
extern int coin_tile,arrow_tile,key_tile,door_tile;
extern int character_tile[2],character_tile_inv[2];
extern int colision_tile;
extern int sword_tile[2],explosion_tile;
extern int worm_tile,bat_tile,skeleton_tile,smoke_tile,slime_tile,jumpingbush_tile;
extern int bluespider_tile,whitefireball_tile,fstone_tile,knight_tile,blob_tile;
extern int porcupine_tile,fairy_tile,whitebear_tile,feet_tile,redjumper_tile;
extern int items_tile,armas_tile,lever_tile,livingwall_tile,watermonster_tile;
extern int lava_tile,piranha_tile,knighthead_tile,chicken_tile,rockman_tile,bfly_tile;
extern int armourarrow_tile,ghost_tile,head_tile,worm2_tile,hairy_tile,waterbug_tile;
extern int stoneman_tile,pacman_tile,reddragon_tile,greenmonkey_tile,plant_tile;
extern int witch2_tile,flame_tile,transformer_tile,cyclops_tile,lcloud_tile,glizard_tile;
extern int snake_tile,bdemon_tile,gorilla_tile,pamperseball_tile;

extern int worm_state_tile[28];
extern int worm_state_tile_i[28];
extern int worm_state_y[28];


/* Efectos de sonido: */ 
extern SOUNDT S_powerup,S_jumptree,S_enemybullet,S_headbullet;
extern SOUNDT S_stones,S_blob,S_flapwings,S_karrow,S_lightning;
extern SOUNDT S_fireball2,S_bearbullet,S_chickenbomb,S_colormonster;
extern SOUNDT S_rockman,S_enterwater,S_owl,S_flamebullet,S_firebreath;
extern SOUNDT S_snakebullet,S_gorilla,S_lizardtongue,S_bdemonbullet;


/* Control de pantalla: */ 
extern int logic_dx[4],logic_dy[4],logic_x[4],logic_y[4];
extern int physic_dx[4],physic_dy[4],physic_x[4],physic_y[4];

extern int zoom;





void GameObjectAnimation(int dx,int dy) 
{
	int i,j;
	int t_x,t_y;
	int colision;
	int first_jumpingbush=true;

	/* Animaciones de objetos: */ 
	for(i=0;i<n_objects;i++) {
		if (object[i].last_hit>0) object[i].last_hit--;

		if (object[i].type==T_BROKEN_STONE) {
			object[i].size--;
			if (object[i].size==0) {
				if (object[i].state2==0) {
					object[i].type=T_NADA;
				} else {
					/* Dar objetos específicos: */ 
					switch(object[i].state2) {
					case -1:object[i].type=T_ITEM;
							object[i].size=10;
							object[i].state=-1;
							break;
					case -2:object[i].type=T_ITEM;
							object[i].size=2;
							object[i].state=-2;
							break;
					case -3:object[i].type=T_ITEM;
							object[i].size=50;
							object[i].state=-3;
							break;
					default:/* Dar objetos que no sean monedas,llaves o flechas:	*/ 
							if (object[i].state2<37) {
								/* Arma o item: */ 
								if (!item[object[i].state2-1]) {
									object[i].type=T_ITEM;
									object[i].state=object[i].state2-1;
									if (object[i].state>=32) {
										/* Los objetos arma son más pequeños: */ 
										object[i].y+=TILE_SIZE_Y;
									} /* if */ 
								} else {
									object[i].type=T_NADA;
								} /* if */ 
							} else {
								object[i].type=T_NADA;
								if (object[i].state2==38 && next_world==0 && !world_key) {
									/* Llave de mundo: */ 
									object[i].type=T_ITEM;
									object[i].state=37;
								} /* if */ 
								if (object[i].state2==37 && !cross) {
									/* Cruz: */ 
									object[i].type=T_ITEM;
									object[i].state=36;
								} /* if */ 
								if (object[i].state2==39) {
									/* Enemigo: */ 
									object[i].type=T_ENEMY;
									object[i].enemy=T_PAMPERSE_BALL;
									object[i].size=20;
									object[i].state=0;
									object[i].state2=0;
									object[i].base_tile=pamperseball_tile;
									object[i].tile=0;
								} /* if */ 
								if (object[i].state2==40) {
									/* Pamperse: */ 
									object[i].type=T_PAMPERSE;
								} /* if */ 
							} /* if */ 
					} /* switch */ 
				} /* if */ 
			} /* if */ 
		} /* if */ 

		if (object[i].type==T_EXPLOSION) {
			object[i].size--;
			if (object[i].size==0) {
				switch(object[i].state) {
				case 0:
					object[i].type=T_NADA;
					break;
				case 1:
					switch(rand()%30) {
					case 0:
					case 2:
					case 3: object[i].type=T_ITEM;
							object[i].size=1;
							object[i].state=-1;
							break;
					case 4:
					case 5:	object[i].type=T_ITEM;
							object[i].size=1;
							object[i].state=-2;
							break;
					case 6:	
					case 7:	
					case 8:	object[i].type=T_ITEM;
							object[i].size=10;
							object[i].state=-3;
							break;
					default:object[i].type=T_NADA;
					} /* switch */ 
					break;
				case 2:
					switch(rand()%30) {
					case 0:
					case 1:
					case 2:	object[i].type=T_ITEM;
							object[i].size=5;
							object[i].state=-1;
							break;
					case 3:
					case 4:	object[i].type=T_ITEM;
							object[i].size=1;
							object[i].state=-2;
							break;
					case 5:
					case 6:	
					case 7:	object[i].type=T_ITEM;
							object[i].size=20;
							object[i].state=-3;
							break;
					default:object[i].type=T_NADA;
					} /* switch */ 
					break;
				case 3:
					switch(rand()%3) {
					case 0: object[i].type=T_ITEM;
							object[i].size=1;
							object[i].state=-1;
							break;
					case 1: object[i].type=T_ITEM;
							object[i].size=1;
							object[i].state=-2;
							break;
					case 2:	object[i].type=T_ITEM;
							object[i].size=10;
							object[i].state=-3;
							break;
					default:object[i].type=T_NADA;
					} /* switch */ 
					break;
				} /* switch */ 
			} /* if */ 
		} /* if */ 

		if (object[i].type==T_WEAPON_ARROW ||
			object[i].type==T_WEAPON_ARROW2) {
			if (object[i].type==T_WEAPON_ARROW) {
				if (object[i].state==0) object[i].x+=TILE_SIZE_X;
								   else object[i].x-=TILE_SIZE_X;
			} /* if */ 
			if (object[i].type==T_WEAPON_ARROW2) {
				if (object[i].state==0) object[i].x+=TILE_SIZE_X/2;
								   else object[i].x-=TILE_SIZE_X/2;
			} /* if */ 

			if (object[i].x<(-2*TILE_SIZE_X) ||
				object[i].x>(room_size_x*TILE_SIZE_X)) {
				object[i].type=T_NADA;
				n_fired_arrows--;
			} /* if */ 
		} /* if */ 

		if (object[i].type==T_RFL_ARROW ||
			object[i].type==T_RFL_ARROW2 ||
			object[i].type==T_RFL_FIRE ||
			object[i].type==T_RFL_RFIRE) {
			if (object[i].type==T_RFL_ARROW ||
				object[i].type==T_RFL_FIRE ||
				object[i].type==T_RFL_RFIRE) {
				if (object[i].state==0) object[i].x-=TILE_SIZE_X;
								   else object[i].x+=TILE_SIZE_X;
			} /* if */ 
			if (object[i].type==T_RFL_ARROW2) {
				if (object[i].state==0) object[i].x-=TILE_SIZE_X/2;
								   else object[i].x+=TILE_SIZE_X/2;
			} /* if */ 
			object[i].y-=TILE_SIZE_Y;

			if (object[i].x<(-2*TILE_SIZE_X) ||
				object[i].x>(room_size_x*TILE_SIZE_X) ||
				object[i].y<0) {
				n_fired_arrows--;
				object[i].type=T_NADA;
			} /* if */ 
		} /* if */ 

		if (object[i].type==T_WEAPON_FIRE) {
			if (object[i].state==0) object[i].x+=TILE_SIZE_X/2;
							   else object[i].x-=TILE_SIZE_X/2;
			if (object[i].x<(-2*TILE_SIZE_X) ||
				object[i].x>(room_size_x*TILE_SIZE_X) ||
				object[i].y>(room_size_y*TILE_SIZE_Y)) {
				object[i].type=T_NADA;
				n_fired_arrows--;
			} /* if */ 
		} /* if */ 

		if (object[i].type==T_WEAPON_RFIRE) {
			object[i].state2++;
			if (((object[i].state2>>2)&0x01)==0) object[i].tile=2;
											else object[i].tile=21;
			switch(object[i].state) {
			case 0:object[i].x+=TILE_SIZE_X/2;
				  break;
			case 1:object[i].x-=TILE_SIZE_X/2;
				  break;
			case 2:object[i].y-=TILE_SIZE_Y/2;
				  break;
			case 3:object[i].y+=TILE_SIZE_Y/2;
				  break;
			} /* switch */ 
			if (object[i].x<(-2*TILE_SIZE_X) ||
				object[i].x>(room_size_x*TILE_SIZE_X) ||
				object[i].y>(room_size_y*TILE_SIZE_Y) ||
				object[i].state2>=64) {
				object[i].type=T_NADA;
				n_fired_arrows--;
			} /* if */ 
		} /* if */ 


		if (object[i].type==T_WEAPON_MINE) {
			int off[4]={0,1,2,1};
			object[i].state++;
			object[i].tile=9+off[(object[i].state>>2)%4];
		} /* if */ 

		if (object[i].type==T_MINE_EXP) {
			object[i].state++;
			object[i].tile=12+((object[i].state>>2)%2);
			if (object[i].state>=64) object[i].type=T_NADA;
		} /* if */ 


		if (object[i].type==T_FAIRY_BUBBLE) {
			object[i].state3=(object[i].state3+1)%16;

			object[i].x+=object[i].state;
			object[i].y+=object[i].state2;

			if (object[i].x<2*TILE_SIZE_X) {
				object[i].state=(rand()%2)+1;
				if (object[i].y<10*TILE_SIZE_Y) {
					object[i].state2=(rand()%2)+1;
				} else {
					object[i].state2=-((rand()%2)+1);
				} /* if */ 
			} /* if */ 
			if (object[i].x>30*TILE_SIZE_X) {
				object[i].state=-((rand()%2)+1);
				if (object[i].y<10*TILE_SIZE_Y) {
					object[i].state2=(rand()%2)+1;
				} else {
					object[i].state2=-((rand()%2)+1);
				} /* if */ 
			} /* if */ 
			if (object[i].y<2*TILE_SIZE_Y) {
				object[i].state2=(rand()%2)+1;
				if (object[i].x<16*TILE_SIZE_X) {
					object[i].state=(rand()%2)+1;
				} else {
					object[i].state=-((rand()%2)+1);
				} /* if */ 
			} /* if */ 
			if (object[i].y>18*TILE_SIZE_Y) {
				object[i].state2=-((rand()%2)+1);
				if (object[i].x<16*TILE_SIZE_X) {
					object[i].state=(rand()%2)+1;
				} else {
					object[i].state=-((rand()%2)+1);
				} /* if */ 
			} /* if */ 
		} /* if */ 

		if (object[i].type==T_FAIRY) {
			object[i].state3++;
			object[i].x+=object[i].state;
			object[i].y+=object[i].state2;

			if (object[i].state3==112) {
				object[i].size=1;
				object[i].state=(rand()%2)+1;
				if ((rand()%2)==0) object[i].state=-object[i].state;
				object[i].state2=(rand()%2)+1;
				if ((rand()%2)==0) object[i].state2=-object[i].state2;
			} /* if */ 
			if (object[i].size==1) {
				if (((object[i].state3)&0x1f)==0) {
					object[i].state=(rand()%2)+1;
					if ((rand()%2)==0) object[i].state=-object[i].state;
					object[i].state2=(rand()%2)+1;
					if ((rand()%2)==0) object[i].state2=-object[i].state2;
				} /* if */ 
				if (object[i].state3>=320) object[i].size=2;
			} /* if */ 
			if (object[i].size>=1) {
				if (object[i].x<2*TILE_SIZE_X) {
					object[i].state=(rand()%2)+1;
					if (object[i].y<10*TILE_SIZE_Y) {
						object[i].state2=(rand()%2)+1;
					} else {
						object[i].state2=-((rand()%2)+1);
					} /* if */ 
				} /* if */ 
				if (object[i].x>30*TILE_SIZE_X) {
					object[i].state=-((rand()%2)+1);
					if (object[i].y<10*TILE_SIZE_Y) {
						object[i].state2=(rand()%2)+1;
					} else {
						object[i].state2=-((rand()%2)+1);
					} /* if */ 
				} /* if */ 
				if (object[i].y<2*TILE_SIZE_Y) {
					object[i].state2=(rand()%2)+1;
					if (object[i].x<16*TILE_SIZE_X) {
						object[i].state=(rand()%2)+1;
					} else {
						object[i].state=-((rand()%2)+1);
					} /* if */ 
				} /* if */ 
				if (object[i].y>18*TILE_SIZE_Y) {
					object[i].state2=-((rand()%2)+1);
					if (object[i].x<16*TILE_SIZE_X) {
						object[i].state=(rand()%2)+1;
					} else {
						object[i].state=-((rand()%2)+1);
					} /* if */ 
				} /* if */ 
			} /* if */ 

		} /* if */ 

		if (object[i].type==T_ARMOUR) {
			if (object[i].state==0) {
				if (pers_y==object[i].y+TILE_SIZE_Y &&
					pers_x<object[i].x &&
					(pers_x+8*TILE_SIZE_X)>object[i].x) {
					object[i].state2++;
				} else {
					object[i].state2=0;
				} /* if */ 
				if (object[i].state2>=64 &&
					(rand()%32)==0) {
					int o;
					/* Disparar flechas: */ 
					Sound_play(S_karrow);
					o=add_object(T_ENEMY,object[i].x-2*TILE_SIZE_X,object[i].y,0,T_ARMOUR_ARROW,0);
					object[o].base_tile=armourarrow_tile;
					object[i].tile=0;
				} /* if */ 
			} else {
				if (pers_y==object[i].y+TILE_SIZE_Y &&
					pers_x>object[i].x &&
					(pers_x-8*TILE_SIZE_X)<object[i].x) {
					object[i].state2++;
				} else {
					object[i].state2=0;
				} /* if */ 
				if (object[i].state2>=64 &&
					(rand()%32)==0) {
					int o;
					/* Disparar flechas: */ 
					Sound_play(S_karrow);
					o=add_object(T_ENEMY,object[i].x+2*TILE_SIZE_X,object[i].y,0,T_ARMOUR_ARROW,1);
					object[o].base_tile=armourarrow_tile;
					object[i].tile=1;
				} /* if */ 
			} /* if */ 
		} /* if */ 

		if (object[i].type==T_ENEMY && bible_subcounter==0) {
			colision=T_WALL|T_LADDER_WALL|T_DOOR_WALL;
			switch(object[i].enemy) {
			case T_WORM:
				if (object[i].state==0) object[i].state=1;
				if (object[i].state>0) object[i].state++;
								  else object[i].state--;
				if (object[i].state>=29) object[i].state=1;
				if (object[i].state<=-29) object[i].state=-1;

				if (object[i].state>0) {
					if (
						(tiles[worm_tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     (object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
						(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						if ((object[i].state%2)==0) object[i].x+=TILE_UNIT;
					} else {
						if (object[i].state>20) object[i].state=-21;
					} /* if */ 
				} else {
					if (
						(tiles[worm_tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     object[i].x>0 &&
						(col_buffer[GAME_VIEW_X+object[i].x-1+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						if ((object[i].state%2)==0) object[i].x-=TILE_UNIT;
					} else {
						if (object[i].state<-20) object[i].state=21;
					} /* if */ 
				} /* if */ 
				break;

			case T_BAT:
				switch(object[i].state) {
				case 0:/* Murcialago Quieto: */ 
					object[i].state2++;
					if (object[i].state2>=64) {
						int x,y;

						x=object[i].x/TILE_SIZE_X;
						y=object[i].y/TILE_SIZE_Y;
						object[i].state=1;
						object[i].state2=0;
						object[i].state3=-1;
						if (iswall(x,y) || !iswall(x,y-1)) {
							object[i].state2=16;
							object[i].state3=0;
						} /* if */ 
					} /* if */ 
					break;
				case 1:/* Murcielago moviendose: */ 
					switch(object[i].state3) {
					case -1:object[i].y+=TILE_UNIT;
						break;
					case 0:object[i].y+=TILE_UNIT*2;
						break;
					case 1:object[i].y+=TILE_UNIT*2;
						   object[i].x-=TILE_UNIT*2;
						break;
					case 2:object[i].y-=TILE_UNIT*2;
						   object[i].x-=TILE_UNIT*2;
						break;
					case 3:object[i].y-=TILE_UNIT*2;
						break;
					case 4:object[i].y-=TILE_UNIT*2;
						   object[i].x+=TILE_UNIT*2;
						break;
					case 5:object[i].y+=TILE_UNIT*2;
						   object[i].x+=TILE_UNIT*2;
						break;
					case 6:object[i].x+=TILE_UNIT*2;
						break;
					case 7:object[i].x-=TILE_UNIT*2;
						break;

					case 8:object[i].y+=TILE_UNIT*2;
						   object[i].x-=TILE_UNIT;
						break;
					case 9:object[i].y-=TILE_UNIT*2;
						   object[i].x-=TILE_UNIT;
						break;
					case 10:object[i].y-=TILE_UNIT*2;
						   object[i].x+=TILE_UNIT;
						break;
					case 11:object[i].y+=TILE_UNIT*2;
						   object[i].x+=TILE_UNIT;
						break;

					} /* switch */ 
					object[i].state2++;
					if ((object[i].state2>=16 && object[i].state3!=-1) ||
						(object[i].state2>=32 && object[i].state3==-1)) {
						t_x=object[i].x/TILE_SIZE_X;
						t_y=object[i].y/TILE_SIZE_Y;
						if (iswall(t_x,t_y-1) && iswall(t_x+1,t_y-1) &&
							!iswall(t_x,t_y) && !iswall(t_x+1,t_y) &&
							(rand()%8)==0 && t_y!=0) {
							object[i].state=0;
							object[i].state2=0;
						} else {
							object[i].state=1;
							object[i].state2=0;

							/* Si el número aleatorio es mayor que 7, se sigue en la	*/ 
							/* misma dirección.											*/ 
							j=object[i].state3;
							if (j==-1) object[i].state3=rand()%12;
								  else object[i].state3=rand()%20;
							if (object[i].state3>11) object[i].state3=j;

							if (object[i].x>(room_size_x-7)*TILE_SIZE_X) {
								if (object[i].state3==4) object[i].state3=2;
								if (object[i].state3==5) object[i].state3=1;
								if (object[i].state3==6) object[i].state3=7;
								if (object[i].state3==10) object[i].state3=9;
								if (object[i].state3==11) object[i].state3=8;
							} /* if */ 
							if (object[i].x<TILE_SIZE_X*5) {
								if (object[i].state3==2) object[i].state3=4;
								if (object[i].state3==1) object[i].state3=5;
								if (object[i].state3==7) object[i].state3=6;
								if (object[i].state3==9) object[i].state3=10;
								if (object[i].state3==8) object[i].state3=11;
							} /* if */ 
							if (object[i].y>(room_size_y-6)*TILE_SIZE_Y) {
								if (object[i].state3==0) object[i].state3=3;
								if (object[i].state3==1) object[i].state3=2;
								if (object[i].state3==5) object[i].state3=4;
								if (object[i].state3==11) object[i].state3=10;
								if (object[i].state3==8) object[i].state3=9;
							} /* if */ 
							if (object[i].y<TILE_SIZE_Y*5) {
								if (object[i].state3==3) object[i].state3=0;
								if (object[i].state3==2) object[i].state3=1;
								if (object[i].state3==4) object[i].state3=5;
								if (object[i].state3==10) object[i].state3=11;
								if (object[i].state3==9) object[i].state3=8;
							} /* if */ 
						} /* if */ 
					} /* if */ 
					break;
				} /* switch */ 
				break;

			case T_SKELETON:
				object[i].state2++;

				if (object[i].size==8) {
					if ((rand()%128)==0 && ((object[i].state2>>2)%2)==0 && object[i].state<2) {
						object[i].state+=2;
						object[i].state2=0;
					} /* if */ 
					if (object[i].state==0) {
						if (
							(tiles[skeleton_tile]->
								coltest(GAME_VIEW_X+object[i].x-TILE_UNIT,GAME_VIEW_Y+object[i].y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							(object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
							(col_buffer[GAME_VIEW_X+object[i].x-1+
										(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
							object[i].x-=TILE_UNIT;
							if ((rand()%32)==0) { 
								/* Saltar: */ 
								int tx,ty;
								int nfloors=0,floors[8];
								tx=object[i].x/TILE_SIZE_X;
								ty=object[i].y/TILE_SIZE_Y;
								if (tx>5 &&
									!iswall(tx-3,ty-3) &&
									!iswall(tx-4,ty-3) &&
									!iswall(tx-5,ty-3) &&
									iswall(tx-3,ty-2) &&
									iswall(tx-4,ty-2) &&
									iswall(tx-5,ty-2) &&
									!iswall(tx-1,ty-2) &&
									(tiles[skeleton_tile]->
										coltest(GAME_VIEW_X+object[i].x-4*TILE_SIZE_X,GAME_VIEW_Y+object[i].y-4*TILE_SIZE_Y,
											TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
									) {
									floors[nfloors++]=ty-4;
								} /* if */ 
								if (tx>5 &&
									!iswall(tx-3,ty+5) &&
									!iswall(tx-4,ty+5) &&
									!iswall(tx-5,ty+5) &&
									iswall(tx-3,ty+6) &&
									iswall(tx-4,ty+6) &&
									iswall(tx-5,ty+6) &&
									!iswall(tx-1,ty+2) &&
									(tiles[skeleton_tile]->
										coltest(GAME_VIEW_X+object[i].x-(4*TILE_SIZE_X+6*TILE_UNIT),GAME_VIEW_Y+object[i].y+4*TILE_SIZE_Y,
											TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
									) {
									floors[nfloors++]=ty+4;
								} /* if */ 
								if (nfloors!=0) {
									object[i].desired_floor=floors[rand()%nfloors]*TILE_SIZE_Y;
									object[i].state=5;
									object[i].state3=0;
								} /* if */ 
							} /* if */ 

						} else {
							object[i].state=1;
							object[i].state2=0;
							object[i].x+=TILE_UNIT*2;
						} /* if */ 
					} /* if */ 
					if (object[i].state==1) {
						if (
							(tiles[skeleton_tile+2]->
								coltest(GAME_VIEW_X+object[i].x+TILE_UNIT,GAME_VIEW_Y+object[i].y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							 object[i].x>0 &&
							(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
										(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
							object[i].x+=TILE_UNIT;
							if ((rand()%32)==0) { 
								/* Saltar: */ 
								int tx,ty;
								int nfloors=0,floors[8];
								tx=object[i].x/TILE_SIZE_X;
								ty=object[i].y/TILE_SIZE_Y;
								if (tx+7<room_size_x &&
									!iswall(tx+5,ty-3) &&
									!iswall(tx+6,ty-3) &&
									!iswall(tx+7,ty-3) &&
									iswall(tx+5,ty-2) &&
									iswall(tx+6,ty-2) &&
									iswall(tx+7,ty-2) &&
									!iswall(tx+3,ty-2) &&
									(tiles[skeleton_tile+2]->
										coltest(GAME_VIEW_X+object[i].x+4*TILE_SIZE_X,GAME_VIEW_Y+object[i].y-4*TILE_SIZE_Y,
											TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
									) {
									floors[nfloors++]=ty-4;
								} /* if */ 
								if (tx+6<room_size_x &&
									!iswall(tx+5,ty+5) &&
									!iswall(tx+6,ty+5) &&
									!iswall(tx+7,ty+5) &&
									iswall(tx+5,ty+6) &&
									iswall(tx+6,ty+6) &&
									iswall(tx+7,ty+6) &&
									!iswall(tx+3,ty+2) &&
									(tiles[skeleton_tile+2]->
										coltest(GAME_VIEW_X+object[i].x+4*TILE_SIZE_X+6*TILE_UNIT,GAME_VIEW_Y+object[i].y+4*TILE_SIZE_Y,
											TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
									) {
									floors[nfloors++]=ty+4;
								} /* if */ 
								if (nfloors!=0) {
									object[i].desired_floor=floors[rand()%nfloors]*TILE_SIZE_Y;
									object[i].state=4;
									object[i].state3=0;
								} /* if */ 
							} /* if */ 

						} else {
							object[i].state=0;
							object[i].state2=0;
							object[i].x-=TILE_UNIT*2;
						} /* if */ 
					} /* if */ 
					if ((object[i].state==2 || object[i].state==3) && object[i].state2>=64) {
						object[i].state-=2;
						object[i].state2=0;
					} /* if */ 

				} else {
					if (object[i].state2>=2) {
						object[i].size++;
						if (object[i].state>=2) object[i].state-=2;
						object[i].state2=0;
					} /* if */ 
				} /* if */ 
				/* Estados de salto: */ 
				if (object[i].state==4) { 
					object[i].x+=2*TILE_UNIT;
					if (object[i].desired_floor<object[i].y) {
						object[i].state3=0;
						object[i].y-=4*TILE_UNIT;
					} else {
						if (object[i].state3<2) object[i].y-=TILE_UNIT*2;
						if (object[i].state3>=2 && object[i].state3<4) object[i].y-=TILE_UNIT;
						if (object[i].state3>=6) {
							object[i].state=6;
							object[i].state3=0;
						} /* if */ 
					} /* if */ 	
					object[i].state3++;
				} /* if */ 
				if (object[i].state==5) { 
					object[i].x-=2*TILE_UNIT;
					if (object[i].desired_floor<object[i].y) {
						object[i].state3=0;
						object[i].y-=4*TILE_UNIT;
					} else {
						if (object[i].state3<2) object[i].y-=TILE_UNIT*2;
						if (object[i].state3>=2 && object[i].state3<4) object[i].y-=TILE_UNIT;
						if (object[i].state3>=6) {
							object[i].state=7;
							object[i].state3=0;
						} /* if */ 
					} /* if */ 
					object[i].state3++;
				} /* if */ 
				if (object[i].state==6) { 
					object[i].x+=2*TILE_UNIT;
					if (object[i].state3>=2) object[i].y+=TILE_UNIT;
					if (object[i].state3>=4) object[i].y+=TILE_UNIT;
					if (object[i].state3>=6) object[i].y+=TILE_UNIT*2;
					if (object[i].state3>=10) object[i].y+=TILE_UNIT*2;
					if (object[i].y>=object[i].desired_floor) {
						object[i].y=object[i].desired_floor;
						object[i].state=1;
						object[i].state3=0;
					} /* if */ 
					object[i].state3++;
				} /* if */ 
				if (object[i].state==7) { 
					object[i].x-=2*TILE_UNIT;
					if (object[i].state3>=2) object[i].y+=TILE_UNIT;
					if (object[i].state3>=4) object[i].y+=TILE_UNIT;
					if (object[i].state3>=6) object[i].y+=TILE_UNIT*2;
					if (object[i].state3>=10) object[i].y+=TILE_UNIT*2;
					if (object[i].y>=object[i].desired_floor) {
						object[i].y=object[i].desired_floor;
						object[i].state=0;
						object[i].state3=0;
					} /* if */ 
					object[i].state3++;
				} /* if */     
				break;
			case T_SMOKE:
				object[i].state++;
				if (object[i].state2==0) {
					if (object[i].state>=48) {
						object[i].state2++;
						object[i].state=0;
					} /* if */ 
				} else {
					if (object[i].state2==4) {
						if (object[i].state>=51) {
							object[i].state2++;
							object[i].state=0;
						} /* if */ 

					} else {
						if (object[i].state>=33) {
							object[i].state2++;
							object[i].state=0;
						} /* if */ 
						if (object[i].state2>=8) object[i].state2=0;
					} /* if */ 
				} /* if */ 
				break;
			case T_SLIME:
				if (object[i].state==0) {
					/* Moviendose a la izquierda: */ 
					if ((tiles[slime_tile]->
							coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y+TILE_UNIT,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
						/* Cayendo: */ 
						object[i].y+=TILE_UNIT;
					} else {
						if ((tiles[slime_tile]->
								coltest(GAME_VIEW_X+object[i].x-TILE_UNIT,GAME_VIEW_Y+object[i].y,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
							object[i].state=1;
							object[i].x+=TILE_UNIT;
							object[i].state2++;
						} else {
							object[i].x-=TILE_UNIT;
							object[i].state2++;
						} /* if */ 
					} /* if */ 

				} else {
					/* Moviendose a la derecha: */ 
					if ((tiles[slime_tile+2]->
							coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y+TILE_UNIT,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
						/* Cayendo: */ 
						object[i].y+=TILE_UNIT;
					} else {
						if ((tiles[slime_tile+2]->
								coltest(GAME_VIEW_X+object[i].x+TILE_UNIT,GAME_VIEW_Y+object[i].y,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
							object[i].state=0;
							object[i].x-=TILE_UNIT;
							object[i].state2++;
						} else {
							object[i].x+=TILE_UNIT;
							object[i].state2++;
						} /* if */ 
					} /* if */ 
				} /* if */ 

				if (object[i].state2>=12) object[i].state2=0;
				/* Si se sale de pantalla, eliminarla: */ 
				if (object[i].x>=dx || object[i].x<=(-2*TILE_SIZE_X) ||
					object[i].y>=dy) {
					object[i].type=T_NADA;
				} /* if */ 
				break;

			case T_BOUNCINGBALL:
				object[i].tile=0;
				if (object[i].state2==0) {
					object[i].state-=TILE_UNIT*object[i].state3;
					if (iswall(object[i].x/TILE_SIZE_X,(object[i].y+object[i].state)/TILE_SIZE_Y)) object[i].state2=1;
				} else {
					object[i].state+=TILE_UNIT*object[i].state3;
					if (object[i].state>=0) object[i].state2=0;
				} /* if */ 
				break;

			case T_WATERMONSTER:
				if (object[i].state<16 && (object[i].state&0x01)==0) object[i].state2+=TILE_UNIT;
				if (object[i].state==16) {
					int pos;
					int val;

					if (object[i].x<pers_x) {
						pos=pers_x-TILE_SIZE_X;
					} else {
						pos=pers_x+2*TILE_SIZE_X;
					} /* if */ 
					val=add_object(T_ENEMY,pos,15*TILE_SIZE_Y,5,T_WATERMONSTER_ARM,0);
					object[val].base_tile=watermonster_tile;
				} /* if */ 

				if (object[i].state==24 || object[i].state==64) {
					Sound_play(S_enemybullet);
					j=add_object(T_ENEMY,object[i].x+TILE_SIZE_X,object[i].y,0,T_BUBBLE,0);
					object[j].state=(pers_x)-object[i].x;
					object[j].state2=(pers_y)-(object[i].y+TILE_SIZE_Y);
					object[j].base_tile=watermonster_tile;
					object[j].tile=4;
				} /* if */ 

				if (object[i].state>=16 && object[i].state<32) {
					object[i].state3++;
				} /* if */ 
				if (object[i].state>=32 && object[i].state<48) {
					object[i].state3--;
				} /* if */ 

				if (object[i].state>=80 && (object[i].state&0x01)==0) object[i].state2-=TILE_UNIT;
				if (object[i].state>=96) {
					object[i].type=T_NADA;
					watermonster_state=-96;
				} /* if */ 
				object[i].state++;
				break;

			case T_WATERMONSTER_ARM:
				if (object[i].state<24) object[i].state2++;
								   else object[i].state2--;
				if (object[i].state>=48) object[i].type=T_NADA;
				object[i].state++;
				break;

			case T_BUBBLE:
				if (abs(object[i].state)>abs(object[i].state2)) {
					object[i].x+=(object[i].state>0 ? TILE_UNIT:-TILE_UNIT);
					object[i].state3-=abs(object[i].state2);
					if (object[i].state3<0) {
						object[i].state3+=abs(object[i].state);
						object[i].y+=(object[i].state2>0 ? TILE_UNIT:-TILE_UNIT);
					} /* if */ 
				} else {
					object[i].y+=(object[i].state2>0 ? TILE_UNIT:-TILE_UNIT);
					object[i].state3-=abs(object[i].state);
					if (object[i].state3<0) {
						object[i].state3+=abs(object[i].state2);
						object[i].x+=(object[i].state>0 ? TILE_UNIT:-TILE_UNIT);
					} /* if */ 
				} /* if */ 
				if (object[i].x<0 || object[i].x>=dx ||
					object[i].y<0 || object[i].y>=dy) {
					object[i].type=T_NADA;
				} /* if */ 
				break;

			case T_FSTONE:
				{
					int colision=T_WALL|T_LADDER_WALL;
					object[i].state3++;
					if (object[i].state2<=0) {
						if (object[i].y>=0 &&
							(tiles[fstone_tile+((object[i].state3>>2)&0x03)]->
								coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							(tiles[fstone_tile+((object[i].state3>>2)&0x03)]->
								coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y+2*TILE_UNIT,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
							object[i].state2=23;
							object[i].state=rand()%2;
							Sound_play(S_stones);
						} else {
							object[i].y+=4*TILE_UNIT;
						} /* if */ 
					
					} else {
						if (object[i].state2<4) object[i].y+=4*TILE_UNIT;
						if (object[i].state2>=4 && object[i].state2<8) object[i].y+=2*TILE_UNIT;
						if (object[i].state2>=8 && object[i].state2<12) object[i].y+=TILE_UNIT;
						if (object[i].state2>=14 && object[i].state2<18) object[i].y-=TILE_UNIT;
						if (object[i].state2>=18 && object[i].state2<22) object[i].y-=2*TILE_UNIT;
						if (object[i].state2>=22) object[i].y-=4*TILE_UNIT;
						object[i].state2--;
					} /* if */ 
					if (object[i].state==0) object[i].x-=TILE_UNIT;
									   else object[i].x+=TILE_UNIT;

					if (object[i].x>=dx || object[i].x<=(-2*TILE_SIZE_X) ||
						object[i].y>=dy) {
						object[i].type=T_NADA;
					} /* if */ 
				}
				break;

			case T_RSTONE:
				object[i].state3++;
				if (object[i].state==0) {
					/* Moviendose a la izquierda: */ 
					if ((tiles[slime_tile]->
							coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y+2*TILE_UNIT,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
						/* Cayendo: */ 
						object[i].y+=TILE_UNIT;
						if (object[i].state2<4) object[i].y+=4*TILE_UNIT;
						if (object[i].state2>=4 && object[i].state2<8) object[i].y+=2*TILE_UNIT;
						if (object[i].state2>=8) object[i].y+=TILE_UNIT;
						object[i].state2--;
						object[i].x-=TILE_UNIT;
					} else {
						object[i].y=(object[i].y/TILE_SIZE_Y)*TILE_SIZE_Y;
						if (object[i].state2!=12) Sound_play(S_stones);
						object[i].state2=12;
						if ((tiles[slime_tile]->
								coltest(GAME_VIEW_X+object[i].x-TILE_UNIT,GAME_VIEW_Y+object[i].y,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
							object[i].state=1;
							object[i].x+=TILE_UNIT;
						} else {
							object[i].x-=TILE_UNIT;
						} /* if */ 
					} /* if */ 

				} else {
					/* Moviendose a la derecha: */ 
					if ((tiles[slime_tile]->
							coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y+2*TILE_UNIT,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
						/* Cayendo: */ 
						object[i].y+=TILE_UNIT;
						if (object[i].state2<4) object[i].y+=4*TILE_UNIT;
						if (object[i].state2>=4 && object[i].state2<8) object[i].y+=2*TILE_UNIT;
						if (object[i].state2>=8) object[i].y+=TILE_UNIT;
						object[i].state2--;
						object[i].x+=TILE_UNIT;
					} else {
						object[i].y=(object[i].y/TILE_SIZE_Y)*TILE_SIZE_Y;
						if (object[i].state2!=12) Sound_play(S_stones);
						object[i].state2=12;
						if ((tiles[slime_tile]->
								coltest(GAME_VIEW_X+object[i].x+TILE_UNIT,GAME_VIEW_Y+object[i].y,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
							object[i].state=0;
							object[i].x-=TILE_UNIT;
						} else {
							object[i].x+=TILE_UNIT;
						} /* if */ 
					} /* if */ 
				} /* if */ 

				/* Si se sale de pantalla, eliminarla: */ 
				if (object[i].x>=dx || object[i].x<=(-2*TILE_SIZE_X) ||
					object[i].y>=dy) {
					object[i].type=T_NADA;
				} /* if */ 
			break;

			case T_KNIGHT:
				object[i].state3++;
				switch(object[i].state) {
				case 10: // Igual, pero no puede subir/bajar escaleras
				case 0:
						object[i].in_ladder=-1;
						object[i].state2=2+((object[i].state3>>2)&0x01);
					    if ((tiles[knight_tile+object[i].state2]->
							coltest(GAME_VIEW_X+object[i].x+TILE_UNIT,GAME_VIEW_Y+object[i].y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							(object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
							(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
										(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
							int j;

							if ((object[i].state3%2)==0) object[i].x+=TILE_UNIT;

							if (object[i].state==0) {
								for(j=0;j<n_objects;j++) {
									if (object[j].type==T_LADDER && object[i].x==object[j].x && object[j].y!=0 && (rand()%5)==0) {
										if (object[i].y==object[j].y-3*TILE_SIZE_Y) {
											object[i].state=4;
										} /* if */ 
										if (object[i].y==object[j].y+(object[j].size-2)*TILE_SIZE_Y) {
											if (object[i].state!=4 || (rand()%2)==0) object[i].state=2;
										} /* if */ 
										if (object[i].state==4) object[i].y+=TILE_UNIT;
										if (object[i].state==2) object[i].y-=TILE_UNIT;
									} /* if */ 
								} /* for */  
							} else {
								object[i].state=0;
							} /* if */ 
						} else {
							object[i].state=1;
						} /* if */ 
						if ((rand()%64)==0) { 
							/* Saltar: */ 
							int tx,ty;
							int nfloors=0,floors[8];
							tx=object[i].x/TILE_SIZE_X;
							ty=object[i].y/TILE_SIZE_Y;
							if (!iswall(tx+4,ty-3) &&
								!iswall(tx+5,ty-3) &&
								!iswall(tx+6,ty-3) &&
								!iswall(tx+7,ty-3) &&
								iswall(tx+4,ty-2) &&
								iswall(tx+5,ty-2) &&
								iswall(tx+6,ty-2) &&
								iswall(tx+7,ty-2) &&
								!iswall(tx+3,ty-2) &&
								(tiles[knight_tile+object[i].state2]->
									coltest(GAME_VIEW_X+object[i].x+4*TILE_SIZE_X,GAME_VIEW_Y+object[i].y-4*TILE_SIZE_Y,
										TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
								) {
								floors[nfloors++]=ty-4;
							} /* if */ 
							if (!iswall(tx+4,ty+5) &&
								!iswall(tx+5,ty+5) &&
								!iswall(tx+6,ty+5) &&
								!iswall(tx+7,ty+5) &&
								iswall(tx+4,ty+6) &&
								iswall(tx+5,ty+6) &&
								iswall(tx+6,ty+6) &&
								iswall(tx+7,ty+6) &&
								!iswall(tx+3,ty+2) &&
								(tiles[knight_tile+object[i].state2]->
									coltest(GAME_VIEW_X+object[i].x+4*TILE_SIZE_X+4*TILE_UNIT,GAME_VIEW_Y+object[i].y+4*TILE_SIZE_Y,
										TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
								) {
								floors[nfloors++]=ty+4;
							} /* if */ 
							if (nfloors!=0) {
								object[i].desired_floor=floors[rand()%nfloors]*TILE_SIZE_Y;
								object[i].state=6;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 
					   break;
				case 11: // Igual, pero no puede subir/bajar escaleras
				case 1:
						object[i].in_ladder=-1;
						object[i].state2=(object[i].state3>>2)&0x01;
					    if ((tiles[knight_tile+object[i].state2]->
							coltest(GAME_VIEW_X+object[i].x-TILE_UNIT,GAME_VIEW_Y+object[i].y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							(object[i].x-TILE_UNIT)>0 &&
							(col_buffer[(GAME_VIEW_X+object[i].x-TILE_UNIT)+
										(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
							int j;

							if ((object[i].state3%2)==0) object[i].x-=TILE_UNIT;

							if (object[i].state==1) {
								for(j=0;j<n_objects;j++) {
									if (object[j].type==T_LADDER && object[i].x==object[j].x && object[j].y!=0 && (rand()%4)==0) {
										if (object[i].y==object[j].y-3*TILE_SIZE_Y) {
											object[i].state=5;
										} /* if */ 
										if (object[i].y==object[j].y+(object[j].size-2)*TILE_SIZE_Y) {
											if (object[i].state!=5 || (rand()%2)==0) object[i].state=3;
										} /* if */ 
										if (object[i].state==5) object[i].y+=TILE_UNIT;
										if (object[i].state==3) object[i].y-=TILE_UNIT;
									} /* if */ 
								} /* for */ 
							} else {
								object[i].state=1;
							} /* if */ 
						} else {
							object[i].state=0;
						} /* if */ 
						if ((rand()%64)==0) { 
							/* Saltar: */ 
							int tx,ty;
							int nfloors=0,floors[8];
							tx=object[i].x/TILE_SIZE_X;
							ty=object[i].y/TILE_SIZE_Y;
							if (!iswall(tx-3,ty-3) &&
								!iswall(tx-4,ty-3) &&
								!iswall(tx-5,ty-3) &&
								!iswall(tx-6,ty-3) &&
								iswall(tx-3,ty-2) &&
								iswall(tx-4,ty-2) &&
								iswall(tx-5,ty-2) &&
								iswall(tx-6,ty-2) &&
								!iswall(tx-2,ty-2) &&
								(tiles[knight_tile+object[i].state2]->
									coltest(GAME_VIEW_X+object[i].x-4*TILE_SIZE_X,GAME_VIEW_Y+object[i].y-4*TILE_SIZE_Y,
										TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
								) {
								floors[nfloors++]=ty-4;
							} /* if */ 
							if (!iswall(tx-3,ty+5) &&
								!iswall(tx-4,ty+5) &&
								!iswall(tx-5,ty+5) &&
								!iswall(tx-6,ty+5) &&
								iswall(tx-3,ty+6) &&
								iswall(tx-4,ty+6) &&
								iswall(tx-5,ty+6) &&
								iswall(tx-6,ty+6) &&
								!iswall(tx-2,ty+2) &&
								(tiles[knight_tile+object[i].state2]->
									coltest(GAME_VIEW_X+object[i].x-(4*TILE_SIZE_X+4*TILE_UNIT),GAME_VIEW_Y+object[i].y+4*TILE_SIZE_Y,
										TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
								) {
								floors[nfloors++]=ty+4;
							} /* if */ 
							if (nfloors!=0) {
								object[i].desired_floor=floors[rand()%nfloors]*TILE_SIZE_Y;
								object[i].state=7;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 
					   break;
				case 2:
				case 3:{
						if (object[i].in_ladder==-1) object[i].in_ladder=search_ladder(object[i].x,object[i].y);

						object[i].state2=4+((object[i].state3>>2)&0x01);

						if (object[i].y>(object[object[i].in_ladder].y-3*TILE_SIZE_Y)) {
							object[i].y-=TILE_UNIT;
							if (object[i].y<0) object[i].state+=8;
						} else {
							object[i].y=(object[object[i].in_ladder].y-3*TILE_SIZE_Y);
							object[i].state+=8;
						} /* if */ 						
					   }
					   break;
				case 4:
				case 5:{
						if (object[i].in_ladder==-1) object[i].in_ladder=search_ladder(object[i].x,object[i].y);

						object[i].state2=6+((object[i].state3>>2)&0x01);

						if (object[i].y<(object[object[i].in_ladder].y+(object[object[i].in_ladder].size-2)*TILE_SIZE_Y)) {
							object[i].y+=TILE_UNIT;
							if (object[i].y>(room_size_y-2)*TILE_SIZE_Y) object[i].state+=6;
						} else {
							object[i].y=(object[object[i].in_ladder].y+(object[object[i].in_ladder].size-2)*TILE_SIZE_Y);
							object[i].state+=6;
						} /* if */ 						
					   }
					   break;
				/* Estados de salto: */ 
				case 6: object[i].state2=11;
						object[i].x+=2*TILE_UNIT;
						if (object[i].desired_floor<object[i].y) {
							object[i].state3=0;
							object[i].y-=4*TILE_UNIT;
						} else {
							if (object[i].state3<2) object[i].y-=TILE_UNIT*2;
							if (object[i].state3>=2 && object[i].state3<4) object[i].y-=TILE_UNIT;
							if (object[i].state3>=8) {
								object[i].state=8;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 	
						break;
				case 7:	object[i].state2=10;
						object[i].x-=2*TILE_UNIT;
						if (object[i].desired_floor<object[i].y) {
							object[i].state3=0;
							object[i].y-=4*TILE_UNIT;
						} else {
							if (object[i].state3<2) object[i].y-=TILE_UNIT*2;
							if (object[i].state3>=2 && object[i].state3<4) object[i].y-=TILE_UNIT;
							if (object[i].state3>=8) {
								object[i].state=9;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 
						break;
				case 8: object[i].state2=11;
						object[i].x+=2*TILE_UNIT;
						if (object[i].state3>=2) object[i].y+=TILE_UNIT;
						if (object[i].state3>=4) object[i].y+=TILE_UNIT;
						if (object[i].state3>=6) object[i].y+=TILE_UNIT*2;
						if (object[i].state3>=10) object[i].y+=TILE_UNIT*2;
						if (object[i].y>=object[i].desired_floor) {
							object[i].y=object[i].desired_floor;
							object[i].state=0;
							object[i].state3=0;
						} /* if */ 
						break;
				case 9: object[i].state2=10;
						object[i].x-=2*TILE_UNIT;
						if (object[i].state3>=2) object[i].y+=TILE_UNIT;
						if (object[i].state3>=4) object[i].y+=TILE_UNIT;
						if (object[i].state3>=6) object[i].y+=TILE_UNIT*2;
						if (object[i].state3>=10) object[i].y+=TILE_UNIT*2;
						if (object[i].y>=object[i].desired_floor) {
							object[i].y=object[i].desired_floor;
							object[i].state=1;
							object[i].state3=0;
						} /* if */ 
						break;
				} /* switch */ 
			break;


			case T_BLOB:
				{
					int colision=T_WALL|T_LADDER_WALL;
					int res;

					if (object[i].state3==0) {
						object[i].state2=0;
						res=(tiles[blob_tile+1]->
								coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y-4*TILE_UNIT,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision);
						if (object[i].y>=TILE_SIZE_Y && 
							object[i].y<TILE_SIZE_Y*16 &&
							(tiles[blob_tile+1]->
								coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							res!=0 && res!=(T_WALL|T_LADDER_WALL)) {
							Sound_play(S_blob);
							object[i].state3=50;
							object[i].state=rand()%2;
							object[i].y=((object[i].y/TILE_SIZE_Y))*TILE_SIZE_Y;
						} else {
							object[i].y-=4*TILE_UNIT;
						} /* if */ 
					
					} else {
						if (object[i].state3>=24) object[i].state2=1;
											 else object[i].state2=0;
						if (object[i].state3<4) object[i].y-=4*TILE_UNIT;
						if (object[i].state3>=4 && object[i].state3<8) object[i].y-=2*TILE_UNIT;
						if (object[i].state3>=8 && object[i].state3<12) object[i].y-=TILE_UNIT;
						if (object[i].state3>=14 && object[i].state3<18) object[i].y+=TILE_UNIT;
						if (object[i].state3>=18 && object[i].state3<22) object[i].y+=2*TILE_UNIT;
						if (object[i].state3>=22 && object[i].state3<24) object[i].y+=4*TILE_UNIT;

						object[i].state3--;
					} /* if */ 

					if (object[i].state2==0) {
						if (object[i].state==0) object[i].x-=TILE_UNIT;
										   else object[i].x+=TILE_UNIT;
					} /* if */ 

					if (object[i].x>=dx || object[i].x<=(-2*TILE_SIZE_X) ||
						object[i].y<(-2*TILE_SIZE_Y)) {
						object[i].type=T_NADA;
					} /* if */ 
				}
				break;

			case T_BAMBU:
				object[i].state--;

				if (object[i].state<0) {
					object[i].state=(rand()%256)+128;
				} /* if */ 

				if (object[i].state<20) object[i].tile=0;
				if (object[i].state>=20 && object[i].state<24) object[i].tile=1;
				if (object[i].state>=24 && object[i].state<28) object[i].tile=2;
				if (object[i].state>=28 && object[i].state<44) object[i].tile=3;
				if (object[i].state>=44 && object[i].state<48) object[i].tile=2;
				if (object[i].state>=48 && object[i].state<52) object[i].tile=1;
				if (object[i].state>=52 && object[i].state<68) object[i].tile=0;
				if (object[i].state>=68) object[i].tile=-1;
				break;

			case T_PORCUPINE:
				object[i].state3++;

				switch(object[i].state) {
				case 0:
					object[i].tile=(object[i].state3>>3)&0x01;
					if ((tiles[porcupine_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     object[i].x>0 &&
						(col_buffer[GAME_VIEW_X+object[i].x-1+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x-=TILE_UNIT*2;
					} else {
						object[i].state=1;
					} /* if */ 
					if ((rand()%128)==0) {
						object[i].state=2;
						object[i].state3=0;
					} else {
						if ((rand()%128)==0) {
							object[i].state=4;
							object[i].state3=0;
						} /* if */ 
					} /* if */ 
					break;
				case 1:
					object[i].tile=((object[i].state3>>3)&0x01)+4;
					if ((tiles[porcupine_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     (object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
						(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x+=TILE_UNIT*2;
					} else {
						object[i].state=0;
					} /* if */ 
					if ((rand()%128)==0) {
						object[i].state=3;
						object[i].state3=0;
					} else {
						if ((rand()%128)==0) {
							object[i].state=5;
							object[i].state3=0;
						} /* if */ 
					} /* if */ 
					break;
				case 2:
					object[i].tile=((object[i].state3>>3)&0x01)*4+3;
					if (object[i].state3>=64) object[i].state=0;
					break;
				case 3:
					object[i].tile=((object[i].state3>>3)&0x01)*4+3;
					if (object[i].state3>=64) object[i].state=1;
					break;
				case 4:
					object[i].tile=2;
					if (object[i].state3==48) {
						int val;
						Sound_play(S_karrow);
						val=add_object(T_ENEMY,object[i].x,object[i].y,0,T_PORCUPINE_BULLET,1);
						object[val].base_tile=porcupine_tile;
					} /* if */ 
					if (object[i].state3>=64) object[i].state=0;
					break;
				case 5:
					object[i].tile=6;
					if (object[i].state3==48) {
						int val;
						Sound_play(S_karrow);
						val=add_object(T_ENEMY,object[i].x,object[i].y,0,T_PORCUPINE_BULLET,0);
						object[val].base_tile=porcupine_tile;
					} /* if */ 
					if (object[i].state3>=64) object[i].state=1;
					break;
				} /* switch */ 
				break;

			case T_PORCUPINE_BULLET:
				if ((tiles[porcupine_tile+8]->
						coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
					object[i].type=T_NADA;
				} /* if */ 			   

				if (object[i].state==0) object[i].x+=TILE_SIZE_X/2;
								   else object[i].x-=TILE_SIZE_X/2;

				if (object[i].x<(-2*TILE_SIZE_X) ||
					object[i].x>(room_size_x*TILE_SIZE_X)) {
					object[i].type=T_NADA;
				} /* if */ 
				break;

			case T_JUMPINGBUSH:
				{
					int tmp,dif;
					object[i].tile=0;

					tmp=object[i].state;

					if (pers_state==S_JUMPING || pers_state==S_JUMPING_SWORD) {
						if (pers_substate==1 && first_jumpingbush) Sound_play(S_jumptree);
						if (character==0) {
							if (pers_substate-1== 0) object[i].state=object[i].state-TILE_UNIT*7;
							if (pers_substate-1== 1) object[i].state=object[i].state-TILE_UNIT*6;
							if (pers_substate-1== 2) object[i].state=object[i].state-TILE_UNIT*6;
							if (pers_substate-1== 3) object[i].state=object[i].state-TILE_UNIT*6;
							if (pers_substate-1== 4) object[i].state=object[i].state-TILE_UNIT*5;
							if (pers_substate-1== 5) object[i].state=object[i].state-TILE_UNIT*5;
							if (pers_substate-1== 6) object[i].state=object[i].state-TILE_UNIT*5;
							if (pers_substate-1== 7) object[i].state=object[i].state-TILE_UNIT*4;
							if (pers_substate-1== 8) object[i].state=object[i].state-TILE_UNIT*4;
							if (pers_substate-1== 9) object[i].state=object[i].state-TILE_UNIT*3;
							if (pers_substate-1==10) object[i].state=object[i].state-TILE_UNIT*2;
							if (pers_substate-1==11) object[i].state=object[i].state-TILE_UNIT*1;
						} else {
							if (pers_substate-1== 0) object[i].state=object[i].state-TILE_UNIT*7;
							if (pers_substate-1== 1) object[i].state=object[i].state-TILE_UNIT*6;
							if (pers_substate-1== 2) object[i].state=object[i].state-TILE_UNIT*6;
							if (pers_substate-1== 3) object[i].state=object[i].state-TILE_UNIT*6;
							if (pers_substate-1== 4) object[i].state=object[i].state-TILE_UNIT*5;
							if (pers_substate-1== 5) object[i].state=object[i].state-TILE_UNIT*5;
							if (pers_substate-1== 6) object[i].state=object[i].state-TILE_UNIT*4;
							if (pers_substate-1== 7) object[i].state=object[i].state-TILE_UNIT*3;
							if (pers_substate-1==11) object[i].state=object[i].state-TILE_UNIT*1;
						} /* if */ 
						object[i].state3=0;
					} else {
						tmp=object[i].state=object[i].state2;
						if (object[i].state<0 && pers_state!=S_STOPPED_JUMPING && pers_state!=S_SJ_SWORD) {
							if (object[i].state3== 3) object[i].state=object[i].state+TILE_UNIT;
							if (object[i].state3== 4) object[i].state=object[i].state+TILE_UNIT*2;
							if (object[i].state3== 5) object[i].state=object[i].state+TILE_UNIT*3;
							if (object[i].state3== 6) object[i].state=object[i].state+TILE_UNIT*4;
							if (object[i].state3== 7) object[i].state=object[i].state+TILE_UNIT*4;
							if (object[i].state3== 8) object[i].state=object[i].state+TILE_UNIT*5;
							if (object[i].state3== 9) object[i].state=object[i].state+TILE_UNIT*5;
							if (object[i].state3==10) object[i].state=object[i].state+TILE_UNIT*5;
							if (object[i].state3>=11) object[i].state=object[i].state+TILE_UNIT*6;
							if (object[i].state>=0) {
								tmp=object[i].state=0;
								object[i].state2=0;
							} /* if */ 
							object[i].state3++;
						} else {
							if (object[i].state>=0) {
								tmp=object[i].state=0;
								object[i].state2=0;
							} /* if */ 
							object[i].state3=0;
						} /* if */ 
					} /* if */ 

					dif=object[i].state-tmp;
					if (dif!=0) {
						if ((tiles[jumpingbush_tile]->
								coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y+object[i].state2+dif,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) {
							object[i].state2+=dif;
						} /* if */ 
					} /* if */ 

  					first_jumpingbush=false;
				}
				break;

			case T_BLUESPIDER:
				{
					object[i].tile=(object[i].state3%12)>>2;

					switch(object[i].state) {
					case 0:/* QUIETO */ 
						if (pers_y>=object[i].y-TILE_UNIT && pers_y<object[i].y+TILE_SIZE_Y) {
							if (pers_x<object[i].x) object[i].state=1;
											   else object[i].state=3;
						} /* if */ 
						break;
					case 1:/* AVANZANDO IZQ */ 
						if ((tiles[bluespider_tile]->
								coltest(GAME_VIEW_X+object[i].x+object[i].state2-2*TILE_UNIT,GAME_VIEW_Y+object[i].y,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							(col_buffer[GAME_VIEW_X+object[i].x+object[i].state2+
							 			(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
							object[i].state2-=2*TILE_UNIT;
						} else {
							object[i].state=2;
						} /* if */ 
						object[i].state3+=2;
						break;
					case 2:/* RETROCEDIENDO IZQ */ 
						object[i].state3++;
						object[i].state2+=TILE_UNIT;
						if (object[i].state2==0) object[i].state=0;
						break;
					case 3:/* AVANZANDO DER */ 
						if ((tiles[bluespider_tile]->
								coltest(GAME_VIEW_X+object[i].x+object[i].state2+2*TILE_UNIT,GAME_VIEW_Y+object[i].y,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							(col_buffer[GAME_VIEW_X+object[i].x+object[i].state2+TILE_SIZE_X*2+
							 			(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
							object[i].state2+=2*TILE_UNIT;
						} else {
							object[i].state=4;
						} /* if */ 
						object[i].state3+=2;
						break;
					case 4:/* RETROCEDIENDO IZQ */ 
						object[i].state3++;
						object[i].state2-=TILE_UNIT;
						if (object[i].state2==0) object[i].state=0;
						break;
					} /* switch */ 
				}
				break;

			case T_WHITEFIREBALL:
				switch(object[i].state) {
				case 0:object[i].x+=TILE_UNIT*2;
					   break;
				case 1:object[i].x+=TILE_UNIT*2;
					   object[i].y-=TILE_UNIT*2;
					   break;
				case 2:object[i].y-=TILE_UNIT*2;
					   break;
				case 3:object[i].x-=TILE_UNIT*2;
					   object[i].y-=TILE_UNIT*2;
					   break;
				case 4:object[i].x-=TILE_UNIT*2;
					   break;
				case 5:object[i].x-=TILE_UNIT*2;
					   object[i].y+=TILE_UNIT*2;
					   break;
				case 6:object[i].y+=TILE_UNIT*2;
					   break;
				case 7:object[i].x+=TILE_UNIT*2;
					   object[i].y+=TILE_UNIT*2;
					   break;
				} /* switch */ 
				if (object[i].x<=0) {
					if (object[i].state==3) object[i].state=1;
					if (object[i].state==4) object[i].state=0;
					if (object[i].state==5) object[i].state=7;
					object[i].state3=0;
				} /* if */ 
				if (object[i].x>=30*TILE_SIZE_X) {
					if (object[i].state==7) object[i].state=5;
					if (object[i].state==0) object[i].state=4;
					if (object[i].state==1) object[i].state=3;
					object[i].state3=0;
				} /* if */ 
				if (object[i].y<=0) {
					if (object[i].state==1) object[i].state=7;
					if (object[i].state==2) object[i].state=6;
					if (object[i].state==3) object[i].state=5;
					object[i].state3=0;
				} /* if */ 
				if (object[i].y>=17*TILE_SIZE_X) {
					if (object[i].state==7) object[i].state=1;
					if (object[i].state==6) object[i].state=2;
					if (object[i].state==5) object[i].state=3;
					object[i].state3=0;
				} /* if */ 

				if (object[i].state3>=32) {
					object[i].state=rand()%8;
					object[i].state3=0;
				} /* if */ 
				object[i].state3++;
				object[i].tile=object[i].state;
				break;

			case T_DEMON1_BONES:
				object[i].x+=object[i].state;
				object[i].state2++;
				object[i].state3++;
				if (object[i].state3<16) {
					if (object[i].state3<8) object[i].y-=TILE_UNIT;
					if (object[i].state3<12) object[i].y-=TILE_UNIT;
					object[i].y-=TILE_UNIT;
				} else {
					if (object[i].state3>20) object[i].y+=TILE_UNIT;
					if (object[i].state3>24) object[i].y+=TILE_UNIT;
					if (object[i].state3>32) {
						object[i].y+=TILE_UNIT;
//						object[i].x-=object[i].state;
					} /* if */ 
					object[i].y+=TILE_UNIT;
				} /* if */ 
				if (object[i].state2>=8) object[i].state2=0;
				if (object[i].x<0 || object[i].y>=dy) object[i].type=T_NADA;
				break;
			case T_DEMON1_BONES2:
				object[i].x+=object[i].state;
				object[i].state2++;
				object[i].state3++;
				if (object[i].state3<16) {
					if (object[i].state3<8) object[i].y-=TILE_UNIT;
					if (object[i].state3<12) object[i].y-=TILE_UNIT;
					object[i].y-=TILE_UNIT;
				} else {
					if (object[i].state3>20) object[i].y+=TILE_UNIT;
					if (object[i].state3>24) object[i].y+=TILE_UNIT;
					if (object[i].state3>32) {
						object[i].y+=TILE_UNIT;
//						object[i].x-=object[i].state;
					} /* if */ 
					object[i].y+=TILE_UNIT;
				} /* if */ 
				if (object[i].state2>=8) object[i].state2=0;
				if (object[i].x<0 || object[i].y>=dy) object[i].type=T_NADA;
				break;

			case T_BLACK:
				object[i].state3++;

				switch(object[i].state) {
				case 0:
					if (
						!iswall_or_blockingobject(((object[i].x)/TILE_SIZE_X),
												  (object[i].y/TILE_SIZE_Y)) &&
						iswall_or_blockingobject(((object[i].x)/TILE_SIZE_X),
												  (object[i].y/TILE_SIZE_Y)+2) &&
					     object[i].x>0) {
						if ((object[i].state3&0x01)==0 ||
							object[i].state2==1) object[i].x--;
						if (object[i].state2==1) object[i].x--;
						if (pers_x<object[i].x && object[i].y==pers_y) object[i].state2=1;
					} else {
						object[i].state2=0;
						object[i].state=1;
					} /* if */ 
					break;
				case 1:
					if (!iswall_or_blockingobject(((object[i].x)/TILE_SIZE_X)+2,
												  (object[i].y/TILE_SIZE_Y)) &&
						iswall_or_blockingobject(((object[i].x)/TILE_SIZE_X)+2,
												  (object[i].y/TILE_SIZE_Y)+2)) {
						if ((object[i].state3&0x01)==0 ||
							object[i].state2==1) object[i].x++;
						if (object[i].state2==1) object[i].x++;
						if (pers_x>object[i].x && object[i].y==pers_y) object[i].state2=1;
					} else {
						object[i].state2=0;
						object[i].state=0;
					} /* if */ 
					break;
				} /* switch */ 
				break;

			case T_WITCH:
				{
					int j;
					bool no_other_witch=true;

					if (object[i].state<=0) {
						for(j=0;j<n_objects;j++) {
							if (object[j].type==T_ENEMY && object[j].enemy==T_WITCH &&
								object[j].state>0) {
								no_other_witch=false;
							} /* if */ 
						} /* for */ 

						if (no_other_witch && (rand()%64)==0) {
							object[i].state=16;
						} /* if */ 
					} else {
						object[i].state--;
					} /* if */ 
				}
				break;

			case T_WHITEBEAR:
				object[i].state3++;

				if (pers_x>object[i].x && object[i].y==pers_y &&
					object[i].state<2) {
					object[i].state=3;
					object[i].state3=0;
				} /* if */ 
				if (pers_x<object[i].x && object[i].y==pers_y &&
					object[i].state<2) {
					object[i].state=2;
					object[i].state3=0;
				} /* if */ 

				switch(object[i].state) {
				case 0:
					object[i].tile=(object[i].state3>>3)&0x01;
					if ((tiles[whitebear_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     object[i].x>0 &&
						(col_buffer[GAME_VIEW_X+object[i].x-1+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x-=TILE_UNIT*2;

						if ((rand()%32)==0) { 
							/* Saltar: */ 
							int tx,ty;
							int nfloors=0,floors[8];
							tx=object[i].x/TILE_SIZE_X;
							ty=object[i].y/TILE_SIZE_Y;
							if (!iswall(tx-3,ty-3) &&
								!iswall(tx-4,ty-3) &&
								!iswall(tx-5,ty-3) &&
								!iswall(tx-6,ty-3) &&
								iswall(tx-3,ty-2) &&
								iswall(tx-4,ty-2) &&
								iswall(tx-5,ty-2) &&
								iswall(tx-6,ty-2) &&
								!iswall(tx-2,ty-2) &&
								(tiles[whitebear_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x-4*TILE_SIZE_X,GAME_VIEW_Y+object[i].y-4*TILE_SIZE_Y,
										TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
								) {
								floors[nfloors++]=ty-4;
							} /* if */ 
							if (!iswall(tx-3,ty+5) &&
								!iswall(tx-4,ty+5) &&
								!iswall(tx-5,ty+5) &&
								!iswall(tx-6,ty+5) &&
								iswall(tx-3,ty+6) &&
								iswall(tx-4,ty+6) &&
								iswall(tx-5,ty+6) &&
								iswall(tx-6,ty+6) &&
								!iswall(tx-2,ty+2) &&
								(tiles[whitebear_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x-(4*TILE_SIZE_X+4*TILE_UNIT),GAME_VIEW_Y+object[i].y+4*TILE_SIZE_Y,
										TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
								) {
								floors[nfloors++]=ty+4;
							} /* if */ 
							if (nfloors!=0) {
								object[i].desired_floor=floors[rand()%nfloors]*TILE_SIZE_Y;
								object[i].state=7;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 
					} else {
						object[i].state=1;
					} /* if */ 
					break;
				case 1:
					object[i].tile=((object[i].state3>>3)&0x01)+2;
					if ((tiles[whitebear_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     (object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
						(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x+=TILE_UNIT*2;

						if ((rand()%32)==0) { 
							/* Saltar: */ 
							int tx,ty;
							int nfloors=0,floors[8];
							tx=object[i].x/TILE_SIZE_X;
							ty=object[i].y/TILE_SIZE_Y;
							if (!iswall(tx+4,ty-3) &&
								!iswall(tx+5,ty-3) &&
								!iswall(tx+6,ty-3) &&
								!iswall(tx+7,ty-3) &&
								iswall(tx+4,ty-2) &&
								iswall(tx+5,ty-2) &&
								iswall(tx+6,ty-2) &&
								iswall(tx+7,ty-2) &&
								!iswall(tx+3,ty-2) &&
								(tiles[whitebear_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x+4*TILE_SIZE_X,GAME_VIEW_Y+object[i].y-4*TILE_SIZE_Y,
										TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
								) {
								floors[nfloors++]=ty-4;
							} /* if */ 
							if (!iswall(tx+4,ty+5) &&
								!iswall(tx+5,ty+5) &&
								!iswall(tx+6,ty+5) &&
								!iswall(tx+7,ty+5) &&
								iswall(tx+4,ty+6) &&
								iswall(tx+5,ty+6) &&
								iswall(tx+6,ty+6) &&
								iswall(tx+7,ty+6) &&
								!iswall(tx+3,ty+2) &&
								(tiles[whitebear_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x+4*TILE_SIZE_X+4*TILE_UNIT,GAME_VIEW_Y+object[i].y+4*TILE_SIZE_Y,
										TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
								) {
								floors[nfloors++]=ty+4;
							} /* if */ 
							if (nfloors!=0) {
								object[i].desired_floor=floors[rand()%nfloors]*TILE_SIZE_Y;
								object[i].state=6;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 
					} else {
						object[i].state=0;
					} /* if */ 
					break;
				case 2:
					if (object[i].state3<32) object[i].tile=1;
										else object[i].tile=0;
					if (object[i].state3==32) {
						int val;
						Sound_play(S_bearbullet);
						val=add_object(T_ENEMY,object[i].x-TILE_SIZE_X,object[i].y,0,T_WHITEBEAR_BULLET,0);
						object[val].base_tile=whitebear_tile;
						object[i].desired_floor=val;
					} /* if */ 
					if (object[i].state3>=64 &&
						object[object[i].desired_floor].type!=T_ENEMY) {
						if (freezed>0) {
							object[i].state=4;
							object[i].state3=0;
						} else {
							object[i].state=0;
							object[i].state3=0;
						} /* if */ 
					} /* if */ 
					break;
				case 3:
					if (object[i].state3<32) object[i].tile=3;
										else object[i].tile=2;
					if (object[i].state3==32) {
						int val;
						Sound_play(S_bearbullet);
						val=add_object(T_ENEMY,object[i].x+TILE_SIZE_X,object[i].y,0,T_WHITEBEAR_BULLET,1);
						object[val].base_tile=whitebear_tile;
						object[i].desired_floor=val;
					} /* if */ 
					if (object[i].state3>=64 &&
						object[object[i].desired_floor].type!=T_ENEMY) {
						if (freezed>0) {
							object[i].state=5;
							object[i].state3=0;
						} else {
							object[i].state=1;
							object[i].state3=0;
						} /* if */ 
					} /* if */ 
					break;
				case 4:
					object[i].tile=(object[i].state3>>3)&0x01;
					if ((tiles[whitebear_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     object[i].x>0 &&
						(col_buffer[GAME_VIEW_X+object[i].x-1+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x-=TILE_UNIT*4;
					} else {
						object[i].state=1;
					} /* if */ 
					break;
				case 5:
					object[i].tile=((object[i].state3>>3)&0x01)+2;
					if ((tiles[whitebear_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     (object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
						(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x+=TILE_UNIT*4;
					} else {
						object[i].state=0;
					} /* if */ 
					break;
				/* Estados de salto: */ 
				case 6: object[i].tile=2;
						object[i].x+=2*TILE_UNIT;
						if (object[i].desired_floor<object[i].y) {
							object[i].state3=0;
							object[i].y-=4*TILE_UNIT;
						} else {
							if (object[i].state3<2) object[i].y-=TILE_UNIT*2;
							if (object[i].state3>=2 && object[i].state3<4) object[i].y-=TILE_UNIT;
							if (object[i].state3>=8) {
								object[i].state=8;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 	
						break;
				case 7:	object[i].tile=0;
						object[i].x-=2*TILE_UNIT;
						if (object[i].desired_floor<object[i].y) {
							object[i].state3=0;
							object[i].y-=4*TILE_UNIT;
						} else {
							if (object[i].state3<2) object[i].y-=TILE_UNIT*2;
							if (object[i].state3>=2 && object[i].state3<4) object[i].y-=TILE_UNIT;
							if (object[i].state3>=8) {
								object[i].state=9;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 
						break;
				case 8: object[i].tile=2;
						object[i].x+=2*TILE_UNIT;
						if (object[i].state3>=2) object[i].y+=TILE_UNIT;
						if (object[i].state3>=4) object[i].y+=TILE_UNIT;
						if (object[i].state3>=6) object[i].y+=TILE_UNIT*2;
						if (object[i].state3>=10) object[i].y+=TILE_UNIT*2;
						if (object[i].y>=object[i].desired_floor) {
							object[i].y=object[i].desired_floor;
							object[i].state=1;
							object[i].state3=0;
						} /* if */ 
						break;
				case 9: object[i].tile=0;
						object[i].x-=2*TILE_UNIT;
						if (object[i].state3>=2) object[i].y+=TILE_UNIT;
						if (object[i].state3>=4) object[i].y+=TILE_UNIT;
						if (object[i].state3>=6) object[i].y+=TILE_UNIT*2;
						if (object[i].state3>=10) object[i].y+=TILE_UNIT*2;
						if (object[i].y>=object[i].desired_floor) {
							object[i].y=object[i].desired_floor;
							object[i].state=0;
							object[i].state3=0;
						} /* if */ 
						break;
				} /* switch */ 
				break;

			case T_WHITEBEAR_BULLET:
				if ((tiles[whitebear_tile+4+object[i].state]->
						coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
					object[i].type=T_NADA;
				} /* if */ 			   

				if (object[i].state==0) object[i].x-=TILE_SIZE_X/2;
								   else object[i].x+=TILE_SIZE_X/2;

				if (object[i].x<(-2*TILE_SIZE_X) ||
					object[i].x>(room_size_x*TILE_SIZE_X)) {
					object[i].type=T_NADA;
				} /* if */ 
				break;

			case T_FEET:
				{
					bool haspair=false;
					int j;
					int val=TILE_SIZE_Y/2;

					for(j=0;j<n_objects;j++) {
						if (object[j].type==T_ENEMY && object[j].enemy==T_FEET &&
							object[j].y==object[i].y &&	i!=j) {
							haspair=true;
						} /* if */ 
					} /* for */ 
					object[i].yoffs=-TILE_SIZE_Y/2;

					if (haspair) {
						if (object[i].state3>=0 && object[i].state3<8) {
							object[i].tile=2+object[i].state;
							if (object[i].state==0) object[i].x-=TILE_UNIT;
											   else object[i].x+=TILE_UNIT;
						} /* if */ 
						if (object[i].state3>=8 && object[i].state3<24) {
							object[i].tile=object[i].state;
							if (object[i].state==0) object[i].x-=TILE_UNIT;
											   else object[i].x+=TILE_UNIT;
						} /* if */ 
						if (object[i].state3>=24 && object[i].state3<32) {
							object[i].tile=1+object[i].state;
							if (object[i].state==0) object[i].x-=TILE_UNIT;
											   else object[i].x+=TILE_UNIT;
						} /* if */ 
						if (object[i].state3>=32) {
							val=0;
							object[i].yoffs=0;
							object[i].tile=object[i].state;
							if (object[i].state3>=64) object[i].state3=0;
						} /* if */ 
					} else {
						if (object[i].state3>=0 && object[i].state3<4) {
							object[i].tile=2+object[i].state;
							if (object[i].state==0) object[i].x-=TILE_UNIT*2;
											   else object[i].x+=TILE_UNIT*2;
						} /* if */ 
						if (object[i].state3>=4 && object[i].state3<12) {
							object[i].tile=object[i].state;
							if (object[i].state==0) object[i].x-=TILE_UNIT*2;
											   else object[i].x+=TILE_UNIT*2;
						} /* if */ 
						if (object[i].state3>=12 && object[i].state3<16) {
							object[i].tile=1+object[i].state;
							if (object[i].state==0) object[i].x-=TILE_UNIT*2;
											   else object[i].x+=TILE_UNIT*2;
						} /* if */ 
						if (object[i].state3>=16) {
							val=0;
							object[i].yoffs=0;
							object[i].tile=object[i].state;
							if (object[i].state3>=20) object[i].state3=0;
						} /* if */ 					
					} /* if */ 
					
					if (object[i].state==0) {
						if ((tiles[feet_tile+object[i].tile]->
							coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y-val,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							 object[i].x>0 &&
							(col_buffer[GAME_VIEW_X+object[i].x-TILE_UNIT+
										(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						} else {
							object[i].state=3;
						} /* if */ 
					} else {
						if ((tiles[feet_tile+object[i].tile]->
							coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y-val,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							 (object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
							(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
										(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						} else {
							object[i].state=0;
						} /* if */ 
					} /* if */ 
						
					object[i].state3++;
				}
				break;

			case T_DEMON2_BALLS:
				object[i].state2--;
				if (object[i].state2>=12) object[i].y-=TILE_UNIT;
				if (object[i].state2>=8) object[i].y-=TILE_UNIT;
				if (object[i].state2>=4) object[i].y-=TILE_UNIT;
				if (object[i].state2>=0) object[i].y-=TILE_UNIT;
				if (object[i].state2<-4) object[i].y+=TILE_UNIT;
				if (object[i].state2<-8) object[i].y+=TILE_UNIT;
				if (object[i].state2<-12) object[i].y+=TILE_UNIT;
				if (object[i].state2<-16) object[i].y+=TILE_UNIT;
				if (object[i].state2<-20) object[i].y+=TILE_UNIT;
				if (object[i].state2<-24) object[i].y+=TILE_UNIT;
				object[i].x+=object[i].state;
				if (object[i].x<0 || object[i].x>room_size_x*TILE_SIZE_X ||
					object[i].y>=room_size_y*TILE_SIZE_Y) object[i].type=0;
				break;

			case T_REDJUMPER:
				object[i].state3++;
				switch(object[i].state) {
				case 0:object[i].tile=((object[i].state3>>2)&0x01);
					    if ((tiles[redjumper_tile+object[i].tile]->
							coltest(GAME_VIEW_X+object[i].x+2*TILE_UNIT,GAME_VIEW_Y+object[i].y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							(object[i].x+2*TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
							(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
										(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0 &&
							(rand()%32!=0)) {
							object[i].x+=2*TILE_UNIT;
						} else {
							object[i].state=1;
						} /* if */ 
						if ((rand()%64)==0) { 
							/* Saltar: */ 
							int tx,ty;
							int nfloors=0,floors[8];
							int py[5]={1,5,9,13,17};
							tx=object[i].x/TILE_SIZE_X;
							ty=object[i].y/TILE_SIZE_Y;
							for(j=0;j<5;j++) {
								if (py[j]!=ty &&
									!iswall(tx+4,py[j]+1) &&
									!iswall(tx+5,py[j]+1) &&
									!iswall(tx+6,py[j]+1) &&
									!iswall(tx+7,py[j]+1) &&
									iswall(tx+4,py[j]+2) &&
									iswall(tx+5,py[j]+2) &&
									iswall(tx+6,py[j]+2) &&
									iswall(tx+7,py[j]+2) &&
									(tiles[redjumper_tile+object[i].tile]->
										coltest(GAME_VIEW_X+object[i].x+4*TILE_SIZE_X,GAME_VIEW_Y+py[j]*TILE_SIZE_Y,
											TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
									(abs(ty-py[j])<12 ||
									((tiles[redjumper_tile+object[i].tile]->
										coltest(GAME_VIEW_X+object[i].x+6*TILE_SIZE_X,GAME_VIEW_Y+py[j]*TILE_SIZE_Y,
											TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) &&
									!iswall(tx+8,py[j]+1) &&
									!iswall(tx+9,py[j]+1) &&
									iswall(tx+8,py[j]+2) &&
									iswall(tx+9,py[j]+2))
									) {
									floors[nfloors++]=py[j];
								} /* if */ 
							} /* for */ 
							if (nfloors!=0) {
								object[i].desired_floor=floors[rand()%nfloors]*TILE_SIZE_Y;
								object[i].state=6;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 
					   break;
				case 1:object[i].tile=3+((object[i].state3>>2)&0x01);
					    if ((tiles[redjumper_tile+object[i].tile]->
							coltest(GAME_VIEW_X+object[i].x-2*TILE_UNIT,GAME_VIEW_Y+object[i].y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							(object[i].x-2*TILE_UNIT)>0 &&
							(col_buffer[(GAME_VIEW_X+object[i].x-2*TILE_UNIT)+
										(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0 &&
							(rand()%32!=0)) {
							object[i].x-=2*TILE_UNIT;
						} else {
							object[i].state=0;
						} /* if */ 
						if ((rand()%64)==0) { 
							/* Saltar: */ 
							int tx,ty,j;
							int nfloors=0,floors[8];
							int py[5]={1,5,9,13,17};
							tx=object[i].x/TILE_SIZE_X;
							ty=object[i].y/TILE_SIZE_Y;
							for(j=0;j<5;j++) {
								if (py[j]!=ty &&
									!iswall(tx-3,py[j]+1) &&
									!iswall(tx-4,py[j]+1) &&
									!iswall(tx-5,py[j]+1) &&
									!iswall(tx-6,py[j]+1) &&
									iswall(tx-3,py[j]+2) &&
									iswall(tx-4,py[j]+2) &&
									iswall(tx-5,py[j]+2) &&
									iswall(tx-6,py[j]+2) &&
									(tiles[redjumper_tile+object[i].tile]->
										coltest(GAME_VIEW_X+object[i].x-4*TILE_SIZE_X,GAME_VIEW_Y+py[j]*TILE_SIZE_Y,
											TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
									(abs(ty-py[j])<12 ||
									 ((tiles[redjumper_tile+object[i].tile]->
										coltest(GAME_VIEW_X+object[i].x-6*TILE_SIZE_X,GAME_VIEW_Y+py[j]*TILE_SIZE_Y,
											TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0) &&
									!iswall(tx-7,py[j]+1) &&
									!iswall(tx-8,py[j]+1) &&
									iswall(tx-7,py[j]+2) &&
									iswall(tx-8,py[j]+2))
									) {
									floors[nfloors++]=py[j];
								} /* if */ 
							} /* for */ 

							if (nfloors!=0) {
								object[i].desired_floor=floors[rand()%nfloors]*TILE_SIZE_Y;
								object[i].state=7;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 
					   break;
				/* Estados de salto: */ 
				case 6: object[i].tile=2;
						object[i].x+=2*TILE_UNIT;
						if (object[i].desired_floor<object[i].y) {
							object[i].state3=0;
							object[i].y-=6*TILE_UNIT;
						} else {
							if (object[i].state3<2) object[i].y-=TILE_UNIT*2;
							if (object[i].state3>=2 && object[i].state3<4) object[i].y-=TILE_UNIT;
							if (object[i].state3>=8) {
								object[i].state=8;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 	
						break;
				case 7:	object[i].tile=5;
						object[i].x-=2*TILE_UNIT;
						if (object[i].desired_floor<object[i].y) {
							object[i].state3=0;
							object[i].y-=6*TILE_UNIT;
						} else {
							if (object[i].state3<2) object[i].y-=TILE_UNIT*2;
							if (object[i].state3>=2 && object[i].state3<4) object[i].y-=TILE_UNIT;
							if (object[i].state3>=8) {
								object[i].state=9;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 
						break;
				case 8: object[i].tile=2;
						object[i].x+=2*TILE_UNIT;
						if (object[i].state3>=2) object[i].y+=TILE_UNIT;
						if (object[i].state3>=4) object[i].y+=TILE_UNIT;
						if (object[i].state3>=6) object[i].y+=TILE_UNIT*2;
						if (object[i].state3>=10) {
							object[i].y+=TILE_UNIT*2;
							object[i].x-=TILE_UNIT;
						} /* if */ 
						if (object[i].y>=object[i].desired_floor) {
							object[i].y=object[i].desired_floor;
							object[i].state=0;
							object[i].state3=0;
						} /* if */ 
						break;
				case 9: object[i].tile=5;
						object[i].x-=2*TILE_UNIT;
						if (object[i].state3>=2) object[i].y+=TILE_UNIT;
						if (object[i].state3>=4) object[i].y+=TILE_UNIT;
						if (object[i].state3>=6) object[i].y+=TILE_UNIT*2;
						if (object[i].state3>=10) {
							object[i].y+=TILE_UNIT*2;
							object[i].x+=TILE_UNIT;
						} /* if */ 
						if (object[i].y>=object[i].desired_floor) {
							object[i].y=object[i].desired_floor;
							object[i].state=1;
							object[i].state3=0;
						} /* if */ 
						break;
				} /* switch */ 
				break;

			case T_VENT2:
				object[i].state3++;
				switch(object[i].state) {
				case 0:
					if (object[i].state3<6) {
						object[i].tile=3;
					} /* if */ 
					if (object[i].state3>=6 && object[i].state3<22) {
						if (
							(tiles[blob_tile]->
								coltest(GAME_VIEW_X+object[i].x+TILE_UNIT,GAME_VIEW_Y+object[i].y,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							(object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
							(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
										(GAME_VIEW_Y+object[i].y-TILE_UNIT)*dx]&colision)!=0)
						{
							object[i].x+=TILE_UNIT;
						} else {
							object[i].state=1;
							object[i].x-=TILE_UNIT;
						} /* if */ 
						object[i].tile=0;
					} /* if */ 
					if (object[i].state3>=22 && object[i].state3<28) {
						if (object[i].state3==22) Sound_play(S_blob);
						object[i].tile=2;
					} /* if */ 
					if (object[i].state3>=28) {
						object[i].tile=1;
						if (object[i].state3>52) object[i].state3=0;						
					} /* if */ 
					break;
				case 1:
					if (object[i].state3<6) {
						object[i].tile=2;
					} /* if */ 
					if (object[i].state3>=6 && object[i].state3<22) {
						if (
							(tiles[blob_tile]->
								coltest(GAME_VIEW_X+object[i].x-TILE_UNIT,GAME_VIEW_Y+object[i].y,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							(object[i].x-TILE_UNIT)>0 &&
							(col_buffer[GAME_VIEW_X+object[i].x-TILE_UNIT+
										(GAME_VIEW_Y+object[i].y-TILE_UNIT)*dx]&colision)!=0)
						{
							object[i].x-=TILE_UNIT;
						} else {
							object[i].state=0;
							object[i].x+=TILE_UNIT;
						} /* if */ 
						object[i].tile=0;
					} /* if */ 
					if (object[i].state3>=22 && object[i].state3<28) {
						if (object[i].state3==22) Sound_play(S_blob);
						object[i].tile=3;
					} /* if */ 
					if (object[i].state3>=28) {
						object[i].tile=1;
						if (object[i].state3>52) object[i].state3=0;						
					} /* if */ 
					break;
				case 2:
					if (abs(object[i].x-pers_x)<5*TILE_SIZE_X &&
						pers_y>=object[i].y && pers_y<=(object[i].y+TILE_SIZE_Y*2)) {
						if (object[i].x<pers_x) object[i].state=0;
										   else object[i].state=1;
						object[i].state3=0;
					} /* if */ 
					object[i].tile=1;
					break;
				} /* switch */ 
				break;

			case T_LIVINGWALL:
				object[i].state3++;
				if (object[i].state2==0) {
					if ((object[i].y==pers_y-TILE_SIZE_Y) && (object[i].x>(pers_x+2*TILE_SIZE_X))) object[i].state=1;
					if (object[i].state==0) {
						object[i].tile=-1;
					} else {
						object[i].tile=0;
					} /* if */ 
					if (object[i].state!=0 && (object[i].state3&0x01)==0) {
						object[i].x--;
					} /* if */ 
				} else {
					if ((object[i].y==pers_y-TILE_SIZE_Y) && (object[i].x<(pers_x-2*TILE_SIZE_X))) object[i].state=1;
					if (object[i].state==0) {
						object[i].tile=-1;
					} else {
						object[i].tile=0;
					} /* if */ 
					if (object[i].state!=0 && (object[i].state3&0x01)==0) {
						object[i].x++;
					} /* if */ 
				} /* if */ 

				break;

			case T_MEGABAT:
				if ((object[i].state2&0x0f)==8) Sound_play(S_flapwings);
			case T_MEGABAT2:
			case T_MEGABAT3:
				switch(object[i].state3) {
				case -1:object[i].y+=TILE_UNIT;
					break;
				case 0:object[i].y+=TILE_UNIT*2;
					break;
				case 1:object[i].y+=TILE_UNIT*2;
					   object[i].x-=TILE_UNIT*2;
					break;
				case 2:object[i].y-=TILE_UNIT*2;
					   object[i].x-=TILE_UNIT*2;
					break;
				case 3:object[i].y-=TILE_UNIT*2;
					break;
				case 4:object[i].y-=TILE_UNIT*2;
					   object[i].x+=TILE_UNIT*2;
					break;
				case 5:object[i].y+=TILE_UNIT*2;
					   object[i].x+=TILE_UNIT*2;
					break;
				case 6:object[i].x+=TILE_UNIT*2;
					break;
				case 7:object[i].x-=TILE_UNIT*2;
					break;

				case 8:object[i].y+=TILE_UNIT*2;
					   object[i].x-=TILE_UNIT;
					break;
				case 9:object[i].y-=TILE_UNIT*2;
					   object[i].x-=TILE_UNIT;
					break;
				case 10:object[i].y-=TILE_UNIT*2;
					   object[i].x+=TILE_UNIT;
					break;
				case 11:object[i].y+=TILE_UNIT*2;
					   object[i].x+=TILE_UNIT;
					break;

				} /* switch */ 
				object[i].state2++;
				if ((object[i].state2>=16 && object[i].state3!=-1) ||
					(object[i].state2>=32 && object[i].state3==-1) ||
					object[i].y==(room_size_y-2)*TILE_SIZE_Y) {
					object[i].state=1;
					object[i].state2=0;

					/* Si el número aleatorio es mayor que 7, se sigue en la	*/ 
					/* misma dirección.											*/ 
					j=object[i].state3;
					if (j==-1) object[i].state3=rand()%12;
						  else object[i].state3=rand()%20;
					if (object[i].state3>11) object[i].state3=j;

					if (object[i].x>(room_size_x-7)*TILE_SIZE_X) {
						if (object[i].state3==4) object[i].state3=2;
						if (object[i].state3==5) object[i].state3=1;
						if (object[i].state3==6) object[i].state3=7;
						if (object[i].state3==10) object[i].state3=9;
						if (object[i].state3==11) object[i].state3=8;
					} /* if */ 
					if (object[i].x<TILE_SIZE_X*5) {
						if (object[i].state3==2) object[i].state3=4;
						if (object[i].state3==1) object[i].state3=5;
						if (object[i].state3==7) object[i].state3=6;
						if (object[i].state3==9) object[i].state3=10;
						if (object[i].state3==8) object[i].state3=11;
					} /* if */ 
					if (object[i].y>(room_size_y-4)*TILE_SIZE_Y) {
						if (object[i].state3==0) object[i].state3=3;
						if (object[i].state3==1) object[i].state3=2;
						if (object[i].state3==5) object[i].state3=4;
						if (object[i].state3==11) object[i].state3=10;
						if (object[i].state3==8) object[i].state3=9;
					} /* if */ 
					if (object[i].y<TILE_SIZE_Y*5) {
						if (object[i].state3==3) object[i].state3=0;
						if (object[i].state3==2) object[i].state3=1;
						if (object[i].state3==4) object[i].state3=5;
						if (object[i].state3==10) object[i].state3=11;
						if (object[i].state3==9) object[i].state3=8;
					} /* if */ 
				} /* if */ 
				break;
			case T_DEMON3_FIRE:
				object[i].state2++;
				if (object[i].state2>10) object[i].type=T_NADA;
				break;

			case T_DEMON9_FIRE:
				object[i].state2++;
				if (object[i].state2>5) object[i].type=T_NADA;
				break;

			case T_DEMON5_FIRE:
				object[i].state2++;
				if (object[i].state2>5) object[i].type=T_NADA;
				break;

			case T_LAVA1:
				object[i].tile=5;
				object[i].state2++;
				if (object[i].state2>=16) object[i].type=T_NADA;
				break;
			case T_LAVA2:
				if (object[i].state2==0) object[i].state3=22+(rand()%10);
				object[i].tile=object[i].state2%2+(object[i].state2&0x08)/4;
				object[i].state2++;
				if (object[i].state2<object[i].state3) {
					object[i].y-=TILE_UNIT*4;
				} else {
					switch(object[i].state2%6) {
					case 0:
					case 1:
							object[i].y-=TILE_UNIT;
							break;
					case 3:
					case 4:
							object[i].y+=TILE_UNIT;
							break;
					} /* if */ 
					if (object[i].state2==object[i].state3+32) {
						int j;

						object[i].type=T_NADA;
						j=add_object(T_ENEMY,object[i].x,object[i].y,1,T_LAVA3,-4);
						object[j].base_tile=lava_tile;
						j=add_object(T_ENEMY,object[i].x,object[i].y,1,T_LAVA3,-3);
						object[j].base_tile=lava_tile;
						j=add_object(T_ENEMY,object[i].x,object[i].y,1,T_LAVA3,-2);
						object[j].base_tile=lava_tile;
						j=add_object(T_ENEMY,object[i].x,object[i].y,1,T_LAVA3,-1);
						object[j].base_tile=lava_tile;
						j=add_object(T_ENEMY,object[i].x,object[i].y,1,T_LAVA3,1);
						object[j].base_tile=lava_tile;
						j=add_object(T_ENEMY,object[i].x,object[i].y,1,T_LAVA3,2);
						object[j].base_tile=lava_tile;
						j=add_object(T_ENEMY,object[i].x,object[i].y,1,T_LAVA3,3);
						object[j].base_tile=lava_tile;
						j=add_object(T_ENEMY,object[i].x,object[i].y,1,T_LAVA3,4);
						object[j].base_tile=lava_tile;

						Sound_play(S_fireball2);
					} /* if */ 
				} /* if */ 
				break;
			case T_LAVA3:
				{
					int xm[9]={-4,-3,-2,-1,0,1,2,3,4};
					int xm2[9]={-6,-4,-2,-1,0,1,2,4,6};
					int xm3[9]={-7,-5,-3,-1,0,1,3,5,7};

					if (object[i].state2==0) {
						switch(object[i].state) {
						case 4:
						case -4:
								object[i].state2=12;
								break;
						case 3:
						case -3:
								object[i].state2=8;
								break;
						case 2:
						case -2:
								object[i].state2=4;
								break;
						} /* if */ 
					} /* if */ 
					object[i].tile=4;
					if (object[i].state2<16) object[i].x+=xm3[object[i].state+4];
					if (object[i].state2>=16 && object[i].state2<32) object[i].x+=xm2[object[i].state+4];
					if (object[i].state2>=32) object[i].x+=xm[object[i].state+4];
					object[i].state2++;
					if (object[i].state2<20) {
						if (object[i].state2<8) object[i].y-=TILE_UNIT;
						if (object[i].state2<12) object[i].y-=TILE_UNIT;
						if (object[i].state2<16) object[i].y-=TILE_UNIT;				
						if (object[i].state2<18) object[i].y-=TILE_UNIT/2;				
						if (object[i].state2<19) object[i].y-=TILE_UNIT/2;
					} else {
						int miny=TILE_SIZE_Y*12;

						if (map==10) miny=TILE_SIZE_Y*16;
						if (object[i].state2>=22) object[i].y+=TILE_UNIT/2;
						if (object[i].state2>=24) object[i].y+=TILE_UNIT/2;
						if (object[i].state2>=26) object[i].y+=TILE_UNIT;
						if (object[i].state2>=28) object[i].y+=TILE_UNIT;
						if (object[i].state2>=32) object[i].y+=TILE_UNIT;
						if (object[i].state2>=50 && object[i].y>=miny) {
							object[i].enemy=T_LAVA1;
							object[i].state2=0;
						} /* if */ 
					} /* if */ 
				}
				break;
			case T_PIRANHA:
				switch(object[i].state) {
				case 0:
				case 2:
					object[i].tile=object[i].state;
					if (object[i].state2==0) {
						/* Decidir a que altura va a saltar: */ 
						object[i].state2=object[i].state3=(rand()%16)+12;
						Sound_play(S_enterwater);
					} /* if */ 
					if (object[i].state2>=16) object[i].y-=TILE_UNIT*2;
					if (object[i].state2>=12) object[i].y-=TILE_UNIT*2;
					if (object[i].state2>=8) object[i].y-=TILE_UNIT*2;
					if (object[i].state2>=4) object[i].y-=TILE_UNIT*2;
					if (object[i].state2>=2) object[i].y-=TILE_UNIT;
					object[i].state2--;
					if (object[i].state2==0) object[i].state++;
					break;
				case 1:
				case 3:
					object[i].tile=object[i].state;
					if (object[i].state2>=16) object[i].y+=TILE_UNIT*2;
					if (object[i].state2>=12) object[i].y+=TILE_UNIT*2;
					if (object[i].state2>=8) object[i].y+=TILE_UNIT*2;
					if (object[i].state2>=4) object[i].y+=TILE_UNIT*2;
					if (object[i].state2>=2) object[i].y+=TILE_UNIT;
					object[i].state2++;
					if (object[i].state2==object[i].state3) {
						object[i].enemy=T_PIRANHA2;
						object[i].state2=0;
						Sound_play(S_enterwater);
					} /* if */ 
					break;
				} /* switch */ 
				if (object[i].state==0 || object[i].state==1) object[i].x+=TILE_UNIT;	
														 else object[i].x-=TILE_UNIT;
				break;
			case T_PIRANHA2:
				object[i].tile=4;
				object[i].state2++;
				if (object[i].state2>=16) object[i].type=T_NADA;
				break;

			case T_WHITESTAR:
				{
					bool col[8];

					switch(object[i].state3) {
					case 0:object[i].x+=TILE_UNIT;
						break;
					case 1:object[i].x-=TILE_UNIT;
						break;
					case 2:object[i].y+=TILE_UNIT;
						break;
					case 3:object[i].y-=TILE_UNIT;
						break;
					} /* switch */  
					object[i].state2++;
					if ((object[i].state2&0x01)==0) object[i].tile=(object[i].tile+1)%2;

					if ((object[i].x%TILE_SIZE_X)==0 &&
						(object[i].y%TILE_SIZE_Y)==0) {
						int tx,ty;
						tx=object[i].x/TILE_SIZE_X;
						ty=object[i].y/TILE_SIZE_Y;

						col[0]=iswall(tx+2,ty)|iswall(tx+2,ty+1);
						col[1]=iswall(tx-1,ty)|iswall(tx-1,ty+1);
						col[2]=iswall(tx,ty+2)|iswall(tx+1,ty+2);
						col[3]=iswall(tx,ty-1)|iswall(tx+1,ty-1);

						col[4]=iswall(tx+2,ty+2);
						col[5]=iswall(tx-1,ty+2);
						col[6]=iswall(tx+2,ty-1);
						col[7]=iswall(tx-1,ty-1);

						if (col[0]) 
							if (object[i].state==0) {
								object[i].state3=2;
							} else {
								object[i].state3=3;
							} /* if */ 
						if (col[1]) 
							if (object[i].state==0) {
								object[i].state3=3;
							} else {
								object[i].state3=2;
							} /* if */ 
						if (col[2]) 
							if (object[i].state==0) {
								object[i].state3=1;
							} else {
								object[i].state3=0;
							} /* if */ 
						if (col[3]) 
							if (object[i].state==0) {
								object[i].state3=0;
							} else {
								object[i].state3=1;
							} /* if */ 
						if (!col[0] && !col[1] && !col[2] && !col[3]) {
							if (col[4]) 
								if (object[i].state==0) {
									object[i].state3=2;
								} else {
									object[i].state3=0;
								} /* if */ 
							if (col[5]) 
								if (object[i].state==0) {
									object[i].state3=1;
								} else {
									object[i].state3=2;
								} /* if */ 
							if (col[6]) 
								if (object[i].state==0) {
									object[i].state3=0;
								} else {
									object[i].state3=3;
								} /* if */ 
							if (col[7]) 
								if (object[i].state==0) {
									object[i].state3=3;
								} else {
									object[i].state3=1;
								} /* if */ 
						} /* if */ 
					} /* if */ 

					if ((rand()%500)==0) object[i].state=(object[i].state+1)%2;
				}
				break;

			case T_SPIDER:
				object[i].state2++;
				if (object[i].state2>=48) object[i].state2=0;
				if (object[i].state2<16) object[i].tile=0;
				if (object[i].state2>=16 && object[i].state2<24) object[i].tile=1;
				if (object[i].state2>=24 && object[i].state2<40) object[i].tile=2;
				if (object[i].state2>=40 && object[i].state2<48) object[i].tile=1;
				break;

			case T_KNIGHTHEAD:
				object[i].state2++;
				if (object[i].state2>=256 &&
					(object[i].state2&0x01f)>=15) object[i].tile=0;
											 else object[i].tile=1;
				if ((object[i].state2&0x1f)==0 && object[i].state<4*TILE_SIZE_X) object[i].state+=TILE_UNIT;
				object[i].xoffs=int(float(object[i].state)*cos(float(object[i].state2)/10));
				object[i].yoffs=int(float(object[i].state)*sin(float(object[i].state2)/10));
				if ((rand()%200)==0) {
					Sound_play(S_enemybullet);
					j=add_object(T_ENEMY,object[i].x+TILE_SIZE_X+object[i].xoffs,object[i].y+TILE_SIZE_Y+object[i].yoffs,0,T_BUBBLE,0);
					object[j].state=(pers_x)-object[i].x;
					object[j].state2=(pers_y)-(object[i].y+TILE_SIZE_Y);
					object[j].base_tile=knighthead_tile;
					object[j].tile=2;
				} /* if */ 
				break;
			case T_CHICKEN:
				object[i].state3++;
				switch(object[i].state) {
				case 0: object[i].tile=2+((object[i].state3>>3)&0x01);
					    if ((tiles[chicken_tile+object[i].tile]->
							coltest(GAME_VIEW_X+object[i].x+2*TILE_UNIT,GAME_VIEW_Y+object[i].y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							(object[i].x+2*TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
							(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
										(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
							object[i].x+=2*TILE_UNIT;
						} else {
							object[i].state=1;
						} /* if */ 
					   break;
				case 1: object[i].tile=((object[i].state3>>3)&0x01);
					    if ((tiles[chicken_tile+object[i].tile]->
							coltest(GAME_VIEW_X+object[i].x-2*TILE_UNIT,GAME_VIEW_Y+object[i].y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							(object[i].x-2*TILE_UNIT)>0 &&
							(col_buffer[(GAME_VIEW_X+object[i].x-2*TILE_UNIT)+
										(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
							object[i].x-=2*TILE_UNIT;
						} else {
							object[i].state=0;
						} /* if */ 
					   break;
				case 2:	object[i].tile=1;
						if (object[i].state3==32) {
							/* Disparar huevo: */ 
							int obj;
							obj=add_object(T_ENEMY,object[i].x,object[i].y,0,T_CHICKEN_EGG,0);
							object[obj].base_tile=chicken_tile;
							object[obj].tile=4;
							object[obj].state3=pers_x;
							object[i].state2=obj;
						} /* if */ 
						if (object[i].state3>=32 && object[object[i].state2].type==T_NADA) {
							object[i].state=0;
						} /* if */ 
						break;
				case 3:	object[i].tile=3;
						if (object[i].state3==32) {
							/* Disparar huevo: */ 
							int obj;
							obj=add_object(T_ENEMY,object[i].x,object[i].y,0,T_CHICKEN_EGG,0);
							object[obj].base_tile=chicken_tile;
							object[obj].tile=4;
							object[obj].state3=pers_x;
							object[i].state2=obj;
						} /* if */ 
						if (object[i].state3>=32 && object[object[i].state2].type==T_NADA) {
							object[i].state=1;
						} /* if */ 
						break;
				} /* switch */ 
				if (object[i].y==pers_y && object[i].state<2) {
					if (object[i].x<pers_x) {
						object[i].state=3;
						object[i].state3=0;
					} else {
						object[i].state=2;
						object[i].state3=0;
					} /* if */ 
				} /* if */ 
				break;

			case T_CHICKEN_EGG:
				if (object[i].state2==0) {
					/* Comprobar el recorrido: */ 
					int tx;
					for(tx=object[i].x;tx!=object[i].state3;(tx<object[i].state3 ? tx++ : tx--)) {
						if ((tiles[chicken_tile+object[i].tile]->
							coltest(GAME_VIEW_X+tx,GAME_VIEW_Y+object[i].y,
								    col_buffer,dx,dy,dx)&colision)!=0 ||
							(tx)<0 ||
							(tx>room_size_x*TILE_SIZE_X) ||
							(col_buffer[(GAME_VIEW_X+tx+TILE_SIZE_X)+
										(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)==0) {
							if (tx<object[i].state3) {
								object[i].state3=tx-1;
							} else {
								object[i].state3=tx+1;
							} /* if */ 
						} /* if */ 
					} /* for */ 
				} /* if */ 
				object[i].state2++;
				object[i].tile=4+((object[i].state2>>3)&0x01);
				if (object[i].x<object[i].state3) {
					object[i].x++;
					if ((object[i].state3-object[i].x)>16) object[i].x+=TILE_UNIT;
					if ((object[i].state3-object[i].x)>32) object[i].x+=TILE_UNIT;
					if ((object[i].state3-object[i].x)>64) object[i].x+=TILE_UNIT;
				} else {
					object[i].x--;
					if ((object[i].x-object[i].state3)>16) object[i].x-=TILE_UNIT;
					if ((object[i].x-object[i].state3)>32) object[i].x-=TILE_UNIT;
					if ((object[i].x-object[i].state3)>64) object[i].x-=TILE_UNIT;
				} /* if */ 
				if (object[i].state3==object[i].x) {
					object[i].state3=-1;
					object[i].state2=0;
					object[i].enemy=T_EGG_EXPLOSION;
				} /* if */ 
				break;

			case T_EGG_EXPLOSION:
				object[i].state2++;
				if (object[i].state2==32) {
					Sound_play(S_chickenbomb);
					object[i].x-=TILE_SIZE_X;
					object[i].y-=TILE_SIZE_Y;
					object[i].tile=7;
				} /* if */ 
				if (object[i].state2==40) {
					object[i].tile=6;
				} /* if */ 
				if (object[i].state2==48) {
					object[i].type=T_NADA;
				} /* if */ 
				break;

			case T_CLOUD:
				object[i].tile=(object[i].state2>>3)%2;
				switch(object[i].state3) {
				case 0:object[i].y+=TILE_UNIT;
					break;
				case 1:object[i].y+=TILE_UNIT;
					   object[i].x-=TILE_UNIT;
					break;
				case 2:object[i].y-=TILE_UNIT;
					   object[i].x-=TILE_UNIT;
					break;
				case 3:object[i].y-=TILE_UNIT;
					break;
				case 4:object[i].y-=TILE_UNIT;
					   object[i].x+=TILE_UNIT;
					break;
				case 5:object[i].y+=TILE_UNIT;
					   object[i].x+=TILE_UNIT;
					break;
				case 6:object[i].x+=TILE_UNIT;
					break;
				case 7:object[i].x-=TILE_UNIT;
					break;

				case 8:object[i].y+=TILE_UNIT;
					   object[i].x-=TILE_UNIT/2;
					break;
				case 9:object[i].y-=TILE_UNIT;
					   object[i].x-=TILE_UNIT/2;
					break;
				case 10:object[i].y-=TILE_UNIT;
					   object[i].x+=TILE_UNIT/2;
					break;
				case 11:object[i].y+=TILE_UNIT;
					   object[i].x+=TILE_UNIT/2;
					break;

				} /* switch */ 
				object[i].state2++;
				if ((object[i].state2>=16 && object[i].state3!=-1) ||
					(object[i].state2>=32 && object[i].state3==-1) ||
					object[i].y==(room_size_y-2)*TILE_SIZE_Y) {
					object[i].state=1;
					object[i].state2=0;

					/* Si el número aleatorio es mayor que 11, se sigue en la	*/ 
					/* misma dirección.											*/ 
					j=object[i].state3;
				    object[i].state3=rand()%32;
					if (object[i].state3>11) object[i].state3=j;

					if (object[i].x>(room_size_x-7)*TILE_SIZE_X) {
						if (object[i].state3==4) object[i].state3=2;
						if (object[i].state3==5) object[i].state3=1;
						if (object[i].state3==6) object[i].state3=7;
						if (object[i].state3==10) object[i].state3=9;
						if (object[i].state3==11) object[i].state3=8;
					} /* if */ 
					if (object[i].x<TILE_SIZE_X*5) {
						if (object[i].state3==2) object[i].state3=4;
						if (object[i].state3==1) object[i].state3=5;
						if (object[i].state3==7) object[i].state3=6;
						if (object[i].state3==9) object[i].state3=10;
						if (object[i].state3==8) object[i].state3=11;
					} /* if */ 
					if (object[i].y>(room_size_y-4)*TILE_SIZE_Y) {
						if (object[i].state3==0) object[i].state3=3;
						if (object[i].state3==1) object[i].state3=2;
						if (object[i].state3==5) object[i].state3=4;
						if (object[i].state3==11) object[i].state3=10;
						if (object[i].state3==8) object[i].state3=9;
					} /* if */ 
					if (object[i].y<TILE_SIZE_Y*5) {
						if (object[i].state3==3) object[i].state3=0;
						if (object[i].state3==2) object[i].state3=1;
						if (object[i].state3==4) object[i].state3=5;
						if (object[i].state3==10) object[i].state3=11;
						if (object[i].state3==9) object[i].state3=8;
					} /* if */ 
				} /* if */ 
				break;
			case T_ROCKMAN:
				object[i].state2++;
				switch(object[i].state) {
				case 0: object[i].tile=-1;
						if (object[i].state2>=64) {
							Sound_play(S_rockman);
							object[i].state=1;
							object[i].state2=0;
						} /* if */ 
						break;
				case 1: if (object[i].state2<8) object[i].tile=0;
						if (object[i].state2>=8 && object[i].state2<16) object[i].tile=1;
						if (object[i].state2>=16 && object[i].state2<24) object[i].tile=2;
						if (object[i].state2>=24) {
							if (object[i].state3==0) object[i].tile=3;
											    else object[i].tile=5;
						} /* if */ 
						if (object[i].state2>=32) {
							object[i].state=2;
							object[i].state2=0;
						} /* if */ 
						break;
				case 2: if (object[i].state3==0) {
							object[i].tile=3+((object[i].state2>>3)&0x01);
							if ((tiles[rockman_tile+object[i].tile]->
								coltest(GAME_VIEW_X+object[i].x+2*TILE_UNIT,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
								(object[i].x+2*TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
								(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
											(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
								object[i].x+=TILE_UNIT;
							} else {
								object[i].state3=1;
							} /* if */ 
						} else {
							object[i].tile=5+((object[i].state2>>3)&0x01);
							if ((tiles[rockman_tile+object[i].tile]->
								coltest(GAME_VIEW_X+object[i].x-2*TILE_UNIT,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
								(object[i].x-2*TILE_UNIT)>0 &&
								(col_buffer[(GAME_VIEW_X+object[i].x-2*TILE_UNIT)+
											(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
								object[i].x-=TILE_UNIT;
							} else {
								object[i].state3=0;
							} /* if */ 
						} /* if */ 
						if (object[i].state2>=128 && (rand()%400)==0) {
							Sound_play(S_rockman);
							object[i].state=3;
							object[i].state2=0;
						} /* if */ 
						break;
				case 3: if (object[i].state2>=24) object[i].tile=0;
						if (object[i].state2>=16 && object[i].state2<24) object[i].tile=1;
						if (object[i].state2>=8 && object[i].state2<16) object[i].tile=2;
						if (object[i].state2<8) {
							if (object[i].state3==0) object[i].tile=3;
											    else object[i].tile=5;
						} /* if */ 
						if (object[i].state2>=32) {
							object[i].state=0;
							object[i].state2=-(rand()%8)*8;
						} /* if */ 
						break;
				} /* switch */ 
				break;
			
			case T_BFLY_MEDIUM:
			case T_BFLY_LARGE:
			case T_BFLY_GIANT:
			case T_BFLY_SMALL:
				if (object[i].enemy==T_BFLY_SMALL) object[i].tile=1+((object[i].state>>3)&0x01);
				if (object[i].enemy==T_BFLY_MEDIUM) object[i].tile=3+((object[i].state>>3)&0x01);
				if (object[i].enemy==T_BFLY_LARGE) object[i].tile=5+((object[i].state>>3)&0x01);
				if (object[i].enemy==T_BFLY_GIANT) object[i].tile=7+((object[i].state>>4)&0x01);
				object[i].state2++;
				object[i].state++;
				
				if (object[i].union_x!=-1) {
					/* Fusioning Butterflys: */ 
					if (object[i].x>(object[i].union_x+TILE_UNIT*4)) {
						object[i].x-=TILE_UNIT*4;
					} else {
						if (object[i].x<(object[i].union_x-TILE_UNIT*4)) {
							object[i].x+=TILE_UNIT*4;
						} else {
							object[i].x=object[i].union_x;
						} /* if */ 
					} /* if */ 
					if (object[i].y>(object[i].union_y+TILE_UNIT*4)) {
						object[i].y-=TILE_UNIT*4;
					} else {
						if (object[i].y<(object[i].union_y-TILE_UNIT*4)) {
							object[i].y+=TILE_UNIT*4;
						} else {
							object[i].y=object[i].union_y;
						} /* if */ 
					} /* if */ 

					if (object[i].x==object[i].union_x &&
						object[i].y==object[i].union_y &&
						object[i].x==object[object[i].desired_floor].x &&
						object[i].y==object[object[i].desired_floor].y) {
						/* FUSION! */ 
						Sound_play(S_blob);
						object[object[i].desired_floor].type=T_NADA;
						object[i].desired_floor=-1;
						object[i].union_x=-1;
						object[i].union_x=-1;
						if (object[i].enemy==T_BFLY_LARGE) {
							object[i].enemy=T_BFLY_GIANT;
							object[i].size=8;
							object[i].experience=16;
						} /* if */ 
						if (object[i].enemy==T_BFLY_MEDIUM) {
							object[i].enemy=T_BFLY_LARGE;
							object[i].size=4;
							object[i].experience=8;
						} /* if */ 
						if (object[i].enemy==T_BFLY_SMALL) {
							object[i].enemy=T_BFLY_MEDIUM;
							object[i].size=2;
							object[i].experience=4;
						} /* if */ 
						object[i].x-=TILE_SIZE_X/2;
						object[i].y-=TILE_SIZE_Y/2;
						object[i].state=0;
					} /* if */ 
				} else {
					/* Flying Butterflys: */ 
					switch(object[i].state3) {
					case 0:object[i].y+=TILE_UNIT;
						break;
					case 1:object[i].y+=TILE_UNIT;
						   object[i].x-=TILE_UNIT;
						break;
					case 2:object[i].y-=TILE_UNIT;
						   object[i].x-=TILE_UNIT;
						break;
					case 3:object[i].y-=TILE_UNIT;
						break;
					case 4:object[i].y-=TILE_UNIT;
						   object[i].x+=TILE_UNIT;
						break;
					case 5:object[i].y+=TILE_UNIT;
						   object[i].x+=TILE_UNIT;
						break;
					case 6:object[i].x+=TILE_UNIT;
						break;
					case 7:object[i].x-=TILE_UNIT;
						break;

					case 8:object[i].y+=TILE_UNIT;
						   object[i].x-=TILE_UNIT/2;
						break;
					case 9:object[i].y-=TILE_UNIT;
						   object[i].x-=TILE_UNIT/2;
						break;
					case 10:object[i].y-=TILE_UNIT;
						   object[i].x+=TILE_UNIT/2;
						break;
					case 11:object[i].y+=TILE_UNIT;
						   object[i].x+=TILE_UNIT/2;
						break;

					} /* switch */ 
					object[i].state2++;
					if (object[i].state2>=16) { 
						object[i].state2=0;

						/* Si el número aleatorio es mayor que 11, se sigue en la	*/ 
						/* misma dirección.											*/ 
						j=object[i].state3;
						object[i].state3=rand()%32;
						if (object[i].state3>11) object[i].state3=j;

						if (object[i].x>(room_size_x-7)*TILE_SIZE_X) {
							if (object[i].state3==4) object[i].state3=2;
							if (object[i].state3==5) object[i].state3=1;
							if (object[i].state3==6) object[i].state3=7;
							if (object[i].state3==10) object[i].state3=9;
							if (object[i].state3==11) object[i].state3=8;
						} /* if */ 
						if (object[i].x<TILE_SIZE_X*5) {
							if (object[i].state3==2) object[i].state3=4;
							if (object[i].state3==1) object[i].state3=5;
							if (object[i].state3==7) object[i].state3=6;
							if (object[i].state3==9) object[i].state3=10;
							if (object[i].state3==8) object[i].state3=11;
						} /* if */ 
						if (object[i].y>(room_size_y-4)*TILE_SIZE_Y) {
							if (object[i].state3==0) object[i].state3=3;
							if (object[i].state3==1) object[i].state3=2;
							if (object[i].state3==5) object[i].state3=4;
							if (object[i].state3==11) object[i].state3=10;
							if (object[i].state3==8) object[i].state3=9;
						} /* if */ 
						if (object[i].y<TILE_SIZE_Y*5) {
							if (object[i].state3==3) object[i].state3=0;
							if (object[i].state3==2) object[i].state3=1;
							if (object[i].state3==4) object[i].state3=5;
							if (object[i].state3==10) object[i].state3=11;
							if (object[i].state3==9) object[i].state3=8;
						} /* if */ 
					} /* if */ 
					if ((object[i].enemy==T_BFLY_MEDIUM && (rand()%150)==0) ||
						(object[i].enemy==T_BFLY_LARGE && (rand()%80)==0) ||
						(object[i].enemy==T_BFLY_GIANT && (rand()%50)==0)) {
						int o;
						/* Disparar: */ 
						o=add_object(T_ENEMY,object[i].x+TILE_SIZE_X,object[i].y+TILE_SIZE_Y,0,T_BFLY_BULLET,0);
						object[o].base_tile=bfly_tile;
						object[o].tile=0;
					} /* if */ 

					if (object[i].state>=256) {
						if (object[i].enemy!=T_BFLY_GIANT) {
							int other,j,n,me=0;
							int others[8];

							for(j=0,n=0;j<n_objects;j++) {
								if (object[j].type==T_ENEMY && object[j].enemy==object[i].enemy) {
									others[n]=j;
									if (j==i) me=n;
									n++;
								} /* if */ 
							} /* for */ 
							if ((me&0x01)==0) other=me+1;
										 else other=me-1;
							if (other>=0 && other<n) {
								int tx,ty;
								object[i].desired_floor=others[other];
								object[others[other]].desired_floor=i;
								tx=(object[i].x+object[others[other]].x)/2;
								ty=(object[i].y+object[others[other]].y)/2;
								object[others[other]].union_x=object[i].union_x=tx;
								object[others[other]].union_y=object[i].union_y=ty;					
							} /* if */ 
						} /* if */ 
					} /* if */ 
				} /* if */ 

				break;
			case T_BFLY_BULLET:
				switch(object[i].state3) {
				case 0:object[i].y+=TILE_UNIT;
					break;
				case 1:object[i].y+=TILE_UNIT;
					   object[i].x-=TILE_UNIT;
					break;
				case 2:object[i].y-=TILE_UNIT;
					   object[i].x-=TILE_UNIT;
					break;
				case 3:object[i].y-=TILE_UNIT;
					break;
				case 4:object[i].y-=TILE_UNIT;
					   object[i].x+=TILE_UNIT;
					break;
				case 5:object[i].y+=TILE_UNIT;
					   object[i].x+=TILE_UNIT;
					break;
				case 6:object[i].x+=TILE_UNIT;
					break;
				case 7:object[i].x-=TILE_UNIT;
					break;

				case 8:object[i].y+=TILE_UNIT;
					   object[i].x-=TILE_UNIT/2;
					break;
				case 9:object[i].y-=TILE_UNIT;
					   object[i].x-=TILE_UNIT/2;
					break;
				case 10:object[i].y-=TILE_UNIT;
					   object[i].x+=TILE_UNIT/2;
					break;
				case 11:object[i].y+=TILE_UNIT;
					   object[i].x+=TILE_UNIT/2;
					break;

				} /* switch */ 
				object[i].state2++;
				if (object[i].state2>=16) { 
					object[i].state2=0;

					/* Si el número aleatorio es mayor que 11, se sigue en la	*/ 
					/* misma dirección.											*/ 
					j=object[i].state3;
					object[i].state3=rand()%32;
					if (object[i].state3>11) object[i].state3=j;
				} /* if */ 
				if (object[i].x>=TILE_SIZE_X*room_size_x ||
					object[i].y>=TILE_SIZE_X*room_size_y ||
					object[i].x<0 ||
					object[i].y<0) {
					object[i].type=T_NADA;
				} /* if */ 
					
				break;

			case T_ARMOUR_ARROW:
				{
					int colision;

					colision=T_WALL|T_LADDER_WALL|T_DOOR_WALL;
					if ((tiles[armourarrow_tile+object[i].tile]->
							   coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							   TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
						object[i].type=T_NADA;
					} /* if */ 
					if (object[i].state==0) {
						object[i].x-=TILE_SIZE_X/2;
						object[i].tile=0;
						if (object[i].x<(-TILE_SIZE_X*2)) object[i].type=T_NADA;
					} else {
						object[i].x+=TILE_SIZE_X/2;
						object[i].tile=1;
						if (object[i].x>=room_size_x*TILE_SIZE_X) object[i].type=T_NADA;
					} /* if */ 
				}
				break;

			case T_GHOST:
				object[i].state2++;
				object[i].tile=(object[i].state2>>2)%2;
				if ((object[i].state2&0x1F)==0) {
					/* Disparar: */ 
					j=add_object(T_ENEMY,object[i].x+TILE_SIZE_X/2,object[i].y+TILE_SIZE_Y/2,0,T_GHOST_BULLET,0);
					object[j].state=(pers_x)-object[i].x;
					object[j].state2=(pers_y)-(object[i].y+TILE_SIZE_Y);
					object[j].base_tile=ghost_tile;
					object[j].tile=2;
					object[j].size=1;
					Sound_play(S_enemybullet);
				} /* if */ 
				if (object[i].state2>=80 &&
					(object[i].state2&0x01)==0) object[i].tile=-1;
				if (object[i].state2==88) {
					object[i].state2=0;
					/* Cambio de posición: */ 
					int j;
					bool posibles[25];
					int nposibles,selected,act;
					int xs[5]={6,11,16,21,25};
					int ys[5]={1,5,9,13,17};
					int xt,yt;

					nposibles=0;
					for(j=0;j<25;j++) {
						xt=xs[j%5];
						yt=ys[j/5];
						if (!iswall(xt,yt) &&
							!iswall(xt+1,yt) &&
							iswall(xt,yt+2) &&
							iswall(xt+1,yt+2)) {
							posibles[j]=true;
							nposibles++;
						} else {
							posibles[j]=false;
						} /* if */ 
					} /* if */ 
					selected=rand()%nposibles;
					act=0;
					for(j=0;j<25;j++) {
						if (posibles[j]) {
							if (act==selected) {
								object[i].x=xs[j%5]*TILE_SIZE_X;
								object[i].y=ys[j/5]*TILE_SIZE_Y;
							} /* if */ 
							act++;
						} /* if */ 
					} /* for */ 
				} /* if */ 
				break;

			case T_GHOST_BULLET:
				{
					int j;
					for(j=0;j<2;j++) {
						if (abs(object[i].state)>abs(object[i].state2)) {
							object[i].x+=(object[i].state>0 ? TILE_UNIT:-TILE_UNIT);
							object[i].state3-=abs(object[i].state2);
							if (object[i].state3<0) {
								object[i].state3+=abs(object[i].state);
								object[i].y+=(object[i].state2>0 ? TILE_UNIT:-TILE_UNIT);
							} /* if */ 
						} else {
							object[i].y+=(object[i].state2>0 ? TILE_UNIT:-TILE_UNIT);
							object[i].state3-=abs(object[i].state);
							if (object[i].state3<0) {
								object[i].state3+=abs(object[i].state2);
								object[i].x+=(object[i].state>0 ? TILE_UNIT:-TILE_UNIT);
							} /* if */ 
						} /* if */ 
						if (object[i].x<0 || object[i].x>=dx ||
							object[i].y<0 || object[i].y>=dy) {
							object[i].type=T_NADA;
						} /* if */ 
					} /* for */ 
				}
				break;

			case T_HEAD:
				switch(object[i].state) {
				case 0:
					object[i].tile=2;
					if (pers_x>object[i].x &&
						pers_y>=object[i].y-TILE_SIZE_Y &&
						pers_y<object[i].y+TILE_SIZE_Y) {
						object[i].state2++;
						if (object[i].state2>=64) {
							object[i].state=2;
							object[i].state2=128;
						} /* if */ 
					} else {
						object[i].state2=0;
					} /* if */ 
					break;
				case 1:
					object[i].tile=3;
					if (pers_x<object[i].x &&
						pers_y>=object[i].y-TILE_SIZE_Y &&
						pers_y<object[i].y+TILE_SIZE_Y) {
						object[i].state2++;
						if (object[i].state2>=64) {
							object[i].state=3;
							object[i].state2=128;
						} /* if */ 
					} else {
						object[i].state2=0;
					} /* if */ 
					break;
				case 2:
					object[i].tile=0;
					object[i].state2--;
					if ((object[i].state2&0x07)==0) {
						int j;
						/* Disparar bola: */ 
						j=add_object(T_ENEMY,object[i].x+TILE_SIZE_X,object[i].y+TILE_SIZE_Y/2,0,T_HEAD_BULLET,0);
						object[j].state=((pers_x)-object[i].x)+(rand()%(4*TILE_SIZE_X))-2*TILE_SIZE_X;
						object[j].state2=((pers_y)-(object[i].y))+(rand()%(4*TILE_SIZE_Y))-2*TILE_SIZE_Y;
						object[j].base_tile=head_tile;
						object[j].tile=4;
						object[j].size=1;
						Sound_play(S_headbullet);
					} /* if */ 
					if (object[i].state2<=0) object[i].state=0;
					break;
				case 3:
					object[i].tile=1;
					object[i].state2--;
					if ((object[i].state2&0x07)==0) {
						int j;
						/* Disparar bola: */ 
						j=add_object(T_ENEMY,object[i].x-TILE_SIZE_X,object[i].y+TILE_SIZE_Y/2,0,T_HEAD_BULLET,0);
						object[j].state=((pers_x)-object[i].x)+(rand()%(4*TILE_SIZE_X))-2*TILE_SIZE_X;
						object[j].state2=((pers_y)-(object[i].y))+(rand()%(4*TILE_SIZE_Y))-2*TILE_SIZE_Y;
						object[j].base_tile=head_tile;
						object[j].tile=4;
						object[j].size=1;
						Sound_play(S_headbullet);
					} /* if */ 
					if (object[i].state2<=0) object[i].state=1;
					break;
				} /* switch */ 
				break;

			case T_HEAD_BULLET:
				{
					int j;
					for(j=0;j<2;j++) {
						if (abs(object[i].state)>abs(object[i].state2)) {
							object[i].x+=(object[i].state>0 ? TILE_UNIT:-TILE_UNIT);
							object[i].state3-=abs(object[i].state2);
							if (object[i].state3<0) {
								object[i].state3+=abs(object[i].state);
								object[i].y+=(object[i].state2>0 ? TILE_UNIT:-TILE_UNIT);
							} /* if */ 
						} else {
							object[i].y+=(object[i].state2>0 ? TILE_UNIT:-TILE_UNIT);
							object[i].state3-=abs(object[i].state);
							if (object[i].state3<0) {
								object[i].state3+=abs(object[i].state2);
								object[i].x+=(object[i].state>0 ? TILE_UNIT:-TILE_UNIT);
							} /* if */ 
						} /* if */ 
						if (object[i].x<0 || object[i].x>=dx ||
							object[i].y<0 || object[i].y>=dy) {
							object[i].type=T_NADA;
						} /* if */ 
					} /* for */ 
				}
				break;

			case T_WORM2:
				object[i].state2++;
				switch(object[i].state) {
				case 0:
					object[i].tile=(object[i].state2>>3)%2;
					if (
						(tiles[worm2_tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							 (object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X) {
						if ((col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
							(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)==0) {
							if ((col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*7+
											(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0 &&
								(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*7+
											(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2-1)*dx]&colision)==0) {
								object[i].state2=0;
								object[i].state=2;
							} else {
								object[i].state=1;
							} /* if */ 
						} else {
							if ((object[i].state2%2)==0) object[i].x+=TILE_UNIT;
						} /* if */ 
					} else {
						object[i].state=1;
					} /* if */ 
					break;
				case 1:
					object[i].tile=((object[i].state2>>3)%2)+2;
					if (
						(tiles[worm2_tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
							 object[i].x>0) {
						if ((col_buffer[GAME_VIEW_X+object[i].x-1+
										(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)==0) {
							if ((col_buffer[GAME_VIEW_X+object[i].x-(1+TILE_SIZE_X*5)+
											(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0 &&
								(col_buffer[GAME_VIEW_X+object[i].x-(1+TILE_SIZE_X*5)+
											(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2-1)*dx]&colision)==0) {
								object[i].state2=0;
								object[i].state=3;
							} else {
								object[i].state=0;
							} /* if */ 
						} else {
							if ((object[i].state2%2)==0) object[i].x-=TILE_UNIT;
						} /* if */ 
					} else {
						object[i].state=0;
					} /* if */ 
					break;
					/* Saltando! */ 
				case 2:
					object[i].tile=1;
					if (object[i].state2<6) object[i].y-=TILE_UNIT*2;
					if (object[i].state2>=6 && object[i].state2<14) object[i].y-=TILE_UNIT;
					if (object[i].state2>=14 && object[i].state2<20) object[i].y-=TILE_UNIT/2;

					if (object[i].state2>=30 && object[i].state2<36) object[i].y+=TILE_UNIT/2;
					if (object[i].state2>=36 && object[i].state2<44) object[i].y+=TILE_UNIT;
					if (object[i].state2>=44) object[i].y+=TILE_UNIT*2;
					object[i].x+=TILE_UNIT;
					if (object[i].state2>=48) object[i].state=0;
					break;
				case 3:
					object[i].tile=3;
					if (object[i].state2<6) object[i].y-=TILE_UNIT*2;
					if (object[i].state2>=6 && object[i].state2<14) object[i].y-=TILE_UNIT;
					if (object[i].state2>=14 && object[i].state2<20) object[i].y-=TILE_UNIT/2;

					if (object[i].state2>=30 && object[i].state2<36) object[i].y+=TILE_UNIT/2;
					if (object[i].state2>=36 && object[i].state2<44) object[i].y+=TILE_UNIT;
					if (object[i].state2>=44) object[i].y+=TILE_UNIT*2;
					object[i].x-=TILE_UNIT;
					if (object[i].state2>=48) object[i].state=1;
					break;
				} /* switch */ 
				break;

			case T_OCTOPUS:
				object[i].state2++;
				object[i].tile=((object[i].state2>>2)%2)*8;
				switch(object[i].state) {
				case 0: object[i].yoffs=int(sin(float(object[i].state2)/4.0)*TILE_SIZE_Y);
						break;
				case 1: /* Buscar un objetivo: */ 
						{
							int nposibles=0;
							int posibles[16];
							int j;

							object[i].yoffs=0;
							for(j=0;j<n_objects;j++) {
								if (object[j].type==T_ENEMY &&
									object[j].enemy==T_OCTOPUS &&
									j!=i) {
									posibles[nposibles++]=j;
								} /* if */ 
							} /* for */ 
							j=posibles[rand()%nposibles];
							object[i].union_x=object[j].x-object[i].x;
							object[i].union_y=object[j].y-object[i].y;
							object[i].desired_floor=j;
							object[i].state=2;
							object[i].state3=0;
						}
						break;
				case 2:	{
							int j;
							int tx=object[i].union_x,ty=object[i].union_y;

							for(j=0;j<4 && (abs(object[i].union_x)>TILE_UNIT ||
											abs(object[i].union_y)>TILE_UNIT);j++) {
								if (abs(tx)>abs(ty)) {
									int tmp=(tx>0 ? TILE_UNIT:-TILE_UNIT);
									object[i].x+=tmp;
									object[i].union_x-=tmp;
									object[i].state3-=abs(ty);
									if (object[i].state3<0) {
										int tmp=(ty>0 ? TILE_UNIT:-TILE_UNIT);
										object[i].state3+=abs(tx);
										object[i].y+=tmp;
										object[i].union_y-=tmp;
									} /* if */ 
								} else {
									int tmp=(ty>0 ? TILE_UNIT:-TILE_UNIT);
									object[i].y+=tmp;
									object[i].union_y-=tmp;
									object[i].state3-=abs(tx);
									if (object[i].state3<0) {
										int tmp=(tx>0 ? TILE_UNIT:-TILE_UNIT);
										object[i].state3+=abs(ty);
										object[i].x+=tmp;
										object[i].union_x-=tmp;
									} /* if */ 
								} /* if */ 
							} /* for */ 

						}

						if (abs(object[i].union_x)<=TILE_UNIT &&
							abs(object[i].union_y)<=TILE_UNIT) {
							Sound_play(S_colormonster);
							object[i].x=object[object[i].desired_floor].x;
							object[i].y=object[object[i].desired_floor].y;
							object[i].state=0;
							object[i].state2=0;
							object[object[i].desired_floor].state=1;
						} /* if */ 
						break;
				} /* if */ 
				break;

			case T_HAIRY:
				object[i].state3++;
				object[i].tile=(object[i].state3>>4)&0x01;
				if ((object[i].state3%0x0f)==0) {
					int j=add_object(T_ENEMY,object[i].x+TILE_SIZE_X,object[i].y+TILE_SIZE_Y,0,T_HAIRYBULLET,0);
					object[j].state=1+(rand()%11);
					object[j].base_tile=hairy_tile;
					object[j].tile=2;
				} /* if */ 
				switch(object[i].state) {
				case 1:object[i].y+=TILE_UNIT;
					   object[i].x-=TILE_UNIT;
					break;
				case 2:object[i].y-=TILE_UNIT;
					   object[i].x-=TILE_UNIT;
					break;
				case 3:object[i].y-=TILE_UNIT;
					break;
				case 4:object[i].y-=TILE_UNIT;
					   object[i].x+=TILE_UNIT;
					break;
				case 5:object[i].y+=TILE_UNIT;
					   object[i].x+=TILE_UNIT;
					break;
				case 6:object[i].x+=TILE_UNIT;
					break;
				case 7:object[i].x-=TILE_UNIT;
					break;

				case 8:object[i].y+=TILE_UNIT;
					   object[i].x-=TILE_UNIT/2;
					break;
				case 9:object[i].y-=TILE_UNIT;
					   object[i].x-=TILE_UNIT/2;
					break;
				case 10:object[i].y-=TILE_UNIT;
					   object[i].x+=TILE_UNIT/2;
					break;
				case 11:object[i].y+=TILE_UNIT;
					   object[i].x+=TILE_UNIT/2;
					break;

				} /* switch */ 
				object[i].state2++;
				if (object[i].state2>=16) { 
					object[i].state2=0;

					/* Si el número aleatorio es mayor que 11, se sigue en la	*/ 
					/* misma dirección.											*/ 
					j=object[i].state;
					object[i].state=1+(rand()%31);
					if (object[i].state>11) object[i].state=j;

					if (object[i].x>(room_size_x-7)*TILE_SIZE_X) {
						if (object[i].state==4) object[i].state=2;
						if (object[i].state==5) object[i].state=1;
						if (object[i].state==6) object[i].state=7;
						if (object[i].state==10) object[i].state=9;
						if (object[i].state==11) object[i].state=8;
					} /* if */ 
					if (object[i].x<TILE_SIZE_X*5) {
						if (object[i].state==2) object[i].state=4;
						if (object[i].state==1) object[i].state=5;
						if (object[i].state==7) object[i].state=6;
						if (object[i].state==9) object[i].state=10;
						if (object[i].state==8) object[i].state=11;
					} /* if */ 
					if (object[i].y>(room_size_y-4)*TILE_SIZE_Y) {
						if (object[i].state==0) object[i].state=3;
						if (object[i].state==1) object[i].state=2;
						if (object[i].state==5) object[i].state=4;
						if (object[i].state==11) object[i].state=10;
						if (object[i].state==8) object[i].state=9;
					} /* if */ 
					if (object[i].y<TILE_SIZE_Y*5) {
						if (object[i].state==3) object[i].state=0;
						if (object[i].state==2) object[i].state=1;
						if (object[i].state==4) object[i].state=5;
						if (object[i].state==10) object[i].state=11;
						if (object[i].state==9) object[i].state=8;
					} /* if */ 
				} /* if */ 
				break;

			case T_HAIRYBULLET:
				{
					int seq[4]={0,3,1,2};

					object[i].state2++;
					object[i].tile=2+seq[(object[i].state2>>2)%4];

					switch(object[i].state) {
					case 1:object[i].y+=TILE_UNIT;
						   object[i].x-=TILE_UNIT;
						break;
					case 2:object[i].y-=TILE_UNIT;
						   object[i].x-=TILE_UNIT;
						break;
					case 3:object[i].y-=TILE_UNIT;
						break;
					case 4:object[i].y-=TILE_UNIT;
						   object[i].x+=TILE_UNIT;
						break;
					case 5:object[i].y+=TILE_UNIT;
						   object[i].x+=TILE_UNIT;
						break;
					case 6:object[i].x+=TILE_UNIT;
						break;
					case 7:object[i].x-=TILE_UNIT;
						break;

					case 8:object[i].y+=TILE_UNIT;
						   object[i].x-=TILE_UNIT/2;
						break;
					case 9:object[i].y-=TILE_UNIT;
						   object[i].x-=TILE_UNIT/2;
						break;
					case 10:object[i].y-=TILE_UNIT;
						   object[i].x+=TILE_UNIT/2;
						break;
					case 11:object[i].y+=TILE_UNIT;
						   object[i].x+=TILE_UNIT/2;
						break;

					} /* switch */ 

					if (object[i].x<-2*TILE_SIZE_X ||
						object[i].x>32*TILE_SIZE_X ||
						object[i].y<-2*TILE_SIZE_Y ||
						object[i].y>=20*TILE_SIZE_Y) object[i].type=T_NADA;
				}
				break;

			case T_WATERDRAGON:
				switch(object[i].state) {
				case 0:
				case 1:
						Sound_play(S_enterwater);
						object[i].desired_floor=1+((rand()%40)/10)*4;
						object[i].state+=2;
						object[i].state2=0;
						break;
				case 2:
				case 3:
						if (object[i].y>object[i].desired_floor*TILE_SIZE_Y) {
							object[i].y-=TILE_SIZE_Y/2;
						} else {
							object[i].state+=2;
						} /* if */ 
						break;
				case 4:
				case 5:
						if (object[i].state2<2) object[i].y-=TILE_UNIT*3;
						if (object[i].state2>=2 && object[i].state2<4) object[i].y-=TILE_UNIT*2;
						if (object[i].state2>=4 && object[i].state2<6) object[i].y-=TILE_UNIT;
						if (object[i].state2>=6 && object[i].state2<8) object[i].y-=TILE_UNIT/2;
						if (object[i].state2>=10 && object[i].state2<12) object[i].y+=TILE_UNIT/2;
						if (object[i].state2>=12 && object[i].state2<14) object[i].y+=TILE_UNIT;
						if (object[i].state2>=14 && object[i].state2<16) object[i].y+=TILE_UNIT*2;
						if (object[i].state2>=16 && object[i].state2<18) object[i].y+=TILE_UNIT*3;
						if (object[i].state2==18) {
							int tx,ty;
							object[i].y=object[i].desired_floor*TILE_SIZE_Y;

							tx=object[i].x/TILE_SIZE_X;
							ty=object[i].y/TILE_SIZE_Y;
							if (iswall_or_blockingobject(tx+1,ty+1) ||
								iswall_or_blockingobject(tx+2,ty+1) ||
								!iswall_or_blockingobject(tx+1,ty+2) ||
								!iswall_or_blockingobject(tx+2,ty+2)) {
								object[i].state+=6;
							} else {
								object[i].state+=2;
								object[i].state2=0;
								object[i].state3=0;
							} /* if */ 
						} else {
							object[i].state2++;
						} /* if */ 
						break;
				case 6: 
						{
							int tx,ty;

							object[i].state2++;
							object[i].tile=(object[i].state2>>3)%2;
				
							tx=object[i].x/TILE_SIZE_X;
							ty=object[i].y/TILE_SIZE_Y;
							if (iswall_or_blockingobject(tx+1,ty+1) ||
								iswall_or_blockingobject(tx+2,ty+1) ||
								iswall_or_blockingobject(tx+3,ty+1) ||
								!iswall_or_blockingobject(tx+1,ty+2) ||
								!iswall_or_blockingobject(tx+2,ty+2) ||
								!iswall_or_blockingobject(tx+3,ty+2)) {
								object[i].state=7;
								object[i].x-=TILE_UNIT*2;
								object[i].state3++;
								if (object[i].state3>=4) {
									object[i].state+=2;
									object[i].state2=0;
								} /* if */ 
							} else {
								object[i].x+=TILE_UNIT*2;
							} /* if */ 
							
						}						
						break;
				case 7:
						{
							int tx,ty;

							object[i].state2++;
							object[i].tile=2+(object[i].state2>>3)%2;
				
							tx=object[i].x/TILE_SIZE_X;
							ty=object[i].y/TILE_SIZE_Y;
							if (iswall_or_blockingobject(tx+1,ty+1) ||
								iswall_or_blockingobject(tx+2,ty+1) ||
								iswall_or_blockingobject(tx+3,ty+1) ||
								!iswall_or_blockingobject(tx+1,ty+2) ||
								!iswall_or_blockingobject(tx+2,ty+2) ||
								!iswall_or_blockingobject(tx+3,ty+2)) {
								object[i].state=6;
								object[i].x+=TILE_UNIT*2;
								object[i].state3++;
								if (object[i].state3>=4) {
									object[i].state+=2;
									object[i].state2=0;
								} /* if */ 
							} else {
								object[i].x-=TILE_UNIT*2;
							} /* if */ 
							
						}						
						break;
				case 8:
				case 9:
						if (object[i].state2<2) object[i].y-=TILE_UNIT*3;
						if (object[i].state2>=2 && object[i].state2<4) object[i].y-=TILE_UNIT*2;
						if (object[i].state2>=4 && object[i].state2<6) object[i].y-=TILE_UNIT;
						if (object[i].state2>=6 && object[i].state2<8) object[i].y-=TILE_UNIT/2;
						if (object[i].state2>=10 && object[i].state2<12) object[i].y+=TILE_UNIT/2;
						if (object[i].state2>=12 && object[i].state2<14) object[i].y+=TILE_UNIT;
						if (object[i].state2>=14 && object[i].state2<16) object[i].y+=TILE_UNIT*2;
						if (object[i].state2>=16 && object[i].state2<18) object[i].y+=TILE_UNIT*3;
						if (object[i].state2==18) {
							object[i].state+=2;
							object[i].y=object[i].desired_floor*TILE_SIZE_Y;
						} else {
							object[i].state2++;
						} /* if */ 
						break;
				case 10:
				case 11:
						if (object[i].y<14*TILE_SIZE_Y) {
							object[i].y+=TILE_SIZE_Y/2;
						} else {
							Sound_play(S_enterwater);
							object[i].type=T_ENEMY;
							object[i].enemy=T_PIRANHA2;
							object[i].x+=TILE_SIZE_X;
							object[i].state=0;
							object[i].state2=0;
							object[i].state3=0;
							object[i].size=0;
							object[i].base_tile=piranha_tile;
							object[i].tile=0;
						} /* if */ 
						break;
				} /* switch */ 
				break;

			case T_WATERBUG:
				switch(object[i].state) {
				case 0:
					object[i].state2++;
					object[i].tile=(object[i].state2>>4)%2;
					if (
						(tiles[waterbug_tile]->
						coltest(GAME_VIEW_X+object[i].x+4*TILE_UNIT,GAME_VIEW_Y+object[i].y,
							 col_buffer,dx,dy,dx)&colision)==0 &&
							 (object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X) {
						object[i].x+=4*TILE_UNIT;
					} else {
						object[i].state=1;
					} /* if */ 
					if ((rand()%200)==0) {
						object[i].state=64;
						object[i].state3=0;
					} /* if */ 
					break;
				case 1:
					object[i].state2++;
					object[i].tile=((object[i].state2>>4)%2)+2;
					if (
						(tiles[waterbug_tile+2]->
						coltest(GAME_VIEW_X+object[i].x-4*TILE_UNIT,GAME_VIEW_Y+object[i].y,
							 col_buffer,dx,dy,dx)&colision)==0 &&
							 object[i].x>0) {
						object[i].x-=4*TILE_UNIT;
					} else {
						object[i].state=0;
					} /* if */ 
					if ((rand()%200)==0) {
						object[i].state=64;
						object[i].state3=1;
					} /* if */ 
					break;
				default:
					object[i].state--;
					if (object[i].state==2) object[i].state=object[i].state3;
				} /* switch */ 
				break;

			case T_BIRD:
				switch(object[i].state) {
				case 0:
					object[i].state2--;
					if (object[i].state2<=0) object[i].state=3;
					break;
				case 1:
					object[i].tile=((object[i].state2>>3)%2)+(1-object[i].state3);
					object[i].state2++;
					if (object[i].state2<=6) {
						object[i].x+=TILE_UNIT*object[i].state3;
						object[i].y+=TILE_UNIT;
					} /* if */ 
					if (object[i].state2>6 && object[i].state2<=10) object[i].x+=TILE_UNIT*object[i].state3;
					if (object[i].state2>10) {
						object[i].x+=TILE_UNIT*object[i].state3;
						object[i].y-=TILE_UNIT;
					} /* if */ 
					if (object[i].state2==16) {
						if (object[i].x>=(room_size_x-6)*TILE_SIZE_X) object[i].state3=-1;
						if (object[i].x<=4*TILE_SIZE_X) object[i].state3=1;
						object[i].state2=0;
						object[i].state=object[i].desired_floor;
					} /* if */ 
					break;
				case 2:
					object[i].tile=((object[i].state2>>3)%2)+(1-object[i].state3);
					object[i].state2++;
					if (object[i].state2<=6) {
						object[i].x+=TILE_UNIT*object[i].state3;
						object[i].y-=TILE_UNIT;
					} /* if */ 
					if (object[i].state2>6 && object[i].state2<=10) object[i].x+=TILE_UNIT*object[i].state3;
					if (object[i].state2>10) {
						object[i].x+=TILE_UNIT*object[i].state3;
						object[i].y+=TILE_UNIT;
					} /* if */ 
					if (object[i].state2==16) {
						if (object[i].x>=(room_size_x-6)*TILE_SIZE_X) object[i].state3=-1;
						if (object[i].x<=4*TILE_SIZE_X) object[i].state3=1;
						object[i].state2=0;
						object[i].state=object[i].desired_floor;
					} /* if */ 
					break;
					break;
				case 3:
					object[i].tile=((object[i].state2>>3)%2)+(1-object[i].state3);
					object[i].state2++;
					object[i].y+=2*TILE_UNIT;
					if (object[i].y>=16*TILE_SIZE_Y) {
						object[i].desired_floor=4;
						object[i].state=1;
						object[i].state2=0;
					} /* if */ 
					break;
				case 4:
					object[i].tile=((object[i].state2>>3)%2)+(1-object[i].state3);
					object[i].state2++;
					object[i].y-=2*TILE_UNIT;
					if (object[i].y<4*TILE_SIZE_Y) {
						object[i].desired_floor=5;
						object[i].state=2;
						object[i].state2=0;
					} /* if */ 
					break;
				case 5:
					object[i].tile=((object[i].state2>>3)%2)+(1-object[i].state3);
					object[i].state2++;
					object[i].y+=2*TILE_UNIT;
					if (object[i].y>=12*TILE_SIZE_Y) {
						object[i].desired_floor=6;
						object[i].state=1;
						object[i].state2=0;
					} /* if */ 
					break;
				case 6:
					object[i].tile=((object[i].state2>>3)%2)+(1-object[i].state3);
					object[i].state2++;
					object[i].y-=2*TILE_UNIT;
					if (object[i].y<12*TILE_SIZE_Y) {
						object[i].desired_floor=7;
						object[i].state=2;
						object[i].state2=0;
					} /* if */ 
					break;
				case 7:
					object[i].tile=((object[i].state2>>3)%2)+(1-object[i].state3);
					object[i].state2++;
					object[i].y+=2*TILE_UNIT;
					if (object[i].y>=16*TILE_SIZE_Y) {
						object[i].desired_floor=8;
						object[i].state=1;
						object[i].state2=0;
					} /* if */ 
					break;
				case 8:
					object[i].tile=((object[i].state2>>3)%2)+(1-object[i].state3);
					object[i].state2++;
					object[i].y-=2*TILE_UNIT;
					if (object[i].y<2*TILE_SIZE_Y) {
						object[i].desired_floor=3;
						object[i].state=2;
						object[i].state2=0;
					} /* if */ 
					break;
				} /* siwtch */ 

				break;

			case T_STONEMAN:
				switch(object[i].state) {
				case 0:
					object[i].tile=0;
					if ((rand()%200)==0) {
						object[i].state=1;
						object[i].state2=0;
					} /* if */ 
					break;
				case 1:
					object[i].tile=(object[i].state2>>5)+1;
					object[i].state2++;
					if (object[i].state2>=64) {
						object[i].state=2;
						object[i].state2=0;
						object[i].state3=0;
					} /* if */ 
					break;
				case 2:
					object[i].state2++;
					object[i].tile=((object[i].state2>>3)%2)+3;
					if (
						(tiles[stoneman_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x+1*TILE_UNIT,GAME_VIEW_Y+object[i].y,
							 col_buffer,dx,dy,dx)&colision)==0 &&
						(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0 &&
						(object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X) {
						object[i].x+=1*TILE_UNIT;
						if ((rand()%200)==0) {
							if (object[i].state3==1) {
								object[i].state=4;
								object[i].state2=0;
							} /* if */ 
							if (object[i].state3==0) {
								object[i].state3=rand()%2;
								object[i].state=5;
								object[i].state2=0;
							} /* if */ 
						} /* if */ 
					} else {
						object[i].state=3;
					} /* if */ 
					break;
				case 3:
					object[i].state2++;
					object[i].tile=((object[i].state2>>3)%2)+5; 
					if (
						(tiles[stoneman_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x-1*TILE_UNIT,GAME_VIEW_Y+object[i].y,
							 col_buffer,dx,dy,dx)&colision)==0 &&
						(col_buffer[(GAME_VIEW_X+object[i].x-1)+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0 &&
						 object[i].x>0) {
						object[i].x-=1*TILE_UNIT;
						if ((rand()%200)==0) {
							if (object[i].state3==1) {
								object[i].state=4;
								object[i].state2=0;
							} /* if */ 
							if (object[i].state3==0) {
								object[i].state3=rand()%2;
								object[i].state=6;
								object[i].state2=0;
							} /* if */ 
						} /* if */ 					
					} else {
						object[i].state=2; 
					} /* if */ 
					break;
				case 4:
					object[i].tile=2-(object[i].state2>>5);
					object[i].state2++;
					if (object[i].state2>=64) {
						object[i].state=0;
						object[i].state2=0;
						object[i].state3=0;
					} /* if */ 
					break;
				case 5:
					object[i].state2++;
					if (object[i].state2==16) {
						int val;
						Sound_play(S_enemybullet);
						val=add_object(T_ENEMY,object[i].x,object[i].y,0,T_STONEMAN_BULLET,0);
						object[val].base_tile=stoneman_tile;
						object[val].tile=7;

						object[i].state2=0;
						object[i].state=2;
					} /* if */ 
					break;
				case 6:
					object[i].state2++;
					if (object[i].state2==16) {
						int val;
						Sound_play(S_enemybullet);
						val=add_object(T_ENEMY,object[i].x,object[i].y,0,T_STONEMAN_BULLET,1);
						object[val].base_tile=stoneman_tile;
						object[val].tile=7;

						object[i].state2=0;
						object[i].state=3;
					} /* if */ 
					break;
				} /* switch */ 
				break;

			case T_STONEMAN_BULLET:
				if ((tiles[stoneman_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						col_buffer,dx,dy,dx)&colision)!=0) {
					object[i].type=T_NADA;
				} /* if */ 			   

				if (object[i].state==0) object[i].x+=TILE_SIZE_X/2;
								   else object[i].x-=TILE_SIZE_X/2;

				if (object[i].x<(-2*TILE_SIZE_X) ||
					object[i].x>(room_size_x*TILE_SIZE_X)) {
					object[i].type=T_NADA;
				} /* if */ 
				break;

			case T_PACMAN:
				{
					int bt=0;

					bt=(6-object[i].size)*2;
					if (bt>=6) bt=6;

					switch(object[i].state) { 
					case 0:
						object[i].state2++;
						object[i].tile=((object[i].state2>>2)%2)+bt;
						if (!iswall_or_blockingobject(((object[i].x)/TILE_SIZE_X)+2,
													  (object[i].y/TILE_SIZE_Y)) &&
							iswall_or_blockingobject(((object[i].x)/TILE_SIZE_X)+2,
													  (object[i].y/TILE_SIZE_Y)+2) &&
							(object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X) {
							object[i].x+=TILE_UNIT;
						} else {
							object[i].state=1;
						} /* if */ 
						break;
					case 1:
						object[i].state2++;
						object[i].tile=((object[i].state2>>2)%2)+bt+8; 
						if (
							!iswall_or_blockingobject(((object[i].x)/TILE_SIZE_X),
													  (object[i].y/TILE_SIZE_Y)) &&
							iswall_or_blockingobject(((object[i].x)/TILE_SIZE_X),
													  (object[i].y/TILE_SIZE_Y)+2) &&
/*							(tiles[pacman_tile+object[i].tile]->
							coltest(GAME_VIEW_X+object[i].x-1*TILE_UNIT,GAME_VIEW_Y+object[i].y,
								 col_buffer,dx,dy,dx)&colision)==0 &&
							(col_buffer[(GAME_VIEW_X+object[i].x-1)+
										(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0 &&*/
							 object[i].x>0) {
							object[i].x-=TILE_UNIT;
						} else {
							object[i].state=0; 
						} /* if */ 
						break;
					} /* switch */ 
				}
				break;

			case T_REDDRAGON:
				switch(object[i].state) {
				case 0:
					{
						int dx=(2+(rand()%24))*TILE_SIZE_X;
						int dy=(3+(rand()%9))*TILE_SIZE_Y;
						object[i].x=dx;
						object[i].y=dy;
						object[i].state=1;
						object[i].state2=0;
					}
					break;
				case 1:
						object[i].state2++;
						object[i].tile=((object[i].state2>>1)%2)-1;
						if (object[i].state2>=32) {
							object[i].state=2;
							object[i].union_x=pers_x-object[i].x;
							object[i].union_y=pers_y-object[i].y;
						} /* if */ 
					break;
				case 2:	{
							int j;
							int tx=object[i].union_x,ty=object[i].union_y;
							object[i].state2++;
							if (object[i].union_x>0) {
								object[i].tile=((object[i].state2>>2)%2)+2;
							} else {
								object[i].tile=((object[i].state2>>2)%2);
							} /* if */ 

							for(j=0;j<2 && (abs(object[i].union_x)>TILE_UNIT ||
											abs(object[i].union_y)>TILE_UNIT);j++) {
								if (abs(tx)>abs(ty)) {
									int tmp=(tx>0 ? TILE_UNIT:-TILE_UNIT);
									object[i].x+=tmp;
									object[i].union_x-=tmp;
									object[i].state3-=abs(ty);
									if (object[i].state3<0) {
										int tmp=(ty>0 ? TILE_UNIT:-TILE_UNIT);
										object[i].state3+=abs(tx);
										object[i].y+=tmp;
										object[i].union_y-=tmp;
									} /* if */ 
								} else {
									int tmp=(ty>0 ? TILE_UNIT:-TILE_UNIT);
									object[i].y+=tmp;
									object[i].union_y-=tmp;
									object[i].state3-=abs(tx);
									if (object[i].state3<0) {
										int tmp=(tx>0 ? TILE_UNIT:-TILE_UNIT);
										object[i].state3+=abs(ty);
										object[i].x+=tmp;
										object[i].union_x-=tmp;
									} /* if */ 
								} /* if */ 
							} /* for */ 

						}

						if (abs(object[i].union_x)<=TILE_UNIT &&
							abs(object[i].union_y)<=TILE_UNIT) {
							object[i].state=3;
							object[i].state2=0;
							object[i].state3=object[i].tile;
						} /* if */ 
						break;
				case 3:
						object[i].state2++;
						object[i].tile=((object[i].state2>>2)%2)-1;
						if (object[i].tile!=-1) object[i].tile=object[i].state3;
						if (object[i].state2>=32) {
							object[i].state=4;
							object[i].state2=0;
							object[i].union_x=pers_x;
							object[i].union_y=pers_y;
						} /* if */ 
					break;
				case 4:
					object[i].tile=-1;
					object[i].state2++;
					if (object[i].state2==16) {
						int j;

						Sound_play(S_enemybullet);
						j=add_object(T_ENEMY,object[i].x+TILE_SIZE_X,object[i].y,0,T_REDDRAGON_BULLET,0);
						object[j].state=(pers_x)-object[i].x;
						object[j].state2=(pers_y)-(object[i].y+TILE_SIZE_Y);
						object[j].base_tile=reddragon_tile;
						object[j].tile=4;			
					} /* if */ 
					if (object[i].state2==32) {
						int j;

						Sound_play(S_enemybullet);
						j=add_object(T_ENEMY,object[i].x+TILE_SIZE_X,object[i].y,0,T_REDDRAGON_BULLET,0);
						object[j].state=(pers_x)-object[i].x;
						object[j].state2=(pers_y)-(object[i].y+TILE_SIZE_Y);
						object[j].base_tile=reddragon_tile;
						object[j].tile=4;
					} /* if */ 
					if (object[i].state2==48) object[i].state=0;
					break;
				} /* switch */ 
				break;

			case T_REDDRAGON_BULLET:
				{
					int j;
					for(j=0;j<4;j++) {
						if (abs(object[i].state)>abs(object[i].state2)) {
							object[i].x+=(object[i].state>0 ? TILE_UNIT:-TILE_UNIT);
							object[i].state3-=abs(object[i].state2);
							if (object[i].state3<0) {
								object[i].state3+=abs(object[i].state);
								object[i].y+=(object[i].state2>0 ? TILE_UNIT:-TILE_UNIT);
							} /* if */ 
						} else {
							object[i].y+=(object[i].state2>0 ? TILE_UNIT:-TILE_UNIT);
							object[i].state3-=abs(object[i].state);
							if (object[i].state3<0) {
								object[i].state3+=abs(object[i].state2);
								object[i].x+=(object[i].state>0 ? TILE_UNIT:-TILE_UNIT);
							} /* if */ 
						} /* if */ 
						if (object[i].x<-2*TILE_SIZE_X || object[i].x>=dx ||
							object[i].y<-2*TILE_SIZE_Y || object[i].y>=dy) {
							object[i].type=T_NADA;
						} /* if */ 
					} /* for */ 
				}
				break;

			case T_DEMON6_BULLET:
				object[i].state3++;
				object[i].tile=4+((object[i].state3>>3)%2);
				object[i].union_x+=object[i].state;
				object[i].union_y+=object[i].state2;
				object[i].x=object[i].union_x/TILE_SIZE_X;
				object[i].y=object[i].union_y/TILE_SIZE_Y;
				if (object[i].x<-2*TILE_SIZE_X || object[i].x>=dx ||
					object[i].y<-2*TILE_SIZE_Y || object[i].y>=dy) {
					object[i].type=T_NADA;
				} /* if */ 
				break;

			case T_OWL:
				object[i].state2++;
//				object[i].xoffs=2*TILE_SIZE_X*object[i].state3;
				if (object[i].state!=2) {
					if (object[i].yoffs>0) object[i].yoffs--;
					if (object[i].yoffs<0) object[i].yoffs++;
				} /* if */ 
				switch(object[i].state) {
				case 0: switch(rand()%8) {
						case 0: object[i].union_x=-TILE_UNIT;
								object[i].union_y=0;
								break;
						case 1: object[i].union_x=TILE_UNIT;
								object[i].union_y=0;
								break;
						case 2: object[i].union_x=-TILE_UNIT;
								object[i].union_y=-TILE_UNIT;
								break;
						case 3: object[i].union_x=-TILE_UNIT;
								object[i].union_y=TILE_UNIT;
								break;
						case 4: object[i].union_x=TILE_UNIT;
								object[i].union_y=-TILE_UNIT;
								break;
						case 5: object[i].union_x=TILE_UNIT;
								object[i].union_y=TILE_UNIT;
								break;
						case 6: object[i].union_x=0;
								object[i].union_y=-TILE_UNIT;
								break;
						case 7: object[i].union_x=0;
								object[i].union_y=TILE_UNIT;
								break;
						} /* if */ 
						object[i].state=1;
						{
							int d,dx,dy;

							dx=object[i].x-pers_x;
							dy=object[i].y-pers_y;
							d=int(sqrt(dx*dx+dy*dy));
							if (d<5*TILE_SIZE_X && object[i].state3==1) {
								Sound_play(S_owl);
								object[i].state=2;
								object[i].state3=0;
								object[i].union_x=pers_x-object[i].x;
								object[i].union_y=pers_y-object[i].y;
								break;
							} /* if */ 
							object[i].state3=1;
						}
				case 1: if (object[i].size==9) object[i].tile=((object[i].state2>>3)%2);
										  else object[i].tile=((object[i].state2>>3)%2)+2;
						if (object[i].x<TILE_SIZE_X && object[i].union_x<0) object[i].union_x=TILE_UNIT;
						if (object[i].x>TILE_SIZE_X*29 && object[i].union_x>0) object[i].union_x=-TILE_UNIT;
						if (object[i].y<TILE_SIZE_Y && object[i].union_y<0) object[i].union_y=TILE_UNIT;
						if (object[i].y>TILE_SIZE_Y*13 && object[i].union_y>0) object[i].union_y=-TILE_UNIT;

						object[i].x+=object[i].union_x;
						object[i].y+=object[i].union_y;
						if (object[i].state2==32) {
							object[i].state=0;
							object[i].state2=0;
						} /* if */ 
						break;
				case 2: if (object[i].size==9) object[i].tile=((object[i].state2>>3)%2);
										  else object[i].tile=((object[i].state2>>3)%2)+2;
						object[i].yoffs=int(sin(float(object[i].state2)/4.0)*TILE_SIZE_Y);
						if (object[i].state2>=64) object[i].state=3;
						break;
				case 3: if (object[i].size==9) object[i].tile=((object[i].state2>>3)%2);
										  else object[i].tile=((object[i].state2>>3)%2)+2;
						{
							int j;
							int d,dx,dy;


							for(j=0;j<4;j++) {
								dx=pers_x-object[i].x;
								dy=pers_y-object[i].y;
								if (dx>dy) {
									object[i].x+=(dx>0 ? TILE_UNIT:-TILE_UNIT);
									object[i].state3-=abs(dy);
									if (object[i].state3<0) {
										object[i].state3+=abs(dx);
										object[i].y+=(dy>0 ? TILE_UNIT:-TILE_UNIT);
									} /* if */ 
								} else {
									object[i].y+=(dy>0 ? TILE_UNIT:-TILE_UNIT);
									object[i].state3-=abs(dx);
									if (object[i].state3<0) {
										object[i].state3+=abs(dy);
										object[i].x+=(dx>0 ? TILE_UNIT:-TILE_UNIT);
									} /* if */ 
								} /* if */ 
							} /* for */ 

							d=int(sqrt(dx*dx+dy*dy));
							if (d<3*TILE_SIZE_X) {
								object[i].state=0;
								object[i].state2=0;
								object[i].state3=0;
							} /* if */ 
						}
						break;
				} /* if */ 
				break;

			case T_GREENMONKEY:
				object[i].state3++;
				switch(object[i].state) {
				case 0:
					object[i].tile=((object[i].state3>>3)&0x01)+2;
					if ((tiles[greenmonkey_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
						 object[i].x>0 &&
						(col_buffer[GAME_VIEW_X+object[i].x-1+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x-=TILE_UNIT*4;

						if ((rand()%32)==0) { 
							/* Saltar: */ 
							int tx,ty;
							int nfloors=0,floors[8];
							tx=object[i].x/TILE_SIZE_X;
							ty=object[i].y/TILE_SIZE_Y;
							if (!iswall(tx-3,ty-3) &&
								!iswall(tx-4,ty-3) &&
								!iswall(tx-5,ty-3) &&
								!iswall(tx-6,ty-3) &&
								iswall(tx-3,ty-2) &&
								iswall(tx-4,ty-2) &&
								iswall(tx-5,ty-2) &&
								iswall(tx-6,ty-2) &&
								!iswall(tx-2,ty-2) &&
								(tiles[greenmonkey_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x-4*TILE_SIZE_X,GAME_VIEW_Y+object[i].y-4*TILE_SIZE_Y,
										TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
								) {
								floors[nfloors++]=ty-4;
							} /* if */ 
							if (!iswall(tx-3,ty+5) &&
								!iswall(tx-4,ty+5) &&
								!iswall(tx-5,ty+5) &&
								!iswall(tx-6,ty+5) &&
								iswall(tx-3,ty+6) &&
								iswall(tx-4,ty+6) &&
								iswall(tx-5,ty+6) &&
								iswall(tx-6,ty+6) &&
								!iswall(tx-2,ty+2) &&
								(tiles[greenmonkey_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x-(4*TILE_SIZE_X+4*TILE_UNIT),GAME_VIEW_Y+object[i].y+4*TILE_SIZE_Y,
										TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
								) {
								floors[nfloors++]=ty+4;
							} /* if */ 
							if (nfloors!=0) {
								object[i].desired_floor=floors[rand()%nfloors]*TILE_SIZE_Y;
								object[i].state=7;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 
					} else {
						object[i].state=1;
					} /* if */ 
					break;
				case 1: 
					object[i].tile=((object[i].state3>>3)&0x01);
					if ((tiles[greenmonkey_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
						 (object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
						(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x+=TILE_UNIT*4;

						if ((rand()%32)==0) { 
							/* Saltar: */ 
							int tx,ty;
							int nfloors=0,floors[8];
							tx=object[i].x/TILE_SIZE_X;
							ty=object[i].y/TILE_SIZE_Y;
							if (!iswall(tx+4,ty-3) &&
								!iswall(tx+5,ty-3) &&
								!iswall(tx+6,ty-3) &&
								!iswall(tx+7,ty-3) &&
								iswall(tx+4,ty-2) &&
								iswall(tx+5,ty-2) &&
								iswall(tx+6,ty-2) &&
								iswall(tx+7,ty-2) &&
								!iswall(tx+3,ty-2) &&
								(tiles[greenmonkey_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x+5*TILE_SIZE_X,GAME_VIEW_Y+object[i].y-4*TILE_SIZE_Y,
										TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
								) {
								floors[nfloors++]=ty-4;
							} /* if */ 
							if (!iswall(tx+4,ty+5) &&
								!iswall(tx+5,ty+5) &&
								!iswall(tx+6,ty+5) &&
								!iswall(tx+7,ty+5) &&
								iswall(tx+4,ty+6) &&
								iswall(tx+5,ty+6) &&
								iswall(tx+6,ty+6) &&
								iswall(tx+7,ty+6) &&
								!iswall(tx+3,ty+2) &&
								(tiles[greenmonkey_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x+5*TILE_SIZE_X,GAME_VIEW_Y+object[i].y+4*TILE_SIZE_Y,
										TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
								) {
								floors[nfloors++]=ty+4;
							} /* if */ 
							if (nfloors!=0) {
								object[i].desired_floor=floors[rand()%nfloors]*TILE_SIZE_Y;
								object[i].state=6;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 
					} else {
						object[i].state=0;
					} /* if */ 
					break;
				/* Estados de salto: */ 
				case 6: object[i].tile=4;
						object[i].x+=2*TILE_UNIT;
						if (object[i].desired_floor<object[i].y) {
							object[i].state3=0;
							object[i].y-=4*TILE_UNIT;
						} else {
							if (object[i].state3<2) object[i].y-=TILE_UNIT*2;
							if (object[i].state3>=2 && object[i].state3<4) object[i].y-=TILE_UNIT;
							if (object[i].state3>=8) {
								object[i].state=8;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 	
						break;
				case 7:	object[i].tile=5;
						object[i].x-=2*TILE_UNIT;
						if (object[i].desired_floor<object[i].y) {
							object[i].state3=0;
							object[i].y-=4*TILE_UNIT;
						} else {
							if (object[i].state3<2) object[i].y-=TILE_UNIT*2;
							if (object[i].state3>=2 && object[i].state3<4) object[i].y-=TILE_UNIT;
							if (object[i].state3>=8) {
								object[i].state=9;
								object[i].state3=0;
							} /* if */ 
						} /* if */ 
						break;
				case 8: object[i].tile=4;
						object[i].x+=2*TILE_UNIT;
						if (object[i].state3>=2) object[i].y+=TILE_UNIT;
						if (object[i].state3>=4) object[i].y+=TILE_UNIT;
						if (object[i].state3>=6) object[i].y+=TILE_UNIT*2;
						if (object[i].state3>=10) object[i].y+=TILE_UNIT*2;
						if (object[i].y>=object[i].desired_floor) {
							object[i].y=object[i].desired_floor;
							object[i].state=1;
							object[i].state3=0;
						} /* if */ 
						break;
				case 9: object[i].tile=5;
						object[i].x-=2*TILE_UNIT;
						if (object[i].state3>=2) object[i].y+=TILE_UNIT;
						if (object[i].state3>=4) object[i].y+=TILE_UNIT;
						if (object[i].state3>=6) object[i].y+=TILE_UNIT*2;
						if (object[i].state3>=10) object[i].y+=TILE_UNIT*2;
						if (object[i].y>=object[i].desired_floor) {
							object[i].y=object[i].desired_floor;
							object[i].state=0;
							object[i].state3=0;
						} /* if */ 
						break;
				} /* switch */  
				break;

			case T_PLANT:
				if ((object[i].state2&0x0f)==0) {
					switch(rand()%8) {
						case 0: object[i].union_x=-TILE_UNIT;
								object[i].union_y=0;
								break;
						case 1: object[i].union_x=TILE_UNIT;
								object[i].union_y=0;
								break;
						case 2: object[i].union_x=-TILE_UNIT;
								object[i].union_y=-TILE_UNIT;
								break;
						case 3: object[i].union_x=-TILE_UNIT;
								object[i].union_y=TILE_UNIT;
								break;
						case 4: object[i].union_x=TILE_UNIT;
								object[i].union_y=-TILE_UNIT;
								break;
						case 5: object[i].union_x=TILE_UNIT;
								object[i].union_y=TILE_UNIT;
								break;
						case 6: object[i].union_x=0;
								object[i].union_y=-TILE_UNIT;
								break;
						case 7: object[i].union_x=0;
								object[i].union_y=TILE_UNIT;
								break;
					} /* switch */ 
				} /* if */ 
				if (object[i].x<TILE_SIZE_X && object[i].union_x<0) object[i].union_x=TILE_UNIT;
				if (object[i].x>TILE_SIZE_X*29 && object[i].union_x>0) object[i].union_x=-TILE_UNIT;
				if (object[i].y<TILE_SIZE_Y && object[i].union_y<0) object[i].union_y=TILE_UNIT;
				if (object[i].y>TILE_SIZE_Y*13 && object[i].union_y>0) object[i].union_y=-TILE_UNIT;

				object[i].x+=object[i].union_x;
				object[i].y+=object[i].union_y;
				switch(object[i].state) {
				case 0: object[i].tile=0;
					if ((object[i].state2&0x7f)==0) {
						object[i].state=1;
						Sound_play(S_enemybullet);
					} /* if */ 
					break;
				case 1:	object[i].tile=1;
					if ((object[i].state2&0x03)==0) {
						int j;

						/* Disparar: */ 
						j=add_object(T_ENEMY,object[i].x+TILE_SIZE_X,object[i].y,0,T_PLANT_BULLET,0);
						object[j].state=(rand()%15)-8;
						object[j].state2=-TILE_UNIT*6;
						object[j].base_tile=plant_tile;
						object[j].tile=2;
					} /* if */ 
					if ((object[i].state2&0x7f)==0) object[i].state=0;
					break;
				} /* switch */ 
				object[i].state2++;
				break;

			case T_PLANT_BULLET:
				object[i].x+=object[i].state;
				object[i].y+=object[i].state2;
				if (object[i].state2<8*TILE_UNIT) object[i].state2++;
				if (object[i].x<-2*TILE_SIZE_X || object[i].x>=dx ||
					object[i].y>=dy) {
					object[i].type=T_NADA;
				} /* if */ 
				break;

			case T_DEMON7_BULLET:
				switch(object[i].tile) {
				case 8:
					object[i].x+=TILE_UNIT*2;
					break;
				case 9:
					object[i].x-=TILE_UNIT*2;
					break;
				case 10:
					object[i].y-=TILE_UNIT*2;
					break;
				case 11:
					object[i].y+=TILE_UNIT*2;
					break;
				case 12:
					object[i].x-=TILE_UNIT;
					object[i].y-=TILE_UNIT;
					break;
				case 13:
					object[i].x+=TILE_UNIT;
					object[i].y-=TILE_UNIT;
					break;
				case 14:
					object[i].x+=TILE_UNIT;
					object[i].y+=TILE_UNIT;
					break;
				case 15:
					object[i].x-=TILE_UNIT;
					object[i].y+=TILE_UNIT;
					break;
				} /* switch */ 
				if (object[i].x<-2*TILE_SIZE_X || object[i].x>=dx ||
					object[i].y<-2*TILE_SIZE_Y || object[i].y>=dy) {
					object[i].type=T_NADA;
				} /* if */ 
				break;

			case T_WITCH2:
				{
					bool col[4];
					object[i].state2++;
					object[i].tile=(object[i].tile&0x02)+((object[i].state2>>3)%2);

					colision=T_WALL|T_LADDER_WALL|T_DOOR_WALL;

					if ((object[i].x%TILE_SIZE_X)==0 &&
						(object[i].y%TILE_SIZE_Y)==0) {
						int tx,ty;
						tx=object[i].x/TILE_SIZE_X;
						ty=object[i].y/TILE_SIZE_Y;

						col[0]=iswall(tx+2,ty)|iswall(tx+2,ty+1);
						col[1]=iswall(tx-1,ty)|iswall(tx-1,ty+1);
						col[2]=iswall(tx,ty+2)|iswall(tx+1,ty+2);
						col[3]=iswall(tx,ty-1)|iswall(tx+1,ty-1);

						if ((tiles[witch2_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x+1*TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
										    col_buffer,dx,dy,dx)&colision)!=0) col[0]=true;
						if ((tiles[witch2_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x-1*TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
										    col_buffer,dx,dy,dx)&colision)!=0) col[1]=true;
						if ((tiles[witch2_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y,
										    col_buffer,dx,dy,dx)&colision)!=0) col[2]=true;
						if ((tiles[witch2_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y-TILE_SIZE_Y,
										    col_buffer,dx,dy,dx)&colision)!=0) col[3]=true;

//						col[4]=iswall(tx+2,ty+2);
//						col[5]=iswall(tx-1,ty+2);
//						col[6]=iswall(tx+2,ty-1);
//						col[7]=iswall(tx-1,ty-1);

						switch(object[i].state3) {
						case 0:if (!col[3]) {
								object[i].state3=2;
							   } else {
								   if (col[0]) {
									   object[i].state3=6;
								   } /* if */ 
							   } /* if */ 
							   break;
						case 1:if (!col[2]) {
								object[i].state3=7;
							   } else {
								   if (col[0]) {
									   object[i].state3=3;
								   } /* if */ 
							   } /* if */ 
							   break;
						case 2:if (!col[1]) {
								object[i].state3=4;
							   } else {
								   if (col[3]) {
									   object[i].state3=0;
								   } /* if */ 
							   } /* if */ 
							   break;
						case 3:if (!col[0]) {
								object[i].state3=1;
							   } else {
								   if (col[3]) {
									   object[i].state3=5;
								   } /* if */ 
							   } /* if */ 
							   break;
						case 4:if (!col[2]) {
								object[i].state3=6;
							   } else {
								   if (col[1]) {
									   object[i].state3=2;
								   } /* if */ 
							   } /* if */ 
							   break;
						case 5:if (!col[3]) {
								object[i].state3=3;
							   } else {
								   if (col[1]) {
									   object[i].state3=7;
								   } /* if */ 
							   } /* if */ 
							   break;
						case 6:if (!col[0]) {
								object[i].state3=0;
							   } else {
								   if (col[2]) {
									   object[i].state3=4;
								   } /* if */ 
							   } /* if */ 
							   break;
						case 7:if (!col[1]) {
								object[i].state3=5;
							   } else {
								   if (col[2]) {
									   object[i].state3=1;
								   } /* if */ 
							   } /* if */ 
							   break;
						} /* switch */ 

					} /* if */ 

					switch(object[i].state3) {
					case 0:
					case 1:object[i].x+=TILE_UNIT;
						   object[i].tile=object[i].tile&0x01;
						break;
					case 4:
					case 5:object[i].x-=TILE_UNIT;
						   object[i].tile=object[i].tile|0x02;
						break;
					case 6:
					case 7:object[i].y+=TILE_UNIT;
						break;
					case 2:
					case 3:object[i].y-=TILE_UNIT;
						break;
					} /* switch */  


				}
				break;

			case T_FLAME:
				object[i].state2++;
				switch(object[i].state) {
				case 0:if (object[i].state2<32) object[i].tile=0;
					   if (object[i].state2>=32 && object[i].state2<40) object[i].tile=1;
					   if (object[i].state2>=40 && object[i].state2<48) object[i].tile=2;
					   if (object[i].state2>=48 && object[i].state2<56) object[i].tile=3;
					   if (object[i].state2>=56) {
						   object[i].state=1;
						   object[i].state2=0;
					   } /* if */ 
					break;
				case 1:
					object[i].tile=6+((object[i].state2>>3)%2);
					if ((tiles[flame_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
						 (object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&				 
						(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x+=TILE_UNIT;

						if ((rand()%200)==0) {
							int j;

							/* Disparar: */ 
							Sound_play(S_flamebullet);
							j=add_object(T_ENEMY,object[i].x,object[i].y,0,T_FLAME_BULLET,0);
							object[j].state=(pers_x)-object[i].x;
							object[j].state2=(pers_y)-(object[i].y);
							object[j].base_tile=flame_tile;
							object[j].tile=8;
						} /* if */ 
						if (object[i].state2>=256) {
							object[i].state=3;
							object[i].state2=0;
						} /* if */ 
					} else {
						object[i].state=2;
					} /* if */ 
					break;
				case 2: 
					object[i].tile=4+((object[i].state2>>3)%2);
					if ((tiles[flame_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
						 object[i].x>0 && 
						(col_buffer[GAME_VIEW_X+object[i].x-1+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x-=TILE_UNIT;

						if ((rand()%200)==0) {
							int j;

							/* Disparar: */ 
//							Sound_play(S_flamebullet);
							j=add_object(T_ENEMY,object[i].x,object[i].y,0,T_FLAME_BULLET,0);
							object[j].state=(pers_x)-object[i].x;
							object[j].state2=(pers_y)-(object[i].y);
							object[j].base_tile=flame_tile;
							object[j].tile=8;
						} /* if */ 
						if (object[i].state2>=256) {
							object[i].state=3;
							object[i].state2=0;
						} /* if */ 
					} else {
						object[i].state=1;
					} /* if */ 
					break;
				case 3:if (object[i].state2<32) object[i].tile=3;
					   if (object[i].state2>=32 && object[i].state2<40) object[i].tile=2;
					   if (object[i].state2>=40 && object[i].state2<48) object[i].tile=1;
					   if (object[i].state2>=48 && object[i].state2<56) object[i].tile=0;
					   if (object[i].state2>=56) {
						   object[i].state=4;
						   object[i].state2=0;
						   object[i].state3=rand()%12;
					   } /* if */ 
					break;
				case 4:
					object[i].tile=((object[i].state2>>2)%2);

					switch(object[i].state3) {
					case 0:object[i].y+=TILE_UNIT*2;
						break;
					case 1:object[i].y+=TILE_UNIT*2;
						   object[i].x-=TILE_UNIT*2;
						break;
					case 2:object[i].y-=TILE_UNIT*2;
						   object[i].x-=TILE_UNIT*2;
						break;
					case 3:object[i].y-=TILE_UNIT*2;
						break;
					case 4:object[i].y-=TILE_UNIT*2;
						   object[i].x+=TILE_UNIT*2;
						break;
					case 5:object[i].y+=TILE_UNIT*2;
						   object[i].x+=TILE_UNIT*2;
						break;
					case 6:object[i].x+=TILE_UNIT*2;
						break;
					case 7:object[i].x-=TILE_UNIT*2;
						break;

					case 8:object[i].y+=TILE_UNIT*2;
						   object[i].x-=TILE_UNIT;
						break;
					case 9:object[i].y-=TILE_UNIT*2;
						   object[i].x-=TILE_UNIT;
						break;
					case 10:object[i].y-=TILE_UNIT*2;
						   object[i].x+=TILE_UNIT;
						break;
					case 11:object[i].y+=TILE_UNIT*2;
						   object[i].x+=TILE_UNIT;
						break;

					} /* switch */ 
					object[i].state2++;
					if ((object[i].state2>=16 && object[i].state3!=-1) ||
						(object[i].state2>=32 && object[i].state3==-1) ||
						object[i].y==(room_size_y-2)*TILE_SIZE_Y) {
						t_x=object[i].x/TILE_SIZE_X;
						t_y=object[i].y/TILE_SIZE_Y;
						if (iswall(t_x,t_y+2) &&
							iswall(t_x+1,t_y+2) &&
							!iswall(t_x,t_y+1) &&
							!iswall(t_x+1,t_y+1) &&
							(rand()%4)==0) {
							object[i].state=0;
							object[i].state2=0;
						} else {
							object[i].state=4;
							object[i].state2=0;

							/* Si el número aleatorio es mayor que 7, se sigue en la	*/ 
							/* misma dirección.											*/ 
							j=object[i].state3;
							if (j==-1) object[i].state3=rand()%12;
								  else object[i].state3=rand()%20;
							if (object[i].state3>11) object[i].state3=j;

							if (object[i].x>(room_size_x-7)*TILE_SIZE_X) {
								if (object[i].state3==4) object[i].state3=2;
								if (object[i].state3==5) object[i].state3=1;
								if (object[i].state3==6) object[i].state3=7;
								if (object[i].state3==10) object[i].state3=9;
								if (object[i].state3==11) object[i].state3=8;
							} /* if */ 
							if (object[i].x<TILE_SIZE_X*5) {
								if (object[i].state3==2) object[i].state3=4;
								if (object[i].state3==1) object[i].state3=5;
								if (object[i].state3==7) object[i].state3=6;
								if (object[i].state3==9) object[i].state3=10;
								if (object[i].state3==8) object[i].state3=11;
							} /* if */ 
							if (object[i].y>(room_size_y-4)*TILE_SIZE_Y) {
								if (object[i].state3==0) object[i].state3=3;
								if (object[i].state3==1) object[i].state3=2;
								if (object[i].state3==5) object[i].state3=4;
								if (object[i].state3==11) object[i].state3=10;
								if (object[i].state3==8) object[i].state3=9;
							} /* if */ 
							if (object[i].y<TILE_SIZE_Y*5) {
								if (object[i].state3==3) object[i].state3=0;
								if (object[i].state3==2) object[i].state3=1;
								if (object[i].state3==4) object[i].state3=5;
								if (object[i].state3==10) object[i].state3=11;
								if (object[i].state3==9) object[i].state3=8;
							} /* if */ 
						} /* if */ 
					} /* if */ 
					break;
 

				} /* switch */ 
				break;

			case T_FLAME_BULLET:
			case T_TRANSF_BULLET:
				{
					int j;
					for(j=0;j<2;j++) {
						if (abs(object[i].state)>abs(object[i].state2)) {
							object[i].x+=(object[i].state>0 ? TILE_UNIT:-TILE_UNIT);
							object[i].state3-=abs(object[i].state2);
							if (object[i].state3<0) {
								object[i].state3+=abs(object[i].state);
								object[i].y+=(object[i].state2>0 ? TILE_UNIT:-TILE_UNIT);
							} /* if */ 
						} else {
							object[i].y+=(object[i].state2>0 ? TILE_UNIT:-TILE_UNIT);
							object[i].state3-=abs(object[i].state);
							if (object[i].state3<0) {
								object[i].state3+=abs(object[i].state2);
								object[i].x+=(object[i].state>0 ? TILE_UNIT:-TILE_UNIT);
							} /* if */ 
						} /* if */ 
						if (object[i].x<-2*TILE_SIZE_X || object[i].x>=dx ||
							object[i].y<-2*TILE_SIZE_Y || object[i].y>=dy) {
							object[i].type=T_NADA;
						} /* if */ 
					} /* for */ 
				}
				break;

			case T_TRANSFORMER:
				object[i].state2++;
				switch(object[i].state) {
				case 0:if (object[i].state2<32) object[i].tile=0;
					   if (object[i].state2>=32 && object[i].state2<48) object[i].tile=1;
					   if (object[i].state2>=48 && object[i].state2<64) object[i].tile=2;
					   if (object[i].state2>=64) {
						   object[i].state=1;
						   object[i].state2=0;
					   } /* if */ 
					break;
				case 1:
					object[i].tile=5+((object[i].state2>>3)%2);
					if ((tiles[transformer_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
						 (object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&				 
						(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x+=TILE_UNIT;

						if ((rand()%200)==0) {
							int j;

							/* Disparar: */ 
							Sound_play(S_enemybullet);
							j=add_object(T_ENEMY,object[i].x,object[i].y,0,T_TRANSF_BULLET,0);
							object[j].state=(pers_x)-object[i].x;
							object[j].state2=(pers_y)-(object[i].y);
							object[j].base_tile=transformer_tile;
							object[j].tile=10;
						} /* if */ 
						if (object[i].state2>=256) {
							object[i].state=3;
							object[i].state2=0;
						} /* if */ 
					} else {
						object[i].state=2;
					} /* if */ 
					break;
				case 2: 
					object[i].tile=7+((object[i].state2>>3)%2);
					if ((tiles[transformer_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
						 object[i].x>0 && 
						(col_buffer[GAME_VIEW_X+object[i].x-1+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x-=TILE_UNIT;

						if ((rand()%200)==0) {
							int j;

							/* Disparar: */ 
							Sound_play(S_enemybullet);
							j=add_object(T_ENEMY,object[i].x,object[i].y,0,T_TRANSF_BULLET,0);
							object[j].state=(pers_x)-object[i].x;
							object[j].state2=(pers_y)-(object[i].y);
							object[j].base_tile=transformer_tile;
							object[j].tile=10;
						} /* if */ 
						if (object[i].state2>=256) {
							object[i].state=3;
							object[i].state2=0;
						} /* if */ 
					} else {
						object[i].state=1;
					} /* if */ 
					break;
				case 3:if (object[i].state2<16) object[i].tile=9;
					   if (object[i].state2>=16 && object[i].state2<32) object[i].tile=1;
					   if (object[i].state2>=32 && object[i].state2<48) object[i].tile=0;
					   if (object[i].state2>=48 && object[i].state2<64) object[i].tile=2;
					   if (object[i].state2>=64) {
						   object[i].state=4;
						   object[i].state2=0;
						   object[i].state3=rand()%12;
					   } /* if */ 
					break;
				case 4:
					object[i].tile=3+((object[i].state2>>3)%2);

					switch(object[i].state3) {
					case 0:object[i].y+=TILE_UNIT*2;
						break;
					case 1:object[i].y+=TILE_UNIT*2;
						   object[i].x-=TILE_UNIT*2;
						break;
					case 2:object[i].y-=TILE_UNIT*2;
						   object[i].x-=TILE_UNIT*2;
						break;
					case 3:object[i].y-=TILE_UNIT*2;
						break;
					case 4:object[i].y-=TILE_UNIT*2;
						   object[i].x+=TILE_UNIT*2;
						break;
					case 5:object[i].y+=TILE_UNIT*2;
						   object[i].x+=TILE_UNIT*2;
						break;
					case 6:object[i].x+=TILE_UNIT*2;
						break;
					case 7:object[i].x-=TILE_UNIT*2;
						break;

					case 8:object[i].y+=TILE_UNIT*2;
						   object[i].x-=TILE_UNIT;
						break;
					case 9:object[i].y-=TILE_UNIT*2;
						   object[i].x-=TILE_UNIT;
						break;
					case 10:object[i].y-=TILE_UNIT*2;
						   object[i].x+=TILE_UNIT;
						break;
					case 11:object[i].y+=TILE_UNIT*2;
						   object[i].x+=TILE_UNIT;
						break;

					} /* switch */ 
					object[i].state2++;
					if ((object[i].state2>=16 && object[i].state3!=-1) ||
						(object[i].state2>=32 && object[i].state3==-1) ||
						object[i].y==(room_size_y-2)*TILE_SIZE_Y) {
						t_x=object[i].x/TILE_SIZE_X;
						t_y=object[i].y/TILE_SIZE_Y;
						if (iswall(t_x,t_y+2) &&
							iswall(t_x+1,t_y+2) &&
							!iswall(t_x,t_y+1) &&
							!iswall(t_x+1,t_y+1) &&
							(rand()%4)==0) {
							object[i].state=0;
							object[i].state2=0;
						} else {
							object[i].state=4;
							object[i].state2=0;

							/* Si el número aleatorio es mayor que 7, se sigue en la	*/ 
							/* misma dirección.											*/ 
							j=object[i].state3;
							if (j==-1) object[i].state3=rand()%12;
								  else object[i].state3=rand()%20;
							if (object[i].state3>11) object[i].state3=j;

							if (object[i].x>(room_size_x-7)*TILE_SIZE_X) {
								if (object[i].state3==4) object[i].state3=2;
								if (object[i].state3==5) object[i].state3=1;
								if (object[i].state3==6) object[i].state3=7;
								if (object[i].state3==10) object[i].state3=9;
								if (object[i].state3==11) object[i].state3=8;
							} /* if */ 
							if (object[i].x<TILE_SIZE_X*5) {
								if (object[i].state3==2) object[i].state3=4;
								if (object[i].state3==1) object[i].state3=5;
								if (object[i].state3==7) object[i].state3=6;
								if (object[i].state3==9) object[i].state3=10;
								if (object[i].state3==8) object[i].state3=11;
							} /* if */ 
							if (object[i].y>(room_size_y-4)*TILE_SIZE_Y) {
								if (object[i].state3==0) object[i].state3=3;
								if (object[i].state3==1) object[i].state3=2;
								if (object[i].state3==5) object[i].state3=4;
								if (object[i].state3==11) object[i].state3=10;
								if (object[i].state3==8) object[i].state3=9;
							} /* if */ 
							if (object[i].y<TILE_SIZE_Y*5) {
								if (object[i].state3==3) object[i].state3=0;
								if (object[i].state3==2) object[i].state3=1;
								if (object[i].state3==4) object[i].state3=5;
								if (object[i].state3==10) object[i].state3=11;
								if (object[i].state3==9) object[i].state3=8;
							} /* if */ 
						} /* if */ 
					} /* if */ 
					break;
 

				} /* switch */ 
				break;

			case T_CYCLOPS:
				object[i].state2++;
				object[i].tile=((object[i].state2>>3)%2);

				switch(object[i].state3) {
				case 0:object[i].y+=TILE_UNIT/2;
					break;
				case 1:object[i].y+=TILE_UNIT/2;
					   object[i].x-=TILE_UNIT/2;
					break;
				case 2:object[i].y-=TILE_UNIT/2;
					   object[i].x-=TILE_UNIT/2;
					break;
				case 3:object[i].y-=TILE_UNIT/2;
					break;
				case 4:object[i].y-=TILE_UNIT/2;
					   object[i].x+=TILE_UNIT/2;
					break;
				case 5:object[i].y+=TILE_UNIT/2;
					   object[i].x+=TILE_UNIT/2;
					break;
				case 6:object[i].x+=TILE_UNIT/2;
					break;
				case 7:object[i].x-=TILE_UNIT/2;
					break;

				} /* switch */ 
				if (object[i].state2>=32 ||
					object[i].y==(room_size_y-2)*TILE_SIZE_Y) {
					if (object[i].state3==-1) {
						int j;
						/* Dispara Rayo: */ 
						j=add_object(T_ENEMY,object[i].x+TILE_SIZE_X,object[i].y+TILE_SIZE_Y,0,T_CYCLOPS_BULLET,0);
						object[j].base_tile=cyclops_tile;
						dx=pers_x-(object[i].x+1*TILE_SIZE_X);
						dy=pers_y-(object[i].y+1*TILE_SIZE_Y);
						if (abs(dx)>(abs(dy)*2)) {
							object[j].tile=5;
							if (dx>0) {
								object[j].state=8;
							} else {
								object[j].state=9;
							} /* if */ 
						} else {
							if (abs(dy)>(abs(dx)*2)) {
								object[j].tile=4;
								if (dy>0) {
									object[j].state=11;
								} else {
									object[j].state=10;
								} /* if */ 
							} else {
								if (dx<0) {
									if (dy<0) {
										object[j].tile=2;
										object[j].state=12;
									} else {
										object[j].tile=3;
										object[j].state=15;
									} /* if */ 
								} else {
									if (dy<0) {
										object[j].tile=3;
										object[j].state=13;
									} else {
										object[j].tile=2;
										object[j].state=14;
									} /* if */ 
								} /* if */ 
							} /* if */ 
						} /* if */ 
					} /* if */ 
					if (object[i].state3!=-1 && (rand()%80)<=20) {
						object[i].state3=-1;
						object[i].state2=0;
					} else {
						object[i].state2=0;

						/* Si el número aleatorio es mayor que 7, va en	*/ 
						/* la dirección del personaje.					*/ 
						object[i].state3=rand()%15;
						if (object[i].state3>7) {
							int dx,dy;

							dx=pers_x-(object[i].x+1*TILE_SIZE_X);
							dy=pers_y-(object[i].y+1*TILE_SIZE_Y);
							if (abs(dx)>(abs(dy)*2)) {
								if (dx>0) {
									object[i].state3=6;
								} else {
									object[i].state3=7;
								} /* if */ 
							} else {
								if (abs(dy)>(abs(dx)*2)) {
									if (dy>0) {
										object[i].state3=0;
									} else {
										object[i].state3=3;
									} /* if */ 
								} else {
									if (dx<0) {
										if (dy<0) {
											object[i].state3=2;
										} else {
											object[i].state3=1;
										} /* if */ 
									} else {
										if (dy<0) {
											object[i].state3=4;
										} else {
											object[i].state3=5;
										} /* if */ 
									} /* if */ 
								} /* if */ 
							} /* if */ 
						} /* if */ 

						if (object[i].x>(room_size_x-7)*TILE_SIZE_X) {
							if (object[i].state3==4) object[i].state3=2;
							if (object[i].state3==5) object[i].state3=1;
							if (object[i].state3==6) object[i].state3=7;
							if (object[i].state3==10) object[i].state3=9;
							if (object[i].state3==11) object[i].state3=8;
						} /* if */ 
						if (object[i].x<TILE_SIZE_X*5) {
							if (object[i].state3==2) object[i].state3=4;
							if (object[i].state3==1) object[i].state3=5;
							if (object[i].state3==7) object[i].state3=6;
							if (object[i].state3==9) object[i].state3=10;
							if (object[i].state3==8) object[i].state3=11;
						} /* if */ 
						if (object[i].y>(room_size_y-4)*TILE_SIZE_Y) {
							if (object[i].state3==0) object[i].state3=3;
							if (object[i].state3==1) object[i].state3=2;
							if (object[i].state3==5) object[i].state3=4;
							if (object[i].state3==11) object[i].state3=10;
							if (object[i].state3==8) object[i].state3=9;
						} /* if */ 
						if (object[i].y<TILE_SIZE_Y*5) {
							if (object[i].state3==3) object[i].state3=0;
							if (object[i].state3==2) object[i].state3=1;
							if (object[i].state3==4) object[i].state3=5;
							if (object[i].state3==10) object[i].state3=11;
							if (object[i].state3==9) object[i].state3=8;
						} /* if */ 
					} /* if */ 
				} /* if */  
				break;

			case T_CYCLOPS_BULLET:
				switch(object[i].state) {
				case 8:
					object[i].x+=TILE_UNIT*2;
					break;
				case 9:
					object[i].x-=TILE_UNIT*2;
					break;
				case 10:
					object[i].y-=TILE_UNIT*2;
					break;
				case 11:
					object[i].y+=TILE_UNIT*2;
					break;
				case 12:
					object[i].x-=TILE_UNIT;
					object[i].y-=TILE_UNIT;
					break;
				case 13:
					object[i].x+=TILE_UNIT;
					object[i].y-=TILE_UNIT;
					break;
				case 14:
					object[i].x+=TILE_UNIT;
					object[i].y+=TILE_UNIT;
					break;
				case 15:
					object[i].x-=TILE_UNIT;
					object[i].y+=TILE_UNIT;
					break;
				} /* switch */ 
				if (object[i].x<-2*TILE_SIZE_X || object[i].x>=32*TILE_SIZE_X ||
					object[i].y<-2*TILE_SIZE_Y || object[i].y>=20*TILE_SIZE_Y) {
					object[i].type=T_NADA;
				} /* if */ 
				break;

			case T_DEMON8_BULLET:
				{
					int step=TILE_UNIT*4;

					colision=T_WALL|T_LADDER_WALL|T_DOOR_WALL;
					object[i].state2++;
					if (object[i].state2>=1024) object[i].type=T_NADA;

					switch(object[i].state) {
					case 0: /* - - */ 
						if ((tiles[object[i].base_tile+object[i].tile]->
								coltest(GAME_VIEW_X+object[i].x-step,GAME_VIEW_Y+object[i].y,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
								object[i].x-step>0) {
							if ((tiles[object[i].base_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x-step,GAME_VIEW_Y+object[i].y-step,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
									object[i].y-step>0) {
								object[i].x-=step;
								object[i].y-=step;
							} else {
								object[i].x-=step;
								object[i].y+=step;
								object[i].state=1;
							} /* if */ 
						} else {
							if ((tiles[object[i].base_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x+step,GAME_VIEW_Y+object[i].y-step,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
									object[i].y-step>0) {
								object[i].x+=step;
								object[i].y-=step;
								object[i].state=2;
							} else {
								object[i].x+=step;
								object[i].y+=step;
								object[i].state=3;
							} /* if */ 
						} /* if */ 
						break;
					case 1: /* - + */ 
						if ((tiles[object[i].base_tile+object[i].tile]->
								coltest(GAME_VIEW_X+object[i].x-step,GAME_VIEW_Y+object[i].y,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
								object[i].x-step>0) {
							if ((tiles[object[i].base_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x-step,GAME_VIEW_Y+object[i].y+step,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
									object[i].y+step<18*TILE_SIZE_X) {
								object[i].x-=step;
								object[i].y+=step;
							} else {
								object[i].x-=step;
								object[i].y-=step;
								object[i].state=0;
							} /* if */ 
						} else {
							if ((tiles[object[i].base_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x+step,GAME_VIEW_Y+object[i].y+step,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
									object[i].y+step<18*TILE_SIZE_X) {
								object[i].x+=step;
								object[i].y+=step;
								object[i].state=3;
							} else {
								object[i].x+=step;
								object[i].y-=step;
								object[i].state=2;
							} /* if */ 
						} /* if */ 
						break;
					case 2: /* + - */ 
						if ((tiles[object[i].base_tile+object[i].tile]->
								coltest(GAME_VIEW_X+object[i].x+step,GAME_VIEW_Y+object[i].y,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
								object[i].x+step<30*TILE_SIZE_X) {
							if ((tiles[object[i].base_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x+step,GAME_VIEW_Y+object[i].y-step,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
									object[i].y-step>0) {
								object[i].x+=step;
								object[i].y-=step;
							} else {
								object[i].x+=step;
								object[i].y+=step;
								object[i].state=3;
							} /* if */ 
						} else {
							if ((tiles[object[i].base_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x-step,GAME_VIEW_Y+object[i].y-step,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
									object[i].y-step>0) {
								object[i].x-=step;
								object[i].y-=step;
								object[i].state=0;
							} else {
								object[i].x-=step;
								object[i].y+=step;
								object[i].state=1;
							} /* if */ 
						} /* if */ 
						break;
					case 3: /* + + */ 
						if ((tiles[object[i].base_tile+object[i].tile]->
								coltest(GAME_VIEW_X+object[i].x+step,GAME_VIEW_Y+object[i].y,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
								object[i].x+step<30*TILE_SIZE_X) {
							if ((tiles[object[i].base_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x+step,GAME_VIEW_Y+object[i].y+step,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
									object[i].y+step<18*TILE_SIZE_X) {
								object[i].x+=step;
								object[i].y+=step;
							} else {
								object[i].x+=step;
								object[i].y-=step;
								object[i].state=2;
							} /* if */ 
						} else {
							if ((tiles[object[i].base_tile+object[i].tile]->
									coltest(GAME_VIEW_X+object[i].x-step,GAME_VIEW_Y+object[i].y+step,
									TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
									object[i].y+step<18*TILE_SIZE_X) {
								object[i].x-=step;
								object[i].y+=step;
								object[i].state=1;
							} else {
								object[i].x-=step;
								object[i].y-=step;
								object[i].state=0;
							} /* if */ 
						} /* if */ 
						break;
					} /* switch */ 
				}
				break;

			case T_SNAKE:

				object[i].state2++;

				if (pers_x>object[i].x && object[i].y==pers_y &&
					object[i].state<2 && object[i].state3<=96) {
					object[i].state=3;
					object[i].state2=0;
				} /* if */ 
				if (pers_x<object[i].x && object[i].y==pers_y &&
					object[i].state<2 && object[i].state3<=96) {
					object[i].state=2;
					object[i].state2=0;
				} /* if */ 

				if (object[i].state<2 && (rand()%300)==0) {
					object[i].state+=2;
					object[i].state2=0;
				} /* if */ 

				switch(object[i].state) {
				case 0:
					if (object[i].state3>=0) object[i].state3--;
					object[i].tile=(object[i].state2>>3)&0x01;
					if ((tiles[snake_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     object[i].x>0 &&
						(col_buffer[GAME_VIEW_X+object[i].x-1+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x-=TILE_UNIT;
					} else {
						object[i].state=1;
					} /* if */ 
					break;
				case 1:
					if (object[i].state3>=0) object[i].state3--;
					object[i].tile=((object[i].state2>>3)&0x01)+2;
					if ((tiles[snake_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     (object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
						(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x+=TILE_UNIT;
					} else {
						object[i].state=0;
					} /* if */ 
					break;
				case 2:
					object[i].state3++;
					object[i].tile=4;
					if (object[i].state2==32) {
						int val;
						Sound_play(S_snakebullet);
						val=add_object(T_ENEMY,object[i].x-TILE_SIZE_X,object[i].y+TILE_SIZE_Y/2,1,T_SNAKE_BULLET,0);
						object[val].base_tile=snake_tile;
						object[i].desired_floor=val;
						object[val].tile=7;
					} /* if */ 
					if (object[i].state2>=64 &&
						object[object[i].desired_floor].type!=T_ENEMY) {

						object[i].state=0;
						object[i].state2=0;
					} /* if */ 
					break;
				case 3:
					object[i].state3++;
					object[i].tile=5;
					if (object[i].state2==32) {
						int val;
						Sound_play(S_snakebullet);
						val=add_object(T_ENEMY,object[i].x+TILE_SIZE_X,object[i].y+TILE_SIZE_Y/2,1,T_SNAKE_BULLET,1);
						object[val].base_tile=snake_tile;
						object[i].desired_floor=val;
						object[val].tile=6;
					} /* if */ 
					if (object[i].state2>=64 &&
						object[object[i].desired_floor].type!=T_ENEMY) {

						object[i].state=1;
						object[i].state2=0;
					} /* if */ 
					break;
				} /* switch */ 
				break;

			case T_SNAKE_BULLET:
				if ((tiles[snake_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)!=0) {
					object[i].type=T_NADA;
				} /* if */ 			   

				if (object[i].state==0) object[i].x-=TILE_SIZE_X/2;
								   else object[i].x+=TILE_SIZE_X/2;

				if (object[i].x<(-2*TILE_SIZE_X) ||
					object[i].x>(room_size_x*TILE_SIZE_X)) {
					object[i].type=T_NADA;
				} /* if */ 
				break;


			case T_GORILLA:
				object[i].state2++;

				if ((object[i].y==pers_y || object[i].y==(pers_y+4*TILE_SIZE_Y)) &&
					object[i].state<2) {
					object[i].state=2;
					object[i].state2=0;
				} /* if */ 

				switch(object[i].state) {
				case 0:
					object[i].tile=(object[i].state2>>3)&0x01;
					if ((tiles[snake_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     object[i].x>0 &&
						(col_buffer[GAME_VIEW_X+object[i].x-1+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x-=TILE_UNIT*2;
					} else {
						object[i].state=1;
					} /* if */ 
					break;
				case 1:
					object[i].tile=((object[i].state2>>3)&0x01)+2;
					if ((tiles[snake_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     (object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
						(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x+=TILE_UNIT*2;
					} else {
						object[i].state=0;
					} /* if */ 
					break;
				case 2:
					object[i].tile=4+((object[i].state2>>3)&0x01);
					if (object[i].state2>=48) {
						object[i].union_x=pers_x;
						if (pers_x<object[i].x) object[i].state=3;
										   else object[i].state=4;
					} /* if */ 
					break;
				case 3:
					object[i].tile=(object[i].state2>>3)&0x01;
					if ((tiles[snake_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     object[i].x>0 &&
						(col_buffer[GAME_VIEW_X+object[i].x-1+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x-=TILE_UNIT*2;
					} else {
						object[i].state=1;
					} /* if */ 
					if (abs(object[i].x-object[i].union_x)<=(TILE_UNIT*2)) {
						object[i].state=5;
						object[i].state2=0;
						object[i].tile=7;
						Sound_play(S_gorilla);
					} /* if */ 
					break;
				case 4:
					object[i].tile=((object[i].state2>>3)&0x01)+2;
					if ((tiles[snake_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     (object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
						(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*2+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x+=TILE_UNIT*2;
					} else {
						object[i].state=0;
					} /* if */ 
					if (abs(object[i].x-object[i].union_x)<=(TILE_UNIT*2)) {
						object[i].state=6;
						object[i].state2=0;
						object[i].tile=7;
						Sound_play(S_gorilla);
					} /* if */ 
					break;
				case 5:
				case 6:
					object[i].tile=7;
					if (object[i].state2>=16) {
						object[i].state-=5;
						object[i].state2=0;
					} /* if */ 
					break;
				} /* switch */ 

				break;

			case T_GLIZARD:
				object[i].state2++;

				if (object[i].x>=(pers_x-3*TILE_SIZE_X) && object[i].x<=(pers_x+1*TILE_SIZE_Y) &&
					object[i].state<2 && object[i].state2>=32) {
					int val;

					Sound_play(S_lizardtongue);
					if (pers_y<=object[i].y) {
						object[i].state+=2;
						object[i].state2=0;

						val=add_object(T_ENEMY,object[i].x,object[i].y,6,T_GLIZARD_TONGUE,0);
						object[val].base_tile=glizard_tile;
					} else {
						object[i].state+=4;
						object[i].state2=0;

						val=add_object(T_ENEMY,object[i].x,object[i].y,6,T_GLIZARD_TONGUE,1);
						object[val].base_tile=glizard_tile;
					} /* if */ 
				} /* if */ 

				switch(object[i].state) {
				case 0:
					object[i].tile=(object[i].state2>>1)&0x01;
					if ((tiles[snake_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*4,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     object[i].x>0 &&
						(col_buffer[GAME_VIEW_X+object[i].x-1+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x-=TILE_UNIT*2;
					} else {
						object[i].state=1;
					} /* if */ 
					break;
				case 1:
					object[i].tile=((object[i].state2>>1)&0x01);
					if ((tiles[snake_tile+object[i].tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*4,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0 &&
					     (object[i].x+TILE_UNIT)<(room_size_x-2)*TILE_SIZE_X &&
						(col_buffer[GAME_VIEW_X+object[i].x+TILE_SIZE_X*4+
									(GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*2)*dx]&colision)!=0) {
						object[i].x+=TILE_UNIT*2;
					} else {
						object[i].state=0;
					} /* if */ 
					break;
				case 2:
				case 3:
					object[i].tile=0;
					if (object[i].state2>=48) {
						object[i].state2=0;
						object[i].state-=2;
					} /* if */ 
					break;
				case 4:
				case 5:
					object[i].tile=0;
					if (object[i].state2>=48) {
						object[i].state2=0;
						object[i].state-=4;
					} /* if */ 
					break;
				} /* switch */ 

				break;

			case T_GLIZARD_TONGUE:
				object[i].state2++;
				if (object[i].state2>=48) object[i].type=T_NADA;
				break;

			case T_BDEMON:
				object[i].state2++;
				object[i].tile=(object[i].state2>>3)%2;
				switch(object[i].state) {
				case 0:
					object[i].state3=0;
					object[i].desired_floor=2;
					object[i].state2=320;
					object[i].state=1;
					object[i].union_x=5*TILE_SIZE_X+(rand()%18)*TILE_SIZE_X;
					object[i].union_y=5*TILE_SIZE_Y+(rand()%6)*TILE_SIZE_Y;

					object[i].union_x=(object[i].union_x+pers_x)/2;
					object[i].union_y=(object[i].union_y+pers_y)/2;
					if (object[i].union_x<5*TILE_SIZE_X) object[i].union_x=5*TILE_SIZE_X;
					if (object[i].union_x>23*TILE_SIZE_X) object[i].union_x=23*TILE_SIZE_X;
					if (object[i].union_y<5*TILE_SIZE_Y) object[i].union_x=5*TILE_SIZE_Y;
					if (object[i].union_y>11*TILE_SIZE_Y) object[i].union_x=11*TILE_SIZE_Y;
				case 1:
					object[i].state3+=object[i].desired_floor;
					if ((object[i].state2&0x1f)==0 && object[i].desired_floor<9) {
						object[i].desired_floor++;
					} /* if */ 
					if (object[i].state2>=640) {
						object[i].state=2;
						object[i].state2=0;
					} /* if */ 
					break;
				case 2:
					object[i].state3+=object[i].desired_floor;
					if ((object[i].state2&0x1f)==0 && object[i].desired_floor>-9) {
						object[i].desired_floor--;
					} /* if */ 
					if (object[i].state2>=640) {
						object[i].state=1;
						object[i].state2=0;
					} /* if */ 
					break;
				} /* switch */ 

				if (abs(object[i].desired_floor)>2 && (object[i].state2&0x01)==0) {
					if (object[i].x!=object[i].union_x) {
						if (object[i].x<object[i].union_x) object[i].x+=TILE_UNIT/2;
													  else object[i].x-=TILE_UNIT/2;
						if (object[i].y!=object[i].union_y) {
							if (object[i].y<object[i].union_y) object[i].y+=TILE_UNIT/2;
														  else object[i].y-=TILE_UNIT/2;
						} /* if */ 
					} else {
						if (object[i].y!=object[i].union_y) {
							if (object[i].y<object[i].union_y) object[i].y+=TILE_UNIT/2;
														  else object[i].y-=TILE_UNIT/2;
						} else {
							object[i].union_x=5*TILE_SIZE_X+(rand()%18)*TILE_SIZE_X/2;
							object[i].union_y=5*TILE_SIZE_Y+(rand()%6)*TILE_SIZE_Y/2;

							object[i].union_x=(object[i].union_x+pers_x)/2;
							object[i].union_y=(object[i].union_y+pers_y)/2;
							if (object[i].union_x<5*TILE_SIZE_X) object[i].union_x=5*TILE_SIZE_X;
							if (object[i].union_x>23*TILE_SIZE_X) object[i].union_x=23*TILE_SIZE_X;
							if (object[i].union_y<5*TILE_SIZE_Y) object[i].union_x=5*TILE_SIZE_Y;
							if (object[i].union_y>11*TILE_SIZE_Y) object[i].union_x=11*TILE_SIZE_Y;
						} /* if */ 
					} /* if */ 
				} /* if */ 

				if ((object[i].state2&0x3f)==0x3f) {
					Sound_play(S_bdemonbullet);
					j=add_object(T_ENEMY,object[i].x+TILE_SIZE_X+TILE_SIZE_X/2+object[i].xoffs,
										 object[i].y+TILE_SIZE_Y*2+object[i].yoffs,0,T_BDEMON_BULLET,0);
					object[j].state=pers_x-object[j].x;
					object[j].state2=pers_y-object[j].y;
					object[j].base_tile=bdemon_tile;
					object[j].tile=2;	
				} /* if */ 

				object[i].xoffs=int(cos(float(object[i].state3)/80.0F)*5*TILE_SIZE_X);
				object[i].yoffs=int(sin(float(object[i].state3)/80.0F)*5*TILE_SIZE_Y);
				break;

			case T_BDEMON_BULLET:
				{
					int j;
					for(j=0;j<2;j++) {
						if (abs(object[i].state)>abs(object[i].state2)) {
							object[i].x+=(object[i].state>0 ? TILE_UNIT:-TILE_UNIT);
							object[i].state3-=abs(object[i].state2);
							if (object[i].state3<0) {
								object[i].state3+=abs(object[i].state);
								object[i].y+=(object[i].state2>0 ? TILE_UNIT:-TILE_UNIT);
							} /* if */ 
						} else {
							object[i].y+=(object[i].state2>0 ? TILE_UNIT:-TILE_UNIT);
							object[i].state3-=abs(object[i].state);
							if (object[i].state3<0) {
								object[i].state3+=abs(object[i].state2);
								object[i].x+=(object[i].state>0 ? TILE_UNIT:-TILE_UNIT);
							} /* if */ 
						} /* if */ 
						if (object[i].x<-2*TILE_SIZE_X || object[i].x>=dx ||
							object[i].y<-2*TILE_SIZE_Y || object[i].y>=dy) {
							object[i].type=T_NADA;
						} /* if */ 
					} /* for */ 
				}
				break;

			case T_LCLOUD:
				if ((rand()%32)!=0) object[i].state2++;

				switch(object[i].state) {
				case 0:object[i].tile=(object[i].state2>>3)%2;
					if ((object[i].state2&0x01)==0) object[i].x+=TILE_UNIT/2;
					if (object[i].x>=27*TILE_SIZE_X) object[i].state=1;
					if ((rand()%300)==0) {
						object[i].state+=2;
						object[i].state3=0;
					} /* if */ 
					break;
				case 1:object[i].tile=(object[i].state2>>3)%2;
					if ((object[i].state2&0x01)==0) object[i].x-=TILE_UNIT/2;
					if (object[i].x<=2*TILE_SIZE_X) object[i].state=0;
					if ((rand()%300)==0) {
						object[i].state+=2;
						object[i].state3=0;
					} /* if */ 
					break;
				case 2:
					object[i].state3++;
					object[i].tile=((object[i].state2>>3)%2)+(object[i].state2&0x01)*2;
					if ((object[i].state2&0x01)==0) object[i].x+=TILE_UNIT/2;
					if (object[i].x>=28*TILE_SIZE_X) object[i].state=3;
					if (object[i].state3>=32) {
						int j;
						Sound_play(S_lightning);
						/* Rayo: */ 
						j=add_object(T_ENEMY,object[i].x,object[i].y+TILE_SIZE_Y*2+object[i].yoffs,0,T_LIGHTNING,0);
						object[j].base_tile=lcloud_tile;
						object[i].state-=2;
					} /* if */ 
					break;
				case 3:
					object[i].state3++;
					object[i].tile=((object[i].state2>>3)%2)+(object[i].state2&0x01)*2;
					if ((object[i].state2&0x01)==0) object[i].x-=TILE_UNIT/2;
					if (object[i].x<=2*TILE_SIZE_X) object[i].state=2;
					if (object[i].state3>=32) {
						Sound_play(S_lightning);
						/* Rayo: */ 
						j=add_object(T_ENEMY,object[i].x,object[i].y+TILE_SIZE_Y*2+object[i].yoffs,0,T_LIGHTNING,0);
						object[j].base_tile=lcloud_tile;
						object[i].state-=2;
					} /* if */ 
					break;
				} /* switch */ 

				object[i].yoffs=int(sin(float(object[i].state2)/10.0F)*TILE_SIZE_Y/2);
				break;

			case T_LIGHTNING:
				object[i].state2++;
				object[i].state++;
				if ((object[i].state-1)<6) {
					object[i].state3++;
					object[i].lightning[object[i].state-1]=rand()%5;
				} else {
					int init[5]={6,10,15,3,3};
					int end[5]={5,5,10,13,13};
					int j;

					object[i].y+=2*TILE_SIZE_Y;
					object[i].x+=(end[object[i].lightning[0]]-init[object[i].lightning[1]])*TILE_UNIT;

					for(j=0;j<5;j++) object[i].lightning[j]=object[i].lightning[j+1];
					object[i].lightning[5]=rand()%5;

					if (object[i].state>=9) {
						object[i].state3--;
						if (object[i].state3==0) {
							int tx;
							tx=object[i].x/TILE_SIZE_X;
							if (iswall(tx,19) &&
								iswall(tx+1,19)) {
								object[i].enemy=T_LIGHTNING_FIRE;
								object[i].y=17*TILE_SIZE_Y;
								object[i].tile=4;
								Sound_play(S_firebreath);
							} else {
								object[i].type=T_NADA;
							} /* if */ 
						} /* if */ 
					} /* if */ 
				} /* if */ 
				break;

			case T_LIGHTNING_FIRE:
				object[i].state2++;
				object[i].tile=4+((object[i].state2>>2)%2);
				if (object[i].state2>=32) object[i].type=T_NADA;
				break;

			case T_DEMON10_BALL:
				if (object[i].state==0) {
					object[i].state2++;
					object[i].tile=4+(object[i].state2%5);
					object[i].xoffs=int(cos(float(object[i].state2)/5.0F)*object[i].state3);
					object[i].yoffs=int(sin(float(object[i].state2)/5.0F)*object[i].state3);
				} else {
					object[i].tile=-1;
				} /* if */ 
				break;

			case T_PAMPERSE_BALL:
				object[i].state2++;
				object[i].xoffs=int(cos(float(object[i].state2)/16.0F)*object[i].state);
				object[i].yoffs=int(sin(float(object[i].state2)/16.0F)*object[i].state);
				object[i].state++;
				if ((object[i].x+object[i].xoffs)<-2*TILE_SIZE_X || (object[i].x+object[i].xoffs)>=32*TILE_SIZE_X ||
					(object[i].y+object[i].yoffs)<-2*TILE_SIZE_Y || (object[i].y+object[i].yoffs)>=20*TILE_SIZE_X) {
					object[i].type=T_NADA;
				} /* if */ 				
				break;

			} /* switch */ 
		} /* if */ 
	} /* for */ 
} /* GameObjectAnimation */ 

