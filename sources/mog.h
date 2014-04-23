#ifndef __BRAIN_MOG
#define __BRAIN_MOG

#define MAX_EXPERIENCE	96

#define MAX_TILES		1024
#define MAX_OBJECTS		256

#define MAX_AW_RECORD	32

#define TILE_SIZE_X		16
#define TILE_SIZE_Y		16
#define GAME_VIEW_X		64
#define GAME_VIEW_Y		64
#define TILE_UNIT		2


#define T_WALL			1
#define T_WATER			2
#define T_LADDER_WALL	4
#define T_DOOR_WALL		8
#define T_PLAYER		16
#define T_WEAPON		32
#define T_ENEMY			64
#define T_LAVA		    128

#define T_NADA		    0
#define T_LADDER		2
#define T_WDOOR			10
#define T_STONE			20
#define T_BROKEN_STONE	30
#define T_KEYDOOR		40
#define T_EXPLOSION		50
#define T_DOOR			60
#define T_ITEM		    70
#define T_WEAPON_ARROW	80
#define T_ARMOUR		90
#define T_TOMB			100
#define T_WKEYDOOR		110
#define T_WNOKEYDOOR	120
#define T_WEAPON_ARROW2	130
#define T_FAIRY_BUBBLE	140
#define T_FAIRY			150
#define T_HITWALL		160
#define T_WNOKEYDOOR2	170
#define T_WEAPON_FIRE	180
#define T_WEAPON_MINE	190
#define T_MINE_EXP		200
#define T_RFL_ARROW		210
#define T_RFL_ARROW2	220
#define T_RFL_FIRE		230
#define T_WEAPON_RFIRE	240
#define T_RFL_RFIRE		250
#define T_WNOKEYDOOR3	260
#define T_PAMPERSE		270


#define T_WORM				1
#define T_BAT				2
#define T_SKELETON			3
#define T_SMOKE				4
#define T_SLIME				5
#define T_BOUNCINGBALL		6
#define T_WATERMONSTER		7
#define T_WATERMONSTER_ARM	8
#define T_BUBBLE			9
#define T_JUMPINGBUSH		10	
#define T_BLUESPIDER		11
#define T_WHITEFIREBALL		12
#define T_FSTONE			13
#define T_RSTONE			14
#define T_KNIGHT			15
#define T_BLOB				16
#define T_BAMBU				17
#define T_PORCUPINE			18
#define T_PORCUPINE_BULLET	19
#define T_BLACK				20
#define T_WITCH				21
#define T_WHITEBEAR			22
#define T_WHITEBEAR_BULLET	23
#define T_FEET				24
#define T_REDJUMPER			25
#define T_VENT2				26
#define T_LIVINGWALL		27
#define T_MEGABAT			28
#define T_MEGABAT2			29
#define T_MEGABAT3			30
#define T_LAVA1				31
#define T_LAVA2				32
#define T_LAVA3				33
#define T_PIRANHA			34
#define T_PIRANHA2			35
#define T_WHITESTAR			36
#define T_SPIDER			37
#define T_KNIGHTHEAD		38
#define T_CHICKEN			39
#define T_CHICKEN_EGG		40
#define T_EGG_EXPLOSION		41
#define T_ROCKMAN			42
#define T_CLOUD				43
#define T_BFLY_SMALL		44
#define T_BFLY_MEDIUM		45
#define T_BFLY_LARGE		46
#define T_BFLY_GIANT		47
#define T_BFLY_BULLET		48
#define T_ARMOUR_ARROW		49
#define T_GHOST				50
#define T_GHOST_BULLET		51
#define T_HEAD				52
#define T_HEAD_BULLET		53
#define T_WORM2				54
#define T_OCTOPUS			55
#define T_HAIRY				56
#define T_HAIRYBULLET		57
#define T_WATERDRAGON		58
#define T_WATERBUG			59
#define T_BIRD				60
#define T_STONEMAN			61
#define T_STONEMAN_BULLET	62
#define T_PACMAN			63
#define T_REDDRAGON			64
#define T_REDDRAGON_BULLET	65
#define T_OWL				66
#define T_GREENMONKEY		67
#define T_PLANT				68
#define T_PLANT_BULLET		69
#define T_TRANSFORMER		70
#define T_TRANSF_BULLET		71
#define T_FLAME				72
#define T_FLAME_BULLET		73
#define T_WITCH2			74
#define T_CYCLOPS			75
#define T_CYCLOPS_BULLET	76
#define T_LCLOUD			77
#define T_LIGHTNING			78
#define T_LIGHTNING_FIRE	80
#define T_SNAKE				81
#define T_SNAKE_BULLET		82
#define T_GLIZARD			83
#define T_GLIZARD_TONGUE	84
#define T_GORILLA			85
#define T_GORILLA_ARM		86
#define T_BDEMON			87
#define T_BDEMON_BULLET		88
#define T_PAMPERSE_BALL		89

