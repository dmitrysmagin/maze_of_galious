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

extern BYTE *col_buffer,*buffer_screen;

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
extern unsigned char old_keyboard[SDLK_LAST];
extern int sword_x,sword_y;	/* Coordenadas de la espada.	*/ 
extern int sword_time;
extern int in_ladder;			/* En qué escalera está el personaje	*/ 
extern bool previous_x_collision;	/* Hubo una colisión en X en el frame anterior? */ 
extern bool previous_y_collision;	/* Hubo una colisión en Y en el frame anterior? */ 
extern int hit_time;	/* Contador para saber cuando pueden dañarnos de nuevo	*/ 
extern int character;
extern bool live_character[2];
extern int current_weapon;
extern int n_fired_arrows,n_fired_mines;
extern bool mine_stepped;
extern int stone_hit_counter;

extern int in_door;
extern int to_enter_cut;

extern bool shop_item[3];
extern int passage_state,currently_selecting;

extern int slime_counter,stones_counter,piranhas_counter;
extern bool room_slimes,room_watermonster,room_fallingstones,room_rollingstones;
extern int room_waterdragons;
extern bool room_blobs,room_lava,room_piranhas;
extern int room_reddragon;
extern bool room_demonroom,old_room_demonroom;
extern int room_fairy,fairy_x,fairy_y;
extern int room_appearwall,room_appearwall_x,room_appearwall_y;
extern int appearwall_record_x[MAX_AW_RECORD];
extern int appearwall_record_y[MAX_AW_RECORD];
extern int fighting_demon,watermonster_state;
extern int world3_room06_counter,world4_state;
extern int world5_room44_state,world5_room44_state2,world5_room44_state3,world5_room64_state;
extern int world6_room35_state;
extern int world7_room12_state;
extern int world8_room12_state;
extern int world10_door_x,world10_door_y;
extern int room26_state,room56_state;
extern int lava_state;
extern int room_bfly,room_birds;

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
extern bool in_water;
extern int water_counter;

bool todelete[MAX_OBJECTS];

extern int room_size_x,room_size_y;
extern int *room_tiles;
extern bool character_over_vertical_lever;

/* Objetos: (escaleras, piedras, etc...) */ 
extern MOGObject object[MAX_OBJECTS];
extern int n_objects;

extern int wall_tile;
extern int stone_tile,ladder_tile,wdoor_tile,keydoor_tile,stats_tile;
extern int coin_tile,arrow_tile,key_tile,door_tile;
extern int character_tile[2],character_tile_inv[2];
extern int pause_tile[2];
extern int colision_tile;
extern int sword_tile[2],explosion_tile;
extern int worm_tile,bat_tile,skeleton_tile,smoke_tile,slime_tile,bouncingball_tile;
extern int watermonster_tile,jumpingbush_tile,bluespider_tile,whitefireball_tile;
extern int knight_tile,blob_tile,bambu_tile,porcupine_tile,fairy_tile,witch_tile;
extern int whitebear_tile,feet_tile,redjumper_tile,lever_tile,livingwall_tile;
extern int megabat_tile,fstone_tile,lava_tile,piranha_tile,bfly_tile;
extern int items_tile,armas_tile,tomb_tile,wkeydoor_tile,wnokeydoor_tile;
extern int demonsign_tile,lupa_tile,demonsmoke_tile,demon_tile,waterdragon_tile;
extern int bird_tile,reddragon_tile,lcloud_tile,glizard_tile,snake_tile,bdemon_tile;
extern int gorilla_tile;

/* Estado de las habitaciones por las que ya has pasado: */ 
#define HP_OBJS_MAX	512

extern int HP_room_x[HP_OBJS_MAX],HP_room_y[HP_OBJS_MAX];
extern int HP_obj[HP_OBJS_MAX];
extern int HP_obj_type[HP_OBJS_MAX];

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

extern int worm_state_tile[28];
extern int worm_state_tile_i[28];
extern int worm_state_y[28];


/* Efectos de sonido: */ 
extern SOUNDT S_pause,S_death,S_gameover,S_worldkey,S_entering;
extern SOUNDT S_gamestart,S_jump,S_sword,S_swordhit;
extern SOUNDT S_item,S_ah,S_powerup,S_enemyhit,S_door;
extern SOUNDT S_enemykill,S_ladderdisapear,S_armourhit;
extern SOUNDT S_enterwater,S_select,S_firearrow,S_firearrow2;
extern SOUNDT S_nocoins,S_rotatedoor,S_appearing,S_skeletonhit;
extern SOUNDT S_firefire,S_dropmine,S_fireball,S_fireball2;
extern SOUNDT S_wdoor,S_door2,S_demonhit,S_waterstone,S_mine;
extern SOUNDT S_enemybullet;

/* Control de pantalla: */ 
extern int logic_dx[4],logic_dy[4],logic_x[4],logic_y[4];
extern int physic_dx[4],physic_dy[4],physic_x[4],physic_y[4];

extern int zoom;


/* Teclas: */ 
extern SDLKey UP_KEY,DOWN_KEY,LEFT_KEY,RIGHT_KEY;
extern SDLKey SWORD_KEY,WEAPON_KEY,ITEM_KEY,PAUSE_KEY;

void ActualizeLevers(int dx,int dy)
{
	int i;
	int old_x=pers_x;
	int old_y=pers_y;

	character_over_vertical_lever=false;

	for(i=0;i<MAX_LEVERS;i++) {
		bool character_over=false;

		if (map==0 && map_x==lever_room_x[i] && map_y==lever_room_y[i] &&
			pers_y>=lever_y[i]-TILE_SIZE_Y*2 &&
			pers_y<lever_y[i]+TILE_SIZE_Y*2) {
			if (pers_x>((lever_x[i]-(TILE_SIZE_Y*2))) && pers_x<((lever_x[i]+4*TILE_SIZE_X)) 
				&& 
				(pers_state==S_QUIET ||
				pers_state==S_WALKING_RIGHT ||
				pers_state==S_WALKING_LEFT ||
				pers_state==S_QUIET_SWORD ||
				pers_state==S_HIT_RECOVERY ||
				pers_state==S_CONTROLED_FALL ||
				pers_state==S_UNCONTROLED_FALL)
				) {
				character_over=true;
			} /* if */ 
			if (pers_x>((lever_x[i]-(TILE_SIZE_Y*2-5*TILE_UNIT))) && pers_x<((lever_x[i]+4*TILE_SIZE_X-4*TILE_UNIT))) {
				character_over=true;
			} /* if */ 
		} /* if */ 
		if (lever_x[i]<lever_dx[i][lever_going_to[i]]) {
			lever_x[i]+=TILE_UNIT*2;
			if (character_over) pers_x+=TILE_UNIT*2;
		} /* if */ 
		if (lever_x[i]>lever_dx[i][lever_going_to[i]]) {
			lever_x[i]-=TILE_UNIT*2;
			if (character_over) pers_x-=TILE_UNIT*2;
		} /* if */ 
		if ((lever_y[i]<lever_dy[i][lever_going_to[i]] && lever_room_y[i]==lever_dry[i][lever_going_to[i]]) || 
			 lever_room_y[i]<lever_dry[i][lever_going_to[i]]) {
			if (character_over && pers_y==lever_y[i]-TILE_SIZE_Y*2) pers_y+=TILE_UNIT*2;
			lever_y[i]+=TILE_UNIT*2;
			if (lever_y[i]>room_size_y*TILE_SIZE_Y) {
				lever_y[i]=TILE_SIZE_Y;
				lever_room_y[i]++;
			} /* if */ 

			if (character_over) character_over_vertical_lever=true;
		} /* if */ 
		if ((lever_y[i]>lever_dy[i][lever_going_to[i]] && lever_room_y[i]==lever_dry[i][lever_going_to[i]]) || 
			 lever_room_y[i]>lever_dry[i][lever_going_to[i]]) {
			if (character_over && pers_y==lever_y[i]-TILE_SIZE_Y*2) pers_y-=TILE_UNIT*2;
			lever_y[i]-=TILE_UNIT*2;
			if (lever_y[i]<TILE_SIZE_Y) {
				lever_y[i]=(room_size_y-1)*TILE_SIZE_Y;
				lever_room_y[i]--;
			} /* if */ 
			if (character_over) character_over_vertical_lever=true;
		} /* if */ 
		if (lever_x[i]==lever_dx[i][lever_going_to[i]] &&
			lever_y[i]==lever_dy[i][lever_going_to[i]] &&
			lever_room_x[i]==lever_drx[i][lever_going_to[i]] &&
			lever_room_y[i]==lever_dry[i][lever_going_to[i]]) {
			if (lever_going_to[i]==0) lever_going_to[i]=1;
								 else lever_going_to[i]=0;
		} /* if */ 

	} /* for */ 

	/* Corregir la posición de la espada: */ 
	if (sword) {
		sword_x+=pers_x-old_x;
		sword_y+=pers_y-old_y;
	} /* if */ 

} /* ActualizeLevers */ 


