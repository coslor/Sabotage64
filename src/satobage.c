#include "satobage.h"

// const char* hello_text = "HELLO, WORLD!";
//char* screen = (char*) (0x8000);
unsigned char* color = (unsigned char*) (0xd800);

MOB troopers[NUM_TROOPERS];
MOB bullets[NUM_BULLETS];
//fx_96 speed=0; //for debugger

const byte MAX_TROOPER_COUNT=60;
byte trooper_clock=MAX_TROOPER_COUNT;

const byte TROOPER_CHAR=152;
const byte EMPTY_CHAR=0x20;

int main() {
	//MUST BE THE FIRST INSTRUCTION
	mmap_trampoline();
	mmap_set(MMAP_NO_ROM);

    // Disable CIA interrupts, we do not want interference
	// with our joystick interrupt
	cia_init();

	init_screen(25);

	// enable raster interrupt via direct path
	rirq_init(false);

	// initialize sprite multiplexer
	vspr_init((char*)screen);

	//clock_t time = clock();
	//srand(time);

	//init_troopers();

	// for (int i=0;i<NUM_BULLETS;i++) {
	// 	bullets[i].vsprite_num=NUM_TROOPERS+i;
	// 	bullets[i].type=BULLET;
		
	// 	fx_96 speed = (fx_96)16;	//fraction, actual value is speed/64
	// 	// printf("SPEED=%d\n",speed);
		
	// 	fire_bullet(&bullets[i],160,100,32+i*8,speed);
	// }

	// initial sort and update
	vspr_sort();
	vspr_update();
	rirq_sort();

	rirq_start();
	

	while (game_state==GS_RUNNING) {
		add_troopers();
		move_troopers();
		//move_bullets();

		// 2. wait for raster IRQ to reach end of frame
		rirq_wait();

		// 1. sort virtual sprites by y position
		vspr_sort();

		// 3. update sprites back to normal and set up raster IRQ for sprites 8 to 31
		vspr_update();

		// 4. sort raster IRQs
		rirq_sort();
	}

    return 0;
}

void init_screen(byte num_stars) {
	vic_setmode(VICM_TEXT,screen,charset);//(byte *)0x1000); //TODO is this correct?

	//memset(screen,0x20,1000);
	memset(color,1,1000);

	vic.color_border=VCOL_DARK_GREY;
	vic.color_back=VCOL_BLACK;

	//NOTE: can't unroll this loop, since the bounds are not constant
	//#pragma unroll(full)
	// for(byte i=0;i<num_stars;i++) {
	// 	unsigned int pos;
	// 	byte* text=(byte*)0x8000;//0x4000;	//FIXME remove magic number
	// 	byte* color=(byte*)(0xd800);

	// 	while ( text[pos=40+(rand() % 960)] != 32);
	// 	text[pos]=46; //screencode for "."
	// 	color[pos]=rand() % 16;
	// }

	// gotoxy(0,0);
	// printf("LIVES:XXXXX");

	// gotoxy(15,0);
	// printf("INVADERS");

	// gotoxy(27,0);
	// printf("SCORE:000000");

}

void move_sprites() {

}

//assumes that the MOB is of the right type
void fire_bullet(MOB* bullet, int x, int y, byte direction, fx_96 speed) {
	__assume(direction<64);
	__assume(bullet->type == BULLET);

	bullet->active = true;
	bullet->x=TO_FX96(x);
	bullet->y=TO_FX96(y);
	bullet->speed_x = speed*short_cos[direction] / 64;
	bullet->speed_y = speed*short_sin[direction] / 64;

	vspr_set(bullet->vsprite_num,TO_INT(bullet->x), TO_INT(bullet->y), 26, VCOL_WHITE);

}

void move_bullets() {

	for (int i=0;i<NUM_BULLETS;i++) {
		MOB* bullet = &bullets[i];
		__assume(bullet->type == BULLET);
		if (! bullet->active) continue;

		bullet->x += bullet->speed_x;
		bullet->y += bullet->speed_y;

		int x=TO_INT(bullet->x);
		int y=TO_INT(bullet->y);
		if ((x < 24) || (x > 344) || (y<50) || (y>250)) {
			bullet->active = false;
			vspr_hide(bullet->vsprite_num);
			continue;
		}
		vspr_move(bullet->vsprite_num, x, y);
	}

}

void init_troopers() {
	for (int i=0;i<NUM_TROOPERS;i++) {
		//No conversion on speed; it's in n/64 units
		init_trooper(i,i,TO_FX96(24+(i*16)), TO_FX96(rand()%50), (fx_96)32);
		// troopers[i].vsprite_num=i;		
		// /***NOTE: x MUST be an even multiple of 8 or the characters on the bottom won't line up! ***/
		// troopers[i].x=TO_FX96(24+(i*16)); 
		// troopers[i].y=TO_FX96(rand()%50);
		// troopers[i].speed_y=32;	//No conversion here; speed is in n/64 units
		// troopers[i].active = true;

		// vspr_set(troopers[i].vsprite_num,TO_INT(troopers[i].x),TO_INT(troopers[i].y),0x11,VCOL_GREEN);
	}
}

/** **NOTE: x MUST be an even multiple of 8 or the characters on the bottom won't line up!** **/
void init_trooper(byte trooper_num, byte vsprite_num, fx_96 x, fx_96 y, fx_96 speed_y) {
	troopers[trooper_num].vsprite_num=vsprite_num;		
	troopers[trooper_num].x=x; 
	troopers[trooper_num].y=y;
	troopers[trooper_num].speed_x=0;
	troopers[trooper_num].speed_y=speed_y;
	troopers[trooper_num].active = true;

	vspr_set(troopers[trooper_num].vsprite_num,TO_INT(troopers[trooper_num].x),
		TO_INT(troopers[trooper_num].y),SPRITE_OFFSET+1,VCOL_GREEN);
}


void move_troopers() {
	for (int i=0;i<NUM_TROOPERS;i++) {
		if (!troopers[i].active) {
			continue;
		}
		troopers[i].y+=troopers[i].speed_y;

		int x=TO_INT(troopers[i].x);
		int y=TO_INT(troopers[i].y);

		int screen_loc=((y-44)/8)*40+(x-23)/8;//Why (y-45) instead of (y-50)? The world may never know.

		if (screen[screen_loc+40]!=EMPTY_CHAR) {
			//troopers[i].y=rand()%50;
			troopers[i].speed_y=0;
			troopers[i].active=false;
			vspr_hide(troopers[i].vsprite_num);
			screen[screen_loc]=TROOPER_CHAR; //placeholder for trooper character
			color[screen_loc]=VCOL_GREEN;
			continue;
		}
		// else if (y<50) {
		// 	continue;
		// }

		//vspr_move(troopers[i].vsprite_num,x,y);
		vspr_movey(troopers[i].vsprite_num,y);
	}	
}

void add_troopers() {
	if (--trooper_clock == 0) {
		trooper_clock=MAX_TROOPER_COUNT;
		byte tnum=find_inactive_trooper();
		if (tnum==0xff) {
			return;	//no troopers available
		}
		int r=rand() % 39;
		int x=23+(r*8);
		byte y=39;
		init_trooper(tnum, tnum, TO_FX96(x), TO_FX96(y),(fx_96)32);//speed = n/64
	}
}

/* Returns the index of the next inactive trooper, or 0xff if all troopers are active */
byte find_inactive_trooper() {
	for (byte i=0;i<NUM_TROOPERS;i++) {
		if (! troopers[i].active) {
			return i;
		}
	}
	return 0xff;
}