#define T_DEMON1			101
#define T_DEMON1_BONES		102
#define T_DEMON1_BONES2		103
#define T_DEMON2			201
#define T_DEMON2_BALLS		202
#define T_DEMON2_HEAD		203
#define T_DEMON3			301
#define T_DEMON3_FIRE		302
#define T_DEMON4			401
#define T_DEMON5			501
#define T_DEMON5_FIRE		502
#define T_DEMON6			601
#define T_DEMON6_BULLET		602
#define T_DEMON7			701
#define T_DEMON7_BULLET		702
#define T_DEMON8			801
#define T_DEMON8_BULLET		802
#define T_DEMON9			901
#define T_DEMON9_FIRE		902
#define T_DEMON10		   1001
#define T_DEMON10_BALL	   1002


#define S_QUIET				0
#define S_WALKING_RIGHT		1
#define S_WALKING_LEFT		2
#define S_UNCONTROLED_FALL	3
#define S_CONTROLED_FALL	4
#define S_JUMPING			5
#define S_STOPPED_JUMPING	6
#define S_QUIET_SWORD		7
#define S_UC_FALL_SWORD		8
#define S_C_FALL_SWORD		9
#define S_JUMPING_SWORD		10
#define S_SJ_SWORD		    11
#define S_UP_LADDER			12
#define S_DOWN_LADDER		13
#define S_QUIET_LADDER		14
#define S_SWORD_LADDER		15
#define S_HIT				16
#define S_HIT_COLISION		17
#define S_HIT_RECOVERY		18
#define S_DEATH				19
#define S_DOORPASS			20
#define S_ENTERING_PASSAGE	21
#define S_ENTERING_WORLD	22
#define S_SLOWDOORPASS		23


#define MASK_OFFSET			18

#define HIT_TIME			32

#define FIRE_ARROW_PERIOD	4
#define HIT_PERIOD			4

#define MAX_LEVERS			21


/* Prototipos de funciones: */ 

void init_paths(void);
void newgame(void);
void fire_arrow();
int  test_if_weapon_colision(int tile,int tx,int ty,int *old=0);
void GameObjectAnimation(int dx,int dy); 
void ActualizeLevers(int dx,int dy);
void RoomChange(void);
bool readpassword(char *passwd);
void get_palette(void);

void HP_objs_reset(void);
void HP_filter(int room_x,int room_y);
void HP_restore(int room_x,int room_y);
void HP_add_record(int room_x,int room_y,int obj,int type);
void HP_modify_record_aux(int room_x,int room_y,int obj,int aux);

void SetSFXVolume(int volume);
void ReleaseGraphics(void);
void ReleaseSound(bool definitive);
void ReloadGraphics(int dx,int dy);
void ReloadSound(void);
void ReloadStuff(int dx,int dy);
void GameInit(int dx,int dy);
void GameEnd(void);
void drawcharacter(BYTE *screen,int dx,int dy);
void drawmap(BYTE *screen,int dx,int dy,int type);
void drawstats(BYTE *screen,int dx,int dy);
void draw_items_map(int map,int map_x,int map_y,bool selfindicator,unsigned char *screen,int dx,int dy);
bool loadroom(int map,int map_x,int map_y);
void tile_print(char *str,int x,int y,BYTE *screen,int dx,int dy);
void tile_print2(char *str,int x,int y,BYTE *screen,int dx,int dy);
int  search_ladder(int x,int y);
int  add_object(int type,int x,int y,int size,int enemy,int state);
bool iswall(int x,int y);
bool iswall_or_blockingobject(int x,int y);

void guardar_configuracion(char *filename);
bool cargar_configuracion(char *filename);
void configuracion_por_defecto(void);

void HP_objs_reset(void);
void HP_filter(int room_x,int room_y);
void HP_restore(int room_x,int room_y);
void HP_add_record(int room_x,int room_y,int obj,int type);
void clear_typed_word(void);
void check_typed_word(void);
bool typed_word_p(char *word);

void redo_demonintro(int state,unsigned char *screen,int dx,int dy);
void demon_intro(int state,unsigned char *screen,int dx,int dy);
void load_demon1(void);
void demon1_cycle(void);
void load_demon2(void);
void demon2_cycle(void);
void load_demon3(void);
void demon3_cycle(void);
void load_demon4(void);
void demon4_cycle(void);
void load_demon5(void);
void demon5_cycle(void);
void load_demon6(void);
void demon6_cycle(void);
void load_demon7(void);
void demon7_cycle(void);
void load_demon8(void);
void demon8_cycle(void);
void load_demon9(void);
void demon9_cycle(void);
void load_demon10(void);
void demon10_cycle(void);

Bitmap *load_bitmap(char *name);

void draw_demon_background(BYTE *screen,int dx,int dy,int pitch);

void GameCycle(BYTE *screen,int dx,int dy);
void GameInGameCycle(int dx,int dy);
void GameTestInteractions(int dx,int dy);
void GameRoomEvents();
void fire_arrow();
void tile_anims(Bitmap *tiles,int dx,int dy);
void passage_mainloop(int map,int map_x,int map_y,unsigned char *screen,int dx,int dy);

void generatepassword(char *passwd);
bool readpassword(char *passwd);

void write_debug_report(char *file);

bool IsAltPressed();
bool IsAltPressed2();

#endif