void GameTestInteractions(int dx,int dy)
{
	int i,j,val;
	bool hit=false;
	unsigned char *keyboard;
	int enemy_killed=false;

	for(i=0;i<MAX_OBJECTS;i++) todelete[i]=false;

	SDL_PumpEvents();
	keyboard = (unsigned char *)SDL_GetKeyState(NULL);

	if (sword && !old_sword) hit=true;
	old_sword=sword; 

	/* Comprobar si se ha golpeado una piedra o un objeto: */ 
	if (stone_hit_counter>0) stone_hit_counter--;
	for(i=0;i<n_objects;i++) {
		if (object[i].type==T_PAMPERSE) {
			if ((tiles[182]->
				 	coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_PLAYER)!=0) {
				/* FINAL DEL JUEGO!!!! TIENEN A PAMPERSE!!!!: */ 
				STATE=18;
				SUBSTATE=0;
			} /* if */ 	
		} /* if */ 

		if (object[i].type==T_STONE && object[i].last_hit==0) {
			if ((tiles[stone_tile+2]->
					coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_WEAPON)!=0 ||
				(item[20] && keyboard[SWORD_KEY] && map!=10)) {
				/* Piedra tocada!! */ 
				stone_hit_counter+=8;
				int obj=test_if_weapon_colision(stone_tile,object[i].draw_x,object[i].draw_y);
				if (obj==-1 || object[obj].type==T_MINE_EXP) object[i].last_hit=HIT_PERIOD;
				if (!(item[20] && keyboard[SWORD_KEY] && map!=10)) Sound_play(S_swordhit);
				if (character==0 || item[18]) object[i].size-=5;
										 else object[i].size--;
				if (object[i].size<=0) Sound_play(S_enemykill);
				if (object[i].size<=0 ||
					(item[20] && keyboard[SWORD_KEY] && map!=10)) {
					/* Piedra destruida */ 
					HP_add_record(map_x,map_y,i,T_STONE);
					object[i].type=T_BROKEN_STONE;
					object[i].size=8;
				} /* if */ 
			} /* if */ 
		} /* if */ 

		if (object[i].type==T_FAIRY) {
			/* Comprobar si ha tocado al jugador: */ 
			if (object[i].state3>=80) {
				int val;

				if (object[i].state>=0) {
					val=fairy_tile+((object[i].state3>>2)&0x01);
				} else {
					val=fairy_tile+((object[i].state3>>2)&0x01);
				} /* if */ 

				if ((tiles[val]->
					 	coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_PLAYER)!=0) {
					object[i].type=T_NADA;
					Sound_play(S_powerup);
					player_energy[character]=player_max[character];
				} /* if */ 
			
			} /* if */ 
		} /* if */ 

		if (object[i].type==T_ARMOUR) {
			if (object[i].tile>=0 &&
				(tiles[object[i].base_tile+object[i].tile]->
					coltest(GAME_VIEW_X+object[i].x+object[i].xoffs,
							GAME_VIEW_Y+object[i].y+object[i].yoffs,
							col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
				int old_type=0;
				int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,
												object[i].draw_x,
												object[i].draw_y,&old_type);
				if (obj==-1 && object[i].last_hit==0) Sound_play(S_armourhit);		
				if (obj==-1 || old_type==T_WEAPON_ARROW2) object[i].last_hit=HIT_PERIOD;
				if (obj!=-1) {
					if (object[obj].type==T_WEAPON_MINE || 
						object[obj].type==T_MINE_EXP) {
						object[i].last_hit=HIT_PERIOD*2;
					} else {
						if (obj!=-1) {
							if (old_type==T_WEAPON_ARROW ||
								old_type==T_WEAPON_ARROW2 ||
								old_type==T_WEAPON_FIRE ||
								old_type==T_WEAPON_RFIRE) {
								if (old_type==T_WEAPON_ARROW) object[obj].type=T_RFL_ARROW;
								if (old_type==T_WEAPON_ARROW2) object[obj].type=T_RFL_ARROW2;
								if (old_type==T_WEAPON_FIRE) object[obj].type=T_RFL_FIRE;
								if (old_type==T_WEAPON_RFIRE) object[obj].type=T_RFL_RFIRE;
								todelete[obj]=false;
							} else {
								todelete[obj]=true;
							} /* if */ 
						} /* if */ 
						Sound_play(S_armourhit);						
					} /* if */ 
				} /* if */ 
				
			} /* if */ 
		} /* if */ 

		if (object[i].type==T_ENEMY && object[i].last_hit==0) {
			switch(object[i].enemy) {
			case T_WORM:
				j=object[i].state;
				if (j<0) {
					j=-j;
					j--;
					if (j<0) j=0;
					if (j>27) j=27;
					val=worm_state_tile_i[j];
				} else {
					j--;
					if (j<0) j=0;
					if (j>27) j=27;
					val=worm_state_tile[j];
				} /* if */ 
				if ((tiles[worm_tile+val]->
						coltest(object[i].draw_x,object[i].draw_y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {
					enemy_killed=true;
					test_if_weapon_colision(worm_tile+val,object[i].draw_x,object[i].draw_y);
					/* Gusano tocado!! */ 
					HP_add_record(map_x,map_y,i,T_ENEMY);
					Sound_play(S_enemykill);
					player_experience[character]+=2;
					if (item[14]) player_experience[character]+=2;
					object[i].type=T_EXPLOSION;
					object[i].size=27;
					object[i].state=1;
				} /* if */ 
				break;
			case T_BAT:
				val=2;
				if (object[i].state==1) val=((object[i].state2>>2)%2);
				if (((tiles[bat_tile+val]->
						coltest(object[i].draw_x,object[i].draw_y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) ||
					(item[25] && typed_word_p("umbrella"))) {
					if (current_weapon!=1) enemy_killed=true;
					test_if_weapon_colision(bat_tile+val,object[i].draw_x,object[i].draw_y);
					/* Murcielago tocado!! */ 
					HP_add_record(map_x,map_y,i,T_ENEMY);
					Sound_play(S_enemykill);
					player_experience[character]+=2;
					if (item[14]) player_experience[character]+=2;
					object[i].type=T_EXPLOSION;
					object[i].size=27;
					object[i].state=1;
				} /* if */ 
				break;
			case T_SKELETON:
				switch(object[i].size) {
				case 8:
					val=(object[i].state2>>2)%2;
					if (object[i].state==1 ||
						object[i].state==3) {
						val+=2;
					} /* if */ 
					break;
				default:
					val=4;
					break;
				} /* switch */ 
				
				if ((tiles[skeleton_tile+val]->
						coltest(object[i].draw_x,object[i].draw_y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {
					int old_type=0;
					int obj=test_if_weapon_colision(skeleton_tile+val,object[i].draw_x,object[i].draw_y,&old_type);
					enemy_killed=true;
					if ((obj==-1 &&
						 (((object[i].state&0x01)==0 && pers_x>object[i].x) ||
						  ((object[i].state&0x01)==1 && pers_x<object[i].x))) 
						||
						(obj>=0 &&
 						 (((object[i].state&0x01)==0 && object[obj].state==1) ||
						  ((object[i].state&0x01)==1 && object[obj].state==0) ||
						  object[obj].type==T_WEAPON_MINE || 
						  object[obj].type==T_MINE_EXP))) {
						/* Esqueleto tocado!! */ 
						object[i].size--;
						if (item[24]) object[i].size--;
						if (obj==-1 ||
							object[obj].type==T_WEAPON_ARROW2) object[i].last_hit=HIT_PERIOD;
						if (obj!=-1 &&
							(object[obj].type==T_WEAPON_MINE || 
							 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;

						if (object[i].size<=0) {
							/* Esqueleto muerto!! */ 
							HP_add_record(map_x,map_y,i,T_ENEMY);
							Sound_play(S_enemykill);
							player_experience[character]+=8;
							if (item[14]) player_experience[character]+=8;
							object[i].type=T_EXPLOSION;
							object[i].state=0;
							object[i].size=27;
						} else {
							Sound_play(S_skeletonhit);
							object[i].state2=-48;
						} /* if */ 
					} else {
						if (obj!=-1) {
							if (old_type==T_WEAPON_ARROW ||
								old_type==T_WEAPON_ARROW2 ||
								old_type==T_WEAPON_FIRE ||
								old_type==T_WEAPON_RFIRE) {
								if (old_type==T_WEAPON_ARROW) object[obj].type=T_RFL_ARROW;
								if (old_type==T_WEAPON_ARROW2) object[obj].type=T_RFL_ARROW2;
								if (old_type==T_WEAPON_FIRE) object[obj].type=T_RFL_FIRE;
								if (old_type==T_WEAPON_RFIRE) object[obj].type=T_RFL_RFIRE;
								todelete[obj]=false;
							} else {
								todelete[obj]=true;
							} /* if */ 
						} /* if */ 
						Sound_play(S_armourhit);
					} /* if */ 
				} /* if */ 

				break;
			case T_SLIME:
				val=object[i].state2/6;
				if (object[i].state==1) val+=2;
				if ((tiles[slime_tile+val]->
						coltest(object[i].draw_x,object[i].draw_y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {
					int obj=test_if_weapon_colision(slime_tile+val,object[i].draw_x,object[i].draw_y);
					enemy_killed=true;
					/* Oruga tocada!! */ 
					if (obj==-1 || current_weapon==1) object[i].last_hit=HIT_PERIOD;
					if (obj!=-1 &&
						(object[obj].type==T_WEAPON_MINE || 
						 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;
					object[i].size--;
					if (item[24]) object[i].size--;
					if (object[i].size<=0) {
						/* Oruga muerta: */ 
						player_experience[character]+=4;
						if (item[14]) player_experience[character]+=4;
						object[i].type=T_EXPLOSION;
						object[i].state=1;
						object[i].size=27;
						Sound_play(S_enemykill);
					} else {
						Sound_play(S_enemyhit);
					} /* if */ 
				} /* if */ 
				break;

			case T_WATERMONSTER_ARM:
				{
					int obj=-1;
					bool colision=false;

					if ((tiles[watermonster_tile+1]->
							coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y-(object[i].state2*TILE_SIZE_Y)/2,
							TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
						obj=test_if_weapon_colision(watermonster_tile+1,GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y);
						colision=true;
					} /* if */ 
					for(j=0;j<(object[i].state2*TILE_SIZE_Y)/2;j+=TILE_SIZE_Y) {
						int ot;
						if ((tiles[watermonster_tile+2]->
								coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y-j,
								TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
							ot=test_if_weapon_colision(watermonster_tile+2,GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y-j);
							if (ot!=-1) obj=ot;
							colision=true;
						} /* if */ 
					} /* for */ 
					if (colision) {
						/* Monstruo tocado!! */ 
						if (obj==-1 || current_weapon==1) object[i].last_hit=HIT_PERIOD;
						if (obj!=-1 &&
							(object[obj].type==T_WEAPON_MINE || 
							 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;
						object[i].size--;
						if (item[24]) object[i].size--;
						if (object[i].size<=0) {
							player_experience[character]+=6;
							if (item[14]) player_experience[character]+=6;
							object[i].y=object[i].y-(object[i].state2*TILE_SIZE_Y)/2;
							object[i].type=T_EXPLOSION;
							object[i].state=0;
							object[i].size=27;
							Sound_play(S_enemykill);

							add_object(T_EXPLOSION,object[i].x-TILE_SIZE_X,object[i].y-TILE_SIZE_Y,43,0,3);
							add_object(T_EXPLOSION,object[i].x-TILE_SIZE_X,object[i].y+TILE_SIZE_Y,59,0,3);
							add_object(T_EXPLOSION,object[i].x+TILE_SIZE_X,object[i].y-TILE_SIZE_Y,75,0,3);
							add_object(T_EXPLOSION,object[i].x+TILE_SIZE_X,object[i].y+TILE_SIZE_Y,91,0,3);
						} else {
							Sound_play(S_enemyhit);
						} /* if */ 
					} /* if */ 

				}
				break;

			case T_KNIGHT:
				if ((tiles[knight_tile+object[i].state2]->
						coltest(object[i].draw_x,object[i].draw_y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_WEAPON)!=0 && 
						!enemy_killed) {
					int old_type=0;
					int obj=test_if_weapon_colision(knight_tile+object[i].state2,object[i].draw_x,object[i].draw_y,&old_type);
					enemy_killed=true;
					if (object[i].state>=2 
						||
						(obj==-1 &&
						 ((object[i].state==1 && pers_x>object[i].x) ||
						  (object[i].state==0 && pers_x<object[i].x))) 
						||
						(obj>=0 &&
 						 ((object[i].state==1 && object[obj].state==1) ||
						  (object[i].state==0 && object[obj].state==0) ||
						  object[obj].type==T_WEAPON_MINE || 
						  object[obj].type==T_MINE_EXP))) {
						/* Caballero tocado!! */ 
						object[i].size--;
						if (item[24]) object[i].size--;
						if (obj==-1 ||
							object[obj].type==T_WEAPON_ARROW2) object[i].last_hit=HIT_PERIOD;
						if (obj!=-1 &&
							(object[obj].type==T_WEAPON_MINE || 
							 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;
						if (object[i].size<=0) {
							/* Caballero muerto!! */ 
							HP_add_record(map_x,map_y,i,T_ENEMY);
							Sound_play(S_enemykill);
							player_experience[character]+=8;
							if (item[14]) player_experience[character]+=8;
							object[i].type=T_EXPLOSION;
							object[i].state=2;
							object[i].size=27;
						} else {
							Sound_play(S_enemyhit); 
						} /* if */ 
					} else {
						if (obj!=-1) {
							if (old_type==T_WEAPON_ARROW ||
								old_type==T_WEAPON_ARROW2 ||
								old_type==T_WEAPON_FIRE ||
								old_type==T_WEAPON_RFIRE) {
								if (old_type==T_WEAPON_ARROW) object[obj].type=T_RFL_ARROW;
								if (old_type==T_WEAPON_ARROW2) object[obj].type=T_RFL_ARROW2;
								if (old_type==T_WEAPON_FIRE) object[obj].type=T_RFL_FIRE;
								if (old_type==T_WEAPON_RFIRE) object[obj].type=T_RFL_RFIRE;
								todelete[obj]=false;
							} else {
								todelete[obj]=true;
							} /* if */ 
						} /* if */ 
						Sound_play(S_armourhit);
					} /* if */ 
				} /* if */ 
				break;
			case T_BLOB:
					if ((tiles[blob_tile+object[i].state2]->
							coltest(object[i].draw_x,object[i].draw_y,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
						!enemy_killed) {
						enemy_killed=true;
						/* Bola roja tocada!! */ 
						test_if_weapon_colision(blob_tile+object[i].state2,object[i].draw_x,object[i].draw_y);
						player_experience[character]+=2;
						if (item[14]) player_experience[character]+=2;
						object[i].type=T_EXPLOSION;
						object[i].state=1;
						object[i].size=27;
						Sound_play(S_enemykill);
					} /* if */ 
				break;

			case T_WITCH:
				{
					int j;

					if (object[i].state!=0 &&
						(tiles[witch_tile]->
							coltest(object[i].draw_x,object[i].draw_y,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
						!enemy_killed) {
						enemy_killed=true;
						/* Bruja tocada!! */ 
						int obj=test_if_weapon_colision(witch_tile,object[i].draw_x,object[i].draw_y);
						if (obj==-1 || current_weapon==1) object[i].last_hit=HIT_PERIOD;
						if (obj!=-1 &&
							(object[obj].type==T_WEAPON_MINE || 
							 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;
						for(j=0;j<n_objects;j++) 
							if (object[j].type==T_ENEMY && object[j].enemy==T_WITCH) {
								object[j].size--;
								if (item[24]) object[i].size--;
							} /* if */ 

						if (object[i].size<=0) {

							for(j=0;j<n_objects;j++) 
								if (object[j].type==T_ENEMY && object[j].enemy==T_WITCH) {
									object[j].type=T_NADA;
									HP_add_record(map_x,map_y,j,T_ENEMY);
								} /* if */ 
							player_experience[character]+=4;
							if (item[14]) player_experience[character]+=4;
							object[i].type=T_EXPLOSION;
							object[i].state=1;
							object[i].size=27;
							Sound_play(S_enemykill);
						} else {
							Sound_play(S_enemyhit);
						} /* if */ 
					} /* if */ 
				}
				break;

			case T_RSTONE:
			case T_FSTONE:
			case T_BLACK:
			case T_HEAD:
			case T_OCTOPUS:
					if (object[i].tile>=0 &&
						(tiles[object[i].base_tile+object[i].tile]->
							coltest(object[i].draw_x,object[i].draw_y,
									col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
						!enemy_killed) {
						int old_type=0;
						int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,
														object[i].draw_x,
														object[i].draw_y,&old_type);
						enemy_killed=true;
						if (obj==-1 && object[i].last_hit==0) Sound_play(S_armourhit);
						if (obj==-1 || old_type==T_WEAPON_ARROW2) object[i].last_hit=HIT_PERIOD;
						if (obj!=-1) {
							if (object[obj].type==T_WEAPON_MINE || 
								object[obj].type==T_MINE_EXP) {
								object[i].last_hit=HIT_PERIOD*2;
							} else {
								if (obj!=-1) {
									if (old_type==T_WEAPON_ARROW ||
										old_type==T_WEAPON_ARROW2 ||
										old_type==T_WEAPON_FIRE ||
										old_type==T_WEAPON_RFIRE) {
										if (old_type==T_WEAPON_ARROW) object[obj].type=T_RFL_ARROW;
										if (old_type==T_WEAPON_ARROW2) object[obj].type=T_RFL_ARROW2;
										if (old_type==T_WEAPON_FIRE) object[obj].type=T_RFL_FIRE;
										if (old_type==T_WEAPON_RFIRE) object[obj].type=T_RFL_RFIRE;
										todelete[obj]=false;
									} else {
										todelete[obj]=true;
									} /* if */ 
								} /* if */ 
								Sound_play(S_armourhit);
							} /* if */ 
						} /* if */ 
					} /* if */ 
				break;

			case T_JUMPINGBUSH:
			case T_WHITEFIREBALL:
			case T_PORCUPINE:
			case T_WHITEBEAR:
			case T_REDJUMPER:
			case T_VENT2:
			case T_LAVA3:
			case T_PIRANHA:
			case T_WHITESTAR:
			case T_KNIGHTHEAD:
			case T_CHICKEN:
			case T_ROCKMAN:
			case T_BFLY_SMALL:
			case T_BFLY_MEDIUM:
			case T_BFLY_LARGE:
			case T_BFLY_GIANT:
			case T_GHOST:
			case T_GHOST_BULLET:
			case T_HEAD_BULLET:
			case T_WORM2:
			case T_WATERDRAGON:
			case T_WATERBUG:
			case T_BIRD:
			case T_PACMAN:
			case T_DEMON6_BULLET:
			case T_OWL:
			case T_GREENMONKEY:
			case T_PLANT:
			case T_DEMON7_BULLET:
			case T_TRANSFORMER:
			case T_FLAME:
			case T_WITCH2:
			case T_FLAME_BULLET:
			case T_DEMON8_BULLET:
			case T_SNAKE:
			case T_GLIZARD:
			case T_GORILLA:
			case T_SNAKE_BULLET: 
			case T_DEMON10_BALL:
			case T_PAMPERSE_BALL:
				if (object[i].tile>=0 &&
					(tiles[object[i].base_tile+object[i].tile]->
						coltest(object[i].draw_x,object[i].draw_y,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {
					int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,
													object[i].draw_x,
													object[i].draw_y);
					enemy_killed=true;
					if (obj==-1 || (obj!=-1 && object[obj].type==T_WEAPON_ARROW2)) object[i].last_hit=HIT_PERIOD;
					if (obj!=-1 &&
						(object[obj].type==T_WEAPON_MINE || 
						 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;
					object[i].size--;
					if (item[24]) object[i].size--;
					if (object[i].size<=0) {
						HP_add_record(map_x,map_y,i,T_ENEMY);
						player_experience[character]+=object[i].experience;
						if (item[14]) player_experience[character]+=object[i].experience;
						object[i].x+=object[i].xoffs;
						object[i].y+=object[i].yoffs;
						object[i].type=T_EXPLOSION;
						object[i].state=1;
						if (object[i].enemy==T_HEAD_BULLET ||
							object[i].enemy==T_GHOST_BULLET ||
							object[i].enemy==T_LAVA3 ||
							object[i].enemy==T_DEMON6_BULLET ||
							object[i].enemy==T_DEMON7_BULLET ||
							object[i].enemy==T_DEMON8_BULLET ||
							object[i].enemy==T_SNAKE_BULLET ||
							object[i].enemy==T_DEMON10_BALL ||
							object[i].enemy==T_PAMPERSE_BALL) object[i].state=0;
						object[i].size=27;
						Sound_play(S_enemykill);
					} else {
						Sound_play(S_enemyhit);
					} /* if */ 
				} /* if */ 
				break;

			case T_BAMBU:
				if (object[i].tile>=1 &&
					(tiles[object[i].base_tile+object[i].tile]->
						coltest(object[i].draw_x,object[i].draw_y,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {
					int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,
													object[i].draw_x,
													object[i].draw_y);
					enemy_killed=true;
					if (obj==-1 || current_weapon==1) object[i].last_hit=HIT_PERIOD;
					if (obj!=-1 &&
						(object[obj].type==T_WEAPON_MINE || 
						 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;
					object[i].size--;
					if (item[24]) object[i].size--;
					if (object[i].size<=0) {
						HP_add_record(map_x,map_y,i,T_ENEMY);
						player_experience[character]+=object[i].experience;
						if (item[14]) player_experience[character]+=object[i].experience;
						object[i].x+=object[i].xoffs;
						object[i].y+=object[i].yoffs;
						object[i].type=T_EXPLOSION;
						object[i].state=1;
						object[i].state=0;
						object[i].size=27;
						Sound_play(S_enemykill);
					} else {
						Sound_play(S_enemyhit);
					} /* if */ 
				} /* if */ 
				break;

			case T_REDDRAGON:
				if (object[i].tile>=0 &&
					(tiles[object[i].base_tile+object[i].tile]->
						coltest(object[i].draw_x,object[i].draw_y,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {
					int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,
													object[i].draw_x,
													object[i].draw_y);
					enemy_killed=true;
					if (obj==-1 || current_weapon==1) object[i].last_hit=HIT_PERIOD;
					if (obj!=-1 &&
						(object[obj].type==T_WEAPON_MINE || 
						 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;
					object[i].size--;
					if (item[24]) object[i].size--;
					if (object[i].size<=0) {
						HP_add_record(map_x,map_y,i,T_ENEMY);
						player_experience[character]+=object[i].experience;
						if (item[14]) player_experience[character]+=object[i].experience;
						object[i].x+=object[i].xoffs;
						object[i].y+=object[i].yoffs;
						object[i].type=T_EXPLOSION;
						object[i].state=1;
						if (object[i].enemy==T_HEAD_BULLET ||
							object[i].enemy==T_GHOST_BULLET ||
							object[i].enemy==T_LAVA3) object[i].state=0;
						object[i].size=27;
						Sound_play(S_enemykill);

						add_object(T_EXPLOSION,object[i].x-TILE_SIZE_X,object[i].y-TILE_SIZE_Y,43,0,3);
						add_object(T_EXPLOSION,object[i].x-TILE_SIZE_X,object[i].y+TILE_SIZE_Y,59,0,3);
						add_object(T_EXPLOSION,object[i].x+TILE_SIZE_X,object[i].y-TILE_SIZE_Y,75,0,3);
						add_object(T_EXPLOSION,object[i].x+TILE_SIZE_X,object[i].y+TILE_SIZE_Y,91,0,3);
					} else {
						Sound_play(S_enemyhit);
					} /* if */ 
				} /* if */ 
				break;


			case T_STONEMAN:
				if (object[i].tile<3) {
					if (object[i].tile>=0 &&
						(tiles[object[i].base_tile+object[i].tile]->
							coltest(object[i].draw_x,object[i].draw_y,
									col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
						!enemy_killed) {
						int old_type=0;
						int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,
														object[i].draw_x,
														object[i].draw_y,&old_type);
						enemy_killed=true;
						if (obj==-1 && object[i].last_hit==0) Sound_play(S_armourhit);
						if (obj==-1 || old_type==T_WEAPON_ARROW2) object[i].last_hit=HIT_PERIOD;
						if (obj!=-1) {
							if (object[obj].type==T_WEAPON_MINE || 
								object[obj].type==T_MINE_EXP) {
								object[i].last_hit=HIT_PERIOD*2;
							} else {
								if (obj!=-1) {
									if (old_type==T_WEAPON_ARROW ||
										old_type==T_WEAPON_ARROW2 ||
										old_type==T_WEAPON_FIRE ||
										old_type==T_WEAPON_RFIRE) {
										if (old_type==T_WEAPON_ARROW) object[obj].type=T_RFL_ARROW;
										if (old_type==T_WEAPON_ARROW2) object[obj].type=T_RFL_ARROW2;
										if (old_type==T_WEAPON_FIRE) object[obj].type=T_RFL_FIRE;
										if (old_type==T_WEAPON_RFIRE) object[obj].type=T_RFL_RFIRE;
										todelete[obj]=false;
									} else {
										todelete[obj]=true;
									} /* if */ 
								} /* if */ 
								Sound_play(S_armourhit);
							} /* if */ 
						} /* if */ 
					} /* if */ 				
				} else {
					if (object[i].tile>=0 &&
						(tiles[object[i].base_tile+object[i].tile]->
							coltest(object[i].draw_x,object[i].draw_y,
									col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
						int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,
														object[i].draw_x,
														object[i].draw_y);
						if (obj==-1 || current_weapon==1) object[i].last_hit=HIT_PERIOD;
						if (obj!=-1 &&
							(object[obj].type==T_WEAPON_MINE || 
							 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;
						object[i].size--;
						if (item[24]) object[i].size--;
						if (object[i].size<=0) {
							HP_add_record(map_x,map_y,i,T_ENEMY);
							player_experience[character]+=object[i].experience;
							if (item[14]) player_experience[character]+=object[i].experience;
							object[i].x+=object[i].xoffs;
							object[i].y+=object[i].yoffs;
							object[i].type=T_EXPLOSION;
							object[i].state=1;
							if (object[i].enemy==T_HEAD_BULLET ||
								object[i].enemy==T_GHOST_BULLET ||
								object[i].enemy==T_LAVA3) object[i].state=0;
							object[i].size=27;
							Sound_play(S_enemykill);
						} else {
							Sound_play(S_enemyhit);
						} /* if */ 
					} /* if */ 
				} /* if */ 
				break;

			case T_HAIRY:
			case T_CYCLOPS:
			case T_BDEMON:
				if (object[i].tile>=0 &&
					(tiles[object[i].base_tile+object[i].tile]->
						coltest(object[i].draw_x,object[i].draw_y,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {
					int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,
													object[i].draw_x,
													object[i].draw_y);
					if (obj==-1 || current_weapon==1) object[i].last_hit=HIT_PERIOD;
					if (obj!=-1 &&
						(object[obj].type==T_WEAPON_MINE || 
						 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;
					enemy_killed=true;
					object[i].size--;
					if (item[24]) object[i].size--;
					if (object[i].size<=0) {
						HP_add_record(map_x,map_y,i,T_ENEMY);
						player_experience[character]+=object[i].experience;
						if (item[14]) player_experience[character]+=object[i].experience;
						object[i].x+=TILE_SIZE_X/2+object[i].xoffs;
						object[i].y+=TILE_SIZE_Y/2+object[i].yoffs;
						object[i].type=T_EXPLOSION;
						object[i].state=1;
						if (object[i].enemy==T_HEAD_BULLET ||
							object[i].enemy==T_GHOST_BULLET ||
							object[i].enemy==T_LAVA3) object[i].state=0;
						object[i].size=27;
						Sound_play(S_enemykill);

						add_object(T_EXPLOSION,object[i].x-TILE_SIZE_X,object[i].y-TILE_SIZE_Y,43,0,3);
						add_object(T_EXPLOSION,object[i].x-TILE_SIZE_X,object[i].y+TILE_SIZE_Y,59,0,3);
						add_object(T_EXPLOSION,object[i].x+TILE_SIZE_X,object[i].y-TILE_SIZE_Y,75,0,3);
						add_object(T_EXPLOSION,object[i].x+TILE_SIZE_X,object[i].y+TILE_SIZE_Y,91,0,3);
					} else {
						Sound_play(S_enemyhit);
					} /* if */ 
				} /* if */ 
				break;

			case T_SPIDER:
				if (object[i].tile>=0 &&
					(tiles[object[i].base_tile]->
						coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y*object[i].tile,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {
//					int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,GAME_VIEW_X+object[i].x+object[i].state2,GAME_VIEW_Y+object[i].y);
					enemy_killed=true;
					object[i].size--;
					if (item[24]) object[i].size--;
					if (object[i].size<=0) {
						HP_add_record(map_x,map_y,i,T_ENEMY);
						player_experience[character]+=object[i].experience;
						if (item[14]) player_experience[character]+=object[i].experience;
						object[i].type=T_EXPLOSION;
						object[i].state=1;
						object[i].size=27;
						Sound_play(S_enemykill);
					} else {
						Sound_play(S_enemyhit);
					} /* if */ 
				} /* if */ 
				break;

			case T_BLUESPIDER:
				if (object[i].tile>=0 &&
					(tiles[object[i].base_tile+object[i].tile]->
						coltest(object[i].draw_x,object[i].draw_y,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {
//					int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,object[i].draw_x,object[i].draw_y);
					enemy_killed=true;
					object[i].size--;
					if (item[24]) object[i].size--;
					if (object[i].size<=0) {
						HP_add_record(map_x,map_y,i,T_ENEMY);
						player_experience[character]+=object[i].experience;
						if (item[14]) player_experience[character]+=object[i].experience;
						object[i].x+=object[i].state2;
						object[i].type=T_EXPLOSION;
						object[i].state=1;
						object[i].size=27;
						Sound_play(S_enemykill);
					} else {
						Sound_play(S_enemyhit);
					} /* if */ 
				} /* if */ 
				break;

			case T_BOUNCINGBALL:
				if (object[i].tile>=0 &&
					(tiles[object[i].base_tile+object[i].tile]->
						coltest(object[i].draw_x,object[i].draw_y,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {
					int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,object[i].draw_x,object[i].draw_y);
					enemy_killed=true;
					if (obj==-1 || current_weapon==1) object[i].last_hit=HIT_PERIOD;
					if (obj!=-1 &&
						(object[obj].type==T_WEAPON_MINE || 
						 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;
					object[i].size--;
					if (item[24]) object[i].size--;
					if (object[i].size<=0) {
						HP_add_record(map_x,map_y,i,T_ENEMY);
						player_experience[character]+=object[i].experience;
						if (item[14]) player_experience[character]+=object[i].experience;
						object[i].y+=object[i].state;
						object[i].type=T_EXPLOSION;
						object[i].state=1;
						object[i].size=27;
						Sound_play(S_enemykill);
					} else {
						Sound_play(S_enemyhit);
					} /* if */ 
				} /* if */ 
				break;

			case T_FEET:
				{
					int val=0;
					if (object[i].state3<32) val=-TILE_SIZE_Y/2;
										else val=0;
					if (object[i].tile>=0 &&
						(tiles[object[i].base_tile+object[i].tile]->
							coltest(object[i].draw_x,object[i].draw_y,
									col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
						!enemy_killed) {
						int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,object[i].draw_x,object[i].draw_y);
						enemy_killed=true;
						if (obj==-1 || current_weapon==1) object[i].last_hit=HIT_PERIOD;
						if (obj!=-1 &&
							(object[obj].type==T_WEAPON_MINE || 
							 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;
						object[i].size--;
						if (item[24]) object[i].size--;
						if (object[i].size<=0) {
							HP_add_record(map_x,map_y,i,T_ENEMY);
							player_experience[character]+=object[i].experience;
							if (item[14]) player_experience[character]+=object[i].experience;
							object[i].type=T_EXPLOSION;
							object[i].state=1;
							object[i].size=27;
							Sound_play(S_enemykill);
						} else {
							Sound_play(S_enemyhit);
						} /* if */ 
					} /* if */ 
				}
				break;


			case T_LIVINGWALL:
				{
					int val;
					if (object[i].state!=0 && 
						(tiles[livingwall_tile]->
							coltest(object[i].draw_x,object[i].draw_y,
								TILE_SIZE_X*2,TILE_SIZE_Y*3,col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
						!enemy_killed) {
						/* muro viviente tocado!! */ 
						int obj=test_if_weapon_colision(livingwall_tile,object[i].draw_x,object[i].draw_y);
						enemy_killed=true;
						if (obj==-1 || current_weapon==1) object[i].last_hit=HIT_PERIOD;
						if (obj!=-1 &&
							(object[obj].type==T_WEAPON_MINE || 
							 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;
						object[i].size--;
						if (item[24]) object[i].size--;
						if (object[i].size<=0) {
							HP_add_record(map_x,map_y,i,T_ENEMY);
							object[i].y+=TILE_SIZE_Y;
							player_experience[character]+=4;
							if (item[14]) player_experience[character]+=4;
							object[i].type=T_EXPLOSION;
							object[i].state=1;
							object[i].size=27;
							Sound_play(S_enemykill);
						} else {
							Sound_play(S_enemyhit);
						} /* if */ 
					} /* if */ 
				
					if (object[i].type==T_ENEMY) {
						if (pers_right) val=character_tile[character];
								   else val=character_tile_inv[character];

						if (object[i].state2==0) {
							if (object[i].state!=0 && 
								tiles[livingwall_tile]->
									to_tile_colision(tiles[val+pers_pos+MASK_OFFSET],
													 GAME_VIEW_X+object[i].x+TILE_UNIT,GAME_VIEW_Y+object[i].y,
													 GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y)) {
								player_energy[character]=0;
							} /* if */ 
						} else {
							if (object[i].state!=0 && 
								tiles[livingwall_tile]->
									to_tile_colision(tiles[val+pers_pos+MASK_OFFSET],
													 GAME_VIEW_X+object[i].x-TILE_UNIT,GAME_VIEW_Y+object[i].y,
													 GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y)) {
								player_energy[character]=0;
							} /* if */ 
						} /* if */ 
					} /* if */ 
				}
				break;

			case T_MEGABAT:
				if ((tiles[megabat_tile+((object[i].state2>>3)%2)]->
						coltest(object[i].draw_x,object[i].draw_y,
							TILE_SIZE_X*4,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {
					int val;
					
					enemy_killed=true;
					/* Mega murcielago tocado!! */ 
					test_if_weapon_colision(megabat_tile+((object[i].state2>>3)%2),object[i].draw_x,object[i].draw_y);
					object[i].type=T_NADA;
					Sound_play(S_enemyhit);
					val=add_object(T_ENEMY,object[i].x+TILE_SIZE_X,object[i].y,1,T_MEGABAT2,0);
					object[val].base_tile=bat_tile;
					val=add_object(T_ENEMY,object[i].x+TILE_SIZE_X*2,object[i].y,1,T_MEGABAT2,0);
					object[val].base_tile=bat_tile;
				} /* if */ 
				break;
			case T_MEGABAT2:
				if ((tiles[bat_tile+((object[i].state2>>2)%2)]->
						coltest(object[i].draw_x,object[i].draw_y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {
					int val;

					enemy_killed=true;
					/* Mega murcielago 2 tocado!! */ 
					test_if_weapon_colision(bat_tile+((object[i].state2>>2)%2),object[i].draw_x,object[i].draw_y);
					object[i].type=T_NADA;
					Sound_play(S_enemyhit);
					val=add_object(T_ENEMY,object[i].x,object[i].y,1,T_MEGABAT3,0);
					object[val].base_tile=megabat_tile;
					val=add_object(T_ENEMY,object[i].x,object[i].y,1,T_MEGABAT3,0);
					object[val].base_tile=megabat_tile;
				} /* if */ 
				break;
			case T_MEGABAT3:
				if ((tiles[megabat_tile+((object[i].state2>>2)%2)]->
						coltest(object[i].draw_x,object[i].draw_y,
							TILE_SIZE_X*4,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {

					enemy_killed=true;
					/* Mega murcielago 3 tocado!! */ 
					test_if_weapon_colision(megabat_tile+((object[i].state2>>2)%2),object[i].draw_x,object[i].draw_y);
					player_experience[character]+=2;
					if (item[14]) player_experience[character]+=2;
					object[i].type=T_EXPLOSION;
					object[i].state=1;
					object[i].size=27;
					Sound_play(S_enemykill);
				} /* if */ 
				break;

			case T_GLIZARD_TONGUE:
				{
					bool touched=false;
					int obj=-1,tmpobj;

					if (object[i].state==0) {
						int j,v;
				
						if (object[i].state2<=28) {
							v=(object[i].state2/2)+2;
							if (v>=12) v=12;
							for(j=0;j<v;j++) {
								if ((tiles[object[i].base_tile+14+j]->
										coltest(GAME_VIEW_X+object[i].x+object[i].xoffs+TILE_SIZE_X+TILE_UNIT*5,GAME_VIEW_Y+object[i].y+object[i].yoffs+TILE_SIZE_Y+TILE_UNIT*5-(v-j)*TILE_SIZE_Y,
											TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
									touched=true;
									tmpobj=test_if_weapon_colision(object[i].base_tile+14+j,
																   GAME_VIEW_X+object[i].x+object[i].xoffs+TILE_SIZE_X+TILE_UNIT*5,
																   GAME_VIEW_Y+object[i].y+object[i].yoffs+TILE_SIZE_Y+TILE_UNIT*5-(v-j)*TILE_SIZE_Y);
									if (tmpobj!=-1) obj=tmpobj;
								} /* if */ 
							} /* for */ 
						} else {
							v=((48-object[i].state2)/2)+2;
							if (v>=12) v=12;
							for(j=0;j<v;j++) {
								if ((tiles[object[i].base_tile+14+j]->
										coltest(GAME_VIEW_X+object[i].x+object[i].xoffs+TILE_SIZE_X+TILE_UNIT*5,GAME_VIEW_Y+object[i].y+object[i].yoffs+TILE_SIZE_Y+TILE_UNIT*5-(v-j)*TILE_SIZE_Y,
											TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
									touched=true;
									tmpobj=test_if_weapon_colision(object[i].base_tile+14+j,
																   GAME_VIEW_X+object[i].x+object[i].xoffs+TILE_SIZE_X+TILE_UNIT*5,
																   GAME_VIEW_Y+object[i].y+object[i].yoffs+TILE_SIZE_Y+TILE_UNIT*5-(v-j)*TILE_SIZE_Y);
									if (tmpobj!=-1) obj=tmpobj;
								} /* if */ 
							} /* for */ 
						} /* if */ 
					} else {
						int j,v;
				
						if (object[i].state2<=28) {
							v=(object[i].state2/2)+2;
							if (v>=12) v=12;
							for(j=0;j<v;j++) {
								if ((tiles[object[i].base_tile+14+(v-j)]->
										coltest(GAME_VIEW_X+object[i].x+object[i].xoffs+TILE_SIZE_X+TILE_UNIT*5,GAME_VIEW_Y+object[i].y+object[i].yoffs+TILE_SIZE_Y+TILE_UNIT*5+j*TILE_SIZE_Y,
											TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
									touched=true;
									tmpobj=test_if_weapon_colision(object[i].base_tile+14+(v-j),
																   GAME_VIEW_X+object[i].x+object[i].xoffs+TILE_SIZE_X+TILE_UNIT*5,
																   GAME_VIEW_Y+object[i].y+object[i].yoffs+TILE_SIZE_Y+TILE_UNIT*5+j*TILE_SIZE_Y);
									if (tmpobj!=-1) obj=tmpobj;
								} /* if */ 
							} /* for */ 
						} else {
							v=((48-object[i].state2)/2)+2;
							if (v>=12) v=12;
							for(j=0;j<v;j++) {
								if ((tiles[object[i].base_tile+14+(v-j)]->
										coltest(GAME_VIEW_X+object[i].x+object[i].xoffs+TILE_SIZE_X+TILE_UNIT*5,GAME_VIEW_Y+object[i].y+object[i].yoffs+TILE_SIZE_Y+TILE_UNIT*5+j*TILE_SIZE_Y,
											TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
									touched=true;
									tmpobj=test_if_weapon_colision(object[i].base_tile+14+(v-j),
																   GAME_VIEW_X+object[i].x+object[i].xoffs+TILE_SIZE_X+TILE_UNIT*5,
																   GAME_VIEW_Y+object[i].y+object[i].yoffs+TILE_SIZE_Y+TILE_UNIT*5+j*TILE_SIZE_Y);
									if (tmpobj!=-1) obj=tmpobj;
								} /* if */ 
							} /* for */ 
						} /* if */ 
					} /* if */  

					if (touched) {
						/* Lengua tocada!! */ 
						if (obj==-1 || current_weapon==1) object[i].last_hit=HIT_PERIOD;
						if (obj!=-1 &&
							(object[obj].type==T_WEAPON_MINE || 
							 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;
						object[i].size--;
						if (item[24]) object[i].size--;
						if (object[i].size<=0) {
							object[i].x+=TILE_SIZE_X;
							object[i].type=T_EXPLOSION;
							object[i].state=0;
							object[i].size=27;
							Sound_play(S_enemykill);
						} else {
							Sound_play(S_enemyhit);
						} /* if */ 
					} /* if */ 
				}
				break;

			/* WORLD DEMONS: */ 
			case T_DEMON1:
				if (object[i].size>=0 &&
					(tiles[object[i].state+6]->
						coltest(object[i].draw_x,object[i].draw_y,
							TILE_SIZE_X*10,TILE_SIZE_Y*7,col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
					/* Primer monstruo tocado: */ 
					int obj=test_if_weapon_colision(object[i].state+6,object[i].draw_x,object[i].draw_y);
					if (obj==-1 ||
						object[obj].type==T_WEAPON_ARROW) {
						if (obj==-1) object[i].last_hit=HIT_PERIOD;
						object[i].size--;
						if (object[i].size<0) {
							char str[80],str2[80];
							fighting_demon++;
							Sound_release_music();
							sprintf(str,"demon3");
							sprintf(str2,"demon4");
							Sound_create_music(str,str2,0);
						} else {
							Sound_play(S_demonhit);
						} /* if */ 
					} /* if */ 
				} /* if */ 
				break;
			case T_DEMON1_BONES:
				if ((tiles[demon_tile+12+(object[i].state2>>1)]->
						coltest(object[i].draw_x,object[i].draw_y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {

					int val;
					enemy_killed=true;
					/* Hueso tocado!! */ 
					test_if_weapon_colision(demon_tile+12+(object[i].state2>>1),object[i].draw_x,object[i].draw_y);
					object[i].type=T_EXPLOSION;
					object[i].state=0;
					object[i].size=27;
					Sound_play(S_enemykill);
					val=add_object(T_ENEMY,object[i].x,object[i].y,1,T_DEMON1_BONES2,-TILE_UNIT);
					object[val].base_tile=demon_tile;
					val=add_object(T_ENEMY,object[i].x,object[i].y,1,T_DEMON1_BONES2,TILE_UNIT);
					object[val].base_tile=demon_tile;
				} /* if */ 
				break;
			case T_DEMON1_BONES2:
				if ((tiles[demon_tile+12+(object[i].state2>>1)]->
						coltest(object[i].draw_x,object[i].draw_y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {
					/* Hueso pequeño tocado!! */ 
					enemy_killed=true;
					test_if_weapon_colision(demon_tile+12+(object[i].state2>>1),object[i].draw_x,object[i].draw_y);
					object[i].type=T_EXPLOSION;
					object[i].state=0;
					object[i].size=27;
					Sound_play(S_enemykill);
				} /* if */ 
				break;

			case T_DEMON2_HEAD:
				if (object[i].state>=0 &&
					(tiles[object[i].state]->
						coltest(object[i].draw_x,object[i].draw_y,
							TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
					/* Segundo monstruo tocado!! */ 
					int obj=test_if_weapon_colision(object[i].state,object[i].draw_x,object[i].draw_y);
					if (obj==-1 ||
						object[obj].type==T_WEAPON_ARROW) {
						if (obj==-1) object[i].last_hit=HIT_PERIOD;
						object[i].size--;
						if (object[i].size<0) {
							char str[80],str2[80];
							fighting_demon++;
							Sound_release_music();
							sprintf(str,"demon3");
							sprintf(str2,"demon4");
							Sound_create_music(str,str2,0);
						} else {
							Sound_play(S_demonhit);
						} /* if */ 
					} /* if */ 
				} /* if */ 
				break;
			case T_DEMON2_BALLS:
				if ((tiles[demon_tile+7]->
						coltest(object[i].draw_x,object[i].draw_y,
							TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					!enemy_killed) {
					enemy_killed=true;
					/* Bola Tocada!! */ 
					test_if_weapon_colision(demon_tile+7,object[i].draw_x,object[i].draw_y);
					object[i].type=T_EXPLOSION;
					object[i].state=0;
					object[i].size=27;
					Sound_play(S_enemykill);
				} /* if */ 
				break;

			case T_DEMON3:
				if (object[i].size>=0 &&
					(tiles[object[i].base_tile+object[i].tile]->
						coltest(object[i].draw_x,object[i].draw_y,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
					/* Tercer monstruo tocado: */ 
					int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,
													object[i].draw_x,object[i].draw_y);
					if (obj==-1 ||
						object[obj].type==T_WEAPON_FIRE) {
						if (obj==-1) object[i].last_hit=HIT_PERIOD;
						object[i].size--;
						if (object[i].size<0) {
							char str[80],str2[80];
							fighting_demon++;
							Sound_release_music();
							sprintf(str,"demon3");
							sprintf(str2,"demon4");
							Sound_create_music(str,str2,0);
						} else {
							Sound_play(S_demonhit);
						} /* if */ 
					} /* if */ 
				} /* if */ 
				break;

			case T_DEMON4:
				if (object[i].size>=0 &&
					(tiles[object[i].base_tile+object[i].tile]->
						coltest(object[i].draw_x,object[i].draw_y,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
					/* Cuarto monstruo tocado: */ 
					int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,
													object[i].draw_x,object[i].draw_y);
					if (obj!=-1 &&
						(object[obj].type==T_WEAPON_MINE ||
						 (object[obj].type==T_MINE_EXP && object[obj].state==0))) {
						object[i].last_hit=HIT_PERIOD*2;
						object[i].size--;
						if (object[i].size<0) {
							char str[80],str2[80];
							fighting_demon++;
							Sound_release_music();
							sprintf(str,"demon3");
							sprintf(str2,"demon4");
							Sound_create_music(str,str2,0);
						} else {
//							Sound_play(S_demonhit);
						} /* if */ 
					} /* if */ 
				} /* if */ 
				break;
				
			case T_DEMON5:
				if (object[i].size>=0 &&
					(tiles[object[i].base_tile+object[i].state3+8]->
						coltest(object[i].draw_x,object[i].draw_y+TILE_SIZE_Y,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
					/* Quinto monstruo tocado: */ 
					int obj_type;
					int obj=test_if_weapon_colision(object[i].base_tile+object[i].state3+8,
													object[i].draw_x,object[i].draw_y+TILE_SIZE_Y,&obj_type);
					if (obj==-1 ||
						(obj_type==T_WEAPON_ARROW)) {
						object[i].last_hit=HIT_PERIOD;
						object[i].size--;
						if (object[i].size<0) {
							char str[80],str2[80];
							fighting_demon++;
							Sound_release_music();
							sprintf(str,"demon3");
							sprintf(str2,"demon4");
							Sound_create_music(str,str2,0);
						} else {
							Sound_play(S_demonhit);
						} /* if */ 
					} /* if */ 
				} /* if */ 
				break;

			case T_DEMON6:
				if (object[i].size>=0 &&
					(tiles[object[i].base_tile+3]->
						coltest(object[i].draw_x,object[i].draw_y,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
					/* Sexto monstruo tocado: */ 
					int obj_type;
					int obj=test_if_weapon_colision(object[i].base_tile+3,
													object[i].draw_x,object[i].draw_y,&obj_type);
					if (obj==-1 ||
						(obj_type==T_WEAPON_ARROW) ||
						(obj_type==T_WEAPON_FIRE)) {
						object[i].last_hit=HIT_PERIOD;
						object[i].size--;
						if (object[i].size<0) {
							char str[80],str2[80];
							fighting_demon++;
							Sound_release_music();
							sprintf(str,"demon3");
							sprintf(str2,"demon4");
							Sound_create_music(str,str2,0);
						} else {
							Sound_play(S_demonhit);
						} /* if */ 
					} /* if */ 
				} /* if */ 
				break;

			case T_DEMON7:
				if (object[i].size>=0 &&
					(tiles[object[i].base_tile+object[i].tile+4]->
						coltest(object[i].draw_x,object[i].draw_y,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
					/* Septimo monstruo tocado: */ 
					int obj_type;
					int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile+4,
													object[i].draw_x,object[i].draw_y,&obj_type);
					if (obj==-1 ||
						(obj_type==T_WEAPON_ARROW2)) {
						object[i].last_hit=HIT_PERIOD*2;
						object[i].size--;
						if (object[i].size<0) {
							char str[80],str2[80];
							fighting_demon++;
							Sound_release_music();
							sprintf(str,"demon3");
							sprintf(str2,"demon4");
							Sound_create_music(str,str2,0);
						} else {
							Sound_play(S_demonhit);
						} /* if */ 
					} /* if */ 
				} /* if */ 
				break;

			case T_DEMON8:
				if (object[i].size>=0 &&
					(tiles[object[i].base_tile+object[i].tile]->
						coltest(object[i].draw_x,object[i].draw_y,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
					/* Octavo monstruo tocado: */ 
					int obj_type;
					int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile,
													object[i].draw_x,object[i].draw_y,&obj_type);
					if (obj==-1 ||
						(obj_type==T_WEAPON_RFIRE)) {
						object[i].last_hit=HIT_PERIOD;
						object[i].size--;
						if (object[i].size<0) {
							char str[80],str2[80];
							fighting_demon++;
							Sound_release_music();
							sprintf(str,"demon3");
							sprintf(str2,"demon4");
							Sound_create_music(str,str2,0);
						} else {
							Sound_play(S_demonhit);
						} /* if */ 
					} /* if */ 
				} /* if */ 
				break;

			case T_DEMON9:
				if (object[i].size>=0 &&
					(tiles[object[i].base_tile+object[i].tile+3]->
						coltest(object[i].draw_x,object[i].draw_y,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
					/* Noveno monstruo tocado: */ 
					int obj_type;
					int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile+3,
													object[i].draw_x,object[i].draw_y,&obj_type);
					if (obj==-1 ||
						(obj_type==T_WEAPON_ARROW2)) {
						object[i].last_hit=HIT_PERIOD*2;
						object[i].size--;
						if (object[i].size<0) {
							char str[80],str2[80];
							fighting_demon++;
							Sound_release_music();
							sprintf(str,"demon3");
							sprintf(str2,"demon4");
							Sound_create_music(str,str2,0);
						} else {
							Sound_play(S_demonhit);
						} /* if */ 
					} /* if */ 
				} /* if */ 
				break;

			case T_DEMON10:
				if (object[i].size>=0 &&
					object[i].tile>=0 && 
					(tiles[object[i].base_tile+object[i].tile+2]->
						coltest(object[i].draw_x,object[i].draw_y,
								col_buffer,dx,dy,dx)&T_WEAPON)!=0 &&
					cross) {
					/* Décimo monstruo tocado: */ 
					int obj_type;
					int obj=test_if_weapon_colision(object[i].base_tile+object[i].tile+2,
													object[i].draw_x,object[i].draw_y,&obj_type);

					if (obj==-1 || object[obj].type==T_WEAPON_ARROW2) object[i].last_hit=HIT_PERIOD;
					if (obj!=-1 &&
						(object[obj].type==T_WEAPON_MINE || 
						 object[obj].type==T_MINE_EXP)) object[i].last_hit=HIT_PERIOD*2;
				
					object[i].size--;
					if (object[i].size<0) {
						char str[80],str2[80];
						fighting_demon++;
						Sound_release_music();
						sprintf(str,"demon3");
						sprintf(str2,"demon4");
						Sound_create_music(str,str2,0);
					} else {
						Sound_play(S_demonhit);
					} /* if */ 
				} /* if */ 
				break;


			} /* switch */ 
		} /* if */ 

		if (object[i].type==T_HITWALL && object[i].last_hit==0) {
			int val;
			bool hit=false;
			if (map==0) val=105;
				   else val=125;
			if (sword && (character==0 || item[15]) &&
				(tiles[105]->
					coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X*4,TILE_SIZE_Y*3,col_buffer,dx,dy,dx)&T_WEAPON)!=0) {
				// int obj=-1;
				int tx,ty;
				
				for(tx=0;tx<4;tx++) {
					for(ty=0;ty<3;ty++) {
						if (pers_right) {
							if (tiles[105]->to_tile_colision(tiles[sword_tile[character]],
															 GAME_VIEW_X+tx*TILE_SIZE_X+object[i].x,GAME_VIEW_Y+ty*TILE_SIZE_Y+object[i].y,
															 GAME_VIEW_X+sword_x,GAME_VIEW_Y+sword_y)) hit=true;
						} else {
							if (tiles[105]->to_tile_colision(tiles[sword_tile[character]+1],
															 GAME_VIEW_X+tx*TILE_SIZE_X+object[i].x,GAME_VIEW_Y+ty*TILE_SIZE_Y+object[i].y,
															 GAME_VIEW_X+sword_x,GAME_VIEW_Y+sword_y)) hit=true;
						} /* if */ 

//						int tobj=test_if_weapon_colision(105,GAME_VIEW_X+object[i].x+tx*TILE_SIZE_X,GAME_VIEW_Y+object[i].y+ty*TILE_SIZE_Y);
//						if (tobj!=-1) obj=tobj;
					} /* for */ 
				} /* for */ 
				if (hit) {
					/* Muro tocado!! */ 
					object[i].last_hit=HIT_PERIOD;
					object[i].size--;
					if (object[i].size==0) {
						Sound_play(S_ladderdisapear);
						HP_add_record(map_x,map_y,i,T_HITWALL);
						object[i].type=T_NADA;
						if (map==0 && map_x==9 && map_y==11) {
							object[i].type=T_ITEM;
							object[i].state=-1;
							object[i].size=10;
							object[i].x+=TILE_SIZE_X;
							object[i].y+=TILE_SIZE_Y;
						} /* if */ 
					} else {
						Sound_play(S_swordhit);
					} /* if */ 
				} /* if */ 
			} /* if */ 		
		} /* if */ 

		if (object[i].type==T_WEAPON_ARROW ||
			object[i].type==T_WEAPON_ARROW2) {
			int colision=T_WALL|T_LADDER_WALL|T_DOOR_WALL;
			int res,t=0;
			if (object[i].type==T_WEAPON_ARROW2) t++;
			if (object[i].state!=0) t+=6;

			res=(tiles[armas_tile+t]->
					coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
					TILE_SIZE_X*2,TILE_SIZE_Y,col_buffer,dx,dy,dx)&colision);
			if (res!=0 && res!=(T_WALL|T_LADDER_WALL)) {
//				if (!todelete[i]) n_fired_arrows--;
				todelete[i]=true;
			} /* if */ 			   
		} /* if */ 

		if (object[i].type==T_WEAPON_FIRE) {
			int colision=T_WALL|T_LADDER_WALL|T_DOOR_WALL;
			int t=3;
			if (object[i].state!=0) t=8;
			if ((tiles[armas_tile+t]->
					coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
					TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&colision)!=0) {
//				if (!todelete[i]) n_fired_arrows--;
				todelete[i]=true;
			} /* if */ 			   
			if ((tiles[armas_tile]->
					coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y/2,
					TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&colision)==0) {
				object[i].y+=TILE_SIZE_Y/2;
				if (object[i].state==0) object[i].x-=TILE_SIZE_X/2;
								   else object[i].x+=TILE_SIZE_X/2;
			} else {
			} /* if */ 
		} /* if */ 

		if (object[i].type==T_WEAPON_RFIRE) {
//			int colision=T_WALL|T_LADDER_WALL|T_DOOR_WALL;
			int tx,ty;
			int n_tx,n_ty;
			tx=object[i].x/TILE_SIZE_X;
			ty=object[i].y/TILE_SIZE_Y;
			if (object[i].x==tx*TILE_SIZE_X &&
				object[i].y==ty*TILE_SIZE_Y) {
				n_tx=tx;
				n_ty=ty;
				switch(object[i].state) {
				case 0:n_tx++;
					  break;
				case 1:n_tx--;
					  break;
				case 2:n_ty--;
					  break;
				case 3:n_ty++;
					  break;
				} /* switch */ 
				
				if (iswall(n_tx,n_ty) && 
					n_tx>=0 && n_tx<32 &&
					n_ty>=0 && n_ty<20) {
					switch(object[i].state) {
					case 0:
					case 1:
					case 2:
//						if (!todelete[i]) n_fired_arrows--;
						todelete[i]=true;
						break;
					case 3:
						if (iswall(tx+1,n_ty)) {
							object[i].state=0;
						} else {
							object[i].state=1;
						} /* if */ 
						break;
					} /* switch */ 
				} else {
					switch(object[i].state) {
					case 0:
						if (iswall(tx-1,ty+1) && !iswall(tx,ty+1)) {
							object[i].state=3;
						} else {
							if (iswall(tx-1,ty-1) && !iswall(tx,ty-1)) {
								object[i].state=2;
							} /* if */ 
						} /* if */ 
					break;
					case 1:
						if (iswall(tx+1,ty+1) && !iswall(tx,ty+1)) {
							object[i].state=3;
						} else {
							if (iswall(tx+1,ty-1) && !iswall(tx,ty-1)) {
								object[i].state=2;
							} /* if */ 
						} /* if */ 
					break;
					case 2:
						if (iswall(tx-1,ty+1) && !iswall(tx-1,ty)) {
							object[i].state=1;
						} else {
							if (iswall(tx+1,ty+1) && !iswall(tx+1,ty)) {
								object[i].state=0;
							} /* if */ 
						} /* if */ 
					break;
					case 3:
						if (iswall(tx-1,ty-1) && !iswall(tx-1,ty)) {
							object[i].state=1;
						} else {
							if (iswall(tx+1,ty-1) && !iswall(tx+1,ty)) {
								object[i].state=0;
							} /* if */ 
						} /* if */ 
					break;
					} /* switch */ 
					if (!iswall(tx+1,ty) &&
						!iswall(tx-1,ty) &&
						!iswall(tx,ty+1) &&
						!iswall(tx,ty-1) &&
						!iswall(tx+1,ty+1) &&
						!iswall(tx+1,ty-1) &&
						!iswall(tx-1,ty+1) &&
						!iswall(tx-1,ty-1)) {
						object[i].state=3;
					} /* if */ 

				} /* if */ 			   
			} /* if */ 
		} /* if */ 


		if (object[i].type==T_WEAPON_MINE) {
			if ((tiles[armas_tile+9]->
					coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
					col_buffer,dx,dy,dx)&T_PLAYER)!=0) {
				if (object[i].state2==1) {
					/* Personaje ha pisado una mina: */ 
					Sound_play(S_mine);
					mine_stepped=true;
					object[i].x-=TILE_SIZE_X;
					object[i].y-=TILE_SIZE_Y*2+TILE_UNIT;
					object[i].type=T_MINE_EXP;
					object[i].state=0;
					n_fired_mines--;
				} /* if */ 
			} else {
				object[i].state2=1;
			} /* if */ 
		} /* if */ 

		if (object[i].type==T_MINE_EXP) {
			if ((tiles[armas_tile+12]->
					coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
					col_buffer,dx,dy,dx)&T_PLAYER)!=0) {
				if ((object[i].state%8)==0) {
					player_energy[character]-=2;
				} /* if */ 
			} else {
				object[i].state2=1;
			} /* if */ 
		} /* if */ 
	} /* for */ 


	for(i=0;i<n_objects;i++) {
		switch(object[i].type) {
		case T_KEYDOOR:
			if (object[i].state>=0 &&
				object[i].state<=24) {
				if ((((tiles[keydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0 ||
					  (tiles[keydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0 ||
					  (tiles[keydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0) &&
						pers_state==S_WALKING_LEFT) ||
					(((tiles[keydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0 ||
					  (tiles[keydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0 ||
					  (tiles[keydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0) &&
						pers_state==S_WALKING_RIGHT)) {
					/* Player tocando puerta: */ 
					if (n_keys>0) {
						object[i].state++;
						if (object[i].state>=24) {
							Sound_play(S_door2);
							object[i].state=-28;
							n_keys--;
							pers_state=S_DOORPASS;
							pers_substate=0;
						} /* if */ 
					} else {
						object[i].state=0;
					} /* if */ 
				} else {
					object[i].state=0;
				} /* if */ 
			} else {
				object[i].state++;
			} /* if */ 
			break;

		case T_WKEYDOOR:
			if (object[i].state>=0 &&
				object[i].state<=24) {
				if ((((tiles[wkeydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0 ||
					  (tiles[wkeydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0 ||
					  (tiles[wkeydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0) &&
						pers_state==S_WALKING_LEFT) ||
					(((tiles[wkeydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0 ||
					  (tiles[wkeydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0 ||
					  (tiles[wkeydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0) &&
						pers_state==S_WALKING_RIGHT)) {
					/* Player tocando puerta: */ 
					if (n_keys>0) {
						object[i].state++;
						if (object[i].state>=24) {
							Sound_play(S_door);
							object[i].state=-28;
							if (pers_x>object[i].x) object[i].state2=1;
											   else object[i].state2=0;
							n_keys--;
							pers_state=S_DOORPASS;
							pers_substate=0;
						} /* if */ 
					} else {
						object[i].state=0;
					} /* if */ 
				} else {
					object[i].state=0;
				} /* if */ 
			} else {
				object[i].state++;
			} /* if */ 
			break;

		case T_WNOKEYDOOR:
		case T_WNOKEYDOOR2:
		case T_WNOKEYDOOR3:
			if (object[i].state>=0 &&
				object[i].state<=24) {
				if ((((tiles[wnokeydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0 ||
					  (tiles[wnokeydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0 ||
					  (tiles[wnokeydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x+TILE_UNIT*2,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0) &&
						pers_state==S_WALKING_LEFT) ||
					(((tiles[wnokeydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0 ||
					  (tiles[wnokeydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0 ||
					  (tiles[wnokeydoor_tile]->
						coltest(GAME_VIEW_X+object[i].x-TILE_UNIT*2,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y,
						TILE_SIZE_X,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0) &&
						pers_state==S_WALKING_RIGHT)) {
					/* Player tocando puerta: */ 
					if (character==0 ||
						item[28]) {
						object[i].state++;
						if (object[i].state>=24) {
							object[i].state=-24;
							if (pers_x>object[i].x) object[i].state2=1;
											   else object[i].state2=0;
							pers_state=S_SLOWDOORPASS;
							pers_substate=0;
							Sound_play(S_rotatedoor);
						} /* if */ 
						if (object[i].type==T_WNOKEYDOOR2 && pers_state==S_WALKING_RIGHT) object[i].state=0;
						if (object[i].type==T_WNOKEYDOOR3 && pers_state==S_WALKING_LEFT) object[i].state=0;
					} else {
						object[i].state=0;
					} /* if */ 
				} else {
					object[i].state=0;
				} /* if */ 
			} else {
				object[i].state++;
			} /* if */ 
			break;

		case T_WDOOR:
			{
				int in_wdoor=10;
				if (map_x==3 && map_y==12) in_wdoor=1;
				if (map_x==10 && map_y==11) in_wdoor=2;
				if (map_x==10 && map_y==8) in_wdoor=3;
				if (map_x==0 && map_y==8) in_wdoor=4;
				if (map_x==8 && map_y==8) in_wdoor=5;
				if (map_x==14 && map_y==12) in_wdoor=6;
				if (map_x==5 && map_y==2) in_wdoor=7;
				if (map_x==4 && map_y==4) in_wdoor=8;
				if (map_x==7 && map_y==0) in_wdoor=9;
				if (map_x==world10_door_x && map_y==world10_door_y) in_wdoor=10;

				if (world_key && in_wdoor==next_world) {
					if (pers_x>=object[i].x && pers_x<=(object[i].x+2*TILE_SIZE_X) &&
						pers_y==object[i].y+TILE_SIZE_Y) {
						Sound_play(S_wdoor);
						world_doors_open[next_world-1]=true;
						object[i].state=1; 
						world_key=false;
					} /*  if */ 
				} /* if*/ 

				if (object[i].state==1 &&
					pers_x>=(object[i].x+TILE_SIZE_X-4*TILE_UNIT) && pers_x<=(object[i].x+TILE_SIZE_X+4*TILE_UNIT) &&
					pers_y==object[i].y+TILE_SIZE_Y && keyboard[DOWN_KEY]) {
					bool forbiden=false;

					if (in_wdoor==5 && !item[22] && character==1) forbiden=true;
					if (in_wdoor==7 && !item[0] && character==0) forbiden=true;
					if (!forbiden && pers_state!=S_ENTERING_WORLD) {
						pers_substate=0;
						pers_state=S_ENTERING_WORLD;
						in_door=i;
					} /* if */ 
				} /*  if */ 			
			}
			break;

		case T_DOOR:
			{
				bool need_salt=false;

				if (pers_x>=(object[i].x-4*TILE_UNIT) && pers_x<=(object[i].x+4*TILE_UNIT) &&
					pers_y==object[i].y && keyboard[DOWN_KEY] && !old_keyboard[DOWN_KEY] && to_enter_cut==0 &&
					pers_state!=S_DEATH) {
					if (map_x==11 && map_y==10 && map==0) need_salt=true;

					if (!need_salt || item[29]) {
						pers_substate=0;
						pers_state=S_ENTERING_PASSAGE;
						in_door=i;
					} /* if */ 
				} /*  if */ 
			}

			break;

		case T_ITEM:
			switch(object[i].state) {
			case -1:
				if ((tiles[coin_tile]->
						coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_PLAYER)!=0) {
					/* Monedas cogida: */ 
					Sound_play(S_item);
					object[i].type=T_NADA;
					n_coins+=object[i].size;
					if (object[i].size==10) HP_modify_record_aux(map_x,map_y,i,1);
				} /* if */ 
				break;
			case -2:
				if ((tiles[key_tile]->
						coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_PLAYER)!=0) {
					/* Llaves cogida: */ 
					Sound_play(S_item);
					object[i].type=T_NADA;
					n_keys+=object[i].size;
					if (object[i].size==2) HP_modify_record_aux(map_x,map_y,i,1);
				} /* if */ 
				break;
			case -3:
				if ((tiles[arrow_tile]->
						coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_PLAYER)!=0) {
					/* Flechas cogida: */ 
					Sound_play(S_item);
					object[i].type=T_NADA;
					n_arrows+=object[i].size;
					if (object[i].size==50) HP_modify_record_aux(map_x,map_y,i,1);
				} /* if */ 
				break;

			default:
				/* Efecto de coger un objeto cualquiera: */ 
				if (object[i].state>=0 && object[i].state<32) {
					if ((tiles[items_tile+object[i].state]->
						coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_PLAYER)!=0) {	
						Sound_play(S_item);
						object[i].type=T_NADA;
						item[object[i].state]=true;
					} /* if */ 
				} /* if */ 
				if (object[i].state==36) {
					if ((tiles[items_tile+object[i].state]->
						coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_PLAYER)!=0) {	
						Sound_play(S_item);
						object[i].type=T_NADA;
						cross=true;
					} /* if */ 
				} /* if */ 
				/* Efecto de coger un arma: */ 
				if (object[i].state>=32 && object[i].state<36) {
					if ((tiles[armas_tile+object[i].state-32]->
						coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X*2,TILE_SIZE_Y,col_buffer,dx,dy,dx)&T_PLAYER)!=0) {	
						Sound_play(S_item);
						object[i].type=T_NADA;
						item[object[i].state]=true;
					} /* if */ 
				} /* if */ 
				/* Efecto de coger una llave de mundo: */ 
				if (object[i].state==37) {
					if ((tiles[items_tile+37]->
						coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_PLAYER)!=0 &&
						fighting_demon<196) {	
						char tmp[80],tmp2[80],tmp3[80];

						world_key=true;
						player_max[character]+=16;
						player_energy[character]=player_max[character];

						if (map==0) {
							Sound_release_music();
							sprintf(tmp2,"worldkey");
							sprintf(tmp,"castle");
							Sound_create_music(tmp2,tmp,0);
							next_world=1;
							object[i].type=T_NADA;
						} else {
							Sound_release_music();
							sprintf(tmp,"worldkey");
							sprintf(tmp2,"worldkey2");
							sprintf(tmp3,"world");
							Sound_create_music(tmp,tmp2,tmp3);
							fighting_demon++;
							next_world=map+1;
						} /* if */ 
					} /* if */ 
				} /* if */ 
				/* Efecto de coger objeto de mundo: */ 
				if (object[i].state>=38) {
					if ((tiles[items_tile+object[i].state-6]->
						coltest(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&T_PLAYER)!=0) {	
						world_item[map-1][object[i].state-38]=true;
						Sound_play(S_item);
						object[i].type=T_NADA;
					} /* if */ 
				} /* if */ 
				break;
			} /* switch */ 
		} /* switch */ 
	} /* for */ 

	/* Reset the typed word: */ 
	if (item[25] && typed_word_p("umbrella")) {
		clear_typed_word();
	} /* if */ 


	for(i=0;i<MAX_OBJECTS;i++) {
		if (todelete[i]) {
			if (object[i].type==T_WEAPON_ARROW ||
				object[i].type==T_WEAPON_ARROW2 ||
				object[i].type==T_WEAPON_FIRE ||
				object[i].type==T_WEAPON_RFIRE) n_fired_arrows--;
			object[i].type=T_NADA;
		} /* if */ 
	} /* for */ 

	if (n_coins>999) n_coins=999;
	if (n_arrows>999) n_arrows=999;
	if (n_keys>999) n_keys=999;
} /* GameTestInteractions */ 


void GameRoomEvents()
{
	unsigned char *keyboard;

	SDL_PumpEvents();
	keyboard = (unsigned char *)SDL_GetKeyState(NULL);

	if (room_slimes) {
		/* ORUGAS: */ 
		if ((rand()%200)==0 && slime_counter==0) {
			int direction=rand()%2;
			int pos;
			int val;

			if (direction==1) pos=rand()%(14*TILE_SIZE_X);
						 else pos=(16*TILE_SIZE_X)+(rand()%(14*TILE_SIZE_X));
			
			pos/=TILE_UNIT;
			pos*=TILE_UNIT;

			val=add_object(T_ENEMY,pos,-2*TILE_SIZE_Y,2,T_SLIME,direction);
			object[val].base_tile=slime_tile;
			slime_counter=128;
		} else {
			if (slime_counter>0) {
				slime_counter--;
			} else {
				slime_counter=0;
			} /* if */ 
		} /* if */ 

	} /* if */ 

	if (room_watermonster) {
		if (watermonster_state==0 && !in_water) {
			int val;

			Sound_play(S_appearing);
			watermonster_state=1;
			val=add_object(T_ENEMY,4*TILE_SIZE_X+((rand()%21)*TILE_SIZE_X),15*TILE_SIZE_Y,
					   4,T_WATERMONSTER,0);
			object[val].base_tile=watermonster_tile;
		} /* if */ 
		if (watermonster_state<0 && !in_water) watermonster_state++;
	} /* if */ 

	if (room_fallingstones) {
		/* Piedras que caen: */ 
		if ((rand()%150)==0 && stones_counter==0) {
			int direction=rand()%2;
			int pos;
			int val;

			if (direction==1) pos=rand()%(14*TILE_SIZE_X);
						 else pos=(16*TILE_SIZE_X)+(rand()%(14*TILE_SIZE_X));
			
			pos/=TILE_UNIT;
			pos*=TILE_UNIT;

			val=add_object(T_ENEMY,pos,-2*TILE_SIZE_Y,0,T_FSTONE,direction);
			object[val].base_tile=fstone_tile;
			stones_counter=128;
		} else {
			if (stones_counter>0) {
				stones_counter--;
			} else {
				stones_counter=0;
			} /* if */ 
		} /* if */ 
	} /* if */ 

	if (room_rollingstones) {
		/* Piedras que caen: */ 
		if ((rand()%150)==0 && stones_counter==0) {
			int direction=rand()%2;
			int pos;
			int val;

			if (direction==1) pos=rand()%(14*TILE_SIZE_X);
						 else pos=(16*TILE_SIZE_X)+(rand()%(14*TILE_SIZE_X));
			
			pos/=TILE_UNIT;
			pos*=TILE_UNIT;

			val=add_object(T_ENEMY,pos,-2*TILE_SIZE_Y,0,T_RSTONE,direction);
			object[val].base_tile=fstone_tile;
			stones_counter=192;
		} else {
			if (stones_counter>0) {
				stones_counter--;
			} else {
				stones_counter=0;
			} /* if */ 
		} /* if */ 
	} /* if */ 

	if (room_blobs) {
		if ((rand()%250)==0) {
			int direction=rand()%2;
			int pos;
			int val;

			if (direction==1) pos=rand()%(14*TILE_SIZE_X);
						 else pos=(16*TILE_SIZE_X)+(rand()%(14*TILE_SIZE_X));
			
			pos/=TILE_UNIT;
			pos*=TILE_UNIT;

			val=add_object(T_ENEMY,pos,20*TILE_SIZE_Y,1,T_BLOB,direction);
			object[val].base_tile=blob_tile;
		} /* if */ 
	} /* if */ 


	if (room_appearwall!=-1) {
		int i;
		
		/* Que aparezca un muro: */ 
		switch(room_appearwall) {
		case 0:if (pers_y>=(room_appearwall_y-1)*TILE_SIZE_Y &&
				   pers_y<=(room_appearwall_y+2)*TILE_SIZE_Y &&
				   pers_x<(room_appearwall_x-3)*TILE_SIZE_X) room_appearwall=1;
			   if (pers_y>=(room_appearwall_y-1)*TILE_SIZE_Y &&
				   pers_y<=(room_appearwall_y+2)*TILE_SIZE_Y &&
				   pers_x>(room_appearwall_x+2)*TILE_SIZE_X) room_appearwall=2;
			break;
		case 1: if (pers_x>=(room_appearwall_x+2)*TILE_SIZE_X) room_appearwall=3;
				if (pers_y<(room_appearwall_y-1)*TILE_SIZE_Y ||
					pers_y>(room_appearwall_y+2)*TILE_SIZE_Y) room_appearwall=0;
				
			break;
		case 2: if (pers_x<=(room_appearwall_x-3)*TILE_SIZE_X) room_appearwall=3;
				if (pers_y<(room_appearwall_y-1)*TILE_SIZE_Y ||
					pers_y>(room_appearwall_y+2)*TILE_SIZE_Y) room_appearwall=0;			
			break;
		default:
			if (room_appearwall<26) room_appearwall++;

			room_tiles[room_appearwall_y*room_size_x+room_appearwall_x]=wall_tile;
			if (room_appearwall>=12) room_tiles[(room_appearwall_y+1)*room_size_x+room_appearwall_x]=wall_tile;
			if (room_appearwall>=26) {
				bool found;

				room_tiles[(room_appearwall_y+2)*room_size_x+room_appearwall_x]=wall_tile;

				for(found=false,i=0;i<MAX_AW_RECORD && !found;i++) {
					if (appearwall_record_x[i]==map_x &&
						appearwall_record_y[i]==map_y) {
						found=true;
					} /* if */ 
				} /* for */ 
				if (!found) {
					for(i=0;i<MAX_AW_RECORD && !found;i++) {
						if (appearwall_record_x[i]==-1 &&
							appearwall_record_y[i]==-1) {
							appearwall_record_x[i]=map_x;
							appearwall_record_y[i]=map_y;
							found=true;
						} /* if */ 
					} /* for */ 
				} /* if */ 
			} /* if */ 
			break;
		} /* switch */ 

		for(i=0;i<MAX_AW_RECORD;i++) {
			if (appearwall_record_x[i]==map_x &&
				appearwall_record_y[i]==map_y) {

				room_tiles[room_appearwall_y*room_size_x+room_appearwall_x]=wall_tile;
				room_tiles[(room_appearwall_y+1)*room_size_x+room_appearwall_x]=wall_tile;
				room_tiles[(room_appearwall_y+2)*room_size_x+room_appearwall_x]=wall_tile;
				room_appearwall=-1;
			} /* if */ 
		} /* for */ 
	} /* if */ 


	if (room_fairy!=-1) {
		if (pers_state==S_QUIET &&
			pers_x>=fairy_x-TILE_SIZE_X/2 && pers_y<=fairy_y+TILE_SIZE_X/2 &&
			pers_y==fairy_y) {
			if (room_fairy<128) room_fairy++;
		} else {
			if (room_fairy<128)	room_fairy=0;
		} /* if */ 

		if (room_fairy>=128) {
			/* Que aparezca la fairy!! */ 
			int i;
			bool any_bubble;
			if (room_fairy==128) {
				char tmp[256],tmp2[256];
				Sound_release_music();
				sprintf(tmp2,"fairy");
				sprintf(tmp,"castle");
				Sound_create_music(tmp2,tmp,0);
			} /* if */ 

			room_fairy++;
			any_bubble=false;

			/* Desviar todas las burbujas hacia el centro: */ 

			for(i=0;i<n_objects;i++) {
				if (object[i].type==T_FAIRY_BUBBLE) {
					any_bubble=true;
					if (object[i].x>=13*TILE_SIZE_X && object[i].x<17*TILE_SIZE_X &&
						object[i].y>=9*TILE_SIZE_X && object[i].y<13*TILE_SIZE_Y) {
						object[i].type=T_NADA;
					} else {
						int dx,dy;

						dx=15*TILE_SIZE_X-object[i].x;
						dy=11*TILE_SIZE_Y-object[i].y;

						if (abs(dx)>4*abs(dy)) {
							object[i].state=(dx>0 ? 4:-4);
							object[i].state2=0;
						} else {
							if (abs(dx)>2*abs(dy)) {
								object[i].state=(dx>0 ? 4:-4);
								object[i].state2=(dy>0 ? 2:-2);
							} else {
								if (abs(dy)>2*abs(dx)) {
									if (abs(dy)>4*abs(dx)) {
										object[i].state=0;
										object[i].state2=(dy>0 ? 2:-4);
									} else {
										object[i].state=(dx>0 ? 2:-2);
										object[i].state2=(dy>0 ? 4:-4);
									} /* if */ 
								} else {
									object[i].state=(dx>0 ? 4:-4);
									object[i].state2=(dy>0 ? 4:-4);
								} /* if */ 
							} /* if */ 
						} /* if */ 
						object[i].x+=object[i].state;
						object[i].y+=object[i].state2;

					} /* if */ 
				} /* if */ 
			} /* for */ 

			if (room_fairy==153) {
				add_object(T_FAIRY,15*TILE_SIZE_X,11*TILE_SIZE_Y,0,0,0);
			} /* if */ 

			if (!any_bubble && room_fairy>=153) {
				room_fairy=-1;
			} /* if */ 

		} /* if */ 

	} /* if */ 


	if (room_lava && !item[11]) {
		/* Fuego que sale de la lava: */ 
		if ((rand()%200)==0) {
			int i;
			int xtmp;
			/* Sale una bola de lava: */ 
			xtmp=2*TILE_SIZE_X+(rand()%(TILE_SIZE_X*22));
			if (map==10) {
				i=add_object(T_ENEMY,xtmp,17*TILE_SIZE_Y,0,T_LAVA1,0);
				object[i].base_tile=lava_tile;
				i=add_object(T_ENEMY,xtmp,17*TILE_SIZE_Y,0,T_LAVA2,0);
				object[i].base_tile=lava_tile;
			} else {
				i=add_object(T_ENEMY,xtmp,16*TILE_SIZE_Y,0,T_LAVA1,0);
				object[i].base_tile=lava_tile;
				i=add_object(T_ENEMY,xtmp,16*TILE_SIZE_Y,0,T_LAVA2,0);
				object[i].base_tile=lava_tile;
			} /* if */ 
			Sound_play(S_fireball);
		} /* if */ 

	} /* if */ 

	if (room_piranhas) {
		if (piranhas_counter>0) piranhas_counter--;
		if (!in_water && (rand()%100)==0 && piranhas_counter==0) {
			int i;
			int xtmp;
			/* Sale una piraña: */ 
			piranhas_counter=200;
			xtmp=2*TILE_SIZE_X+(rand()%(TILE_SIZE_X*22));
			i=add_object(T_ENEMY,xtmp,14*TILE_SIZE_Y,1,T_PIRANHA,(xtmp<16*TILE_SIZE_X ? 0 : 2));
			object[i].base_tile=piranha_tile;
			object[i].last_hit=0;
			object[i].experience=2;
			i=add_object(T_ENEMY,xtmp,14*TILE_SIZE_Y,0,T_PIRANHA2,0);
			object[i].base_tile=piranha_tile;
		} /* if */ 
	} /* if */ 

	if (room_waterdragons!=-1) {
		if (room_waterdragons>0) room_waterdragons--;
		if (room_waterdragons==0) {
			if (!in_water) {
				int i;
				int xtmp,ttmp;
				/* Sale un waterdragon: */ 
				xtmp=2*TILE_SIZE_X+(rand()%(TILE_SIZE_X*22));
				ttmp=(xtmp<16*TILE_SIZE_X ? 0 : 1);
				i=add_object(T_ENEMY,xtmp,14*TILE_SIZE_Y,4,T_WATERDRAGON,ttmp);
				object[i].base_tile=waterdragon_tile;
				object[i].tile=ttmp*2;
				object[i].last_hit=0;
				object[i].experience=6;
				i=add_object(T_ENEMY,xtmp+TILE_SIZE_X,14*TILE_SIZE_Y,0,T_PIRANHA2,0);
				object[i].base_tile=piranha_tile;
			} /* if */ 
			room_waterdragons=320;
		} /* if */ 
	} /* if */ 

	if (room_bfly==1) {
		int i;

		i=add_object(T_ENEMY,4*TILE_SIZE_X,6*TILE_SIZE_Y,1,T_BFLY_SMALL,0);
		object[i].base_tile=bfly_tile;
		object[i].last_hit=0;
		object[i].experience=2;
		i=add_object(T_ENEMY,4*TILE_SIZE_X,12*TILE_SIZE_Y,1,T_BFLY_SMALL,0);
		object[i].base_tile=bfly_tile;
		object[i].last_hit=0;
		object[i].experience=2;
		i=add_object(T_ENEMY,12*TILE_SIZE_X,6*TILE_SIZE_Y,1,T_BFLY_SMALL,0);
		object[i].base_tile=bfly_tile;
		object[i].last_hit=0;
		object[i].experience=2;
		i=add_object(T_ENEMY,12*TILE_SIZE_X,12*TILE_SIZE_Y,1,T_BFLY_SMALL,0);
		object[i].base_tile=bfly_tile;
		object[i].last_hit=0;
		object[i].experience=2;
		i=add_object(T_ENEMY,20*TILE_SIZE_X,6*TILE_SIZE_Y,1,T_BFLY_SMALL,0);
		object[i].base_tile=bfly_tile;
		object[i].last_hit=0;
		object[i].experience=2;
		i=add_object(T_ENEMY,20*TILE_SIZE_X,12*TILE_SIZE_Y,1,T_BFLY_SMALL,0);
		object[i].base_tile=bfly_tile;
		object[i].last_hit=0;
		object[i].experience=2;
		i=add_object(T_ENEMY,24*TILE_SIZE_X,6*TILE_SIZE_Y,1,T_BFLY_SMALL,0);
		object[i].base_tile=bfly_tile;
		object[i].last_hit=0;
		object[i].experience=2;
		i=add_object(T_ENEMY,24*TILE_SIZE_X,12*TILE_SIZE_Y,1,T_BFLY_SMALL,0);
		object[i].base_tile=bfly_tile;
		object[i].last_hit=0;
		object[i].experience=2;
		room_bfly=2;
	} /* if */ 

	if (room_birds>=0) {
		int i,j;
		if (room_birds==1) {
			for(j=0;j<5;j++) {
				i=add_object(T_ENEMY,22*TILE_SIZE_X,-2*TILE_SIZE_Y,2,T_BIRD,0);
				object[i].base_tile=bird_tile;
				object[i].tile=-1;
				object[i].last_hit=0;
				object[i].experience=4;
				object[i].state2=16+j*16;
				object[i].state3=1;
			} /* if */ 
			for(j=0;j<5;j++) {
				i=add_object(T_ENEMY,8*TILE_SIZE_X,-2*TILE_SIZE_Y,2,T_BIRD,0);
				object[i].base_tile=bird_tile;
				object[i].tile=-1;
				object[i].last_hit=0;
				object[i].experience=4;
				object[i].state2=16+(j+5)*16;
				object[i].state3=1;
			} /* if */ 
		} else {
			for(j=0;j<5;j++) {
				i=add_object(T_ENEMY,8*TILE_SIZE_X,-2*TILE_SIZE_Y,2,T_BIRD,0);
				object[i].base_tile=bird_tile;
				object[i].tile=-1;
				object[i].last_hit=0;
				object[i].experience=4;
				object[i].state2=16+j*16;
				object[i].state3=1;
			} /* if */ 
		} /* if */ 
		room_birds=-1;
	} /* if */ 

	if (room_reddragon==1) {
		int i;

		i=add_object(T_ENEMY,0,0,25,T_REDDRAGON,0);
		object[i].base_tile=reddragon_tile;
		object[i].tile=-1;
		object[i].experience=16;
		room_reddragon=0;
	} /* if */ 

	if (map==0 && map_x==8 && map_y==9) {
		int i;
		bool vela=true;

		if (item[9]) vela=false;

		for(i=0;vela && i<n_objects;i++) {
			if (object[i].type==T_ENEMY || 
				((object[i].type==T_STONE || object[i].type==T_BROKEN_STONE ||
				object[i].type==T_ITEM) &&
				object[i].x==4*TILE_SIZE_X && object[i].y==17*TILE_SIZE_Y)) {
				vela=false;
			} /* if */ 
		} /* for */ 

		if (vela) {
			Sound_play(S_ladderdisapear);
			add_object(T_ITEM,4*TILE_SIZE_X,17*TILE_SIZE_Y,0,T_ITEM,9);
		} /* if */ 
	} /* if */ 

	if (map==0 && map_x==10 && map_y==6) {
		int i;
		bool habito=true;

		if (item[6]) habito=false;

		for(i=0;habito && i<n_objects;i++) {
			if (object[i].type==T_ENEMY ||
				object[i].type==T_ITEM && 
				object[i].state==6) habito=false;
		} /* for */ 

		if (habito) {
			Sound_play(S_ladderdisapear);
			add_object(T_ITEM,10*TILE_SIZE_X,TILE_SIZE_Y,0,T_ITEM,6);
		} /* if */ 
	} /* if */ 

	if (map==0 && map_x==2 && map_y==6 && next_world>=5) {
		if (pers_x<TILE_SIZE_X*19 || pers_x>=TILE_SIZE_X*23 ||
			pers_y<12*TILE_SIZE_Y) {
			room26_state=0;
		} else {
			if (room26_state==0 && pers_state==S_QUIET_SWORD && pers_right) room26_state=1;
			if (room26_state==1 && pers_state==S_QUIET && pers_right) room26_state=2;
			if (room26_state==2 && pers_state==S_QUIET_SWORD && pers_right) room26_state=3;
			if (room26_state==3 && pers_state==S_QUIET && pers_right) room26_state=4;
			if (room26_state==4 && pers_state==S_QUIET_SWORD && pers_right) room26_state=5;
			if (room26_state==5 && pers_state==S_QUIET && pers_right) {
				room26_state=6;
			} /* if */ 
			if (room26_state<=5 && pers_state!=S_QUIET && pers_state!=S_QUIET_SWORD) room26_state=0;

			if (room26_state==6 && pers_state==S_QUIET_SWORD && !pers_right) room26_state=7;
			if (room26_state==7 && pers_state==S_QUIET && !pers_right) room26_state=8;
			if (room26_state==8 && pers_state==S_QUIET_SWORD && !pers_right) room26_state=9;
			if (room26_state==9 && pers_state==S_QUIET && !pers_right) room26_state=10;
			if (room26_state==10 && pers_state==S_QUIET_SWORD && !pers_right) room26_state=11;
			if (room26_state==11 && pers_state==S_QUIET && !pers_right) {
				room26_state=12;
			} /* if */ 
			if (room26_state>6 && room26_state<=11 && pers_state!=S_QUIET && pers_state!=S_QUIET_SWORD) room26_state=0;

			if (room26_state==12 && pers_state==S_JUMPING) room26_state=13;
			if (room26_state==13 && pers_state==S_QUIET) room26_state=14;
			if (room26_state==14 && pers_state==S_JUMPING) room26_state=15;
			if (room26_state==15 && pers_state==S_QUIET) room26_state=16;
			if (room26_state==16 && pers_state==S_JUMPING) room26_state=17;
			if (room26_state==17 && pers_state==S_QUIET) room26_state=18;
			if (room26_state>=12 && 
				pers_state!=S_QUIET && 
				pers_state!=S_JUMPING && 
				pers_state!=S_CONTROLED_FALL &&
				pers_state!=S_STOPPED_JUMPING) room26_state=0;

			if (room26_state==18) {
				room26_state=0;
				int i;
				bool vase=true;

				if (item[14]) vase=false;

				for(i=0;vase && i<n_objects;i++) {
					if (object[i].type==T_ITEM && 
						object[i].state==14) vase=false;
				} /* for */ 

				if (vase) {
					Sound_play(S_ladderdisapear);
					add_object(T_ITEM,16*TILE_SIZE_X,13*TILE_SIZE_Y,0,T_ITEM,14);
				} /* if */ 

			} /* if */ 
		} /* if */ 
	} /* if */ 


	if (map==0 && map_x==5 && map_y==6) {
		if (pers_x>=4*TILE_SIZE_X && pers_x<=26*TILE_SIZE_X &&
			pers_y==9*TILE_SIZE_Y && room56_state<4) {
			if (room56_state==0 && keyboard[RIGHT_KEY] && !keyboard[DOWN_KEY]) room56_state=1;
			if (room56_state==1 && keyboard[DOWN_KEY] &&  !keyboard[LEFT_KEY]) room56_state=2;
			if (room56_state==2 && keyboard[LEFT_KEY] && !keyboard[UP_KEY]) room56_state=3;
			if (room56_state==3 && keyboard[UP_KEY]) room56_state=4;
		} else {
			if (room56_state==4) { 
				int i;
				bool dagger=true;

				if (item[25]) dagger=false;

				for(i=0;dagger && i<n_objects;i++) {
					if (object[i].type==T_STONE ||
						object[i].type==T_BROKEN_STONE ||
						(object[i].type==T_ITEM && 
						 object[i].state==25)) dagger=false;
				} /* for */ 

				if (dagger) {
					Sound_play(S_ladderdisapear);
					add_object(T_ITEM,12*TILE_SIZE_X,1*TILE_SIZE_Y,0,T_ITEM,25);
				} /* if */ 
			} else {
			room56_state=0;
			} /* if */ 
		} /* if */ 
	} /* if */ 


	if (map==0 && map_x==6 && map_y==4 && next_world>=7) {
		int i;
		int count=0;

		for(i=0;i<HP_OBJS_MAX;i++) {
			if (HP_obj[i]!=-1 && HP_obj_type[i]==T_ENEMY &&
				HP_room_x[i]==6 &&
				(HP_room_y[i]==3 ||
				 HP_room_y[i]==4 ||
				 HP_room_y[i]==5)) {
				count++;
			} /* if */ 
		} /* for */ 		
		if (count>=11) {
			int i;
			bool sabre=true;

			if (item[24]) sabre=false;

			for(i=0;sabre && i<n_objects;i++) {
				if ((object[i].type==T_ITEM && 
					 object[i].state==24)) sabre=false;
			} /* for */ 

			if (sabre) {
				Sound_play(S_ladderdisapear);
				add_object(T_ITEM,26*TILE_SIZE_X,9*TILE_SIZE_Y,0,T_ITEM,24);
			} /* if */ 
		} /* if */ 
	} /* if */ 


	if (map==0 && map_x==6 && map_y==15 && pers_x<=5*TILE_SIZE_X) {
		int i;

		for(i=0;i<HP_OBJS_MAX;i++) {
			if (HP_obj[i]!=-1 && HP_obj_type[i]==T_LADDER) HP_obj[i]=-1;
		} /* for */ 
 
	} /* if */ 


	if (map==1 && map_x==2 && map_y==7) {
		int i;
		bool any_spider=false;

		/* Muro que desaparece cuando matas a todos los bichos: */ 
		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY && object[i].enemy==T_BLUESPIDER) {
				any_spider=true;
			} /* if */ 
		} /* for */ 
		if (!any_spider && room_tiles[0]!=127) {
			room_tiles[0]=127;
			room_tiles[1]=128;
			room_tiles[2]=127;
			room_tiles[3]=128;
			room_tiles[room_size_x]=129;
			room_tiles[room_size_x+1]=130;
			room_tiles[room_size_x+2]=129;
			room_tiles[room_size_x+3]=130;
			room_tiles[room_size_x*2]=128;
			room_tiles[room_size_x*2+1]=127;
			room_tiles[room_size_x*2+2]=128;
			room_tiles[room_size_x*2+3]=127;
			/* Sonido: */ 
			Sound_play(S_ladderdisapear);
		} /* if */ 
	} /* if */ 

	if (map==1 && map_x==2 && map_y==5) {
		int i;
		bool any_enemy=false;

		/* Escalera que aparece cuando matas a todos los enemigos: */ 
		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY) {
				any_enemy=true;
			} /* if */ 
			if (object[i].type==T_LADDER && object[i].x==20*TILE_SIZE_X) any_enemy=true;
		} /* for */ 

		if (!any_enemy && room_tiles[room_size_x*3+20]!=126) {
			add_object(T_LADDER,20*TILE_SIZE_X,4*TILE_SIZE_Y,3,0,0);
			room_tiles[room_size_x*3+20]=126;
			room_tiles[room_size_x*3+21]=126;
			/* Sonido: */ 
			Sound_play(S_ladderdisapear);
		} /* if */ 
	} /* if */ 

	if (map==2 && map_x==3 && map_y==2) {
		int i;
		bool any_witch=false;

		/* Escalera que aparece cuando matas a la bruja: */ 
		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY && object[i].enemy==T_WITCH) {
				any_witch=true;
			} /* if */ 
			if (object[i].type==T_LADDER) any_witch=true;
		} /* for */ 

		if (!any_witch) {
			add_object(T_LADDER,15*TILE_SIZE_X,0,3,0,0);
			/* Sonido: */ 
			Sound_play(S_ladderdisapear);
		} /* if */ 
	} /* if */ 

	if (map==2 && map_x==5 && map_y==1) {
		/* Puente que aparece con la muñeca: */ 
		if (item[5] && room_tiles[8+room_size_x*7]!=162) {
			int i;
			for(i=8;i<24;i++) room_tiles[i+room_size_x*7]=162;
			Sound_play(S_ladderdisapear);
		} /* if */ 
	} /* if */ 

	if (map==2 && map_x==7 && map_y==0) {
		int i;
		bool any_enemy=false;

		/* Escalera que aparece cuando matas a todos los enemigos: */ 
		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY) {
				any_enemy=true;
			} /* if */ 
			if (object[i].type==T_LADDER && object[i].x==25*TILE_SIZE_X) any_enemy=true;
		} /* for */ 

		if (!any_enemy && room_tiles[room_size_x*3+25]!=126) {
			add_object(T_LADDER,25*TILE_SIZE_X,4*TILE_SIZE_Y,3,0,0);
			room_tiles[room_size_x*3+25]=126;
			room_tiles[room_size_x*3+26]=126;
			/* Sonido: */ 
			Sound_play(S_ladderdisapear);
		} /* if */ 
	} /* if */ 

	if (map==2 && map_x==7 && map_y==1) {
		int i;
		bool witch=false;

		/* Muro que desaparece cuando matas a la bruja: */ 
		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY) {
				witch=true;
			} /* if */ 
		} /* for */ 
		if (!witch && room_tiles[room_size_x*8+room_size_x-4]!=127) {
			room_tiles[room_size_x*8+room_size_x-4]=127;
			room_tiles[room_size_x*8+room_size_x-3]=128;
			room_tiles[room_size_x*8+room_size_x-2]=127;
			room_tiles[room_size_x*8+room_size_x-1]=128;
			room_tiles[room_size_x*9+room_size_x-4]=129;
			room_tiles[room_size_x*9+room_size_x-3]=130;
			room_tiles[room_size_x*9+room_size_x-2]=129;
			room_tiles[room_size_x*9+room_size_x-1]=130;
			room_tiles[room_size_x*10+room_size_x-4]=128;
			room_tiles[room_size_x*10+room_size_x-3]=127;
			room_tiles[room_size_x*10+room_size_x-2]=128;
			room_tiles[room_size_x*10+room_size_x-1]=127;
			/* Sonido: */ 
			Sound_play(S_ladderdisapear);
		} /* if */ 
	} /* if */ 

	if (map==3 && map_x==0 && map_y==6) {
		if (world3_room06_counter>=5 && room_tiles[room_size_x-4]!=127) {
			room_tiles[room_size_x*0+room_size_x-4]=127;
			room_tiles[room_size_x*0+room_size_x-3]=128;
			room_tiles[room_size_x*0+room_size_x-2]=127;
			room_tiles[room_size_x*0+room_size_x-1]=128;
			room_tiles[room_size_x*1+room_size_x-4]=129;
			room_tiles[room_size_x*1+room_size_x-3]=130;
			room_tiles[room_size_x*1+room_size_x-2]=129;
			room_tiles[room_size_x*1+room_size_x-1]=130;
			room_tiles[room_size_x*2+room_size_x-4]=128;
			room_tiles[room_size_x*2+room_size_x-3]=127;
			room_tiles[room_size_x*2+room_size_x-2]=128;
			room_tiles[room_size_x*2+room_size_x-1]=127;
			/* Sonido: */ 
			Sound_play(S_ladderdisapear);
		} /* if */ 
	} /* if */ 

	if (map==3 && map_x==3 && map_y==1) {
		int i;
		bool any_enemy=false;

		/* Escalera que aparece cuando matas a todos los enemigos: */ 
		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY) {
				any_enemy=true;
			} /* if */ 
			if (object[i].type==T_LADDER && object[i].x==2*TILE_SIZE_X) any_enemy=true;
		} /* for */ 

		if (!any_enemy) {
			bool found=false;
			int i;
			for(i=0;i<n_objects;i++) {
				if (object[i].type==T_LADDER && object[i].x==2*TILE_SIZE_X && object[i].y==0) found=true;
			} /* for */ 
			if (!found) {
				add_object(T_LADDER,2*TILE_SIZE_X,0,3,0,0);
				/* Sonido: */ 
				Sound_play(S_ladderdisapear);
			} /* if */ 
		} /* if */ 
	} /* if */ 

	if (map==4 && map_x==2 && map_y==1) {
		int i;
		bool any_enemy=false;

		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY) {
				any_enemy=true;
			} /* if */ 
		} /* for */ 

		if (!any_enemy) world4_state|=1;
	} /* if */ 
	if (map==4 && map_x==3 && map_y==0) {
		int i;
		bool any_enemy=false;

		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY) {
				any_enemy=true;
			} /* if */ 
		} /* for */ 

		if (!any_enemy) world4_state|=2;
	} /* if */ 

	if (map==4 && map_x==4 && map_y==2) {
		if ((world4_state&0x01)==0) {
			int tx,ty;
			for(ty=12;ty<15;ty++) {
				for(tx=14;tx<18;tx++) {
					room_tiles[ty*room_size_x+tx]=126;
				} /* for */ 
			} /* for */ 
		} /* if */ 
		if ((world4_state&0x02)!=0) {
			bool found=false;
			int i;
			for(i=0;i<n_objects;i++) {
				if (object[i].type==T_LADDER && object[i].x==15*TILE_SIZE_X && object[i].y==0) found=true;
			} /* for */ 
			/* Hacer aparecer la escalera del Shrine: */ 
			if (!found) {
				add_object(T_LADDER,15*TILE_SIZE_X,0,3,0,0);
				/* Sonido: */ 
				Sound_play(S_ladderdisapear);
			} /* if */ 
		} /* if */ 
	} /* if */ 

	if (map==5 && map_x==4 && map_y==4) {
		const int interval=160;
		/* Pantalla donde va apareciendo piedras en el agua: */ 
		if (world5_room44_state>0 && world5_room44_state<interval) {
			if (pers_y==9*TILE_SIZE_Y) {
				if (pers_x>=2*TILE_SIZE_X && pers_x<4*TILE_SIZE_X) {			
					world5_room44_state++;
					world5_room44_state3=0;
					if (world5_room44_state==interval) {
						Sound_play(S_waterstone);
						room_tiles[7+11*room_size_x]=179;
						room_tiles[8+11*room_size_x]=180;
					} /* if */ 
				} else {
					if (pers_x>=26*TILE_SIZE_X && pers_x<28*TILE_SIZE_X) {
						world5_room44_state++;
						world5_room44_state3=1;
						if (world5_room44_state==interval) {
							Sound_play(S_waterstone);
							room_tiles[23+11*room_size_x]=179;
							room_tiles[24+11*room_size_x]=180;
						} /* if */ 
					} else {
						world5_room44_state=world5_room44_state2;
					} /* if */ 
				} /* if */ 
			} else {
				world5_room44_state=world5_room44_state2;
			} /* if */ 
		} /* if */ 
		if (world5_room44_state3==0) {
			if (world5_room44_state>=interval) {
				world5_room44_state++;
				if (world5_room44_state==interval*2) {
					Sound_play(S_waterstone);
					room_tiles[12+11*room_size_x]=179;
					room_tiles[13+11*room_size_x]=180;
				} /* if */ 
				if (world5_room44_state==interval*3) {
					Sound_play(S_waterstone);
					room_tiles[18+11*room_size_x]=179;
					room_tiles[19+11*room_size_x]=180;
					room_tiles[7+11*room_size_x]=129;
					room_tiles[8+11*room_size_x]=130;
				} /* if */ 
				if (world5_room44_state==interval*4) {
					Sound_play(S_waterstone);
					room_tiles[23+11*room_size_x]=179;
					room_tiles[24+11*room_size_x]=180;
					room_tiles[12+11*room_size_x]=130;
					room_tiles[13+11*room_size_x]=129;
				} /* if */ 
				if (world5_room44_state==interval*5) {
					Sound_play(S_waterstone);
					room_tiles[18+11*room_size_x]=130;
					room_tiles[19+11*room_size_x]=129;
				} /* if */ 
				if (world5_room44_state==interval*6) {
					Sound_play(S_waterstone);
					room_tiles[23+11*room_size_x]=129;
					room_tiles[24+11*room_size_x]=130;
				} /* if */ 
			} /* if */ 
		} else {
			if (world5_room44_state>=interval) {
				world5_room44_state++;
				if (world5_room44_state==interval*2) {
					Sound_play(S_waterstone);
					room_tiles[18+11*room_size_x]=179;
					room_tiles[19+11*room_size_x]=180;
				} /* if */ 
				if (world5_room44_state==interval*3) {
					Sound_play(S_waterstone);
					room_tiles[23+11*room_size_x]=129;
					room_tiles[24+11*room_size_x]=130;
					room_tiles[12+11*room_size_x]=179;
					room_tiles[13+11*room_size_x]=180;
				} /* if */ 
				if (world5_room44_state==interval*4) {
					Sound_play(S_waterstone);
					room_tiles[18+11*room_size_x]=130;
					room_tiles[19+11*room_size_x]=129;
					room_tiles[7+11*room_size_x]=179;
					room_tiles[8+11*room_size_x]=180;
				} /* if */ 
				if (world5_room44_state==interval*5) {
					Sound_play(S_waterstone);
					room_tiles[12+11*room_size_x]=130;
					room_tiles[13+11*room_size_x]=129;
				} /* if */ 
				if (world5_room44_state==interval*6) {
					Sound_play(S_waterstone);
					room_tiles[7+11*room_size_x]=129;
					room_tiles[8+11*room_size_x]=130;
				} /* if */ 
			} /* if */ 
		} /* if */ 
	} /* if */ 

	if (map==5 && map_x==1 && map_y==3) {
		int i;
		bool any_enemy=false;

		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY && object[i].enemy==T_HAIRY) {
				any_enemy=true;
			} /* if */ 
		} /* for */ 

		if (!any_enemy) world5_room44_state2=1;
	} /* if */ 

	if (map==5 && map_x==6 && map_y==3) {
		int i;
		bool any_enemy=false;

		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY && object[i].enemy==T_HAIRY) {
				any_enemy=true;
			} /* if */ 
		} /* for */ 

		if (!any_enemy) world5_room64_state=1;
	} /* if */ 

	if (map==5 && map_x==6 && map_y==4) {
		if (room_tiles[31+8*room_size_x]==125 && world5_room64_state==1) {
			Sound_play(S_ladderdisapear);
			room_tiles[28+8*room_size_x]=131;
			room_tiles[29+8*room_size_x]=135;
			room_tiles[30+8*room_size_x]=136;
			room_tiles[31+8*room_size_x]=132;

			room_tiles[28+9*room_size_x]=129;
			room_tiles[29+9*room_size_x]=137;
			room_tiles[30+9*room_size_x]=138;
			room_tiles[31+9*room_size_x]=130;

			room_tiles[28+10*room_size_x]=128;
			room_tiles[29+10*room_size_x]=139;
			room_tiles[30+10*room_size_x]=140;
			room_tiles[31+10*room_size_x]=127;
		} /* if */ 
	} /* if */ 

	if (map==6 && map_x==3 && map_y==2) {
		int i;
		bool any_enemy=false;

		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY) {
				any_enemy=true;
			} /* if */ 
		} /* for */ 

		if (!any_enemy) world6_room35_state|=1;
	} /* if */ 

	if (map==6 && map_x==2 && map_y==5) {
		int i;
		bool any_enemy=false;

		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY) {
				any_enemy=true;
			} /* if */ 
		} /* for */ 

		if (!any_enemy) world6_room35_state|=2;
	} /* if */ 

	if (map==6 && map_x==1 && map_y==5) {
		int i;
		bool any_enemy=false;

		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY) {
				any_enemy=true;
			} /* if */ 
		} /* for */ 

		if (!any_enemy) world6_room35_state|=4;
	} /* if */ 


	if (map==6 && map_x==3 && map_y==5) {
		if ((world6_room35_state&0x01)!=0 && 
			room_tiles[0+12*room_size_x]!=131) {
			Sound_play(S_ladderdisapear);
			room_tiles[0+12*room_size_x]=131;
			room_tiles[1+12*room_size_x]=132;
			room_tiles[2+12*room_size_x]=131;
			room_tiles[3+12*room_size_x]=132;

			room_tiles[0+13*room_size_x]=129;
			room_tiles[1+13*room_size_x]=130;
			room_tiles[2+13*room_size_x]=129;
			room_tiles[3+13*room_size_x]=130;

			room_tiles[0+14*room_size_x]=128;
			room_tiles[1+14*room_size_x]=127;
			room_tiles[2+14*room_size_x]=128;
			room_tiles[3+14*room_size_x]=127;
		} /* if */ 
	} /* if */ 

	if (map==6 && map_x==3 && map_y==5) {
		if ((world6_room35_state&0x06)==0x06 && 
			room_tiles[29+19*room_size_x]==125) {
			Sound_play(S_ladderdisapear);
			room_tiles[28+15*room_size_x]=126;
			room_tiles[29+15*room_size_x]=126;

			room_tiles[28+19*room_size_x]=130;
			room_tiles[29+19*room_size_x]=129;

			add_object(T_LADDER,28*TILE_SIZE_X,16*TILE_SIZE_Y,5,0,0);
		} /* if */ 
	} /* if */ 


	if (map==6 && map_x==5 && map_y==4) {
		int i;
		bool any_enemy=false;

		/* Escalera que aparece cuando matas al dragon: */ 
		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY) {
				any_enemy=true;
			} /* if */ 
		} /* for */ 

		if (!any_enemy) {
			bool found=false;
			int i;
			for(i=0;i<n_objects;i++) {
				if (object[i].type==T_LADDER && object[i].x==15*TILE_SIZE_X && object[i].y==0) found=true;
			} /* for */ 
			if (!found) {
				add_object(T_LADDER,15*TILE_SIZE_X,0,3,0,0);
				/* Sonido: */ 
				Sound_play(S_ladderdisapear);
			} /* if */ 
		} /* if */ 
	} /* if */ 

	if (map==7 && map_x==0 && map_y==2) {
		int i;
		bool any_enemy=false;

		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY && object[i].enemy==T_BAT) {
				any_enemy=true;
			} /* if */ 
		} /* for */ 

		if (!any_enemy) world7_room12_state=1;
	} /* if */ 

	if (map==7 && map_x==1 && map_y==2) {
		if ((world7_room12_state&0x01)!=0 && 
			room_tiles[8+0*room_size_x]!=127) {
			Sound_play(S_ladderdisapear);
			room_tiles[8+0*room_size_x]=127;
			room_tiles[9+0*room_size_x]=128;
			room_tiles[10+0*room_size_x]=127;
			room_tiles[11+0*room_size_x]=128;

			room_tiles[8+1*room_size_x]=129;
			room_tiles[9+1*room_size_x]=130;
			room_tiles[10+1*room_size_x]=129;
			room_tiles[11+1*room_size_x]=130;

			room_tiles[8+2*room_size_x]=128;
			room_tiles[9+2*room_size_x]=127;
			room_tiles[10+2*room_size_x]=128;
			room_tiles[11+2*room_size_x]=127;
		} /* if */ 
	} /* if */ 

	if (map==7 && map_x==5 && map_y==2) {
		int i;
		bool any_enemy=false;

		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY) {
				any_enemy=true;
			} /* if */ 
		} /* for */ 

		if (!any_enemy && 
			room_tiles[4+16*room_size_x]!=131) {
			Sound_play(S_ladderdisapear);
			room_tiles[4+16*room_size_x]=131;
			room_tiles[5+16*room_size_x]=132;
			room_tiles[6+16*room_size_x]=131;
			room_tiles[7+16*room_size_x]=132;

			room_tiles[4+17*room_size_x]=129;
			room_tiles[5+17*room_size_x]=130;
			room_tiles[6+17*room_size_x]=129;
			room_tiles[7+17*room_size_x]=130;

			room_tiles[4+18*room_size_x]=128;
			room_tiles[5+18*room_size_x]=127;
			room_tiles[6+18*room_size_x]=128;
			room_tiles[7+18*room_size_x]=127;			
		} /* if */ 

	} /* if */ 

	if (map==7 && map_x==3 && map_y==4) {
		int i;
		bool any_enemy=false;

		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY ||
				(object[i].type==T_LADDER && object[i].y==12*TILE_SIZE_Y)) {
				any_enemy=true;
			} /* if */ 
		} /* for */ 

		if (!any_enemy && 
			room_tiles[16+11*room_size_x]!=126) {
			Sound_play(S_ladderdisapear);
			room_tiles[16+11*room_size_x]=126;
			room_tiles[17+11*room_size_x]=126;
			add_object(T_LADDER,16*TILE_SIZE_Y,12*TILE_SIZE_Y,3,0,0);
		} /* if */ 

	} /* if */ 

	if (map==8 && map_x==3 && map_y==2) {
		int i;
		bool any_enemy=false;

		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY && object[i].enemy==T_TRANSFORMER) {
				any_enemy=true;
			} /* if */ 
		} /* for */ 

		if (!any_enemy && 
			room_tiles[28+4*room_size_x]!=131) {
			Sound_play(S_ladderdisapear);
			room_tiles[28+4*room_size_x]=131;
			room_tiles[29+4*room_size_x]=132;
			room_tiles[30+4*room_size_x]=131;
			room_tiles[31+4*room_size_x]=132;

			room_tiles[28+5*room_size_x]=129;
			room_tiles[29+5*room_size_x]=130;
			room_tiles[30+5*room_size_x]=129;
			room_tiles[31+5*room_size_x]=130;

			room_tiles[28+6*room_size_x]=128;
			room_tiles[29+6*room_size_x]=127;
			room_tiles[30+6*room_size_x]=128;
			room_tiles[31+6*room_size_x]=127;			
		} /* if */ 

	} /* if */ 

	if (map==8 && map_y==4 && map_x<6 && room_tiles[4+12*room_size_x]!=125 && item[11]) {
		int i,j;

		Sound_play(S_ladderdisapear);

		for(i=12;i<20;i++) {
			for (j=0;j<32;j++) room_tiles[j+i*room_size_x]=125;
		} /* for */ 

	} /* if */ 


	if (map==8 && map_x==5 && map_y==4) {
		int i;
		bool any_enemy=false;

		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY && object[i].enemy==T_CYCLOPS) {
				any_enemy=true;
			} /* if */ 
		} /* for */ 

		if (!any_enemy && 
			room_tiles[28+4*room_size_x]!=131) {
			Sound_play(S_ladderdisapear);
			room_tiles[28+4*room_size_x]=131;
			room_tiles[29+4*room_size_x]=132;
			room_tiles[30+4*room_size_x]=131;
			room_tiles[31+4*room_size_x]=132;

			room_tiles[28+5*room_size_x]=129;
			room_tiles[29+5*room_size_x]=130;
			room_tiles[30+5*room_size_x]=129;
			room_tiles[31+5*room_size_x]=130;

			room_tiles[28+6*room_size_x]=128;
			room_tiles[29+6*room_size_x]=127;
			room_tiles[30+6*room_size_x]=128;
			room_tiles[31+6*room_size_x]=127;			
		} /* if */ 
	} /* if */ 

	if (map==8 && map_x==1 && map_y==3) {
		int i;
		bool any_enemy=false;

		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY) {
				any_enemy=true;
			} /* if */ 
		} /* for */ 

		if (!any_enemy) world8_room12_state=1;
	} /* if */ 

	if (map==8 && map_x==1 && map_y==2	&& world8_room12_state!=0) {
		bool found=false;
		int i;
		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_LADDER && object[i].y==0) found=true;
		} /* for */ 
		if (!found) {
			add_object(T_LADDER,15*TILE_SIZE_X,0,3,0,0);
			/* Sonido: */ 
			Sound_play(S_ladderdisapear);
		} /* if */ 
	} /* if */ 

	if (map==9 && map_x==1 && map_y==7) {
		bool found=false;
		int i;
		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_LADDER && object[i].y==0) found=true;
			if (object[i].type==T_ENEMY) found=true;
		} /* for */ 
		if (!found) {
			/* Sonido: */ 
			Sound_play(S_ladderdisapear);
			add_object(T_LADDER,2*TILE_SIZE_X,0,3,0,0);
			room_tiles[2]=127;
			room_tiles[3]=128;
			room_tiles[2+room_size_x]=129;
			room_tiles[3+room_size_x]=130;
			room_tiles[2+2*room_size_x]=128;
			room_tiles[3+2*room_size_x]=127;
		} /* if */ 
	} /* if */ 

	if (map==9 && map_x==3 && map_y==2) {
		bool found=false;
		int i;
		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_LADDER && object[i].y==0) found=true;
			if (object[i].type==T_ENEMY) found=true;
		} /* for */ 
		if (!found) {
			/* Sonido: */ 
			Sound_play(S_ladderdisapear);
			add_object(T_LADDER,4*TILE_SIZE_X,0,3,0,0);
		} /* if */ 	
	} /* if */ 

	if (map==9 && map_x==2 && map_y==1) {
		bool found=false;
		int i;
		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_LADDER && object[i].y==0) found=true;
			if (object[i].type==T_ENEMY) found=true;
		} /* for */ 
		if (!found) {
			/* Sonido: */ 
			Sound_play(S_ladderdisapear);
			add_object(T_LADDER,15*TILE_SIZE_X,0,3,0,0);
		} /* if */ 	
	} /* if */ 
	if (map==9 && map_x==1 && map_y==0) {
		bool found=false;
		int i;
		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_ENEMY) found=true;
		} /* for */ 
		if (!found && room_tiles[4]==125) {
			/* Sonido: */ 
			Sound_play(S_ladderdisapear);
			room_tiles[4]=127;
			room_tiles[5]=128;
			room_tiles[6]=127;
			room_tiles[7]=128;
			room_tiles[4+room_size_x]=129;
			room_tiles[5+room_size_x]=130;
			room_tiles[6+room_size_x]=129;
			room_tiles[7+room_size_x]=130;
			room_tiles[4+room_size_x*2]=128;
			room_tiles[5+room_size_x*2]=127;
			room_tiles[6+room_size_x*2]=128;
			room_tiles[7+room_size_x*2]=127;
		} /* if */ 	
	} /* if */ 

	if (map==0 && map_x==world10_door_x && map_y==world10_door_y && next_world==10) {
		bool found=false;
		int i;
		for(i=0;i<n_objects;i++) {
			if (object[i].type==T_WDOOR) {
				found=true;
				if (world_doors_open[9]) object[i].state=1;
			} /* if */ 
		} /* for */ 

		if (!found) {
			/* Hacer que la puerta esté detrás de todo: */ 
			for(i=n_objects;i>0;i--) {
				object[i]=object[i-1];
			} /* if */ 
			n_objects++;

			if (in_ladder!=-1) in_ladder++;

			object[0].type=T_NADA;

			if (map_x==1) add_object(T_WDOOR,12*TILE_SIZE_X,16*TILE_SIZE_Y,10,0,0);
			if (map_x==5) add_object(T_WDOOR,20*TILE_SIZE_X,8*TILE_SIZE_Y,10,0,0);
			if (map_x==9) add_object(T_WDOOR,8*TILE_SIZE_X,16*TILE_SIZE_Y,10,0,0);
			if (map_x==14) add_object(T_WDOOR,20*TILE_SIZE_X,0*TILE_SIZE_Y,10,0,0);
		} /* if */ 
	} /* if */ 
} /* GameRoomEvents */ 



void fire_arrow()
{	
	int MAX_FIRED_ARROWS[2]={2,3};
	int tmpy;

	if (item[16]) MAX_FIRED_ARROWS[0]=3;
	if (current_weapon==5) {
		MAX_FIRED_ARROWS[0]=2;
		if (item[17]) MAX_FIRED_ARROWS[0]=3;
	} /* if */ 
	if (current_weapon==2 || current_weapon==3) MAX_FIRED_ARROWS[0]=MAX_FIRED_ARROWS[1]=2;

	tmpy=pers_y/TILE_SIZE_Y;
	if ((pers_y%TILE_SIZE_Y)>(TILE_SIZE_Y/2)) tmpy++;
	tmpy*=TILE_SIZE_Y;

	if (n_arrows>0 && 
		((current_weapon==5 && n_fired_mines<MAX_FIRED_ARROWS[character]) 
		 ||
		 (current_weapon!=5 && n_fired_arrows<MAX_FIRED_ARROWS[character]))) {
		switch(current_weapon) {
		case 0:/* FLECHAS NORMALES: */ 
			/* Al tener el "ROD", el número de flechas no disminuye: */ 
			if (fighting_demon!=0 && world_item[map-1][2]) n_arrows++;
			n_arrows--;
			n_fired_arrows++;
			if (pers_right) add_object(T_WEAPON_ARROW,pers_x+TILE_SIZE_X,tmpy+TILE_SIZE_Y,0,0,0);
					   else add_object(T_WEAPON_ARROW,pers_x-TILE_SIZE_X,tmpy+TILE_SIZE_Y,0,0,1);
			Sound_play(S_firearrow);
			break;
		case 1:/* FLECHAS DE CERAMICA: */ 
			/* Al tener el "ROD", el número de flechas no disminuye: */ 
			if (fighting_demon!=0 && world_item[map-1][2]) n_arrows++;
			n_arrows--;
			n_fired_arrows++;
			if (pers_right) add_object(T_WEAPON_ARROW2,pers_x+TILE_SIZE_X,tmpy+TILE_SIZE_Y,0,0,0);
					   else add_object(T_WEAPON_ARROW2,pers_x-TILE_SIZE_X,tmpy+TILE_SIZE_Y,0,0,1);
			Sound_play(S_firearrow2);
			break;
		case 2: /* ROLLING FIRE: */ 
			if (pers_state==S_QUIET || pers_state==S_WALKING_LEFT || pers_state==S_WALKING_RIGHT) {
				if (iswall(pers_x/TILE_SIZE_X,(pers_y/TILE_SIZE_Y)+2) ||
					iswall((pers_x/TILE_SIZE_X)+1,(pers_y/TILE_SIZE_Y)+2)) {
					int tx,ty;
					int val;
					tx=pers_x/TILE_SIZE_X;
					ty=tmpy/TILE_SIZE_Y;
					if (fighting_demon!=0 && world_item[map-1][2]) n_arrows++;
					n_arrows--;
					n_fired_arrows++;
					if (pers_right) val=add_object(T_WEAPON_RFIRE,(tx+1)*TILE_SIZE_X,(ty+1)*TILE_SIZE_Y,0,0,0);
							   else val=add_object(T_WEAPON_RFIRE,(tx+1)*TILE_SIZE_X,(ty+1)*TILE_SIZE_Y,0,0,1);
					object[val].tile=2;
					Sound_play(S_firefire);
				} /* if */ 
			} /* if */ 
			break;
		case 3: /* FUEGO: */ 
			if (pers_state==S_QUIET || pers_state==S_WALKING_LEFT || pers_state==S_WALKING_RIGHT) {
				if (iswall(pers_x/TILE_SIZE_X,(pers_y/TILE_SIZE_Y)+2) ||
					iswall((pers_x/TILE_SIZE_X)+1,(pers_y/TILE_SIZE_Y)+2)) {
					if (fighting_demon!=0 && world_item[map-1][2]) n_arrows++;
					n_arrows--;
					n_fired_arrows++;
					if (pers_right) add_object(T_WEAPON_FIRE,pers_x+TILE_SIZE_X,tmpy+TILE_SIZE_Y,0,0,0);
							   else add_object(T_WEAPON_FIRE,pers_x-TILE_SIZE_X,tmpy+TILE_SIZE_Y,0,0,1);
					Sound_play(S_firefire);
				} /* if */ 
			} /* if */ 
			break;
		case 4:/* LUPA */ 
			{
				int i;

				for(i=0;i<n_objects;i++) {
					if (object[i].type==T_TOMB &&
						object[i].x>=pers_x-TILE_SIZE_X && object[i].x<=pers_x+TILE_SIZE_X &&
						object[i].y>=pers_y-TILE_UNIT && object[i].y<=pers_y+TILE_UNIT) {
						/* Mirar una tumba: */ 
						STATE=12;
						SUBSTATE=0;
					} /* if */ 
				} /* for */ 
			}
			break;
		case 5:/* MINAS */ 
			if (pers_state==S_QUIET || pers_state==S_WALKING_LEFT || pers_state==S_WALKING_RIGHT) {
				if (iswall(pers_x/TILE_SIZE_X,(pers_y/TILE_SIZE_Y)+2) ||
					iswall((pers_x/TILE_SIZE_X)+1,(pers_y/TILE_SIZE_Y)+2)) {
					int o;
					if (fighting_demon!=0 && world_item[map-1][2]) n_arrows++;
					n_arrows--;
					n_fired_mines++;
					o=add_object(T_WEAPON_MINE,pers_x,tmpy+TILE_SIZE_Y+TILE_UNIT,0,0,0);
					object[o].tile=9;
					Sound_play(S_dropmine);
				} /* if */ 
			} /* if */ 
			
			break;
		} /* switch */ 
	} /* if */ 
} /* fire_arrow */ 


int test_if_weapon_colision(int tile,int tx,int ty,int *old)
{
	int retval=-1;
	int i,t=armas_tile;

	for(i=0;i<n_objects;i++) {
		if (object[i].type==T_WEAPON_ARROW || 
			object[i].type==T_WEAPON_ARROW2 ||
			object[i].type==T_WEAPON_FIRE ||
			object[i].type==T_WEAPON_RFIRE ||
			object[i].type==T_WEAPON_MINE ||
			object[i].type==T_MINE_EXP) {
			if (object[i].type==T_WEAPON_ARROW) {
				if (object[i].state==0) t=armas_tile;
								   else t=armas_tile+6;
			} /* if */ 
			if (object[i].type==T_WEAPON_ARROW2) {
				if (object[i].state==0) t=armas_tile+1;
								   else t=armas_tile+7;
			} /* if */ 
			if (object[i].type==T_WEAPON_FIRE) {
				if (object[i].state==0) t=armas_tile+3;
								   else t=armas_tile+8;
			} /* if */ 
			if (object[i].type==T_WEAPON_RFIRE) {
				t=armas_tile+2;
			} /* if */ 
			if (object[i].type==T_WEAPON_MINE) t=armas_tile+9;
			if (object[i].type==T_MINE_EXP && !todelete[i] &&
				(tiles[armas_tile+object[i].tile]->
						to_tile_colision(tiles[tile],object[i].draw_x,object[i].draw_y,tx,ty) ||
				 tiles[armas_tile+object[i].tile]->
						to_tile_colision(tiles[tile],object[i].draw_x,object[i].draw_y-(TILE_SIZE_Y*2)/3,tx,ty))) {
				if (old!=0) *old=object[i].type;
				retval=i;
			} else {
				if ((object[i].type==T_WEAPON_ARROW2 || !todelete[i]) &&
					tiles[t]->
						to_tile_colision(tiles[tile],object[i].draw_x,object[i].draw_y,tx,ty)) {		

					if (old!=0) *old=object[i].type;
					if (object[i].type==T_WEAPON_ARROW || 
						object[i].type==T_WEAPON_FIRE ||
						object[i].type==T_WEAPON_RFIRE)	{
//						if (!todelete[i]) n_fired_arrows--;
						todelete[i]=true;
	//					object[i].type=T_NADA;
 					} /* if */ 
					if (object[i].type==T_WEAPON_MINE) {
						Sound_play(S_mine);
						object[i].x-=TILE_SIZE_X;
						object[i].y-=TILE_SIZE_Y*2+TILE_UNIT;
						object[i].type=T_MINE_EXP;
						object[i].state=0;
						n_fired_mines--;
						retval=i;
					} else {
						return i;
					} /* if */ 
				} /* if */ 			   
			} /* if */ 
		} /* if */ 
	} /* for */ 
	return retval;
} /* test_if_weapon_colision */ 


void tile_anims(Bitmap *tiles,int dx,int dy)
{
	int i,j,k,offs;
	char *img,tmp;
	int disp[TILE_SIZE_Y]={1,1,2,2,2,2,3,3,3,3,4,4,4,4,4,4};

	/* Animación del agua: */ 
	/* Tile agua: 19,3	*/ 
	img=tiles->get_image();

	for(j=0;j<TILE_SIZE_Y;j++) {
		for(k=0;k<disp[j];k++) {
			tmp=img[19*TILE_SIZE_X+(3*TILE_SIZE_Y+j)*dx];
			for(i=0;i<TILE_SIZE_X-1;i++) {
				img[19*TILE_SIZE_X+(3*TILE_SIZE_Y+j)*dx+i]=img[19*TILE_SIZE_X+(3*TILE_SIZE_Y+j)*dx+i+1];
			} /* for */ 
			img[19*TILE_SIZE_X+(3*TILE_SIZE_Y+j)*dx+TILE_SIZE_X-1]=tmp;
		} /* for */ 
	} /* for */ 	

	lava_state++;
	if ((lava_state&0x01)==0) {
		for(offs=0,j=0;j<room_size_y;j++) {
			for(k=0;k<room_size_x;k++,offs++) {
				switch(room_tiles[offs]) {
				case 163:room_tiles[offs]=173;
						break;
				case 168:room_tiles[offs]=169;
						break;
				case 169:room_tiles[offs]=170;
						break;
				case 170:room_tiles[offs]=171;
						break;
				case 171:room_tiles[offs]=172;
						break;
				case 172:room_tiles[offs]=163;
						break;
				case 173:room_tiles[offs]=174;
						break;
				case 174:room_tiles[offs]=175;
						break;
				case 175:room_tiles[offs]=176;
						break;
				case 176:room_tiles[offs]=168;
						break;
				} /* switch */ 
			} /* for */ 
		} /* for */ 
	} /* if */ 

} /* tile_anims */ 

