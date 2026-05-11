#include "../headers/screen.h"

s32 BG_COLOR = BLACK;
s32 FG_COLOR = CYAN;

static u8	c_rows = 0;
static u8	c_cols = 0;

void	clear_screen(void) {
	u16	*video = (u16 *)VIDEO_ADDRESS;
	u16	blank = 0x0F20;

	c_rows = 0;
	c_cols = 0;
	move_cursor(0);

	for (u16 i = 0; i < VGA_MAX_CHAR; i++) {
		video[i] = blank;
	}
}

void	print_hex(u32 num) {
	char	hex_chars[] = "0123456789ABCDEF";
	char	buffer[11] = "0x00000000";

	for (int i = 9; i >= 2; i--) {
	buffer[i] = hex_chars[num & 0xF];
	num >>= 4;
	}

	print_string((u8 *)buffer, WHITE_ON_BLACK);
}

void	print_int(s32 num) {
	char	buffer[12];  // -2147483648 max length
	int	i = 0;
	int	is_negative = 0;

	if (num == 0) {
		print_char('0', WHITE_ON_BLACK);
		return;
	}

	if (num < 0) {
		is_negative = 1;
		num = -num;
	}
	
	while (num > 0) {
		buffer[i++] = '0' + (num % 10);
		num /= 10;
	}

	if (is_negative) {
		print_char('-', WHITE_ON_BLACK);
	}

	while (i > 0) {
		print_char(buffer[--i], WHITE_ON_BLACK);
	}
}

void	print_char(u8 c, u8 color) {
	volatile u8 *video = (volatile u8 *)VIDEO_ADDRESS + (c_rows * MAX_COLS + c_cols) * 2;

	if (c == '\n') {
		c_rows++;
		c_cols = 0;
		if (c_rows >= MAX_ROWS) {
			c_rows--;
			scroll_screen();
		}
		move_cursor((c_rows * MAX_COLS + c_cols));
		return ;
	}
	else if (c == '\b') {
		backspace(color);
		return ;
	}
	*video++ = c;
	*video = color;
	c_cols++;
	if (c_cols >= MAX_COLS) {
		c_cols = 0;
		c_rows++;
		if (c_rows >= MAX_ROWS) {
			c_rows--;
			scroll_screen();
		}
	}
	move_cursor((c_rows * MAX_COLS + c_cols));
}

void	print_string(s8 *str, u8 color) {
	//volatile u8 *video = (volatile u8 *)VIDEO_ADDRESS + (c_rows * MAX_COLS + c_cols);

	while (*str != '\0') {
		print_char(*str, color);
		str++;
		//*video++ = *str++;
		//*video++ = color;
	}
}

void	scroll_screen(void) {
	for (u8 i = 1; i < MAX_ROWS; i++){
		mem_move(((u16 *)VIDEO_ADDRESS + ((i - 1) * MAX_COLS)), ((u16 *)VIDEO_ADDRESS + i * MAX_COLS), MAX_COLS);
	}
}

void	mem_move(u16 *dst, u16 *src, u8 len) {
	u8 i = 0;
	while (i < len * 2) {
		dst[i] = src[i];
		i++;
	}	
}

static void	find_end_c_cols(void) {
	volatile u8 *video = (volatile u8 *)VIDEO_ADDRESS + (--c_rows * MAX_COLS + 0) * 2;
	u8	i = 0;

	c_cols = 0;
	while (*video != '\n' && c_cols < MAX_COLS) {
		if (*video == ' ') {
			video += 2;
			i++;
			while (*video == ' ') {
				i++;
				video += 2;
				if (i >= MAX_COLS)
					return ;
			}
		}
		else {
			video += 2;	
			i++;
			c_cols = i;
		}
	}
}

void	backspace(u8 color) {
	if (c_cols > 0)
		c_cols--;
	else if (c_cols == 0 && c_rows > 0)
		find_end_c_cols();
	move_cursor((c_rows * MAX_COLS + c_cols));

	volatile u8 *video = (volatile u8 *)VIDEO_ADDRESS + (c_rows * MAX_COLS + c_cols) * 2;
	*video++ = ' ';
	*video = color;
}
