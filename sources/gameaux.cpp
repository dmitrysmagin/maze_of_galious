#include "math.h"
#include "stdio.h"
#include "stdlib.h" 
#include "string.h"
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
extern char *default_g_path;
extern char *default_s_path;
extern char **g_paths;
extern int n_g_paths,act_g_path;
extern char *g_path;
extern char **s_paths;
extern int n_s_paths,act_s_path;
extern char *s_path;

/* Gr·ficos: */ 
extern Bitmap *konami_bmp,*menu_bmp,*tiles_bmp,*tiles2_bmp,*enemy_bmp,*enemy2_bmp,*final_bmp;
extern int n_tiles;
extern CTile **tiles;
extern SDL_Surface *screen;

extern BYTE *col_buffer;

/* Estado: */ 
extern int STATE,SUBSTATE;

/* Developers variables: */ 
extern int developer_start_x,developer_start_y,developer_start_map;


/* Juego: */ 
extern int frame_counter;
extern int map;			/* En que mundo est· el personaje			*/ 
extern int map_x,map_y;	/* En que habitaciÛn dentro del mundo		*/ 
extern int pers_x,pers_y;	/* PosiciÛn del personaje en la habitaciÛn	*/ 
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
extern int in_ladder;			/* En quÈ escalera est· el personaje	*/ 
extern bool previous_x_collision;	/* Hubo una colisiÛn en X en el frame anterior? */ 
extern bool previous_y_collision;	/* Hubo una colisiÛn en Y en el frame anterior? */ 
extern int hit_time;	/* Contador para saber cuando pueden daÒarnos de nuevo	*/ 
extern int character;
extern int freezed;
extern bool live_character[2],can_revive_character[2];
extern int current_weapon;
extern int n_fired_arrows,n_fired_mines;
extern int to_enter_cut;
extern bool mine_stepped;
extern bool ZEUS_password;

extern int slime_counter,stones_counter;
extern bool room_slimes,room_watermonster,room_demonroom,room_fallingstones;
extern bool room_rollingstones,room_blobs,room_lava;
extern int room_reddragon;
extern int room_appearwall,room_appearwall_x,room_appearwall_y,room_fairy;
extern int appearwall_record_x[MAX_AW_RECORD];
extern int appearwall_record_y[MAX_AW_RECORD];
extern bool room_piranhas;
extern int fairy_x,fairy_y;
extern int fighting_demon;
extern int watermonster_state;
extern int world4_state;
extern int world6_room35_state;
extern int world3_room06_counter;
extern int world5_room44_state,world5_room44_state2;
extern int world5_room64_state;
extern int world6_room35_state;
extern int world7_room12_state;
extern int world8_room12_state;
extern int room26_state,room56_state;
extern int world10_door_x,world10_door_y;
extern int room_bfly;
extern int room_waterdragons;
extern int room_birds;

extern int n_arrows,n_coins,n_keys;
extern int player_energy[2],player_experience[2],player_max[2];
extern int player_denergy[2],player_dexperience[2];
extern bool item[38];
extern bool world_item[10][4];
extern bool cross,world_key;
extern int next_world;
extern bool world_doors_open[10];

extern int room_size_x,room_size_y;
extern int *room_tiles;
extern int bible_counter,bible_subcounter;

/* Objetos: (escaleras, piedras, etc...) */ 
extern int n_objects;
extern MOGObject object[MAX_OBJECTS];

extern int wall_tile;
extern int stone_tile,ladder_tile,wdoor_tile,keydoor_tile,stats_tile;
extern int coin_tile,arrow_tile,key_tile,door_tile;
extern int character_tile[2],character_tile_inv[2];
extern int colision_tile;
extern int sword_tile[2],explosion_tile;
extern int worm_tile,bat_tile,skeleton_tile,smoke_tile,slime_tile,bouncingball_tile;
extern int watermonster_tile,jumpingbush_tile,bluespider_tile,whitefireball_tile;
extern int fstone_tile,knight_tile,blob_tile,bambu_tile,fairy_tile,porcupine_tile;
extern int black_tile,witch_tile,whitebear_tile,feet_tile,redjumper_tile,livingwall_tile;
extern int megabat_tile,piranha_tile,whitestar_tile,spider_tile;
extern int items_tile,armas_tile,tomb_tile,wkeydoor_tile,wnokeydoor_tile,lever_tile;
extern int demonsign_tile,demon_tile,knighthead_tile,chicken_tile,rockman_tile,cloud_tile;
extern int ghost_tile,head_tile,worm2_tile,octopus_tile,hairy_tile,waterbug_tile,stoneman_tile;
extern int pacman_tile,owl_tile,greenmonkey_tile,plant_tile,transformer_tile,flame_tile;
extern int witch2_tile,cyclops_tile,lcloud_tile,glizard_tile,snake_tile,bdemon_tile,gorilla_tile;

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


/* Estado de las habitaciones por las que ya has pasado: */ 
#define HP_OBJS_MAX	512

extern int HP_room_x[HP_OBJS_MAX],HP_room_y[HP_OBJS_MAX];
extern int HP_obj[HP_OBJS_MAX];
extern int HP_obj_type[HP_OBJS_MAX];
extern int HP_obj_aux[HP_OBJS_MAX];

/* Teclado: */ 
extern unsigned char old_keyboard[SDLK_LAST];

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
extern SOUNDT S_dropmine,S_firefire,S_fireball,S_fireball2;
extern SOUNDT S_wdoor,S_door2,S_demonhit,S_bearbullet,S_chickenbomb;
extern SOUNDT S_colormonster,S_waterstone,S_demon4jump,S_rockman,S_mine;
extern SOUNDT S_bible,S_demon1bones,S_demon1jump,S_owl,S_demon7bullet;
extern SOUNDT S_demon8bullet,S_flamebullet,S_snakebullet,S_gorilla,S_lizardtongue;
extern SOUNDT S_bdemonbullet,S_lightning;
extern int music_volume,sfx_volume;
extern bool sound_enabled;

/* Control de pantalla: */ 
extern int logic_dx[4],logic_dy[4],logic_x[4],logic_y[4];
extern int physic_dx[4],physic_dy[4],physic_x[4],physic_y[4];

extern int zoom;


/* Teclas: */ 
extern SDLKey UP_KEY,DOWN_KEY,LEFT_KEY,RIGHT_KEY;
extern SDLKey SWORD_KEY,WEAPON_KEY,ITEM_KEY,PAUSE_KEY;
extern SDLKey last_word[16];

extern char password[48];



void ReleaseGraphics(void)
{
	int i;

	delete col_buffer;
	col_buffer=0;

	for(i=0;i<n_tiles;i++) {
		delete tiles[i];
		tiles[i]=0;
	} /* for */ 
	delete tiles;

	delete enemy_bmp;
	enemy_bmp=0;
	delete enemy2_bmp;
	enemy2_bmp=0;
	delete tiles_bmp;
	tiles_bmp=0;
	delete tiles2_bmp;
	tiles2_bmp=0;
	delete menu_bmp;
	menu_bmp=0;
	delete konami_bmp;
	konami_bmp=0;
	delete final_bmp;
	final_bmp=0;

} /* ReleaseGraphics */ 


void SetSFXVolume(int volume) 
{
	if (sound_enabled) {
		Mix_VolumeChunk(S_gamestart,volume);
		Mix_VolumeChunk(S_jump,volume);
		Mix_VolumeChunk(S_sword,volume);
		Mix_VolumeChunk(S_swordhit,volume);
		Mix_VolumeChunk(S_item,volume);
		Mix_VolumeChunk(S_ah,volume);
		Mix_VolumeChunk(S_ah2,volume);
		Mix_VolumeChunk(S_powerup,volume);
		Mix_VolumeChunk(S_enemyhit,volume);
		Mix_VolumeChunk(S_door,volume);
		Mix_VolumeChunk(S_enemykill,volume);
		Mix_VolumeChunk(S_ladderdisapear,volume);
		Mix_VolumeChunk(S_armourhit,volume);
		Mix_VolumeChunk(S_pause,volume);
		Mix_VolumeChunk(S_death,volume);
		Mix_VolumeChunk(S_gameover,volume);
		Mix_VolumeChunk(S_worldkey,volume);
		Mix_VolumeChunk(S_entering,volume);
		Mix_VolumeChunk(S_enterwater,volume);
		Mix_VolumeChunk(S_select,volume);
		Mix_VolumeChunk(S_firearrow,volume);
		Mix_VolumeChunk(S_firearrow2,volume);
		Mix_VolumeChunk(S_fall,volume);
		Mix_VolumeChunk(S_waterhit,volume);
		Mix_VolumeChunk(S_lavahit,volume);
		Mix_VolumeChunk(S_nocoins,volume);
		Mix_VolumeChunk(S_stones,volume);
		Mix_VolumeChunk(S_blob,volume);
		Mix_VolumeChunk(S_shield,volume);
		Mix_VolumeChunk(S_flapwings,volume);
		Mix_VolumeChunk(S_rotatedoor,volume);
		Mix_VolumeChunk(S_demon2ball,volume);
		Mix_VolumeChunk(S_appearing,volume);
		Mix_VolumeChunk(S_firebreath,volume);
		Mix_VolumeChunk(S_F1,volume);
		Mix_VolumeChunk(S_karrow,volume);
		Mix_VolumeChunk(S_jumptree,volume);
		Mix_VolumeChunk(S_skeletonhit,volume);
		Mix_VolumeChunk(S_enemybullet,volume);
		Mix_VolumeChunk(S_headbullet,volume);
		Mix_VolumeChunk(S_dropmine,volume);
		Mix_VolumeChunk(S_firefire,volume);
		Mix_VolumeChunk(S_fireball,volume);
		Mix_VolumeChunk(S_fireball2,volume);
		Mix_VolumeChunk(S_wdoor,volume);
		Mix_VolumeChunk(S_door2,volume);
		Mix_VolumeChunk(S_demonhit,volume);
		Mix_VolumeChunk(S_bearbullet,volume);
		Mix_VolumeChunk(S_chickenbomb,volume);
		Mix_VolumeChunk(S_colormonster,volume);
		Mix_VolumeChunk(S_waterstone,volume);
		Mix_VolumeChunk(S_demon4jump,volume);
		Mix_VolumeChunk(S_rockman,volume);
		Mix_VolumeChunk(S_mine,volume);
		Mix_VolumeChunk(S_bible,volume);
		Mix_VolumeChunk(S_demon1bones,volume);
		Mix_VolumeChunk(S_demon1jump,volume);
		Mix_VolumeChunk(S_owl,volume);
		Mix_VolumeChunk(S_demon7bullet,volume);
		Mix_VolumeChunk(S_demon8bullet,volume);
		Mix_VolumeChunk(S_flamebullet,volume);
		Mix_VolumeChunk(S_snakebullet,volume);
		Mix_VolumeChunk(S_gorilla,volume);
		Mix_VolumeChunk(S_lizardtongue,volume);
		Mix_VolumeChunk(S_bdemonbullet,volume);
		Mix_VolumeChunk(S_lightning,volume);
	} /* if */ 
} /* SetSFXVolume */ 


void ReleaseSound(bool definitive)
{
	if (definitive) Sound_release_music();
			   else Sound_temporary_release_music();
	Delete_sound(S_gamestart);
	Delete_sound(S_jump);
	Delete_sound(S_sword);
	Delete_sound(S_swordhit);
	Delete_sound(S_item);
	Delete_sound(S_ah);
	Delete_sound(S_ah2);
	Delete_sound(S_powerup);
	Delete_sound(S_enemyhit);
	Delete_sound(S_door);
	Delete_sound(S_enemykill);
	Delete_sound(S_ladderdisapear);
	Delete_sound(S_armourhit);
	Delete_sound(S_pause);
	Delete_sound(S_death);
	Delete_sound(S_gameover);
	Delete_sound(S_worldkey);
	Delete_sound(S_entering);
	Delete_sound(S_enterwater);
	Delete_sound(S_select);
	Delete_sound(S_firearrow);
	Delete_sound(S_firearrow2);
	Delete_sound(S_fall);
	Delete_sound(S_waterhit);
	Delete_sound(S_lavahit);
	Delete_sound(S_nocoins);
	Delete_sound(S_stones);
	Delete_sound(S_blob);
	Delete_sound(S_shield);
	Delete_sound(S_bell);
	Delete_sound(S_flapwings);
	Delete_sound(S_rotatedoor);
	Delete_sound(S_demon2ball);
	Delete_sound(S_appearing);
	Delete_sound(S_firebreath);
	Delete_sound(S_F1);
	Delete_sound(S_karrow);
	Delete_sound(S_jumptree);
	Delete_sound(S_skeletonhit);
	Delete_sound(S_enemybullet);
	Delete_sound(S_headbullet);
	Delete_sound(S_dropmine);
	Delete_sound(S_firefire);
	Delete_sound(S_fireball);
	Delete_sound(S_fireball2);
	Delete_sound(S_wdoor);
	Delete_sound(S_door2);
	Delete_sound(S_demonhit);
	Delete_sound(S_bearbullet);
	Delete_sound(S_chickenbomb);
	Delete_sound(S_colormonster);
	Delete_sound(S_waterstone);
	Delete_sound(S_demon4jump);
	Delete_sound(S_rockman);
	Delete_sound(S_mine);
	Delete_sound(S_bible);
	Delete_sound(S_demon1bones);
	Delete_sound(S_demon1jump);
	Delete_sound(S_owl);
	Delete_sound(S_demon7bullet);
	Delete_sound(S_demon8bullet);
	Delete_sound(S_flamebullet);
	Delete_sound(S_snakebullet);
	Delete_sound(S_gorilla);
	Delete_sound(S_lizardtongue);
	Delete_sound(S_bdemonbullet);
	Delete_sound(S_lightning);
} /* ReleaseSound */ 




