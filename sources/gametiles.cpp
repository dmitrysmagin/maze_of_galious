#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "sound.h"
#include "MOGtypes.h"

#include "drawing.h"
#include "bitmaps.h"
#include "sprites.h"
#include "tiles.h"

#include "mog.h"


extern SDL_Surface *screen;

/* Paths: */ 

extern char **g_paths;
extern int n_g_paths,act_g_path;
extern char *g_path;

/* Gráficos: */ 
extern Bitmap *konami_bmp,*menu_bmp,*tiles_bmp,*tiles2_bmp,*enemy_bmp,*enemy2_bmp,*final_bmp;
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
extern int n_fired_arrows;

extern int n_arrows,n_coins,n_keys;
extern int player_energy[2],player_experience[2],player_max[2];
extern int player_denergy[2],player_dexperience[2];
extern bool cross,world_key;
extern int next_world;
extern bool world_doors_open[10];

extern int fighting_demon;

extern int room_size_x,room_size_y;
extern int *room_tiles;

extern int wall_tile;
extern int stone_tile,ladder_tile,wdoor_tile,keydoor_tile,stats_tile;
extern int coin_tile,arrow_tile,key_tile,door_tile;
extern int character_tile[3],character_tile_inv[3];
extern int pause_tile[2];
extern int colision_tile;
extern int sword_tile[2],explosion_tile;
extern int worm_tile,bat_tile,skeleton_tile,smoke_tile,slime_tile,bouncingball_tile;
extern int watermonster_tile,jumpingbush_tile,bluespider_tile,whitefireball_tile;
extern int fstone_tile,knight_tile,blob_tile,bambu_tile,fairy_tile;
extern int porcupine_tile,black_tile,witch_tile,whitebear_tile,feet_tile,redjumper_tile;
extern int livingwall_tile,megabat_tile,lava_tile,piranha_tile,whitestar_tile;
extern int items_tile,armas_tile,tomb_tile,wkeydoor_tile,wnokeydoor_tile;
extern int lever_tile,spider_tile,knighthead_tile,chicken_tile,rockman_tile,cloud_tile;
extern int demonsign_tile,lupa_tile,demonsmoke_tile,demon_tile,bfly_tile;
extern int armourarrow_tile,ghost_tile,head_tile,worm2_tile,octopus_tile,hairy_tile;
extern int waterdragon_tile,waterbug_tile,bird_tile,stoneman_tile,pacman_tile,reddragon_tile;
extern int owl_tile,greenmonkey_tile,plant_tile,transformer_tile,flame_tile,witch2_tile;
extern int cyclops_tile,lcloud_tile,glizard_tile,snake_tile,bdemon_tile,gorilla_tile;
extern int pamperseball_tile;

extern int worm_state_tile[28];
extern int worm_state_tile_i[28];
extern int worm_state_y[28];


/* Efectos de sonido: */ 
extern SOUNDT S_pause,S_death,S_gameover,S_worldkey,S_entering;
extern SOUNDT S_gamestart,S_jump,S_sword,S_swordhit;
extern SOUNDT S_item,S_ah,S_ah2,S_powerup,S_enemyhit,S_door;
extern SOUNDT S_enemykill,S_ladderdisapear,S_armourhit;
extern SOUNDT S_enterwater,S_select,S_firearrow,S_firearrow2;
extern SOUNDT S_waterhit,S_fall,S_nocoins,S_stones,S_blob,S_shield;
extern SOUNDT S_bell,S_lavahit,S_flapwings,S_rotatedoor,S_demon2ball;
extern SOUNDT S_appearing,S_firebreath,S_F1,S_karrow;
extern SOUNDT S_jumptree,S_skeletonhit,S_enemybullet,S_headbullet;
extern SOUNDT S_dropmine,S_firefire,S_fireball,S_fireball2,S_wdoor;
extern SOUNDT S_door2,S_demonhit,S_bearbullet,S_chickenbomb;
extern SOUNDT S_colormonster,S_waterstone,S_demon4jump,S_rockman,S_mine;
extern SOUNDT S_bible,S_demon1bones,S_demon1jump,S_owl,S_demon7bullet;
extern SOUNDT S_demon8bullet,S_flamebullet,S_snakebullet,S_gorilla,S_lizardtongue;
extern SOUNDT S_bdemonbullet,S_lightning;

