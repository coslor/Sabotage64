#include "sabotage64_memory.h"
#include "SIDFX.h"
#include <c64/sprites.h>
#include <c64/rasterirq.h>
#include <c64/vic.h>
#include <c64/memmap.h>
#include <c64/cia.h>	//for cia_init()
#include <stdio.h>
#include <string.h>		//for memset()
#include <c64/joystick.h>
#include <c64/keyboard.h>
#include <audio/sidfx.h>
#include <string.h>
#include <oscar.h>	//for oscar_expand_lzo()

//These have to be DEFINES (I think?)
#define VSPRITES_MAX		22	//32
#define NUM_IRQS			30	//40

/****Fixed Point 9.6 Stuff ****/
typedef signed int fx_96;

#define TO_INT(n) (int)(n>>6)
#define TO_FX96(n) (fx_96)(n<<6)

const byte MAX_TROOPERS			=8;
const byte MAX_CHUTES			=MAX_TROOPERS;
const byte MAX_BULLETS			=4;
//actual bullet speed is this value /64
const fx_96 BULLET_SPEED		=120;

const byte MAX_HELIS			=6;

//If defined, all shots automatically hit
//#define PERFECT_SHOT

const byte SPRITE_OFFSET		=0x80;
const byte CHUTE_SPRITE			=SPRITE_OFFSET+0;
const byte TROOPER_SPRITE		=SPRITE_OFFSET+1;
const byte BARREL_SPRITE		=SPRITE_OFFSET+9;
const byte BULLET_SPRITE		=SPRITE_OFFSET+18;
const byte HELI_SPRITE			=SPRITE_OFFSET+16;

/** VSprite # offsets. VSprites are stored as:
*	RTT..TCC..CBB..BHHHHH
*	...where T are troopers, C are chutes, B are bullets, and R is the barrel
*	...and H are the helicopters
**/

const byte VS_TROOPER_OFFSET	=1;
const byte VS_CHUTE_OFFSET		=VS_TROOPER_OFFSET + MAX_TROOPERS;
const byte VS_BULLET_OFFSET		=VS_CHUTE_OFFSET + MAX_CHUTES;
const byte VS_BARREL_OFFSET		=0;

const enum VICColors BULLET_COLOR	=VCOL_WHITE;
const enum VICColors TROOPER_COLOR	=VCOL_DARK_GREY;
const enum VICColors CHUTE_COLOR	=VCOL_WHITE;

//In pixels
const byte BARREL_X				=176;
//In pixels
const byte BARREL_Y				=189;

//Can you steer the shells after they've been shot?
//#define STEERABLE_BULLETS

//speed is fractional, as in n/64
const fx_96 TROOPER_CHUTE_SPEED	=20;
//speed is fractional, as in n/64
const fx_96 TROOPER_NO_CHUTE_SPEED=36;

/*** Screen codes  ***/
const byte TROOPER_CHAR			=152;
const byte EMPTY_CHAR			=0x20;
const byte GROUND_CHAR			=0xa0;
const byte BUILDING_CHAR		=0x80;
const byte SMOOSHED_CHAR		=153;
//half-square used on screen to give the "SCORE" display a little more room
const byte HALF_GROUND_SQUARE=	0xf9;	

const byte SCREEN_CODE_0=		48;

const byte TROOPER_SCORE_VALUE=		1;

const byte* SCREEN_POS=(byte *)(SCREEN_LOC+992);

///** HIBASE points to the current text screen for the Kernal. High byte only. */
//byte *HIBASE=(byte *)648;

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

// typedef enum MOBType{
// 	SOLDIER,PARACHUTE,BULLET
// } MobType;

typedef enum {
	GS_INITIAL_START, GS_WELCOME, GS_STARTING_GAME, GS_START_LEVEL, GS_RUNNING, 
	GS_LEVEL_ENDING, GS_STOPPING, GS_ENDING, GS_GAME_OVER
} GameState;
GameState game_state=GS_INITIAL_START;


