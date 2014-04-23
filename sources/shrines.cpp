#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "sound.h"
#include "MOGtypes.h"

// #include "drawing.h"
#include "bitmaps.h"
#include "sprites.h"
#include "tiles.h"
#include "mog.h"
#include "filehandling.h"

const int MAX_FIRED_ARROWS[2]={2,3};


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
extern int sword_x,sword_y;	/* Coordenadas de la espada.	*/ 
extern int sword_time;
extern unsigned char old_keyboard[SDLK_LAST];
extern int in_ladder;			/* En qué escalera está el personaje	*/ 
extern bool previous_x_collision;	/* Hubo una colisión en X en el frame anterior? */ 
extern bool previous_y_collision;	/* Hubo una colisión en Y en el frame anterior? */ 
extern int hit_time;	/* Contador para saber cuando pueden dañarnos de nuevo	*/ 
extern int character;
extern bool live_character[2],can_revive_character[2];
extern int current_weapon;
extern int n_fired_arrows;

extern int in_door;
extern int to_enter_cut;

extern bool shop_item[3];
extern int passage_state,currently_selecting;

extern bool room_slimes,room_watermonster,room_fallingstones,room_rollingstones;
extern bool room_blobs;
extern bool room_demonroom,old_room_demonroom;
extern int room_fairy,fairy_x,fairy_y;
extern int room_appearwall,room_appearwall_x,room_appearwall_y;
extern int fighting_demon,watermonster_state;
extern int world3_room06_counter;

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

extern int room_size_x,room_size_y;
extern int *room_tiles;

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
extern int whitebear_tile,feet_tile,redjumper_tile,lever_tile;
extern int items_tile,armas_tile,tomb_tile,wkeydoor_tile,wnokeydoor_tile;
extern int demonsign_tile,lupa_tile,demonsmoke_tile,demon_tile;

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
extern SOUNDT S_enterwater,S_select,S_firearrow;
extern SOUNDT S_nocoins;


/* Control de pantalla: */ 
extern int logic_dx[4],logic_dy[4],logic_x[4],logic_y[4];
extern int physic_dx[4],physic_dy[4],physic_x[4],physic_y[4];

extern int zoom;


/* Teclas: */ 
extern SDLKey UP_KEY,DOWN_KEY,LEFT_KEY,RIGHT_KEY;
extern SDLKey SWORD_KEY,WEAPON_KEY,ITEM_KEY,PAUSE_KEY;



