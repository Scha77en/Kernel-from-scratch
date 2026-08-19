#include "../headers/screen.h"

s32 BG_COLOR = BLACK;
s32 FG_COLOR = GRAY;

struct gdtr_pointer {
    u16 limit;
    u32 base;
} __attribute__((packed));

static u32		c_rows = 0;
static u32		c_cols = 0;
static screen_t		display[3];
static bool		disable_scroll = false;
static u8		display_count = 0;

s32			screen_tracker = 0;

u8	screen_color[3] = {YELLOW, LIGHT_RED, LIGHT_PURPLE};

static u32	strlen(const u8 *str) {
	u32	i = 0;

	while (str[i])
		i++;
	return i;
}

static int	strcmp(const char * s1, const char *s2, u32 n) {
	const char * ps1 = s1;
	const char * ps2 = s2;

	while ((*ps1 ||* ps2 )&& *ps1 == *ps2) {
		ps2++;
		ps1++;
    	}
	return *ps1 - *ps2;
}

static	void	memcpy(u8 *dst, u8 *src, u32 n) {
	u32	i = 0;

	if (src == 0 && dst == 0)
		return ;
	while (i < n) {
		dst[i] = src[i];
		i++;
	}
}

static void	trim_cmd_spaces(void) {
	u8	i = 0;
	while (display[screen_tracker].cmd[i] != '\0')
		i++;
	for (s8 k = i - 1; k > 0; k--) {
		if (display[screen_tracker].cmd[k] != ' ') {
			display[screen_tracker].cmd[k + 1] = '\0';
			return ;
		}
		continue ;
	}
}

static void	handle_help(void) {
	print_string("Available commands:\n", (BG_COLOR << 4) + LIGHT_GREEN);
	print_string("help       --> Display this help menu\n", (BG_COLOR << 4) + YELLOW);
	print_string("FG <color> --> Change the foreground color\n", (BG_COLOR << 4) + BROWN);
	print_string("clear      --> Clear the screen buffer\n", (BG_COLOR << 4) + RED);
	print_string("p_stack    --> Print the stack bytes\n", (BG_COLOR << 4) + LIGHT_BLUE);
	print_string("gdt        --> Print the address and content of gdt\n", (BG_COLOR << 4) + CYAN);
	print_string("reboot     --> Reboot the kernel\n", (BG_COLOR << 4) + LIGHT_PURPLE);
	print_string("poweroff   --> Turn off the kernel\n", (BG_COLOR << 4) + PURPLE);
}

static void	fg_wrong_cmd(void) {
	print_string("wrong command argument, correct arguments:\n", (BG_COLOR << 4) + LIGHT_RED);
	print_string("FG BLACK\n", (BG_COLOR << 4) + DARK_GRAY);
	print_string("FG WHITE\n", (BG_COLOR << 4) + WHITE);
	print_string("FG YELLOW\n", (BG_COLOR << 4) + YELLOW);
	print_string("FG BLUE\n", (BG_COLOR << 4) + BLUE);
	print_string("FG GREEN\n", (BG_COLOR << 4) + GREEN);
	print_string("FG CYAN\n", (BG_COLOR << 4) + CYAN);
	print_string("FG RED\n", (BG_COLOR << 4) + RED);
	print_string("FG PURPLE\n", (BG_COLOR << 4) + PURPLE);
	print_string("FG BROWN\n", (BG_COLOR << 4) + BROWN);
	print_string("FG GRAY\n", (BG_COLOR << 4) + GRAY);
	print_string("FG DARK_GRAY\n", (BG_COLOR << 4) + DARK_GRAY);
	print_string("FG LIGHT_BLUE\n", (BG_COLOR << 4) + LIGHT_BLUE);
	print_string("FG LIGHT_GREEN\n", (BG_COLOR << 4) + LIGHT_GREEN);
	print_string("FG LIGHT_CYAN\n", (BG_COLOR << 4) + LIGHT_CYAN);
	print_string("FG LIGHT_RED\n", (BG_COLOR << 4) + LIGHT_RED);
	print_string("FG LIGHT_PURPLE\n", (BG_COLOR << 4) + LIGHT_PURPLE);
}

