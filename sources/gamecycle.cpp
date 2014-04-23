#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "sound.h"
#include "MOGtypes.h"

#include "drawing.h"
#include "bitmaps.h"
#include "sprites.h"
#include "tiles.h"

#include "mog.h"
#include "object.h"
#include "filehandling.h"

/* Paths: */ 


#ifndef _WIN32
#ifndef HAVE_STRUPR

char *strupr (char *str)
{
	char *ptr;
	
	ptr = str;
	while (*ptr)
	{
	  	*ptr = toupper (*ptr);
		ptr++;
	}
	return str;
}


#endif
#endif

extern char **g_paths;
extern int n_g_paths,act_g_path;
extern char *g_path;
extern char **s_paths;
extern int n_s_paths,act_s_path;
extern char *s_path;

/* Gráficos: */ 
Bitmap *konami_bmp=0,*menu_bmp=0,*tiles_bmp=0,*tiles2_bmp=0,*enemy_bmp=0,*enemy2_bmp=0,*final_bmp=0;
int n_tiles=0;
CTile **tiles=0;

BYTE *col_buffer=0;

/* Estado: */ 
int STATE=0,SUBSTATE=0,OLDSTATE=0,MENUOPTION=0;
int cycle=0;

/* Developers variables: */ 
int developer_start_x=-1,developer_start_y=-1,developer_start_map=-1;

/* Juego: */ 
int inter_screen=0;
int frame_counter=0;
bool pause=false;

int pause_state=0;
int map=0;			/* En que mundo está el personaje			*/ 
int map_x=0,map_y=0;	/* En que habitación dentro del mundo		*/ 
int pers_x=0,pers_y=0;	/* Posición del personaje en la habitación	*/ 
int pers_pos=0;
bool pers_right=true;	/* Hacia donde mira el personaje			*/ 
int pers_state=0;		/* Estado en el que se encuentra el personaje	*/ 
int pers_substate=0;	/* Variable de estado secundaria.			*/ 
int pers_walk_state=0;	/* Variable que controla el caminar.	*/ 
int jump_inertia=0;
bool sword=false;				/* Hay que dibujar la espada?	*/ 
bool old_sword=false;
int sword_x=0,sword_y=0;	/* Coordenadas de la espada.	*/ 
int sword_time=0;
int in_ladder=-1;			/* En qué escalera está el personaje	*/ 
bool previous_x_collision=false;	/* Hubo una colisión en X en el frame anterior? */ 
bool previous_y_collision=false;	/* Hubo una colisión en Y en el frame anterior? */ 
int hit_time=0;	/* Contador para saber cuando pueden dañarnos de nuevo	*/ 
int character=0;	/* Personaje seleccionado	*/ 
bool live_character[2]={true,true};	/* Si los personajes están vivos o muertos */ 
bool can_revive_character[2]={true,true}; /* Si los personajes pueden revivir otra vez o no */ 
int current_weapon=-1;	/* arma seleccionada */ 
int n_fired_arrows=0;	/* Nº de flechas activas	*/ 
int n_fired_mines=0;	/* Nº de flechas activas	*/ 
int in_door=-1; /* Objeto en el que está la puerta en la que se acaba de entrar	*/ 
int freezed=0;	/* Controla si el protagonista está congelado */ 
bool in_water=false;	/* Indica si el personaje está dentro del agua o no */ 
bool in_lava=false;
int water_counter=0;	/* Indica cuanto tiempo hace de que un personaje entró en el agua */ 
int lava_counter=0;	/* Indica cuanto tiempo hace de que un personaje entró en la lava */ 
bool mine_stepped=false;
bool item[38];	/*  0 - 29	: items	*/ 
				/* 30 - 31  : escudos de bronce y plata */ 
				/* 32 - 37	: armas	*/ 
bool world_item[10][4];
bool cross=false,world_key=false;
int next_world=0;
bool world_doors_open[10];
int world_key_item=0;
int to_enter_cut=0;	/* Controla los cortes que sufre el personaje al	*/ 
					/* entrar por las puertas.							*/ 
int currently_selecting=0;	/* Sirve para saber si se está seleccionando	*/ 
							/* personaje o arma.							*/ 
bool shop_item[3];
int passage_state=0;
unsigned char old_keyboard[SDLK_LAST]={
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	
	0,0};
int stone_hit_counter=0;	/* Sirve para acelerar el movimiento de la espada cuando	*/ 
							/* Aprodite está rompiedo rocas.							*/ 
int bible_counter=0,bible_subcounter=0;
bool ZEUS_password=false,ZEUS_used=false;

bool room_slimes=false;	/* Indica si en esta habitacion hay orugas o no.	*/ 
int room_waterdragons=-1;
int room_reddragon=0;
int slime_counter=0;
bool room_watermonster=false; /* Indica que hay monstruo en el agua.	*/ 
bool room_fallingstones=false,room_rollingstones=false; /* Indica si caen piedras */ 
int stones_counter=0;
bool room_blobs=false;	/* Indica si hay bichos rojos que suben	*/ 
bool room_demonroom=false,old_room_demonroom=false;	/* Indica si es la habitación de un monstruo. */ 
int world5_room44_state=0,world5_room44_state2=0,world5_room44_state3=0;
int world5_room64_state=0;
int world6_room35_state=0;
int world7_room12_state=0;
int world8_room12_state=0;
int room26_state=0,room56_state=0;
int room_bfly=0;
int room_fairy=-1;
int room_appearwall=-1;
int room_appearwall_x=0,room_appearwall_y=0;
int appearwall_record_x[MAX_AW_RECORD]={-1,-1,-1,-1,-1,-1,-1,-1,
										-1,-1,-1,-1,-1,-1,-1,-1,
										-1,-1,-1,-1,-1,-1,-1,-1,
										-1,-1,-1,-1,-1,-1,-1,-1};
int appearwall_record_y[MAX_AW_RECORD]={-1,-1,-1,-1,-1,-1,-1,-1,
										-1,-1,-1,-1,-1,-1,-1,-1,
										-1,-1,-1,-1,-1,-1,-1,-1,
										-1,-1,-1,-1,-1,-1,-1,-1};
int fairy_x=0,fairy_y=0;
int fighting_demon=0;
int watermonster_state=0;
int world3_room06_counter=0;
int world4_state=0;
int world10_door_x=-1,world10_door_y=-1;
bool room_lava=false;
int lava_state=0;
bool room_piranhas=false;
int piranhas_counter=0;
int room_birds=-1;

int n_arrows=0,n_coins=0,n_keys=0;
int player_energy[2],player_experience[2],player_max[2];
int player_denergy[2],player_dexperience[2];

int room_size_x=0,room_size_y=0;
int *room_tiles=0;
bool jumping_from_elevator=false;
bool character_over_vertical_lever=false;


/* Objetos: (escaleras, piedras, etc...) */ 
MOGObject object[MAX_OBJECTS];
int n_objects;
 
int wall_tile;
int stone_tile,ladder_tile,wdoor_tile,keydoor_tile,stats_tile;
int coin_tile,arrow_tile,key_tile,door_tile;
int character_tile[3],character_tile_inv[3],colision_tile;
int pause_tile[2];
int sword_tile[2],explosion_tile;
int worm_tile,bat_tile,skeleton_tile,smoke_tile,slime_tile,bouncingball_tile;
int watermonster_tile,jumpingbush_tile,bluespider_tile,whitefireball_tile;
int fstone_tile,knight_tile,blob_tile,bambu_tile,fairy_tile,porcupine_tile;
int items_tile,armas_tile,tomb_tile,wkeydoor_tile,wnokeydoor_tile;
int black_tile,witch_tile,whitebear_tile,feet_tile,redjumper_tile,livingwall_tile;
int lever_tile,megabat_tile,lava_tile,piranha_tile,whitestar_tile;
int demonsign_tile,lupa_tile,demonsmoke_tile,demon_tile,spider_tile,knighthead_tile;
int chicken_tile,rockman_tile,cloud_tile,bfly_tile,armourarrow_tile;
int ghost_tile,head_tile,worm2_tile,octopus_tile,hairy_tile,waterdragon_tile;
int waterbug_tile,bird_tile,stoneman_tile,pacman_tile,reddragon_tile;
int demon_obj,demon_obj2,owl_tile,greenmonkey_tile,plant_tile,transformer_tile;
int flame_tile,witch2_tile,cyclops_tile,lcloud_tile,glizard_tile,snake_tile,bdemon_tile;
int gorilla_tile,pamperseball_tile;

/* Transportadores: */ 
int lever_room_x[MAX_LEVERS];
int lever_room_y[MAX_LEVERS];
int lever_x[MAX_LEVERS];
int lever_y[MAX_LEVERS];
int lever_drx[MAX_LEVERS][2]={{13,13},{13,13},{1,1},{1,1},{3,3},{0,0},{3,3},{3,3},{2,2},{5,5},
							{11,11},{11,11},{12,12},{11,11},{8,8},{6,6},{6,6},{6,6},
							{8,8},{8,8},{12,12}};
int lever_dry[MAX_LEVERS][2]={{13,13},{9,15},{9,15},{14,14},{15,15},{12,12},{4,4},{3,7},{2,2},
							{6,6},{4,4},{5,5},{5,5},{3,7},{6,6},{4,4},{3,5},{2,2},
							{2,2},{2,6},{2,2}};
int lever_dx[MAX_LEVERS][2]={{5*TILE_SIZE_X,23*TILE_SIZE_X},{14*TILE_SIZE_X,14*TILE_SIZE_X},
							{14*TILE_SIZE_X,14*TILE_SIZE_X},{5*TILE_SIZE_X,23*TILE_SIZE_X},
							{4*TILE_SIZE_X,19*TILE_SIZE_X},{8*TILE_SIZE_X,18*TILE_SIZE_X},
							{4*TILE_SIZE_X,24*TILE_SIZE_X},{14*TILE_SIZE_X,14*TILE_SIZE_X},
							{8*TILE_SIZE_X,20*TILE_SIZE_X},{6*TILE_SIZE_X,22*TILE_SIZE_X},
							{4*TILE_SIZE_X,24*TILE_SIZE_X},{6*TILE_SIZE_X,22*TILE_SIZE_X},
							{6*TILE_SIZE_X,20*TILE_SIZE_X},{14*TILE_SIZE_X,14*TILE_SIZE_X},
							{4*TILE_SIZE_X,24*TILE_SIZE_X},{7*TILE_SIZE_X,24*TILE_SIZE_X,},
							{14*TILE_SIZE_X,14*TILE_SIZE_X,},{4*TILE_SIZE_X,24*TILE_SIZE_X},
							{6*TILE_SIZE_X,24*TILE_SIZE_X},{14*TILE_SIZE_X,14*TILE_SIZE_X},
							{10*TILE_SIZE_X,18*TILE_SIZE_X}};
int lever_dy[MAX_LEVERS][2]={{11*TILE_SIZE_Y,11*TILE_SIZE_Y},{11*TILE_SIZE_Y,15*TILE_SIZE_Y},
							{11*TILE_SIZE_Y,13*TILE_SIZE_Y},{11*TILE_SIZE_Y,11*TILE_SIZE_Y},
							{7*TILE_SIZE_Y,7*TILE_SIZE_Y},{7*TILE_SIZE_Y,7*TILE_SIZE_Y},
							{7*TILE_SIZE_Y,7*TILE_SIZE_Y},{11*TILE_SIZE_Y,15*TILE_SIZE_Y},
							{7*TILE_SIZE_Y,7*TILE_SIZE_Y},{11*TILE_SIZE_Y,11*TILE_SIZE_Y},
							{11*TILE_SIZE_Y,11*TILE_SIZE_Y},{7*TILE_SIZE_Y,7*TILE_SIZE_Y},
							{11*TILE_SIZE_Y,11*TILE_SIZE_Y},{3*TILE_SIZE_Y,11*TILE_SIZE_Y},
							{7*TILE_SIZE_Y,7*TILE_SIZE_Y},{7*TILE_SIZE_Y,7*TILE_SIZE_Y,},
							{7*TILE_SIZE_Y,16*TILE_SIZE_Y},{7*TILE_SIZE_Y,7*TILE_SIZE_Y},
							{3*TILE_SIZE_Y,3*TILE_SIZE_Y},{7*TILE_SIZE_Y,15*TILE_SIZE_Y},
							{7*TILE_SIZE_Y,7*TILE_SIZE_Y}};

int lever_going_to[MAX_LEVERS];


int worm_state_tile[28]={1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,
						 0,0,0,0,0,0,0,0};
int worm_state_tile_i[28]={5,5,5,5,4,4,4,4,3,3,3,3,2,2,2,2,1,1,1,1,
						 0,0,0,0,0,0,0,0};
