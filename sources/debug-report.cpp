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
#include "filehandling.h"

extern int SCREEN_X,SCREEN_Y,COLOUR_DEPTH;
extern bool fullscreen;
extern int REDRAWING_PERIOD;
extern float frames_per_sec;
extern int init_time;

extern char **g_paths,*default_g_path;
extern int n_g_paths,act_g_path;
extern char *g_path;
extern char **s_paths,*default_s_path;
extern int n_s_paths,act_s_path;
extern char *s_path;


/* Gr·ficos: */ 
extern Bitmap *konami_bmp,*menu_bmp,*tiles_bmp,*enemy_bmp,*enemy2_bmp,*demon_bmp,*final_bmp;
extern int n_tiles;
extern CTile **tiles;

extern BYTE *col_buffer;
extern BYTE *buffer_screen;

/* Estado: */ 
extern int STATE,SUBSTATE;
extern int cycle;

/* Juego: */ 
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
extern unsigned char old_keyboard[SDLK_LAST];
extern int in_ladder;			/* En quÈ escalera est· el personaje	*/ 
extern bool previous_x_collision;	/* Hubo una colisiÛn en X en el frame anterior? */ 
extern bool previous_y_collision;	/* Hubo una colisiÛn en Y en el frame anterior? */ 
extern int hit_time;	/* Contador para saber cuando pueden daÒarnos de nuevo	*/ 
extern int character;
extern int freezed;
extern bool live_character[2];
extern int current_weapon;
extern int n_fired_arrows,n_fired_mines;
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

/* TILES: */ 
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
extern int password_pos;


