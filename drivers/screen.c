#include "../headers/screen.h"

s32 BG_COLOR = BLACK;
s32 FG_COLOR = CYAN;

static u32		c_rows = 0;
static u32		c_cols = 0;
static screen_t		display[3];
s32			screen_tracker = 0;

void	clear_buffers(void) {
	for (u32 j = 0; j < 3; j++) {
		for (u32 i = 0; i < VGA_MAX_CHAR * 2; i++) {
			display[j].buffer[i++] = 0x20;
			display[j].buffer[i] = GREEN;
		}
	}
}

void	clear_screen(void) {
	u16	*video = (u16 *)VIDEO_ADDRESS;
	u16	blank = 0x0F20;

	c_rows = 0;
	c_cols = 0;
	move_cursor(0);

	for (u32 i = 0; i < VGA_MAX_CHAR; i++) {
		*video++ = blank;
	}
}

void	print_hex(u32 num) {
	char	hex_chars[] = "0123456789ABCDEF";
	char	buffer[11] = "0x00000000";

	for (int i = 9; i >= 2; i--) {
	buffer[i] = hex_chars[num & 0xF];
	num >>= 4;
	}

	print_string((s8 *)buffer, WHITE_ON_BLACK);
}

void	print_int(s32 num) {
	char	buffer[12];
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

void	switch_screen(void) {
	clear_screen();
	volatile u8 *video = (volatile u8 *)VIDEO_ADDRESS;
	for (u32 i = 0; i < VGA_MAX_CHAR * 2; i += 2) {
		*video++ = display[screen_tracker].buffer[i];
		*video++ = display[screen_tracker].buffer[i + 1];
	}
	c_cols = display[screen_tracker].c_cols;
	c_rows = display[screen_tracker].c_rows;
	move_cursor((c_rows * MAX_COLS + c_cols));
}

void	print_char(u8 c, u8 color) {
	volatile u8 *video = (volatile u8 *)VIDEO_ADDRESS + (c_rows * MAX_COLS + c_cols) * 2;
	u32	i = (c_rows * MAX_COLS + c_cols) * 2;

	if (c == '\n') {
		c_rows++;
		c_cols = 0;
		if (c_rows >= MAX_ROWS) {
			c_rows--;
			scroll_screen();
		}
		move_cursor((c_rows * MAX_COLS + c_cols));
		display[screen_tracker].c_cols = c_cols;
		display[screen_tracker].c_rows = c_rows;
		return ;
	}
	else if (c == '\b') {
		backspace(color);
		return ;
	}
	*video++ = c;
	*video = color;
	display[screen_tracker].buffer[i++] = c;
	display[screen_tracker].buffer[i] = color;

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
	display[screen_tracker].c_cols = c_cols;
	display[screen_tracker].c_rows = c_rows;
}

void	print_string(s8 *str, u8 color) {
	while (*str != '\0') {
		print_char(*str, color);
		str++;
	}
}

void	scroll_screen(void) {
	volatile u8 *video = (volatile u8 *)VIDEO_ADDRESS;
	for (u32 i = 1; i < MAX_ROWS; i++){
		mem_move(((u16 *)VIDEO_ADDRESS + ((i - 1) * MAX_COLS)), ((u16 *)VIDEO_ADDRESS + i * MAX_COLS), MAX_COLS);
	}
	for (u32 i = 0; i < VGA_MAX_CHAR * 2; i += 2) {
		display[screen_tracker].buffer[i] = *video++;
		display[screen_tracker].buffer[i + 1] = *video++;
	}
}

void	mem_move(u16 *dst, u16 *src, u8 len) {
	u8 i = 0;
	while (i < len * 2) {
		dst[i] = src[i];
		i++;
	}	
}

static _bool	find_end_c_cols(u8 arrow) {
	volatile u8 *video = (volatile u8 *)VIDEO_ADDRESS + (c_rows * MAX_COLS) * 2;
	u8	i = 0;
	u8	cols_state = c_cols;

	if (arrow == 2) {
		i = c_cols;
		video = (volatile u8 *)VIDEO_ADDRESS + (c_rows * MAX_COLS + c_cols) * 2; 
		while (*video == ' ') {
			video += 2;
			i++;
			if (i >= MAX_COLS)
				return true;
		}
		return false;
	}
	c_cols = 0;
	while (i < MAX_COLS) {
		while (*video == ' ') {
			video += 2;
			i++;
			if (i >= MAX_COLS) {
				if (c_cols > cols_state && arrow)
					c_cols = cols_state;
				return false;
			}
		}
		video += 2;
		c_cols = i;
		i++;
	}
	if (c_cols > cols_state && arrow)
		c_cols = cols_state;
	return false;
}

void	backspace(u8 color) {
	if (c_cols > 0)
		c_cols--;
	else if (c_cols == 0 && c_rows > 0) {
		c_rows--;
		find_end_c_cols(0);
	}
	move_cursor((c_rows * MAX_COLS + c_cols));
	display[screen_tracker].c_cols = c_cols;
	display[screen_tracker].c_rows = c_rows;

	volatile u8 *video = (volatile u8 *)VIDEO_ADDRESS + (c_rows * MAX_COLS + c_cols) * 2;
	volatile u8 *display_buff = (volatile u8 *)&display[screen_tracker].buffer[0] + (c_rows * MAX_COLS + c_cols) * 2;
	*video++ = ' ';
	*video = color;
	*display_buff++ = ' ';
	*display_buff = color;
}


/*
 			- - - Arrow Functions ---
 */

void	arrow_up(void) {
	if (c_rows > 0) {
		c_rows--;
		find_end_c_cols(1);
		move_cursor((c_rows * MAX_COLS + c_cols));
		display[screen_tracker].c_cols = c_cols;
		display[screen_tracker].c_rows = c_rows;
	}
	return ;
}

void	arrow_down(void) {
	if (c_rows < MAX_ROWS - 1) {
		c_rows++;
		find_end_c_cols(1);

		move_cursor((c_rows * MAX_COLS + c_cols));
		display[screen_tracker].c_cols = c_cols;
		display[screen_tracker].c_rows = c_rows;
	}
	return ;
}

void	arrow_left(void) {
	if (c_cols > 0) {
		c_cols--;
		move_cursor((c_rows * MAX_COLS + c_cols));
		display[screen_tracker].c_cols = c_cols;
		display[screen_tracker].c_rows = c_rows;
	}
	else if (c_cols == 0 && c_rows > 0) {
		c_rows--;
		find_end_c_cols(0);
		move_cursor((c_rows * MAX_COLS + c_cols));
		display[screen_tracker].c_cols = c_cols;
		display[screen_tracker].c_rows = c_rows;
	}
	return ;
}

void	arrow_right(void) {
	if ((c_cols + 1) < MAX_COLS) {
		c_cols++;
		u8 check = find_end_c_cols(2);
		if (check && c_rows < MAX_ROWS - 1) {
			c_rows++;
			c_cols = 0;
			move_cursor((c_rows * MAX_COLS + c_cols));
			display[screen_tracker].c_cols = c_cols;
			display[screen_tracker].c_rows = c_rows;
		}
		else if (c_cols < MAX_COLS && (c_rows < MAX_ROWS - 1 || !check)) {
			move_cursor((c_rows * MAX_COLS + c_cols));
			display[screen_tracker].c_cols = c_cols;
			display[screen_tracker].c_rows = c_rows;
		}
		else
			c_cols--;
	}
	else if (c_cols == MAX_COLS - 1 && c_rows < MAX_ROWS - 1) {
		c_rows++;
		c_cols = 0;
		move_cursor((c_rows * MAX_COLS + c_cols));
		display[screen_tracker].c_cols = c_cols;
		display[screen_tracker].c_rows = c_rows;
	}
	return ;
}

void	print_buffer_sc(void) {
	print_int(screen_tracker);
	print_char('\n', (BG_COLOR << 4) + FG_COLOR);
	print_int(c_rows);
	print_char('\n', (BG_COLOR << 4) + FG_COLOR);
	print_int(display[screen_tracker].c_rows);
	print_char('\n', (BG_COLOR << 4) + FG_COLOR);
	print_hex(display[screen_tracker].buffer[0]);
	print_char('\n', (BG_COLOR << 4) + FG_COLOR);
	print_int(display[screen_tracker].buffer[3]);
	print_char('\n', (BG_COLOR << 4) + FG_COLOR);
	print_int((BG_COLOR << 4) + FG_COLOR);
	print_char('\n', (BG_COLOR << 4) + FG_COLOR);
	for (u8 i = 0; i < 200; i += 2) {
		print_char(display[0].buffer[i], display[0].buffer[i + 1]);
	}
}