static void	handle_fg(void) {
	trim_cmd_spaces();
	if (display[screen_tracker].cmd[2] != ' ') {
		fg_wrong_cmd();
		return ;
	}
	s8	*colors[16] = {"BLACK", "WHITE", "YELLOW", "BLUE", "GREEN", "CYAN", "RED", \
				"PURPLE", "BROWN", "GRAY", "DARK_GRAY", "LIGHT_BLUE", "LIGHT_GREEN", \
				"LIGHT_CYAN", "LIGHT_RED", "LIGHT_PURPLE"};
	u32	fg_colors[16] = {BLACK, WHITE, YELLOW, BLUE, GREEN, CYAN, RED, PURPLE, BROWN, GRAY, \
			DARK_GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, LIGHT_PURPLE};
	for (u8 i = 0; i < 16; i++) {
		if (!strcmp(&display[screen_tracker].cmd[3], colors[i], strlen(colors[i]))) {
			FG_COLOR = fg_colors[i];
			display[screen_tracker].S_FG_C = FG_COLOR;
			print_string("FG color changed successfully!\n", (BG_COLOR << 4) + FG_COLOR);
			return ;
		}
	}
	fg_wrong_cmd();
}

static void	handle_clear(void) {
	clear_screen();
	for (u32 i = 0; i < VGA_MAX_CHAR * 2; i++) {
		display[screen_tracker].buffer[i++] = 0x20;
		display[screen_tracker].buffer[i] = GRAY;
	}
}

static void	handle_reboot(void) {
	outb(0xCF9, 0xE);
}

static void	print_stack(void) {
	print_stack_dump();
}

static void	print_gdt(void) {
    struct gdtr_pointer gdtr;

    asm volatile("sgdt %0" : "=m"(gdtr));

    printk("=== CPU GDT LIVE HARDWARE STATE ===\n");
    printk("GDT Base Address : %x\n", gdtr.base);
    printk("GDT Limit (Size) : %d bytes\n", gdtr.limit + 1);

    int total_entries = (gdtr.limit + 1) / 8;
    printk("Total Entries    : %d\n\n", total_entries);

    u32* gdt_memory = (u32*)gdtr.base;

    printk("    Selector     |  Raw Hex Value (High / Low) | Guess Type\n");
    printk("-----------------|-----------------------------|------------\n");

    for (int i = 0; i < total_entries; i++) {
        u32 low_32  = gdt_memory[i * 2];
        u32 high_32 = gdt_memory[(i * 2) + 1];

        u16 selector = i * 8;
int strcmp(const char * s1, const char *s2){
    const char * ps1 = s1;
    const char * ps2 = s2;
    while ((*ps1 ||* ps2 )&& *ps1 == *ps2){
        ps2++;
        ps1++;
    }
    return *ps1 - *ps2;
}
        const char* type = "Unknown";
        if (i == 0) type = "Null Descriptor";
        else if (i == 1) type = "Kernel Code (Ring 0)";
        else if (i == 2) type = "Kernel Data (Ring 0)";
        else if (i == 3) type = "Kernel Stack (Ring 0)";
        else if (i == 4) type = "User Code (Ring 3)";
        else if (i == 5) type = "User Data (Ring 3)";
        else if (i == 6) type = "User Stack (Ring 3)";

        printk("    %x   |    %x %x    | %s\n", selector, high_32, low_32, type);
    }
}

static void	power_off(void) {
	__asm__ volatile ("cli");
	outw(0x604, 0x2000);
}

