#include "satobage_memory.h"
#include <c64/sprites.h>
#include <c64/rasterirq.h>
#include <c64/vic.h>
#include <c64/memmap.h>
#include <c64/cia.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>		//for memset()

#define NUM_SPRITES 9
//
// Example
//
struct {
	int		x;
	byte	y;
} sprites[NUM_SPRITES];

// const char* hello_text = "HELLO, WORLD!";
char* screen = (char*) (0x8000);
unsigned char* color = (unsigned char*) (0xd80e);

void init_screen(byte num_stars);

int main() {
	//MUST BE THE FIRST INSTRUCTION
	mmap_trampoline();
	mmap_set(MMAP_NO_ROM);

	//iocharmap(IOCHM_PETSCII_1);
	// while(true);

    // Disable CIA interrupts, we do not want interference
	// with our joystick interrupt
	cia_init();

	init_screen(25);

	//printf("Hello World!\n");
	//while(true);

	// enable raster interrupt via direct path
	rirq_init(false);

	// initialize sprite multiplexer
	vspr_init(screen);

	for (int i=0;i<NUM_SPRITES;i++) {
		//vspr_set(i,25+i*20,rand()%50,0x10,VCOL_YELLOW);		
		sprites[i].x=25+i*20;
		sprites[i].y=rand()%50;
		vspr_set(i,sprites[i].x,sprites[i].y,0x10,VCOL_YELLOW);
	}
	//vspr_set(0,160,100,0x10,VCOL_YELLOW);

		// initial sort and update
	vspr_sort();
	vspr_update();
	rirq_sort();

	rirq_start();
	
	while (true) {
		for (int y=0;y<200;y+=1) {
			for (int i=0;i<NUM_SPRITES;i++) {
				sprites[i].y++;
				vspr_move(i,sprites[i].x,sprites[i].y);
				//vspr_movey(i,y+50);
			}

			// sort virtual sprites by y position
			vspr_sort();

			// wait for raster IRQ to reach and of frame
			rirq_wait();

			// update sprites back to normal and set up raster IRQ for sprites 8 to 31
			vspr_update();

			// sort raster IRQs
			rirq_sort();
		}
	}
	//for(;;);

    return 0;
}

void init_screen(byte num_stars) {
	vic_setmode(VICM_TEXT,screen,(byte *)0x1000); //TODO is this correct?

	memset(screen,0x20,1000);
	memset(color,1,1000);

	vic.color_border=VCOL_BLACK;
	vic.color_back=VCOL_BLACK;

	//NOTE: can't unroll this loop, since the bounds are not constant
	//#pragma unroll(full)
	for(byte i=0;i<num_stars;i++) {
		unsigned int pos;
		byte* text=(byte*)0x8000;//0x4000;	//FIXME remove magic number
		byte* color=(byte*)(0xd800);

		while ( text[pos=40+(rand() % 960)] != 32);
		text[pos]=46; //screencode for "."
		color[pos]=rand() % 16;
	}

	// gotoxy(0,0);
	// printf("LIVES:XXXXX");

	// gotoxy(15,0);
	// printf("INVADERS");

	// gotoxy(27,0);
	// printf("SCORE:000000");

}