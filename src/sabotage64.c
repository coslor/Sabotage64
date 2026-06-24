#include "sabotage64.h"



// const char* hello_text = "HELLO, WORLD!";
//char* screen = (char*) (0x8000);
unsigned char* color = (unsigned char*) (0xd800);

MOB troopers[MAX_TROOPERS];
MOB bullets[NUM_BULLETS];
//fx_96 speed=0; //for debugger

//const byte MAX_TROOPER_CLOCK=90;
byte trooper_clock;

//TODO Is this a compiler bug? If so, file it with DMW
//const byte TROOPER_COLOR=VCOL_BROWN;

#define TROOPER_COLOR VCOL_DARK_GREY

/**
 * Angles look like:
 * 		48
 * 		|
 *	32 ----0
 * 		|
 * 		16
 */
const byte BARREL_ANGLES[]={32,37,42,48,53,58,0};
fx_96 barrel_dir;//=TO_FX96(3);

const byte MAX_BULLET_CLOCK=20;
byte bullet_clock;//=MAX_BULLET_CLOCK;
bool is_firing;

long score;


int main() {

	//MUST BE THE FIRST INSTRUCTION
	mmap_trampoline();
	mmap_set(MMAP_NO_ROM);

	//init_barrel();

	//clock_t time = clock();


	int frame_num=0;



	while (game_state!=GS_ENDING) {

		switch (game_state) {
			case GS_INITIAL_START: {
				initial_start();
				game_state = GS_WELCOME;
				break;
			}
			case GS_WELCOME: {
				game_state = GS_STARTING_GAME;
				break;
			}
			case GS_STARTING_GAME: {
				srand(1);	//make every game the same TODO: do I want every game the same?
				//NOTE TO SELF: don't use srand(0) -- every rand() comes out as 0!
				sidfx_play(1,SFXReveille,20);
				clear_troopers();
				init_screen();
				draw_barrel();
				init_bullets();
				set_score(99990L);
				trooper_clock=levels[current_level].max_trooper_clock;
				barrel_dir=TO_FX96(3);
				bullet_clock=MAX_BULLET_CLOCK;
				is_firing=false;
				current_level = 0;

				game_state = GS_RUNNING;
				break;
			}
			case GS_RUNNING: {
				run_game();				
				break;
			}
			case GS_STOPPING:{
				game_state = GS_ENDING;
				break;
			}
		}
	}

    return 0;
}

void init_screen() {
	memcpy(screen,title_screen,1000);
	memcpy(charset, stored_charset, 0x800);
	memcpy(spriteset, stored_spriteset, 1280);
	memset(color,1,1000);

	vic.color_border=VCOL_BLACK;
	vic.color_back=VCOL_LT_BLUE;

}