int worm_state_y[28]={0,0,0,0,-2,-2,-2,-2,-6,-6,-6,-6,-2,-2,-2,-2,0,0,0,0,
					  0,0,0,0,0,0,0,0};

/* Estado de las habitaciones por las que ya has pasado: */ 
#define HP_OBJS_MAX	512

int HP_room_x[HP_OBJS_MAX],HP_room_y[HP_OBJS_MAX];
int HP_obj[HP_OBJS_MAX];
int HP_obj_type[HP_OBJS_MAX];
int HP_obj_aux[HP_OBJS_MAX];


/* Efectos de sonido: */ 
SOUNDT S_pause=0,S_death=0,S_gameover=0,S_worldkey=0,S_entering=0;
SOUNDT S_gamestart=0,S_jump=0,S_sword=0,S_swordhit=0;
SOUNDT S_item=0,S_ah=0,S_ah2=0,S_powerup=0,S_enemyhit=0,S_door=0;
SOUNDT S_enemykill=0,S_ladderdisapear=0,S_armourhit=0;
SOUNDT S_enterwater=0,S_select=0,S_firearrow=0,S_firearrow2=0;
SOUNDT S_waterhit=0,S_fall=0,S_nocoins=0,S_stones=0,S_blob=0,S_shield=0;
SOUNDT S_bell=0,S_lavahit=0,S_flapwings=0,S_rotatedoor=0,S_demon2ball=0;
SOUNDT S_appearing=0,S_firebreath=0,S_F1=0,S_karrow=0;
SOUNDT S_jumptree=0,S_skeletonhit=0,S_enemybullet=0,S_headbullet=0;
SOUNDT S_dropmine=0,S_firefire=0,S_fireball=0,S_fireball2=0;
SOUNDT S_wdoor=0,S_door2=0,S_demonhit=0,S_bearbullet=0,S_chickenbomb=0;
SOUNDT S_colormonster=0,S_waterstone=0,S_demon4jump=0,S_rockman=0,S_mine=0;
SOUNDT S_bible=0,S_demon1jump=0,S_demon1bones=0,S_owl=0,S_demon7bullet=0;
SOUNDT S_demon8bullet=0,S_flamebullet=0,S_snakebullet=0,S_gorilla=0,S_lizardtongue=0;
SOUNDT S_bdemonbullet=0,S_lightning=0;

int music_volume=127;
int sfx_volume=127;


/* Control de pantalla: */ 
extern int logic_dx[4],logic_dy[4],logic_x[4],logic_y[4];
extern int physic_dx[4],physic_dy[4],physic_x[4],physic_y[4];

int zoom=640;


/* Teclas: */ 
SDLKey UP_KEY=SDLK_q,DOWN_KEY=SDLK_a,LEFT_KEY=SDLK_o,RIGHT_KEY=SDLK_p;
SDLKey SWORD_KEY=SDLK_SPACE,WEAPON_KEY=SDLK_m,ITEM_KEY=SDLK_F1,PAUSE_KEY=SDLK_F2;
SDLKey last_word[16];

char password[48]="UR3FUR3FUR4F423RUR3FUR3FUR3FUR3FUR3FUR3FURS48";
int password_pos;


void DebugReport(void)
{
 	FILE *fp=f1open("report.txt","w+",USERDATA);

	fprintf(fp,"tiles: %i\n",n_tiles);
	fprintf(fp,"STATE: %i/%i\n",STATE,SUBSTATE);
	fprintf(fp,"GPATH: %s\n",g_paths[act_g_path]);
	fprintf(fp,"SPATH: %s\n",s_paths[act_s_path]);
	fprintf(fp,"pers_state: %i/%i\n",pers_state,pers_substate);
	fflush(fp);
	fclose(fp);

} /* DebugReport */ 


