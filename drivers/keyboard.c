#include "../headers/keyboard.h"

static u8	buff_pos = 0;
static u8	shift = 0;
static u8	caps = 0;
static u8	cntl = 0;

static const u8 scancode_to_ascii_low[87] = {0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
				'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j',
				'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static const u8 scancode_to_ascii_shift[87] = {0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
				'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J',
				'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ',
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static const u8 scancode_to_ascii_caps[87] = {0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
				'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J',
				'K', 'L', ';', '\'', '`', 0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ',
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};



static _bool	getascii(u8 code) {
	if (code > 0x57 || scancode_to_ascii_low[code] == 0) {
		//print_string("return false", (BLACK << 4) + LIGHT_RED);
		return false;
	}
	//print_string("return true", (BLACK << 4) + LIGHT_GREEN);
	return true; 
}

static void	cntl_handler(u8 scancode) {
	return ;
}

void	keyboard_handler(void) {
	FG_COLOR = CYAN;
	u8 scancode = inb(0x60); // reading byte from keyboard data port 0x60
	
	print_hex((u32)scancode);
	print_char('\n', (BG_COLOR << 4) + FG_COLOR);
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

	
		
	if (!(scancode & 0x80)) {  // Bit 7 clear = key press
		//print_hex((u32)scancode);
		//print_char(scancode_to_ascii_low[2], (BLACK << 4) + LIGHT_BLUE);
		if (getascii(scancode)) {
			if (cntl)
				cntl_handler(scancode);
			if ((!caps && !shift) || (caps && shift))
				print_char(scancode_to_ascii_low[scancode], (BG_COLOR << 4) + FG_COLOR);
			else {
				if (shift)
					print_char(scancode_to_ascii_shift[scancode], (BG_COLOR << 4) + FG_COLOR);
				else
					print_char(scancode_to_ascii_caps[scancode], (BG_COLOR << 4) + FG_COLOR);
			}
		}

    	}
	//print_char(scancode, (BLACK << 4) + PURPLE);
	//print_int((u32)scancode);	
	//keyboard_buffer[buff_pos].code = scancode;
	buff_pos = (buff_pos + 1) % MAX_KEYB_BUFFER_SIZE;
	
	PIC_sendEOI(1);
	return ;
}

