#include "sabotage64.h"

#include "SIDFX.h"


// const char* hello_text = "HELLO, WORLD!";
//char* screen = (char*) (0x8000);
unsigned char* color = (unsigned char*) (0xd800);

MOB troopers[NUM_TROOPERS];
MOB bullets[NUM_BULLETS];
//fx_96 speed=0; //for debugger

const byte MAX_TROOPER_CLOCK=45;
byte trooper_clock=MAX_TROOPER_CLOCK;

const byte TROOPER_CHAR=152;
const byte EMPTY_CHAR=0x20;

/**
 * Angles look like:
 * 		48
 * 		|
 *	32 ----0
 * 		|
 * 		16
 */
const byte BARREL_ANGLES[]={32,37,42,48,53,58,0};
fx_96 barrel_dir=TO_FX96(3);

const byte MAX_BULLET_CLOCK=20;
byte bullet_clock=MAX_BULLET_CLOCK;
bool is_firing;

////
// for debugging
////
__export int bx;
__export int by;
__export int tx;
__export int ty;
__export int tx2;
__export int ty2;



int main() {
	//MUST BE THE FIRST INSTRUCTION
	mmap_trampoline();
	mmap_set(MMAP_NO_ROM);

    // Disable CIA interrupts, we do not want interference
	// with our joystick interrupt
	cia_init();

	init_screen(25);

	sidfx_init();
	sid.fmodevol = 15;


	// enable raster interrupt via direct path
	rirq_init(false);

	// initialize sprite multiplexer
	vspr_init((char*)screen);

	__asm {
		nop
	}

	init_barrel();
	init_bullets();

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
	//edcbedcbedcba
	int frame_num=0;

	sidfx_play(1,SFXOpening,13);

	while (game_state==GS_RUNNING) {


		handle_inputs();

		check_bullet_collisions();

		add_troopers();
		move_troopers();

		draw_barrel();

		move_bullets();

		sidfx_loop();

		//NOTE:Do these in the stated order! It can make a BIG performence difference!
		
		// 1. sort virtual sprites by y position
		vspr_sort();

		// 2. wait for raster IRQ to reach end of frame
		rirq_wait();

		// 3. update sprites back to normal and set up raster IRQ for sprites 8 to 31
		vspr_update();

		// 4. sort raster IRQs
		rirq_sort();
	}

    return 0;
}