void GameCycle(BYTE *screen,int dx,int dy)
{
	char tmp[256];
	unsigned char *keyboard;

	SDL_PumpEvents();
	keyboard = (unsigned char *)SDL_GetKeyState(NULL);

	frame_counter++;

	switch(STATE) {

	/* APARECIENDO SIMBOLO DE KONAMI: */ 
	case 0:	{
				int i;

				if (SUBSTATE==0) {
					init_paths();
					if (!cargar_configuracion("MoG.cfg")) {
						configuracion_por_defecto();
						guardar_configuracion("MoG.cfg");
					} /* if */ 
					GameEnd();
					GameInit(dx,dy);
					HP_objs_reset();
//					Mix_VolumeMusic(music_volume);
					SetSFXVolume(sfx_volume);
					/* Borrar la pantalla: */ 
					memset(screen,konami_bmp->get_image()[0],dx*dy);
//					set_palete((BYTE *)konami_bmp->get_palete());

					for(;SUBSTATE<25;SUBSTATE++) {
						konami_bmp->draw_sprite(0,SUBSTATE,konami_bmp->get_ancho(),1,0,SUBSTATE*(dy/200),dx,1,screen,dx,dy,dx);
					} /* for */ 
				} /* if */ 

				for(i=0;i<SUBSTATE*(dy/200);i++) {
					konami_bmp->draw_sprite(0,i,konami_bmp->get_ancho(),1,0,i,dx,1,screen,dx,dy,dx);
				} /* for */ 
				SUBSTATE++;
				if (SUBSTATE>=200) {
					SUBSTATE=0;
					STATE=1;
				} /* if */ 

				if (keyboard[SDLK_SPACE]) {
					STATE=2;
					SUBSTATE=0;
				} /* if */ 
			}
			break;
	case 1:	SUBSTATE++;
			konami_bmp->draw(0,0,dx,dy,screen,dx,dy,dx);
			if (SUBSTATE>100) {
				SUBSTATE=0;
				STATE=2;
			} /* if */ 
			if (keyboard[SDLK_SPACE]) {
				STATE=2;
				SUBSTATE=0;
			} /* if */ 

			break;

	/* MENU PRINCIPAL: */ 
	case 2:	/* Borrar la pantalla: */ 
			SUBSTATE++;
			memset(screen,0,dx*dy);
//				set_palete((BYTE *)tiles_bmp->get_palete());
			menu_bmp->draw(0,0,dx,dy,screen,dx,dy,dx);
			tile_print("PUSH SPACE KEY",TILE_SIZE_X*13,TILE_SIZE_Y*19,screen,dx,dy);

			sprintf(tmp,"F10 OR 9 CHANGES GRAPHIC SET: %s",g_path+9);
			tmp[strlen(tmp)-1]=0;
			strupr(tmp);
			tile_print(tmp,TILE_SIZE_X*2,TILE_SIZE_Y*21,screen,dx,dy);
			sprintf(tmp,"F11 OR 0 CHANGES SOUND SET: %s",s_path+6);
			strupr(tmp);
			tmp[strlen(tmp)-1]=0;
			tile_print(tmp,TILE_SIZE_X*2,TILE_SIZE_Y*22,screen,dx,dy);
			tile_print("PRESS K TO REDEFINE THE KEYS",TILE_SIZE_X*6,TILE_SIZE_Y*23,screen,dx,dy);

			if (keyboard[SDLK_SPACE]  && !old_keyboard[SDLK_SPACE]) {
				developer_start_x=-1;
				developer_start_y=-1;
				developer_start_map=-1;
				strcpy(password,"UR3FUR3FUR4F423RUR3FUR3FUR3FUR3FUR3FUR3FURS48");
				STATE=3;
				SUBSTATE=0;
			} /* if */ 
			if (keyboard[SDLK_k] && !old_keyboard[SDLK_k]) {
				STATE=10;
				SUBSTATE=0;
			} /* if */ 
			if (keyboard[SDLK_l] && !old_keyboard[SDLK_l]) {
				int i;

				for(i=0;i<45;i++) password[i]=' ';
				password_pos=0;
				STATE=14;
				SUBSTATE=0;
			} /* if */ 

			if (keyboard[SDLK_g] && !old_keyboard[SDLK_g]) {
				developer_start_x=5;
				developer_start_y=12;
				developer_start_map=0;
				STATE=15;
				SUBSTATE=0;
			} /* if */ 

			if (SUBSTATE>=256) {
				STATE=17;
				SUBSTATE=0;
			} /* if */ 
			break;
	case 3: /* Se ha pulsado ESPACIO: */ 
			if (SUBSTATE==0) Sound_play(S_gamestart);
			if ((SUBSTATE%4)==0) {
				memset(screen,0,dx*dy);
				menu_bmp->draw(0,0,dx,dy,screen,dx,dy,dx);
				tile_print("  PLAY START  ",TILE_SIZE_X*13,TILE_SIZE_Y*19,screen,dx,dy);
			} /* if */ 
			if ((SUBSTATE%4)==2) {
				memset(screen,0,dx*dy);
				menu_bmp->draw(0,0,dx,dy,screen,dx,dy,dx);
			} /* if */ 
			if (SUBSTATE>=30) {

				if (map==0) sprintf(tmp,"castle");
					   else sprintf(tmp,"world");
				Sound_create_music(tmp,0,0);
								
				STATE=4;
				SUBSTATE=0;
				newgame();
			} else {
				SUBSTATE++;
			} /* if */ 
			break;
	case 4: /* JUEGO!!! */ 
			if (SUBSTATE==0) {
				SUBSTATE=1;
				/* Primer ciclo: */ 
				HP_filter(map_x,map_y);
				loadroom(map,map_x,map_y);
				HP_restore(map_x,map_y);
//				set_palete((BYTE *)tiles_bmp->get_palete());

				old_sword=false;
				hit_time=0;
				freezed=0;
				in_ladder=-1;
				previous_x_collision=false;
				previous_y_collision=false;
				pause=false;
			} /* if */ 

			if (next_world>=10 && world10_door_x==-1) {
				int x[4]={1,5,9,14};
				int y[4]={6,12,2,4};
				int i=rand()%4;
				world10_door_x=x[i];
				world10_door_y=y[i];
			} /* if */ 

			memset(screen,0,dx*dy);
			memset(col_buffer,0,dx*dy);	
			drawmap(screen,dx,dy,0);

			check_typed_word();

			if (!pause) {
				if (room_demonroom) {
					if ((next_world<=1 && fighting_demon==0 && map==1 && typed_word_p("yomar")) ||
						(next_world<=2 && fighting_demon==0 && map==2 && typed_word_p("elohim")) ||
						(next_world<=3 && fighting_demon==0 && map==3 && typed_word_p("hahakla")) ||
						(next_world<=4 && fighting_demon==0 && map==4 && typed_word_p("barechet")) ||
						(next_world<=5 && fighting_demon==0 && map==5 && typed_word_p("heotymeo")) ||
						(next_world<=6 && fighting_demon==0 && map==6 && typed_word_p("lepha")) ||
						(next_world<=7 && fighting_demon==0 && map==7 && typed_word_p("nawabra")) ||
						(next_world<=8 && fighting_demon==0 && map==8 && typed_word_p("ascher")) ||
						(next_world<=9 && fighting_demon==0 && map==9 && typed_word_p("xywoleh")) ||
						(next_world<=10 && fighting_demon==0 && map==10 && typed_word_p("hamalech"))) {
						/* MONSTRUO!!!!!! */ 
						char str[80],str2[80];

						clear_typed_word();
						Sound_release_music();
						sprintf(str,"demon1");
					    sprintf(str2,"demon2");
						Sound_create_music(str,str2,0);
						fighting_demon=1;
					} /* if */ 
				} else {
					if (fighting_demon!=0) {
						ReleaseGraphics();
						ReloadGraphics(dx,dy);
					} /* if */ 
					fighting_demon=0;
				} /* if */ 

				/* Efectos de los demonios: */ 
				if (fighting_demon!=0) {
					if (fighting_demon<160) {
						demon_intro(fighting_demon,screen,dx,dy);
						fighting_demon++;
					} /* if */ 
					if (fighting_demon==160) {
						if (map==1) load_demon1();
						if (map==2) load_demon2();
						if (map==3) load_demon3();
						if (map==4) load_demon4();
						if (map==5) load_demon5();
						if (map==6) load_demon6();
						if (map==7) load_demon7();
						if (map==8) load_demon8();
						if (map==9) load_demon9();
						if (map==10) load_demon10();
						fighting_demon++;
					} /* if */ 
					if (fighting_demon==161 && bible_subcounter==0) {
						if (map==1)	demon1_cycle();
						if (map==2) demon2_cycle();
						if (map==3) demon3_cycle();
						if (map==4) demon4_cycle();
						if (map==5) demon5_cycle();
						if (map==6) demon6_cycle();
						if (map==7) demon7_cycle();
						if (map==8) demon8_cycle();
						if (map==9) demon9_cycle();
						if (map==10) demon10_cycle();
					} /* if */ 
					if (fighting_demon>=162 && fighting_demon<194) {
						demon_intro(fighting_demon,screen,dx,dy);
						fighting_demon++;
					} /* if */ 
					if (fighting_demon==194) {
						object[demon_obj].type=T_NADA;
						switch(map) {
						case 1:
							world_key_item=add_object(T_ITEM,15*TILE_SIZE_X,13*TILE_SIZE_Y,0,0,37);
							break;
						case 2:
							object[demon_obj2].type=T_NADA;
							world_key_item=add_object(T_ITEM,15*TILE_SIZE_X,9*TILE_SIZE_Y,0,0,37);
							break;
						case 3:
							world_key_item=add_object(T_ITEM,15*TILE_SIZE_X,9*TILE_SIZE_Y,0,0,37);
							break;
						case 4:
							world_key_item=add_object(T_ITEM,15*TILE_SIZE_X,13*TILE_SIZE_Y,0,0,37);
							break;
						case 5:
							world_key_item=add_object(T_ITEM,15*TILE_SIZE_X,13*TILE_SIZE_Y,0,0,37);
							break;
						case 6:
							world_key_item=add_object(T_ITEM,12*TILE_SIZE_X,17*TILE_SIZE_Y,0,0,37);
							break;
						case 7:
							world_key_item=add_object(T_ITEM,16*TILE_SIZE_X,17*TILE_SIZE_Y,0,0,37);
							break;
						case 8:
							world_key_item=add_object(T_ITEM,16*TILE_SIZE_X,9*TILE_SIZE_Y,0,0,37);
							break;
						case 9:
							{
								int i;
								for(i=0;i<n_objects;i++) if (object[i].type==T_ENEMY) object[i].type=T_NADA;
								world_key_item=add_object(T_ITEM,15*TILE_SIZE_X,17*TILE_SIZE_Y,0,0,37);
							}
							break;
						case 10:
							next_world=11;
							fighting_demon++;
							break;
						} /* switch */ 
						fighting_demon++;
					} /* if */ 
					if (fighting_demon==195) {
						/* Esperando a coger la llave: */ 					
					} /* if */ 
					if (fighting_demon>=196 && fighting_demon<356) {
						/* Llave cogida: */ 
						fighting_demon++;

						if (map==10 && fighting_demon>=230) {
							char tmp[80],tmp2[80];

							Sound_release_music();
							sprintf(tmp,"worldkey2");
							sprintf(tmp2,"world");
							Sound_create_music(tmp,tmp2,0);
								
							fighting_demon=431;
							next_world=11;
						} /* if */ 

						/* Hacer que el personaje salte: */ 
						if (map!=10) {
							int base_y=13;
							int t=(fighting_demon-196)%32;
							if ((((fighting_demon-196)/32)&0x01)==0) pers_right=true;
																else pers_right=false;
							pers_pos=2;
							pers_state=0;
							pers_substate=0;
							if (map==1) base_y=13;
							if (map==2) base_y=9;
							if (map==3) base_y=9;
							if (map==4) base_y=13;
							if (map==5) base_y=13;
							if (map==6) base_y=17;
							if (map==7) base_y=17;
							if (map==8) base_y=9;
							if (map==9) base_y=17;
							if (t<12) {
								pers_y=base_y*TILE_SIZE_Y-(t*TILE_UNIT)*8;
							} /* if */ 
							if (t>=12 && t<14) {
								pers_y=base_y*TILE_SIZE_Y-(12*TILE_UNIT)*8-(t-12)*TILE_UNIT*4;
							} /* if */ 
							if (t>=14 && t<16) {
								pers_y=base_y*TILE_SIZE_Y-(12*TILE_UNIT)*8-(2*TILE_UNIT*4+(t-14)*TILE_UNIT*2);
							} /* if */ 
							if (t>=16 && t<18) {
								pers_y=base_y*TILE_SIZE_Y-(12*TILE_UNIT)*8-(2*TILE_UNIT*4+(18-t)*TILE_UNIT*2);
							} /* if */ 
							if (t>=18 && t<20) {
								pers_y=base_y*TILE_SIZE_Y-(12*TILE_UNIT)*8-(20-t)*TILE_UNIT*4;
							} /* if */ 
							if (t>=20) {
								pers_y=base_y*TILE_SIZE_Y-((32-t)*TILE_UNIT)*8;
							} /* if */ 
						}
					} /* if */ 
					if (fighting_demon>=356 && fighting_demon<432) {
						char tmp[80];
						if (next_world==11) {
							fighting_demon=432;
						} else {
							object[world_key_item].type=T_NADA;
							sprintf(tmp," I HAVE TAKEN WORLD %i BACK ! ",map);
							tile_print2(tmp,GAME_VIEW_X+2*TILE_SIZE_X,8*TILE_SIZE_Y,screen,dx,dy);
							fighting_demon++;
							pers_pos=0;
							if (fighting_demon==431 && map==3) {
								/* Hacer reaparecer las parededs laterales en el mundo 3: */ 
								int tx,ty;
								int x[4]={0,1,30,31};

								for(ty=0;ty<20;ty++) {
									if ((ty%4)!=3) {
										for(tx=0;tx<4;tx++) room_tiles[x[tx]+ty*room_size_x]=125;
									} /* if */ 
								} /* for */ 
								room_tiles[2+19*room_size_x]=126;
								room_tiles[3+19*room_size_x]=126;
								object[0].size=9;
							} /* if */ 
						} /* if */ 
					} /* if */ 
				} /* if */ 

				drawcharacter(screen,dx,dy);
				
				GameTestInteractions(dx,dy);
				tile_anims(tiles_bmp,dx,dy);
				GameObjectAnimation(dx,dy); 

				if (fighting_demon<196 || fighting_demon>=432) GameInGameCycle(dx,dy);
				ActualizeLevers(dx,dy);
				GameRoomEvents();
				RoomChange();
			} else {
				if (typed_word_p("zeus") && map==0 && map_x==5 && map_y==12 && pause) {
					ZEUS_password=true;
				} /* if */ 
				if (pause_state<0) {
					tiles[pause_tile[character]]->
						draw(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y-TILE_SIZE_Y,
							 TILE_SIZE_X*2+2*TILE_UNIT,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
				} else {
					if (pause_state<4) {
						tiles[pause_tile[character]+1]->
							draw(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y-TILE_SIZE_Y,
								 TILE_SIZE_X*2+2*TILE_UNIT,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
					} else {
						tiles[pause_tile[character]+2]->
							draw(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y-TILE_SIZE_Y,
								 TILE_SIZE_X*2+2*TILE_UNIT,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
					} /* if */ 
				} /* if */ 
				pause_state++;
				if (pause_state>=8) pause_state=0;
			} /* if */ 

			drawstats(screen,dx,dy);

			if (keyboard[SDLK_ESCAPE] && !old_keyboard[SDLK_ESCAPE]) {
				OLDSTATE=STATE;
				MENUOPTION=0;
				STATE=19;
			} /* if */ 

			if (keyboard[ITEM_KEY] && !old_keyboard[ITEM_KEY] && player_energy[character]>0 && !pause && to_enter_cut==0) {
				Sound_play(S_F1);
				currently_selecting=0;
				STATE=5;
				SUBSTATE=0;
			} /* if */ 

			if (keyboard[PAUSE_KEY] && !old_keyboard[PAUSE_KEY]) {
				pause=(pause ? false:true);
				if (pause) {
					pause_state=-256;
					Sound_pause_music();
					Sound_play(S_pause);
				} else {
					if (bible_subcounter==0) Sound_unpause_music();
				} /* if */ 
			} /* if */ 


			/* Gestiona vistas: */ 
			if (keyboard[SDLK_PAGEUP] && zoom<640) zoom+=8;
			if (keyboard[SDLK_PAGEDOWN] && zoom>320) zoom-=8;

			break;
	case 5:	/* PANTALLA DE ITEMS: */ 
			memset(screen,0,dx*dy);
			memset(col_buffer,0,dx*dy);	

			drawstats(screen,dx,dy);
		
			/* Popolón y aphrodite: */ 
			if (live_character[0]) 
				tiles[stats_tile+1]->
					draw(GAME_VIEW_X+TILE_SIZE_X*12,GAME_VIEW_Y+TILE_SIZE_Y*1,
						 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
			if (live_character[1])
				tiles[stats_tile+2]->
					draw(GAME_VIEW_X+TILE_SIZE_X*18,GAME_VIEW_Y+TILE_SIZE_Y*1,
						 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
			
			/* Cuadro que parpadea: */ 
			if (((SUBSTATE>>2)&0x01)==0 || currently_selecting==1) {
				if (character==0) {
					tiles[stats_tile+3]->
						draw(GAME_VIEW_X+TILE_SIZE_X*12,GAME_VIEW_Y+TILE_SIZE_Y*1,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				} else {
					tiles[stats_tile+3]->
						draw(GAME_VIEW_X+TILE_SIZE_X*18,GAME_VIEW_Y+TILE_SIZE_Y*1,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				} /* if */ 
			} /* if */ 

			/* Ítems: */ 
			tile_print("ITEM",TILE_SIZE_X*18,GAME_VIEW_Y+TILE_SIZE_Y*5,screen,dx,dy);
			{
				int i,j;

				for(i=0;i<3;i++) {
					for(j=0;j<10;j++) {
						if (i==2 && j==7) {
							if (item[j+i*10]) {
								tiles[items_tile+j+i*10]->
									draw(GAME_VIEW_X+TILE_SIZE_X*(6+j*2),GAME_VIEW_Y+TILE_SIZE_Y*(6+i*2),
										TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
							} else {
								if (item[30]) {
									tiles[items_tile+30]->
										draw(GAME_VIEW_X+TILE_SIZE_X*(6+j*2),GAME_VIEW_Y+TILE_SIZE_Y*(6+i*2),
											TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
								} else {
									if (item[31]) {
										tiles[items_tile+31]->
											draw(GAME_VIEW_X+TILE_SIZE_X*(6+j*2),GAME_VIEW_Y+TILE_SIZE_Y*(6+i*2),
												TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
									} /* if */ 
								} /* if */ 
							} /* if */ 
						} else {
							if (item[j+i*10]) {
								tiles[items_tile+j+i*10]->
									draw(GAME_VIEW_X+TILE_SIZE_X*(6+j*2),GAME_VIEW_Y+TILE_SIZE_Y*(6+i*2),
										TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
							} /* if */ 
						} /* if */ 
					} /* for */ 
				} /* for */ 
			}

			/* Tiles de mundo: */ 
			if (world_key) 
				tiles[items_tile+37]->
					draw(GAME_VIEW_X+TILE_SIZE_X*25,GAME_VIEW_Y+TILE_SIZE_Y*13,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
			if (cross) 
				tiles[items_tile+36]->
					draw(GAME_VIEW_X+TILE_SIZE_X*23,GAME_VIEW_Y+TILE_SIZE_Y*13,
						TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

			if (map>=1) {
				int i;
				for(i=0;i<4;i++) {
					if (world_item[map-1][i]) {
						tiles[items_tile+32+i]->
							draw(GAME_VIEW_X+TILE_SIZE_X*(15+i*2),GAME_VIEW_Y+TILE_SIZE_Y*13,
								TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					} /* if */ 
				} /* for */ 
			} /* if */ 

			if (world_item[map-1][3]) {
				/* Dibujar mapa: */ 
				tile_print("MAP",6*TILE_SIZE_X+GAME_VIEW_X,GAME_VIEW_Y+13*TILE_SIZE_Y,screen,dx,dy);
				draw_items_map(map,map_x,map_y,(((SUBSTATE>>2)&0x01)==0 ? true:false),screen,dx,dy);
			} /* if */ 

			/* Armas: */ 
			{ 
				int i;
				tiles[stats_tile+5]->
					draw(GAME_VIEW_X+TILE_SIZE_X*13,GAME_VIEW_Y+TILE_SIZE_Y*15,
						TILE_SIZE_X*1,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
				for(i=0;i<15;i++) {
					tiles[stats_tile+6]->
						draw(GAME_VIEW_X+TILE_SIZE_X*(14+i),GAME_VIEW_Y+TILE_SIZE_Y*15,
							TILE_SIZE_X*1,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
				} /* for */ 
				tiles[stats_tile+7]->
					draw(GAME_VIEW_X+TILE_SIZE_X*29,GAME_VIEW_Y+TILE_SIZE_Y*15,
						TILE_SIZE_X*1,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);

				if (item[32]) 
					tiles[armas_tile]->
						draw(GAME_VIEW_X+TILE_SIZE_X*14,GAME_VIEW_Y+TILE_SIZE_Y*16,
							TILE_SIZE_X*2,TILE_SIZE_Y*1,screen,col_buffer,dx,dy,dx);
				if (item[33]) 
					tiles[armas_tile+1]->
						draw(GAME_VIEW_X+TILE_SIZE_X*17,GAME_VIEW_Y+TILE_SIZE_Y*16,
							TILE_SIZE_X*2,TILE_SIZE_Y*1,screen,col_buffer,dx,dy,dx);
				if (item[34]) 
					tiles[armas_tile+2]->
						draw(GAME_VIEW_X+TILE_SIZE_X*20+TILE_SIZE_X/2,GAME_VIEW_Y+TILE_SIZE_Y*16,
							TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
				if (item[35]) 
					tiles[armas_tile+3]->
						draw(GAME_VIEW_X+TILE_SIZE_X*22+TILE_SIZE_X/2,GAME_VIEW_Y+TILE_SIZE_Y*16,
							TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
				if (item[37]) 
					tiles[armas_tile+5]->
						draw(GAME_VIEW_X+TILE_SIZE_X*24,GAME_VIEW_Y+TILE_SIZE_Y*16,
							TILE_SIZE_X*2,TILE_SIZE_Y*1,screen,col_buffer,dx,dy,dx);
				if (item[36]) 
					tiles[armas_tile+4]->
						draw(GAME_VIEW_X+TILE_SIZE_X*27,GAME_VIEW_Y+TILE_SIZE_Y*16,
							TILE_SIZE_X*2,TILE_SIZE_Y*1,screen,col_buffer,dx,dy,dx);

				if (current_weapon!=-1) {
					/* Cuadro que parpadea(armas): */ 
					if (((SUBSTATE>>2)&0x01)==0 || currently_selecting==0) {
						int pos[6]={0,3,6,8,13,10};

						if (character==0) {
							tiles[stats_tile+4]->
								draw(GAME_VIEW_X+TILE_SIZE_X*(14+pos[current_weapon]),GAME_VIEW_Y+TILE_SIZE_Y*15+TILE_UNIT*4,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
						} else {
							tiles[stats_tile+4]->
								draw(GAME_VIEW_X+TILE_SIZE_X*(14+pos[current_weapon]),GAME_VIEW_Y+TILE_SIZE_Y*15+TILE_UNIT*4,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
						} /* if */ 
					} /* if */ 
				} /* if */ 
			
			}

			if (!old_keyboard[LEFT_KEY] && !old_keyboard[RIGHT_KEY] &&
				(keyboard[LEFT_KEY] || keyboard[RIGHT_KEY])) {
				if (currently_selecting==0) {
					if (live_character[0] &&
						live_character[1]) {
						bool forbiden=false;

						if (map==5 && !item[22]) forbiden=true;
						if (map==7 && !item[0]) forbiden=true;

						if (!forbiden) {
							if (character==0) character=1;
										else  character=0;
							Sound_play(S_select);
						} /* if */ 
					} /* if */ 
				} else {
					if (keyboard[RIGHT_KEY]) {
						Sound_play(S_select);
						do {
							switch(current_weapon) {
							case 0:current_weapon=1;
								break;
							case 1:current_weapon=2;
								break;
							case 2:current_weapon=3;
								break;
							case 3:current_weapon=5;
								break;
							case 4:current_weapon=0;
								break;
							case 5:current_weapon=4;
								break;
							} /* switch */ 
						} while(!item[32+current_weapon]);
					} else {
						Sound_play(S_select);
						do {
							switch(current_weapon) {
							case 0:current_weapon=4;
								break;
							case 1:current_weapon=0;
								break;
							case 2:current_weapon=1;
								break;
							case 3:current_weapon=2;
								break;
							case 4:current_weapon=5;
								break;
							case 5:current_weapon=3;
								break;
							} /* switch */ 
						} while(!item[32+current_weapon]);
					} /* if */ 
				} /* if */ 
			} /* if */ 

			if (!old_keyboard[UP_KEY] && !old_keyboard[DOWN_KEY] &&
				current_weapon!=-1 &&
				(keyboard[UP_KEY] || keyboard[DOWN_KEY])) {
				Sound_play(S_select);
				if (currently_selecting==1) currently_selecting=0;
									   else currently_selecting=1;
			} /* if */ 

			if (current_weapon==-1) {
				if (item[37]) current_weapon=5;
				if (item[36]) current_weapon=4;
				if (item[35]) current_weapon=3;
				if (item[34]) current_weapon=2;
				if (item[33]) current_weapon=1;
				if (item[32]) current_weapon=0;
			} /* if */ 

			SUBSTATE++;

			if (!old_keyboard[ITEM_KEY] && keyboard[ITEM_KEY]) {
				STATE=4;
				SUBSTATE=1;
			} /* if */ 

			/* Halo de ángel: */ 
			if (item[8] && (fighting_demon==0 || fighting_demon==432) &&
				!old_keyboard[SDLK_RETURN] && keyboard[SDLK_RETURN]) 
			{
				if (map!=0) {
					STATE=9;
					SUBSTATE=0;
				} else {
					STATE=4;
					SUBSTATE=1;
				} /* if */ 
				if (map!=0) HP_objs_reset();
				map=0;
				map_x=5;
				map_y=9;
				pers_x=TILE_SIZE_X*16;
				pers_y=TILE_SIZE_Y*5;
				pers_state=0;
				pers_pos=0;
				n_fired_arrows=0;
				n_fired_mines=0;
				loadroom(map,map_x,map_y);
			} /* if */ 

			/* Pluma: */ 
			if (item[26] && map==0 && !keyboard[SDLK_RETURN] &&
				((!old_keyboard[SDLK_1] && keyboard[SDLK_1]) ||
				 (!old_keyboard[SDLK_2] && keyboard[SDLK_2]) ||
				 (!old_keyboard[SDLK_3] && keyboard[SDLK_3]) ||
				 (!old_keyboard[SDLK_4] && keyboard[SDLK_4]) ||
				 (!old_keyboard[SDLK_5] && keyboard[SDLK_5]) ||
				 (!old_keyboard[SDLK_6] && keyboard[SDLK_6]) ||
				 (!old_keyboard[SDLK_7] && keyboard[SDLK_7]) ||
				 (!old_keyboard[SDLK_8] && keyboard[SDLK_8]) ||
				 (!old_keyboard[SDLK_9] && keyboard[SDLK_9]))) {
				bool go=false;

				if ((!old_keyboard[SDLK_1] && keyboard[SDLK_1]) && world_doors_open[0]) {
					map_x=3;
					map_y=12;
					pers_x=TILE_SIZE_X*9;
					pers_y=TILE_SIZE_Y*5;
					go=true;
				} /* if */ 
				if ((!old_keyboard[SDLK_2] && keyboard[SDLK_2]) && world_doors_open[1]) {
					map_x=10;
					map_y=11;
					pers_x=TILE_SIZE_X*25;
					pers_y=TILE_SIZE_Y*5;
					go=true;
				} /* if */ 
				if ((!old_keyboard[SDLK_3] && keyboard[SDLK_3]) && world_doors_open[2]) {
					map_x=10;
					map_y=8;
					pers_x=TILE_SIZE_X*21;
					pers_y=TILE_SIZE_Y*5;
					go=true;
				} /* if */ 
				if ((!old_keyboard[SDLK_4] && keyboard[SDLK_4]) && world_doors_open[3]) {
					map_x=0;
					map_y=8;
					pers_x=TILE_SIZE_X*9;
					pers_y=TILE_SIZE_Y*9;
					go=true;
				} /* if */ 
				if ((!old_keyboard[SDLK_5] && keyboard[SDLK_5]) && world_doors_open[4]) {
					map_x=8;
					map_y=8;
					pers_x=TILE_SIZE_X*9;
					pers_y=TILE_SIZE_Y*5;
					go=true;
				} /* if */ 
				if ((!old_keyboard[SDLK_6] && keyboard[SDLK_6]) && world_doors_open[5]) {
					map_x=14;
					map_y=12;
					pers_x=TILE_SIZE_X*21;
					pers_y=TILE_SIZE_Y*5;
					go=true;
				} /* if */ 
				if ((!old_keyboard[SDLK_7] && keyboard[SDLK_7]) && world_doors_open[6]) {
					map_x=5;
					map_y=2;
					pers_x=TILE_SIZE_X*5;
					pers_y=TILE_SIZE_Y*17;
					go=true;
				} /* if */ 
				if ((!old_keyboard[SDLK_8] && keyboard[SDLK_8]) && world_doors_open[7]) {
					map_x=4;
					map_y=4;
					pers_x=TILE_SIZE_X*13;
					pers_y=TILE_SIZE_Y*17;
					go=true;
				} /* if */ 
				if ((!old_keyboard[SDLK_9] && keyboard[SDLK_9]) && world_doors_open[8]) {
					map_x=7;
					map_y=0;
					pers_x=TILE_SIZE_X*13;
					pers_y=TILE_SIZE_Y*5;
					go=true;
				} /* if */ 
				if (go) {
					STATE=4;
					SUBSTATE=1;
					pers_state=0;
					pers_pos=0;
					loadroom(map,map_x,map_y);
				} /* if */ 
			} /* if */ 

			if (keyboard[SDLK_ESCAPE] && !old_keyboard[SDLK_ESCAPE]) {
				OLDSTATE=STATE;
				MENUOPTION=0;
				STATE=19;
			} /* if */ 

			break;
	case 6:	if (!old_keyboard[ITEM_KEY] && !old_keyboard[SDLK_RETURN]) {
				STATE=4;
				SUBSTATE=1;
			} /* if */ 
			break;
	case 7:	/* DENTRO DE ALGÚN PASAJE, TIENDA, ETC.: */ 
			if (SUBSTATE==0) {
				SUBSTATE=1;
				currently_selecting=0;
				passage_state=0;
				shop_item[0]=true;
				shop_item[1]=true;
				shop_item[2]=true;
			} /* if */ 


			memset(screen,0,dx*dy);
			memset(col_buffer,0,dx*dy);	

			drawstats(screen,dx,dy);

			passage_mainloop(map,map_x,map_y,screen,dx,dy);
			
			if (!old_keyboard[DOWN_KEY] && keyboard[DOWN_KEY]) {
				STATE=4;
				SUBSTATE=1;
				/* Restaurar la música de juego normal: */ 
				char tmp[80],tmp2[80];
				Sound_release_music();
				sprintf(tmp2,"entering");
				if (map==0) sprintf(tmp,"castle");
					   else sprintf(tmp,"world");
				Sound_create_music(tmp2,tmp,0);

				if (map!=0) {
					map_y++;
					pers_y=0;
					pers_state=S_DOWN_LADDER;
					pers_pos=5;
				} /* if */ 

			} /* if */ 

			if (keyboard[SDLK_ESCAPE] && !old_keyboard[SDLK_ESCAPE]) {
				OLDSTATE=STATE;
				MENUOPTION=0;
				STATE=19;
			} /* if */ 
			break;
	case 8:	if (!old_keyboard[DOWN_KEY]) {
				STATE=4;
				SUBSTATE=1;
			} /* if */ 
			break;
	case 9: /* MOSTRANDO MENSAJE DE ENTRADA A MUNDO O CASTILLO: */ 
			world3_room06_counter=0;
			world5_room44_state=0;
			world5_room44_state2=0;
			world5_room64_state=0;
			world6_room35_state=0;
			world7_room12_state=0;
			world8_room12_state=0;
			{
				int i;
				for(i=0;i<MAX_AW_RECORD;i++) {
					appearwall_record_x[i]=-1;
					appearwall_record_y[i]=-1;
				} /* for */ 
			}

			if (SUBSTATE==0) {
				HP_objs_reset();
				Sound_release_music();
				if (map!=0) Sound_play(S_gamestart);
			} /* if */ 

			memset(screen,0,dx*dy);
			memset(screen,0,dx*dy);
			memset(col_buffer,0,dx*dy);	

			drawstats(screen,dx,dy);

			{
				char tmp[80];
				if (map==0) sprintf(tmp,"CASTLE !");
					   else sprintf(tmp,"WORLD %i !",map);
				
				tile_print(tmp,TILE_SIZE_X*16,TILE_SIZE_Y*12,screen,dx,dy);
			}


			if (SUBSTATE>=30) {
				if (map==0) {
					char tmp[80],tmp2[80];
					
					if (map_x==5 && map_y==9) {
						/* Entrada por Halo: */ 
						if (SUBSTATE==30) {
							Sound_release_music();
							sprintf(tmp,"castle");
							Sound_create_music(tmp,0,0);
						} /* if */ 
					} else {
						if (SUBSTATE==30) {
							Sound_release_music();
							sprintf(tmp2,"entering");
							sprintf(tmp,"castle");
							Sound_create_music(tmp2,tmp,0);
						} /* if */ 

						pers_state=S_ENTERING_WORLD;
						pers_substate=33;
						to_enter_cut=32;
					} /* if */ 
				} else {
					char tmp[80];
					Sound_release_music();
					sprintf(tmp,"world");
					Sound_create_music(tmp,0,0);

					pers_state=S_QUIET;
					pers_substate=0;
					pers_pos=0;
					pers_right=true;
					to_enter_cut=0;

					if (map==1) {
						pers_x=16*TILE_SIZE_X;
						pers_y=TILE_SIZE_Y;
					} /* if */ 
					if (map==2) {
						pers_x=15*TILE_SIZE_X;
						pers_y=13*TILE_SIZE_Y;
					} /* if */ 
					if (map==3) {
						pers_x=15*TILE_SIZE_X;
						pers_y=13*TILE_SIZE_Y;
					} /* if */ 
					if (map==4) {
						world4_state=0;
						pers_x=3*TILE_SIZE_X;
						pers_y=17*TILE_SIZE_Y;
					} /* if */ 
					if (map==5) {
						pers_x=27*TILE_SIZE_X;
						pers_y=9*TILE_SIZE_Y;
					} /* if */ 
					if (map==6) {
						pers_x=16*TILE_SIZE_X;
						pers_y=TILE_SIZE_Y;
					} /* if */ 
					if (map==7) {
						pers_x=16*TILE_SIZE_X;
						pers_y=17*TILE_SIZE_Y;
					} /* if */ 
					if (map==8) {
						pers_x=15*TILE_SIZE_X;
						pers_y=1*TILE_SIZE_Y;
					} /* if */ 
					if (map==9) {
						pers_x=3*TILE_SIZE_X;
						pers_y=1*TILE_SIZE_Y;
					} /* if */ 
					if (map==10) {
						pers_x=2*TILE_SIZE_X;
						pers_y=1*TILE_SIZE_Y;
					} /* if */ 
				} /* if */ 
				STATE=4;
				SUBSTATE=0;
			} else {
				SUBSTATE++;
			} /* if */ 
			break;


	case 10:/* REDEFINIENDO TECLAS!!! */ 
			{
				bool found;
				char tmp[80];
				int i;

				memset(screen,0,dx*dy);
				memset(col_buffer,0,dx*dy);	

				tmp[0]=0;

				if (SUBSTATE>=0) {
					tile_print("PRESS A KEY FOR UP:",TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,dx,dy);
				} /* if */ 
				if (SUBSTATE>=1) { 
					sprintf(tmp,SDL_GetKeyName((SDLKey)UP_KEY));
					tile_print(strupr(tmp),TILE_SIZE_X*26,TILE_SIZE_Y*2,screen,dx,dy);
					tile_print("PRESS A KEY FOR DOWN:",TILE_SIZE_X*2,TILE_SIZE_Y*4,screen,dx,dy);
				} /* if */ 
				if (SUBSTATE>=2) { 
					sprintf(tmp,SDL_GetKeyName((SDLKey)DOWN_KEY));
					tile_print(strupr(tmp),TILE_SIZE_X*26,TILE_SIZE_Y*4,screen,dx,dy);
					tile_print("PRESS A KEY FOR LEFT:",TILE_SIZE_X*2,TILE_SIZE_Y*6,screen,dx,dy);
				} /* if */ 
				if (SUBSTATE>=3) {  
					sprintf(tmp,SDL_GetKeyName((SDLKey)LEFT_KEY));
					tile_print(strupr(tmp),TILE_SIZE_X*26,TILE_SIZE_Y*6,screen,dx,dy);
					tile_print("PRESS A KEY FOR RIGHT:",TILE_SIZE_X*2,TILE_SIZE_Y*8,screen,dx,dy);
				} /* if */ 
				if (SUBSTATE>=4) { 
					sprintf(tmp,SDL_GetKeyName((SDLKey)RIGHT_KEY));
					tile_print(strupr(tmp),TILE_SIZE_X*26,TILE_SIZE_Y*8,screen,dx,dy);
					tile_print("PRESS A KEY FOR SWORD:",TILE_SIZE_X*2,TILE_SIZE_Y*10,screen,dx,dy);
				} /* if */ 
				if (SUBSTATE>=5) { 
					sprintf(tmp,SDL_GetKeyName((SDLKey)SWORD_KEY));
					tile_print(strupr(tmp),TILE_SIZE_X*26,TILE_SIZE_Y*10,screen,dx,dy);
					tile_print("PRESS A KEY FOR WEAPON:",TILE_SIZE_X*2,TILE_SIZE_Y*12,screen,dx,dy);
				} /* if */ 
				if (SUBSTATE>=6) { 
					sprintf(tmp,SDL_GetKeyName((SDLKey)WEAPON_KEY));
					tile_print(strupr(tmp),TILE_SIZE_X*26,TILE_SIZE_Y*12,screen,dx,dy);
					tile_print("PRESS A KEY FOR ITEM:",TILE_SIZE_X*2,TILE_SIZE_Y*14,screen,dx,dy);
				} /* if */ 
				if (SUBSTATE>=7) {
					sprintf(tmp,SDL_GetKeyName((SDLKey)ITEM_KEY));
					tile_print(strupr(tmp),TILE_SIZE_X*26,TILE_SIZE_Y*14,screen,dx,dy);
					tile_print("PRESS A KEY FOR PAUSE:",TILE_SIZE_X*2,TILE_SIZE_Y*16,screen,dx,dy);
				} /* if */ 
				if (SUBSTATE>=8) { 
					sprintf(tmp,SDL_GetKeyName((SDLKey)PAUSE_KEY));
					tile_print(strupr(tmp),TILE_SIZE_X*26,TILE_SIZE_Y*16,screen,dx,dy);
				} /* if */ 

				tile_print("TO GO TO THE TITLE SCREEN PRESS:  ESC",TILE_SIZE_X*2,TILE_SIZE_Y*18,screen,dx,dy);
				tile_print("TO EXIT THE GAME PRESS:   F12/APPLE+Q",TILE_SIZE_X*2,TILE_SIZE_Y*19,screen,dx,dy);
				tile_print("TO CHANGE THE ZOOM:         PGUP:PGDN",TILE_SIZE_X*2,TILE_SIZE_Y*20,screen,dx,dy);

				found=false;
				for(i=0;!found && i<SDLK_LAST;i++) {
					if (keyboard[i] && !old_keyboard[i]) {
						switch(SUBSTATE) {
						case 0:UP_KEY=(SDLKey)i;
							   SUBSTATE++;
							   found=true;
							   break;
						case 1:DOWN_KEY=(SDLKey)i;
							   SUBSTATE++;
							   found=true;
							   break;
						case 2:LEFT_KEY=(SDLKey)i;
							   SUBSTATE++;
							   found=true;
							   break;
						case 3:RIGHT_KEY=(SDLKey)i;
							   SUBSTATE++;
							   found=true;
							   break;
						case 4:SWORD_KEY=(SDLKey)i;
							   SUBSTATE++;
							   found=true;
							   break;
						case 5:WEAPON_KEY=(SDLKey)i;
							   SUBSTATE++;
							   found=true;
							   break;
						case 6:ITEM_KEY=(SDLKey)i;
							   SUBSTATE++;
							   found=true;
							   break;
						case 7:PAUSE_KEY=(SDLKey)i;
							   SUBSTATE++;
							   found=true;
							   break;
						} /* switch */ 
					} /* if */ 
				} /* for */ 

				if (SUBSTATE==8 && 
					((keyboard[SDLK_SPACE] && !old_keyboard[SDLK_SPACE]) ||
					 (keyboard[SDLK_RETURN] && !old_keyboard[SDLK_RETURN]) ||
					 (keyboard[SDLK_ESCAPE] && !old_keyboard[SDLK_ESCAPE]) ||
					 (keyboard[SDLK_k] && !old_keyboard[SDLK_k]))) {
					STATE=2;
					SUBSTATE=0;
					guardar_configuracion("MoG.cfg");
				} /* if */ 
			}
			break;

	case 11:/* MENSAJE DE PERSONAJE MUERTO: */ 
			{
				int i,j;
			
				j=SUBSTATE*TILE_SIZE_X;
				if (j>dx) j=dx;
				if (j>0) {
					for(i=0;i<dy;i++) {
						memset(screen+i*dx,0,j);
					} /* if */ 
				} /* if */ 
				
				if (SUBSTATE>=40) {
					if (SUBSTATE==40) {
						if (!live_character[1] &&
							!live_character[0]) {
							ZEUS_used=false;
							Sound_play(S_gameover);
						} /* if */ 
					} /* if */ 
					if (!live_character[0] &&
						 live_character[1]) {
						tile_print("POPOLON OUT",TILE_SIZE_X*14,TILE_SIZE_Y*12,screen,dx,dy);
					} /* if */ 
					if (!live_character[1] &&
						 live_character[0]) {
						tile_print("APHRODITE OUT",TILE_SIZE_X*13,TILE_SIZE_Y*12,screen,dx,dy);
					} /* if */ 
					if (!live_character[1] &&
						!live_character[0]) {
						tile_print("GAME OVER",TILE_SIZE_X*15,TILE_SIZE_Y*12,screen,dx,dy);
						if (ZEUS_password && !ZEUS_used) {
							tile_print("F5 CONTINUE",TILE_SIZE_X*14,TILE_SIZE_Y*14,screen,dx,dy);
							if (keyboard[SDLK_F5] && !old_keyboard[SDLK_F5]) {
								ZEUS_used=true;
							} /* if */ 
						} /* if */ 
					} /* if */ 

				} /* if */ 

				if (live_character[1] ||
					live_character[0]) {
					if (SUBSTATE>72) {
						j=(SUBSTATE-72)*TILE_SIZE_X;
						if (j>dx) j=dx;
						if (j>0) {
							for(i=0;i<dy;i++) {
								memset(screen+i*dx,0,j);
							} /* if */ 
						} /* if */ 
					} /* if */ 
				} else {
					if (SUBSTATE>160) {
						j=(SUBSTATE-160)*TILE_SIZE_X;
						if (j>dx) j=dx;
						if (j>0) {
							for(i=0;i<dy;i++) {
								memset(screen+i*dx,0,j);
							} /* if */ 
						} /* if */ 
					} /* if */ 
				} /* if */ 

				drawstats(screen,dx,dy);

				if (live_character[1] ||
					live_character[0]) {
					if (SUBSTATE>=120) { 
						int tmp_x,tmp_y,heur,act_heur;
						int colision=T_WALL|T_LADDER_WALL|T_DOOR_WALL;

						/* Reinicializar la habitación: */ 
						HP_filter(map_x,map_y);
						loadroom(map,map_x,map_y);
						HP_restore(map_x,map_y);

						memset(col_buffer,0,dx*dy);	
						drawmap(screen,dx,dy,1);
						memset(screen,0,dx*dy);

						/* Calcular la nueva posición del personaje en el mapa: */ 
						character=1-character;
						tmp_x=8;
						tmp_y=4;
						act_heur=(15-7)*(15-7)+(10-3)*(10-3); 
						for(i=2;i<16;i++) {
							for(j=8;j<22;j++)  {
								if (iswall_or_blockingobject(j,i) &&
									iswall_or_blockingobject(j+1,i) &&
									!iswall_or_blockingobject(j,i-1) &&
									!iswall_or_blockingobject(j+1,i-1) &&
									!iswall_or_blockingobject(j,i-2) &&
									!iswall_or_blockingobject(j+1,i-2)
									&&
									(tiles[character_tile[character]+MASK_OFFSET]->
										coltest(GAME_VIEW_X+j*TILE_SIZE_X,GAME_VIEW_Y+(i-2)*TILE_SIZE_Y,
											TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx)&colision)==0
									) {
									heur=(15-j)*(15-j)+(9-i)*(9-i);
									if (heur<act_heur) {
										tmp_x=j;
										tmp_y=i;
										act_heur=heur;
									} /* if */ 
								} /* if */ 
							} /* for */ 
						} /* for */ 
						pers_x=tmp_x*TILE_SIZE_X;
						pers_y=(tmp_y-2)*TILE_SIZE_Y;
						pers_state=S_QUIET;
						pers_substate=0;
						pers_pos=0;
						n_fired_arrows=0;
						n_fired_mines=0;
						in_ladder=search_ladder(pers_x,pers_y); 

						if (fighting_demon!=0) {
							char tmp[256],tmp2[256];
	
							ReleaseGraphics();
							ReloadGraphics(dx,dy);

							Sound_release_music();
							sprintf(tmp,"demonroom1");
							sprintf(tmp2,"demonroom2");
							Sound_create_music(tmp2,tmp,0);
						} else {
							char tmp[80];
							Sound_release_music();
							if (map==0) sprintf(tmp,"castle");
								   else sprintf(tmp,"world");
							Sound_create_music(tmp,0,0);
						} /* if */ 
						old_room_demonroom=false;
						fighting_demon=0;
						STATE=4;
						SUBSTATE=0;
					} else {
						SUBSTATE++;			
					} /* if */ 
				} else {
					if (SUBSTATE>=208) {
						if (ZEUS_used) {
							map=0;
							map_x=5;
							map_y=12;

							n_fired_arrows=0;
							n_fired_mines=0;
							player_experience[0]=player_experience[0]=0;
							player_denergy[0]=player_energy[0]=player_max[0];
							player_denergy[1]=player_energy[1]=player_max[1];
							player_dexperience[0]=player_dexperience[0]=0;
							live_character[0]=live_character[1]=true;
							character=0;
							freezed=0;

							old_room_demonroom=false;
							pers_x=20*TILE_SIZE_X;
							pers_y=9*TILE_SIZE_Y;
							pers_right=true;
							pers_pos=0;
							pers_state=S_QUIET;
							pers_substate=0;
							current_weapon=-1;
							fighting_demon=0;
							to_enter_cut=0;
							mine_stepped=false;
							bible_counter=16;
							bible_subcounter=0;
							can_revive_character[0]=can_revive_character[1]=true;

							ReleaseGraphics();
							ReloadGraphics(dx,dy);

							Sound_release_music();
							Sound_create_music("castle",0,0);
							STATE=4;
							SUBSTATE=0;
						} else {
							STATE=0;
							SUBSTATE=0;
						} /* if */ 
					} else {
						SUBSTATE++;
					} /* if */ 
				} /* if */ 
			
			}

			if (keyboard[SDLK_ESCAPE] && !old_keyboard[SDLK_ESCAPE]) {
				OLDSTATE=STATE;
				MENUOPTION=0;
				STATE=19;
			} /* if */ 
			break;

		case 12: /* MIRANDO UNA TUMBA: */ 
			if (SUBSTATE==0) {
				char tmp[80],tmp2[80];
				Sound_release_music();
				/* Musica de cuando miras con la lupa: */ 
				sprintf(tmp,"tomb1");
				sprintf(tmp2,"tomb2");
				Sound_create_music(tmp,tmp2,0);
				SUBSTATE=1;
			} /* if */ 

			memset(screen,0,dx*dy);
			memset(col_buffer,0,dx*dy);	

			drawstats(screen,dx,dy);

			/* Dibujar lupa: */ 
			tiles[lupa_tile]->
				draw(GAME_VIEW_X+TILE_SIZE_X*4,GAME_VIEW_Y+TILE_SIZE_Y*2,
					 TILE_SIZE_X*24,TILE_SIZE_Y*12,screen,col_buffer,dx,dy,dx);

			if (map==1 && map_x==2 && map_y==7) 
				tile_print2("YOMAR",GAME_VIEW_X+TILE_SIZE_X*13+TILE_SIZE_X/2,GAME_VIEW_Y+TILE_SIZE_Y*6+TILE_SIZE_Y/2,screen,dx,dy);
			if (map==2 && map_x==4 && map_y==1) 
				tile_print2("ELOHIM",GAME_VIEW_X+TILE_SIZE_X*13,GAME_VIEW_Y+TILE_SIZE_Y*6+TILE_SIZE_Y/2,screen,dx,dy);
			if (map==3 && map_x==2 && map_y==4) 
				tile_print2("HAHAKLA",GAME_VIEW_X+TILE_SIZE_X*13-TILE_SIZE_X/2,GAME_VIEW_Y+TILE_SIZE_Y*6+TILE_SIZE_Y/2,screen,dx,dy);
			if (map==4 && map_x==4 && map_y==0) 
				tile_print2("BARECHET",GAME_VIEW_X+TILE_SIZE_X*12,GAME_VIEW_Y+TILE_SIZE_Y*6+TILE_SIZE_Y/2,screen,dx,dy);
			if (map==5 && map_x==1 && map_y==2) 
				tile_print2("HEOTYMEO",GAME_VIEW_X+TILE_SIZE_X*12,GAME_VIEW_Y+TILE_SIZE_Y*6+TILE_SIZE_Y/2,screen,dx,dy);
			if (map==6 && map_x==1 && map_y==3) 
				tile_print2("LEPHA",GAME_VIEW_X+TILE_SIZE_X*13+TILE_SIZE_X/2,GAME_VIEW_Y+TILE_SIZE_Y*6+TILE_SIZE_Y/2,screen,dx,dy);
			if (map==7 && map_x==4 && map_y==4) 
				tile_print2("NAWABRA",GAME_VIEW_X+TILE_SIZE_X*12+TILE_SIZE_X/2,GAME_VIEW_Y+TILE_SIZE_Y*6+TILE_SIZE_Y/2,screen,dx,dy);
			if (map==8 && map_x==3 && map_y==3) 
				tile_print2("ASCHER",GAME_VIEW_X+TILE_SIZE_X*13,GAME_VIEW_Y+TILE_SIZE_Y*6+TILE_SIZE_Y/2,screen,dx,dy);
			if (map==9 && map_x==1 && map_y==0) 
				tile_print2("XYWOLEH",GAME_VIEW_X+TILE_SIZE_X*12+TILE_SIZE_X/2,GAME_VIEW_Y+TILE_SIZE_Y*6+TILE_SIZE_Y/2,screen,dx,dy);
			if (map==10) 
				tile_print2("HAMALECH",GAME_VIEW_X+TILE_SIZE_X*12,GAME_VIEW_Y+TILE_SIZE_Y*6+TILE_SIZE_Y/2,screen,dx,dy);

			if ((!old_keyboard[WEAPON_KEY] && keyboard[WEAPON_KEY]) ||
				(!old_keyboard[SWORD_KEY] && keyboard[SWORD_KEY])) {
				STATE=13;
				SUBSTATE=0;
			} /* if */ 

			if (keyboard[SDLK_ESCAPE] && !old_keyboard[SDLK_ESCAPE]) {
				OLDSTATE=STATE;
				MENUOPTION=0;
				STATE=19;
			} /* if */ 
			break;
		case 13:
			if (!keyboard[WEAPON_KEY]) {
				STATE=4;
				SUBSTATE=1;
				{
					char tmp[80];
					Sound_release_music();
					if (map==0) sprintf(tmp,"castle");
						   else sprintf(tmp,"world");
					Sound_create_music(tmp,0,0);
				}
			} /* if */ 

			break;

		case 14:
			{
				int i,j;
				bool found;
				char buf[20]="0000 0000 0000 0000";
				int ypos[3]={8,10,12};
				int xpos[16]={10,11,12,13,15,16,17,18,20,21,22,23,25,26,27,28};
				int x,y;

				/* Entrando código!!!!: */ 
				memset(screen,0,dx*dy);
				tile_print("h F5 TO F9",TILE_SIZE_X*9,TILE_SIZE_Y*2,screen,dx,dy);

				tile_print("ENTER THE SECRET",TILE_SIZE_X*9,TILE_SIZE_Y*4,screen,dx,dy);
				tile_print("CODE AND PRESS",TILE_SIZE_X*9,TILE_SIZE_Y*5,screen,dx,dy);
				tile_print("RETURN KEY.",TILE_SIZE_X*9,TILE_SIZE_Y*6,screen,dx,dy);

				buf[0]=password[0]; buf[1]=password[1]; buf[2]=password[2]; buf[3]=password[3];
				buf[5]=password[4]; buf[6]=password[5]; buf[7]=password[6]; buf[8]=password[7];
				buf[10]=password[8]; buf[11]=password[9]; buf[12]=password[10]; buf[13]=password[11];
				buf[15]=password[12]; buf[16]=password[13]; buf[17]=password[14]; buf[18]=password[15];
				tile_print(buf,TILE_SIZE_X*10,TILE_SIZE_Y*8,screen,dx,dy);

				buf[0]=password[16]; buf[1]=password[17]; buf[2]=password[18]; buf[3]=password[19];
				buf[5]=password[20]; buf[6]=password[21]; buf[7]=password[22]; buf[8]=password[23];
				buf[10]=password[24]; buf[11]=password[25]; buf[12]=password[26]; buf[13]=password[27];
				buf[15]=password[28]; buf[16]=password[29]; buf[17]=password[30]; buf[18]=password[31];
				tile_print(buf,TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);

				buf[0]=password[32]; buf[1]=password[33]; buf[2]=password[34]; buf[3]=password[35];
				buf[5]=password[36]; buf[6]=password[37]; buf[7]=password[38]; buf[8]=password[39];
				buf[10]=password[40]; buf[11]=password[41]; buf[12]=password[42]; buf[13]=password[43];
				buf[15]=password[44]; buf[16]=' '; buf[17]=' '; buf[18]=' ';
				tile_print(buf,TILE_SIZE_X*10,TILE_SIZE_Y*12,screen,dx,dy);

				/* Pintar cursor: */ 
				x=xpos[password_pos%16];
				y=ypos[password_pos/16];
				B_rectangle2(screen,x*TILE_SIZE_X-1,y*TILE_SIZE_Y-1,TILE_SIZE_X,TILE_SIZE_Y,dx,180);

				if (SUBSTATE==0) {
					found=false;
					int slot=-1;
					/* Recuperar el juego a disco: */ 

					if (!old_keyboard[SDLK_F5] && keyboard[SDLK_F5]) slot=1;
					if (!old_keyboard[SDLK_F6] && keyboard[SDLK_F6]) slot=2;
					if (!old_keyboard[SDLK_F7] && keyboard[SDLK_F7]) slot=3;
					if (!old_keyboard[SDLK_F8] && keyboard[SDLK_F8]) slot=4;
					if (!old_keyboard[SDLK_F9] && keyboard[SDLK_F9]) slot=5;

					if (slot!=-1) {
						char tmp[80];
						FILE *fp;

						sprintf(tmp,"sgame%.2i.txt",slot);
						fp=f1open(tmp,"r",USERDATA);
						if (fp!=0) {
							int i;
							for(i=0;i<48;i++) {
								password[i]=fgetc(fp);
							} /* for */ 
							fclose(fp);
							Sound_play(S_select);
						} else {
							Sound_play(S_nocoins);
						} /* if */ 
					} /* if */ 

					if (!old_keyboard[SDLK_ESCAPE] && keyboard[SDLK_ESCAPE]) {
						STATE=0;
						SUBSTATE=0;
					} /* if */ 

					if (!old_keyboard[SDLK_RETURN] && keyboard[SDLK_RETURN]) {
						if (readpassword(password)) SUBSTATE=2;
											   else SUBSTATE=1;
					} /* if */ 
					if (!old_keyboard[SDLK_LEFT] && keyboard[SDLK_LEFT] && password_pos>0) password_pos--;
					if (!old_keyboard[SDLK_RIGHT] && keyboard[SDLK_RIGHT] && password_pos<44) password_pos++;
					if (!old_keyboard[SDLK_0] && keyboard[SDLK_0]) {
						password[password_pos]='0';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_1] && keyboard[SDLK_1]) {
						password[password_pos]='1';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_2] && keyboard[SDLK_2]) {
						password[password_pos]='2';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_3] && keyboard[SDLK_3]) {
						password[password_pos]='3';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_4] && keyboard[SDLK_4]) {
						password[password_pos]='4';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_5] && keyboard[SDLK_5]) {
						password[password_pos]='5';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_6] && keyboard[SDLK_6]) {
						password[password_pos]='6';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_7] && keyboard[SDLK_7]) {
						password[password_pos]='7';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_8] && keyboard[SDLK_8]) {
						password[password_pos]='8';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_9] && keyboard[SDLK_9]) {
						password[password_pos]='9';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_a] && keyboard[SDLK_a]) {
						password[password_pos]='A';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_b] && keyboard[SDLK_b]) {
						password[password_pos]='B';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_c] && keyboard[SDLK_c]) {
						password[password_pos]='C';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_d] && keyboard[SDLK_d]) {
						password[password_pos]='D';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_e] && keyboard[SDLK_e]) {
						password[password_pos]='E';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_f] && keyboard[SDLK_f]) {
						password[password_pos]='F';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_g] && keyboard[SDLK_g]) {
						password[password_pos]='G';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_h] && keyboard[SDLK_h]) {
						password[password_pos]='H';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_i] && keyboard[SDLK_i]) {
						password[password_pos]='I';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_j] && keyboard[SDLK_j]) {
						password[password_pos]='J';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_k] && keyboard[SDLK_k]) {
						password[password_pos]='K';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_l] && keyboard[SDLK_l]) {
						password[password_pos]='L';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_m] && keyboard[SDLK_m]) {
						password[password_pos]='M';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_n] && keyboard[SDLK_n]) {
						password[password_pos]='N';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_o] && keyboard[SDLK_o]) {
						password[password_pos]='O';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_p] && keyboard[SDLK_p]) {
						password[password_pos]='P';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_q] && keyboard[SDLK_q]) {
						password[password_pos]='Q';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_r] && keyboard[SDLK_r]) {
						password[password_pos]='R';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_s] && keyboard[SDLK_s]) {
						password[password_pos]='S';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_t] && keyboard[SDLK_t]) {
						password[password_pos]='T';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_u] && keyboard[SDLK_u]) {
						password[password_pos]='U';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_v] && keyboard[SDLK_v]) {
						password[password_pos]='V';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_w] && keyboard[SDLK_w]) {
						password[password_pos]='W';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_x] && keyboard[SDLK_x]) {
						password[password_pos]='X';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_y] && keyboard[SDLK_y]) {
						password[password_pos]='Y';
						password_pos++;
					} /* if */ 
					if (!old_keyboard[SDLK_z] && keyboard[SDLK_z]) {
						password[password_pos]='Z';
						password_pos++;
					} /* if */ 

					if (password_pos>44) password_pos=44;
				} /* if */ 

				if (SUBSTATE==1) {
					tile_print("THAT IS THE WRONG",TILE_SIZE_X*9,TILE_SIZE_Y*17,screen,dx,dy);
					tile_print("CODE",TILE_SIZE_X*9,TILE_SIZE_Y*19,screen,dx,dy);
					if (!old_keyboard[SDLK_RETURN] && keyboard[SDLK_RETURN]) SUBSTATE=0;
				} /* if */ 
				if (SUBSTATE>=2) {
					tile_print("MY MEMORY HAS BEEN",TILE_SIZE_X*9,TILE_SIZE_Y*17,screen,dx,dy);
					tile_print("BROUGHT BACK!",TILE_SIZE_X*9,TILE_SIZE_Y*18,screen,dx,dy);
					tile_print("DEFEAT GALIOUS AND",TILE_SIZE_X*9,TILE_SIZE_Y*19,screen,dx,dy);
					tile_print("SAVE PAMPERSE AS",TILE_SIZE_X*9,TILE_SIZE_Y*20,screen,dx,dy);
					tile_print("SOON AS POSSIBLE! GO!",TILE_SIZE_X*9,TILE_SIZE_Y*21,screen,dx,dy);
					SUBSTATE++;

					if (SUBSTATE>66) {
						j=(SUBSTATE-66)*TILE_SIZE_X;
						if (j>dx) j=dx;
						if (j>0) {
							for(i=0;i<dy;i++) {
								memset(screen+i*dx,0,j);
							} /* if */ 
						} /* if */ 
					} /* if */ 

					if (SUBSTATE>=112) {
						if (map==0) sprintf(tmp,"castle");
							   else sprintf(tmp,"world");
						Sound_create_music(tmp,0,0);
						STATE=4;
						SUBSTATE=0;
						newgame();
					} /* if */ 
				} /* if */ 
			}

			break;

		case 15:
			{
				char tmp[80],filename[80];
				FILE *fp;
				char *prefixes[11]={"map","w01","w02","w03","w04","w05","w06","w07","w08","w09","w10"};

				/* Developers entry code: */ 
				memset(screen,0,dx*dy);

				tile_print("START ROOM:",TILE_SIZE_X*9,TILE_SIZE_Y*2,screen,dx,dy);
	
				sprintf(tmp,"ROOM X: %.2i",developer_start_x);			
				tile_print(tmp,TILE_SIZE_X*9,TILE_SIZE_Y*4,screen,dx,dy);
				sprintf(tmp,"ROOM Y: %.2i",developer_start_y);			
				tile_print(tmp,TILE_SIZE_X*9,TILE_SIZE_Y*5,screen,dx,dy);
				sprintf(tmp,"MAP: %.2i",developer_start_map);			
				tile_print(tmp,TILE_SIZE_X*9,TILE_SIZE_Y*6,screen,dx,dy);

				if (keyboard[UP_KEY] && !old_keyboard[UP_KEY]) {
					if (developer_start_y>0) developer_start_y--;
				} /* if */ 
				if (keyboard[DOWN_KEY] && !old_keyboard[DOWN_KEY]) {
					if (developer_start_y<15) developer_start_y++;
				} /* if */ 
				if (keyboard[LEFT_KEY] && !old_keyboard[LEFT_KEY]) {
					if (developer_start_x>0) developer_start_x--;
				} /* if */ 
				if (keyboard[RIGHT_KEY] && !old_keyboard[RIGHT_KEY]) {
					if (developer_start_x<14) developer_start_x++;
				} /* if */ 
				if (keyboard[SDLK_PAGEDOWN] && !old_keyboard[SDLK_PAGEDOWN]) {
					if (developer_start_map>0) developer_start_map--;
				} /* if */ 
				if (keyboard[SDLK_PAGEUP] && !old_keyboard[SDLK_PAGEUP]) {
					if (developer_start_map<10) developer_start_map++;
				} /* if */ 

				sprintf(filename,"rooms/%s%.2i%.2i.txt",prefixes[developer_start_map],
														developer_start_x,
														developer_start_y);
				fp=f1open(filename,"r",GAMEDATA);
				if (fp!=0) {
					tile_print("PRESS SPACE TO START",TILE_SIZE_X*9,TILE_SIZE_Y*8,screen,dx,dy);
					if (keyboard[SDLK_SPACE] && !old_keyboard[SDLK_SPACE]) {
						strcpy(password,"UR3QUR5FUG4F123NUL7WHUD4VT7WHUD4VYTFUR3FURS76");
						STATE=3;
						SUBSTATE=30;
					} /* if */ 
					fclose(fp);
				} else {
					tile_print("NOT VALID ROOM",TILE_SIZE_X*9,TILE_SIZE_Y*8,screen,dx,dy);
				} /* if */ 
			}
			break;
		case 16:
			/* INTER-PANTALLA */ 
			if (inter_screen==0) {
				STATE=4;
			} else {
				memset(screen,0,dx*dy);
				inter_screen--;
			} /* if */ 
			break;

		case 17:
			{
				/* PANTALLA DE HISTORIA: */ 
				int i;
				int y=0,tmpy;
				char *text[12]={
					"           STORY",
					"POPOLON WENT INTO MT.ATOS TO",
					"RESCUE APHRODITE KIDNAPPED",
					"BY HUDNOS. BUT IT WAS A TRAP",
					"MADE BY GALIOUS. HE ATTACKED",
					"THE CASTLE DURING POPOLON'S",
					"ABSENCE AND ALSO KIDNAPPED",
					"AND IMPRISONED A BABY FROM",
					"THE HEAVEN WHO WAS SUPPOSED",
					"TO BE BORN TO POPOLON AND",
					"APHRODITE SEVERAL YEARS",
					"LATER..."};

				if (SUBSTATE==0) {
					/* musica: */ 
					Sound_create_music("story",0,0);
				} /* if */ 

				SUBSTATE++;

				if (SUBSTATE<27*TILE_SIZE_Y) y=27*TILE_SIZE_Y-SUBSTATE;
				if (SUBSTATE>=27*TILE_SIZE_Y && SUBSTATE<39*TILE_SIZE_Y) y=0;
				if (SUBSTATE>=39*TILE_SIZE_Y) y=39*TILE_SIZE_Y-SUBSTATE;

				memset(screen,0,dx*dy);
				for(i=0;i<12;i++) {
					tmpy=y+i*TILE_SIZE_Y*2;
					if (tmpy<25*TILE_SIZE_Y && tmpy>(-TILE_SIZE_Y)) {
						tile_print(text[i],GAME_VIEW_X+TILE_SIZE_X*2,tmpy,screen,dx,dy);
					} /* if */ 
				} /* for */ 

				if (SUBSTATE>=64*TILE_SIZE_Y ||
					(keyboard[SDLK_SPACE]  && !old_keyboard[SDLK_SPACE])) {
					/* musica: */ 
					Sound_release_music();
					STATE=0;
					SUBSTATE=0;
				} /* if */ 
			}
			break;
		
		case 18:
			/* SECUENCIA DEL FINAL!!!!: */ 
			SUBSTATE++;
			if (SUBSTATE<40) {
				int i,j;
			
				j=(SUBSTATE/4)*TILE_SIZE_X;
				for(i=0;i<room_size_y*TILE_SIZE_Y;i++) {
					memset(screen+(GAME_VIEW_Y+i)*dx+GAME_VIEW_X,0,j);
					memset(screen+(GAME_VIEW_Y+i)*dx+GAME_VIEW_X+(room_size_x*TILE_SIZE_X-j),0,j);
				} /* if */ 
				for(i=0;i<TILE_SIZE_Y;i++) {
					memset(screen+(GAME_VIEW_Y+(SUBSTATE/4)*TILE_SIZE_Y+i)*dx+GAME_VIEW_X,0,room_size_x*TILE_SIZE_X);
					memset(screen+(GAME_VIEW_Y+(room_size_y-((SUBSTATE/4)+1))*TILE_SIZE_Y+i)*dx+GAME_VIEW_X,0,room_size_x*TILE_SIZE_X);
				} /* if */ 
				for(i=0;i<n_objects;i++) {
					if (object[i].type==T_PAMPERSE) {
						tiles[182]->
							draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);		
					} /* if */ 
				} /* for */ 
				drawcharacter(screen,dx,dy);
			} /* if */ 

			if (SUBSTATE==96) {
				int i;

				Sound_release_music();
				memset(screen,0,dx*dy);

				for(i=0;i<8;i++) {
					if (tiles[demon_tile+i]!=0) delete tiles[demon_tile+i];
				} /* for */ 
				tiles[demon_tile]=new CTile(final_bmp,0,0,TILE_SIZE_X*20,TILE_SIZE_Y*14,T_NADA);

				tiles[demon_tile+1]=new CTile(final_bmp,TILE_SIZE_X*20,0,TILE_SIZE_X*6,TILE_SIZE_Y*2,T_NADA);
				tiles[demon_tile+2]=new CTile(final_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*2,TILE_SIZE_X*6,TILE_SIZE_Y*2,T_NADA);
				tiles[demon_tile+3]=new CTile(final_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*4,TILE_SIZE_X*6,TILE_SIZE_Y*2,T_NADA);
				tiles[demon_tile+4]=new CTile(final_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_NADA);
				tiles[demon_tile+5]=new CTile(final_bmp,TILE_SIZE_X*26,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_NADA);
				tiles[demon_tile+6]=new CTile(final_bmp,TILE_SIZE_X*28,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_NADA);
				tiles[demon_tile+7]=new CTile(final_bmp,TILE_SIZE_X*30,0,TILE_SIZE_X*8,TILE_SIZE_Y*4,T_NADA);
			} /* if */ 

			if (SUBSTATE==128) {
				char str[256];

				Sound_release_music();
				sprintf(str,"end1");
				Sound_create_music(str,0,0);
			} /* if */ 
			if (SUBSTATE>=128 && SUBSTATE<464) {
				memset(screen,0,dx*dy);
				/* POPOLON Y APHRODITE: */ 
				tiles[demon_tile+1+((SUBSTATE>>4)%2)]->
					draw(GAME_VIEW_X+13*TILE_SIZE_X+TILE_SIZE_X/2,GAME_VIEW_Y+8*TILE_SIZE_Y,
							screen,col_buffer,dx,dy,dx);

				/* PAMPERSE: */ 
				{
					int y=10*TILE_SIZE_Y-(SUBSTATE-256);
					if (y>=10*TILE_SIZE_Y) y=10*TILE_SIZE_Y;
					if (y<=8*TILE_SIZE_Y) y=8*TILE_SIZE_Y;
					tiles[demon_tile+3]->
						draw(GAME_VIEW_X+13*TILE_SIZE_X+TILE_SIZE_X/2,GAME_VIEW_Y+y,
								screen,col_buffer,dx,dy,dx);
				}

				/* CASTILLO: */ 
				tiles[demon_tile]->
					draw(GAME_VIEW_X+6*TILE_SIZE_X,GAME_VIEW_Y,
							screen,col_buffer,dx,dy,dx);
				/* PETARDOS: */ 
				{
					int i;
					i=(SUBSTATE>>4)%4;

					if (i==0) {
						tiles[demon_tile+6]->
							draw(GAME_VIEW_X+18*TILE_SIZE_X+TILE_SIZE_X/2,GAME_VIEW_Y+1*TILE_SIZE_Y+TILE_SIZE_Y/2,
									screen,col_buffer,dx,dy,dx);
					} /* if */ 
					if (i==2) {
						tiles[demon_tile+6]->
							draw(GAME_VIEW_X+12*TILE_SIZE_X,GAME_VIEW_Y+1*TILE_SIZE_Y+TILE_SIZE_Y/2,
									screen,col_buffer,dx,dy,dx);
					} /* if */ 
				}

				/* BANDERA: */ 
				if (SUBSTATE<196) {
					tiles[demon_tile+4]->
						draw(GAME_VIEW_X+16*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y+TILE_UNIT*2,
								screen,col_buffer,dx,dy,dx);
				} /* if */ 
				if (SUBSTATE>=192 && SUBSTATE<224) {
					tiles[demon_tile+4]->
						draw(GAME_VIEW_X+16*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y+(SUBSTATE-192)+TILE_UNIT*2,
								screen,col_buffer,dx,dy,dx);
				} /* if */ 
				if (SUBSTATE>=224 && SUBSTATE<256) {
					tiles[demon_tile+5]->
						draw(GAME_VIEW_X+16*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y+(32-(SUBSTATE-224))+TILE_UNIT*2,
								screen,col_buffer,dx,dy,dx);
				} /* if */ 
				if (SUBSTATE>=256) {
					tiles[demon_tile+5]->
						draw(GAME_VIEW_X+16*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y+TILE_UNIT*2,
								screen,col_buffer,dx,dy,dx);
				} /* if */ 

				/* GRITOS: */ 
				if (SUBSTATE>=320) {
					tiles[demon_tile+7]->
						draw(GAME_VIEW_X+20*TILE_SIZE_X,GAME_VIEW_Y+9*TILE_SIZE_Y,
								screen,col_buffer,dx,dy,dx);
				} /* if */ 
			} /* if */ 

			if (SUBSTATE==464) {
				memset(screen,0,dx*dy);
				Sound_release_music();
			} /* if */ 

			if (SUBSTATE==512) {
				char str[256];

				sprintf(str,"end2");
				Sound_create_music(str,0,0);
			} /* if */ 

			if (SUBSTATE>=512) {
				int i;
				int y=0,tmpy;
				char *text[68]={
					"          EPILOGUE",
					"THANKS TO YOU,THE BABY WAS",
					"RESCUED SAFELY.THE KINGDOM",
					"TOOK BACK PACE AND VIGOR.",
					"POPOLON WAS AWARDED THE",
					"CROWN FOR HIS ACHIEVEMENT.IT",
					"WAS DECIDED THAT HE PROTECT",
					"THE KINGDOM WITH APHRODITE",
					"AS HIS WIFE FOR THE REST OF",
					"THEIR LIVES.THIS LEGEND WILL",
					"BE TOLD AGAIN WHEN THEY ARE",
					"APT TO LOSE BELIEF IN LOVE.",
					"",
					"",
					"",
					"",
					"",
					"",
					"",
					"",
					"       ORIGINAL  STAFF",
					"",
					"       MAIN PROGRAMMER",
					"          Y.HARUKI",
					"",
					"       SUB  PROGRAMMER",
					"           I.AKADA",
					"           M.OZAWA",
					"            T.OKA",
					"         Y.HIRONAKA",
					"           K.OZAWA",
					"         S.YOSHINAWA",
					"",
					"      GRAPHIC DESIGNER",
					"          R.SHOGAKI",
					"         C.TANIGAKI",
					"          T.MIYOSHI",
					"         K.SHIRAKAGI",
					"",
					"       SOUND  CREATOR",
					"          K.UEHARA",
					"",
					"",
					"    PLANNER AND DIRECTOR",
					"          R.SHOGAKI",
					"",
					"",
					"",
					"",
					"   WE PROMISE TO CONTINUE",
					"   SENDING ROMANTIC GAME",
					"   MESSAGES.",
					"",
					"   THANK YOU VERY MUCH",
					"   FOR YOUR JOINING IN",
					"   THIS STORY.",
					"",
					"",
					"",
					"",
					"",
					"",
					"",
					"    PRESENTED BY KONAMI",
					"    ORIGINALLY AT  1987",
					"",
					"    REMAKE PRESENTED BY ",
					"   SANTI ONTAÑON AT 2002"};


				if (SUBSTATE<(512+27*TILE_SIZE_Y)) y=27*TILE_SIZE_Y-(SUBSTATE-512);
				if (SUBSTATE>=(512+27*TILE_SIZE_Y) && SUBSTATE<(512+39*TILE_SIZE_Y)) y=0;
				if (SUBSTATE>=(512+39*TILE_SIZE_Y) && SUBSTATE<(512+157*TILE_SIZE_Y)) y=39*TILE_SIZE_Y-(SUBSTATE-512);
				if (SUBSTATE>=(512+157*TILE_SIZE_Y) && SUBSTATE<(512+169*TILE_SIZE_Y)) y=-118*TILE_SIZE_Y;
				if (SUBSTATE>=(512+169*TILE_SIZE_Y)) y=51*TILE_SIZE_Y-(SUBSTATE-512);

				memset(screen,0,dx*dy);
				for(i=0;i<68;i++) {
					tmpy=y+i*TILE_SIZE_Y*2;
					if (tmpy<25*TILE_SIZE_Y && tmpy>(-TILE_SIZE_Y)) {
						tile_print(text[i],GAME_VIEW_X+TILE_SIZE_X*2,tmpy,screen,dx,dy);
					} /* if */ 
				} /* for */ 
			} /* if */ 

			if (SUBSTATE>=3500) {
				Sound_release_music();
				STATE=0;
				SUBSTATE=0;
			} /* if */ 
			break;

		case 19:
			/* MENU DE OPCIONES: */ 
			{
				char tmp[80];
				char *letter[15]={"A","B","C","D","E", "F","G","H","I","J", "K","L","M","N","O"};

				memset(screen,0,dx*dy);

				B_rectangle(screen,TILE_SIZE_X*10,TILE_SIZE_Y*5+MENUOPTION*TILE_SIZE_Y*2,
								   TILE_SIZE_X*19,TILE_SIZE_Y,dx,47);

				tile_print("OPTIONS MENU",TILE_SIZE_X*14,TILE_SIZE_Y*2,screen,dx,dy);
				tile_print("QUIT GAME",TILE_SIZE_X*15,TILE_SIZE_Y*5,screen,dx,dy);
				sprintf(tmp,"MUSIC VOLUME: %.3i",(100*music_volume)/127);
				tile_print(tmp,TILE_SIZE_X*11,TILE_SIZE_Y*7,screen,dx,dy);
				sprintf(tmp,"SFX VOLUME: %.3i",(100*sfx_volume)/127);
				tile_print(tmp,TILE_SIZE_X*12,TILE_SIZE_Y*9,screen,dx,dy);

				if (map==0) sprintf(tmp,"CASTLE    %s%i",letter[map_x],map_y+1);
					   else sprintf(tmp,"WORLD %.2i  %s%i",map,letter[map_x],map_y+1);
				tile_print(tmp,TILE_SIZE_X*13,TILE_SIZE_Y*12,screen,dx,dy);

				if (keyboard[SDLK_ESCAPE] && !old_keyboard[SDLK_ESCAPE]) {
					STATE=OLDSTATE;
					guardar_configuracion("MoG.cfg");
				} /* if */ 
				if (keyboard[DOWN_KEY] && !old_keyboard[DOWN_KEY] && MENUOPTION<2) MENUOPTION++;
				if (keyboard[UP_KEY] && !old_keyboard[UP_KEY] && MENUOPTION>0) MENUOPTION--;

				if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY] && MENUOPTION==0) {
					Sound_release_music();
					STATE=0;
					SUBSTATE=0;
					guardar_configuracion("MoG.cfg");
				} /* if */ 

				if (keyboard[RIGHT_KEY] && MENUOPTION==1 && music_volume<127) {
					music_volume++;
//					Mix_VolumeMusic(music_volume);
				} /* if */ 
				if (keyboard[LEFT_KEY] && MENUOPTION==1 && music_volume>0) {
					music_volume--;
//					Mix_VolumeMusic(music_volume);
				} /* if */ 
				if (keyboard[RIGHT_KEY] && MENUOPTION==2 && sfx_volume<127) {
					sfx_volume++;
					SetSFXVolume(sfx_volume);
				} /* if */ 
				if (keyboard[LEFT_KEY] && MENUOPTION==2 && sfx_volume>0) {
					sfx_volume--;
					SetSFXVolume(sfx_volume);
				} /* if */ 

			}
			break;

	} /* switch */ 

	old_room_demonroom=room_demonroom;

	/* Borrar los bordes de la pantalla: */ 
	if (STATE==4 || STATE==16) {
		B_rectangle(screen,0,0,GAME_VIEW_X,dy,dx,0);
		B_rectangle(screen,GAME_VIEW_X+32*TILE_SIZE_X,0,dx-(GAME_VIEW_X+32*TILE_SIZE_X),dy,dx,0);
		B_rectangle(screen,GAME_VIEW_X,GAME_VIEW_Y+20*TILE_SIZE_Y,32*TILE_SIZE_X,dy-(GAME_VIEW_Y+20*TILE_SIZE_Y),dx,0);
	} /* if */ 


	/* Control de pantalla: */ 
	if (STATE==4 || STATE==16) {
		int zx,zy;

		zx=zoom;
		zy=int((float(zoom)*(400.0-float(GAME_VIEW_Y)))/640.0);

		logic_dx[0]=physic_dx[0]=640;
		logic_dy[0]=physic_dy[0]=GAME_VIEW_Y;
		logic_x[0]=physic_x[0]=0;
		logic_y[0]=physic_y[0]=0;

		logic_dx[1]=zx;
		logic_dy[1]=zy;
		logic_x[1]=max(min(640-zx,GAME_VIEW_X+pers_x-(zx/2-TILE_SIZE_X)),0);
		logic_y[1]=max(min(400-zy,GAME_VIEW_Y+pers_y-zy/2),GAME_VIEW_Y);

		physic_x[1]=0;
		physic_y[1]=GAME_VIEW_Y;
		physic_dx[1]=640;
		physic_dy[1]=400-GAME_VIEW_Y;
	
	} else {
		logic_dx[0]=physic_dx[0]=640;
		logic_dy[0]=physic_dy[0]=400;
		logic_x[0]=physic_x[0]=0;
		logic_y[0]=physic_y[0]=0;

		logic_dx[1]=physic_dx[1]=0;
	} /* if */ 

	/* Viejo estado del teclado: */ 
	for(int i=0;i<SDLK_LAST;i++) old_keyboard[i]=keyboard[i];

	cycle++;
} /* GameCycle */ 