void GameEnd(void)
{
	ReleaseGraphics();
	ReleaseSound(true);

	if (room_tiles!=NULL) delete room_tiles;
} /* FinishGame */ 


void newgame(void)
{
	/* Inicializa juego: */ 
	int i,j;

	hit_time=0;
	map=0;
	map_x=5;
	map_y=12;

	n_arrows=n_coins=n_keys=0;
	n_arrows=0;
	n_coins=0;
	n_keys=0;

	n_fired_arrows=0;
	n_fired_mines=0;
	player_energy[0]=player_energy[1]=16;
	player_max[0]=player_max[1]=16;
	player_experience[0]=player_experience[1]=0;
	player_denergy[0]=player_denergy[1]=16;
	player_dexperience[0]=player_dexperience[1]=0;
	live_character[0]=live_character[1]=true;
	character=0;
	freezed=0;

	pers_x=20*TILE_SIZE_X;
	pers_y=9*TILE_SIZE_Y;
	pers_right=true;
	pers_pos=0;
	pers_state=S_QUIET;
	pers_substate=0;
	character=0;
	current_weapon=-1;
	fighting_demon=0;
	to_enter_cut=0;
	mine_stepped=false;
	bible_counter=16;
	bible_subcounter=0;
	can_revive_character[0]=can_revive_character[1]=true;
	ZEUS_password=false;

	world3_room06_counter=0;
	world5_room44_state=0;
	world5_room44_state2=0;
	world5_room64_state=0;
	world6_room35_state=0;
	world7_room12_state=0;
	world8_room12_state=0;
	world10_door_x=world10_door_y=-1;

	clear_typed_word();
	for(i=0;i<38;i++) item[i]=false;
	for(i=0;i<10;i++) {
		world_doors_open[i]=false;
		for(j=0;j<4;j++) {
			world_item[i][j]=false;
		} /* for */ 
	} /* for */ 
	world_key=false;
	cross=false;
	next_world=0;

	for(i=0;i<MAX_LEVERS;i++) {
		lever_room_x[i]=lever_drx[i][0];
		lever_room_y[i]=lever_dry[i][0];
		lever_x[i]=lever_dx[i][0];
		lever_y[i]=lever_dy[i][0];
		lever_going_to[i]=1;
	} /* for */ 

	for(i=0;i<MAX_AW_RECORD;i++) {
		appearwall_record_x[i]=-1;
		appearwall_record_y[i]=-1;
	} /* for */ 

	readpassword(password);

	/* Quitar: */ 
	if (developer_start_map!=-1) {
		map=developer_start_map;
		map_x=developer_start_x;
		map_y=developer_start_y;

		for(i=0;i<map;i++) world_doors_open[i]=true;
		next_world=map;
	} /* if */ 

/*	map=0;
	map_x=14;
	map_y=13;
	current_weapon=0;
	pers_y=TILE_SIZE_Y*17;
	pers_x=TILE_SIZE_X*24;
	player_energy[0]=player_energy[1]=80;
    player_max[0]=player_max[1]=80; 
//	player_energy[1]=8;
//	live_character[1]=false;
	world_key=true;
//	cross=true;
	world_doors_open[0]=true;
	world_doors_open[1]=true;
	world_doors_open[2]=true;
	world_doors_open[3]=true;
	world_doors_open[4]=true;
	world_doors_open[5]=true;
	world_doors_open[6]=true;
	world_doors_open[7]=true;
	world_doors_open[8]=true;
	next_world=11;
	world4_state=3;
	world5_room44_state2=1;
	world6_room35_state=7;
	for(i=0;i<38;i++) item[i]=true;
	item[30]=false;
	item[31]=false;
	n_arrows=999;
	n_coins=999;
	n_keys=999;
 
	for(i=0;i<10;i++) {
		world_item[i][0]=true;
		world_item[i][1]=true;
		world_item[i][2]=true;
		world_item[i][3]=true;
	} /* for */ 
		
} /* newgame */ 


// type=0 normal
// type=1 only background
// type=2 only objects

