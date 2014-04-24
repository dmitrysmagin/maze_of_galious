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
#include "object.h"

/* Paths: */ 

extern char **g_paths;
extern int n_g_paths,act_g_path;
extern char *g_path;
extern char **s_paths;
extern int n_s_paths,act_s_path;
extern char *s_path;

/* Gráficos: */ 
Bitmap *demon_bmp=0,*back_bmp=0;
extern Bitmap *konami_bmp,*menu_bmp,*tiles_bmp,*tiles2_bmp,*enemy_bmp;
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
extern bool live_character[2];
extern int current_weapon;
extern int n_fired_arrows;

extern int room_state;
extern bool room_slimes,room_watermonster,room_demonroom;
extern int fighting_demon;
extern int watermonster_state;

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

/* Objetos: (escaleras, piedras, etc...) */ 
extern MOGObject object[MAX_OBJECTS];
extern int n_objects;

extern int wall_tile;
extern int stone_tile,ladder_tile,wdoor_tile,keydoor_tile,stats_tile;
extern int coin_tile,arrow_tile,key_tile,door_tile;
extern int character_tile[2],character_tile_inv[2];
extern int colision_tile;
extern int sword_tile[2],explosion_tile;
extern int worm_tile,bat_tile,skeleton_tile,smoke_tile,slime_tile,bouncingball_tile;
extern int watermonster_tile,jumpingbush_tile,bluespider_tile,whitefireball_tile;
extern int items_tile,armas_tile,tomb_tile,wkeydoor_tile,wnokeydoor_tile;
extern int demonsign_tile,demonsmoke_tile,lcloud_tile;

extern int worm_state_tile[28];
extern int worm_state_tile_i[28];
extern int worm_state_y[28];


/* Estado de las habitaciones por las que ya has pasado: */ 
#define HP_OBJS_MAX	512

extern int HP_room_x[HP_OBJS_MAX],HP_room_y[HP_OBJS_MAX];
extern int HP_obj[HP_OBJS_MAX];
extern int HP_obj_type[HP_OBJS_MAX];



/* Sonidos: */ 
extern SOUNDT S_demon2ball,S_firebreath,S_demon4jump,S_demon1jump,S_demon1bones;
extern SOUNDT S_demon7bullet,S_demon8bullet;


/* Variables de demon: */ 
extern int demon_tile;
int n_smoke=0,smoke_x[8],smoke_y[8],smoke_t[8];
int demonback_state=0;
extern int demon_obj,demon_obj2;
int demon_max_life=0;
int demon_music=0;


extern int  add_object(int type,int x,int y,int size,int enemy,int state);


void redo_demonintro(int state,unsigned char *screen,int dx,int dy)
{
	int i;

	if (state<193) {
		for(i=0;i<state;i++) demon_intro(i,screen,dx,dy);
	} else {
		for(i=0;i<160;i++) demon_intro(i,screen,dx,dy);
	} /* if */ 
} /* redo_demonintro */ 


