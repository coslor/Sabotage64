#include "satobage_memory.h"
#include <c64/sprites.h>
#include <c64/rasterirq.h>
#include <c64/vic.h>
#include <c64/memmap.h>
#include <c64/cia.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>		//for memset()
#include <time.h>

#define TOTAL_MOBS 		16
#define NUM_TROOPERS 	4
#define NUM_BULLETS 	4

#define SPRITE_OFFSET	48

signed char short_sin[] = {
	0,6,12,18,24,29,35,39,44,48,52,55,58,60,61,62,63,
	62,61,60,58,55,52,48,44,39,35,29,24,18,12,6,0,
	-6,-12,-18,-24,-29,-35,-39,-44,-48,-52,-55,-58,-60,-61,-62,-63,
	-62,-61,-60,-58,-55,-52,-48,-44,-39,-35,-29,-24,-18,-12,-6,
};

signed char short_cos[] = {
	63,62,61,60,58,55,52,48,44,39,35,29,24,18,12,6,0,
	-6,-12,-18,-24,-29,-35,-39,-44,-48,-52,-55,-58,-60,-61,-62,-63,
	-62,-61,-60,-58,-55,-52,-48,-44,-39,-35,-29,-24,-18,-12,-6,0,
	6,12,18,24,29,35,39,44,48,52,55,58,60,61,62,
};

typedef enum MOBType{
	SOLDIER,PARACHUTE,BULLET
} MobType;

typedef enum {
	GS_WELCOME, GS_STARTING, GS_RUNNING, GS_STOPPING, GS_ENDING
} GameState;
GameState game_state=GS_RUNNING;

/****Fixed Point 9.6 Stuff ****/
typedef signed int fx_96;

#define TO_INT(n) n>>6
#define TO_FX96(n) n<<6

/**** MOB Stuff  ****/
typedef struct {
	byte			vsprite_num;
    bool            active=false;
	MobType 		type;
	fx_96			x;
	fx_96			y;
	fx_96 			speed_x=0;
	fx_96 			speed_y=0;
	byte			width;
	byte			height;
	bool 			has_chute=false;
} MOB;


/****Method Signatures****/

void init_screen(byte num_stars);
void init_troopers();
void init_trooper(byte trooper_num, byte vsprite_num, fx_96 x, fx_96 y, fx_96 speed_y);
void move_troopers();
void fire_bullet(MOB* bullet, int x, int y, byte direction, fx_96 speed);
void move_bullets();
byte find_inactive_trooper();
void add_troopers();