void drawmap(BYTE *screen,int dx,int dy,int type)
{
	int i,j;
	int val;

	if (type!=2) {
		for(i=0;i<room_size_y;i++) {
			for(j=0;j<room_size_x;j++) {
				tiles[room_tiles[j+i*room_size_x]]->
						draw(GAME_VIEW_X+j*TILE_SIZE_X,GAME_VIEW_Y+i*TILE_SIZE_Y,
							TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
			} /* for */ 
		} /* for */ 

		if (map==5 && map_x==1 && map_y==3 && !item[9]) {
			int offset,tile;

			for(i=0;i<room_size_y;i++) {
				for(j=0;j<room_size_x;j++) {
					offset=j+i*room_size_x;
					tile=room_tiles[offset];
					if (tile==125 || tile==126 ||
						tile==131 || tile==132 ||
						tile==133 || tile==134 ||
						(tile>=135 || tile<=140)) {

						 if ((i%4)==0) {
							tile=127+(j%2);
						 } /* if */ 
						 if ((i%4)==1) {
							tile=129+(j%2);
						 } /* if */ 
						 if ((i%4)==2) {
							tile=128-(j%2);
						 } /* if */ 
						 if ((i%4)==3) {
							tile=130-(j%2);
						 } /* if */ 
						tiles[tile]->
								draw(GAME_VIEW_X+j*TILE_SIZE_X,GAME_VIEW_Y+i*TILE_SIZE_Y,
									TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
					} /* if */ 
				} /* for */ 
			} /* for */ 
		} /* if */ 

		if (room_demonroom) {
			tiles[demonsign_tile]->
				draw(GAME_VIEW_X+TILE_SIZE_X*12,GAME_VIEW_Y+TILE_SIZE_Y*4,
					 TILE_SIZE_X*8,TILE_SIZE_Y*8,screen,col_buffer,dx,dy,dx);

			draw_demon_background(screen,dx,dy,dx);

			for(i=0;i<room_size_y;i++) {
				for(j=0;j<room_size_x;j++) {
					if (room_tiles[j+i*room_size_x]==125 ||
						room_tiles[j+i*room_size_x]==126)
						tiles[room_tiles[j+i*room_size_x]]->
								draw(GAME_VIEW_X+j*TILE_SIZE_X,GAME_VIEW_Y+i*TILE_SIZE_Y,
									TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
				} /* for */ 
			} /* for */ 
		} /* if */ 

	} /* if */ 

	if (type!=1) {

		if (map==0) {
			for(i=0;i<MAX_LEVERS;i++) {
				if (map_x==lever_room_x[i] && map_y==lever_room_y[i]) {
					tiles[lever_tile]->
						draw(GAME_VIEW_X+lever_x[i],GAME_VIEW_Y+lever_y[i],
							 TILE_SIZE_X*4,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				} /* if */ 
			} /* for */ 
		} /* if */ 

		for(i=0;i<n_objects;i++) {
			object[i].draw_x=GAME_VIEW_X+object[i].x+object[i].xoffs;
			object[i].draw_y=GAME_VIEW_Y+object[i].y+object[i].yoffs;
			switch(object[i].type) {
			case T_LADDER:
				for(j=0;j<object[i].size && (object[i].y+j*TILE_SIZE_Y)<(room_size_y*TILE_SIZE_Y);j++) {
					tiles[ladder_tile]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+(object[i].y+j*TILE_SIZE_Y),
							 TILE_SIZE_X*2,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
				} /* for */ 
				break;
			case T_STONE:
				tiles[stone_tile]->
					draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				tiles[stone_tile+2]->
					draw_only_mask(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						 TILE_SIZE_X*2,TILE_SIZE_Y*2,col_buffer,dx,dy,dx);			
				break;
			case T_BROKEN_STONE:
				tiles[stone_tile+1]->
					draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				break;
			case T_WDOOR:
				tiles[wdoor_tile]->
					draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						 TILE_SIZE_X*4,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
				if (object[i].state==1) {
					tiles[ladder_tile]->
						draw(GAME_VIEW_X+object[i].x+TILE_SIZE_X,GAME_VIEW_Y+(object[i].y+TILE_SIZE_Y),
							 TILE_SIZE_X*2,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
					tiles[ladder_tile]->
						draw(GAME_VIEW_X+object[i].x+TILE_SIZE_X,GAME_VIEW_Y+(object[i].y+2*TILE_SIZE_Y),
							 TILE_SIZE_X*2,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
				} /* if */ 
				break;
			case T_DOOR:
				tiles[door_tile]->
					draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				break;
			case T_KEYDOOR:
				if (object[i].state>=0) {
					for(j=0;j<3;j++) {
						tiles[keydoor_tile]->
							draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+(object[i].y+j*TILE_SIZE_Y),
								 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
					} /* for */ 
				} else {
					if (object[i].state<-20) {
						tiles[keydoor_tile]->
							draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+(object[i].y+2*TILE_SIZE_Y),
								 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
					} /* if */ 
					if (object[i].state<-12) {
						tiles[keydoor_tile]->
							draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+(object[i].y+TILE_SIZE_Y),
								 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
					} /* if */ 
					tiles[keydoor_tile]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
				} /* if */ 
				break;
			case T_WKEYDOOR:
				if (object[i].state>=0) {
					tiles[wkeydoor_tile]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
				} else {
					if (object[i].state2==0) {
						if (object[i].state<-20) {
							tiles[wkeydoor_tile+1]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
						} else {
							tiles[wkeydoor_tile+2]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*3,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
						} /* if */ 
					} else {
						if (object[i].state<-20) {
							tiles[wkeydoor_tile+1]->
								draw(GAME_VIEW_X+object[i].x-TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
						} else {
							tiles[wkeydoor_tile+2]->
								draw(GAME_VIEW_X+object[i].x-TILE_SIZE_X*2,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*3,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
						} /* if */ 
					} /* if */ 
				} /* if */ 
				break;
			case T_WNOKEYDOOR:
			case T_WNOKEYDOOR2:
			case T_WNOKEYDOOR3:
				{
					int val=0;

					if (object[i].type==T_WNOKEYDOOR2) val=4;
					if (object[i].type==T_WNOKEYDOOR3) val=4;
					if (object[i].state>=0) {
						tiles[wnokeydoor_tile+val]->
							draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
								 TILE_SIZE_X,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
					} else {
						if (object[i].state2==0) {
							if (object[i].state<-16) {
								tiles[wnokeydoor_tile+1+val]->
									draw(GAME_VIEW_X+object[i].x-(TILE_SIZE_X/2),GAME_VIEW_Y+object[i].y,
										 TILE_SIZE_X*2,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
							} else {
								if (object[i].state<-8) {
									tiles[wnokeydoor_tile+2+val]->
										draw(GAME_VIEW_X+object[i].x-TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
											 TILE_SIZE_X*3,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
								} else {
									tiles[wnokeydoor_tile+3+val]->
										draw(GAME_VIEW_X+object[i].x-(TILE_SIZE_X/2),GAME_VIEW_Y+object[i].y,
											 TILE_SIZE_X*2,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
								} /* if */ 
							} /* if */ 
						} else {
							if (object[i].state<-16) {
								tiles[wnokeydoor_tile+3+val]->
									draw(GAME_VIEW_X+object[i].x-(TILE_SIZE_X/2),GAME_VIEW_Y+object[i].y,
										 TILE_SIZE_X*2,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
							} else {
								if (object[i].state<-8) {
									tiles[wnokeydoor_tile+2+val]->
										draw(GAME_VIEW_X+object[i].x-TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
											 TILE_SIZE_X*3,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
								} else {
									tiles[wnokeydoor_tile+1+val]->
										draw(GAME_VIEW_X+object[i].x-(TILE_SIZE_X/2),GAME_VIEW_Y+object[i].y,
											 TILE_SIZE_X*2,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);
								} /* if */ 
							} /* if */ 
						} /* if */ 
					} /* if */ 
				}
				break;
			case T_ITEM:
				if (object[i].state==-1) 
					tiles[coin_tile]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				if (object[i].state==-2) 
					tiles[key_tile]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				if (object[i].state==-3) 
					tiles[arrow_tile]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				if (object[i].state>=0 && object[i].state<32)  {
					tiles[items_tile+object[i].state]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				} /* if */ 
				/* Armas: */ 
				if (object[i].state==32 || object[i].state==33)  {
					tiles[armas_tile+object[i].state-32]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							TILE_SIZE_X*2,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
				} /* if */ 
				if (object[i].state==34 || object[i].state==35)  {
					tiles[armas_tile+object[i].state-32]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
				} /* if */ 
				if (object[i].state==37) {
					tiles[items_tile+37]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				} /* if */ 
				if (object[i].state==36) {
					tiles[items_tile+36]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				} /* if */ 
				if (object[i].state>=38) {
					tiles[items_tile+object[i].state-6]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				} /* if */ 
				break;
			case T_EXPLOSION:
				tiles[explosion_tile+((object[i].size%9)/3)]->
					draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				break;
			case T_WEAPON_ARROW:
				object[i].state3=object[i].x;
				if (object[i].state==0) {
					tiles[armas_tile]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
				} else {
					tiles[armas_tile+6]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
				} /* if */ 
				break;
			case T_WEAPON_ARROW2:
				object[i].state3=object[i].x;
				if (object[i].state==0) {
					tiles[armas_tile+1]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
				} else {
					tiles[armas_tile+7]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
				} /* if */ 
				break;
			case T_RFL_ARROW:
				if (object[i].state==0) {
					tiles[armas_tile+14]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
				} else {
					tiles[armas_tile+15]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
				} /* if */ 
				break;
			case T_RFL_ARROW2:
				if (object[i].state==0) {
					tiles[armas_tile+16]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
				} else {
					tiles[armas_tile+17]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
				} /* if */ 
				break;
			case T_RFL_FIRE:
				if (object[i].state==0) {
					tiles[armas_tile+18]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
				} else {
					tiles[armas_tile+19]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
				} /* if */ 
				break;
			case T_RFL_RFIRE:
				tiles[armas_tile+20]->
					draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						 screen,col_buffer,dx,dy,dx);
				break;
			case T_WEAPON_FIRE:
				object[i].state3=object[i].x;
				if (object[i].state==0) {
					tiles[armas_tile+3]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
				} else {
					tiles[armas_tile+8]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
				} /* if */ 
				break;	
			case T_WEAPON_RFIRE:
				tiles[armas_tile+object[i].tile]->
					draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
				break;	
			case T_WEAPON_MINE:
				object[i].state3=object[i].x;
				tiles[armas_tile+object[i].tile]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
				
				break;
			case T_MINE_EXP:
				tiles[armas_tile+object[i].tile]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
				tiles[armas_tile+object[i].tile]->
						draw_only_mask(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y-(TILE_SIZE_Y*3)/2,
										col_buffer,dx,dy,dx);
				break;
			case T_ARMOUR:
				tiles[object[i].base_tile]->
					draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						 TILE_SIZE_X*2,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);			
				break;
			case T_TOMB:
				tiles[tomb_tile]->
					draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);			
				break;
			case T_FAIRY_BUBBLE:
				tiles[fairy_tile+6+object[i].state3]->
					draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						 TILE_SIZE_X/2,TILE_SIZE_Y/2,screen,col_buffer,dx,dy,dx);			
				break;
			case T_FAIRY:
				if (object[i].state3<80) {
					tiles[fairy_tile+((object[i].state3>>3)&0x01)+4]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);			
				} else {
					if (object[i].state>=0) {
						tiles[fairy_tile+((object[i].state3>>2)&0x01)+2]->
							draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);			
					} else {
						tiles[fairy_tile+((object[i].state3>>2)&0x01)]->
							draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);			
					} /* if */ 
				} /* if */ 
				break;
			case T_HITWALL:
				{
					int j,k;
					int val;

					if (map==0) val=105;
						   else val=125;

					for(j=0;j<3;j++) {
						for(k=0;k<4;k++) {
							tiles[val]->
								draw(GAME_VIEW_X+object[i].x+k*TILE_SIZE_X,
									 GAME_VIEW_Y+object[i].y+j*TILE_SIZE_Y,
									 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
						} /* for */ 
					} /* for */ 
				}
				break;
			case T_PAMPERSE:
				tiles[182]->
					draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
						 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				break;
			case T_ENEMY:
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
					tiles[object[i].base_tile+val]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y+worm_state_y[j]*TILE_UNIT,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					break;
				case T_BAT:
					{
						int x,y;
						x=object[i].x/TILE_SIZE_X;
						y=object[i].y/TILE_SIZE_Y;


						j=2;
						if (iswall(x,y) || !iswall(x,y-1) ||
							object[i].state==1 || y==0) j=((object[i].state2>>2)%2);

						tiles[object[i].base_tile+j]->
							draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
								 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					}
					break;
				case T_SKELETON:
					j=(object[i].state2>>2)%2;
					switch(object[i].size) {
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:tiles[object[i].base_tile+6]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
						break;
					case 6:tiles[object[i].base_tile+5]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
						break;
					case 7:tiles[object[i].base_tile+4]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
						break;
					default: /* NORMAL: */ 
						switch(object[i].state) {
						case 0: /* CAMINA IZQ: */ 
							tiles[object[i].base_tile+j]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
							tiles[object[i].base_tile+7+j]->
								draw(GAME_VIEW_X+object[i].x-8*TILE_UNIT,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
							break;
						case 1: /* CAMINA DER: */ 
							tiles[object[i].base_tile+2+j]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
							tiles[object[i].base_tile+9+j]->
								draw(GAME_VIEW_X+object[i].x+8*TILE_UNIT,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
							break;
						case 2: /* QUIETO IZQ: */ 
							tiles[object[i].base_tile]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
							tiles[object[i].base_tile+7]->
								draw(GAME_VIEW_X+object[i].x-8*TILE_UNIT,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
							break;
						case 3: /* QUIETO DER: */ 
							tiles[object[i].base_tile+2]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
							tiles[object[i].base_tile+9]->
								draw(GAME_VIEW_X+object[i].x+8*TILE_UNIT,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
							break;
						case 5:
						case 7: /* SALTANDO IZQ: */ 
							tiles[object[i].base_tile+1]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
							tiles[object[i].base_tile+7]->
								draw(GAME_VIEW_X+object[i].x-8*TILE_UNIT,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
							break;
						case 4:
						case 6: /* SALTANDO DER: */ 
							tiles[object[i].base_tile+3]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
							tiles[object[i].base_tile+9]->
								draw(GAME_VIEW_X+object[i].x+8*TILE_UNIT,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
							break;
						} /* switch */ 
						break;
					} /* switch */ 
					break;
				case T_SMOKE:
					{
						int val;

						if (object[i].size==0) val=object[i].base_tile;
										  else val=object[i].base_tile+4;

						if (item[21]) {
  							switch(object[i].state2) {
							case 0:
								break;
							case 1:
							case 7:
								if ((object[i].state%6)>=3)
									tiles[val]->
										draw_without_mask(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
											 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,dx,dy,dx);
								break;
							case 2:
							case 6:
								tiles[val+((object[i].state%6)<3 ? 0 : 1)]->
									draw_without_mask(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
										 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,dx,dy,dx);
								break;
							case 3:
							case 5:
								tiles[val+((object[i].state%6)<3 ? 1 : 2)]->
									draw_without_mask(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
										 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,dx,dy,dx);
								break;
							case 4:
								tiles[val+((object[i].state%6)<3 ? 2 : 3)]->
									draw_without_mask(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
										 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,dx,dy,dx);
								break;
							} /* switch */ 
						} else {
  							switch(object[i].state2) {
							case 0:
								break;
							case 1:
							case 7:
								if ((object[i].state%6)>=3)
									tiles[val]->
										draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
											 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
								break;
							case 2:
							case 6:
								tiles[val+((object[i].state%6)<3 ? 0 : 1)]->
									draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
										 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
								break;
							case 3:
							case 5:
								tiles[val+((object[i].state%6)<3 ? 1 : 2)]->
									draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
										 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
								break;
							case 4:
								tiles[val+((object[i].state%6)<3 ? 2 : 3)]->
									draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
										 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
								break;
							} /* switch */ 
						} /* if */ 
					}
					break;

				case T_BAMBU:
					if (object[i].tile>=0) {
						if (item[10]) {
							tiles[object[i].base_tile+object[i].tile]->
								draw_without_mask(GAME_VIEW_X+object[i].x+object[i].xoffs,GAME_VIEW_Y+object[i].y+object[i].yoffs,
									 screen,dx,dy,dx);
						} else {
							tiles[object[i].base_tile+object[i].tile]->
								draw(GAME_VIEW_X+object[i].x+object[i].xoffs,GAME_VIEW_Y+object[i].y+object[i].yoffs,
									 screen,col_buffer,dx,dy,dx);
						} /* if */ 
					} /* if */ 
					break;

				case T_WHITEFIREBALL:
					if (object[i].tile>=0) {
						if (item[2]) {
							tiles[object[i].base_tile+object[i].tile]->
								draw_without_mask(GAME_VIEW_X+object[i].x+object[i].xoffs,GAME_VIEW_Y+object[i].y+object[i].yoffs,
									 screen,dx,dy,dx);
						} else {
							tiles[object[i].base_tile+object[i].tile]->
								draw(GAME_VIEW_X+object[i].x+object[i].xoffs,GAME_VIEW_Y+object[i].y+object[i].yoffs,
									 screen,col_buffer,dx,dy,dx);
						} /* if */ 
					} /* if */ 
					break;


				case T_LAVA1:
				case T_LAVA2:
				case T_LAVA3:
					if (object[i].tile>=0) {
						if (item[23]) {
							tiles[object[i].base_tile+object[i].tile]->
								draw_without_mask(GAME_VIEW_X+object[i].x+object[i].xoffs,GAME_VIEW_Y+object[i].y+object[i].yoffs,
									 screen,dx,dy,dx);
						} else {
							tiles[object[i].base_tile+object[i].tile]->
								draw(GAME_VIEW_X+object[i].x+object[i].xoffs,GAME_VIEW_Y+object[i].y+object[i].yoffs,
									 screen,col_buffer,dx,dy,dx);
						} /* if */ 
					} /* if */ 
					break;

				case T_SLIME:
					if (object[i].state==0) {
						if (object[i].state2<6) {
							tiles[object[i].base_tile]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
						} else {
							tiles[object[i].base_tile+1]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
						} /* if */ 
					} else {
						if (object[i].state2<6) {
							tiles[object[i].base_tile+2]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
						} else {
							tiles[object[i].base_tile+3]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
						} /* if */ 
					} /* if */ 
					break;

				case T_WATERMONSTER:
					tiles[object[i].base_tile]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y-object[i].state2,
							 TILE_SIZE_X*3,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					tiles[object[i].base_tile+5]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*3,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					break;
				case T_WATERMONSTER_ARM:
					tiles[object[i].base_tile+1]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y-(object[i].state2*TILE_SIZE_Y)/2,
							 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
					for(j=0;j<(object[i].state2*TILE_SIZE_Y)/2;j+=TILE_SIZE_Y) {
						if ((object[i].state2&0x01)==0) {
							tiles[object[i].base_tile+2]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y-j,
									 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);				
						} else {
							tiles[object[i].base_tile+3]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y-j,
									 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);				
						} /* if */ 
					} /* for */ 
					break;
				case T_BUBBLE:
					tiles[object[i].base_tile+object[i].tile]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
					break;
				case T_FSTONE:
					tiles[object[i].base_tile+((object[i].state3>>2)&0x03)]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					break;
				case T_RSTONE:
					tiles[object[i].base_tile+((object[i].state3>>2)&0x03)]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					break;
				case T_KNIGHT:
					tiles[object[i].base_tile+object[i].state2]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					if (object[i].state==1 || object[i].state==7 || object[i].state==9) {
						if (object[i].state2==0) {
							tiles[object[i].base_tile+8]->
								draw(GAME_VIEW_X+object[i].x-TILE_SIZE_X*2,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
						} else {
							tiles[object[i].base_tile+8]->
								draw(GAME_VIEW_X+object[i].x+TILE_UNIT*2-TILE_SIZE_X*2,GAME_VIEW_Y+object[i].y-TILE_UNIT,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
						} /* if */ 
					} /* if */ 
					if (object[i].state==0 || object[i].state==6 || object[i].state==8) {
						if (object[i].state2==2) {
							tiles[object[i].base_tile+9]->
								draw(GAME_VIEW_X+object[i].x+TILE_SIZE_X*2,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
						} else {
							tiles[object[i].base_tile+9]->
								draw(GAME_VIEW_X+object[i].x+TILE_SIZE_X*2-TILE_UNIT*2,GAME_VIEW_Y+object[i].y-TILE_UNIT,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
						} /* if */ 
					} /* if */ 
					break;
				case T_BLOB:
					tiles[object[i].base_tile+object[i].state2]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					break;

				case T_PORCUPINE_BULLET:
					tiles[object[i].base_tile+8]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					break;
				case T_JUMPINGBUSH:
					tiles[object[i].base_tile]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y+object[i].state2,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					object[i].draw_y=GAME_VIEW_Y+object[i].y+object[i].state2;
					break;

				case T_BLACK:
					tiles[object[i].base_tile]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					break;
				case T_WITCH:
					if (object[i].state!=0) {
						if (item[1]) {
							tiles[object[i].base_tile]->
								draw_without_mask(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,dx,dy,dx);
						} else {
							tiles[object[i].base_tile]->
								draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
									 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
						} /* if */ 
					} /* if */ 
					break;
				case T_WHITEBEAR_BULLET:
					tiles[object[i].base_tile+4+object[i].state]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					break;
				case T_FEET:
					tiles[object[i].base_tile+object[i].tile]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y+object[i].yoffs,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					object[i].draw_y=GAME_VIEW_Y+object[i].y-TILE_SIZE_Y/2;
					break;
				case T_BOUNCINGBALL:
					if (object[i].tile>=0) {
						tiles[object[i].base_tile+object[i].tile]->
							draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y+object[i].state,
								 screen,col_buffer,dx,dy,dx);
						object[i].draw_y=GAME_VIEW_Y+object[i].y+object[i].state;
					} /* if */ 
					break;
				case T_BLUESPIDER:
					if (object[i].tile>=0) {
						tiles[object[i].base_tile+object[i].tile]->
							draw(GAME_VIEW_X+object[i].x+object[i].state2,GAME_VIEW_Y+object[i].y,
								 screen,col_buffer,dx,dy,dx);
						object[i].draw_x=GAME_VIEW_X+object[i].x+object[i].state2;
					} /* if */ 
					break; 
				case T_MEGABAT:
					tiles[object[i].base_tile+((object[i].state2>>3)%2)]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*4,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					break;
				case T_MEGABAT2:
					tiles[object[i].base_tile+((object[i].state2>>2)%2)]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					break;
				case T_MEGABAT3:
					tiles[object[i].base_tile+2+((object[i].state2>>2)%2)]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*1,screen,col_buffer,dx,dy,dx);
					break;

				case T_DEMON1:
					tiles[object[i].state]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*10,TILE_SIZE_Y*7,screen,col_buffer,dx,dy,dx);
					break;
				case T_DEMON1_BONES:
					tiles[demon_tile+12+(object[i].state2>>1)]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
					break;
				case T_DEMON1_BONES2:
					tiles[demon_tile+12+(object[i].state2>>1)]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
					break;

				case T_DEMON2:
					tiles[object[i].state]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X*6,TILE_SIZE_Y*6,screen,col_buffer,dx,dy,dx);
					break;
				case T_DEMON2_BALLS:
					tiles[demon_tile+7]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
					break;
				case T_DEMON2_HEAD:
					if (object[i].state>=0) {
						tiles[object[i].state]->
							draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
								 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
					} /* if */ 
					break;
				case T_DEMON3_FIRE:
					if (object[i].state==0) {
						int j;
						tiles[demon_tile+6]->
							draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
								 screen,col_buffer,dx,dy,dx);
						for(j=0;j<object[i].state2;j++) {
							tiles[demon_tile+7]->
								draw(GAME_VIEW_X+object[i].x+(j+2)*TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
									 screen,col_buffer,dx,dy,dx);
						} /* if */ 
						tiles[demon_tile+8]->
							draw(GAME_VIEW_X+object[i].x+(object[i].state2+2)*TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
								 screen,col_buffer,dx,dy,dx);
					} else {
						tiles[demon_tile+11]->
							draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
								 screen,col_buffer,dx,dy,dx);
						for(j=0;j<object[i].state2;j++) {
							tiles[demon_tile+10]->
								draw(GAME_VIEW_X+object[i].x-(j+1)*TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
									 screen,col_buffer,dx,dy,dx);
						} /* if */ 
						tiles[demon_tile+9]->
							draw(GAME_VIEW_X+object[i].x-(object[i].state2+2)*TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
								 screen,col_buffer,dx,dy,dx);
					} /* if */ 
					break;
				case T_DEMON9_FIRE:
					tiles[demon_tile+8]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
					for(j=0;j<object[i].state2;j++) {
						tiles[demon_tile+7]->
							draw(GAME_VIEW_X+object[i].x-(j+1)*TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
								 screen,col_buffer,dx,dy,dx);
					} /* if */ 
					tiles[demon_tile+6]->
						draw(GAME_VIEW_X+object[i].x-(object[i].state2+2)*TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
					break;
				case T_DEMON5:
					tiles[object[i].base_tile+object[i].state3]->
						draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y+TILE_SIZE_Y,
							 screen,col_buffer,dx,dy,dx);
					tiles[object[i].base_tile+object[i].tile]->
						draw(GAME_VIEW_X+object[i].x+4*TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
							 screen,col_buffer,dx,dy,dx);
					break;
				case T_DEMON5_FIRE:
					if (object[i].state==0) {
						int j;
						tiles[demon_tile+20]->
							draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
								 screen,col_buffer,dx,dy,dx);
						for(j=0;j<object[i].state2;j++) {
							tiles[demon_tile+21]->
								draw(GAME_VIEW_X+object[i].x+(j+2)*TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
									 screen,col_buffer,dx,dy,dx);
						} /* if */ 
						tiles[demon_tile+22]->
							draw(GAME_VIEW_X+object[i].x+(object[i].state2+2)*TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
								 screen,col_buffer,dx,dy,dx);
					} else {
						tiles[demon_tile+25]->
							draw(GAME_VIEW_X+object[i].x,GAME_VIEW_Y+object[i].y,
								 screen,col_buffer,dx,dy,dx);
						for(j=0;j<object[i].state2;j++) {
							tiles[demon_tile+24]->
								draw(GAME_VIEW_X+object[i].x-(j+1)*TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
									 screen,col_buffer,dx,dy,dx);
						} /* if */ 
						tiles[demon_tile+23]->
							draw(GAME_VIEW_X+object[i].x-(object[i].state2+2)*TILE_SIZE_X,GAME_VIEW_Y+object[i].y,
								 screen,col_buffer,dx,dy,dx);
					} /* if */ 
					break;
				case T_GORILLA:
					if (object[i].tile>=0) {
						if (object[i].state>=5) {
							tiles[object[i].base_tile+6]->
								draw(GAME_VIEW_X+object[i].x+object[i].xoffs,GAME_VIEW_Y+object[i].y+object[i].yoffs-2*TILE_SIZE_Y,
									 screen,col_buffer,dx,dy,dx);
							tiles[object[i].base_tile+6]->
								draw_only_mask(GAME_VIEW_X+object[i].x+object[i].xoffs,GAME_VIEW_Y+object[i].y+object[i].yoffs-3*TILE_SIZE_Y,
									 2*TILE_SIZE_X,2*TILE_SIZE_Y,
									 col_buffer,dx,dy,dx);
							tiles[object[i].base_tile+object[i].tile]->
								draw(GAME_VIEW_X+object[i].x+object[i].xoffs,GAME_VIEW_Y+object[i].y+object[i].yoffs,
									 screen,col_buffer,dx,dy,dx);
						} else {
							tiles[object[i].base_tile+object[i].tile]->
								draw(GAME_VIEW_X+object[i].x+object[i].xoffs,GAME_VIEW_Y+object[i].y+object[i].yoffs,
									 screen,col_buffer,dx,dy,dx);
						} /* if */ 
					} /* if */ 
					break;
				case T_GLIZARD_TONGUE: 
					if (object[i].state==0) {
						int j,v;
				
						if (object[i].state2<=28) {
							v=(object[i].state2/2)+2;
							if (v>=12) v=12;
							for(j=0;j<v;j++) {
								tiles[object[i].base_tile+14+j]->
									draw(GAME_VIEW_X+object[i].x+object[i].xoffs+TILE_SIZE_X+TILE_UNIT*5,GAME_VIEW_Y+object[i].y+object[i].yoffs+TILE_SIZE_Y+TILE_UNIT*5-(v-j)*TILE_SIZE_Y,
										 screen,col_buffer,dx,dy,dx);
							} /* for */ 
						} else {
							v=((48-object[i].state2)/2)+2;
							if (v>=12) v=12;
							for(j=0;j<v;j++) {
								tiles[object[i].base_tile+14+j]->
									draw(GAME_VIEW_X+object[i].x+object[i].xoffs+TILE_SIZE_X+TILE_UNIT*5,GAME_VIEW_Y+object[i].y+object[i].yoffs+TILE_SIZE_Y+TILE_UNIT*5-(v-j)*TILE_SIZE_Y,
										 screen,col_buffer,dx,dy,dx);
							} /* for */ 
						} /* if */ 
					} else {
						int j,v;
				
						if (object[i].state2<=28) {
							v=(object[i].state2/2)+2;
							if (v>=12) v=12;
							for(j=0;j<v;j++) {
								tiles[object[i].base_tile+14-(v-j)]->
									draw(GAME_VIEW_X+object[i].x+object[i].xoffs+TILE_SIZE_X+TILE_UNIT*5,GAME_VIEW_Y+object[i].y+object[i].yoffs+TILE_SIZE_Y+TILE_UNIT*5+j*TILE_SIZE_Y,
										 screen,col_buffer,dx,dy,dx);
							} /* for */ 
						} else {
							v=((48-object[i].state2)/2)+2;
							if (v>=12) v=12;
							for(j=0;j<v;j++) {
								tiles[object[i].base_tile+14-(v-j)]->
									draw(GAME_VIEW_X+object[i].x+object[i].xoffs+TILE_SIZE_X+TILE_UNIT*5,GAME_VIEW_Y+object[i].y+object[i].yoffs+TILE_SIZE_Y+TILE_UNIT*5+j*TILE_SIZE_Y,
										 screen,col_buffer,dx,dy,dx);
							} /* for */ 
						} /* if */ 
					} /* if */  
					break;
				case T_LIGHTNING:
					{
						int j,x=0;
						int init[5]={6,10,15,3,3};
						int end[5]={5,5,10,13,13};

						for(j=0;j<object[i].state3;j++) {
							tiles[object[i].base_tile+6+object[i].lightning[j]+(object[i].state2%3)*5]->
								draw(GAME_VIEW_X+object[i].x+object[i].xoffs+x,GAME_VIEW_Y+object[i].y+object[i].yoffs+j*TILE_SIZE_Y*2,
									 screen,col_buffer,dx,dy,dx);
							if (j<object[i].state3-1) x+=(end[object[i].lightning[j]]-init[object[i].lightning[j+1]])*TILE_UNIT;
						} /* for */ 
					}
					break;
				default:
					if (object[i].tile>=0) {
						tiles[object[i].base_tile+object[i].tile]->
							draw(GAME_VIEW_X+object[i].x+object[i].xoffs,GAME_VIEW_Y+object[i].y+object[i].yoffs,
								 screen,col_buffer,dx,dy,dx);
					} /* if */ 
				} /* switch */ 
				break;
			} /* switch */ 
		} /* for */ 
	} /* if */ 

} /* drawmap */ 


void drawstats(BYTE *screen,int dx,int dy)
{
	int i,val;
	char tmp[80];

	memset(screen+dx*TILE_SIZE_Y*3,0,dx*TILE_SIZE_Y);

	tiles[stats_tile]->
		draw(GAME_VIEW_X+TILE_SIZE_X*2,0,TILE_SIZE_X*30,TILE_SIZE_Y*3,screen,col_buffer,dx,dy,dx);

	/* Flechas, monedas y llaves: */ 
	sprintf(tmp,"%.3i",n_arrows);
	tile_print(tmp,GAME_VIEW_X+TILE_SIZE_X*24,0,screen,dx,dy);
	sprintf(tmp,"%.3i",n_coins);
	tile_print(tmp,GAME_VIEW_X+TILE_SIZE_X*24,TILE_SIZE_Y,screen,dx,dy);
	sprintf(tmp,"%.3i",n_keys);
	tile_print(tmp,GAME_VIEW_X+TILE_SIZE_X*24,TILE_SIZE_Y*2,screen,dx,dy);

	/* Armas: */ 
	if (current_weapon==0) 
		tiles[armas_tile]->
			draw(GAME_VIEW_X+TILE_SIZE_X*28,TILE_SIZE_Y,
				TILE_SIZE_X*2,TILE_SIZE_Y*1,screen,col_buffer,dx,dy,dx);
	if (current_weapon==1) 
		tiles[armas_tile+1]->
			draw(GAME_VIEW_X+TILE_SIZE_X*28,TILE_SIZE_Y,
				TILE_SIZE_X*2,TILE_SIZE_Y*1,screen,col_buffer,dx,dy,dx);
	if (current_weapon==2) 
		tiles[armas_tile+2]->
			draw(GAME_VIEW_X+TILE_SIZE_X*28+TILE_UNIT*4,TILE_SIZE_Y,
				TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	if (current_weapon==3) 
		tiles[armas_tile+3]->
			draw(GAME_VIEW_X+TILE_SIZE_X*28+TILE_UNIT*4,TILE_SIZE_Y,
				TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	if (current_weapon==4) 
		tiles[armas_tile+4]->
			draw(GAME_VIEW_X+TILE_SIZE_X*28,TILE_SIZE_Y,
				TILE_SIZE_X*2,TILE_SIZE_Y*1,screen,col_buffer,dx,dy,dx);
	if (current_weapon==5) 
		tiles[armas_tile+5]->
			draw(GAME_VIEW_X+TILE_SIZE_X*28,TILE_SIZE_Y,
				TILE_SIZE_X*2,TILE_SIZE_Y*1,screen,col_buffer,dx,dy,dx);

	/* Mapa: */ 
	if (map==0) {
		tile_print("CASTLE",3*TILE_SIZE_X+GAME_VIEW_X,3*TILE_SIZE_Y,screen,dx,dy);
	} else {
		sprintf(tmp,"WORLD %i",map);
		tile_print(tmp,3*TILE_SIZE_X+GAME_VIEW_X,3*TILE_SIZE_Y,screen,dx,dy);
	} /* if */ 

//	/* Debug: */ 
//	sprintf(tmp,"%i %i %i %i",logic_dx[1],logic_dy[1],logic_x[1],logic_y[1]);
//	tile_print(tmp,10*TILE_SIZE_X+GAME_VIEW_X,3*TILE_SIZE_Y,screen,dx,dy);

	/* EnergÌa: */ 
	for(i=0;i<2;i++) {
		if (player_denergy[i]<player_energy[i] && (frame_counter&0x01)==0) player_denergy[i]++;
		if (player_denergy[i]>player_energy[i] && (frame_counter&0x01)==0) player_denergy[i]--;
		if (player_dexperience[i]<player_experience[i] && (frame_counter&0x01)==0) player_dexperience[i]++;
		if (player_dexperience[i]>player_experience[i] && (frame_counter&0x01)==0) player_dexperience[i]++;
		if (player_dexperience[i]>MAX_EXPERIENCE) player_dexperience[i]=0;
	} /* for */ 

	val=min(player_max[0],96);
	B_rectangle(screen,112+val,20,2,8,dx,235);
	val=max(0,min(player_denergy[0],96));
	B_rectangle(screen,112,20,val,8,dx,47);
	if (player_dexperience[0]>0) {
		val=min(player_dexperience[0],96);
		B_rectangle(screen,112,36,val,8,dx,47);
	} /* if */ 

	val=min(player_max[1],96);
	B_rectangle(screen,288+val,20,2,8,dx,235);
	val=max(min(player_denergy[1],96),0);
	B_rectangle(screen,288,20,val,8,dx,194);
	if (player_dexperience[1]>0) {
		val=min(player_dexperience[1],96);
		B_rectangle(screen,288,36,val,8,dx,194);
	} /* if */ 


/*
	sprintf(tmp,"ST: %i",pers_state);
	tile_print(tmp,0,0,screen,dx,dy);
	sprintf(tmp,"X: %i",pers_x);
	tile_print(tmp,0,TILE_SIZE_Y,screen,dx,dy);
*/ 

} /* drawstats */ 


void tile_print(char *str,int x,int y,BYTE *screen,int dx,int dy)
{
	int i;

	for(i=0;str[i]!=0;i++) {
		if (str[i]>='A' && str[i]<='Z') {
			tiles_bmp->draw_sprite((str[i]-'A')*TILE_SIZE_X,TILE_SIZE_Y*12,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		} /* if */ 
		if (str[i]>='0' && str[i]<='9') {
			tiles_bmp->draw_sprite((26+str[i]-'0')*TILE_SIZE_X,TILE_SIZE_Y*12,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		} /* if */ 
		if (str[i]=='[') tiles_bmp->draw_sprite(36*TILE_SIZE_X,TILE_SIZE_Y*12,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]==']') tiles_bmp->draw_sprite(37*TILE_SIZE_X,TILE_SIZE_Y*12,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]==':') tiles_bmp->draw_sprite(38*TILE_SIZE_X,TILE_SIZE_Y*12,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='\\') tiles_bmp->draw_sprite(39*TILE_SIZE_X,TILE_SIZE_Y*12,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='!') tiles_bmp->draw_sprite(0,TILE_SIZE_Y*21,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='?') tiles_bmp->draw_sprite(2*TILE_SIZE_X,TILE_SIZE_Y*21,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]==',') tiles_bmp->draw_sprite(3*TILE_SIZE_X,TILE_SIZE_Y*21,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='.') tiles_bmp->draw_sprite(4*TILE_SIZE_X,TILE_SIZE_Y*21,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='\'') tiles_bmp->draw_sprite(0,TILE_SIZE_Y*22,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='-') tiles_bmp->draw_sprite(TILE_SIZE_X,TILE_SIZE_Y*22,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		/* Graphical symbols */ 
		if (str[i]=='a') tiles_bmp->draw_sprite(21*TILE_SIZE_X,TILE_SIZE_Y*22,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='b') tiles_bmp->draw_sprite(22*TILE_SIZE_X,TILE_SIZE_Y*22,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='c') tiles_bmp->draw_sprite(23*TILE_SIZE_X,TILE_SIZE_Y*22,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='d') tiles_bmp->draw_sprite(21*TILE_SIZE_X,TILE_SIZE_Y*23,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='e') tiles_bmp->draw_sprite(22*TILE_SIZE_X,TILE_SIZE_Y*23,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='f') tiles_bmp->draw_sprite(21*TILE_SIZE_X,TILE_SIZE_Y*24,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='g') tiles_bmp->draw_sprite(22*TILE_SIZE_X,TILE_SIZE_Y*24,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='h') tiles_bmp->draw_sprite(5*TILE_SIZE_X,TILE_SIZE_Y*21,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
	} /* for */ 
} /* tile_print */ 


void tile_print2(char *str,int x,int y,BYTE *screen,int dx,int dy)
{
	int i;

	for(i=0;str[i]!=0;i++) {
		B_rectangle(screen,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,dx,0);
		if (str[i]>='A' && str[i]<='Z') {
			tiles_bmp->draw_sprite((str[i]-'A')*TILE_SIZE_X,TILE_SIZE_Y*12,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		} /* if */ 
		if (str[i]>='0' && str[i]<='9') {
			tiles_bmp->draw_sprite((26+str[i]-'0')*TILE_SIZE_X,TILE_SIZE_Y*12,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		} /* if */ 
		if (str[i]=='[') tiles_bmp->draw_sprite(36*TILE_SIZE_X,TILE_SIZE_Y*12,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]==']') tiles_bmp->draw_sprite(37*TILE_SIZE_X,TILE_SIZE_Y*12,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]==':') tiles_bmp->draw_sprite(38*TILE_SIZE_X,TILE_SIZE_Y*12,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='\\') tiles_bmp->draw_sprite(39*TILE_SIZE_X,TILE_SIZE_Y*12,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='!') tiles_bmp->draw_sprite(0,TILE_SIZE_Y*21,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='?') tiles_bmp->draw_sprite(2*TILE_SIZE_X,TILE_SIZE_Y*21,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]==',') tiles_bmp->draw_sprite(3*TILE_SIZE_X,TILE_SIZE_Y*21,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='.') tiles_bmp->draw_sprite(4*TILE_SIZE_X,TILE_SIZE_Y*21,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='\'') tiles_bmp->draw_sprite(0,TILE_SIZE_Y*22,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='-') tiles_bmp->draw_sprite(TILE_SIZE_X,TILE_SIZE_Y*22,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		/* Graphical symbols: */ 
		if (str[i]=='a') tiles_bmp->draw_sprite(21*TILE_SIZE_X,TILE_SIZE_Y*22,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='b') tiles_bmp->draw_sprite(22*TILE_SIZE_X,TILE_SIZE_Y*22,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='c') tiles_bmp->draw_sprite(23*TILE_SIZE_X,TILE_SIZE_Y*22,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='d') tiles_bmp->draw_sprite(21*TILE_SIZE_X,TILE_SIZE_Y*23,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='e') tiles_bmp->draw_sprite(22*TILE_SIZE_X,TILE_SIZE_Y*23,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='f') tiles_bmp->draw_sprite(21*TILE_SIZE_X,TILE_SIZE_Y*24,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='g') tiles_bmp->draw_sprite(22*TILE_SIZE_X,TILE_SIZE_Y*24,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
		if (str[i]=='h') tiles_bmp->draw_sprite(5*TILE_SIZE_X,TILE_SIZE_Y*21,TILE_SIZE_X,TILE_SIZE_Y,x+i*TILE_SIZE_X,y,TILE_SIZE_X,TILE_SIZE_Y,screen,dx,dy,dx);
	} /* for */ 
} /* tile_print2 */ 



bool loadroom(int map,int map_x,int map_y)
{
	int i;
	char filename[80];
	char *prefixes[11]={"map","w01","w02","w03","w04","w05","w06","w07","w08","w09","w10"};
	char buffer[80],buffer2[80];
	FILE *fp;
	int n_smokes=0;

	if (room_tiles!=NULL) {
		delete room_tiles;
		room_tiles=NULL;
	} /* if */ 

	sprintf(filename,"rooms/%s%.2i%.2i.txt",prefixes[map],map_x,map_y);

	fp=f1open(filename,"r",GAMEDATA);
	if (fp==NULL) return false;

	/* Leer fondo: */ 
	fscanf(fp,"%i %i",&room_size_x,&room_size_y);

	room_tiles=new int[room_size_x*room_size_y];
	for(i=0;i<room_size_x*room_size_y;i++) {
		fscanf(fp,"%i",&(room_tiles[i]));
	} /* for */ 

	/* Leer objetos: */ 
	fscanf(fp,"%i",&n_objects);

	for(i=0;i<n_objects;i++) {
		object[i].type=T_NADA;
		object[i].x=0;
		object[i].y=0;
		object[i].xoffs=0;
		object[i].yoffs=0;
		object[i].size=0;
		object[i].enemy=0;
		object[i].state=0;
		object[i].state2=0;
		object[i].state3=0;
		object[i].last_hit=0;
		object[i].tile=0;
		object[i].union_x=-1;
		object[i].union_y=-1;

		fscanf(fp,"%s",buffer);
		if (strcmp(buffer,"LADDER")==0) {
			object[i].type=T_LADDER;
			fscanf(fp,"%i %i %i %i",&(object[i].x),&(object[i].y),&(object[i].size),&(object[i].state));
		} /* if */ 
		if (strcmp(buffer,"STONE")==0) {
			object[i].type=T_STONE;
			object[i].size=15;
			fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].state2));
		} /* if */ 
		if (strcmp(buffer,"WDOOR")==0) {
			object[i].type=T_WDOOR;
			object[i].state=0;
			fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].size));
			if (map_x==3 && map_y==12 && world_doors_open[0]) object[i].state=1;
			if (map_x==10 && map_y==11 && world_doors_open[1]) object[i].state=1;
			if (map_x==10 && map_y==8 && world_doors_open[2]) object[i].state=1;
			if (map_x==0 && map_y==8 && world_doors_open[3]) object[i].state=1;
			if (map_x==8 && map_y==8 && world_doors_open[4]) object[i].state=1;
			if (map_x==14 && map_y==12 && world_doors_open[5]) object[i].state=1;
			if (map_x==5 && map_y==2 && world_doors_open[6]) object[i].state=1;
			if (map_x==4 && map_y==4 && world_doors_open[7]) object[i].state=1;
			if (map_x==7 && map_y==0 && world_doors_open[8]) object[i].state=1;
		} /* if */ 
		if (strcmp(buffer,"DOOR")==0) {
			object[i].type=T_DOOR;
			fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].size));
		} /* if */ 
		if (strcmp(buffer,"KEYDOOR")==0) {
			object[i].type=T_KEYDOOR;
			object[i].state=0;
			fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].size));
		} /* if */ 
		if (strcmp(buffer,"WKEYDOOR")==0) {
			object[i].type=T_WKEYDOOR;
			object[i].state=0;
			fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].size));
		} /* if */ 
		if (strcmp(buffer,"WNOKEYDOOR")==0) {
			object[i].type=T_WNOKEYDOOR;
			object[i].state=0;
			fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
		} /* if */ 
		if (strcmp(buffer,"WNOKEYDOOR2")==0) {
			object[i].type=T_WNOKEYDOOR2;
			object[i].state=0;
			fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
		} /* if */ 
		if (strcmp(buffer,"WNOKEYDOOR3")==0) {
			object[i].type=T_WNOKEYDOOR3;
			object[i].state=0;
			fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
		} /* if */ 
		if (strcmp(buffer,"ARMOUR")==0) {
			object[i].type=T_ARMOUR;
			fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].state));
			object[i].tile=0;
			if (object[i].state==0) object[i].base_tile=107;
							   else object[i].base_tile=108;
		} /* if */ 
		if (strcmp(buffer,"WITEM")==0) {
			object[i].type=T_NADA;
			fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].state));
			if (!world_item[map-1][object[i].state]) {
				object[i].type=T_ITEM;
				object[i].state+=38;
			} /* if */ 
		} /* if */ 
		if (strcmp(buffer,"ITEM")==0) {
			object[i].type=T_NADA;
			fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].state));
			if (!item[object[i].state]) {
				object[i].type=T_ITEM;
			} /* if */ 
		} /* if */ 
		if (strcmp(buffer,"TOMB")==0) {
			object[i].type=T_TOMB;
			fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			object[i].state+=38;
		} /* if */ 
		if (strcmp(buffer,"HITWALL")==0) {
			object[i].type=T_HITWALL;
			fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].size));
		} /* if */ 
		if (strcmp(buffer,"ENEMY")==0) {
			fscanf(fp,"%s",buffer2);
			if (strcmp(buffer2,"WORM")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_WORM;
				object[i].size=1;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=worm_tile;
				fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].state));
			} /* if */ 
			if (strcmp(buffer2,"BAT")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_BAT;
				object[i].size=1;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=bat_tile;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"SKELETON")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_SKELETON;
				object[i].size=8;
				if (pers_right) object[i].state=1;
						   else object[i].state=0;
				if (rand()%4==0) object[i].state=1-object[i].state;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=skeleton_tile;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"SMOKE")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_SMOKE;
				object[i].state2=0;
				object[i].state=-(n_smokes*33);
				object[i].base_tile=smoke_tile;
				n_smokes++;
				fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].size));
			} /* if */ 
			if (strcmp(buffer2,"BOUNCINGBALL")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_BOUNCINGBALL;
				object[i].state=0;
				object[i].state2=0;
				object[i].size=4;
				object[i].base_tile=bouncingball_tile;
				object[i].experience=2;
				fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].state3));
			} /* if */ 
			if (strcmp(buffer2,"JUMPINGBUSH")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_JUMPINGBUSH;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=0;
				object[i].size=5;
				object[i].base_tile=jumpingbush_tile;
				object[i].experience=2;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
				object[i].state=0;
			} /* if */ 
			if (strcmp(buffer2,"BLUESPIDER")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_BLUESPIDER;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=0;
				object[i].size=1;
				object[i].base_tile=bluespider_tile;
				object[i].experience=2;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
				object[i].state=0;
			} /* if */ 
			if (strcmp(buffer2,"WHITEFIREBALL")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_WHITEFIREBALL;
				object[i].state=rand()%8;
				object[i].state2=0;
				object[i].state3=0;
				object[i].size=1;
				object[i].base_tile=whitefireball_tile;
				object[i].experience=2;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"KNIGHT")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_KNIGHT;
				object[i].size=6;
				if (pers_right) object[i].state=1;
						   else object[i].state=0;
				if (rand()%4==0) object[i].state=1-object[i].state;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=knight_tile;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"BAMBU")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_BAMBU;
				object[i].size=2;
				object[i].state=0;
				object[i].state2=-1;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=bambu_tile;
				object[i].experience=2;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"PORCUPINE")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_PORCUPINE;
				object[i].size=2;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=porcupine_tile;
				object[i].experience=4;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"BLACK")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_BLACK;
				object[i].size=0;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=black_tile;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"WITCH")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_WITCH;
				object[i].size=3;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=witch_tile;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"WHITEBEAR")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_WHITEBEAR;
				object[i].size=2;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=whitebear_tile;
				object[i].experience=4;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
				if (pers_right) object[i].state=1;
						   else object[i].state=0;
			} /* if */ 
			if (strcmp(buffer2,"FEET")==0) {
				int val;
				object[i].type=T_ENEMY;
				object[i].enemy=T_FEET;
				object[i].size=4;
				object[i].state=3;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=feet_tile;
				object[i].experience=4;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
				val=object[i].x/(TILE_SIZE_X*2);
				if ((val&0x01)==0) object[i].state3=32;
			} /* if */ 
			if (strcmp(buffer2,"REDJUMPER")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_REDJUMPER;
				object[i].size=3;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=redjumper_tile;
				object[i].experience=4;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
				if (pers_right) object[i].state=1;
						   else object[i].state=0;
			} /* if */ 
			if (strcmp(buffer2,"REDVENT")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_VENT2;
				object[i].size=1;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=blob_tile;
				object[i].experience=2;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
				if (pers_right) object[i].state=1;
						   else object[i].state=0;
				if (rand()%2==0) object[i].state=2;
			} /* if */ 
			if (strcmp(buffer2,"LIVINGWALL")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_LIVINGWALL;
				object[i].size=32;
				object[i].state=0;
				object[i].tile=-1;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=livingwall_tile;
				fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].state2));
			} /* if */ 
			if (strcmp(buffer2,"MEGABAT")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_MEGABAT;
				object[i].size=1;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=megabat_tile;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"WHITESTAR")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_WHITESTAR;
				object[i].size=1;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=1;
				object[i].last_hit=0;
				object[i].base_tile=whitestar_tile;
				object[i].experience=2;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"SPIDER")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_SPIDER;
				object[i].size=1;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=spider_tile;
				object[i].tile=0;
				object[i].experience=2;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"KNIGHTHEAD")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_KNIGHTHEAD;
				object[i].size=16;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=knighthead_tile;
				object[i].tile=0;
				object[i].experience=8;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 		
			if (strcmp(buffer2,"CHICKEN")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_CHICKEN;
				object[i].size=5;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=chicken_tile;
				object[i].tile=0;
				object[i].experience=6;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"ROCKMAN")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_ROCKMAN;
				object[i].size=4;
				object[i].state=0;
				object[i].state2=-8*(rand()%16);
				object[i].state3=0;
				object[i].last_hit=0;
				object[i].base_tile=rockman_tile;
				object[i].tile=-1;
				object[i].experience=8;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"CLOUD")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_CLOUD;
				object[i].size=0;
				object[i].state=0;
				object[i].state2=0;
				object[i].state3=rand()%12;
				object[i].last_hit=0;
				object[i].base_tile=cloud_tile;
				object[i].tile=0;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"GHOST")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_GHOST;
				object[i].size=7;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=ghost_tile;
				object[i].tile=0;
				object[i].experience=6;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"HEAD")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_HEAD;
				object[i].size=3;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=head_tile;
				fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].state));
				object[i].tile=2+object[i].state;
			} /* if */ 
			if (strcmp(buffer2,"WORM2")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_WORM2;
				object[i].size=2;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=worm2_tile;
				object[i].tile=0;
				object[i].experience=4;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"OCTOPUS")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_OCTOPUS;
				object[i].size=0;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].experience=4;
				fscanf(fp,"%i %i %i %i",&(object[i].x),&(object[i].y),&(object[i].state),&(object[i].state2));
				object[i].base_tile=octopus_tile+object[i].state2;
				object[i].state2=0;
				object[i].tile=0;
			} /* if */ 
			if (strcmp(buffer2,"HAIRY")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_HAIRY;
				object[i].size=24;
				object[i].state=1+(rand()%11);
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=hairy_tile;
				object[i].tile=0;
				object[i].experience=10;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"WATERBUG")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_WATERBUG;
				object[i].size=1;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=waterbug_tile;
				object[i].tile=0;
				object[i].experience=6;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"STONEMAN")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_STONEMAN;
				object[i].size=5;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=stoneman_tile;
				object[i].tile=0;
				object[i].experience=6;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"PACMAN")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_PACMAN;
				object[i].size=6;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=pacman_tile;
				object[i].tile=0;
				object[i].experience=8;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"OWL")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_OWL;
				object[i].size=9;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=owl_tile;
				object[i].tile=0;
				object[i].experience=10;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"GREENMONKEY")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_GREENMONKEY;
				object[i].size=4;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=greenmonkey_tile;
				object[i].tile=0;
				object[i].experience=8;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"PLANT")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_PLANT;
				object[i].size=24;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=plant_tile;
				object[i].tile=0;
				object[i].experience=12;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"TRANSFORMER")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_TRANSFORMER;
				object[i].size=4;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=transformer_tile;
				object[i].tile=0;
				object[i].experience=6;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"FLAME")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_FLAME;
				object[i].size=4;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=flame_tile;
				object[i].tile=0;
				object[i].experience=8;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"WITCH2")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_WITCH2;
				object[i].size=4;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=witch2_tile;
				object[i].tile=0;
				object[i].experience=8;
				fscanf(fp,"%i %i %i",&(object[i].x),&(object[i].y),&(object[i].state3));
			} /* if */ 
			if (strcmp(buffer2,"CYCLOPS")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_CYCLOPS;
				object[i].size=35;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=cyclops_tile;
				object[i].tile=0;
				object[i].experience=16;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"LCLOUD")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_LCLOUD;
				object[i].state=rand()%2;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=lcloud_tile;
				object[i].tile=0;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"SNAKE")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_SNAKE;
				object[i].size=4;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=snake_tile;
				object[i].tile=0;
				object[i].experience=8;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"GLIZARD")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_GLIZARD;
				object[i].size=10;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=glizard_tile;
				object[i].tile=0;
				object[i].experience=16;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"GORILLA")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_GORILLA;
				object[i].size=4;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=gorilla_tile;
				object[i].tile=0;
				object[i].experience=12;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 
			if (strcmp(buffer2,"BDEMON")==0) {
				object[i].type=T_ENEMY;
				object[i].enemy=T_BDEMON;
				object[i].size=60;
				object[i].state=0;
				object[i].state2=0;
				object[i].last_hit=0;
				object[i].base_tile=bdemon_tile;
				object[i].tile=0;
				object[i].experience=16;
				object[i].xoffs=5*TILE_SIZE_X;
				fscanf(fp,"%i %i",&(object[i].x),&(object[i].y));
			} /* if */ 


		} /* if */ 
	} /* for */ 

	/* ESPECIALES: */ 
	slime_counter=0;
	stones_counter=0;
	room_slimes=false;
	room_watermonster=false;
	watermonster_state=-128;
	room_demonroom=false;
	room_fallingstones=false;
	room_rollingstones=false;
	room_blobs=false;
	room_fairy=-1;
	room_appearwall=-1;
	room_appearwall_x=0;
	room_appearwall_y=0;
	room_lava=false;
	room_piranhas=false;
	room_bfly=0;
	room_waterdragons=-1;
	room_birds=-1;
	room_reddragon=0;

	while(1==fscanf(fp,"%s",buffer)) {
		if (strcmp(buffer,"SLIME")==0) room_slimes=true;
		if (strcmp(buffer,"WATERMONSTER")==0) room_watermonster=true;
		if (strcmp(buffer,"DEMONROOM")==0) {
			if (map>=next_world) room_demonroom=true;
		} /* if */ 
		if (strcmp(buffer,"FALLINGSTONES")==0) room_fallingstones=true;
		if (strcmp(buffer,"ROLLINGSTONES")==0) room_rollingstones=true;
		if (strcmp(buffer,"BLOBS")==0) room_blobs=true;
		if (strcmp(buffer,"FAIRY")==0) {
			room_fairy=0;
			fscanf(fp,"%i %i",&fairy_x,&fairy_y);
			/* Crear burbujas: */ 
			for(i=0;i<16;i++) {
				int obj=add_object(T_FAIRY_BUBBLE,rand()%24*TILE_SIZE_X+4*TILE_SIZE_X,
												  rand()%12*TILE_SIZE_X+4*TILE_SIZE_X,0,0,0);
				object[obj].state3=rand()%16;
				object[obj].state=(rand()%2)+1;
				if ((rand()%2)==0) object[obj].state=-object[obj].state;
				object[obj].state2=(rand()%2)+1;
				if ((rand()%2)==0) object[obj].state2=-object[obj].state2;
			} /* for */ 
		} /* if */ 
		if (strcmp(buffer,"APPEARWALL")==0) {
			room_appearwall=0;
			fscanf(fp,"%i %i",&room_appearwall_x,&room_appearwall_y);
		} /* if */ 
		if (strcmp(buffer,"LAVA")==0) {
			room_lava=true;
		} /* if */ 
		if (strcmp(buffer,"PIRANHAS")==0) {
			room_piranhas=true;
		} /* if */ 
		if (strcmp(buffer,"BUTTERFLY")==0) {
			room_bfly=1;
		} /* if */ 
		if (strcmp(buffer,"WATERDRAGONS")==0) {
			room_waterdragons=256;
		} /* if */ 
		if (strcmp(buffer,"BIRDS")==0) {
			room_birds=0;
		} /* if */ 
		if (strcmp(buffer,"BIRDS2")==0) {
			room_birds=1;
		} /* if */ 
		if (strcmp(buffer,"REDDRAGON")==0) {
			room_reddragon=1;
		} /* if */ 
		if (strcmp(buffer,"PAMPERSE")==0) {
			int i;
			bool found=false;

			for(i=0;!found && i<HP_OBJS_MAX;i++) {
				if (HP_obj[i]!=-1 && HP_room_x[i]==map_x && HP_room_y[i]==map_y) {
					if (HP_obj_type[i]==T_STONE && HP_obj_aux[i]==40) found=true;
				} /* if */ 
			} /* for */ 

			while(!found) {
				i=rand()%n_objects;
				if (object[i].type==T_STONE) {
					object[i].state2=40;
					found=true;
				} /* if */ 
			} /* while */ 
		} /* if */ 
	} /* while */ 

	fclose(fp);

	return true;
} /* loadroom */ 



