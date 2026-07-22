#include "sabotage64.h"

byte* color = (byte*) (0xd800);

MOB troopers[MAX_TROOPERS];
MOB bullets[MAX_BULLETS];
MOB helicopters[MAX_HELIS];

//how densely do we drop the troopers?
byte trooper_clock;

//How many troopers are still to be dropped? TODO:unused?
byte troopers_left;

//TODO Is this a compiler bug? If so, file it with DMW
//const byte TROOPER_COLOR=(byte)VCOL_BROWN;

/**
 * Angles look like:
 * 		48
 * 		|
 *	32 ----0
 * 		|
 * 		16
 */
const byte BARREL_ANGLES[]={32,37,40,48,56,60,0};
fx_96 barrel_dir;//=TO_FX96(3);

const byte MAX_BULLET_CLOCK=20;
//How densely do the bullets get fired?
byte bullet_clock;

//Are we currently firing bullets?
bool is_firing;

//Current score
long score;
long highscore=0;

//How many troopers still need to be dropped on this level?
byte remaining_troopers;


#pragma optimize(3)
int main() {

	//MUST BE THE FIRST INSTRUCTION
	mmap_trampoline();
	//TODO why does changing this to MMAP_NO_BASIC cause a crash?
	mmap_set(MMAP_NO_ROM);

	while (game_state!=GS_ENDING) {

		switch (game_state) {
			case GS_INITIAL_START: {
				initial_start();

				show_title_screen();
				wait_for_fire();

				game_state = GS_WELCOME;
				break;
			}//GS_INITIAL_START
			case GS_WELCOME: {
				vic.color_back=VCOL_LT_BLUE;
				vic.color_border=VCOL_BLACK;

				show_welcome_screen();
				wait_for_fire();

				game_state = GS_STARTING_GAME;
				break;
			}//GS_WELCOME
			case GS_STARTING_GAME: {
				srand(1);	//make every game the same 
							//TODO: do I want every game the same?
				//NOTE TO SELF: don't use srand(0) -- every rand() comes out as 0!

				sidfx_play(1,SFXReveille,20);
				show_game_screen();

				current_level = 0;
				score=0;

				barrel_dir=TO_FX96(3);
				draw_barrel();

				init_bullets();
				set_score(0);
				trooper_clock=levels[current_level].max_trooper_clock;
				bullet_clock=MAX_BULLET_CLOCK;
				is_firing=false;

				game_state = GS_START_LEVEL;
				break;
			}//GS_STARTING_GAME
			case GS_START_LEVEL: {
				init_bullets();
				clear_troopers();	//get rid of any left over troopers in the sky

				if (levels[current_level].clear_before_starting) {
					show_game_screen();//erase troopers on the ground
				}
				remaining_troopers = levels[current_level].num_troopers;
				update_vsprites();//show the trooper change

 
				trooper_clock=levels[current_level].max_trooper_clock;
				draw_barrel();

				bullet_clock=MAX_BULLET_CLOCK;
				is_firing=false;

				if (levels[current_level].pause_for_msg) {
					show_messages(levels[current_level].message[0], levels[current_level].message[1]);
				}
				//Wait for a little while before starting the next level
				// for (byte i=0;i<10;i++) {
				// 	rirq_wait();
				// }
				troopers_left = levels[current_level].num_troopers;
				game_state = GS_RUNNING;
				break;
			}//GS_START_LEVEL
			case GS_RUNNING: {
				run_game();				
				break;
			}//GS_RUNNING
			//We've dropped all of our troopers, now we wait until they've all landed
			case GS_LEVEL_ENDING: {
				run_game();
				byte active_trooper_num=find_trooper(true);
				if (active_trooper_num==0xff) {	//no active troopers left

					//Remove any stray bullets from the screen
					init_bullets();
					//Wait for a second or so before starting next level.
					//	This is necessary to let any explosion noises complete
					//	before we change screens & stop processing them.
					for (byte i=0;i<60;i++) {
						sidfx_loop();
						update_vsprites();
					}	
					char l_trooper_count[26];
					char r_trooper_count[26];


					byte l_count=count_landed_troopers(10,0,22,19);
					if (l_count >= 4) {
						sprintf(l_trooper_count, "troopers on left side:%d", l_count);
						petscii_to_screen_str(l_trooper_count, 26);
						center_message(l_trooper_count, 9);
						game_state=GS_GAME_OVER;
						//TODO:INSERT LEFT SABOTAGE ANIMATION HERE
						continue;
					}

					//Wait half a second to separate the two counts a little
					for (byte i=0;i<30;i++) {
						sidfx_loop();
						update_vsprites();
					}

					byte r_count=count_landed_troopers(10,21,22,39);
					if (r_count >= 4) {
						sprintf(r_trooper_count, "troopers on right side:%d", r_count);
						petscii_to_screen_str(r_trooper_count, 26);
						center_message(r_trooper_count,9);
						game_state=GS_GAME_OVER;
						//TODO:INSERT RIGHT SABOTAGE ANIMATION HERE
						continue;
					}
					reset_landed_trooper_color(TROOPER_COLOR);
					current_level=levels[current_level].next_level_num;
					game_state=GS_START_LEVEL;
				}
				break;
			}//GS_LEVEL_ENDING
			case GS_STOPPING:{
				game_state = GS_ENDING;
				break;
			}//GS_STOPPING
			case GS_GAME_OVER:{
				//barrel_dir=TO_FX96(3);

				vic.color_back=VCOL_LT_RED;

				sidfx_play(1, SIDFXPlayerExplosion, 4);

				//remember, a value of 3 here is the default. <3 is up, >3 is down
				byte shake_table[16] = {
					3, 4, 6, 7, 7, 6, 4, 3,
					3, 2, 1, 0, 0, 2, 2, 3
				};


				//shake screen
				for (byte i=0;i<100;i++) {
					byte shake_value=shake_table[i % 16];
					set_vert_scroll(shake_value);
					vspr_set(VS_BARREL_OFFSET,BARREL_X,BARREL_Y+shake_value,SPRITE_OFFSET+19,
							VCOL_YELLOW);
					update_vsprites();
					sidfx_loop();
				}

				//TODO necessary?
				for (byte i=0;i<40;i++) {
					sidfx_loop();
				}

				vic_waitBottom();
				set_vert_scroll(3);//set scroll to neutral position

				vspr_hide(VS_BARREL_OFFSET);
				update_vsprites();

				vic.color_back=VCOL_LT_BLUE;

				center_message(SABOTAGED_MSG_LINE1,10);
				center_message(SABOTAGED_MSG_LINE2,11);
				center_message(PRESS_FIRE_MSG,14);

				if (score > highscore) {
					highscore=score;
					update_onscreen_highscore();

					center_message(HIGH_SCORE_MSG,12);
					sidfx_play(1,SIDFXFlagArrival,7);

					//wait to let the SIDFx play for a bit
					for (byte i=0;i<30;i++) {
						sidfx_loop();
						vic_waitBottom();
					}
				}

				wait_for_fire();

				game_state=GS_INITIAL_START;
				break;
			}//case GS_GAME_OVER
		}//switch(game_state)
	}//while

    return 0;
}