void init_bullets() {
	#pragma unroll(full)
	for (byte i=0;i<NUM_BULLETS;i++) {
		bullets[i].vsprite_num=VS_BULLET_OFFSET+i;
		bullets[i].active=false;
		vspr_hide(bullets[i].vsprite_num);
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

	#pragma unroll(full)
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
	#pragma unroll(full)
	for (byte i=0;i<NUM_BULLETS;i++) {
		if (! bullets[i].active) {
			return i;
		}
	}
	return 0xff;
}

/** **NOTE: x MUST be an even multiple of 8 or the characters on the bottom won't line up!** **/
void drop_trooper(byte num, byte vsprite_num, fx_96 x, fx_96 y, fx_96 speed_y) {
	MOB* t=&troopers[num];
	t->vsprite_num=vsprite_num;

	//NOTE: The sprite goes from 2,7 to 6,14. 
	t->x=x;//+TO_FX96(2);
	//tx=TO_INT(t->x);
	t->y=y;//y+TO_FX96(7);
	//ty = TO_INT(t->y);
	t->end_x=x+TO_FX96(4);
	//tx2=TO_INT(t->end_x);
	t->end_y=t->y+TO_FX96(7);
	//ty2=TO_INT(t->end_y);


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
	//#pragma unroll(full)
	byte mt=levels[current_level].max_troopers;
	for (int i=0;i<mt;i++) {
		if (!troopers[i].active) {
			continue;
		}
		troopers[i].y+=troopers[i].speed_y;
		troopers[i].end_y+=troopers[i].speed_y;

		int x=TO_INT(troopers[i].x);
		int y=TO_INT(troopers[i].y);

		
		//Why (y-44) instead of (y-49)? Because the shape starts at (2,7) in the sprite.
		int screen_loc=calc_screen_offset(x,y); //((y-50)/8)*40+(x-23)/8;


		char c = screen[screen_loc+40];
		if (! troopers[i].has_chute) {
			if ((c==GROUND_CHAR) || (c==BUILDING_CHAR) || (c==HALF_GROUND_SQUARE)) {
				smoosh_trooper(i,screen_loc);
				continue;
			}
			else if (c==TROOPER_CHAR) {
				screen[screen_loc+40]=EMPTY_CHAR;
			}
		}//if ! chute
		//TODO what happens if smooshed char?
		else {	//has chute
			if ((c==GROUND_CHAR) || (c==BUILDING_CHAR) || (c==HALF_GROUND_SQUARE)) {
				land_trooper(i,screen_loc);
				continue;
			}
			else {	//check to see if there are already 3 troopers stacked up. If so, no more.
				if ((y<920) && (screen[screen_loc+40] == TROOPER_CHAR)) {
					if (((y<880) && ((screen[screen_loc+80]==TROOPER_CHAR) || (screen[screen_loc+80]==BUILDING_CHAR)))
						&& ((y<840) && (screen[screen_loc+120]==TROOPER_CHAR) 
										|| (screen[screen_loc+120]==BUILDING_CHAR) ) ) { //too many troopers stacked up
							stop_trooper(i);
							continue;
					}
					else {
						land_trooper(i,screen_loc);
						continue;
					}//else land trooper
				}//else if y<880
			}//else if y<920
		}//else if c==GROUND_CHAR

		vspr_movey(troopers[i].vsprite_num,y-7);
		if (troopers[i].has_chute) {
			vspr_movey(troopers[i].vsprite_num-VS_TROOPER_OFFSET+VS_CHUTE_OFFSET,y-7);	//chute
		}
		// else {
		// 	vspr_hide(troopers[i].vsprite_num-VS_TROOPER_OFFSET+VS_CHUTE_OFFSET);
		// }
	}//for	
}//move_troopers()

//Figure out the screen memory offset (from 0-999) for a given (x,y) character position)
inline int calc_screen_offset(int x, int y) {
		int screen_loc=((y-50)/8)*40+(x-23)/8;
		return screen_loc;
}

//Stops, deactivates, hides trooper and chute; leaves trooper char
void land_trooper(char trooper_num, int screen_loc) {
	stop_trooper(trooper_num);
	screen[screen_loc]=TROOPER_CHAR;
	color[screen_loc]=TROOPER_COLOR;
	//TODO is this necessary or useful?
	//trooper_clock=MAX_TROOPER_CLOCK;
}

//stops, deactivates, and hides trooper and chute; leaves smooshed char; plays explosion, incs score, resets trooper clock
void smoosh_trooper(char trooper_num, int screen_loc) {
	//TODO it would be cool if we could count any troopers squished under their comerades' feet
	stop_trooper(trooper_num);
	//TODO animate smooshed trooper
	screen[screen_loc]=SMOOSHED_CHAR;
	color[screen_loc]=VCOL_RED;
	kill_trooper(trooper_num);
}

#pragma optimize(0)
//stops, deactivates, and hides trooper and chute
void stop_trooper(char trooper_num) {
	troopers[trooper_num].speed_y=0;
	troopers[trooper_num].active=false;
	vspr_hide(troopers[trooper_num].vsprite_num);
	vspr_hide(troopers[trooper_num].vsprite_num+MAX_TROOPERS);	//chute
}

//plays explosion, incs score, resets trooper clock
void kill_trooper(byte trooper_num) {
	//vspr_color(troopers[num].vsprite_num,VCOL_RED);
	//troopers[trooper_num].active=false;
	//vspr_hide(troopers[trooper_num].vsprite_num);
	//vspr_hide(troopers[trooper_num].vsprite_num-VS_TROOPER_OFFSET+VS_CHUTE_OFFSET);
	sidfx_play(1,SIDFXQuickExplosion,1);

	inc_score(TROOPER_VALUE);

	trooper_clock=levels[current_level].max_trooper_clock;
}

void add_troopers() {
	if (--trooper_clock == 0) {
		trooper_clock=levels[current_level].max_trooper_clock;
		byte tnum=find_trooper(false); //find 1st inactive trooper
		if (tnum==0xff) {
			return;	//no troopers available
		}
		byte r=0;
		do {
			r=rand() % 37 + 1;
		} while (r>18 && r<22);

		int x=25+(r*8);
		byte y=50;
		drop_trooper(tnum, tnum+VS_TROOPER_OFFSET, TO_FX96(x), TO_FX96(y),
			(fx_96)TROOPER_CHUTE_SPEED);//speed = n/64
	}
}

/** @return the index of the next inactive trooper, or 0xff if all troopers are active */
byte find_trooper(bool active) {
	byte mt=levels[current_level].max_troopers;
	for (byte i=0;i<mt;i++) {
		if (troopers[i].active == active) {
			return i;
		}
	}
	return 0xff;
}


/**
 * @param dir the direction of the barrel, from 32 to 0 (or 64)
 */
// void draw_barrel(){
// 	int bd=TO_INT(barrel_dir);
// 	int spr_num=BARREL_SPRITE_OFFSET+bd;
// 	//vspr_image(VS_BARREL_OFFSET,BARREL_SPRITE_OFFSET+TO_INT(barrel_dir));
// 	//shouldn't have to make the full call every time, but sometimes it loses the x,y???
// 	init_barrel();
// }

// void init_barrel() {
void draw_barrel() {
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
				// else {
				// 	vic.color_back=VCOL_BLACK;
				// }
				bullet_clock = MAX_BULLET_CLOCK;
			}
		}
		else {
			if (!fire_bullet_now()) {
				//vic.color_back=VCOL_RED;
			}
			// else {
			// 	vic.color_back=VCOL_BLACK;
			// }

			is_firing=true;
			bullet_clock == MAX_BULLET_CLOCK;
		}
	}
	else {
		is_firing = false;
	}

	//for debugging
	if (key_pressed(KSCAN_STOP)) {
		game_state = GS_STARTING_GAME;
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


inline bool point_is_in_box(int x, int y, int bx, int by, int b_endx, int b_endy) {

	return (x >= bx && x <=b_endx && y>=by && y<=b_endy);
}

//#pragma optimize(0)
void check_bullet_collisions() {
	#pragma unroll(full)
	for (int bn=0;bn<NUM_BULLETS;bn++) {
		MOB *bullet = &bullets[bn];
		if (! bullet->active) {
			continue;
		}
		byte mt=levels[current_level].max_troopers;
		for (int tn=0;tn<mt;tn++) {
			MOB *trooper=&troopers[tn];
			if (! trooper->active) {
				continue;
			}
			if (point_is_in_box(bullet->x,bullet->y,
					trooper->x,trooper->y,trooper->end_x,trooper->end_y)) {
				stop_trooper(tn);
				kill_trooper(tn);
				kill_bullet(bn);
				continue;
			} else if(point_is_in_box(bullet->x,bullet->y,
					trooper->x - TO_FX96(2),trooper->y - TO_FX96(7),
					trooper->end_x - TO_FX96(2),trooper->end_y - TO_FX96(7))) {
				kill_chute(tn);
				kill_bullet(bn);
				continue;
			}//if point
		}//for j
	}//for i
}//check_bullet_collisions




void kill_bullet(byte bullet_num) {
	bullets[bullet_num].active = false;
	vspr_hide(bullets[bullet_num].vsprite_num);
}

void kill_chute(byte trooper_num) {
	troopers[trooper_num].has_chute = false;
	troopers[trooper_num].speed_y *= 2;
	byte vsprite_num=troopers[trooper_num].vsprite_num-VS_TROOPER_OFFSET+VS_CHUTE_OFFSET;
	vspr_hide(vsprite_num);
	vspr_image(troopers[trooper_num].vsprite_num, TROOPER_SPRITE+1);
	//TODO implement animation for falling trooper
}


void show_score() {
	
}

void clear_troopers() {
	byte mt=levels[current_level].max_troopers;
	for (int i=0;i<mt;i++) {
		if (troopers[i].active) {
			stop_trooper(i);
		}
	}

}

void initial_start() {
    // Disable CIA interrupts, we do not want interference
	// with our joystick interrupt
	cia_init();

	vic_setmode(VICM_TEXT,screen,charset);


	sidfx_init();
	sid.fmodevol = 15;

	//TODO get SID version of Reveille written
	//init_sid();

	// enable raster interrupt via direct path
	rirq_init(false);

	// initialize sprite multiplexer
	vspr_init((char*)screen);

	vspr_sort();
	vspr_update();
	rirq_sort();

	rirq_start();


}

void run_game() {
	handle_inputs();

	check_bullet_collisions();

	add_troopers();
	move_troopers();

	draw_barrel();

	move_bullets();

	sidfx_loop();

	//TODO get SID version of Reveille written
	//play_sid();

	update_vsprites();

}

void update_vsprites() {
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

void init_sid() {
	__asm {
		jsr $0ff6
	}
}

void play_sid() {
	__asm {
		jsr $1003
	}
}

long set_score(long val) {
	score = val;

	update_onscreen_score();
	return score;
}

long inc_score(long val) {
	score += val;

	//TODO RIght now, the score resets at 100000. Maybe do something else here when we exceed 5 score digits?
	if (score > 99999) {
		score = 0L;
	}
	update_onscreen_score();
	return score;
}

void update_onscreen_score() {

	//TODO maybe convert this to a more efficient form? I can't really be bothered
	//		unless it ends up taking too many cycles
	char	score_chars[5];
	sprintf(score_chars,"%.5ld\n",score);
	for (byte i=0;i<5;i++) {
		//for each char of the score, convert PETSCII value to screen code & store onscreen
		*((char *)SCREEN_POS+i)=score_chars[i]-p'0'+s'0';//SCREEN_CODE_0;
	}
}