int search_ladder(int x,int y)
{
	int i;

	for(i=0;i<n_objects;i++) {
		if (object[i].type==T_LADDER &&
			(object[i].x)==x &&
			(object[i].y)<=(y+3*TILE_SIZE_Y) &&
			(object[i].y+(object[i].size-2)*TILE_SIZE_Y)>=y) return i;
	} /* for */ 

	return -1;
} /* search_ladder */ 


int add_object(int type,int x,int y,int size,int enemy,int state)
{
	int i;

	for(i=0;i<n_objects;i++) {
		/* Si hay alg˙n objeto no utilizado, usarlo: */ 
		if (object[i].type==T_NADA) {
			object[i].type=type;
			object[i].x=x;
			object[i].y=y;
			object[i].xoffs=0;
			object[i].yoffs=0;
			object[i].size=size;
			object[i].enemy=enemy;
			object[i].state=state;
			object[i].state2=0;
			object[i].state3=0;
			object[i].last_hit=HIT_PERIOD;
			object[i].tile=0;
			object[i].union_x=-1;
			object[i].union_y=-1;
			object[i].experience=0;
			object[i].draw_x=x;
			object[i].draw_y=y;
			return i;
		} /* if */ 
	} /* for */ 

	object[n_objects].type=type;
	object[n_objects].x=x;
	object[n_objects].y=y;
	object[n_objects].xoffs=0;
	object[n_objects].yoffs=0;
	object[n_objects].size=size;
	object[n_objects].enemy=enemy;
	object[n_objects].state=state;
	object[n_objects].state2=0;
	object[n_objects].state3=0;
	object[n_objects].last_hit=HIT_PERIOD;
	object[n_objects].tile=0;
	object[n_objects].union_x=-1;
	object[n_objects].union_y=-1;
	object[n_objects].experience=0;
	object[n_objects].draw_x=x;
	object[n_objects].draw_y=y;
	

	n_objects++;

	return n_objects-1;
} /* add_object */ 