void write_debug_report(char *file)
{
	int i,j;
	FILE *fp;

	fp=f1open(file,"w+",USERDATA);
	if (fp==0) return;

	fprintf(fp,"SCREEN_X: %i\nSCREEN_Y:%i\nCOLOUR_DEPTH: %i\n",SCREEN_X,SCREEN_Y,COLOUR_DEPTH);
	fprintf(fp,"fullscreen: %s\n",(fullscreen ? "true":"false"));
	fprintf(fp,"redrawing_period (mseconds): %i\n",REDRAWING_PERIOD);
	fprintf(fp,"frames_per_sec: %g\ninit_time: %i\n",frames_per_sec,init_time);

	fprintf(fp,"GRAPHIC PATHS:\n");
	for(i=0;i<n_g_paths;i++) {
		fprintf(fp,"%s",g_paths[i]);
		if (act_g_path==i) fprintf(fp," (SELECTED)");
		fprintf(fp,"\n");
	} /* for */ 
	fprintf(fp,"SOUND PATHS:\n");
	for(i=0;i<n_s_paths;i++) {
		fprintf(fp,"%s",s_paths[i]);
		if (act_s_path==i) fprintf(fp," (SELECTED)");
		fprintf(fp,"\n");
	} /* for */ 
	fprintf(fp,"DEFAULT GRAPHIC PATH: %s\n",default_g_path);
	fprintf(fp,"DEFAULT SOUND PATH: %s\n",default_s_path);

	fprintf(fp,"Graphic files:\n");
	fprintf(fp,"konami: %p\nmenu: %p\ntiles: %p\nenemy: %p\nenemy2: %p\ndemon: %p\nfinal: %p\n",
			konami_bmp,menu_bmp,tiles_bmp,enemy_bmp,enemy2_bmp,demon_bmp,final_bmp);

	fprintf(fp,"music_volume: %i\n",music_volume);
	fprintf(fp,"sfx_volume: %i\n",sfx_volume);
	fprintf(fp,"sound_enabled: %s\n",(sound_enabled ? "true":"false"));

	fprintf(fp,"Tiles:\n");
	fprintf(fp,"ntiles: %i\n",n_tiles);
	for(i=0;i<n_tiles;i++) {
		fprintf(fp,"Tile %i: %i (%i,%i-%i,%i [%i,%i])\n",i,tiles[i]->m_type,
				tiles[i]->m_sp->m_x,
				tiles[i]->m_sp->m_y,
				tiles[i]->m_sp->m_dx,
				tiles[i]->m_sp->m_dy,
				tiles[i]->m_sp->m_xoffs,
				tiles[i]->m_sp->m_yoffs);
	} /* for */ 

	fprintf(fp,"Buffers: %p %p\n",col_buffer,buffer_screen);

	fprintf(fp,"GAME:\n");
	fprintf(fp,"STATE: %i\nSUBSTATE: %i\ncycle: %i\n",STATE,SUBSTATE,cycle);

	fprintf(fp,"map: %i\nmap_x: %i\nmap_y: %i\n",map,map_x,map_y);
	fprintf(fp,"pers_x: %i\npers_y: %i\npers_pos: %i\n",pers_x,pers_y,pers_pos);
	fprintf(fp,"pers_right: %s\n",(pers_right ? "true":"false"));
	fprintf(fp,"pers_state: %i\npers_substate: %i\npers_walk_state: %i\n",pers_state,pers_substate,pers_walk_state);
	fprintf(fp,"jump_inertia: %i\n",jump_inertia);
	fprintf(fp,"sword: %s\nold_sword: %s\n",(sword ? "true":"false"),(old_sword ? "true":"false"));
	fprintf(fp,"sword_x: %i\nsword_y: %i\nsword_time: %i\n",sword_x,sword_y,sword_time);

	fprintf(fp,"in_ladder: %i\n",in_ladder);
	fprintf(fp,"previous_x_collision: %s\nprevious_y_collision: %s\n",(previous_x_collision ? "true":"false"),(previous_y_collision ? "true":"false"));
	fprintf(fp,"hit_time: %i\n",hit_time);
	fprintf(fp,"character: %i (%s,%s)\n",character,(live_character[0] ? "true":"false"),(live_character[1] ? "true":"false"));
	fprintf(fp,"freezed: %i\n",freezed);
	fprintf(fp,"current_weapon: %i\n",current_weapon);
	fprintf(fp,"n_fired_arrows: %i\n",n_fired_arrows);
	fprintf(fp,"n_fired_mines: %i\n",n_fired_mines);

	fprintf(fp,"mine_stepped: %s\n",(mine_stepped ? "true":"false"));
	fprintf(fp,"stone_hit_counter: %i\n",stone_hit_counter);
	fprintf(fp,"bible_counter: %i\nbible_subsounter: %i\n",bible_counter,bible_subcounter);

	fprintf(fp,"world3_room06_counter: %i\n",world3_room06_counter);
	fprintf(fp,"world10_door_x: %i\nworld10_door_y: %i\n",world10_door_x,world10_door_y);

	fprintf(fp,"in_door: %i\nto_enter_cut: %i\n",in_door,to_enter_cut);

	fprintf(fp,"jumping_from_elevator: %s\ncharacter_over_vertical_lever: %s\n",(jumping_from_elevator ? "true":"false"),(character_over_vertical_lever ? "true":"false"));
	
	fprintf(fp,"room_demonroom: %s\nold_room_demonroom: %s\n",(room_demonroom ? "true":"false"),(old_room_demonroom ? "true":"false"));
	fprintf(fp,"fighting_demon: %i\n",fighting_demon);
	fprintf(fp,"n_arrows: %i\nn_coins: %i\nn_keys: %i\n",n_arrows,n_coins,n_keys);
	fprintf(fp,"player_energy: %i - %i\n",player_energy[0],player_energy[1]);
	fprintf(fp,"player_experience: %i - %i\n",player_experience[0],player_experience[1]);
	fprintf(fp,"player_max: %i - %i\n",player_max[0],player_max[1]);
	fprintf(fp,"player_denergy: %i - %i\n",player_denergy[0],player_denergy[1]);
	fprintf(fp,"player_dexperience: %i - %i\n",player_dexperience[0],player_dexperience[1]);

	for(i=0;i<38;i++) {
		fprintf(fp,"ITEM %i: %s\n",i,(item[i] ? "true":"false"));
	} /* for */ 

	for(i=0;i<10;i++) {
		for(j=0;j<4;j++) {
			fprintf(fp,"WORLD %i ITEM %i: %s\n",i,j,(world_item[i][j] ? "true":"false"));
		} /* for */ 
	} /* for */ 

	fprintf(fp,"cross: %s\n",(cross ? "true":"false"));
	fprintf(fp,"world_key: %s\n",(world_key ? "true":"false"));
	fprintf(fp,"next_world: %i\n",next_world);
	for(i=0;i<10;i++) {
		fprintf(fp,"world %i door open: %s\n",i+1,(world_doors_open[i] ? "true":"false"));
	} /* for */ 

	fprintf(fp,"in_water: %s\nin_lava: %s\n",(in_water ? "true":"false"),(in_lava ? "true":"false"));
	fprintf(fp,"water_counter: %i\nlava_counter: %i\n",water_counter,lava_counter);

	fprintf(fp,"room_size: %i,%i\n",room_size_x,room_size_y);
	fprintf(fp,"ROOM:\n");
	if (room_tiles!=0) {
		for(i=0;i<room_size_y;i++) {
			for(j=0;j<room_size_y;j++) {
				fprintf(fp,"%.3i  ",room_tiles[i+j*room_size_x]);
			} /* for */ 
			fprintf(fp,"\n");
		} /* for */ 
	} /* if */ 

	fprintf(fp,"OBJECTS : %i\n",n_objects);
	for(i=0;i<n_objects;i++) {
		fprintf(fp,"--------------- OBJECT %i ---------------\n",i);
		fprintf(fp,"type: %i\n",object[i].type);
		fprintf(fp,"enemy: %i\n",object[i].enemy);
		fprintf(fp,"x: %i\n",object[i].x);
		fprintf(fp,"y: %i\n",object[i].y);
		fprintf(fp,"base_tile: %i\n",object[i].base_tile);
		fprintf(fp,"tile: %i\n",object[i].tile);
		fprintf(fp,"size: %i\n",object[i].size);
		fprintf(fp,"state: %i\n",object[i].state);
		fprintf(fp,"state2: %i\n",object[i].state2);
		fprintf(fp,"state3: %i\n",object[i].state3);
		fprintf(fp,"last_hit: %i\n",object[i].last_hit);
		fprintf(fp,"desired_floor: %i\n",object[i].desired_floor);
		fprintf(fp,"in_ladder: %i\n",object[i].in_ladder);
		fprintf(fp,"union_x: %i\n",object[i].union_x);
		fprintf(fp,"union_y: %i\n",object[i].union_y);
		fprintf(fp,"experience: %i\n",object[i].experience);
		fprintf(fp,"draw_x: %i\n",object[i].draw_x);
		fprintf(fp,"draw_y: %i\n",object[i].draw_y);
		fprintf(fp,"lightning: %i %i %i %i %i %i\n",object[i].lightning[0],object[i].lightning[1],object[i].lightning[2],object[i].lightning[3],object[i].lightning[4],object[i].lightning[5]);
	} /* for */ 
	
	fprintf(fp,"TILES:\n");
	fprintf(fp,"wall_tile: %i\n",wall_tile);
	fprintf(fp,"stone_tile: %i\n",stone_tile);
	fprintf(fp,"ladder_tile: %i\n",ladder_tile);
	fprintf(fp,"wdoor_tile: %i\n",wdoor_tile);
	fprintf(fp,"keydoor_tile: %i\n",keydoor_tile);
	fprintf(fp,"stats_tile: %i\n",stats_tile);

	fprintf(fp,"coin_tile: %i\n",coin_tile);
	fprintf(fp,"arrow_tile: %i\n",arrow_tile);
	fprintf(fp,"key_tile: %i\n",key_tile);
	fprintf(fp,"door_tile: %i\n",door_tile);
	fprintf(fp,"character_tile: %i %i %i\n",character_tile[0],character_tile[1],character_tile[2]);
	fprintf(fp,"character_tile_inv: %i %i %i\n",character_tile_inv[0],character_tile_inv[1],character_tile_inv[2]);
	fprintf(fp,"pause_tile: %i %i\n",pause_tile[0],pause_tile[1]);
	fprintf(fp,"colision_tile: %i\n",colision_tile);
	fprintf(fp,"sword_tile: %i %i\n",sword_tile[0],sword_tile[1]);
	fprintf(fp,"explosion_tile: %i\n",explosion_tile);

/*
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

*/

	for(i=0;i<MAX_LEVERS;i++) {
		fprintf(fp,"LEVER %i:\n",i);
		fprintf(fp,"lever_room: %i %i\n",lever_room_x[i],lever_room_y[i]);
		fprintf(fp,"lever_pos: %i %i\n",lever_x[i],lever_y[i]);
		fprintf(fp,"lever_dr: %i-%i %i-%i\n",lever_drx[i][0],lever_drx[i][1],lever_dry[i][0],lever_dry[i][1]);
		fprintf(fp,"lever_d: %i-%i %i-%i\n",lever_dx[i][0],lever_dx[i][1],lever_dy[i][0],lever_dy[i][1]);
		fprintf(fp,"lever_going_to: %i\n",lever_going_to[i]);
	} /* for */ 

/*
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
*/

	fprintf(fp,"logic_window 0: %i,%i - %i,%i\n",logic_x[0],logic_y[0],logic_dx[0],logic_dy[0]);
	fprintf(fp,"logic_window 1: %i,%i - %i,%i\n",logic_x[1],logic_y[1],logic_dx[1],logic_dy[1]);
	fprintf(fp,"logic_window 2: %i,%i - %i,%i\n",logic_x[2],logic_y[2],logic_dx[2],logic_dy[2]);
	fprintf(fp,"logic_window 3: %i,%i - %i,%i\n",logic_x[3],logic_y[3],logic_dx[3],logic_dy[3]);
	fprintf(fp,"physic_window 0: %i,%i - %i,%i\n",physic_x[0],physic_y[0],physic_dx[0],physic_dy[0]);
	fprintf(fp,"physic_window 1: %i,%i - %i,%i\n",physic_x[1],physic_y[1],physic_dx[1],physic_dy[1]);
	fprintf(fp,"physic_window 2: %i,%i - %i,%i\n",physic_x[2],physic_y[2],physic_dx[2],physic_dy[2]);
	fprintf(fp,"physic_window 3: %i,%i - %i,%i\n",physic_x[3],physic_y[3],physic_dx[3],physic_dy[3]);
	fprintf(fp,"zoom: %i\n",zoom);

	fprintf(fp,"keyboard:\n");
	fprintf(fp,"UP: %i\n",UP_KEY);
	fprintf(fp,"DOWN: %i\n",DOWN_KEY);
	fprintf(fp,"LEFT: %i\n",LEFT_KEY);
	fprintf(fp,"RIGHT: %i\n",RIGHT_KEY);
	fprintf(fp,"SWORD: %i\n",SWORD_KEY);
	fprintf(fp,"WEAPON: %i\n",WEAPON_KEY);
	fprintf(fp,"ITEM: %i\n",ITEM_KEY);
	fprintf(fp,"PAUSE: %i\n",PAUSE_KEY);

	fprintf(fp,"Last word: ");
	for(i=0;i<16;i++) fprintf(fp,"%i ",last_word[i]);
	fprintf(fp,"\n");

	fprintf(fp,"PASSWORD: %s\n",password);
	fprintf(fp,"password_pos: %i\n",password_pos);

	{
		unsigned char *keyboard;
		SDL_PumpEvents();
		keyboard = (unsigned char *)SDL_GetKeyState(NULL);

		fprintf(fp,"keyboard:\n");
		for(i=0;i<SDLK_LAST;i++) {
			fprintf(fp,"%.3i - %s\n",i,(keyboard[i] ? "true":"false"));
		} /* for */ 
	}

	fprintf(fp,"old_keyboard:\n");
	for(i=0;i<SDLK_LAST;i++) {
		fprintf(fp,"%.3i - %s\n",i,(old_keyboard[i] ? "true":"false"));
	} /* for */ 

	fclose(fp);

} /* write_debug_report */ 