/*** Level Stuff *****/
#define NUM_LEVELS			1
typedef struct Level {
	//max # of troopers onscreen at once
	const byte 	max_troopers;
	//how closely the troopers are clustered together; higher #s mean further apart. Stay at >=50.
	const byte 	max_trooper_clock;
	//how many troopers total should we drop on this level?
	const byte	num_troopers;
	const byte 	num_bullets;
	//NOTE: if you exceed the max length for a message, bad things happen!
	const char	message[2][39];
	//Should we show a message & require fire to continue?
	const bool	pause_for_msg;
	const byte	barrel_color;
	//Clear all landed troopers before starting?
	const bool	clear_before_starting;
	//"Pointer" to next level
	const byte 	next_level_num;
} Level;

Level levels[] = {
	//Level 0 has 4 max troopers, 4 bullets
	{									//#0
		5,								//max_troopers
		90,								//max_trooper_clock
		8,								//num_troopers
		4,								//num_bullets
		{
		   //0123456789012345678901234567890123456789
			s"we can't let the enemy overtake this",	//message
			s"position. it's up to you, soldier!"	//message 2
		},
		true,							//pause for message?
		VCOL_DARK_GREY,					//barrel color
		true,							//clear all troopers before starting?
		1								//next level #
	},

	//Level 1 extends level 0 with no message, 6 troopers, 4 bullets
	{									//#1
		7,								//max_troopers
		90,								//max_trooper_clock
		8,								//num_troopers
		4,								//num_bullets
		{
		   //0123456789012345678901234567890123456789
			s"",	//message
			s""	//message 2
		},
		false,							//pause for message?
		VCOL_DARK_GREY,					//barrel color
		false,							//clear all troopers before starting?
		2								//next level #
	},

	//Level 2: 6 troopers, but troopers come faster and only 3 bullets
	{									//#2
		6,								//max_troopers
		80,								//max_trooper_clock
		12,								//num_troopers
		3,								//num_bullets
		{
		   //0123456789012345678901234567890123456789
			s"your gun is getting warm, so your shot",		//message
			s"rate will be reduced. be careful!"
		},
		true,
		VCOL_LT_GREY,
		false,
		3
	},

	//Level 3: 3 bullets, 8 troopers, faster release
	{									//#3
		8,								//max_troopers
		70,								//max_trooper_clock
		12,								//num_troopers
		3,								//num_bullets
		{
		   //0123456789012345678901234567890123456789
			s"",		//message
			s""
		},
		true,
		VCOL_LT_GREY,
		false,
		4
	},
	//Level 4: 2 bullets, 6 troopers, faster release
	{									//#4
		7,								//max_troopers
		60,								//max_trooper_clock
		16,								//num_troopers
		2,								//num_bullets
		{
		   //0123456789012345678901234567890123456789
			s"the action's getting really hot, as is",		//message
			s"your gun. watch yourself!"
		},
		true,
		VCOL_RED,
		false,
		5
	},

	{
										//#5
		8,
		50,
		18,
		1,
		{
		   //0123456789012345678901234567890123456789
			s"hold out as long as you can-your brave",
			s"sacrifice will never be forgotten."

		},
		true,
		VCOL_LT_RED,
		false,
		6
	},
	{
		8,								//#6
		50,
		20,
		1,
		{
			s"",
			s""

		},
		false,
		VCOL_RED,
		false,
		6	//points to itself
	}
};
byte current_level;


typedef struct MOB {
	byte			vsprite_num;
    bool            active=false;
	fx_96			x;
	fx_96			y;
	fx_96			end_x;
	fx_96			end_y;
	fx_96 			speed_x=0;
	fx_96 			speed_y=0;
	bool 			has_chute=false;
} MOB;

const char PRESS_FIRE_MSG[]=s"press fire or space to continue";


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

byte count_landed_troopers(byte start_row, byte start_col, byte end_row, byte end_col);
void reset_landed_trooper_color(byte new_color);

void show_messages(const char const *msg1, const char* msg2);
byte petscii_to_screen_char(byte c);
void petscii_to_screen_str(char *msg, int len);

void show_title_screen();
void show_welcome_screen();