/**
 * n is the vertical screen scroll. 3 is neutral, so <3 is up
 * 	and >3 is down.
 */
void set_vert_scroll(byte n) {
	__asm {
		sei
		lda $d011
		and #248
		ora n
		sta $d011
		cli
	}
}

void show_messages(const char const *msg1, const char* msg2) {
	center_message(msg1,10);
	center_message(msg2,11);
	center_message(PRESS_FIRE_MSG,13);
	wait_for_fire();
	erase_message(10);
	erase_message(11);
	erase_message(13);
}

inline void show_game_screen() {
	//memcpy(screen, game_screen,1000);
	oscar_expand_lzo(screen, game_screen_lzo);

	oscar_expand_lzo(color, game_screen_color_lzo);

	update_onscreen_score();
	update_onscreen_highscore();

	vic.color_border=VCOL_BLACK;
	vic.color_back=VCOL_LT_BLUE;
}

inline void show_title_screen() {
	oscar_expand_lzo(screen, title_text_screen_lzo);

	oscar_expand_lzo(color, title_color_screen_lzo);

	vic.color_border=VCOL_BLACK;
	vic.color_back=VCOL_WHITE;
}

void init_bullets() {
	#pragma unroll(full)
	for (byte i=0;i<MAX_BULLETS;i++) {
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

	//#pragma unroll(full)
	byte nb=levels[current_level].num_bullets;
	for (int i=0;i<nb;i++) {
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

/** @return the index of the next inactive bullet, or 0xff if all bullets are active */
byte find_inactive_bullet() {
	//#pragma unroll(full)
	byte nb=levels[current_level].num_bullets;
	for (byte i=0;i<nb;i++) {
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
	for (byte i=0;i<mt;i++) {
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
			else {	//check to see if there are already 3 troopers stacked up. If so, no more. And no, you don't get
					//	any points for the troopers smooshing *themselves*.
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
		}//else if chute

		vspr_movey(troopers[i].vsprite_num,y-7);
		if (troopers[i].has_chute) {
			vspr_movey(troopers[i].vsprite_num-VS_TROOPER_OFFSET+VS_CHUTE_OFFSET,y-7);	//chute
		}
	}//for	
}//move_troopers()

//Figure out the screen memory offset (from 0-999) for a given (x,y) character position)
inline int calc_screen_offset(int x, int y) {
		int screen_loc=((y-50)/8)*40+(x-23)/8;
		return screen_loc;
}

//Stops, deactivates, hides trooper and chute; leaves trooper char
void land_trooper(byte trooper_num, int screen_loc) {
	stop_trooper(trooper_num);
	screen[screen_loc]=TROOPER_CHAR;
	color[screen_loc]=TROOPER_COLOR;
}

//stops, deactivates, and hides trooper and chute; leaves smooshed char; plays explosion, incs score, resets trooper clock
void smoosh_trooper(byte trooper_num, int screen_loc) {
	//TODO it would be cool if we could count any troopers squished under their comerades' feet
	stop_trooper(trooper_num);
	//TODO animate smooshed trooper
	screen[screen_loc]=SMOOSHED_CHAR;
	color[screen_loc]=VCOL_RED;
	kill_trooper(trooper_num);
}

//stops, deactivates, and hides trooper and chute
void stop_trooper(byte trooper_num) {
	troopers[trooper_num].speed_y=0;
	troopers[trooper_num].active=false;
	vspr_hide(troopers[trooper_num].vsprite_num);
	vspr_hide(troopers[trooper_num].vsprite_num+MAX_TROOPERS);	//chute
}

//plays explosion, incs score, resets trooper clock
void kill_trooper(byte trooper_num) {
	sidfx_play(1,SIDFXQuickExplosion,1);

	inc_score(TROOPER_SCORE_VALUE);

	trooper_clock=levels[current_level].max_trooper_clock;
}

void add_troopers() {
	if (--trooper_clock == 0) {
		trooper_clock=levels[current_level].max_trooper_clock;
		byte tnum=find_trooper(false); //find 1st inactive trooper
		if (tnum==0xff) {
			return;	//no troopers available
		}

		remaining_troopers--;
		//pick a semi-random scteen column
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
void draw_barrel() {
	int spr_num=BARREL_SPRITE+TO_INT(barrel_dir);
	vspr_set(VS_BARREL_OFFSET,
			BARREL_X,
			BARREL_Y,
			spr_num,
			levels[current_level].barrel_color
	);
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
				}
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
		game_state = GS_START_LEVEL;
	}
}

void steer_bullets() {
	//#pragma unroll(full)
	byte nb=levels[current_level].num_bullets;
	for (byte i=0;i<nb;i++) {
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


inline bool point_is_in_box(int px, int py, int bx, int by, int b_endx, int b_endy) {
	return (px >= bx && px <=b_endx && py>=by && py<=b_endy);
}

inline bool rect_is_in_box_inclusive(int px, int py, int p_endx, int p_endy, 
	int bx, int by, int b_endx, int b_endy) {
		return (px <= b_endx && p_endx >= bx &&
				py <= b_endy && p_endy >= by);
}

void check_bullet_collisions() {
	//yes, this is a crummy algorithm(O(n^2)), but for the small number of bullets/troopers
	//	it should be fine.
	byte nb=levels[current_level].num_bullets;
	for (byte bn=0;bn<nb;bn++) {
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
			//if (point_is_in_box(bullet->x,bullet->y,
			//		trooper->x,trooper->y,trooper->end_x,trooper->end_y)) {

			//TODO:Is the 1x1 hitbox big enough?
			if (rect_is_in_box_inclusive(bullet->x, bullet->y, bullet->x+TO_INT(1), bullet->y+TO_INT(1),
				trooper->x, trooper->y, trooper->end_x, trooper->end_y)) {
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


void clear_troopers() {
	byte mt=levels[current_level].max_troopers;
	for (int i=0;i<mt;i++) {
		if (troopers[i].active) {
			stop_trooper(i);
		}
	}

}

//Only runs once
void initial_start() {
    // Disable CIA interrupts, we do not want interference
	// with our joystick interrupt
	cia_init();

	//Change screen & charset addresses
	vic_setmode(VICM_TEXT,screen,charset);

	sidfx_init();
	sid.fmodevol = 15;

	//TODO get SID version of Reveille written
	//init_sid();

	// enable raster interrupt via direct path
	rirq_init(false);

	// initialize sprite multiplexer
	vspr_init(screen);

	vspr_sort();
	vspr_update();
	rirq_sort();

	rirq_start();

	//# of sprites * 64 bytes
	//memcpy(spriteset, stored_spriteset, 20*64);
	oscar_expand_lzo(spriteset, stored_spriteset_lzo);
	//memcpy(charset, stored_charset, 0x800);
	oscar_expand_lzo(charset, stored_charset_lzo);
}

void run_game() {
	handle_inputs();

	check_bullet_collisions();

	if (remaining_troopers>0) {
		add_troopers();
	}
	else {
		game_state=GS_LEVEL_ENDING;
	}

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

	//TODO Right now, the score resets at 100000. Maybe do something else here when we exceed 5 score digits?
	if (score > 99999) {
		score = 0L;
	}
	update_onscreen_score();
	return score;
}

void update_onscreen_score() {
	char	score_chars[6];
	sprintf(score_chars,"%.5ld",score);
	for (byte i=0;i<5;i++) {
		//for each char of the score, convert PETSCII value to screen code & store onscreen
		*((char *)SCORE_POS+i)=score_chars[i]-p'0'+s'0';//SCREEN_CODE_0;
	}
}

void update_onscreen_highscore() {
	char	hiscore_chars[6];
	sprintf(hiscore_chars,"%.5ld",highscore);
	for (byte i=0;i<5;i++) {
		//for each char of the score, convert PETSCII value to screen code & store onscreen
		*((char *)HISCORE_POS+i)=hiscore_chars[i]-p'0'+s'0';//SCREEN_CODE_0;
	}
}

/**Prints & centers a given message. Message should be in screen codes. 
 * 	Words or phrases surrounded by <> symbols will be displayed in VCOL_RED.
 * 	Ex: "you have been <sabotaged>".
*/
void center_message(const char const *message, byte row) {

	bool use_color=false;

	byte len=0;
	if ((message == NULL) || ((len=strlen(message))==0)) {
		return;
	}
	// byte len=strlen(message);
	// if (len==0) {
	// 	return;
	// }
	byte num_spaces=(40-len)/2;

	int array_offset=0;
	int screen_offset=row*40+num_spaces;

	while (array_offset<len) {
	//for (byte b=0;b<len;b++) {
		byte b=message[array_offset++];

		if (b==s'<') {
			use_color=true;
			continue;
		}
		else if (b==s'>') {
			use_color=false;
			continue;
		}
		//int offset=row*40+num_spaces+b;
		screen[screen_offset]=b;
		if (use_color) {
			color[screen_offset]=VCOL_RED;
		}
		//if !use_color, then keep the existing color map cell for that space
		
		screen_offset++;
	}
}

inline void erase_message(byte row) {
	//39 space chars
	const char empty_msg[]=s"                                       ";
	center_message(empty_msg, row);
}

//Waits for either the joystick button or the spacebar to be pressed, then released
void wait_for_fire() { 
	while (true) {
		joy_poll(0);
		keyb_poll();
		if (key_pressed(KSCAN_SPACE) || joyb[0]>0) {
			break;
		}
	}
	//now debounce
	while (true) {
		joy_poll(0);
		keyb_poll();
		if (!key_pressed(KSCAN_SPACE) && !joyb[0]>0) {
			break;
		}
	}
}

//end_row, end_col are inclusive
byte count_landed_troopers(byte start_row, byte start_col, byte end_row, byte end_col) {
	byte troopers_found=0;
	for (byte r=start_row;r<=end_row;r++) {
		for (byte c=start_col;c<=end_col;c++) {
			if (*(screen+r*40+c) == TROOPER_CHAR) {
				*(color+r*40+c) = 1;
				troopers_found++;
				sidfx_play(0,SIDFXClick,1);
				//wait for soundfx to end
				for (byte b=0;b<30;b++) {
					sidfx_loop();
					//really just here to wait until next frame
					update_vsprites();	
				}//b
			}//if
		}//for c
	}//for r
	return troopers_found;
}

void reset_landed_trooper_color(byte new_color) {
	for (byte r=0;r<=24;r++) {
		for (byte c=0;c<=40;c++) {
			if (*(screen+r*40+c) == TROOPER_CHAR) {
				*(color+r*40+c) = new_color;
			}
		}
	}

}


/**
 * Only works to convert lowercase ASCII to uppercase screen codes, for the C64 UPPER/GRAPHICS charset.
 * 	Ignores digits (which are fine) and uppercase chars (which will show as gibberish). 
 **/
inline byte petscii_to_screen_char(byte c) {
	if (c>='a' && (c<='z')) {
		c-=96;
	}

	return c;
}

//Converts *in-place*
inline void petscii_to_screen_str(char *msg, int len) {
	for (byte i=0;i<len;i++) {
		msg[i] = petscii_to_screen_char(msg[i]);
	}
}

inline void show_welcome_screen() {
	//memcpy(screen,stored_welcome_screen, 1000);
	oscar_expand_lzo(screen, stored_welcome_screen_lzo);
	//memcpy(color,stored_welcome_color, 1000);
	oscar_expand_lzo(color, stored_welcome_color_lzo);
}