void guardar_configuracion(char *filename)
{
	FILE *fp;

	fp=f1open(filename,"w+",USERDATA);
	if (fp!=NULL) {
		/* Keyboard configuration: */ 
		fprintf(fp,"%i %i %i %i\n",UP_KEY,DOWN_KEY,LEFT_KEY,RIGHT_KEY);
		fprintf(fp,"%i %i %i %i\n",SWORD_KEY,WEAPON_KEY,ITEM_KEY,PAUSE_KEY);

		/* Graphics path: */ 
		fprintf(fp,"%s\n",g_path);

		/* Sound path: */ 
		fprintf(fp,"%s\n",s_path);

		/* Music and SFX volumes: */ 
		fprintf(fp,"%i %i\n",music_volume,sfx_volume);
	} /* if */ 

	fclose(fp);
} /* guardar_configuraciÛn */ 



bool cargar_configuracion(char *filename)
{
	FILE *fp;
	int i,itmp[4];
	char tmp[256];

	fp=f1open(filename,"r",USERDATA);
	if (fp==NULL) return false;

	/* Keyboard configuration: */ 
	if (4!=fscanf(fp,"%i %i %i %i",&itmp[0],&itmp[1],&itmp[2],&itmp[3])) return false;
	UP_KEY = (SDLKey) itmp[0];
	DOWN_KEY = (SDLKey) itmp[1];
	LEFT_KEY = (SDLKey) itmp[2];
	RIGHT_KEY = (SDLKey) itmp[3];

	if (4!=fscanf(fp,"%i %i %i %i",&itmp[0],&itmp[1],&itmp[2],&itmp[3])) return false;
	SWORD_KEY = (SDLKey) itmp[0];
	WEAPON_KEY = (SDLKey) itmp[1];
	ITEM_KEY = (SDLKey) itmp[2];
	PAUSE_KEY = (SDLKey) itmp[3];

	/* Graphics path: */ 
	fscanf(fp,"%s",tmp);

	for(i=0;i<n_g_paths;i++) {
		if (strcmp(tmp,g_paths[i])==0) {
			act_g_path=i;
			g_path=g_paths[i];
		} /* if */ 
	} /* for */ 

	if (act_g_path==-1) return false;

	/* Sound path: */ 
	fscanf(fp,"%s",tmp);

	for(i=0;i<n_s_paths;i++) {
		if (strcmp(tmp,s_paths[i])==0) {
			act_s_path=i;
			s_path=s_paths[i];
		} /* if */ 
	} /* for */ 

	if (act_s_path==-1) return false;

	/* Music and SFX volumes: */ 
	fscanf(fp,"%i %i",&music_volume,&sfx_volume);

	fclose(fp);
	return true;
} /* guardar_configuraciÛn */ 