void	clear_buffers(void) {
	for (u32 j = 0; j < 3; j++) {
		for (u32 i = 0; i < VGA_MAX_CHAR * 2; i++) {
			display[j].buffer[i++] = 0x20;
			display[j].buffer[i] = GRAY;
		}
	}
	//screen_tracker_d();
	print_string("kfs>", (BG_COLOR << 4) + YELLOW);
	for (u8 i = 0; i < 8; i++) {
		if (i == 0)
			display[1].buffer[i++] = 'k';
		else if (i == 2)
			display[1].buffer[i++] = 'f';
		else if (i == 4)
			display[1].buffer[i++] = 's';
		else if (i == 6)
			display[1].buffer[i++] = '>';
		display[1].buffer[i] = (BG_COLOR << 4) + LIGHT_RED;
	}
	for (u8 i = 0; i < 8; i++) {
		if (i == 0)
			display[2].buffer[i++] = 'k';
		else if (i == 2)
			display[2].buffer[i++] = 'f';
		else if (i == 4)
			display[2].buffer[i++] = 's';
		else if (i == 6)
			display[2].buffer[i++] = '>';
		display[2].buffer[i] = (BG_COLOR << 4) + LIGHT_PURPLE;
	}
	for (u8 i = 0; i < 3; i++) {
		display[i].c_cols = 4;
		display[i].S_FG_C = GRAY;
	}
}

// ------------------------------

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
	if (display_count < 3) {
		screen_tracker_d();
		display_count++;
	}
	volatile u8 *video = (volatile u8 *)VIDEO_ADDRESS;
	for (u32 i = 0; i < VGA_MAX_CHAR * 2; i += 2) {
		*video++ = display[screen_tracker].buffer[i];
		*video++ = display[screen_tracker].buffer[i + 1];
		//if (i == VGA_MAX_CHAR * 2 - 4)
			//printk("[%c]", display[screen_tracker].buffer[i]);
	}
	c_cols = display[screen_tracker].c_cols;
	c_rows = display[screen_tracker].c_rows;
	move_cursor((c_rows * MAX_COLS + c_cols));
	FG_COLOR = display[screen_tracker].S_FG_C;
	for (u32 i = 0; i < VGA_MAX_CHAR * 2; i++) {
		//if (i % 2 == 0 && i > VGA_MAX_CHAR * 2 - 14)
			//if (!getascii(display[screen_tracker].buffer[i]))
				//printk("null azbi hda");
			//else
				//printk("[%c]", display[screen_tracker].buffer[i]);
	}
}

void	printk(char *format, ...) {
	va_list args;
	va_start(args, format);

	for (char *p = format; *p != '\0'; p++) {
		if (*p == '%' && *(p + 1) != '\0') {
			p++;
			switch (*p) {
				case 'd': {
					int val = va_arg(args, int);
					print_int(val);
					break;
				}
				case 'u': {
					unsigned int val = va_arg(args, unsigned int);
					print_int((int)val);
					break;
				}
				case 's': {
					char *str = va_arg(args, char *);
					if (!str) str = "(null)";
					print_string(str, (BG_COLOR << 4) + FG_COLOR);
					break;
				}
				case 'x': {
					unsigned int val = va_arg(args, unsigned int);
					print_hex(val);
					break;
				}
				case 'c': {
					char c = (char)va_arg(args, int);
					print_char(c, (BG_COLOR << 4) + FG_COLOR);
					break;
				}
				case '%': {
					print_char('%', (BG_COLOR << 4) + FG_COLOR);
					break;
				}
				default:
					print_char('%', (BG_COLOR << 4) + FG_COLOR);
					print_char(*p, (BG_COLOR << 4) + FG_COLOR);
					break;
			}
		} else 
			print_char(*p, (BG_COLOR << 4) + FG_COLOR);
	}
	va_end(args);
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
		if (cmd) {
			handle_command();
			print_string("kfs>", (BG_COLOR << 4) + screen_color[screen_tracker]);
		}
		return ;
	}
	else if (c == '\b') {
		backspace(color);
		return ;
	}
	if (cmd) {
		if ((c_cols + 1) >= MAX_COLS)
			return ;
		display[screen_tracker].cmd[display[screen_tracker].c_cols - 4] = c;
	}
	*video++ = c;
	*video = color;
	display[screen_tracker].buffer[i++] = c;
	display[screen_tracker].buffer[i] = color;
	
	c_cols++;
	if (c_cols >= MAX_COLS) {
		c_cols = 0;
		c_rows++;
		if (c_rows >= MAX_ROWS && !disable_scroll) {
			c_rows--;
			scroll_screen();
		}
	}
	move_cursor((c_rows * MAX_COLS + c_cols));
	if (!disable_scroll) {
		display[screen_tracker].c_cols = c_cols;
		display[screen_tracker].c_rows = c_rows;
	}
}

