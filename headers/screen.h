#ifndef SCREEN_H
#define SCREEN_H

#include "../headers/types.h"
#include "../headers/low_level.h"

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define VGA_MAX_CHAR 2000


// Attribute byte for our default colour scheme .
#define WHITE_ON_BLACK	0x0f

#define BLACK		0x0
#define BLUE		0x1
#define GREEN		0x2
#define CYAN		0x3
#define RED		0x4
#define PURPLE		0x5
#define BROWN		0x6
#define GRAY		0x7
#define DARK_GRAY	0x8
#define LIGHT_BLUE	0x9
#define LIGHT_GREEN	0xa
#define LIGHT_CYAN	0xb
#define LIGHT_RED	0xc
#define LIGHT_PURPLE	0xd
#define YELLOW		0xe
#define WHITE		0xf

typedef struct screens {
	u8	c_cols;
	u8	c_rows;
	u8	buffer[VGA_MAX_CHAR * 2];
} screen_t;


// Global Color selector
extern	s32 BG_COLOR;
extern	s32 FG_COLOR;

int	arr[screens];

// print functions
void	print_char(u8 c, u8 color);
void	print_string(s8 *str, u8 color);
void	print_int(s32 num);
void	print_hex(u32 num);

// screen operations functions
void	clear_screen(void);
void	scroll_screen(void);
void	mem_move(u16 *dst, u16 *src, u8 len);
void	backspace(u8 color);
void	switch_screen(s8 screen_id);

// arrows functions
void	arrow_up(void);
void	arrow_down(void);
void	arrow_right(void);
void	arrow_left(void);

// Screen device I/O ports
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

#endif