void configuracion_por_defecto(void)
{
	UP_KEY=SDLK_UP;
	DOWN_KEY=SDLK_DOWN;
	LEFT_KEY=SDLK_LEFT;
	RIGHT_KEY=SDLK_RIGHT;
	SWORD_KEY=SDLK_SPACE;
	WEAPON_KEY=SDLK_m;
	ITEM_KEY=SDLK_F1;
	PAUSE_KEY=SDLK_F2;

	// default gfx set: naramura
	act_g_path=0;
	g_path=g_paths[4];

	// default sound set: jorito
	act_s_path=0;
	s_path=s_paths[2];

} /* condiguracion_por_defecto */ 


void HP_objs_reset(void)
{
	int i;
	for(i=0;i<HP_OBJS_MAX;i++) HP_obj[i]=-1;
} /* HP_objs_reset */ 


void HP_filter(int room_x,int room_y)
{
	int max_dist;
	int dist;
	int i;

	for(i=0;i<HP_OBJS_MAX;i++) {
		if (HP_obj[i]!=-1) {
			dist=abs(room_x-HP_room_x[i])+abs(room_y-HP_room_y[i]);
			max_dist=2;
			switch(HP_obj_type[i]) {
			case T_LADDER:max_dist=5;
						  break;
			case T_STONE:max_dist=5;
						 break;
			case T_ENEMY:max_dist=2;
						 break;
			} /* switch */ 
			if (dist>max_dist) HP_obj[i]=-1;
		} /* if */ 
	} /* for */ 

} /* HP_filter */ 