void	print_string(s8 *str, u8 color) {
	while (*str != '\0') {
		print_char(*str, color);
		str++;
	}
}

void	handle_command(void) {
	cmd = false;
	u8	*cmd_buf = display[screen_tracker].cmd;
	u8	command[20];
	u8	i = 0;

	while (cmd_buf[i] && cmd_buf[i] != ' ') {
		command[i] = cmd_buf[i];
		i++;
	}
	command[i] = '\0';

	//printk("cmd == [%s]\n", command);
	if (!strcmp(command, "FG", strlen("FG")))
		handle_fg();
	else if (!strcmp(command, "clear", strlen("clear")))
		handle_clear();
	else if (!strcmp(command, "p_stack", strlen("p_stack")))
		print_stack();
	else if (!strcmp(command, "reboot", strlen("reboot")))
		handle_reboot();
	else if (!strcmp(command, "gdt", strlen("gdt")))
		print_gdt();
	else if (!strcmp(command, "help", strlen("help")))
		handle_help();
	else if (!strcmp(command, "poweroff", strlen("poweroff")))
		power_off();
	else
		print_string("command not found!\n", (BG_COLOR << 4) + RED);
	for (u32 i = 0; i < 76; i++)
	    display[screen_tracker].cmd[i] = '\0';
	return ;
}

void	scroll_screen(void) {
	volatile u8 *video = (volatile u8 *)VIDEO_ADDRESS;
	volatile u8 *empty_this = (volatile u8 *)VIDEO_ADDRESS + ((MAX_ROWS - 1) * MAX_COLS) * 2;
	for (u32 i = 1; i < MAX_ROWS - 1; i++){
		mem_move(
			((u16 *)VIDEO_ADDRESS + ((i - 1) * MAX_COLS)),
			((u16 *)VIDEO_ADDRESS + i * MAX_COLS),
			MAX_COLS);
	}

	u8	empty_buf[MAX_COLS * 2];

	for (u32 i = 0; i < MAX_COLS * 2; i += 2) {
		empty_buf[i] = ' ';
		empty_buf[i + 1] = WHITE_ON_BLACK;
	}

	mem_move(
		(u16 *)empty_this,
		(u16 *)empty_buf,
		MAX_COLS / 2);

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
	if (c_cols > 4) {
		display[screen_tracker].cmd[c_cols - 5] = '\0';
		c_cols--;
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

void	screen_tracker_d(void) {
	u32	cols_tmp = c_cols;
	u32	rows_tmp = c_rows;
	c_cols = 0;
	c_rows = 24;

	disable_scroll = true;
	for (u32 i = 0; i < MAX_COLS - strlen("qwerty") ; i++) {
		switch (c_cols) {
			case 37:
				if (screen_tracker == 0) {
					BG_COLOR = screen_color[screen_tracker];
					FG_COLOR = BLACK;
				}
				print_char('1', (BG_COLOR << 4) + FG_COLOR);
				break;
			case 39:
				if (screen_tracker == 1) {
					BG_COLOR = screen_color[screen_tracker];
					FG_COLOR = BLACK;
				}
				print_char('2', (BG_COLOR << 4) + FG_COLOR);
				break;
			case 41:
				if (screen_tracker == 2) {
					BG_COLOR = screen_color[screen_tracker];
					FG_COLOR = BLACK;
				}
				print_char('3', (BG_COLOR << 4) + FG_COLOR);
				break;
			default:
				BG_COLOR = BLACK;
				FG_COLOR = screen_color[screen_tracker];
				print_char('-', (BG_COLOR << 4) + FG_COLOR);
				break;
		}
	}

	BG_COLOR = screen_color[screen_tracker];
	FG_COLOR = BLACK;
	print_string("qwerty", (BG_COLOR << 4) + FG_COLOR);
	int i = c_cols;
	c_cols = 4;
	c_rows = 0;
	printk("%d", i);

	BG_COLOR = BLACK;
	FG_COLOR = GRAY;
	disable_scroll = false;
	c_cols = cols_tmp;
	c_rows = rows_tmp;
	move_cursor((c_rows * MAX_COLS + c_cols));
}