void demon_intro(int state,unsigned char *screen,int dx,int dy)
{
	int i,j,offs;


	if (state==1) {
		char tmp[80];
		if (back_bmp!=0) delete back_bmp;
		sprintf(tmp,"back%i.pcx",map);
		back_bmp=load_bitmap(tmp);
		demonback_state=0;
		demon_music=0;
	} /* if */ 

	/* Efecto de entrada de monstruo: */ 
	if (state<32 && screen!=0) {
		if ((state&0x02)!=0) {
			for(i=GAME_VIEW_Y;i<dy;i++) {
				for(j=0,offs=i*dx;j<dx;j++,offs++) {
					if (((j+i)&0x01)==0) screen[offs]=255;
				} /* for */ 
			} /* for */ 
		} /* if */ 
	} /* if */ 

	/* Hacer desaparecer los tiles: */ 
	if (state>=32 && state<64) {
		int tx,ty;
		int step;
		int x[16]={3,2,1,0,0,0,0,1,2,3,3,3,2,1,1,2};
		int y[16]={3,3,3,3,2,1,0,0,0,0,1,2,2,2,1,1};
		step=(state-32)/2;

		for(tx=0;tx<4;tx++) {
			for(ty=0;ty<2;ty++) {
				for(i=0;i<TILE_UNIT;i++) {
					for(j=0;j<TILE_UNIT;j++) {
						tiles_bmp->get_image()[TILE_SIZE_X*(34+tx)+x[step]*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						tiles_bmp->get_image()[TILE_SIZE_X*(34+tx)+(x[step]+4)*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						tiles_bmp->get_image()[TILE_SIZE_X*(34+tx)+x[step]*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						tiles_bmp->get_image()[TILE_SIZE_X*(34+tx)+(x[step]+4)*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
					} /* for */ 
				} /* for */ 
			} /* for */ 
		} /* for */ 
		for(tx=0;tx<2;tx++) {
			for(ty=0;ty<3;ty++) {
				for(i=0;i<TILE_UNIT;i++) {
					for(j=0;j<TILE_UNIT;j++) {
						tiles_bmp->get_image()[TILE_SIZE_X*(38+tx)+x[step]*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						tiles_bmp->get_image()[TILE_SIZE_X*(38+tx)+(x[step]+4)*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						tiles_bmp->get_image()[TILE_SIZE_X*(38+tx)+x[step]*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						tiles_bmp->get_image()[TILE_SIZE_X*(38+tx)+(x[step]+4)*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
					} /* for */ 
				} /* for */ 
			} /* for */ 
		} /* for */ 
	} /* if */ 

	/* Hacer desaparecer el símbolo del monstruo: */ 
	if (state>=64 && state<96) {
		int tx,ty;
		int step;
		int x[16]={3,2,1,0,0,0,0,1,2,3,3,3,2,1,1,2};
		int y[16]={3,3,3,3,2,1,0,0,0,0,1,2,2,2,1,1};
		step=(state-64)/2;

		for(tx=0;tx<8;tx++) {
			for(ty=0;ty<8;ty++) {
				for(i=0;i<TILE_UNIT;i++) {
					for(j=0;j<TILE_UNIT;j++) {
						tiles2_bmp->get_image()[TILE_SIZE_X*tx+x[step]*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						tiles2_bmp->get_image()[TILE_SIZE_X*tx+(x[step]+4)*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						tiles2_bmp->get_image()[TILE_SIZE_X*tx+x[step]*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						tiles2_bmp->get_image()[TILE_SIZE_X*tx+(x[step]+4)*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
					} /* for */ 
				} /* for */ 
			} /* for */ 
		} /* for */ 
		n_smoke=0;
	} /* if */ 

	/* Hacer desaparecer las parededs laterales en el mundo 3: */ 
	if (state==96 && map==3) {
		int tx,ty;
		int x[4]={0,1,30,31};

		for(ty=0;ty<20;ty++) {
			if ((ty%4)!=3) {
				for(tx=0;tx<4;tx++) room_tiles[x[tx]+ty*room_size_x]=127;
			} /* if */ 
		} /* for */ 
		room_tiles[2+19*room_size_x]=125;
		room_tiles[3+19*room_size_x]=125;
		object[0].size=7;
	} /* if */ 

	/* Humo de antes del monsruo: */ 
	if (state>96 && state<160 && screen!=0) {
		if ((state&0x04)==0) {
			smoke_t[n_smoke]=1;
			smoke_x[n_smoke]=(4+(rand()%23))*TILE_SIZE_X;
			smoke_y[n_smoke]=(4+(rand()%8))*TILE_SIZE_Y;
			n_smoke++;
			if (n_smoke==8) n_smoke=0;
		} /* if */ 

		for(i=0;i<8;i++) {
			if (smoke_t[i]>0) {
				tiles[demonsmoke_tile+((smoke_t[i]>>3)&0x01)]->
					draw(GAME_VIEW_X+smoke_x[i],GAME_VIEW_Y+smoke_y[i],
						 TILE_SIZE_X*4,TILE_SIZE_Y*4,screen,col_buffer,dx,dy,dx);
				smoke_t[i]++;
				if (smoke_t[i]>=32) smoke_t[i]=0;
			} /* if */ 
		} /* for */ 
	} /* if */ 

	/* Hacer desaparecer al monstruo: */ 
	if (state>=162 && state<194 && demon_bmp!=0) {
		int tx,ty;
		int step;
		int x[16]={3,2,1,0,0,0,0,1,2,3,3,3,2,1,1,2};
		int y[16]={3,3,3,3,2,1,0,0,0,0,1,2,2,2,1,1};
		step=(state-162)/2;

		if (map==1) {
			object[demon_obj].state=demon_tile+1;
			for(tx=10;tx<20;tx++) {
				for(ty=0;ty<7;ty++) {
					for(i=0;i<TILE_UNIT;i++) {
						for(j=0;j<TILE_UNIT;j++) {
							demon_bmp->get_image()[TILE_SIZE_X*tx+x[step]*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+(x[step]+4)*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+x[step]*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+(x[step]+4)*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						} /* for */ 
					} /* for */ 
				} /* for */ 
			} /* for */ 
		} /* if */ 
		if (map==2) {
			for(tx=0;tx<24;tx++) {
				for(ty=0;ty<6;ty++) {
					for(i=0;i<TILE_UNIT;i++) {
						for(j=0;j<TILE_UNIT;j++) {
							demon_bmp->get_image()[TILE_SIZE_X*tx+x[step]*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+(x[step]+4)*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+x[step]*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+(x[step]+4)*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						} /* for */ 
					} /* for */ 
				} /* for */ 
			} /* for */ 
		} /* if */ 
		if (map==3) {
			for(tx=0;tx<32;tx++) {
				for(ty=0;ty<9;ty++) {
					for(i=0;i<TILE_UNIT;i++) {
						for(j=0;j<TILE_UNIT;j++) {
							demon_bmp->get_image()[TILE_SIZE_X*tx+x[step]*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+(x[step]+4)*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+x[step]*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+(x[step]+4)*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						} /* for */ 
					} /* for */ 
				} /* for */ 
			} /* for */ 
		} /* if */ 
		if (map==4) {
			for(tx=0;tx<40;tx++) {
				for(ty=0;ty<10;ty++) {
					for(i=0;i<TILE_UNIT;i++) {
						for(j=0;j<TILE_UNIT;j++) {
							demon_bmp->get_image()[TILE_SIZE_X*tx+x[step]*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+(x[step]+4)*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+x[step]*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+(x[step]+4)*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						} /* for */ 
					} /* for */ 
				} /* for */ 
			} /* for */ 
		} /* if */ 
		if (map==5 || map==7 || map==9 || map==10) {
			for(tx=0;tx<40;tx++) {
				for(ty=0;ty<10;ty++) {
					for(i=0;i<TILE_UNIT;i++) {
						for(j=0;j<TILE_UNIT;j++) {
							demon_bmp->get_image()[TILE_SIZE_X*tx+x[step]*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+(x[step]+4)*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+x[step]*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+(x[step]+4)*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						} /* for */ 
					} /* for */ 
				} /* for */ 
			} /* for */ 
		} /* if */ 
		if (map==6 || map==8) {
			for(tx=0;tx<20;tx++) {
				for(ty=0;ty<20;ty++) {
					for(i=0;i<TILE_UNIT;i++) {
						for(j=0;j<TILE_UNIT;j++) {
							demon_bmp->get_image()[TILE_SIZE_X*tx+x[step]*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+(x[step]+4)*TILE_UNIT+(y[step]*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+x[step]*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
							demon_bmp->get_image()[TILE_SIZE_X*tx+(x[step]+4)*TILE_UNIT+((y[step]+4)*TILE_UNIT+i+TILE_SIZE_Y*ty)*dx+j]=0;
						} /* for */ 
					} /* for */ 
				} /* for */ 
			} /* for */ 
		} /* if */ 
	} /* if */ 
} /* demon_intro */ 


void draw_demon_background(BYTE *screen,int dx,int dy,int pitch)
{
	if (back_bmp!=0	&& fighting_demon>96) {
		int mask[16]={ 0, 8, 2, 9,
					  12, 4,14, 6,
					   3,10, 1,11,
					  15, 7,13, 5};

		if (demonback_state<16) {
			int tx,ty;
			int i;
			BYTE tmp;

			for(ty=0;ty<dy-GAME_VIEW_Y;ty++) {
				for(tx=0;tx<dx;tx++) {
					i=(ty%4)*4+(tx%4);
					if (mask[i]<=demonback_state) {
						tmp=back_bmp->get_image()[tx+ty*pitch];
						if (tmp!=0) screen[tx+(ty+GAME_VIEW_Y)*pitch]=tmp;
					} /* if */ 
				} /* for */ 
			} /* for */ 
			demonback_state++;
		} else {
			back_bmp->draw(0,GAME_VIEW_Y,dx,dy,screen,dx,dy,pitch);
		} /* if */ 
	} /* if */ 
} /* draw_demon_background */ 



/*									 */ 
/* ----------- DEMON 1 ------------- */ 
/*                                   */ 


void load_demon1(void)
{
	int i;

	if (demon_bmp!=0) delete demon_bmp;
	if (back_bmp!=0) delete back_bmp;
	demon_bmp=load_bitmap("demon1.pcx");
	back_bmp=load_bitmap("back1.pcx");

	for(i=0;i<20;i++) {
		if (tiles[demon_tile+i]!=0) delete tiles[demon_tile+i];
	} /* for */ 
	tiles[demon_tile]=new CTile(demon_bmp,0,0,TILE_SIZE_X*10,TILE_SIZE_Y*7,T_ENEMY);
	tiles[demon_tile+1]=new CTile(demon_bmp,TILE_SIZE_X*10,0,TILE_SIZE_X*10,TILE_SIZE_Y*7,T_ENEMY);
	tiles[demon_tile+2]=new CTile(demon_bmp,TILE_SIZE_X*20,0,TILE_SIZE_X*10,TILE_SIZE_Y*7,T_ENEMY);
	tiles[demon_tile+3]=new CTile(demon_bmp,TILE_SIZE_X*30,0,TILE_SIZE_X*10,TILE_SIZE_Y*7,T_ENEMY);
	tiles[demon_tile+4]=new CTile(demon_bmp,0,TILE_SIZE_Y*7,TILE_SIZE_X*10,TILE_SIZE_Y*7,T_ENEMY);
	tiles[demon_tile+5]=new CTile(demon_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*7,TILE_SIZE_X*10,TILE_SIZE_Y*7,T_ENEMY);
	
	tiles[demon_tile+6]=new CTile(demon_bmp,TILE_SIZE_X*0,TILE_SIZE_Y*14,TILE_SIZE_X*10,TILE_SIZE_Y*7,T_ENEMY);
	tiles[demon_tile+7]=new CTile(demon_bmp,TILE_SIZE_X*10,TILE_SIZE_Y*14,TILE_SIZE_X*10,TILE_SIZE_Y*7,T_ENEMY);
	tiles[demon_tile+8]=new CTile(demon_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*14,TILE_SIZE_X*10,TILE_SIZE_Y*7,T_ENEMY);
	tiles[demon_tile+9]=new CTile(demon_bmp,TILE_SIZE_X*40,TILE_SIZE_Y*14,TILE_SIZE_X*10,TILE_SIZE_Y*7,T_ENEMY);
	tiles[demon_tile+10]=new CTile(demon_bmp,TILE_SIZE_X*0,TILE_SIZE_Y*14,TILE_SIZE_X*10,TILE_SIZE_Y*7,T_ENEMY);
	tiles[demon_tile+11]=new CTile(demon_bmp,TILE_SIZE_X*0,TILE_SIZE_Y*14,TILE_SIZE_X*10,TILE_SIZE_Y*7,T_ENEMY);

	/* Huesos: */ 
	tiles[demon_tile+12]=new CTile(demon_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+13]=new CTile(demon_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+14]=new CTile(demon_bmp,TILE_SIZE_X*34,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+15]=new CTile(demon_bmp,TILE_SIZE_X*36,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+16]=new CTile(demon_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*7,TILE_SIZE_X*1,TILE_SIZE_Y*1,T_ENEMY);
	tiles[demon_tile+17]=new CTile(demon_bmp,TILE_SIZE_X*31,TILE_SIZE_Y*7,TILE_SIZE_X*1,TILE_SIZE_Y*1,T_ENEMY);
	tiles[demon_tile+18]=new CTile(demon_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*7,TILE_SIZE_X*1,TILE_SIZE_Y*1,T_ENEMY);
	tiles[demon_tile+19]=new CTile(demon_bmp,TILE_SIZE_X*33,TILE_SIZE_Y*7,TILE_SIZE_X*1,TILE_SIZE_Y*1,T_ENEMY);

	if (fighting_demon==160) {
		demon_obj=add_object(T_ENEMY,TILE_SIZE_X*19,TILE_SIZE_Y*8,0,T_DEMON1,demon_tile+1);
		object[demon_obj].base_tile=demon_tile;
		if (world_item[0][0]) object[demon_obj].size=24;
						 else object[demon_obj].size=32;
		demon_max_life=object[demon_obj].size;
	} /* if */ 
} /* load_demon1 */ 


void demon1_cycle(void)
{
	int val;
	object[demon_obj].state2--;

	if (object[demon_obj].size<(demon_max_life/2) && demon_music==0) {
		demon_music=1;
		Sound_subst_music_now("demon-fast");
	} /* if */ 
	if (object[demon_obj].size<(demon_max_life/4) && demon_music==1) {
		demon_music=2;
		Sound_subst_music_now("demon-vfast");
	} /* if */ 

	if (object[demon_obj].y<TILE_SIZE_Y*8) {
		/* Está volando: */ 
		object[demon_obj].state=demon_tile+2+((object[demon_obj].state2>>2)&0x01);
		if (object[demon_obj].state2<=0 && object[demon_obj].state3==1) {
			if (rand()%2==0) {
				/* Disparar huesos: */ 
				Sound_play(S_demon1bones);
				val=add_object(T_ENEMY,object[demon_obj].x+TILE_SIZE_X,object[demon_obj].y+2*TILE_SIZE_Y,1,T_DEMON1_BONES,-TILE_UNIT);
				object[val].base_tile=demon_tile;
				val=add_object(T_ENEMY,object[demon_obj].x+TILE_SIZE_X,object[demon_obj].y+(3*TILE_SIZE_Y)/2,1,T_DEMON1_BONES,-2*TILE_UNIT);
				object[val].base_tile=demon_tile;
				val=add_object(T_ENEMY,object[demon_obj].x+TILE_SIZE_Y,object[demon_obj].y+TILE_SIZE_Y,1,T_DEMON1_BONES,-3*TILE_UNIT);
				object[val].base_tile=demon_tile;
			} /* if */ 
			object[demon_obj].state3=0;
		} /* if */ 

		if (object[demon_obj].state3==1) object[demon_obj].y-=TILE_UNIT*2;
									else object[demon_obj].y+=TILE_UNIT*2;
		if (object[demon_obj].y==TILE_SIZE_Y*8) {
			Sound_play(S_demon1jump);
		} /* if */ 
	} else { 
		if (object[demon_obj].state2<=0) {
			switch(object[demon_obj].state-demon_tile) {
			case 1:switch((rand()%90)/10) {
					case 0:
					case 1:
				  		  object[demon_obj].state=demon_tile+4;
						  object[demon_obj].state2=8;
						  object[demon_obj].state3=demon_tile;
						  object[demon_obj].x=TILE_SIZE_X*18+TILE_SIZE_X/2;
						  break;
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
						  object[demon_obj].state=demon_tile+5;
						  object[demon_obj].state2=8;
						  object[demon_obj].state3=demon_tile+2;
						  object[demon_obj].x=TILE_SIZE_X*19;
						  break;
					default:
						  object[demon_obj].state=demon_tile+2;
						  object[demon_obj].state2=(((rand()%3)+2)*TILE_SIZE_Y)/2;
						  object[demon_obj].state3=1;
						  object[demon_obj].y-=TILE_UNIT*2;
						  break;
					} /* switch */ 
					break;	
			case 0:
  		  			object[demon_obj].state=demon_tile+4;
					object[demon_obj].state2=8;
					object[demon_obj].state3=demon_tile+1;
					object[demon_obj].x=TILE_SIZE_X*18+TILE_SIZE_X/2;
					break;
			case 2:
  		  			object[demon_obj].state=demon_tile+5;
					object[demon_obj].state2=8;
					object[demon_obj].state3=demon_tile+1;
					object[demon_obj].x=TILE_SIZE_X*19;
					break;
			case 4:
					object[demon_obj].state=object[demon_obj].state3;
					object[demon_obj].state2=((rand()%3)+1)*8;
					if (object[demon_obj].state==demon_tile) object[demon_obj].x=TILE_SIZE_X*18;
														else object[demon_obj].x=TILE_SIZE_X*19;
					break;
			case 5:
					if (object[demon_obj].state3==demon_tile+2) {
						/* Disparar huesos: */ 
						Sound_play(S_demon1bones);
						val=add_object(T_ENEMY,object[demon_obj].x+TILE_SIZE_X,object[demon_obj].y+2*TILE_SIZE_Y,1,T_DEMON1_BONES,-TILE_UNIT);
						object[val].base_tile=demon_tile;
						val=add_object(T_ENEMY,object[demon_obj].x+TILE_SIZE_X,object[demon_obj].y+(3*TILE_SIZE_Y)/2,1,T_DEMON1_BONES,-2*TILE_UNIT);
						object[val].base_tile=demon_tile;
						val=add_object(T_ENEMY,object[demon_obj].x+TILE_SIZE_Y,object[demon_obj].y+TILE_SIZE_Y,1,T_DEMON1_BONES,-3*TILE_UNIT);
						object[val].base_tile=demon_tile;
					} /* if */ 
					object[demon_obj].state=object[demon_obj].state3;
					object[demon_obj].state2=((rand()%4)+1)*8;
					object[demon_obj].x=TILE_SIZE_X*19;
					break;
			default:object[demon_obj].state=demon_tile;
					object[demon_obj].state2=8;
					object[demon_obj].x=TILE_SIZE_X*19;
					break;
			} /* switch */ 
		} /* if */ 
	} /* if */ 


} /* demon1_cycle */ 


/*									 */ 
/* ----------- DEMON 2 ------------- */ 
/*                                   */ 


void load_demon2(void)
{
	int i;

	if (demon_bmp!=0) delete demon_bmp;
	if (back_bmp!=0) delete back_bmp;
	demon_bmp=load_bitmap("demon2.pcx");
	back_bmp=load_bitmap("back2.pcx");

	for(i=0;i<8;i++) {
		if (tiles[demon_tile+i]!=0) delete tiles[demon_tile+i];
	} /* for */ 
	tiles[demon_tile]=new CTile(demon_bmp,0,0,TILE_SIZE_X*6,TILE_SIZE_Y*6,T_ENEMY);
	tiles[demon_tile+1]=new CTile(demon_bmp,TILE_SIZE_X*6,0,TILE_SIZE_X*6,TILE_SIZE_Y*6,T_ENEMY);
	tiles[demon_tile+2]=new CTile(demon_bmp,TILE_SIZE_X*12,0,TILE_SIZE_X*6,TILE_SIZE_Y*6,T_ENEMY);
	tiles[demon_tile+3]=new CTile(demon_bmp,TILE_SIZE_X*18,0,TILE_SIZE_X*6,TILE_SIZE_Y*6,T_ENEMY);
	tiles[demon_tile+4]=new CTile(demon_bmp,TILE_SIZE_X*24,0,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);
	tiles[demon_tile+5]=new CTile(demon_bmp,TILE_SIZE_X*25,0,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);
	tiles[demon_tile+6]=new CTile(demon_bmp,TILE_SIZE_X*26,0,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);

	/* Bolas rojas: */ 
	tiles[demon_tile+7]=new CTile(demon_bmp,TILE_SIZE_X*27,0,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);

	if (fighting_demon==160) {
		demon_obj=add_object(T_ENEMY,TILE_SIZE_X*13,TILE_SIZE_Y*5,0,T_DEMON2,demon_tile);
		object[demon_obj].base_tile=demon_tile;
		demon_obj2=add_object(T_ENEMY,TILE_SIZE_X*15+TILE_SIZE_X/2,TILE_SIZE_Y*5+TILE_UNIT,0,T_DEMON2_HEAD,-1);
		object[demon_obj2].base_tile=demon_tile;
		if (world_item[1][0]) object[demon_obj2].size=32;
						 else object[demon_obj2].size=48;
		demon_max_life=object[demon_obj2].size;
	} /* if */ 
} /* load_demon2 */ 

void demon2_cycle(void)
{
	if (object[demon_obj2].size<(demon_max_life/2) && demon_music==0) {
		demon_music=1;
		Sound_subst_music_now("demon-fast");
	} /* if */ 
	if (object[demon_obj2].size<(demon_max_life/4) && demon_music==1) {
		demon_music=2;
		Sound_subst_music_now("demon-vfast");
	} /* if */ 

	if (object[demon_obj].state2<128) {
		object[demon_obj].state=demon_tile;
	} /* if */ 
	if (object[demon_obj].state2>=128 && object[demon_obj].state2<160) {
		object[demon_obj].state=demon_tile+1;
	} /* if */ 
	if (object[demon_obj].state2>=160 && object[demon_obj].state2<352) {
		object[demon_obj].state=demon_tile+2+((object[demon_obj].state2&0x04)==0 ? 0 : 1);
		object[demon_obj2].state=demon_tile+4+((object[demon_obj].state2&0x04)==0 ? 0 : 1);
		if ((object[demon_obj].state2&0x07)==0) {
			int obj;
			/* DISPARAR BOLAS ROJAS: */
			Sound_play(S_demon2ball);
			obj=add_object(T_ENEMY,TILE_SIZE_X*15+TILE_SIZE_X/2,TILE_SIZE_Y*4,1,T_DEMON2_BALLS,0);
			object[obj].base_tile=demon_tile;
			object[obj].state=(rand()%(6*TILE_UNIT+1)-3*TILE_UNIT);
			object[obj].state2=12+(rand()%8);
		} /* if */ 
	} /* if */ 
	if (object[demon_obj].state2>=352 && object[demon_obj].state2<384) {
		object[demon_obj].state=demon_tile+1;
		object[demon_obj2].state=-1;
	} /* if */ 
	if (object[demon_obj].state2>=388) object[demon_obj].state2=0;

	object[demon_obj].state2++;
} /* demon2_cycle */ 



/*									 */ 
/* ----------- DEMON 3 ------------- */ 
/*                                   */ 


void load_demon3(void)
{
	int i;
	
	if (demon_bmp!=0) delete demon_bmp;
	if (back_bmp!=0) delete back_bmp;
	demon_bmp=load_bitmap("demon3.pcx");
	back_bmp=load_bitmap("back3.pcx");

	for(i=0;i<12;i++) {
		if (tiles[demon_tile+i]!=0) delete tiles[demon_tile+i];
	} /* for */ 
	tiles[demon_tile]=new CTile(demon_bmp,0,0,TILE_SIZE_X*16,TILE_SIZE_Y*3,T_ENEMY);
	tiles[demon_tile+1]=new CTile(demon_bmp,0,TILE_SIZE_Y*3,TILE_SIZE_X*16,TILE_SIZE_Y*3,T_ENEMY);
	tiles[demon_tile+2]=new CTile(demon_bmp,0,TILE_SIZE_Y*6,TILE_SIZE_X*16,TILE_SIZE_Y*3,T_ENEMY);
	tiles[demon_tile+3]=new CTile(demon_bmp,TILE_SIZE_X*16,0,TILE_SIZE_X*16,TILE_SIZE_Y*3,T_ENEMY);
	tiles[demon_tile+4]=new CTile(demon_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*3,TILE_SIZE_X*16,TILE_SIZE_Y*3,T_ENEMY);
	tiles[demon_tile+5]=new CTile(demon_bmp,TILE_SIZE_X*16,TILE_SIZE_Y*6,TILE_SIZE_X*16,TILE_SIZE_Y*3,T_ENEMY);

	tiles[demon_tile+6]=new CTile(demon_bmp,0,TILE_SIZE_Y*9,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);
	tiles[demon_tile+7]=new CTile(demon_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*9,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);
	tiles[demon_tile+8]=new CTile(demon_bmp,TILE_SIZE_X*3,TILE_SIZE_Y*9,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);
	tiles[demon_tile+9]=new CTile(demon_bmp,0,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);
	tiles[demon_tile+10]=new CTile(demon_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*10,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);
	tiles[demon_tile+11]=new CTile(demon_bmp,TILE_SIZE_X*3,TILE_SIZE_Y*10,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);

	if (fighting_demon==160) {
		demon_obj=add_object(T_ENEMY,0,0,0,T_DEMON3,0);
		object[demon_obj].base_tile=demon_tile;
		object[demon_obj].tile=-1;
		if (world_item[2][0]) object[demon_obj].size=64;
						 else object[demon_obj].size=96;
		demon_max_life=object[demon_obj].size;
	} /* if */ 
} /* load_demon3 */ 

void demon3_cycle(void)
{
	object[demon_obj].state2++;
	if (object[demon_obj].size<(demon_max_life/2) && demon_music==0) {
		demon_music=1;
		Sound_subst_music_now("demon-fast");
	} /* if */ 
	if (object[demon_obj].size<(demon_max_life/4) && demon_music==1) {
		demon_music=2;
		Sound_subst_music_now("demon-vfast");
	} /* if */ 
			
	switch(object[demon_obj].state) {
	case 0:	object[demon_obj].tile=-1;
			if (object[demon_obj].state2>=64) {
				object[demon_obj].state2=0;
				object[demon_obj].state=2-((rand()%4)/2);
				if (object[demon_obj].state==1) object[demon_obj].x=-TILE_SIZE_X*16;
										   else object[demon_obj].x=32*TILE_SIZE_X;
				object[demon_obj].y=(pers_y/(TILE_SIZE_Y*4))+((rand()%3)-1);;
				if (object[demon_obj].y<0) object[demon_obj].y=0;
				if (object[demon_obj].y>4) object[demon_obj].y=4;
				if ((rand()%10)==0) object[demon_obj].y=rand()%5;
				object[demon_obj].y*=TILE_SIZE_Y*4;
				if ((rand()%8)==0) object[demon_obj].state+=6;
			} /* if */ 
			break;
	case 1:	if ((object[demon_obj].state2&0x08)!=0) object[demon_obj].tile=1;
											   else object[demon_obj].tile=2;
			if ((object[demon_obj].state2&0x03)==0) object[demon_obj].x+=TILE_SIZE_X;
			if (object[demon_obj].x==0) {
				object[demon_obj].state=3;
				object[demon_obj].state2=0;
				object[demon_obj].tile=0;
			} /* if */ 
			break;
	case 2: if ((object[demon_obj].state2&0x08)!=0) object[demon_obj].tile=4;
											   else object[demon_obj].tile=5; 
			if ((object[demon_obj].state2&0x03)==0) object[demon_obj].x-=TILE_SIZE_X;
			if (object[demon_obj].x==16*TILE_SIZE_X) {
				object[demon_obj].state=4;
				object[demon_obj].state2=0;
				object[demon_obj].tile=3;
			} /* if */ 
			break;
	case 3: object[demon_obj].tile=0;
			if (object[demon_obj].state2==16) {
				add_object(T_ENEMY,object[demon_obj].x+16*TILE_SIZE_X,object[demon_obj].y+TILE_SIZE_Y,0,T_DEMON3_FIRE,0);
				Sound_play(S_firebreath);
			} /* if */ 
			if (object[demon_obj].state2>=64) {
				object[demon_obj].state2=0;
				object[demon_obj].state=5;
			} /* if */ 
			break;
	case 4: object[demon_obj].tile=3;
			if (object[demon_obj].state2==16) {
				add_object(T_ENEMY,object[demon_obj].x-2*TILE_SIZE_X,object[demon_obj].y+TILE_SIZE_Y,0,T_DEMON3_FIRE,1);
				Sound_play(S_firebreath);
			} /* if */ 
			if (object[demon_obj].state2>=64) {
				object[demon_obj].state2=0;
				object[demon_obj].state=6;
			} /* if */ 
			break;
	case 5:	if ((object[demon_obj].state2&0x08)==0) object[demon_obj].tile=1;
											   else object[demon_obj].tile=2;
			if ((object[demon_obj].state2&0x03)==0) object[demon_obj].x-=TILE_SIZE_X;
			if (object[demon_obj].x==-16*TILE_SIZE_X) {
				object[demon_obj].state=0;
				object[demon_obj].state2=0;
			} /* if */ 
			break;
	case 6: if ((object[demon_obj].state2&0x08)==0) object[demon_obj].tile=4;
											   else object[demon_obj].tile=5; 
			if ((object[demon_obj].state2&0x03)==0) object[demon_obj].x+=TILE_SIZE_X;
			if (object[demon_obj].x==32*TILE_SIZE_X) {
				object[demon_obj].state=0;
				object[demon_obj].state2=0;
			} /* if */ 
			break;
	case 7:	if ((object[demon_obj].state2&0x08)!=0) object[demon_obj].tile=1;
											   else object[demon_obj].tile=2;
			if ((object[demon_obj].state2&0x03)==0) object[demon_obj].x+=TILE_SIZE_X;
			if (object[demon_obj].x==-4*TILE_SIZE_X) {
				object[demon_obj].state=3;
				object[demon_obj].state2=0;
				object[demon_obj].tile=0;
			} /* if */ 
			break;
	case 8: if ((object[demon_obj].state2&0x08)!=0) object[demon_obj].tile=4;
											   else object[demon_obj].tile=5; 
			if ((object[demon_obj].state2&0x03)==0) object[demon_obj].x-=TILE_SIZE_X;
			if (object[demon_obj].x==20*TILE_SIZE_X) {
				object[demon_obj].state=4;
				object[demon_obj].state2=0;
				object[demon_obj].tile=3;
			} /* if */ 
			break;
	} /* switch */ 

} /* demon3_cycle */ 


/*									 */ 
/* ----------- DEMON 4 ------------- */ 
/*                                   */ 


void load_demon4(void)
{
	int i;

	if (demon_bmp!=0) delete demon_bmp;
	if (back_bmp!=0) delete back_bmp;
	demon_bmp=load_bitmap("demon4.pcx");
	back_bmp=load_bitmap("back4.pcx");

	for(i=0;i<6;i++) {
		if (tiles[demon_tile+i]!=0) delete tiles[demon_tile+i];
	} /* for */ 
	tiles[demon_tile]=new CTile(demon_bmp,0,0,TILE_SIZE_X*10,TILE_SIZE_Y*5,T_ENEMY);
	tiles[demon_tile+1]=new CTile(demon_bmp,TILE_SIZE_X*10,0,TILE_SIZE_X*10,TILE_SIZE_Y*5,T_ENEMY);
	tiles[demon_tile+2]=new CTile(demon_bmp,TILE_SIZE_X*20,0,TILE_SIZE_X*10,TILE_SIZE_Y*5,T_ENEMY);
	tiles[demon_tile+3]=new CTile(demon_bmp,TILE_SIZE_X*30,0,TILE_SIZE_X*10,TILE_SIZE_Y*5,T_ENEMY);
	tiles[demon_tile+4]=new CTile(demon_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*5,TILE_SIZE_X*10,TILE_SIZE_Y*5,T_ENEMY);
	tiles[demon_tile+5]=new CTile(demon_bmp,TILE_SIZE_X*30,TILE_SIZE_Y*5,TILE_SIZE_X*10,TILE_SIZE_Y*5,T_ENEMY);

	if (fighting_demon==160) {
		demon_obj=add_object(T_ENEMY,8*TILE_SIZE_X,10*TILE_SIZE_Y,0,T_DEMON4,0);
		object[demon_obj].base_tile=demon_tile;
		object[demon_obj].tile=0;
		if (world_item[3][0]) object[demon_obj].size=32;
						 else object[demon_obj].size=48;
		demon_max_life=object[demon_obj].size;
	} /* if */ 

} /* load_demon4 */ 

void demon4_cycle(void)
{
	object[demon_obj].state2++;
	if (object[demon_obj].size<(demon_max_life/2) && demon_music==0) {
		demon_music=1;
		Sound_subst_music_now("demon-fast");
	} /* if */ 
	if (object[demon_obj].size<(demon_max_life/4) && demon_music==1) {
		demon_music=2;
		Sound_subst_music_now("demon-vfast");
	} /* if */ 
			
	switch(object[demon_obj].state) {
	case 0: object[demon_obj].x++;
			object[demon_obj].tile=(object[demon_obj].state2>>3)%2;
			if (object[demon_obj].x>=19*TILE_SIZE_X) object[demon_obj].state=1;
			if (pers_x<(object[demon_obj].x+12*TILE_SIZE_X)) {
				object[demon_obj].state=2;
				object[demon_obj].state2=0;
			} /* if */ 
			break;
	case 1: object[demon_obj].x--;
			object[demon_obj].tile=(object[demon_obj].state2>>3)%2;
			if (object[demon_obj].x<=3*TILE_SIZE_X) object[demon_obj].state=0;
			if (pers_x>(object[demon_obj].x-4*TILE_SIZE_X)) {
				object[demon_obj].state=4;
				object[demon_obj].state2=0;
			} /* if */ 
			break;
	case 2:	if (object[demon_obj].x>=19*TILE_SIZE_X) object[demon_obj].state=4;
			if (object[demon_obj].state2<16) {
				object[demon_obj].tile=2;
			} else {
				object[demon_obj].x+=2;
				if (object[demon_obj].state2<26) object[demon_obj].tile=3;
										    else object[demon_obj].tile=0;
				if (object[demon_obj].state2<20) object[demon_obj].y-=TILE_UNIT*2;
				if (object[demon_obj].state2<24) object[demon_obj].y-=TILE_UNIT*2;
				if (object[demon_obj].state2<28) object[demon_obj].y-=TILE_UNIT;
				if (object[demon_obj].state2<30) object[demon_obj].y-=TILE_UNIT;
				if (object[demon_obj].state2<32) object[demon_obj].y-=TILE_UNIT;
				if (object[demon_obj].state2==36) {
					object[demon_obj].state=3;
					object[demon_obj].state2=0;
				} /* if */ 
			} /* if */ 				
			break;
	case 3: if (object[demon_obj].x>=19*TILE_SIZE_X) object[demon_obj].state=5;
			if (object[demon_obj].state2<=20) {
				object[demon_obj].x+=2;
				if (object[demon_obj].state2<10) object[demon_obj].tile=0;
										    else object[demon_obj].tile=5;
				if (object[demon_obj].state2>16) object[demon_obj].y+=TILE_UNIT*2;
				if (object[demon_obj].state2>12) object[demon_obj].y+=TILE_UNIT*2;
				if (object[demon_obj].state2>8) object[demon_obj].y+=TILE_UNIT;
				if (object[demon_obj].state2>6) object[demon_obj].y+=TILE_UNIT;
				if (object[demon_obj].state2>4) object[demon_obj].y+=TILE_UNIT;
			} else {
				if (object[demon_obj].state2==21) Sound_play(S_demon4jump);
				object[demon_obj].tile=4;
				if (object[demon_obj].state2==36) {
					if (pers_x<object[demon_obj].x) object[demon_obj].state=1;
											   else object[demon_obj].state=0;
				} /* if */ 
			} /* if */ 
			break;
	case 4:	if (object[demon_obj].x<=3*TILE_SIZE_X) object[demon_obj].state=2;
			if (object[demon_obj].state2<16) {
				object[demon_obj].tile=4;
			} else {
				object[demon_obj].x-=2;
				if (object[demon_obj].state2<26) object[demon_obj].tile=5;
										    else object[demon_obj].tile=0;
				if (object[demon_obj].state2<20) object[demon_obj].y-=TILE_UNIT*2;
				if (object[demon_obj].state2<24) object[demon_obj].y-=TILE_UNIT*2;
				if (object[demon_obj].state2<28) object[demon_obj].y-=TILE_UNIT;
				if (object[demon_obj].state2<30) object[demon_obj].y-=TILE_UNIT;
				if (object[demon_obj].state2<32) object[demon_obj].y-=TILE_UNIT;
				if (object[demon_obj].state2==36) {
					object[demon_obj].state=5;
					object[demon_obj].state2=0;
				} /* if */ 
			} /* if */ 				
			break;
	case 5: if (object[demon_obj].x<=3*TILE_SIZE_X) object[demon_obj].state=3;
			if (object[demon_obj].state2<=20) {
				object[demon_obj].x-=2;
				if (object[demon_obj].state2<10) object[demon_obj].tile=0;
										    else object[demon_obj].tile=3;
				if (object[demon_obj].state2>16) object[demon_obj].y+=TILE_UNIT*2;
				if (object[demon_obj].state2>12) object[demon_obj].y+=TILE_UNIT*2;
				if (object[demon_obj].state2>8) object[demon_obj].y+=TILE_UNIT;
				if (object[demon_obj].state2>6) object[demon_obj].y+=TILE_UNIT;
				if (object[demon_obj].state2>4) object[demon_obj].y+=TILE_UNIT;
			} else {
				if (object[demon_obj].state2==21) Sound_play(S_demon4jump);
				object[demon_obj].tile=2;
				if (object[demon_obj].state2==36) {
					if (pers_x<object[demon_obj].x) object[demon_obj].state=1;
											   else object[demon_obj].state=0;
				} /* if */ 
			} /* if */ 
			break;
	} /* switch */ 
} /* demon4_cycle */ 


/*									 */ 
/* ----------- DEMON 5 ------------- */ 
/*                                   */ 


void load_demon5(void)
{
	int i;

	if (demon_bmp!=0) delete demon_bmp;
	if (back_bmp!=0) delete back_bmp;
	demon_bmp=load_bitmap("demon5.pcx");
	back_bmp=load_bitmap("back5.pcx");

	for(i=0;i<26;i++) {
		if (tiles[demon_tile+i]!=0) delete tiles[demon_tile+i];
	} /* for */ 
	tiles[demon_tile]=new CTile(demon_bmp,0,0,TILE_SIZE_X*10,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+1]=new CTile(demon_bmp,TILE_SIZE_X*10,0,TILE_SIZE_X*10,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+2]=new CTile(demon_bmp,TILE_SIZE_X*20,0,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+3]=new CTile(demon_bmp,TILE_SIZE_X*24,0,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+4]=new CTile(demon_bmp,TILE_SIZE_X*28,0,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+5]=new CTile(demon_bmp,TILE_SIZE_X*32,0,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+6]=new CTile(demon_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*4,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+7]=new CTile(demon_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*4,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+8]=new CTile(demon_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*4,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+9]=new CTile(demon_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*4,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+10]=new CTile(demon_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*8,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+11]=new CTile(demon_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*8,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+12]=new CTile(demon_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*8,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+13]=new CTile(demon_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*8,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+14]=new CTile(demon_bmp,TILE_SIZE_X*20,TILE_SIZE_Y*12,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+15]=new CTile(demon_bmp,TILE_SIZE_X*24,TILE_SIZE_Y*12,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+16]=new CTile(demon_bmp,TILE_SIZE_X*28,TILE_SIZE_Y*12,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+17]=new CTile(demon_bmp,TILE_SIZE_X*32,TILE_SIZE_Y*12,TILE_SIZE_X*4,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+18]=new CTile(demon_bmp,0,TILE_SIZE_X*6,TILE_SIZE_X*10,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+19]=new CTile(demon_bmp,TILE_SIZE_X*10,TILE_SIZE_X*6,TILE_SIZE_X*10,TILE_SIZE_Y*4,T_ENEMY);

	tiles[demon_tile+20]=new CTile(demon_bmp,0,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);
	tiles[demon_tile+21]=new CTile(demon_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*4,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);
	tiles[demon_tile+22]=new CTile(demon_bmp,TILE_SIZE_X*3,TILE_SIZE_Y*4,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);
	tiles[demon_tile+23]=new CTile(demon_bmp,0,TILE_SIZE_Y*5,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);
	tiles[demon_tile+24]=new CTile(demon_bmp,TILE_SIZE_X*2,TILE_SIZE_Y*5,TILE_SIZE_X,TILE_SIZE_Y,T_ENEMY);
	tiles[demon_tile+25]=new CTile(demon_bmp,TILE_SIZE_X*3,TILE_SIZE_Y*5,TILE_SIZE_X*2,TILE_SIZE_Y,T_ENEMY);

	if (fighting_demon==160) {
		demon_obj=add_object(T_ENEMY,14*TILE_SIZE_X,9*TILE_SIZE_Y,0,T_DEMON5,0);
		object[demon_obj].base_tile=demon_tile;
		object[demon_obj].tile=0;
		object[demon_obj].state3=2;
		if (world_item[3][0]) object[demon_obj].size=40;
						 else object[demon_obj].size=64;
		demon_max_life=object[demon_obj].size;
	} /* if */ 
} /* load_demon5 */ 


void demon5_cycle(void)
{
	int obj[4]={0,18,1,19};
	object[demon_obj].state2++;
	if (object[demon_obj].size<(demon_max_life/2) && demon_music==0) {
		demon_music=1;
		Sound_subst_music_now("demon-fast");
	} /* if */ 
	if (object[demon_obj].size<(demon_max_life/4) && demon_music==1) {
		demon_music=2;
		Sound_subst_music_now("demon-vfast");
	} /* if */ 
			
	switch(object[demon_obj].state) {
	case 0: 
	case 1: if (object[demon_obj].state==0) object[demon_obj].tile=obj[(object[demon_obj].state2>>2)%4];
									   else object[demon_obj].tile=obj[(object[demon_obj].state2>>1)%4];
			if (object[demon_obj].state2>=32) {
				object[demon_obj].state+=2;
				object[demon_obj].state2=0;
				object[demon_obj].union_x=(8+(rand()%8))*TILE_SIZE_X;
				object[demon_obj].union_y=(4+(rand()%7))*TILE_SIZE_Y;
			} /* if */ 
			break;

	case 2: 
	case 3: 
	case 4:
	case 5: if (object[demon_obj].state==2 ||
				object[demon_obj].state==4) object[demon_obj].tile=obj[(object[demon_obj].state2>>2)%4];
									   else object[demon_obj].tile=obj[(object[demon_obj].state2>>1)%4];
			if (object[demon_obj].y<object[demon_obj].union_y) object[demon_obj].y+=TILE_UNIT*2;
			if (object[demon_obj].y>object[demon_obj].union_y) object[demon_obj].y-=TILE_UNIT*2;
			if (object[demon_obj].x<object[demon_obj].union_x) object[demon_obj].x+=TILE_UNIT*2;
			if (object[demon_obj].x>object[demon_obj].union_x) object[demon_obj].x-=TILE_UNIT*2;
			if ((object[demon_obj].state2&0x07)==0) {
				int dp=2;
				if (pers_y>object[demon_obj].y && pers_y<=(object[demon_obj].y+2*TILE_SIZE_Y)) dp=3;
				if (pers_y>(object[demon_obj].y+2*TILE_SIZE_Y) && pers_y<=(object[demon_obj].y+3*TILE_SIZE_Y)) dp=4;
				if (pers_y>(object[demon_obj].y+3*TILE_SIZE_Y)) dp=5;
				if (object[demon_obj].state3<dp) object[demon_obj].state3++;
				if (object[demon_obj].state3>dp) object[demon_obj].state3--;
			} /* if */ 
			if (object[demon_obj].x==object[demon_obj].union_x &&
				object[demon_obj].y==object[demon_obj].union_y) {
				if (object[demon_obj].state<4) {
					object[demon_obj].state+=2;
					object[demon_obj].state2=0;
					object[demon_obj].union_x=(8+(rand()%8))*TILE_SIZE_X;
					object[demon_obj].union_y=(4+(rand()%7))*TILE_SIZE_Y;
				} else {
					if ((object[demon_obj].state2&0x0f)==0) {
						object[demon_obj].state+=2;
						object[demon_obj].state2=0;
						object[demon_obj].state3+=4;
					} /* if */ 
				} /* if */ 
			} /* if */ 
			break;

	case 6:
	case 7: if (object[demon_obj].state==6) object[demon_obj].tile=obj[(object[demon_obj].state2>>2)%4];
									   else object[demon_obj].tile=obj[(object[demon_obj].state2>>1)%4];
			if (object[demon_obj].state2==32) {
				int yo[4]={0,TILE_SIZE_Y,2*TILE_SIZE_Y,3*TILE_SIZE_Y};
				add_object(T_ENEMY,object[demon_obj].x-2*TILE_SIZE_X,object[demon_obj].y+yo[object[demon_obj].state3-6]+TILE_SIZE_Y,0,T_DEMON5_FIRE,1);
				Sound_play(S_firebreath);
			} /* if */ 
		    if (object[demon_obj].state2>=64) {
				object[demon_obj].state=rand()&0x01;;
				object[demon_obj].state2=0;
				object[demon_obj].state3-=4;
			} /* if */ 
			break;
	} /* switch */ 
} /* demon5_cycle */ 


/*									 */ 
/* ----------- DEMON 6 ------------- */ 
/*                                   */ 


void load_demon6(void)
{
	int i;

	if (demon_bmp!=0) delete demon_bmp;
	if (back_bmp!=0) delete back_bmp;
	demon_bmp=load_bitmap("demon6.pcx");
	back_bmp=load_bitmap("back6.pcx");

	for(i=0;i<6;i++) {
		if (tiles[demon_tile+i]!=0) delete tiles[demon_tile+i];
	} /* for */ 
	tiles[demon_tile]=new CTile(demon_bmp,0,0,TILE_SIZE_X*10,TILE_SIZE_Y*5,T_ENEMY);
	tiles[demon_tile+1]=new CTile(demon_bmp,0,5*TILE_SIZE_Y,TILE_SIZE_X*10,TILE_SIZE_Y*5,T_ENEMY);
	tiles[demon_tile+2]=new CTile(demon_bmp,0,10*TILE_SIZE_Y,TILE_SIZE_X*10,TILE_SIZE_Y*5,T_ENEMY);
	tiles[demon_tile+3]=new CTile(demon_bmp,0,15*TILE_SIZE_Y,TILE_SIZE_X*10,TILE_SIZE_Y*5,T_ENEMY);

	/* Bolas azules: */ 
	tiles[demon_tile+4]=new CTile(demon_bmp,TILE_SIZE_X*10,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+5]=new CTile(demon_bmp,TILE_SIZE_X*10,2*TILE_SIZE_Y,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	if (fighting_demon==160) {
		demon_obj=add_object(T_ENEMY,TILE_SIZE_X*12,TILE_SIZE_Y*14,0,T_DEMON6,0);
		object[demon_obj].base_tile=demon_tile;
		if (world_item[1][0]) object[demon_obj].size=48;
						 else object[demon_obj].size=80;

//		object[demon_obj].size=2;
		demon_max_life=object[demon_obj].size;
	} /* if */ 
} /* load_demon6 */ 

void demon6_cycle(void)
{
	int cycle[4]={0,1,2,1};
	if (object[demon_obj].size<(demon_max_life/2) && demon_music==0) {
		demon_music=1;
		Sound_subst_music_now("demon-fast");
	} /* if */ 
	if (object[demon_obj].size<(demon_max_life/4) && demon_music==1) {
		demon_music=2;
		Sound_subst_music_now("demon-vfast");
	} /* if */ 

	object[demon_obj].state2++;
			
	switch(object[demon_obj].state) {
	case 0:
		/* Quieto: */ 
		object[demon_obj].tile=(cycle[(object[demon_obj].state2>>3)%4]);
		object[demon_obj].state=(rand()%32)>>4;
		object[demon_obj].state3=(rand()%80)>>1;

		/* Tendencia a ir hacia el personaje: */ 
		if ((object[demon_obj].x+4*TILE_SIZE_X)<pers_x && (rand()%32)<20) object[demon_obj].state=2;
		if ((object[demon_obj].x+4*TILE_SIZE_X)>pers_x && (rand()%32)<20) object[demon_obj].state=1;

		/* Que no se salga de los bordes: */ 
//		if (object[demon_obj].x<((5+(object[demon_obj].state3/8))*TILE_SIZE_X)) object[demon_obj].state=2;
//		if (object[demon_obj].x>((17-(object[demon_obj].state3/8))*TILE_SIZE_X)) object[demon_obj].state=1;
		break;
	case 1:
		/* Moviendose a la izquierda: */ 
		object[demon_obj].tile=(cycle[(object[demon_obj].state2>>3)%4]);
		object[demon_obj].state3--;
		if (object[demon_obj].state3<=0 || object[demon_obj].x<=5*TILE_SIZE_X) {
			if ((rand()%32)<4) {
				object[demon_obj].state=3;
				object[demon_obj].state2=0;
			} else {
				object[demon_obj].state=0;
			} /* if */ 
		} else {
			object[demon_obj].x-=TILE_UNIT;
		} /* if */ 
		break;
	case 2:
		/* Moviendose a la derecha: */ 
		object[demon_obj].tile=(cycle[(object[demon_obj].state2>>3)%4]);
		object[demon_obj].state3--;
		if (object[demon_obj].state3==0 || object[demon_obj].x>=17*TILE_SIZE_X) {
			if ((rand()%32)<8) {
				object[demon_obj].state=3;
				object[demon_obj].state2=0;
			} else {
				object[demon_obj].state=0;
			} /* if */ 
		} else {
			object[demon_obj].x+=TILE_UNIT;
		} /* if */ 
		break;
	case 3:
		/* Preparado para disparar: */ 
		object[demon_obj].tile=2;
		object[demon_obj].state3=((rand()%13)+3)/4;
		object[demon_obj].state=4;
		object[demon_obj].state2=0;
		break;
	case 4:
		/* Disparando: */ 
		if (object[demon_obj].state2==32) {
			if (object[demon_obj].state3==0) {
				object[demon_obj].state=0;
				object[demon_obj].state2=0;
			} else {
				object[demon_obj].state2=0;
				object[demon_obj].state3--;
				/* Disparar: */ 
				Sound_play(S_demon2ball);
				{
					/* Calcular el ángulo del personaje: */ 
					float angle;
					float dx,dy;
					const float PI=3.141592F;

					dx=float(pers_x-(object[demon_obj].x+4*TILE_SIZE_X));
					dy=float(pers_y-(object[demon_obj].y));
					angle=float(atan2(dy,dx));

					/* Disparar unas cuantas bolas, con diferentes ángulos: */ 
					int presets[4][6]={ {-8,-6,-2,2,4,8},
										{-8,-2, 0,2,6,8},
										{-8,-4,-2,0,2,8},
										{-8,-6,-2,3,5,8} };
					int preset=rand()%4;
					int i;

					for(i=0;i<6;i++) {
						int j;
						j=add_object(T_ENEMY,object[demon_obj].x+4*TILE_SIZE_X,object[demon_obj].y,1,T_DEMON6_BULLET,0);
						object[j].union_x=object[j].x*TILE_SIZE_X;
						object[j].union_y=object[j].y*TILE_SIZE_Y;
						object[j].state=int(cos(angle+(PI/48)*presets[preset][i])*TILE_SIZE_X*6);
						object[j].state2=int(sin(angle+(PI/48)*presets[preset][i])*TILE_SIZE_Y*6);
						object[j].base_tile=demon_tile;
						object[j].tile=4;
					} /* for */ 
					
				}
			} /* if */ 
		} /* if */ 
		break;
	} /* switch */ 
} /* demon6_cycle */ 


/*									 */ 
/* ----------- DEMON 7 ------------- */ 
/*                                   */ 


void load_demon7(void)
{
	int i;

	if (demon_bmp!=0) delete demon_bmp;
	if (back_bmp!=0) delete back_bmp;
	demon_bmp=load_bitmap("demon7.pcx");
	back_bmp=load_bitmap("back7.pcx");

	for(i=0;i<16;i++) {
		if (tiles[demon_tile+i]!=0) delete tiles[demon_tile+i];
	} /* for */ 
	tiles[demon_tile]=new CTile(demon_bmp,0,0*TILE_SIZE_Y,TILE_SIZE_X*8,TILE_SIZE_Y*8,T_ENEMY);
	tiles[demon_tile+1]=new CTile(demon_bmp,8*TILE_SIZE_X,0*TILE_SIZE_Y,TILE_SIZE_X*8,TILE_SIZE_Y*8,T_ENEMY);
	tiles[demon_tile+2]=new CTile(demon_bmp,16*TILE_SIZE_X,0*TILE_SIZE_Y,TILE_SIZE_X*8,TILE_SIZE_Y*8,T_ENEMY);
	tiles[demon_tile+3]=new CTile(demon_bmp,24*TILE_SIZE_X,0*TILE_SIZE_Y,TILE_SIZE_X*8,TILE_SIZE_Y*8,T_ENEMY);
	tiles[demon_tile+4]=new CTile(demon_bmp,0,10*TILE_SIZE_Y,TILE_SIZE_X*8,TILE_SIZE_Y*8,T_ENEMY);
	tiles[demon_tile+5]=new CTile(demon_bmp,8*TILE_SIZE_X,10*TILE_SIZE_Y,TILE_SIZE_X*8,TILE_SIZE_Y*8,T_ENEMY);
	tiles[demon_tile+6]=new CTile(demon_bmp,16*TILE_SIZE_X,10*TILE_SIZE_Y,TILE_SIZE_X*8,TILE_SIZE_Y*8,T_ENEMY);
	tiles[demon_tile+7]=new CTile(demon_bmp,24*TILE_SIZE_X,10*TILE_SIZE_Y,TILE_SIZE_X*8,TILE_SIZE_Y*8,T_ENEMY);

	tiles[demon_tile+8]=new CTile(demon_bmp,0*TILE_SIZE_X,8*TILE_SIZE_Y,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+9]=new CTile(demon_bmp,2*TILE_SIZE_X,8*TILE_SIZE_Y,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+10]=new CTile(demon_bmp,4*TILE_SIZE_X,8*TILE_SIZE_Y,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+11]=new CTile(demon_bmp,6*TILE_SIZE_X,8*TILE_SIZE_Y,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+12]=new CTile(demon_bmp,8*TILE_SIZE_X,8*TILE_SIZE_Y,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+13]=new CTile(demon_bmp,10*TILE_SIZE_X,8*TILE_SIZE_Y,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+14]=new CTile(demon_bmp,12*TILE_SIZE_X,8*TILE_SIZE_Y,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+15]=new CTile(demon_bmp,14*TILE_SIZE_X,8*TILE_SIZE_Y,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	if (fighting_demon==160) {
		demon_obj=add_object(T_ENEMY,TILE_SIZE_X*12,TILE_SIZE_Y*4,0,T_DEMON7,0);
		object[demon_obj].base_tile=demon_tile;
		if (world_item[1][0]) object[demon_obj].size=48;
						 else object[demon_obj].size=80;
		demon_max_life=object[demon_obj].size;
	} /* if */ 
} /* load_demon7 */ 


void demon7_cycle(void)
{
	if (object[demon_obj].size<(demon_max_life/2) && demon_music==0) {
		demon_music=1;
		Sound_subst_music_now("demon-fast");
	} /* if */ 
	if (object[demon_obj].size<(demon_max_life/4) && demon_music==1) {
		demon_music=2;
		Sound_subst_music_now("demon-vfast");
	} /* if */ 

	object[demon_obj].state2++;
			
	switch(object[demon_obj].state) {
	case 0:/* Boca cerrada, está a punto de abrirla: */ 
		object[demon_obj].tile=0;
		if (object[demon_obj].state2>=16) {
			object[demon_obj].state=1;
			object[demon_obj].state2=0;
			switch(rand()%6) {
			case 0:
			case 1:
				   object[demon_obj].union_x=TILE_UNIT;
				   object[demon_obj].union_y=0;
				   object[demon_obj].state3=32+(rand()%4)*8;
				   break;
			case 2:
			case 3:
				   object[demon_obj].union_x=-TILE_UNIT;
				   object[demon_obj].union_y=0;
				   object[demon_obj].state3=32+(rand()%4)*8;
				   break;
			case 4:object[demon_obj].union_x=0;
				   object[demon_obj].union_y=TILE_UNIT;
				   object[demon_obj].state3=8+(rand()%4)*8;
				   break;
			case 5:object[demon_obj].union_x=0;
				   object[demon_obj].union_y=-TILE_UNIT;
				   object[demon_obj].state3=8+(rand()%4)*8;
				   break;
			} /* switch */ 
		} /* if */ 
		break;
	case 1:/* Boca abierta, y disparando: */ 
		object[demon_obj].tile=3;

		if ((object[demon_obj].state2&0x0f)==0) {
			int j;
			int dx,dy;

			/* Disparar: */ 
			Sound_play(S_demon7bullet);
			j=add_object(T_ENEMY,object[demon_obj].x+3*TILE_SIZE_X,object[demon_obj].y+3*TILE_SIZE_Y,1,T_DEMON7_BULLET,0);
			object[j].base_tile=demon_tile;
			dx=pers_x-(object[demon_obj].x+3*TILE_SIZE_X);
			dy=pers_y-(object[demon_obj].y+3*TILE_SIZE_Y);
			if (abs(dx)>(abs(dy)*2)) {
				if (dx>0) {
					object[j].tile=8;
				} else {
					object[j].tile=9;
				} /* if */ 
			} else {
				if (abs(dy)>(abs(dx)*2)) {
					if (dy>0) {
						object[j].tile=11;
					} else {
						object[j].tile=10;
					} /* if */ 
				} else {
					if (dx<0) {
						if (dy<0) {
							object[j].tile=12;
						} else {
							object[j].tile=15;
						} /* if */ 
					} else {
						if (dy<0) {
							object[j].tile=13;
						} else {
							object[j].tile=14;
						} /* if */ 
					} /* if */ 
				} /* if */ 
			} /* if */ 
		} /* if */ 

		/* Moverse: */ 
		if (object[demon_obj].state3<=0) {
			switch(rand()%6) {
			case 0:
			case 1:
				   object[demon_obj].union_x=TILE_UNIT;
				   object[demon_obj].union_y=0;
				   object[demon_obj].state3=32+(rand()%4)*8;
				   break;
			case 2:
			case 3:
				   object[demon_obj].union_x=-TILE_UNIT;
				   object[demon_obj].union_y=0;
				   object[demon_obj].state3=32+(rand()%4)*8;
				   break;
			case 4:object[demon_obj].union_x=0;
				   object[demon_obj].union_y=TILE_UNIT;
				   object[demon_obj].state3=8+(rand()%4)*8;
				   break;
			case 5:object[demon_obj].union_x=0;
				   object[demon_obj].union_y=-TILE_UNIT;
				   object[demon_obj].state3=8+(rand()%4)*8;
				   break;
			} /* switch */ 
		} else {
			object[demon_obj].state3--;
			object[demon_obj].x+=object[demon_obj].union_x;
			object[demon_obj].y+=object[demon_obj].union_y;

			if (object[demon_obj].x<=6*TILE_SIZE_X && object[demon_obj].union_x<0) object[demon_obj].union_x=TILE_UNIT;
			if (object[demon_obj].x>=18*TILE_SIZE_X && object[demon_obj].union_x>0) object[demon_obj].union_x=-TILE_UNIT;
			if (object[demon_obj].y<=4*TILE_SIZE_Y && object[demon_obj].union_y<0) object[demon_obj].union_y=TILE_UNIT;
			if (object[demon_obj].y>=11*TILE_SIZE_Y && object[demon_obj].union_y>0) object[demon_obj].union_y=-TILE_UNIT;
		} /* if */ 

		if (object[demon_obj].state2>=256) {
			object[demon_obj].state=2;
			object[demon_obj].state2=0;
		} /* if */ 
		break;
	case 2:/* Después de disparar, momento de descanso: */ 
		if (object[demon_obj].state2<64) object[demon_obj].tile=1;
		if (object[demon_obj].state2>=64 && object[demon_obj].state2<96) object[demon_obj].tile=2;
		if (object[demon_obj].state2>=96 && object[demon_obj].state2<128) object[demon_obj].tile=1;
		if (object[demon_obj].state2>=128) {
			object[demon_obj].state=0;
			object[demon_obj].state2=8;
		} /* if */ 
		break;
	} /* switch */ 
} /* demon7_cycle */ 


/*									 */ 
/* ----------- DEMON 8 ------------- */ 
/*                                   */ 


void load_demon8(void)
{
	int i;

	if (demon_bmp!=0) delete demon_bmp;
	if (back_bmp!=0) delete back_bmp;
	demon_bmp=load_bitmap("demon8.pcx");
	back_bmp=load_bitmap("back8.pcx");

	for(i=0;i<8;i++) {
		if (tiles[demon_tile+i]!=0) delete tiles[demon_tile+i];
	} /* for */ 
	tiles[demon_tile]=new CTile(demon_bmp,0,0,TILE_SIZE_X*6,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+1]=new CTile(demon_bmp,6*TILE_SIZE_X,0,TILE_SIZE_X*6,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+2]=new CTile(demon_bmp,0,4*TILE_SIZE_Y,TILE_SIZE_X*6,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+3]=new CTile(demon_bmp,6*TILE_SIZE_X,4*TILE_SIZE_Y,TILE_SIZE_X*6,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+4]=new CTile(demon_bmp,0,8*TILE_SIZE_Y,TILE_SIZE_X*6,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+5]=new CTile(demon_bmp,6*TILE_SIZE_X,8*TILE_SIZE_Y,TILE_SIZE_X*6,TILE_SIZE_Y*4,T_ENEMY);
	tiles[demon_tile+6]=new CTile(demon_bmp,0,12*TILE_SIZE_Y,TILE_SIZE_X*6,TILE_SIZE_Y*4,T_ENEMY);
	
	tiles[demon_tile+7]=new CTile(demon_bmp,12*TILE_SIZE_X,0,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	if (fighting_demon==160) {
		demon_obj=add_object(T_ENEMY,TILE_SIZE_X*13,0,0,T_DEMON8,0);
		object[demon_obj].base_tile=demon_tile;
		if (world_item[1][0]) object[demon_obj].size=64;
						 else object[demon_obj].size=96;
		demon_max_life=object[demon_obj].size;
	} /* if */ 
} /* load_demon8 */ 

void demon8_cycle(void)
{
	if (object[demon_obj].size<(demon_max_life/2) && demon_music==0) {
		demon_music=1;
		Sound_subst_music_now("demon-fast");
	} /* if */ 
	if (object[demon_obj].size<(demon_max_life/4) && demon_music==1) {
		demon_music=2;
		Sound_subst_music_now("demon-vfast");
	} /* if */ 

	object[demon_obj].state2++;
			
	switch(object[demon_obj].state) {
	case 0:object[demon_obj].tile=(object[demon_obj].state2>>4)%2;
		  if (object[demon_obj].state2>=32) {
			  switch((rand()%80)/8) {
			  case 0:
			  case 1:
			  case 2:
			  case 3:
					object[demon_obj].state=1;
					object[demon_obj].state3=(4+(rand()%8))*16;
					break;
			  case 4:
			  case 5:
			  case 6:
			  case 7:
					object[demon_obj].state=2;
					object[demon_obj].state3=(4+(rand()%8))*16;
					break;
			  default:
					object[demon_obj].state=3;
			  } /* switch */ 
			  object[demon_obj].state2=0;
		  } /* if */ 
		break;
	case 1:object[demon_obj].tile=4+((object[demon_obj].state2>>2)%2);
		  object[demon_obj].state3--;
		  if (object[demon_obj].state3<0) {
			  object[demon_obj].state=3;
			  object[demon_obj].state2=0;
		  } else {
			  object[demon_obj].x+=TILE_UNIT/2;
			  if (object[demon_obj].x>=24*TILE_SIZE_X) object[demon_obj].state=2;
		  } /* if */ 
		  {
			  int pers_floor;

			  pers_floor=pers_y/(4*TILE_SIZE_Y);
			  if ((object[demon_obj].x==7*TILE_SIZE_X ||
				   object[demon_obj].x==19*TILE_SIZE_X) &&
				  (object[demon_obj].y!=pers_floor*4*TILE_SIZE_Y ||
				   (rand()%4)==0)) {
				  object[demon_obj].state=5;
				  if (object[demon_obj].y==pers_floor*4*TILE_SIZE_Y) pers_floor=rand()%4;
				  object[demon_obj].state3=pers_floor*4*TILE_SIZE_Y;
			  } /* if */ 
		  }
		break;
	case 2:object[demon_obj].tile=4+((object[demon_obj].state2>>2)%2);
		  object[demon_obj].state3--;
		  if (object[demon_obj].state3<0) {
			  object[demon_obj].state=3;
			  object[demon_obj].state2=0;
		  } else {
			  object[demon_obj].x-=TILE_UNIT/2;
			  if (object[demon_obj].x<=2*TILE_SIZE_X) object[demon_obj].state=1;
		  } /* if */ 
		  {
			  int pers_floor;

			  pers_floor=pers_y/(4*TILE_SIZE_Y);
			  if ((object[demon_obj].x==7*TILE_SIZE_X ||
				   object[demon_obj].x==19*TILE_SIZE_X) &&
				  (object[demon_obj].y!=pers_floor*4*TILE_SIZE_Y ||
				   (rand()%4)==0)) {
				  object[demon_obj].state=5;
				  if (object[demon_obj].y==pers_floor*4*TILE_SIZE_Y) pers_floor=rand()%4;
				  object[demon_obj].state3=pers_floor*4*TILE_SIZE_Y;
			  } /* if */ 
		  } 
		break;
	case 3:object[demon_obj].tile=6;
		if (object[demon_obj].state2>=16) {
			object[demon_obj].state=4;
			object[demon_obj].state2=0;
		} /* if */ 
		/* Quieto: */ 
		break;
	case 4:object[demon_obj].tile=2;
		if (object[demon_obj].state2>=48) {
			object[demon_obj].state=0;
			object[demon_obj].state2=0;
		} /* if */ 
		/* Disparar: */ 
		if (object[demon_obj].state2==32 ||
			object[demon_obj].state2==40) {
			int j;

			if (object[demon_obj].state2==32) Sound_play(S_demon8bullet);
			j=add_object(T_ENEMY,object[demon_obj].x+1*TILE_SIZE_X,object[demon_obj].y,1,T_DEMON8_BULLET,0);
			object[j].base_tile=demon_tile;			
			object[j].tile=7;
			object[j].state=rand()%2;
			j=add_object(T_ENEMY,object[demon_obj].x+3*TILE_SIZE_X,object[demon_obj].y,1,T_DEMON8_BULLET,0);
			object[j].base_tile=demon_tile;			
			object[j].tile=7;
			object[j].state=2+(rand()%2);
		} /* if */ 
		break;
	case 5:object[demon_obj].tile=3;
		/* Subir o bajar: */ 
		if (object[demon_obj].y<object[demon_obj].state3) {
			object[demon_obj].y+=TILE_UNIT*2;
		} else {
			if (object[demon_obj].y>object[demon_obj].state3) {
				object[demon_obj].y-=TILE_UNIT*2;
			} else {
				object[demon_obj].state=0;
				object[demon_obj].state2=0;
			} /* if */ 
		} /* if */ 
		break;

	} /* switch */ 
} /* demon8_cycle */ 


/*									 */ 
/* ----------- DEMON 9 ------------- */ 
/*                                   */ 


void load_demon9(void)
{
	int i;

	if (demon_bmp!=0) delete demon_bmp;
	if (back_bmp!=0) delete back_bmp;
	demon_bmp=load_bitmap("demon9.pcx");
	back_bmp=load_bitmap("back9.pcx");

	for(i=0;i<9;i++) {
		if (tiles[demon_tile+i]!=0) delete tiles[demon_tile+i];
	} /* for */ 
	tiles[demon_tile]=new CTile(demon_bmp,0,0,TILE_SIZE_X*15,TILE_SIZE_Y*9,T_ENEMY);
	tiles[demon_tile+1]=new CTile(demon_bmp,15*TILE_SIZE_X,0,TILE_SIZE_X*15,TILE_SIZE_Y*9,T_ENEMY);
	tiles[demon_tile+2]=new CTile(demon_bmp,30*TILE_SIZE_X,0,TILE_SIZE_X*15,TILE_SIZE_Y*9,T_ENEMY);
	tiles[demon_tile+3]=new CTile(demon_bmp,0,TILE_SIZE_Y*9,TILE_SIZE_X*15,TILE_SIZE_Y*9,T_ENEMY);
	tiles[demon_tile+4]=new CTile(demon_bmp,15*TILE_SIZE_X,TILE_SIZE_Y*9,TILE_SIZE_X*15,TILE_SIZE_Y*9,T_ENEMY);
	tiles[demon_tile+5]=new CTile(demon_bmp,30*TILE_SIZE_X,TILE_SIZE_Y*9,TILE_SIZE_X*15,TILE_SIZE_Y*9,T_ENEMY);
	
	tiles[demon_tile+6]=new CTile(demon_bmp,0*TILE_SIZE_X,18*TILE_SIZE_Y,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_ENEMY);
	tiles[demon_tile+7]=new CTile(demon_bmp,2*TILE_SIZE_X,18*TILE_SIZE_Y,TILE_SIZE_X*1,TILE_SIZE_Y*1,T_ENEMY);
	tiles[demon_tile+8]=new CTile(demon_bmp,3*TILE_SIZE_X,18*TILE_SIZE_Y,TILE_SIZE_X*2,TILE_SIZE_Y*1,T_ENEMY);
 
	if (fighting_demon==160) {
		demon_obj=add_object(T_ENEMY,TILE_SIZE_X*12,TILE_SIZE_Y*10,0,T_DEMON9,0);
		object[demon_obj].base_tile=demon_tile;
		object[demon_obj].tile=2;
		if (world_item[1][0]) object[demon_obj].size=96;
						 else object[demon_obj].size=160;
		demon_max_life=object[demon_obj].size;
		for(i=0;i<4;i++) {
			int o=add_object(T_ENEMY,TILE_SIZE_X*(8+4*i),TILE_SIZE_Y,0,T_LCLOUD,0);
			object[o].state=(rand()%16)/10;
			object[o].state2=0;
			object[o].last_hit=0;
			object[o].base_tile=lcloud_tile;
			object[o].tile=0;
		} /* if */ 
	} /* if */  
} /* load_demon9 */ 

void demon9_cycle(void)
{
	int seq[4]={2,1,0,1};

	if (object[demon_obj].size<(demon_max_life/2) && demon_music==0) {
		demon_music=1;
		Sound_subst_music_now("demon-fast");
	} /* if */ 
	if (object[demon_obj].size<(demon_max_life/4) && demon_music==1) {
		demon_music=2;
		Sound_subst_music_now("demon-vfast");
	} /* if */ 

	switch(object[demon_obj].state) {
	case 0:
		object[demon_obj].state2++;
		object[demon_obj].tile=seq[(object[demon_obj].state2>>4)%4];
		if ((object[demon_obj].state2&0x01)==0) object[demon_obj].x-=TILE_UNIT/2;
		if (object[demon_obj].x<=9*TILE_SIZE_X) object[demon_obj].state=1;
		if ((object[demon_obj].state2%0x00f)==0 &&
			(rand()%100)<50) {
			object[demon_obj].state3=64;
			object[demon_obj].state+=2;
		} /* if */ 
		break;
	case 1:
		object[demon_obj].state2++;
		object[demon_obj].tile=seq[(object[demon_obj].state2>>4)%4];
		if ((object[demon_obj].state2&0x01)==0) object[demon_obj].x+=TILE_UNIT/2;
		if (object[demon_obj].x>=15*TILE_SIZE_X) object[demon_obj].state=0;
		if ((object[demon_obj].state2%0x00f)==0 &&
			(rand()%100)<50) {
			object[demon_obj].state3=64;
			object[demon_obj].state+=2;
		} /* if */ 
		break;
	case 2:
	case 3:
		object[demon_obj].state3--;
		if (object[demon_obj].state3<=0) {
			object[demon_obj].state-=2;
		} else {
			if ((object[demon_obj].state3&0x0f)==0) {
				int dx[3]={-TILE_SIZE_X*3,-TILE_SIZE_X*3,-TILE_SIZE_X};
				int dy[3]={7*TILE_SIZE_Y,3*TILE_SIZE_Y,3*TILE_SIZE_Y};
				add_object(T_ENEMY,object[demon_obj].x+dx[object[demon_obj].tile],
								   object[demon_obj].y+dy[object[demon_obj].tile],0,T_DEMON9_FIRE,0);
				Sound_play(S_firebreath);
			} /* if */ 
		} /* if */ 
		break;
	} /* switch */ 
} /* demon9_cycle */ 


/*									 */ 
/* ----------- DEMON 10 ------------- */ 
/*                                   */ 


void load_demon10(void)
{
	int i;

	if (demon_bmp!=0) delete demon_bmp;
	if (back_bmp!=0) delete back_bmp;
	demon_bmp=load_bitmap("demon10.pcx");
	back_bmp=load_bitmap("back10.pcx");

	for(i=0;i<9;i++) {
		if (tiles[demon_tile+i]!=0) delete tiles[demon_tile+i];
	} /* for */ 
	tiles[demon_tile]=new CTile(demon_bmp,0,0,TILE_SIZE_X*7,TILE_SIZE_Y*7,T_ENEMY);
	tiles[demon_tile+1]=new CTile(demon_bmp,7*TILE_SIZE_X,0,TILE_SIZE_X*7,TILE_SIZE_Y*7,T_ENEMY);
	tiles[demon_tile+2]=new CTile(demon_bmp,14*TILE_SIZE_X,0,TILE_SIZE_X*7,TILE_SIZE_Y*7,T_ENEMY);
	tiles[demon_tile+3]=new CTile(demon_bmp,21*TILE_SIZE_X,0,TILE_SIZE_X*7,TILE_SIZE_Y*7,T_ENEMY);

	tiles[demon_tile+4]=new CTile(demon_bmp,0,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+5]=new CTile(demon_bmp,2*TILE_SIZE_X,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+6]=new CTile(demon_bmp,4*TILE_SIZE_X,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+7]=new CTile(demon_bmp,6*TILE_SIZE_X,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);
	tiles[demon_tile+8]=new CTile(demon_bmp,8*TILE_SIZE_X,TILE_SIZE_Y*7,TILE_SIZE_X*2,TILE_SIZE_Y*2,T_ENEMY);

	if (fighting_demon==160) {
		demon_obj=add_object(T_ENEMY,TILE_SIZE_X*12,TILE_SIZE_Y*4,0,T_DEMON10,0);
		object[demon_obj].base_tile=demon_tile;
		object[demon_obj].tile=1;
		if (world_item[1][0]) object[demon_obj].size=128;
						 else object[demon_obj].size=196;
		demon_max_life=object[demon_obj].size;

		for(i=0;i<8;i++) {
			int o=add_object(T_ENEMY,TILE_SIZE_X*15-TILE_SIZE_X/2,TILE_SIZE_Y*7,16,T_DEMON10_BALL,0);
			if (i==0) object[demon_obj].state3=o;
			object[o].last_hit=0;
			object[o].base_tile=demon_tile;
			object[o].tile=4;
			object[o].state2=i*4;
			object[o].state3=TILE_SIZE_X*6;
		} /* for */ 
	} /* if */ 
} /* load_demon10 */ 

void demon10_cycle(void)
{
	object[demon_obj].state2++;
			
	if (object[demon_obj].size<(demon_max_life/2) && demon_music==0) {
		demon_music=1;
		Sound_subst_music_now("demon-fast");
	} /* if */ 
	if (object[demon_obj].size<(demon_max_life/4) && demon_music==1) {
		demon_music=2;
		Sound_subst_music_now("demon-vfast");
	} /* if */ 

	switch(object[demon_obj].state) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		object[demon_obj].tile=1;
		if (object[demon_obj].state2>=48) {
			int i;
			bool found=false;

			object[demon_obj].state+=6;
			object[demon_obj].state2=0;

			for(i=0;i<8;i++) if (object[i+object[demon_obj].state3].type==T_ENEMY) found=true;
			if (!found) object[demon_obj].state=12;
		} /* if */ 
		break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		object[demon_obj].tile=0;
		if (object[demon_obj].state2>16) {
			int i;
			for(i=0;i<8;i+=2) {
				if (object[i+object[demon_obj].state3].type==T_ENEMY) {
					if (object[demon_obj].state2<25) {
						object[i+object[demon_obj].state3].state3+=TILE_SIZE_X;
					} else {
						object[i+object[demon_obj].state3].state3-=TILE_SIZE_X;
					} /* if */ 
				} /* if */ 
			} /* for */ 
		}
		if (object[demon_obj].state2>=32) {
			object[demon_obj].state-=5;
			object[demon_obj].state2=0;
		} /* if */ 

		break;
	case 11:
	case 13:
		{
			object[demon_obj].tile=-1;

			int i;
			for(i=0;i<8;i++) {
				if (object[i+object[demon_obj].state3].type==T_ENEMY) {
					object[i+object[demon_obj].state3].state=1;
				} /* if */ 
			} /* for */ 

			if (object[demon_obj].state2==64) {
				int x[5]={12,8,12,16,22};
				int y[5]={ 4,8, 0, 0, 8};
				i=rand()%5;
				object[demon_obj].x=x[i]*TILE_SIZE_X;
				object[demon_obj].y=y[i]*TILE_SIZE_Y;
			} /* if */ 

			if (object[demon_obj].state2>=64) {
				object[demon_obj].tile=-1+2*((object[demon_obj].state2>>2)%2);
				if (object[demon_obj].state2>=95) {
					if (object[demon_obj].state==13) object[demon_obj].state=12;
											    else object[demon_obj].state=0;
					object[demon_obj].state2=0;
					for(i=0;i<8;i++) {
						if (object[i+object[demon_obj].state3].type==T_ENEMY) {
							object[i+object[demon_obj].state3].x=object[demon_obj].x+2*TILE_SIZE_X+TILE_SIZE_X/2;
							object[i+object[demon_obj].state3].y=object[demon_obj].y+3*TILE_SIZE_Y;
							object[i+object[demon_obj].state3].state=0;
						} /* if */ 
					} /* for */ 
				} /* if */ 
			} /* if */ 
		}
		break;
	case 12:
		object[demon_obj].tile=1;
		if (object[demon_obj].state2>=128) {
			object[demon_obj].state=13;
			object[demon_obj].state2=0;
		} /* if */ 
		break;
	} /* switch */ 
} /* demon10_cycle */ 