/* Control de pantalla: */ 
extern int logic_dx[4],logic_dy[4],logic_x[4],logic_y[4];
extern int physic_dx[4],physic_dy[4],physic_x[4],physic_y[4];

extern int zoom;


void init_paths(void);
void newgame(void);
 

void ReloadGraphics(int dx,int dy)
{
	int i;

	/* Gráficos: */ 
	konami_bmp=load_bitmap("konami.pcx");

	menu_bmp=load_bitmap("start.pcx");
	tiles_bmp=load_bitmap("tiles.pcx");
	tiles2_bmp=load_bitmap("tiles2.pcx");
	enemy_bmp=load_bitmap("enemy.pcx");
	enemy2_bmp=load_bitmap("enemy2.pcx");
	final_bmp=load_bitmap("final.pcx");

	if (konami_bmp==0 ||
		menu_bmp==0 ||
		tiles_bmp==0 ||
		tiles2_bmp==0 ||
		enemy_bmp==0 ||
		enemy2_bmp==0 ||
		final_bmp==0) {
		fprintf(stderr,"ERROR: some image buffers cannot be created!\n");
		return;
	} /* if */ 

	if (konami_bmp->get_image()==0 ||
		menu_bmp->get_image()==0 ||
		tiles_bmp->get_image()==0 ||
		tiles2_bmp->get_image()==0 ||
		enemy_bmp->get_image()==0 ||
		enemy2_bmp->get_image()==0 ||
		final_bmp->get_image()==0) {
		fprintf(stderr,"ERROR: some graphic files cannot be found!\n");
		return;
	} /* if */ 


	get_palette();
	
	col_buffer=new BYTE[dx*dy];
	memset(col_buffer,0,dx*dy);	

	n_tiles=0;
	tiles=new CTile *[MAX_TILES];
	for(i=0;i<MAX_TILES;i++) tiles[i]=0;

	/* Fondos: */ 
	/* Ladrillos azules:*/ 
	tiles[n_tiles++]=new CTile(tiles_bmp,			0, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 0 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);

	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*2, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*3, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);

	/* Ladrillos rojos:*/ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*4, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 4 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*5, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*6, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*7, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*4, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*5, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*6, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*7, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*5,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*7,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*5,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*7,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 20 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*11, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*12, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*13, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);

	/* Hojas verdes:*/ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*14, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 24 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*15, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*16, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*17, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*14, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*15, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*16, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*17, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*15,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*17,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*15,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*17,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*18, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 40 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*19, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*20, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*21, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);

	/* Rocas rojas:*/ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*25, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 44 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*26, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*27, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*28, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*25, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*26, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*27, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*28, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*25,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*27,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*25,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*27,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*29, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 60 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*31, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*32, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);

	/* Columna azul: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, 0, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 64 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, 0,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, 0,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);

	/* Columna roja: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*20, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 70 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*21, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*21,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*21,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);

	/* Bandera amarilla: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*2, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 76 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*3, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*2,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*3,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*2,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*3,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);

	/* Cortina izquierda: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*10, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 82 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*11, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*10,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*11,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*10,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*11,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);

	/* Cortina derecha: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*12, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 88 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*13, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*12,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*13,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*12,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*13,TILE_SIZE_Y*3, TILE_SIZE_X, TILE_SIZE_Y, 0);

	/* Ventana: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*18,TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 94 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*19,TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*19,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);

	/* Cadena: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*22, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 98 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*22, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);

	/* Grieta: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*23, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 101 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*24, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*23, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*24, TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);

	/* Paredes: */ 
	/* Ladrillos: */ 
	wall_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,          0,TILE_SIZE_Y*4, TILE_SIZE_X, TILE_SIZE_Y, T_WALL);	/* 105 */ 

	/* Ladrillos sobre escaleras: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X,TILE_SIZE_Y*4, TILE_SIZE_X, TILE_SIZE_Y, T_LADDER_WALL);	/* 106 */ 

	/* Armadura-left: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*6, TILE_SIZE_Y*4, TILE_SIZE_X*2, TILE_SIZE_Y*3, T_WALL);	/* 107 */ 

	/* Armadura-right: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*8, TILE_SIZE_Y*4, TILE_SIZE_X*2, TILE_SIZE_Y*3, T_WALL);	/* 108 */ 

	/* Pausa 0: */ 
	pause_tile[0]=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*20, TILE_SIZE_Y*19, TILE_SIZE_X*2+2*TILE_UNIT, TILE_SIZE_Y*3, T_WALL);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*22+2*TILE_UNIT, TILE_SIZE_Y*19, TILE_SIZE_X*2+2*TILE_UNIT, TILE_SIZE_Y*3, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*24+4*TILE_UNIT, TILE_SIZE_Y*19, TILE_SIZE_X*2+2*TILE_UNIT, TILE_SIZE_Y*3, 0);

	/* Pausa 1: */ 
	pause_tile[1]=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*26+6*TILE_UNIT, TILE_SIZE_Y*19, TILE_SIZE_X*2+2*TILE_UNIT, TILE_SIZE_Y*3, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*28+8*TILE_UNIT, TILE_SIZE_Y*19, TILE_SIZE_X*2+2*TILE_UNIT, TILE_SIZE_Y*3, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30+10*TILE_UNIT, TILE_SIZE_Y*19, TILE_SIZE_X*2+2*TILE_UNIT, TILE_SIZE_Y*3, 0);

	tomb_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*36, TILE_SIZE_Y*2, TILE_SIZE_X*2, TILE_SIZE_Y*2, 0);

	/* Puertas con llave de los mundos: */ 
	wkeydoor_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*12, TILE_SIZE_Y*17, TILE_SIZE_X*1, TILE_SIZE_Y*3, T_DOOR_WALL);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*13, TILE_SIZE_Y*17, TILE_SIZE_X*2, TILE_SIZE_Y*3, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*15, TILE_SIZE_Y*17, TILE_SIZE_X*3, TILE_SIZE_Y*3, 0);


	/* Columna azul sin marcas: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*8, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);	/* 119 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*9, 0, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*8,TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*9,TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*8,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*9,TILE_SIZE_Y*2, TILE_SIZE_X, TILE_SIZE_Y, 0);

	/* Decoración de mundos: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*33, 0, TILE_SIZE_X,TILE_SIZE_Y, T_WALL);	/* 125 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*33, TILE_SIZE_Y, TILE_SIZE_X,TILE_SIZE_Y, T_LADDER_WALL);

	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*34, 0, TILE_SIZE_X,TILE_SIZE_Y, 0);	/* 127 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*35, 0, TILE_SIZE_X,TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*34, TILE_SIZE_Y, TILE_SIZE_X,TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*35, TILE_SIZE_Y, TILE_SIZE_X,TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*36, 0, TILE_SIZE_X,TILE_SIZE_Y, 0);	
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*37, 0, TILE_SIZE_X,TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*36, TILE_SIZE_Y, TILE_SIZE_X,TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*37, TILE_SIZE_Y, TILE_SIZE_X,TILE_SIZE_Y, 0);

	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*38, 0, TILE_SIZE_X,TILE_SIZE_Y, 0); /* 135 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*39, 0, TILE_SIZE_X,TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*38, TILE_SIZE_Y, TILE_SIZE_X,TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*39, TILE_SIZE_Y, TILE_SIZE_X,TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*38, TILE_SIZE_Y*2, TILE_SIZE_X,TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*39, TILE_SIZE_Y*2, TILE_SIZE_X,TILE_SIZE_Y, 0);

	/* Agua: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*18, TILE_SIZE_Y*3, TILE_SIZE_X,TILE_SIZE_Y, T_WATER); /* 141 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*19, TILE_SIZE_Y*3, TILE_SIZE_X,TILE_SIZE_Y, T_WATER);

	/* Borde de pssages: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*9, TILE_SIZE_Y*3, TILE_SIZE_X,TILE_SIZE_Y, 0); /* 143 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*29, TILE_SIZE_Y, TILE_SIZE_X,TILE_SIZE_Y, 0); 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*30, TILE_SIZE_Y, TILE_SIZE_X,TILE_SIZE_Y, 0); 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*31, TILE_SIZE_Y, TILE_SIZE_X,TILE_SIZE_Y, 0); 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*32, TILE_SIZE_Y, TILE_SIZE_X,TILE_SIZE_Y, 0); 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*34, TILE_SIZE_Y*2, TILE_SIZE_X,TILE_SIZE_Y, 0); 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*35, TILE_SIZE_Y*2, TILE_SIZE_X,TILE_SIZE_Y, 0); 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*34, TILE_SIZE_Y*3, TILE_SIZE_X,TILE_SIZE_Y, 0); 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*35, TILE_SIZE_Y*3, TILE_SIZE_X,TILE_SIZE_Y, 0); 

	/* Mano de passages: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*33, TILE_SIZE_Y*2, TILE_SIZE_X,TILE_SIZE_Y*2, 0);
	
	/* Personajes de los passages: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*36, TILE_SIZE_Y*17, TILE_SIZE_X*2,TILE_SIZE_Y*2, 0); /* 153 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*34, TILE_SIZE_Y*17, TILE_SIZE_X*2,TILE_SIZE_Y*2, 0); /* 154 */ 
	
	/* Columna de decorado tipo 3: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*6, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, 0); /* 155 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*7, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, 0); 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*8, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, 0); 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*9, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, 0); 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*10, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*11, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, 0);

	/* Más personajes de los passages: */ 
	/* ATHENA */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*32, TILE_SIZE_Y*17, TILE_SIZE_X*2,TILE_SIZE_Y*2, 0); /* 161 */ 

	/* Puente del mundo 2:*/ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, T_WALL); /* 162 */ 

	/* Fuego: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*8, TILE_SIZE_Y*11, TILE_SIZE_X,TILE_SIZE_Y, T_LAVA); /* 163 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*9, TILE_SIZE_Y*11, TILE_SIZE_X,TILE_SIZE_Y, T_LAVA); /* 164 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*9, TILE_SIZE_Y*11, TILE_SIZE_X,TILE_SIZE_Y, T_WALL); /* 165 */ 

	/* Muro atravesable: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,          0,TILE_SIZE_Y*4, TILE_SIZE_X, TILE_SIZE_Y, T_NADA);	/* 166 */ 

	/* Aún más personajes de los passages: */ 
	/* DEATH */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*16, TILE_SIZE_Y*20, TILE_SIZE_X*2,TILE_SIZE_Y*2, 0); /* 167 */ 

	/* Fuego (animación): */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*34, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, T_LAVA); /* 168 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*35, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, T_LAVA); /* 169 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*36, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, T_LAVA); /* 170 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*37, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, T_LAVA); /* 171 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, T_LAVA); /* 172 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, T_LAVA); /* 173 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*37, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, T_LAVA); /* 174 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*36, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, T_LAVA); /* 175 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*35, TILE_SIZE_Y*21, TILE_SIZE_X,TILE_SIZE_Y, T_LAVA); /* 176 */ 

	/* Todavía más personajes de los passages: */ 
	/* HEPHAESTUS */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, 0, TILE_SIZE_Y*23, TILE_SIZE_X*2,TILE_SIZE_Y*2, 0); /* 177 */ 
	/* ASCLEPIUS */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*4, TILE_SIZE_Y*10, TILE_SIZE_X*2,TILE_SIZE_Y*2, 0); /* 178 */ 

	/* Piedras que aparecen en el mundo 5: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*38, TILE_SIZE_Y*3, TILE_SIZE_X,TILE_SIZE_Y, T_WALL); /* 179 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*39, TILE_SIZE_Y*3, TILE_SIZE_X,TILE_SIZE_Y, T_WALL); /* 180 */ 

	/* Agua caminable: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*19, TILE_SIZE_Y*3, TILE_SIZE_X,TILE_SIZE_Y, T_WALL); /* 181 */ 

	/* Pamperse: */ 
	tiles[n_tiles++]=new CTile(tiles2_bmp, TILE_SIZE_X*4, TILE_SIZE_Y*14, TILE_SIZE_X*2,TILE_SIZE_Y*2, T_NADA); /* 182 */ 


	/* Objetos: */ 
	/* Escaleras: */ 
	ladder_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp, 0,TILE_SIZE_Y*7,TILE_SIZE_X*2, TILE_SIZE_Y, 0);

	/* Piedras: */ 
	stone_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp, 0,TILE_SIZE_Y*5,TILE_SIZE_X*2,TILE_SIZE_Y*2, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*10,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2, 0);
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*23,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_WALL);

	/* Puertas de mundo: */ 
	wdoor_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*2,TILE_SIZE_Y*4,TILE_SIZE_X*4,TILE_SIZE_Y*3, 0);

	door_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*8,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*2, 0);


	/* Puertas con llave: */ 
	keydoor_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*8,TILE_SIZE_Y*3,TILE_SIZE_X,TILE_SIZE_Y, T_DOOR_WALL);

	/* Transportadores: */ 
	lever_tile=n_tiles;
	/* Este tile tiene 2 flags, T_WALL y T_LADDER_WALL, sirve para indicar el caso especial de que	*/ 
	/* las flechas no deben chocar con él!															*/ 
	tiles[n_tiles++]=new CTile(tiles2_bmp, 0,TILE_SIZE_Y*16,TILE_SIZE_X*4,TILE_SIZE_Y*2, T_WALL|T_LADDER_WALL);

	/* Personaje: --------------------- */ 

	character_tile[0]=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,             0,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 2,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 6,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-3*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 8,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	/* Hit! */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	/* Muerte: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	/* Máscara */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,             0,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);	/* MASCARAS */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 2,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 6,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 8,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,             0,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);	/* MASCARAS */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,             0,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);	/* MASCARAS */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	character_tile_inv[0]=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,             0,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 2,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 6,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,5*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 8,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	/* Hit! */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*20,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*20,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	/* Muerte: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	/* Mascara - inv */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,             0,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);	/* MASCARAS */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 2,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 6,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 8,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,             0,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);	/* MASCARAS */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,             0,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);	/* MASCARAS */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	/* Espada: */ 
	sword_tile[0]=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_WEAPON);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_WEAPON);


	character_tile[1]=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-3*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	/* Hit! */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	/* Muerte: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	/* Máscara */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,             0,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);	/* MASCARAS */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 2,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 6,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 8,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles_bmp,             0,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);	/* MASCARAS */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,             0,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);	/* MASCARAS */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	character_tile_inv[1]=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,5*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	/* Hit! */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*20,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*20,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	/* Muerte: */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	/* Mascara - inv */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,             0,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);	/* MASCARAS */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 2,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 6,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 8,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,             0,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);	/* MASCARAS */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,             0,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);	/* MASCARAS */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	/* Espada: */ 
	sword_tile[1]=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*13,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_WEAPON);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*15,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_WEAPON);

	/* When HIT: */ 
	character_tile[2]=n_tiles;
	tiles[n_tiles++]=new CTile(tiles2_bmp,             0,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X* 2,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X* 6,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-3*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X* 8,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);

	character_tile_inv[2]=n_tiles;
	tiles[n_tiles++]=new CTile(tiles2_bmp,             0,TILE_SIZE_Y*20,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X* 2,TILE_SIZE_Y*20,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X* 4,TILE_SIZE_Y*20,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X* 6,TILE_SIZE_Y*20,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,5*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X* 8,TILE_SIZE_Y*20,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,2*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*20,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*20,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,-1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*20,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*20,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_PLAYER,1*TILE_UNIT,0);


	/* Marcadores: */ 
	stats_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*22,TILE_SIZE_X*30,TILE_SIZE_Y*3,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,0,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*29,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_WALL);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*31,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2, T_WALL);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*27,TILE_SIZE_Y*22,TILE_SIZE_X*1,TILE_SIZE_Y*3,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*22,TILE_SIZE_X*1,TILE_SIZE_Y*3,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*22,TILE_SIZE_X*1,TILE_SIZE_Y*3,0);

	/* Objetos: */ 
	coin_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	arrow_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	key_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	items_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);

	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*36, TILE_SIZE_Y*19, TILE_SIZE_X*2,TILE_SIZE_Y*2, 0); 
	tiles[n_tiles++]=new CTile(tiles_bmp, TILE_SIZE_X*34, TILE_SIZE_Y*19, TILE_SIZE_X*2,TILE_SIZE_Y*2, 0);

	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);

	/* Armas: */ 
	armas_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_WEAPON);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_WEAPON);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*9,TILE_SIZE_X,TILE_SIZE_Y,T_WEAPON);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*7,TILE_SIZE_Y*9,TILE_SIZE_X,TILE_SIZE_Y,T_WEAPON);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_WEAPON);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*11,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_WEAPON);

	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_WEAPON);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_WEAPON);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*3,TILE_SIZE_X,TILE_SIZE_Y,T_WEAPON);

	/* Minas: +9 */ 
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_WEAPON);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_WEAPON);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_WEAPON);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*3,TILE_SIZE_X*4,TILE_SIZE_Y*3,T_WEAPON);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*3,TILE_SIZE_X*4,TILE_SIZE_Y*3,T_WEAPON);

	/* Armas inofensivas: */ 
	/* Felchas: +14 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_NADA);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_NADA);
	/* Flechas de cerámica: +16 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_NADA);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_NADA);
	/* Fuego: +18 */ 
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*7,TILE_SIZE_Y*9,TILE_SIZE_X,TILE_SIZE_Y,T_NADA);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*3,TILE_SIZE_X,TILE_SIZE_Y,T_NADA);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*9,TILE_SIZE_X,TILE_SIZE_Y,T_NADA);

	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*21,TILE_SIZE_X,TILE_SIZE_Y,T_WEAPON);

	/* Explosion: */ 
	explosion_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,0,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);

	/* Enemigos: */ 
	/* Gusano: */ 
	worm_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,0,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Murcielago: */ 
	bat_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,0,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Esqueleto: */ 
	skeleton_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,0,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*2,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*4,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*6,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*8,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*10,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*12,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*14,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*16,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*18,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*20,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Humo: */ 
	smoke_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,0,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	
	/* Orugas: */ 
	slime_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Bolas que rebotan: */ 
	bouncingball_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*2,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);

	/* Monstruo del agua: */ 
	watermonster_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,0,TILE_SIZE_Y*10,TILE_SIZE_X*3,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*10,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*11,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*3,TILE_SIZE_Y*11,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*3,TILE_SIZE_Y*10,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*5,TILE_SIZE_Y*10,TILE_SIZE_X*3,TILE_SIZE_Y*2,0);

	/* Piedras que caen: */ 
	fstone_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*13,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*15,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*17,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*19,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
		
	/* Caballero: */ 
	knight_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*22,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*24,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*26,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*28,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*30,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*32,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*34,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*36,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*38,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Bola roja: */ 
	blob_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Bambú: */ 
	bambu_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Hada: */ 
	fairy_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles2_bmp,0,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,0,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);

	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*8+TILE_SIZE_X/2,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*9,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*9+TILE_SIZE_X/2,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*10+TILE_SIZE_X/2,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*11,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*11+TILE_SIZE_X/2,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*12+TILE_SIZE_X/2,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*13,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*13+TILE_SIZE_X/2,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*14+TILE_SIZE_X/2,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*15,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*15+TILE_SIZE_X/2,TILE_SIZE_Y*12,TILE_SIZE_X/2,TILE_SIZE_Y/2,0);

	/* Puercoespín: */ 
	porcupine_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Fuego que salta de la lava: */ 
	lava_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Arbusto saltarín: */ 
	jumpingbush_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,0,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Araña azul: */ 
	bluespider_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,0,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Bolas de fuego blanco: */ 
	whitefireball_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,0,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Bicho negro del mundo 2: */ 
	black_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Bruja: */ 
	witch_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Osos blancos del mundo 2: */ 
	whitebear_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Pies de piedra: */ 
	feet_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Bicho rojo saltarin: */ 
	redjumper_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Muro viviente: */ 
	livingwall_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*3,T_WALL);

	/* Megamurcielago: */ 
	megabat_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*6,TILE_SIZE_X*4,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*6,TILE_SIZE_X*4,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*5,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*5,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_ENEMY);

	/* Pirañas: */ 
	piranha_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Estrellas blancas: */ 
	whitestar_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Arañas: */ 
	spider_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*37,TILE_SIZE_Y*10,TILE_SIZE_X,TILE_SIZE_Y*3,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*37,TILE_SIZE_Y*13,TILE_SIZE_X,TILE_SIZE_Y*3,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*10,TILE_SIZE_X,TILE_SIZE_Y*3,T_ENEMY);

	/* Cabeza de caballero: */ 
	knighthead_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*8,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);

	/* Pollos: */ 
	chicken_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*25,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*27,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*29,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*31,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*33,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*35,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*25,TILE_SIZE_Y*12,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*29,TILE_SIZE_Y*12,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);

	/* Hombres-roca: */ 
	rockman_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*33,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_NADA);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_NADA);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_NADA);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Nubes: */ 
	cloud_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*33,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*35,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Mariposas: */ 
	bfly_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,0,TILE_SIZE_Y*17,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,0,TILE_SIZE_Y*16,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X,TILE_SIZE_Y*16,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,2*TILE_SIZE_X,TILE_SIZE_Y*16,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,4*TILE_SIZE_X,TILE_SIZE_Y*16,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,6*TILE_SIZE_X,TILE_SIZE_Y*16,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,8*TILE_SIZE_X,TILE_SIZE_Y*16,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,10*TILE_SIZE_X,TILE_SIZE_Y*16,TILE_SIZE_X*4,TILE_SIZE_Y*3,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,14*TILE_SIZE_X,TILE_SIZE_Y*16,TILE_SIZE_X*4,TILE_SIZE_Y*3,T_ENEMY);

	/* Fantasmas: */ 
	ghost_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*1,TILE_SIZE_Y*17,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);

	/* Cabezas: */ 
	head_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_WALL);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_WALL);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*16,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_WALL);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*16,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_WALL);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*16,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Flechas de las armaduras: */ 
	armourarrow_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*9,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);

	/* Gusanos del mundo 5: */ 
	worm2_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,0,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Pulpillos del mundo 5: */ 
	octopus_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*20,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*22,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*21,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*23,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*21,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*23,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*21,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*23,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*21,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*23,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
		
	/* Bicho peludo del mundo 5: */ 
	hairy_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*18,TILE_SIZE_X*3,TILE_SIZE_Y*3,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*21,TILE_SIZE_Y*18,TILE_SIZE_X*3,TILE_SIZE_Y*3,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*3,TILE_SIZE_X*1,TILE_SIZE_Y*1,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*21,TILE_SIZE_Y*2,TILE_SIZE_X*1,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*33,TILE_SIZE_Y*8,TILE_SIZE_X*1,TILE_SIZE_Y*2,T_ENEMY);

	/* Waterdragon: */ 
	waterdragon_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,0,TILE_SIZE_Y*20,TILE_SIZE_X*4,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,0,TILE_SIZE_Y*22,TILE_SIZE_X*4,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*20,TILE_SIZE_X*4,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*22,TILE_SIZE_X*4,TILE_SIZE_Y*2,T_ENEMY);

	/* Water bug del mundo 5: */ 
	waterbug_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,0,TILE_SIZE_Y*24,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*24,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*24,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*24,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);

	/* Pajaros del mundo 6: */ 
	bird_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*21,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*23,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*21,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*23,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Stonemans del mundo 6: */ 
	stoneman_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*13,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*21,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*23,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*21,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*23,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*21,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*23,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*15,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);

	/* Pacman del mundo 6: */ 
	pacman_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*16,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*16,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*16,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*16,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*35,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*16,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*16,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*18,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* red dragon del mundo 6: */ 
	reddragon_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*21,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*21,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*16,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*21,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*14,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);

	/* buho del mundo 7: */ 
	owl_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy2_bmp,0,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*2,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*4,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*6,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* mono verde del mundo 7: */ 
	greenmonkey_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy2_bmp,0,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* planta del mundo 7: */ 
	plant_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy2_bmp,0,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*4,TILE_SIZE_X*1,TILE_SIZE_Y*1,T_ENEMY);

	/* Cambiante del mundo 8: */ 
	transformer_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy2_bmp,0,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*10,TILE_SIZE_X*1,TILE_SIZE_Y*1,T_ENEMY);

	/* Flame del mundo 8: */ 
	flame_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy2_bmp,0,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*8,TILE_SIZE_X*1,TILE_SIZE_Y*1,T_ENEMY);

	/* Brujas del mundo 8: */ 
	witch2_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy2_bmp,0,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*6,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Ciclope del mundo 8: */ 
	cyclops_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy2_bmp,0,TILE_SIZE_Y*12,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*12,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*12,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Serpientes del mundo 9: */ 
	snake_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*8,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*10,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*12,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*14,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*16,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*18,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*20,0,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*20,TILE_SIZE_Y,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);

	/* Gorilas del mundo 9: */ 
	gorilla_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*12,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*14,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Lagartos del mundo 9: */ 
	glizard_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*22,0,TILE_SIZE_X*4,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*2,TILE_SIZE_X*4,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*26,0,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*26,TILE_SIZE_Y,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*2,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*3,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*4,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*5,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*6,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*7,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*8,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*9,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*10,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*26,TILE_SIZE_Y*11,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*27,0,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*27,TILE_SIZE_Y,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*27,TILE_SIZE_Y*2,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*27,TILE_SIZE_Y*3,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*27,TILE_SIZE_Y*4,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*27,TILE_SIZE_Y*5,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*27,TILE_SIZE_Y*6,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*27,TILE_SIZE_Y*7,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*27,TILE_SIZE_Y*8,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*27,TILE_SIZE_Y*9,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*27,TILE_SIZE_Y*10,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*27,TILE_SIZE_Y*11,TILE_SIZE_X*1,TILE_SIZE_Y,T_ENEMY);

	/* Nubes del mundo 9: */ 
	lcloud_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*2,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*6,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*8,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Demonio del mundo 9: */ 
	bdemon_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*6,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*6,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*22,TILE_SIZE_Y*10,TILE_SIZE_X*1,TILE_SIZE_Y*1,T_ENEMY);

	/* Bolas de la pantalla de PAMPERSE: */ 
	pamperseball_tile=n_tiles;
	tiles[n_tiles++]=new CTile(enemy2_bmp,TILE_SIZE_X*8,TILE_SIZE_Y*14,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	/* Puerta sin llave de los mundos: */ 
	wnokeydoor_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*22,TILE_SIZE_X*1,TILE_SIZE_Y*3,T_DOOR_WALL);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*33,TILE_SIZE_Y*22,TILE_SIZE_X*2,TILE_SIZE_Y*3,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*35,TILE_SIZE_Y*22,TILE_SIZE_X*3,TILE_SIZE_Y*3,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*22,TILE_SIZE_X*2,TILE_SIZE_Y*3,0);

	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*32,0,TILE_SIZE_X*1,TILE_SIZE_Y*3,T_DOOR_WALL);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*33,0,TILE_SIZE_X*2,TILE_SIZE_Y*3,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*35,0,TILE_SIZE_X*3,TILE_SIZE_Y*3,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*38,0,TILE_SIZE_X*2,TILE_SIZE_Y*3,0);

	/* Tiles de colisión: */ 
	colision_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*18,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*17,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);
	tiles[n_tiles++]=new CTile(tiles_bmp,TILE_SIZE_X*38,TILE_SIZE_Y*19,TILE_SIZE_X*2,TILE_SIZE_Y*2,0);

	/* Tiles especiales: */ 
	demonsign_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles2_bmp,0,0,TILE_SIZE_X*8,TILE_SIZE_Y*8,0);
	lupa_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*8,0,TILE_SIZE_X*24,TILE_SIZE_Y*12,0);
	demonsmoke_tile=n_tiles;
	tiles[n_tiles++]=new CTile(tiles2_bmp,0,TILE_SIZE_Y*8,TILE_SIZE_X*4,TILE_SIZE_Y*4,0);
	tiles[n_tiles++]=new CTile(tiles2_bmp,TILE_SIZE_X*4,TILE_SIZE_Y*8,TILE_SIZE_X*4,TILE_SIZE_Y*4,0);

	demon_tile=n_tiles;

	if (fighting_demon>=160) {
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
	} /* if */ 

	/* Reload in the ENDING SCENE: */ 
	if (STATE==18 &&
		SUBSTATE>=96) {
		int i;

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

} /* ReloadGraphics */ 