void drawcharacter(BYTE *screen,int dx,int dy)
{
	int oldc=character;

	if (hit_time>0 && pers_pos<9) {
		if (((hit_time>>1)&0x01)!=0) character=2;
								else character=0;
	} /* if */ 
	if (pers_right) {
		if (to_enter_cut==0) {
			tiles[character_tile[character]+pers_pos]->
				draw(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} else {
			tiles[character_tile[character]+pers_pos]->
				draw_cut(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx,to_enter_cut);
		} /* if */ 
		if (sword) {
			tiles[sword_tile[oldc]]->
				draw(GAME_VIEW_X+sword_x,GAME_VIEW_Y+sword_y,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
			tiles[sword_tile[oldc]]->
				draw_only_mask(GAME_VIEW_X+sword_x,GAME_VIEW_Y+sword_y,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2+TILE_UNIT,col_buffer,dx,dy,dx);
			tiles[sword_tile[oldc]]->
				draw_only_mask(GAME_VIEW_X+sword_x,GAME_VIEW_Y+sword_y,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2-TILE_UNIT,col_buffer,dx,dy,dx);
		} /* if */ 
	} else {
		if (to_enter_cut==0) {
			tiles[character_tile_inv[character]+pers_pos]->
				draw(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} else {
			tiles[character_tile_inv[character]+pers_pos]->
				draw_cut(GAME_VIEW_X+pers_x,GAME_VIEW_Y+pers_y,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx,to_enter_cut);
		} /* if */ 
		if (sword) {
			tiles[sword_tile[oldc]+1]->
				draw(GAME_VIEW_X+sword_x,GAME_VIEW_Y+sword_y,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
			tiles[sword_tile[oldc]+1]->
				draw_only_mask(GAME_VIEW_X+sword_x,GAME_VIEW_Y+sword_y,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2+TILE_UNIT,col_buffer,dx,dy,dx);
			tiles[sword_tile[oldc]+1]->
				draw_only_mask(GAME_VIEW_X+sword_x,GAME_VIEW_Y+sword_y,
					 TILE_SIZE_X*2,TILE_SIZE_Y*2-TILE_UNIT,col_buffer,dx,dy,dx);
		} /* if */ 
	} /* if */ 
	character=oldc;
} /* drawcharacter */ 
