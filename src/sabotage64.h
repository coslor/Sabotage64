#include "sabotage64_memory.h"
#include "SIDFX.h"
#include <c64/sprites.h>
#include <c64/rasterirq.h>
#include <c64/vic.h>
#include <c64/memmap.h>
#include <c64/cia.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>		//for memset()
#include <time.h>
#include <c64/joystick.h>
#include <c64/keyboard.h>
#include <audio/sidfx.h>
#include <string.h>
#include <math.h>

#define VSPRITES_MAX		22	//32
#define NUM_IRQS			30	//40

#define MAX_TROOPERS 		8
#define NUM_CHUTES			MAX_TROOPERS
#define MAX_NUM_BULLETS 	4
//actual bullet speed is this value /64
#define BULLET_SPEED		120

//If defined, all shots automatically hit
//#define PERFECT_SHOT

#define SPRITE_OFFSET		0x80
#define CHUTE_SPRITE		SPRITE_OFFSET+0
#define TROOPER_SPRITE		SPRITE_OFFSET+1
#define BULLET_SPRITE		SPRITE_OFFSET+18
#define BARREL_SPRITE_OFFSET SPRITE_OFFSET+9

/** VSprite # offsets. VSprites are stored as:
*	RTT..TCC..CBB..B
*	...where T are troopers, C are chutes, B are bullets, and R is the barrel
**/

#define VS_TROOPER_OFFSET	1
#define VS_CHUTE_OFFSET		VS_TROOPER_OFFSET + MAX_TROOPERS
#define VS_BULLET_OFFSET	VS_CHUTE_OFFSET + NUM_CHUTES
#define VS_BARREL_OFFSET	0


#define BULLET_COLOR		VCOL_WHITE
#define TROOPER_COLOR		VCOL_GREEN
#define BARREL_COLOR		VCOL_LT_GREY
#define CHUTE_COLOR			VCOL_WHITE

#define BARREL_X			176
#define BARREL_Y			189

//Can you steer the shells after they've been shot?
//#define STEERABLE_BULLETS

//speed is fractional, as in n/64
#define TROOPER_CHUTE_SPEED	20
#define TROOPER_NO_CHUTE_SPEED 36


/*** Screen codes  ***/
const byte TROOPER_CHAR	=152;
const byte EMPTY_CHAR	=0x20;
const byte GROUND_CHAR	=0xa0;
const byte BUILDING_CHAR=0x80;
const byte SMOOSHED_CHAR=153;
//half-square used on screen to give the "SCORE" display a little more room
const byte HALF_GROUND_SQUARE=0xf9;	

const byte SCREEN_CODE_0=48;

const byte TROOPER_VALUE=1;

const byte* SCREEN_POS=(byte *)(SCREEN_LOC+992);

/** HIBASE points to the current text screen for the Kernal. High byte only. */
byte *HIBASE=(byte *)648;

/**
 * 6-bit fixed-point sin,cos values. To calculate, use new_loc=old_loc+(speed*sin/cos[direction from 0-63])/64
 */
signed char short_sin[64] = {
	0,6,12,18,24,29,35,39,44,48,52,55,58,60,61,62,63,
	62,61,60,58,55,52,48,44,39,35,29,24,18,12,6,0,
	-6,-12,-18,-24,-29,-35,-39,-44,-48,-52,-55,-58,-60,-61,-62,-63,
	-62,-61,-60,-58,-55,-52,-48,-44,-39,-35,-29,-24,-18,-12,-6
};

signed char short_cos[64] = {
	63,62,61,60,58,55,52,48,44,39,35,29,24,18,12,6,0,
	-6,-12,-18,-24,-29,-35,-39,-44,-48,-52,-55,-58,-60,-61,-62,-63,
	-62,-61,-60,-58,-55,-52,-48,-44,-39,-35,-29,-24,-18,-12,-6,0,
	6,12,18,24,29,35,39,44,48,52,55,58,60,61,62
};

typedef enum MOBType{
	SOLDIER,PARACHUTE,BULLET
} MobType;

typedef enum {
	GS_INITIAL_START, GS_WELCOME, GS_STARTING_GAME, GS_START_LEVEL, GS_RUNNING, GS_STOPPING, GS_ENDING
} GameState;
GameState game_state=GS_INITIAL_START;


/*** Level Stuff *****/
#define NUM_LEVELS			1
typedef struct Level {
	//max # of troopers onscreen at once
	byte 	max_troopers;
	//how closely the troopers are clustered together; higher #s mean further apart
	byte 	max_trooper_clock;
	byte 	num_bullets;
	char	message[32];
	byte	barrel_color;
} Level;

Level levels[] = {
	{
		4,
		90,
		4,
		s"this is the first level",
		VCOL_DARK_GREY
	}
};
byte current_level;


/****Fixed Point 9.6 Stuff ****/
typedef signed int fx_96;

#define TO_INT(n) (int)(n>>6)
#define TO_FX96(n) (fx_96)(n<<6)

/**** MOB Stuff  ****/

// typedef struct Box {
// 	fx_96			x;
// 	fx_96			y;
// 	fx_96			end_x;
// 	fx_96			end_y;
// } Box;

typedef struct MOB {
	byte			vsprite_num;
    bool            active=false;
	//MobType 		type;
	//Box				box;
	fx_96			x;
	fx_96			y;
	fx_96			end_x;
	fx_96			end_y;
	fx_96 			speed_x=0;
	fx_96 			speed_y=0;
	// byte			width;
	// byte			height;
	bool 			has_chute=false;
} MOB;


/****Method Signatures****/

void show_game_screen();

void clear_troopers();
void drop_trooper(byte trooper_num, byte vsprite_num, fx_96 x, fx_96 y, fx_96 speed_y);
void move_troopers();
inline int calc_screen_offset(int x, int y);
void land_trooper(char trooper_num, int screen_loc);
void smoosh_trooper(char trooper_num, int screen_loc);
void stop_trooper(char trooper_num);
byte find_trooper(bool active);
void add_troopers();

void init_bullets();
void fire_bullet(MOB* bullet, fx_96 x, fx_96 y, byte direction, fx_96 speed);
void move_bullets();
void check_bullet_collisions();
inline bool point_is_in_box(int x, int y, int bx, int by, int bx_end, int by_end);

void init_barrel();
void draw_barrel();

void handle_inputs();
bool fire_bullet_now();

void kill_trooper(byte trooper_num);
void kill_bullet(byte bullet_num);
/** @param num is the trooper num **/
void kill_chute(byte trooper_num);

void steer_bullets();

void clear_all();

/** Game States **/
void initial_start();
void run_game();

void update_vsprites();

void init_sid();
void play_sid();

long set_score(long val);
long inc_score(long val);
void update_onscreen_score();

/**Prints & centers a given message. Message should be in screen codes. */
void center_message(const char *message, byte row);
void wait_for_fire();
void erase_message(byte row);