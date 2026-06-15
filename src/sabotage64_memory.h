#define SPRITE_FILE 		"resources/sabotage64-0.3.6.spd"
#define PLAY_SCREEN_FILE 	"resources/sabotage64 text 0.5.bin"
#define CHAR_FILE			"resources/Sabotage64 (ROM charset) 0.4.ctm"

//#pragma region( lower, 0xa00, 0x0fff, , , {code} )


// #pragma section(music_sec, 0)
// #pragma region(music_reg, 0x1000, 0x1fff,,, {music_sec})

// #pragma section( middle_sec, 0)
// #pragma region(middle_reg, 0x2000, 0x3fff,,, {code})

// #pragma section(logo_screen_sec, 0)
// #pragma section(logo_color_sec, 0)
// #pragma section(logo_bmp_sec, 0)

// #pragma region(logo_bmp_reg, 0x4000, 0x5fff,,,{logo_bmp_sec})
// #pragma region(logo_screen_reg, 0x6000, 0x63ff,,,{logo_screen_sec})
// #pragma region(logo_color_reg, 0x6400, 0x6fff,,,{logo_color_sec})

//#pragma region( lower, 0xa00, 0x0fff, , , {code} )

#pragma section(stored_charset_sec,0)
//#pragma region(charset_reg,0x8000,0x8800,,,{charset_rec})
#pragma region(stored_charset_reg,0x0a00,0x1200,,,{stored_charset_sec})
#pragma data(stored_charset_sec)
const char stored_charset[] = {
	#embed ctm_chars CHAR_FILE
};
#pragma reference(stored_charset)

#pragma section(title_screen_sec,0)
//#pragma region(title_screen_reg, x08c00, 0x8fe8,,,{title_screen_sec})
#pragma region(title_screen_reg, 0x1200, 0x15e8,,, {title_screen_sec})
#pragma data(title_screen_sec)
const char title_screen[] = {
	#embed PLAY_SCREEN_FILE
};
#pragma reference(title_screen)

#pragma section(stored_spriteset_sec,0)
#pragma region(stored_spriteset_reg, 0x15e8,0x1ae8,,,{stored_spriteset_sec})
#pragma data(stored_spriteset_sec)
const char stored_spriteset[] =  {
	#embed spd_sprites SPRITE_FILE

};

#pragma region( main, 0x1e00, 0x7fff, , , {heap, stack, code, data, bss} )


#pragma section(charset_sec,0)
#pragma region(charset_reg,0x8000,0x8800,,,{charset_sec})
// #pragma data(charset_sec)
// char charset[0x800];
// #pragma reference(charset)

#pragma section(screen_sec, 0)
#pragma region(screen_reg,0x8800,0x8c00,,,{screen_sec})
// #pragma data(screen_sec)
// char screen[0x3e8];// = {
// // 	#embed SCREEN_FILE
// // };
// #pragma reference(screen)

#pragma region(middle, 0x8c00, 0xa000,,,{code, data, bss})

//NOTE:we can't put our sprites at $8c00, since they go on to like $9100,
//	and in this memory config the default character set gets mapped to $9000
#pragma section(spriteset_sec, 0)
#pragma region(spriteset_reg, 0xa000, 0xa500,,, {spriteset_sec} )
// #pragma data(spriteset_sec)
// const char spriteset[0x500];// =  {
// // 	#embed spd_sprites SPRITE_FILE
// // };
// #pragma reference(spriteset)

#pragma region( upper, 0x9a00, 0xcfff, , , {heap, stack, code, data, bss} )

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

char *screen=(char *)0x8800;
#pragma reference(screen)

char *spriteset=(char *)0xa000;
#pragma reference(spriteset)