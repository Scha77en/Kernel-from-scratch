#include "../headers/keyboard.h"

static u8	shift = 0;
static u8	caps = 0;
static u8	cntl = 0;
_bool	cmd = false;

static const u8 scancode_to_ascii_low[87] = {0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
				'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j',
				'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0};

static const u8 scancode_to_ascii_shift[87] = {0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
				'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J',
				'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ',
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0};

static const u8 scancode_to_ascii_caps[87] = {0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
				'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J',
				'K', 'L', ';', '\'', '`', 0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ',
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0};


_bool	getascii(u8 code) {
	if (code > 0x57 || scancode_to_ascii_low[code] == 0) {
		return false;
	}
	return true; 
}

static void	handle_arrow(u8 scancode) {
	switch (scancode) {
		case 0x48:
			arrow_up();
			break;
		case 0x50:
			arrow_down();
			break;
		case 0x4D:
			arrow_right();
			break;
		case 0x4B:
			arrow_left();
			break;
		default:
			return ;
	}
}

static void	cntl_handler(u8 scancode) {
	switch (scancode) {
		case 0x4D:
			if (++screen_tracker > 2)
				screen_tracker = 0;
			switch_screen();
			break;
		case 0x4B:
			if (--screen_tracker < 0)
				screen_tracker = 2;
			switch_screen();
			break;
		default:
			return ;
	}
}

void	keyboard_handler(void) {
	u8 scancode = inb(0x60);

	if (scancode == 0x2A || scancode == 0x36)
		shift = 1;
	else if (scancode == 0x3A)
		caps = !caps;
	else if (scancode == 0xAA || scancode == 0xB6)
		shift = 0;
	else if (scancode == 0x1D)
		cntl = 1;
	else if (scancode == 0x9D)
		cntl = 0;
	
	if (!(scancode & 0x80)) {
		if (getascii(scancode)) {
			cmd = true;
			if (!cntl && (scancode == 0x48 || scancode == 0x50 || scancode == 0x4D || scancode == 0x4B)){
			}
			else if (cntl && scancode != 0x1D && scancode != 0x9D)
				cntl_handler(scancode);
			else if ((!caps && !shift) || (caps && shift))
				print_char(scancode_to_ascii_low[scancode], (BG_COLOR << 4) + FG_COLOR);
			else {
				if (shift)
					print_char(scancode_to_ascii_shift[scancode], (BG_COLOR << 4) + FG_COLOR);
				else
					print_char(scancode_to_ascii_caps[scancode], (BG_COLOR << 4) + FG_COLOR);
			}
		}
    	}
	
	PIC_sendEOI(1);
	return ;
}