void init_screen(byte num_stars) {
	vic_setmode(VICM_TEXT,screen,charset);

	//memset(screen,0x20,1000);
	memset(color,1,1000);

	vic.color_border=VCOL_BLACK;
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



void init_bullets() {
	for (byte i=0;i<NUM_BULLETS;i++) {
		bullets[i].vsprite_num=VS_BULLET_OFFSET+i;
	}
}

//assumes that the MOB is of the right type
void fire_bullet(MOB* bullet, fx_96 x, fx_96 y, byte direction, fx_96 speed) {
	__assume(direction<64);
	//__assume(bullet->type == BULLET);

	bullet->active = true;
	bullet->x=x;
	bullet->y=y;
	bullet->end_x = x + TO_FX96(1);
	bullet->end_y = y + TO_FX96(1);
	bullet->speed_x = speed*short_cos[direction] / 64;
	bullet->speed_y = speed*short_sin[direction] / 64;

	vspr_set(bullet->vsprite_num,TO_INT(bullet->x), TO_INT(bullet->y), 
		BULLET_SPRITE, BULLET_COLOR);

}

void move_bullets() {

	for (int i=0;i<NUM_BULLETS;i++) {
		MOB* bullet = &bullets[i];
		//__assume(bullet->type == BULLET);
		if (! bullet->active) continue;

		bullet->x += bullet->speed_x;
		bullet->y += bullet->speed_y;
		bullet->end_x += bullet->speed_x;
		bullet->end_y += bullet->speed_y;

		int x=TO_INT(bullet->x);
		int y=TO_INT(bullet->y);
		if ((x < 24) || (x > 344) || (y<50) || (y>250)) {
			bullet->active = false;
			vspr_hide(bullet->vsprite_num);
			continue;
		}
		vspr_move(bullet->vsprite_num, x, y);
		//vspr_move(BULLET);
	}

}

/** @return the index of the next inactive trooper, or 0xff if all troopers are active */
byte find_inactive_bullet() {
	for (byte i=0;i<NUM_BULLETS;i++) {
		if (! bullets[i].active) {
			return i;
		}
	}
	return 0xff;
}

/** **NOTE: x MUST be an even multiple of 8 or the characters on the bottom won't line up!** **/
void init_trooper(byte num, byte vsprite_num, fx_96 x, fx_96 y, fx_96 speed_y) {
	MOB* t=&troopers[num];
	t->vsprite_num=vsprite_num;

	//NOTE: The sprite goes from 2,7 to 6,14. 
	t->x=x;//+TO_FX96(2);
	tx=TO_INT(t->x);
	t->y=y;//y+TO_FX96(7);
	ty = TO_INT(t->y);
	t->end_x=x+TO_FX96(4);
	tx2=TO_INT(t->end_x);
	t->end_y=t->y+TO_FX96(7);
	ty2=TO_INT(t->end_y);


	t->speed_x=0;
	t->speed_y=speed_y;
	t->active = true;
	t->has_chute=true;

	//trooper sprite
	vspr_set(t->vsprite_num,
		TO_INT(t->x)-2,TO_INT(t->y)-7,
		TROOPER_SPRITE,TROOPER_COLOR);
	//chute sprite
	vspr_set(t->vsprite_num-VS_TROOPER_OFFSET+VS_CHUTE_OFFSET,
		TO_INT(t->x)-2,TO_INT(t->y)-7,
		CHUTE_SPRITE,CHUTE_COLOR);
}


void move_troopers() {
	for (int i=0;i<NUM_TROOPERS;i++) {
		if (!troopers[i].active) {
			continue;
		}
		troopers[i].y+=troopers[i].speed_y;
		troopers[i].end_y+=troopers[i].speed_y;

		int x=TO_INT(troopers[i].x);
		int y=TO_INT(troopers[i].y);

		//Why (y-44) instead of (y-49)? Because the shape starts at (2,7) in the sprite.
		int screen_loc=((y-44)/8)*40+(x-23)/8;

		if (screen[screen_loc+40]!=EMPTY_CHAR) {
			land_trooper(i,screen_loc);
			continue;
		}

		vspr_movey(troopers[i].vsprite_num,y-7);
		vspr_movey(troopers[i].vsprite_num-VS_TROOPER_OFFSET+VS_CHUTE_OFFSET,y-7);	//chute
	}	
}

void land_trooper(int trooper_num, int screen_loc) {
	troopers[trooper_num].speed_y=0;
	troopers[trooper_num].active=false;
	vspr_hide(troopers[trooper_num].vsprite_num);
	vspr_hide(troopers[trooper_num].vsprite_num+NUM_TROOPERS);	//chute

	screen[screen_loc]=TROOPER_CHAR; //placeholder for trooper character
	color[screen_loc]=VCOL_GREEN;

}
void add_troopers() {
	if (--trooper_clock == 0) {
		trooper_clock=MAX_TROOPER_CLOCK;
		byte tnum=find_trooper(false); //find 1st inactive trooper
		if (tnum==0xff) {
			return;	//no troopers available
		}
		byte r=0;
		do {
			r=rand() % 39;
		} while (r>18 && r<22);

		int x=23+(r*8);
		byte y=56;
		init_trooper(tnum, tnum+VS_TROOPER_OFFSET, TO_FX96(x), TO_FX96(y),
			(fx_96)TROOPER_CHUTE_SPEED);//speed = n/64
	}
}

/** @return the index of the next inactive trooper, or 0xff if all troopers are active */
byte find_trooper(bool active) {
	#pragma unroll(full)
	for (byte i=0;i<NUM_TROOPERS;i++) {
		if (troopers[i].active == active) {
			return i;
		}
	}
	return 0xff;
}


/**
 * @param dir the direction of the barrel, from 32 to 0 (or 64)
 */
void draw_barrel(){
	int bd=TO_INT(barrel_dir);
	int spr_num=BARREL_SPRITE_OFFSET+bd;
	//vspr_image(VS_BARREL_OFFSET,BARREL_SPRITE_OFFSET+TO_INT(barrel_dir));
	//shouldn't have to make the full call every time, but sometimes it loses the x,y???
	init_barrel();
}

void init_barrel() {
	int spr_num=BARREL_SPRITE_OFFSET+TO_INT(barrel_dir);
	vspr_set(VS_BARREL_OFFSET,
			BARREL_X,
			BARREL_Y,
			spr_num,
			//60,
			BARREL_COLOR);	
}

void handle_inputs() {
	joy_poll(0);
	keyb_poll();

	if (key_pressed(KSCAN_A) || key_pressed(KSCAN_K) || joyx[0]==-1) {
		if (barrel_dir>=0x10) {
			barrel_dir-=0x10;
		}
#ifdef STEERABLE_BULLETS
		steer_bullets();
#endif		
	}

	if (key_pressed(KSCAN_S) || key_pressed(KSCAN_L) || joyx[0]==1) {
		if (barrel_dir<0x180) {
			barrel_dir+=0x10;
		}
#ifdef STEERABLE_BULLETS
		steer_bullets();
#endif
	}

	if (key_pressed(KSCAN_SPACE) || joyb[0]>0) {
		if (is_firing) {
			if (bullet_clock-- == 0) {
				if (! fire_bullet_now()) {
					// vic.color_back=VCOL_RED;
				}
				else {
					vic.color_back=VCOL_BLACK;
				}
				bullet_clock = MAX_BULLET_CLOCK;
			}
		}
		else {
			if (!fire_bullet_now()) {
				//vic.color_back=VCOL_RED;
			}
			else {
				vic.color_back=VCOL_BLACK;
			}

			is_firing=true;
			bullet_clock == MAX_BULLET_CLOCK;
		}
	}
	else {
		is_firing = false;
	}
}

void steer_bullets() {
	#pragma unroll(full)
	for (int i=0;i<NUM_BULLETS;i++) {
		if (! bullets[i].active) {
			continue;
		}
		byte bd=BARREL_ANGLES[TO_INT(barrel_dir)];
		bullets[i].speed_x = (fx_96)BULLET_SPEED*short_cos[bd] / 64;
		bullets[i].speed_y = (fx_96)BULLET_SPEED*short_sin[bd] / 64;
	}
}

bool fire_bullet_now() {
	byte bullet_num=find_inactive_bullet();
	if(bullet_num==0xff) {
		return false;
	}

#ifdef DEBUG
	if (TO_INT(barrel_dir)>6) {
		vic.color_border=VCOL_PURPLE;
		while(true);
	}
#endif

	byte bd=BARREL_ANGLES[TO_INT(barrel_dir)];

	fx_96 bullet_x=TO_FX96(BARREL_X+11);
	fx_96 bullet_y=TO_FX96(BARREL_Y+17);

#ifdef PERFECT_SHOT
	byte trooper_num=find_trooper(true);
	if (trooper_num != 0xff) {
		bullet_x = troopers[trooper_num].x+2;
		bullet_y = troopers[trooper_num].y+2;
	}
#endif

	//speed is a fraction, actual value is speed/64)
	fire_bullet(&bullets[bullet_num],bullet_x, bullet_y, bd,(fx_96)BULLET_SPEED);
	sidfx_play(0,SIDFXFire,1);
	return true;
}


bool point_is_in_box(int x, int y, int bx, int by, int b_endx, int b_endy) {

	return (x >= bx && x <=b_endx && y>=by && y<=b_endy);
}

void check_bullet_collisions() {
	#pragma unroll(full)
	for (int i=0;i<NUM_BULLETS;i++) {
		MOB *bullet = &bullets[i];
		if (! bullet->active) {
			continue;
		}
		
		for (int j=0;j<NUM_TROOPERS;j++) {
			MOB *trooper=&troopers[j];
			if (! trooper->active) {
				continue;
			}
			bx=TO_INT(bullet->x);
			by=TO_INT(bullets->y);
			tx=TO_INT(trooper->x);
			ty=TO_INT(trooper->y);
			tx2=TO_INT(trooper->end_x);
			ty2=TO_INT(trooper->end_y);
			if (point_is_in_box(bullet->x,bullet->y,
					trooper->x,trooper->y,trooper->end_x,trooper->end_y)) {
				kill_trooper(j);
				kill_bullet(i);
			}//if point
		}//for j
	}//for i
}//check_bullet_collisions


void kill_trooper(byte num) {
	//vspr_color(troopers[num].vsprite_num,VCOL_RED);
	troopers[num].active=false;
	vspr_hide(troopers[num].vsprite_num);
	vspr_hide(troopers[num].vsprite_num-VS_TROOPER_OFFSET+VS_CHUTE_OFFSET);
}

void kill_bullet(byte num) {
	bullets[num].active = false;
	vspr_hide(bullets[num].vsprite_num);
}