void HP_restore(int room_x,int room_y)
{
	int i;

	for(i=0;i<HP_OBJS_MAX;i++) {
		if (HP_obj[i]!=-1 &&
			HP_room_x[i]==room_x &&
			HP_room_y[i]==room_y) {
			if (object[HP_obj[i]].type==T_STONE) {
				if (object[HP_obj[i]].state2<37 && object[HP_obj[i]].state2>0) {
					/* Arma o item: */ 
					if (!item[object[HP_obj[i]].state2-1]) {
						object[HP_obj[i]].type=T_ITEM;
						object[HP_obj[i]].state=object[HP_obj[i]].state2-1;
						if (object[HP_obj[i]].state>=32) {
							/* Los objetos arma son m·s pequeÒos: */ 
							object[HP_obj[i]].y+=TILE_SIZE_Y;
						} /* if */ 
					} else {
						object[HP_obj[i]].type=T_NADA;
					} /* if */ 
				} else {
					if (object[HP_obj[i]].state2<0) {
						if (HP_obj_aux[i]==0) {
							switch(object[HP_obj[i]].state2) {
							case -1:object[HP_obj[i]].type=T_ITEM;
									object[HP_obj[i]].size=10;
									object[HP_obj[i]].state=-1;
									break;
							case -2:object[HP_obj[i]].type=T_ITEM;
									object[HP_obj[i]].size=2;
									object[HP_obj[i]].state=-2;
									break;
							case -3:object[HP_obj[i]].type=T_ITEM;
									object[HP_obj[i]].size=50;
									object[HP_obj[i]].state=-3;
									break;
							default:
								object[HP_obj[i]].type=T_NADA;
							} /* switch */ 
						} else {
							object[HP_obj[i]].type=T_NADA;
						} /* if */ 
					} else {
						/* Õtem de mundo: */ 
						object[HP_obj[i]].type=T_NADA;
						if (object[HP_obj[i]].state2==38 && next_world==0 && !world_key) {
							/* Llave de mundo: */ 
							object[HP_obj[i]].type=T_ITEM;
							object[HP_obj[i]].state=37;
						} /* if */ 
						if (object[HP_obj[i]].state2==37 && !cross) {
							/* Cruz: */ 
							object[HP_obj[i]].type=T_ITEM;
							object[HP_obj[i]].state=36;
						} /* if */ 

						if (object[HP_obj[i]].state2==40 || HP_obj_aux[i]==40) {
							object[HP_obj[i]].type=T_PAMPERSE;
						} /* if */ 
					} /* if */ 
				} /* if */ 
			} else {
				object[HP_obj[i]].type=T_NADA;
			} /* if */ 
		} /* if */ 
	} /* for */ 
} /* HP_restore */ 


void HP_add_record(int room_x,int room_y,int obj,int type)
{
	int i;
	bool found=false;

	for(i=0;i<HP_OBJS_MAX && !found;i++) {
		if (HP_obj[i]==-1) {
			HP_room_x[i]=room_x;
			HP_room_y[i]=room_y;
			HP_obj[i]=obj;
			HP_obj_type[i]=type;
			HP_obj_aux[i]=0;
			if (type==T_STONE && object[obj].state2==40) 
				HP_obj_aux[i]=40;
			found=true;
		} /* if */ 
	} /* for */ 
} /* HP_add_Record */ 


void HP_modify_record_aux(int room_x,int room_y,int obj,int aux) 
{
	int i;
	bool found=false;

	for(i=0;i<HP_OBJS_MAX && !found;i++) {
		if (HP_obj[i]==obj && HP_room_x[i]==room_x && HP_room_y[i]==room_y) {
			HP_obj_aux[i]=aux;
			found=true;
		} /* if */ 
	} /* for */ 
} /* HP_add_Record */ 


void clear_typed_word(void)
{
	int i;

	for(i=0;i<16;i++) last_word[i]=SDLK_UNKNOWN;

} /* clear_typed_word */ 



void check_typed_word(void)
{
	BYTE *keyboard;

	SDL_PumpEvents();
	keyboard = (unsigned char *)SDL_GetKeyState(NULL);

	SDLKey i;
	int j;
	for(i=SDLK_a;i<=SDLK_z;i=SDLKey(int(i)+1)) {
		if (keyboard[i] && !old_keyboard[i]) {
			for(j=15;j>0;j--) last_word[j]=last_word[j-1];
			last_word[0]=i;
			return;
		} /* if */ 
	} /* if */ 
} /* check_typed_word */ 


bool typed_word_p(char *word)
{
	char *str,c;
	int i,l;

	l=strlen(word);
	if (l>16) l=16;

	for(i=0;i<l;i++) {
		c=word[(l-1)-i];
		str=SDL_GetKeyName(last_word[i]);
		if (str[1]!=0 || str[0]!=c) return false;
	} /* for */ 

	return true;
} /* typed_word_p */ 