void passage_mainloop(int map,int map_x,int map_y,unsigned char *screen,int dx,int dy)
{
	unsigned char *keyboard;

	SDL_PumpEvents();
	keyboard = (unsigned char *)SDL_GetKeyState(NULL);

	/* Dibujar el marco: */ 
	int i;

	for(i=0;i<32;i++) {
		tiles[143]->
			draw(GAME_VIEW_X+i*TILE_SIZE_X,GAME_VIEW_Y,
				 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
		tiles[143]->
			draw(GAME_VIEW_X+i*TILE_SIZE_X,GAME_VIEW_Y+18*TILE_SIZE_Y,
				 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	} /* for */ 
	for(i=0;i<19;i++) {
		tiles[143]->
			draw(GAME_VIEW_X,GAME_VIEW_Y+i*TILE_SIZE_Y,
				 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
		tiles[143]->
			draw(GAME_VIEW_X+TILE_SIZE_X,GAME_VIEW_Y+i*TILE_SIZE_Y,
				 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
		tiles[143]->
			draw(GAME_VIEW_X+30*TILE_SIZE_X,GAME_VIEW_Y+i*TILE_SIZE_Y,
				 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
		tiles[143]->
			draw(GAME_VIEW_X+31*TILE_SIZE_X,GAME_VIEW_Y+i*TILE_SIZE_Y,
				 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	} /* for */ 

	tiles[ladder_tile]->
		draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+19*TILE_SIZE_Y,
				TILE_SIZE_X*2,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	tiles[character_tile[character]+7]->
		draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+16*TILE_SIZE_Y,
				TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

	/* Dibujar los bocadillos: */ 
	tiles[151]->
		draw(GAME_VIEW_X+4*TILE_SIZE_X,GAME_VIEW_Y+4*TILE_SIZE_Y,
			 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	tiles[150]->
		draw(GAME_VIEW_X+27*TILE_SIZE_X,GAME_VIEW_Y+4*TILE_SIZE_Y,
			 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	tiles[149]->
		draw(GAME_VIEW_X+27*TILE_SIZE_X,GAME_VIEW_Y+10*TILE_SIZE_Y,
			 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	tiles[148]->
		draw(GAME_VIEW_X+4*TILE_SIZE_X,GAME_VIEW_Y+10*TILE_SIZE_Y,
			 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	for(i=5;i<27;i++) {
		tiles[146]->
			draw(GAME_VIEW_X+i*TILE_SIZE_X,GAME_VIEW_Y+4*TILE_SIZE_Y,
				 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
		tiles[146]->
			draw(GAME_VIEW_X+i*TILE_SIZE_X,GAME_VIEW_Y+10*TILE_SIZE_Y,
				 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	} /* for */ 
	for(i=5;i<10;i++) {
		tiles[147]->
			draw(GAME_VIEW_X+4*TILE_SIZE_X,GAME_VIEW_Y+i*TILE_SIZE_Y,
				 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
		tiles[147]->
			draw(GAME_VIEW_X+27*TILE_SIZE_X,GAME_VIEW_Y+i*TILE_SIZE_Y,
				 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	} /* for */ 
	tiles[145]->
		draw(GAME_VIEW_X+11*TILE_SIZE_X,GAME_VIEW_Y+4*TILE_SIZE_Y,
			 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);

	tiles[151]->
		draw(GAME_VIEW_X+4*TILE_SIZE_X,GAME_VIEW_Y+11*TILE_SIZE_Y,
			 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	tiles[150]->
		draw(GAME_VIEW_X+27*TILE_SIZE_X,GAME_VIEW_Y+11*TILE_SIZE_Y,
			 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	tiles[149]->
		draw(GAME_VIEW_X+27*TILE_SIZE_X,GAME_VIEW_Y+15*TILE_SIZE_Y,
			 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	tiles[148]->
		draw(GAME_VIEW_X+4*TILE_SIZE_X,GAME_VIEW_Y+15*TILE_SIZE_Y,
			 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	for(i=5;i<27;i++) {
		tiles[146]->
			draw(GAME_VIEW_X+i*TILE_SIZE_X,GAME_VIEW_Y+11*TILE_SIZE_Y,
				 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
		tiles[146]->
			draw(GAME_VIEW_X+i*TILE_SIZE_X,GAME_VIEW_Y+15*TILE_SIZE_Y,
				 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	} /* for */ 
	for(i=12;i<15;i++) {
		tiles[147]->
			draw(GAME_VIEW_X+4*TILE_SIZE_X,GAME_VIEW_Y+i*TILE_SIZE_Y,
				 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
		tiles[147]->
			draw(GAME_VIEW_X+27*TILE_SIZE_X,GAME_VIEW_Y+i*TILE_SIZE_Y,
				 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
	} /* for */ 
	tiles[144]->
		draw(GAME_VIEW_X+20*TILE_SIZE_X,GAME_VIEW_Y+15*TILE_SIZE_Y,
			 TILE_SIZE_X,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);

	if (map==0 && map_x==5 && map_y==9) {
		/* DEMETER */ 
		tile_print("h F5 TO F9",TILE_SIZE_X*6,TILE_SIZE_Y*21,screen,dx,dy);

		tile_print("DEMETER",TILE_SIZE_X*17,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[154]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		int slot=-1;
		/* Guardar el juego a disco: */ 

		if (keyboard[SDLK_F5] && !old_keyboard[SDLK_F5]) slot=1;
		if (keyboard[SDLK_F6] && !old_keyboard[SDLK_F6]) slot=2;
		if (keyboard[SDLK_F7] && !old_keyboard[SDLK_F7]) slot=3;
		if (keyboard[SDLK_F8] && !old_keyboard[SDLK_F8]) slot=4;
		if (keyboard[SDLK_F9] && !old_keyboard[SDLK_F9]) slot=5;

		if (slot!=-1) {
			char passwd[48];
			char tmp[80];
			FILE *fp;

			generatepassword(passwd);

			sprintf(tmp,"sgame%.2i.txt",slot);
			fp=f1open(tmp,"w",USERDATA);
			if (fp!=0) {
				int i;
				for(i=0;i<48;i++) {
					fprintf(fp,"%c",passwd[i]);
				} /* for */ 
				fclose(fp);
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 

		if (passage_state==0) {
			tile_print("YOU WANT THE SECRET",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("CODE TO WAKEN",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
			tile_print("MY MEMORY KEEP THE",TILE_SIZE_X*10,TILE_SIZE_Y*11,screen,dx,dy);
			tile_print("RECORDS ABOUT YOU?",TILE_SIZE_X*10,TILE_SIZE_Y*12,screen,dx,dy);
			tile_print("   YES         NO",TILE_SIZE_X*10,TILE_SIZE_Y*13,screen,dx,dy);
			if (currently_selecting==0) {
				tiles[152]->
					draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
						TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
			} /* if */ 
			if (currently_selecting==1) {
				tiles[152]->
					draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
						TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
			} /* if */ 
			if (keyboard[RIGHT_KEY] && !old_keyboard[RIGHT_KEY]) {
				if (currently_selecting<1) {
					currently_selecting++;
					Sound_play(S_select);
				} /* if */ 
			} /* if */ 
			if (keyboard[LEFT_KEY] && !old_keyboard[LEFT_KEY]) {
				if (currently_selecting>0) {
					currently_selecting--;
					Sound_play(S_select);
				} /* if */ 
			} /* if */ 
			if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
				if (currently_selecting==0) passage_state=2;
				if (currently_selecting==1) passage_state=1;
				keyboard[SWORD_KEY]=false;
			} /* if */ 
		} /* if */ 
		if (passage_state==1) {
			tile_print("RESCUE THE BABY AS ",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("SOON AS POSSIBLE",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} /* if */ 
		if (passage_state==2) {
			char passwd[48];
			char buf[20];
			int k;

			for(k=0;k<20;k++) buf[k]=' ';
			generatepassword(passwd);
			buf[0]=passwd[0]; buf[1]=passwd[1]; buf[2]=passwd[2]; buf[3]=passwd[3];
			buf[5]=passwd[4]; buf[6]=passwd[5]; buf[7]=passwd[6]; buf[8]=passwd[7];
			buf[10]=passwd[8]; buf[11]=passwd[9]; buf[12]=passwd[10]; buf[13]=passwd[11];
			buf[15]=passwd[12]; buf[16]=passwd[13]; buf[17]=passwd[14]; buf[18]=passwd[15];
			buf[19]=0;
			tile_print(buf,TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);

			buf[0]=passwd[16]; buf[1]=passwd[17]; buf[2]=passwd[18]; buf[3]=passwd[19];
			buf[5]=passwd[20]; buf[6]=passwd[21]; buf[7]=passwd[22]; buf[8]=passwd[23];
			buf[10]=passwd[24]; buf[11]=passwd[25]; buf[12]=passwd[26]; buf[13]=passwd[27];
			buf[15]=passwd[28]; buf[16]=passwd[29]; buf[17]=passwd[30]; buf[18]=passwd[31];
			buf[19]=0;
			tile_print(buf,TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);

			buf[0]=passwd[32]; buf[1]=passwd[33]; buf[2]=passwd[34]; buf[3]=passwd[35];
			buf[5]=passwd[36]; buf[6]=passwd[37]; buf[7]=passwd[38]; buf[8]=passwd[39];
			buf[10]=passwd[40]; buf[11]=passwd[41]; buf[12]=passwd[42]; buf[13]=passwd[43];
			buf[15]=passwd[44]; buf[16]=' '; buf[17]=' '; buf[18]=' ';
			buf[19]=0;
			tile_print(buf,TILE_SIZE_X*10,TILE_SIZE_Y*11,screen,dx,dy);

			tile_print("DID YOU GET THEM?",TILE_SIZE_X*10,TILE_SIZE_Y*12,screen,dx,dy);
			tile_print("   YES",TILE_SIZE_X*10,TILE_SIZE_Y*13,screen,dx,dy);
			tiles[152]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
			if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
				passage_state=3;
			} /* if */ 
		} /* if */ 
		if (passage_state==3) {
			tile_print("SEE YOU AGAIN",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
		} /* if */ 

	} /* if */ 

	if (map==0 && map_x==8 && map_y==15) {
		int price[3]={20,30,20};

		/* ARES */ 
		tile_print("ARES",TILE_SIZE_X*18,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[153]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		if (passage_state==0) {
			tile_print("YOU CAN BUY SOME OF",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("MY TREASURE",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} else {
			tile_print("COINS ARE NOT",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("ENOUGH.",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} /* if */ 
		if (shop_item[0]) {
			tiles[arrow_tile]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[1]) {
			tiles[items_tile+30]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[2]) {
			tiles[armas_tile+4]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+8*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		tile_print("   20    30    20",TILE_SIZE_X*10,TILE_SIZE_Y*13,screen,dx,dy);

		if (currently_selecting==0) {
			tiles[152]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==1) {
			tiles[152]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==2) {
			tiles[152]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 

		if (keyboard[RIGHT_KEY] && !old_keyboard[RIGHT_KEY]) {
			passage_state=0;
			if (currently_selecting<2) {
				currently_selecting++;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[LEFT_KEY] && !old_keyboard[LEFT_KEY]) {
			passage_state=0;
			if (currently_selecting>0) {
				currently_selecting--;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
			passage_state=0;
			if (n_coins>=price[currently_selecting] && shop_item[currently_selecting]) {
				/* Comprar objeto: */ 
				n_coins-=price[currently_selecting];
				shop_item[currently_selecting]=false;
				switch(currently_selecting) {
				case 0:n_arrows+=50;
					break;
				case 1:item[30]=true;
					   item[31]=false;
					   item[27]=false;
					break;
				case 2:item[36]=true;
					break;
				} /* if */ 
				Sound_play(S_item);
			} else {
				if (shop_item[currently_selecting] && n_coins<price[currently_selecting]) {
					Sound_play(S_nocoins);
					passage_state=1;
				} /* if */ 
			} /* if */ 
			
		} /* if */ 

	} /* if */ 

	if (map==0 && map_x==0 && map_y==13) {
		int price[3]={30,5,20};

		/* HEPHAESTUS */ 
		tile_print("HEPHAESTUS",TILE_SIZE_X*15,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[153]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		if (passage_state==0) {
			tile_print("YOU CAN BUY SOME OF",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("MY TREASURE",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} else {
			tile_print("COINS ARE NOT",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("ENOUGH.",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} /* if */ 
		if (shop_item[0]) {
			tiles[armas_tile+5]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+8*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[1]) {
			tiles[arrow_tile+1]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[2]) {
			tiles[arrow_tile]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		tile_print("   30    05    20",TILE_SIZE_X*10,TILE_SIZE_Y*13,screen,dx,dy);

		if (currently_selecting==0) {
			tiles[152]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==1) {
			tiles[152]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==2) {
			tiles[152]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 

		if (keyboard[RIGHT_KEY] && !old_keyboard[RIGHT_KEY]) {
			passage_state=0;
			if (currently_selecting<2) {
				currently_selecting++;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[LEFT_KEY] && !old_keyboard[LEFT_KEY]) {
			passage_state=0;
			if (currently_selecting>0) {
				currently_selecting--;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
			passage_state=0;
			if (n_coins>=price[currently_selecting] && shop_item[currently_selecting]) {
				/* Comprar objeto: */ 
				n_coins-=price[currently_selecting];
				shop_item[currently_selecting]=false;
				switch(currently_selecting) {
				case 0:item[37]=true;
					break;
				case 1:n_keys+=2;
					break;
				case 2:n_arrows+=50;
					break;
				} /* if */ 
				Sound_play(S_item);
			} else {
				if (shop_item[currently_selecting] && n_coins<price[currently_selecting]) {
					Sound_play(S_nocoins);
					passage_state=1;
				} /* if */ 
			} /* if */ 
		} /* if */ 

	} /* if */ 


	if (map==0 && map_x==11 && map_y==10) {
		/* DEATH */ 
		tile_print("DEATH",TILE_SIZE_X*18,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[167]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		if ((live_character[0] || !can_revive_character[0]) && 
			(live_character[1] || !can_revive_character[1]) && passage_state==0) {
			tile_print("IF YOUR LOVER IS",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("KILLED,COME BACK",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
			tile_print("HERE WITH abc100,",TILE_SIZE_X*10,TILE_SIZE_Y*11,screen,dx,dy);
			tile_print("dec100, AND fgc20.",TILE_SIZE_X*10,TILE_SIZE_Y*12,screen,dx,dy);
		} else {
			if (passage_state==0) {
				tile_print("GIVE ME abc100,",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
				tile_print("dec100 AND fgc20 IN",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
				tile_print("RETURN FOR REVIVING",TILE_SIZE_X*10,TILE_SIZE_Y*11,screen,dx,dy);
				tile_print("YOUR LOVER.",TILE_SIZE_X*10,TILE_SIZE_Y*12,screen,dx,dy);
				tile_print("   YES       NO",TILE_SIZE_X*10,TILE_SIZE_Y*13,screen,dx,dy);
				if (currently_selecting==0) {
					tiles[152]->
						draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
							TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				} /* if */ 
				if (currently_selecting==1) {
					tiles[152]->
						draw(GAME_VIEW_X+20 *TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
							TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
				} /* if */ 
				if (keyboard[RIGHT_KEY] && !old_keyboard[RIGHT_KEY]) {
					if (currently_selecting<1) {
						currently_selecting++;
						Sound_play(S_select);
					} /* if */ 
				} /* if */ 
				if (keyboard[LEFT_KEY] && !old_keyboard[LEFT_KEY]) {
					if (currently_selecting>0) {
						currently_selecting--;
						Sound_play(S_select);
					} /* if */ 
				} /* if */ 
				if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
					if (currently_selecting==0 && n_arrows>=100 && n_coins>=100 && n_keys>=20) {
						n_arrows-=100;
						n_coins-=100;
						n_keys-=20;
						can_revive_character[1-character]=false;
						live_character[0]=live_character[1]=true;
						player_energy[1-character]=player_max[1-character];
						passage_state=2;
					} /* if */ 
					if (currently_selecting==1) passage_state=1;
					keyboard[SWORD_KEY]=false;
				} /* if */ 
			} /* if */ 

			if (passage_state==1) {
				tile_print("THEN,YOU MUST FIGHT",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
				tile_print("ALL BY YOURSELF",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
				tile_print("GO,RIGHT NOW.",TILE_SIZE_X*10,TILE_SIZE_Y*11,screen,dx,dy);
			} /* if */ 

			if (passage_state==2) {
				tile_print("YOUR LOVER HAS",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
				tile_print("REVIVED, GO AND GET",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
				tile_print("BACK THE LITTLE BABY",TILE_SIZE_X*10,TILE_SIZE_Y*11,screen,dx,dy);
				tile_print("TOGETHER. GO AHEAD!",TILE_SIZE_X*10,TILE_SIZE_Y*12,screen,dx,dy);
			} /* if */ 
		} /* if */ 	
	} /* if */ 


	if (map==0 && map_x==9 && map_y==7) {
		int price[3]={20,25,5};

		/* HEPHAESTUS */ 
		tile_print("HEPHAESTUS",TILE_SIZE_X*15,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[153]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		if (passage_state==0) {
			tile_print("YOU CAN BUY SOME OF",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("MY TREASURE",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} else {
			tile_print("COINS ARE NOT",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("ENOUGH.",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} /* if */ 
		if (shop_item[0]) {
			tiles[arrow_tile]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[1]) {
			tiles[items_tile+22]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[2]) {
			tiles[key_tile]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		tile_print("   20    25    05",TILE_SIZE_X*10,TILE_SIZE_Y*13,screen,dx,dy);

		if (currently_selecting==0) {
			tiles[152]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==1) {
			tiles[152]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==2) {
			tiles[152]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 

		if (keyboard[RIGHT_KEY] && !old_keyboard[RIGHT_KEY]) {
			passage_state=0;
			if (currently_selecting<2) {
				currently_selecting++;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[LEFT_KEY] && !old_keyboard[LEFT_KEY]) {
			passage_state=0;
			if (currently_selecting>0) {
				currently_selecting--;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
			passage_state=0;
			if (n_coins>=price[currently_selecting] && shop_item[currently_selecting]) {
				/* Comprar objeto: */ 
				n_coins-=price[currently_selecting];
				shop_item[currently_selecting]=false;
				switch(currently_selecting) {
				case 0:n_arrows+=50;
					break;
				case 1:item[22]=true;
					break;
				case 2:n_keys+=2;
					break;
				} /* if */ 
				Sound_play(S_item);
			} else {
				if (shop_item[currently_selecting] && n_coins<price[currently_selecting]) {
					Sound_play(S_nocoins);
					passage_state=1;
				} /* if */ 
			} /* if */ 
			 
		} /* if */ 

	} /* if */ 


	if (map==0 && map_x==12 && map_y==2) {
		int price[3]={20,50,5};

		/* ARES */ 
		tile_print("ARES",TILE_SIZE_X*18,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[153]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		if (passage_state==0) {
			tile_print("YOU CAN BUY SOME OF",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("MY TREASURE",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} else {
			tile_print("COINS ARE NOT",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("ENOUGH.",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} /* if */ 
		if (shop_item[0]) {
			tiles[arrow_tile]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[1]) {
			tiles[items_tile+12]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[2]) {
			tiles[key_tile]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		tile_print("   20    50    05",TILE_SIZE_X*10,TILE_SIZE_Y*13,screen,dx,dy);

		if (currently_selecting==0) {
			tiles[152]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==1) {
			tiles[152]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==2) {
			tiles[152]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 

		if (keyboard[RIGHT_KEY] && !old_keyboard[RIGHT_KEY]) {
			passage_state=0;
			if (currently_selecting<2) {
				currently_selecting++;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[LEFT_KEY] && !old_keyboard[LEFT_KEY]) {
			passage_state=0;
			if (currently_selecting>0) {
				currently_selecting--;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
			passage_state=0;
			if (n_coins>=price[currently_selecting] && shop_item[currently_selecting]) {
				/* Comprar objeto: */ 
				n_coins-=price[currently_selecting];
				shop_item[currently_selecting]=false;
				switch(currently_selecting) {
				case 0:n_arrows+=50;
					break;
				case 1:item[12]=true;
					break;
				case 2:n_keys+=2;
					break;
				} /* if */ 
				Sound_play(S_item);
			} else {
				if (shop_item[currently_selecting] && n_coins<price[currently_selecting]) {
					Sound_play(S_nocoins);
					passage_state=1;
				} /* if */ 
			} /* if */ 
			
		} /* if */ 

	} /* if */ 


	if (map==0 && map_x==2 && map_y==2) {
		int price[3]={30,20,5};

		/* HEPHAESTUS */ 
		tile_print("HEPHAESTUS",TILE_SIZE_X*15,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[153]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		if (passage_state==0) {
			tile_print("YOU CAN BUY SOME OF",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("MY TREASURE",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} else {
			tile_print("COINS ARE NOT",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("ENOUGH.",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} /* if */ 
		if (shop_item[0]) {
			tiles[items_tile+20]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[1]) {
			tiles[arrow_tile]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[2]) {
			tiles[key_tile]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		tile_print("   30    20    05",TILE_SIZE_X*10,TILE_SIZE_Y*13,screen,dx,dy);

		if (currently_selecting==0) {
			tiles[152]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==1) {
			tiles[152]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==2) {
			tiles[152]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 

 
		if (keyboard[RIGHT_KEY] && !old_keyboard[RIGHT_KEY]) {
			passage_state=0;
			if (currently_selecting<2) {
				currently_selecting++;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[LEFT_KEY] && !old_keyboard[LEFT_KEY]) {
			passage_state=0;
			if (currently_selecting>0) {
				currently_selecting--;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
			passage_state=0;
			if (n_coins>=price[currently_selecting] && shop_item[currently_selecting]) {
				/* Comprar objeto: */ 
				n_coins-=price[currently_selecting];
				shop_item[currently_selecting]=false;
				switch(currently_selecting) {
				case 0:item[20]=true;
					break;
				case 1:n_arrows+=50;
					break;
				case 2:n_keys+=2;
					break;
				} /* if */ 
				Sound_play(S_item);
			} else {
				if (shop_item[currently_selecting] && n_coins<price[currently_selecting]) {
					Sound_play(S_nocoins);
					passage_state=1;
				} /* if */ 
			} /* if */ 
		
		} /* if */ 
	} /* if */ 


	if (map==0 && map_x==2 && map_y==5) {
		int price[3]={20,25,5};

		/* DETEMETER */ 
		tile_print("DEMETER",TILE_SIZE_X*17,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[154]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		if (passage_state==0) {
			tile_print("YOU CAN BUY SOME OF",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("MY TREASURE",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} else {
			tile_print("COINS ARE NOT",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("ENOUGH.",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} /* if */ 
		if (shop_item[0]) {
			tiles[arrow_tile]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[1]) {
			tiles[items_tile]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[2]) {
			tiles[key_tile]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		tile_print("   20    25    05",TILE_SIZE_X*10,TILE_SIZE_Y*13,screen,dx,dy);

		if (currently_selecting==0) {
			tiles[152]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==1) {
			tiles[152]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==2) {
			tiles[152]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 

		if (keyboard[RIGHT_KEY] && !old_keyboard[RIGHT_KEY]) {
			passage_state=0;
			if (currently_selecting<2) {
				currently_selecting++;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[LEFT_KEY] && !old_keyboard[LEFT_KEY]) {
			passage_state=0;
			if (currently_selecting>0) {
				currently_selecting--;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
			passage_state=0;
			if (n_coins>=price[currently_selecting] && shop_item[currently_selecting]) {
				/* Comprar objeto: */ 
				n_coins-=price[currently_selecting];
				shop_item[currently_selecting]=false;
				switch(currently_selecting) {
				case 0:n_arrows+=50;
					break;
				case 1:item[0]=true;
					break;
				case 2:n_keys+=2;
					break;
				} /* if */ 
				Sound_play(S_item);
			} else {
				if (shop_item[currently_selecting] && n_coins<price[currently_selecting]) {
					Sound_play(S_nocoins);
					passage_state=1;
				} /* if */ 
			} /* if */  

		} /* if */ 

	} /* if */ 


	if (map==0 && map_x==12 && map_y==8) {
		int price[3]={20,40,5};

		/* ARES */ 
		tile_print("ARES",TILE_SIZE_X*18,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[153]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		if (passage_state==0) {
			tile_print("YOU CAN BUY SOME OF",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("MY TREASURE",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} else {
			tile_print("COINS ARE NOT",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("ENOUGH.",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} /* if */ 
		if (shop_item[0]) {
			tiles[arrow_tile]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[1]) {
			tiles[items_tile+31]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[2]) {
			tiles[key_tile]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		tile_print("   20    40    05",TILE_SIZE_X*10,TILE_SIZE_Y*13,screen,dx,dy);

		if (currently_selecting==0) {
			tiles[152]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==1) {
			tiles[152]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==2) {
			tiles[152]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 


		if (keyboard[RIGHT_KEY] && !old_keyboard[RIGHT_KEY]) {
			passage_state=0;
			if (currently_selecting<2) {
				currently_selecting++;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[LEFT_KEY] && !old_keyboard[LEFT_KEY]) {
			passage_state=0;
			if (currently_selecting>0) {
				currently_selecting--;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
			passage_state=0;
			if (n_coins>=price[currently_selecting] && shop_item[currently_selecting]) {
				/* Comprar objeto: */ 
				n_coins-=price[currently_selecting];
				shop_item[currently_selecting]=false;
				switch(currently_selecting) {
				case 0:n_arrows+=50;
					break;
				case 1: item[27]=false;
						item[30]=false;
						item[31]=true;
					break;
				case 2:n_keys+=2;
					break;
				} /* if */ 
				Sound_play(S_item);
			} else {
				if (shop_item[currently_selecting] && n_coins<price[currently_selecting]) {
					Sound_play(S_nocoins);
					passage_state=1;
				} /* if */ 
			} /* if */ 

		} /* if */ 

	} /* if */ 


	if (map==0 && map_x==8 && map_y==1) {
		int price[3]={20,50,5};

		/* ARES */ 
		tile_print("ARES",TILE_SIZE_X*18,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[153]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		if (passage_state==0) {
			tile_print("YOU CAN BUY SOME OF",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("MY TREASURE",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} else {
			tile_print("COINS ARE NOT",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("ENOUGH.",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} /* if */ 
		if (shop_item[0]) {
			tiles[arrow_tile]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[1]) {
			tiles[items_tile+27]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (shop_item[2]) {
			tiles[key_tile]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		tile_print("   20    50    05",TILE_SIZE_X*10,TILE_SIZE_Y*13,screen,dx,dy);

		if (currently_selecting==0) {
			tiles[152]->
				draw(GAME_VIEW_X+9*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==1) {
			tiles[152]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 
		if (currently_selecting==2) {
			tiles[152]->
				draw(GAME_VIEW_X+21*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 


		if (keyboard[RIGHT_KEY] && !old_keyboard[RIGHT_KEY]) {
			passage_state=0;
			if (currently_selecting<2) {
				currently_selecting++;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[LEFT_KEY] && !old_keyboard[LEFT_KEY]) {
			passage_state=0;
			if (currently_selecting>0) {
				currently_selecting--;
				Sound_play(S_select);
			} /* if */ 
		} /* if */ 
		if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
			passage_state=0;
			if (n_coins>=price[currently_selecting] && shop_item[currently_selecting]) {
				/* Comprar objeto: */ 
				n_coins-=price[currently_selecting];
				shop_item[currently_selecting]=false;
				switch(currently_selecting) {
				case 0:n_arrows+=50;
					break;
				case 1: item[27]=true;
						item[30]=false;
						item[31]=false;
					break;
				case 2:n_keys+=2;
					break;
				} /* if */ 
				Sound_play(S_item);
			} else {
				if (shop_item[currently_selecting] && n_coins<price[currently_selecting]) {
					Sound_play(S_nocoins);
					passage_state=1;
				} /* if */ 
			} /* if */ 

		} /* if */ 

	} /* if */ 


	if (map==0 && map_x==7 && map_y==7) {
		/* ASCLEPIUS */ 
		tile_print("ASCLEPIUS",TILE_SIZE_X*16,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[178]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		tile_print("SEE A FAIRY TO",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
		tile_print("RESTORE YOUR",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		tile_print("VITALITY.",TILE_SIZE_X*10,TILE_SIZE_Y*11,screen,dx,dy);
	} /* if */ 

	if (map==1) {
		/* DEMETER en el mundo 1 */ 
		tile_print("DEMETER",TILE_SIZE_X*17,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[154]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		tile_print("TAKE A MINE WITH YOU",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
		tile_print("WHEN YOU GO TO",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		tile_print("WORLD 4",TILE_SIZE_X*10,TILE_SIZE_Y*11,screen,dx,dy);
	} /* if */ 

	if (map==2) {
		/* ATHENA en el mundo 2 */ 
		tile_print("ATHENA",TILE_SIZE_X*17,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[161]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		tile_print("HIT THE WALL WITH",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
		tile_print("YOUR SWORD IF YOU",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		tile_print("WANT TO GO TO THE",TILE_SIZE_X*10,TILE_SIZE_Y*11,screen,dx,dy);
		tile_print("OPPOSITE SHORE.",TILE_SIZE_X*10,TILE_SIZE_Y*12,screen,dx,dy);
	} /* if */ 

	if (map==3) {
		/* ATHENA en el mundo 3 */ 
		tile_print("ATHENA",TILE_SIZE_X*17,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[161]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		tile_print("APHRODITE.DO NOT BE",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
		tile_print("AFRAID OF WATER.",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
	} /* if */ 

	if (map==4) {
		/* DEMETER en el mundo 4 */ 
		tile_print("DEMETER",TILE_SIZE_X*17,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[154]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		tile_print("THE PURE WATER IS",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
		tile_print("INSIDE THE WALL.",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
	} /* if */ 

	if (map==5) {
		int price[1]={30};

		/* DETEMETER */ 
		tile_print("DEMETER",TILE_SIZE_X*17,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[154]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		if (passage_state==0) {
			tile_print("YOU CAN BUY SOME OF",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("MY TREASURE",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} else {
			tile_print("COINS ARE NOT",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("ENOUGH.",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} /* if */ 
		if (shop_item[0]) {
			tiles[items_tile+3]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		tile_print("         30",TILE_SIZE_X*10,TILE_SIZE_Y*13,screen,dx,dy);

		if (currently_selecting==0) {
			tiles[152]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 

		if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
			passage_state=0;
			if (n_coins>=price[currently_selecting] && shop_item[currently_selecting]) {
				/* Comprar objeto: */ 
				n_coins-=price[currently_selecting];
				shop_item[currently_selecting]=false;
				item[3]=true;
				Sound_play(S_item);
			} else {
				if (shop_item[currently_selecting] && n_coins<price[currently_selecting]) {
					Sound_play(S_nocoins);
					passage_state=1;
				} /* if */ 
			} /* if */  

		} /* if */ 

	} /* if */ 

	if (map==6) {
		/* ATHENA en el mundo 6 */ 
		tile_print("ATHENA",TILE_SIZE_X*17,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[161]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		tile_print("HIT THE WALL WHEN",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
		tile_print("YOU ARE HALFWAY OF",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		tile_print("THE STAIRS.",TILE_SIZE_X*10,TILE_SIZE_Y*11,screen,dx,dy);
	} /* if */ 

	if (map==7) {
		/* DEMETER en el mundo 7 */ 
		tile_print("DEMETER",TILE_SIZE_X*17,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[154]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		tile_print("FIND THE SPELL",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
		tile_print("WRITTEN ON A STONE",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		tile_print("MONUMENT UNDER THE",TILE_SIZE_X*10,TILE_SIZE_Y*11,screen,dx,dy);
		tile_print("WATER.",TILE_SIZE_X*10,TILE_SIZE_Y*12,screen,dx,dy);
	} /* if */ 

	if (map==8) {
		int price[1]={30};

		/* DETEMETER */ 
		tile_print("DEMETER",TILE_SIZE_X*17,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[154]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		if (passage_state==0) {
			tile_print("YOU CAN BUY SOME OF",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("MY TREASURE",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} else {
			tile_print("COINS ARE NOT",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
			tile_print("ENOUGH.",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		} /* if */ 
		if (shop_item[0]) {
			tiles[items_tile+11]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+7*TILE_SIZE_Y,
					 screen,col_buffer,dx,dy,dx);
		} /* if */ 
		tile_print("         30",TILE_SIZE_X*10,TILE_SIZE_Y*13,screen,dx,dy);

		if (currently_selecting==0) {
			tiles[152]->
				draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+12*TILE_SIZE_Y,
					TILE_SIZE_X,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);
		} /* if */ 

		if (keyboard[SWORD_KEY] && !old_keyboard[SWORD_KEY]) {
			passage_state=0;
			if (n_coins>=price[currently_selecting] && shop_item[currently_selecting]) {
				/* Comprar objeto: */ 
				n_coins-=price[currently_selecting];
				shop_item[currently_selecting]=false;
				item[11]=true;
				Sound_play(S_item);
			} else {
				if (shop_item[currently_selecting] && n_coins<price[currently_selecting]) {
					Sound_play(S_nocoins);
					passage_state=1;
				} /* if */ 
			} /* if */  

		} /* if */ 

	} /* if */ 


	if (map==9) {
		/* DEMETER en el mundo 9 */ 
		tile_print("DEMETER",TILE_SIZE_X*17,TILE_SIZE_Y*5,screen,dx,dy);
		tiles[154]->
			draw(GAME_VIEW_X+15*TILE_SIZE_X,GAME_VIEW_Y+2*TILE_SIZE_Y,
					TILE_SIZE_X*2,TILE_SIZE_Y*2,screen,col_buffer,dx,dy,dx);

		tile_print("A GATE WHICH LEADS",TILE_SIZE_X*10,TILE_SIZE_Y*9,screen,dx,dy);
		tile_print("TO GALIOUS WILL",TILE_SIZE_X*10,TILE_SIZE_Y*10,screen,dx,dy);
		tile_print("APPEAR SOMEWHERE IN",TILE_SIZE_X*10,TILE_SIZE_Y*11,screen,dx,dy);
		tile_print("THE CASTLE.",TILE_SIZE_X*10,TILE_SIZE_Y*12,screen,dx,dy);
	} /* if */ 

} /* passage_mainloop */ 
						
