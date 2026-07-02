#define SPRITE_FILE 			"resources/sabotage64-0.3.7.spd"
#define PLAY_SCREEN_FILE 		"resources/sabotage64 text 0.7.1.bin"
#define CHAR_FILE				"resources/Sabotage64 (Hybrid) - Main 0.9 - Chars.bin"
#define PLAY_SCREEN_COLOR_FILE 	"resources/Sabotage64 (Hybrid) - Main - ColorMap 0.9.ctm"

#include <c64/types.h>

// #pragma section(music_sec, 0)
// #pragma region(music_reg, 0x1000, 0x1fff,,, {music_sec})

// #pragma section(logo_screen_sec, 0)
// #pragma section(logo_color_sec, 0)
// #pragma section(logo_bmp_sec, 0)

// #pragma region(logo_bmp_reg, 0x4000, 0x5fff,,,{logo_bmp_sec})
// #pragma region(logo_screen_reg, 0x6000, 0x63ff,,,{logo_screen_sec})
// #pragma region(logo_color_reg, 0x6400, 0x6fff,,,{logo_color_sec})

#pragma region( lower, 0xa00, 0x1200, , , {code} )

//TODO Get a SID version of reveille working
// #pragma section(sid_sec,0)
// #pragma region(sid_sec, 0x0ff6, 0x1fff,,,{sid_sec})
// #pragma data(sid_sec)
// const char music[] = {
// 	#embed 0x01c3 0x7e "DoReMi1000.sid"
// };
// #pragma reference(music)

#pragma data(data)
#include "title_screen.h"

////
// load binary files for different screens, to be copied to the VIC screens when necessary
///

//We don't have to put our stored screens, etc. in any particular place, since they just get
//	copied to the right place when we need them.
const char game_screen[] = { 
	#embed PLAY_SCREEN_FILE 
};
 #pragma reference(game_screen)

 const char game_screen_color[] = {	
	#embed PLAY_SCREEN_COLOR_FILE 
};
 #pragma reference(game_screen_color)

 const char stored_spriteset[] =  { 
	#embed spd_sprites SPRITE_FILE 
};
#pragma reference(stored_spriteset);

const char stored_charset[] = {	
//	#embed ctm_chars CHAR_FILE 
	#embed CHAR_FILE
};
#pragma reference(stored_charset)

#pragma region( main, 0x30e8, 0x7fff, , , {code, data, bss} )

#pragma section(charset_sec,0)
#pragma region(charset_reg,0x8000,0x8800,,,{charset_sec})

#pragma section(screen_sec, 0)
#pragma region(screen_reg,0x8800,0x8c00,,,{screen_sec})

#pragma region(middle, 0x8c00, 0xa000,,,{stack,code, data, bss})

//NOTE:we can't put our sprites at $8c00, since they go on to like $9100,
//	and in this memory config the default character set gets mapped to $9000
#pragma section(spriteset_sec, 0)
#pragma region(spriteset_reg, 0xa000, 0xa500,,, {spriteset_sec} )

#pragma region( upper, 0xa500, 0xcfff, , , {heap, code, data, bss} )


// #pragma section(color_sec, 0)
// #pragma region(color_reg, 0xd800, 0xdc00,,,{color_sec})
// #pragma data(color_sec)
// char color[0x3e8];
// #pragma reference(color)

//#define LOGO_FILE 	"resources/space_invaders_logo.kla"
//#define MUSIC_FILE 	"resources/Space_Invaders_Medley.sid"

// #pragma data(logo_bmp_sec)
// __export const char const logo_bmp[] = {
// 	#embed 8000 2 LOGO_FILE  
// };

// //#section
// #pragma data(logo_screen_sec)
// __export static char logo_screen[1000] = {
// 	#embed 1000 9002 LOGO_FILE
// };
// //#endsection

// #pragma data(logo_color_sec)
// //load the text & color screens into
// __export static char logo_color[1000] = {
// 	#embed 1000 8002 LOGO_FILE
// };

// Set the data segment to be the newly created music section,
// so all initialized data gets placed there by the linker
// #pragma data(music_sec)

// Make sure that the music data is not dropped by the linker for
// not being referenced with the "__export" storage qualifier.
// We also drop the first 0x7e bytes of the SID file, they contain
// only file level meta data
// __export const char music[] = {
// 	#embed 0x2000 0x7e MUSIC_FILE 
// };

#pragma data(data)

char *charset=(char *)0x8000;
#pragma reference(charset)

const int SCREEN_LOC=0x8800;
char *screen=(char *)SCREEN_LOC;
#pragma reference(screen)

char *spriteset=(char *)0xa000;
#pragma reference(spriteset)