void draw_items_map(int map,int map_x,int map_y,bool selfindicator,unsigned char *screen,int dx,int dy)
{
	int offs[10]={3,0,2,1,0,1,1,1,2,2};
	int i=0,j=0;
	int maps[10][100]={ {0,1,1,0,0,0,0,0,0,0,
						 0,1,0,0,0,0,0,0,0,0,
						 0,1,1,0,0,0,0,0,0,0,
						 0,1,0,0,0,0,0,0,0,0,
						 0,1,0,0,0,0,0,0,0,0,
						 1,1,1,0,0,0,0,0,0,0,
						 1,0,1,0,0,0,0,0,0,0,
						 1,1,1,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0},

						{0,0,0,0,0,0,0,1,0,0,
						 0,0,0,1,1,1,1,1,1,0,
						 0,1,1,1,1,0,0,0,0,0,
						 1,1,0,0,0,0,0,0,0,0,
						 1,1,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0},

						{0,0,0,1,0,0,0,0,0,0,
						 0,0,0,1,0,0,0,0,0,0,
						 0,0,1,1,0,0,0,0,0,0,
						 0,0,1,1,0,0,0,0,0,0,
						 0,0,1,1,0,0,0,0,0,0,
						 0,0,1,0,0,0,0,0,0,0,
						 1,1,1,1,1,0,0,0,0,0,
						 0,0,1,0,0,0,0,0,0,0,
						 0,0,1,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0},

						{0,0,1,1,1,0,0,0,0,0,
						 0,0,1,1,1,0,0,0,0,0,
						 0,0,1,1,1,0,0,0,0,0,
						 0,1,1,1,1,0,0,0,0,0,
						 1,1,1,0,1,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0},

						{0,0,0,1,0,0,0,0,0,0,
						 0,0,1,1,0,1,0,0,0,0,
						 0,1,1,0,0,1,1,0,0,0,
						 0,1,0,0,0,0,1,0,0,0,
						 1,1,1,1,1,1,1,1,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0},

						{0,0,0,1,0,0,0,0,0,0,
						 0,0,1,1,1,0,0,0,0,0,
						 0,0,0,1,0,0,0,0,0,0,
						 0,1,0,1,0,1,0,0,0,0,
						 1,1,0,1,0,1,1,0,0,0,
						 0,1,1,1,1,1,0,0,0,0,
						 0,0,0,1,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0},

						{0,0,0,1,0,0,0,0,0,0,
						 1,0,1,1,1,0,1,0,0,0,
						 1,1,1,1,1,1,1,0,0,0,
						 0,0,1,1,1,0,0,0,0,0,
						 0,0,1,1,1,0,0,0,0,0,
						 0,0,0,1,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0},

						{0,0,0,1,0,0,0,0,0,0,
						 0,1,0,1,0,1,0,0,0,0,
						 1,1,1,1,1,1,1,0,0,0,
						 0,1,0,1,0,1,0,0,0,0,
						 1,1,1,1,1,1,1,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0},

						{1,1,1,0,0,0,0,0,0,0,
						 0,1,1,1,0,0,0,0,0,0,
						 0,1,0,1,1,1,0,0,0,0,
						 0,1,0,1,0,1,0,0,0,0,
						 0,1,0,1,0,1,0,0,0,0,
						 0,1,0,1,1,1,0,0,0,0,
						 0,1,0,1,0,0,0,0,0,0,
						 1,1,1,1,1,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0},

						{1,1,1,1,1,1,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0}};

	if (map>=1 && map<=10) {
		for(j=0;j<9;j++) {
			for(i=0;i<9;i++) {
				if (maps[map-1][i+j*10]==1) {
					B_rectangle(screen,
								GAME_VIEW_X+TILE_SIZE_X*3+(offs[map-1]+i)*TILE_SIZE_X,
								GAME_VIEW_Y+15*TILE_SIZE_Y+j*TILE_SIZE_Y/2,
								TILE_SIZE_X-TILE_UNIT,TILE_SIZE_X/2-TILE_UNIT,dx,255);
				} /* if */ 
			} /* for */ 
		} /* for */ 
	} /* if */ 

	/* Draw extras: */ 
	if (map==1) {
		i=0;
		j=6;
		B_rectangle(screen,
					GAME_VIEW_X+TILE_SIZE_X*3+(offs[map-1]+i)*TILE_SIZE_X,
					GAME_VIEW_Y+15*TILE_SIZE_Y+j*TILE_SIZE_Y/2,
					TILE_SIZE_X-TILE_UNIT,TILE_SIZE_X/2-TILE_UNIT,dx,255);
	} /* if */ 

	if (item[13]) {
		
		if (map==1) {i=1;j=7;}
		if (map==2) {i=8;j=1;}
		if (map==3) {i=3;j=0;}
		if (map==4) {i=4;j=4;}
		if (map==5) {i=7;j=4;}
		if (map==6) {i=3;j=6;}
		if (map==7) {i=3;j=5;}
		if (map==8) {i=6;j=4;}
		if (map==9) {i=0;j=0;}
		if (map==10) {i=4;j=0;}
		B_rectangle(screen,
					GAME_VIEW_X+TILE_SIZE_X*3+(offs[map-1]+i)*TILE_SIZE_X,
					GAME_VIEW_Y+15*TILE_SIZE_Y+j*TILE_SIZE_Y/2,
					TILE_SIZE_X-TILE_UNIT,TILE_SIZE_X/2-TILE_UNIT,dx,64);
	} /* if */ 

	if (selfindicator) {
		i=map_x;
		j=map_y;
		B_rectangle(screen,
					GAME_VIEW_X+TILE_SIZE_X*3+(offs[map-1]+i)*TILE_SIZE_X,
					GAME_VIEW_Y+15*TILE_SIZE_Y+j*TILE_SIZE_Y/2,
					TILE_SIZE_X-TILE_UNIT,TILE_SIZE_X/2-TILE_UNIT,dx,180);
	} /* if */ 


} /* draw_map */ 


bool iswall(int x,int y)
{
	if (x<0 || x>=room_size_x || 
		y<0 || y>=room_size_y) return true;
	if (room_tiles[x+y*room_size_x]==105 ||
		room_tiles[x+y*room_size_x]==106 ||
		room_tiles[x+y*room_size_x]==125 ||
		room_tiles[x+y*room_size_x]==126 ||
		room_tiles[x+y*room_size_x]==181) return true;
	return false;
} /* iswall */ 


bool iswall_or_blockingobject(int x,int y)
{
	int i;
	if (iswall(x,y)) return true;

	for(i=0;i<n_objects;i++) {	
		if (object[i].type==T_KEYDOOR ||
			object[i].type==T_WKEYDOOR ||
			object[i].type==T_WNOKEYDOOR) {
			if (object[i].x/TILE_SIZE_X==x && 
				object[i].y/TILE_SIZE_Y<=y &&
				object[i].y/TILE_SIZE_Y+2>=y) return true;
		} /* if */ 
		if (object[i].type==T_STONE) {
			if ((object[i].x/TILE_SIZE_X==x || (object[i].x/TILE_SIZE_X)==x-1) &&
				(object[i].y/TILE_SIZE_Y==y || (object[i].y/TILE_SIZE_Y)==y-1)) return true;
		} /* if */ 
	} /* for */ 

	return false;
} /* iswall_or_blockingobject */ 


void get_palette(void)
{
	char tmp[256];
	/* Get the palette: */ 
	SDL_Surface *img=0;

	sprintf(tmp,"%skonami.pcx",g_path);
	img = IMG_Load(tmp);
	if (img!=0) {
		SDL_SetColors(screen, img->format->palette->colors, 0, img->format->palette->ncolors);
		SDL_FreeSurface(img);
	} else {
		sprintf(tmp,"%skonami.pcx",default_g_path);
		img = IMG_Load(tmp);
		if (img!=0) {
			SDL_SetColors(screen, img->format->palette->colors, 0, img->format->palette->ncolors);
			SDL_FreeSurface(img);
		} /* if */ 
	} /* if */ 
} /* get_palette */ 


Bitmap *load_bitmap(char *name)
{
	char tmp[256];
	Bitmap *bmp;

	sprintf(tmp,"%s%s",g_path,name);
	bmp=new Bitmap(tmp);

	if (bmp->get_image()!=0) return bmp;

	sprintf(tmp,"%s%s",default_g_path,name);
	bmp=new Bitmap(tmp);

	if (bmp->get_image()!=0) return bmp;

	fprintf(stderr,"ERROR: file %s not found.\n",name);

	return 0;
} /* load_bitmap */ 


void ReloadStuff(int dx,int dy) 
{
	ReloadGraphics(dx,dy);
	ReloadSound();
} /* ReloadStuff */ 

void ReloadSound(void)
{	
	/* Sonidos: */  
	S_gamestart=Sound_create_sound("gamestart",0);
	S_jump=Sound_create_sound("jump",0); 
	S_sword=Sound_create_sound("sword",0); 
	S_swordhit=Sound_create_sound("stone_hit",0); 
	S_item=Sound_create_sound("itemget",0); 
	S_ah=Sound_create_sound("ah",0); 
	S_ah2=Sound_create_sound("ah2",0); 
	S_powerup=Sound_create_sound("powerup",0); 
	S_enemyhit=Sound_create_sound("enemy_hit",0); 
	S_skeletonhit=Sound_create_sound("skeleton_hit",0); 
	S_door=Sound_create_sound("doors",0); 
	S_enemykill=Sound_create_sound("enemy_kill",0); 
	S_ladderdisapear=Sound_create_sound("disappear",0); 
	S_armourhit=Sound_create_sound("sword_armour",0); 
	S_pause=Sound_create_sound("pause",0); 
	S_death=Sound_create_sound("death",0); 
	S_gameover=Sound_create_sound("game_over",0); 
	S_worldkey=Sound_create_sound("worldkey",0); 
	S_entering=Sound_create_sound("entering",0); 
	S_enterwater=Sound_create_sound("enterwater",0); 
	S_select=Sound_create_sound("select",0); 
	S_firearrow=Sound_create_sound("firearrow",0); 
	S_firearrow2=Sound_create_sound("firearrow2",0); 
	S_fall=Sound_create_sound("fall",0);
	S_waterhit=Sound_create_sound("waterhit",0);
	S_lavahit=Sound_create_sound("lavahit",0);
	S_nocoins=Sound_create_sound("nocoins",0);
	S_stones=Sound_create_sound("stones",0);
	S_blob=Sound_create_sound("blob",0);
	S_shield=Sound_create_sound("shield",0);
	S_bell=Sound_create_sound("bell",0);
	S_flapwings=Sound_create_sound("flapwings",0);
	S_rotatedoor=Sound_create_sound("rotatedoor",0);
	S_demon2ball=Sound_create_sound("demon2ball",0);
	S_appearing=Sound_create_sound("appearing",0);
	S_firebreath=Sound_create_sound("fbreath",0);
	S_F1=Sound_create_sound("f1",0);
	S_karrow=Sound_create_sound("knightarrow",0);
	S_jumptree=Sound_create_sound("jumptree",0);
	S_enemybullet=Sound_create_sound("enemybullet",0);
	S_headbullet=Sound_create_sound("headbullet",0);
	S_firefire=Sound_create_sound("firefire",0);
	S_dropmine=Sound_create_sound("dropmine",0);
	S_fireball=Sound_create_sound("fireball",0);
	S_fireball2=Sound_create_sound("fireball2",0);
	S_wdoor=Sound_create_sound("wdoor",0);
	S_door2=Sound_create_sound("door2",0);
	S_demonhit=Sound_create_sound("demon_hit",0);
	S_bearbullet=Sound_create_sound("bearbullet",0);
	S_chickenbomb=Sound_create_sound("chickenbomb",0);
	S_colormonster=Sound_create_sound("colormonster",0);
	S_waterstone=Sound_create_sound("waterstone",0);
	S_demon4jump=Sound_create_sound("demon4jump",0);
	S_rockman=Sound_create_sound("rockman",0);
	S_mine=Sound_create_sound("mine",0);
	S_bible=Sound_create_sound("bible",0);
	S_demon1bones=Sound_create_sound("demon1bones",0);
	S_demon1jump=Sound_create_sound("demon1jump",0);
	S_owl=Sound_create_sound("owl",0);
	S_demon7bullet=Sound_create_sound("demon7bullet",0);
	S_demon8bullet=Sound_create_sound("demon8bullet",0);
	S_flamebullet=Sound_create_sound("flamebullet",0);
	S_snakebullet=Sound_create_sound("snakebullet",0);
	S_gorilla=Sound_create_sound("gorilla",0);
	S_lizardtongue=Sound_create_sound("lizardtongue",0);
	S_bdemonbullet=Sound_create_sound("bdemonbullet",0);
	S_lightning=Sound_create_sound("lightning",0);
} /* ReloadSound */ 


void GameInit(int dx,int dy)
{
	init_paths();
	ReloadStuff(dx,dy);

	/* M·quina de estados: */ 
	STATE=0;
	SUBSTATE=0;

	/* Inicializa juego: */ 
	map=0;
	map_x=5;
	map_y=12;
	room_tiles=NULL;
	newgame();
} /* GameInit */ 


