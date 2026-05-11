#include "../headers/keyboard.h"

key_event	keyboard_buffer[MAX_KEYB_BUFFER_SIZE];
static u8	buff_pos = 0;

static const u8 scancode_to_ascii[87] = {0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
				'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j',
				'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static bool	getascii(u8 code) {
	if (code > 0x57 || scancode_to_ascii[code] == 0) {
		//print_string("return false", (BLACK << 4) + LIGHT_RED);
		return false;
	}
	//print_string("return true", (BLACK << 4) + LIGHT_GREEN);
	return true; 
}

void	keyboard_handler(void) {
	u8 scancode = inb(0x60); // reading byte from keyboard data port 0x60
	if (!(scancode & 0x80)) {  // Bit 7 clear = key press
		//print_hex((u32)scancode);
		//print_char(scancode_to_ascii[2], (BLACK << 4) + LIGHT_BLUE);
		if (getascii(scancode))
			print_char(scancode_to_ascii[scancode], (BLACK << 4) + LIGHT_PURPLE);
    	}
	//print_char(scancode, (BLACK << 4) + PURPLE);
	//print_int((u32)scancode);	
	keyboard_buffer[buff_pos].code = scancode;
	buff_pos = (buff_pos + 1) % MAX_KEYB_BUFFER_SIZE;
	
	PIC_sendEOI(1);
	return ;
}

