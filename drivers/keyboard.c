#include <keyboard.h>

static u8	shift = 0;
static u8	caps = 0;
static u8	cntl = 0;
_bool		cmd = false;

kbd_layout_t	current_layout = QWERTY;
u8	sc_color_kbd[3] = {YELLOW, LIGHT_RED, LIGHT_BLUE};

extern void	divide_by_zero(void);

static const u8	azerty_to_ascii_low[87] = {
	0, 0, '&', 0xE9, '"', '\'', '(', '-', 0xE8, '_', 0xE7, 0xE0, ')', '=', '\b', '\t',
	'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n', 0, 
	'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 0xF9, '<', 0, '\\', 
	'w', 'x', 'c', 'v', 'b', 'n', ',', ';', ':', '!', 0, '*', 0, ' ',
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0
};

static const u8	azerty_to_ascii_shift[87] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0xB0, '+', '\b', '\t',
	'A', 'Z', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 0xA8, '*', '\n', 0, 
	'Q', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', '%', '>', 0, '|', 
	'W', 'X', 'C', 'V', 'B', 'N', '?', '.', '/', 0xA7, 0, '*', 0, ' ',
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0
};

static const u8	azerty_to_ascii_caps[87] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
	'A', 'Z', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '^', '$', '\n', 0, 
	'Q', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', '%', '>', 0, '\\', 
	'W', 'X', 'C', 'V', 'B', 'N', ',', ';', ':', '!', 0, '*', 0, ' ',
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0
};

static void	kbd_set_layout(kbd_layout_t layout) {
	current_layout = layout;
	char	*kbd_layout = (layout == AZERTY) ? "azerty" : "qwerty";
	print_layout(kbd_layout, 1);
}

static const u8 qwerty_to_ascii_low[87] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j',
	'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0};

static const u8 qwerty_to_ascii_shift[87] = {
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J',
	'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ',
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0};

static const u8 qwerty_to_ascii_caps[87] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J',
	'K', 'L', ';', '\'', '`', 0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ',
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0};


_bool	getascii(u8 code) {
	if (code > 0x57 || qwerty_to_ascii_low[code] == 0) {
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
		case 0x48:
			kbd_set_layout(!current_layout);
			break;
		case 0x50:
			kbd_set_layout(!current_layout);
			break;
		case 0x4D:
			if (in_getline) {
				printk("You Switched the screens!!\n");
				in_getline = false;
			}
			if (++screen_tracker > 2)
				screen_tracker = 0;
			switch_screen();
			break;
		case 0x4B:
			if (in_getline) {
				printk("You Switched the screens!!\n");
				in_getline = false;
			}
			if (--screen_tracker < 0)
				screen_tracker = 2;
			switch_screen();
			break;
		case 0x2E:
			sig = true;
			schedule_signal(SIGINT);
			break;
		case 0x20:
			sig = true;
			schedule_signal(SIGTERM);
			break;
		case 0x19:
			sig = true;
			schedule_signal(SIGPANIC);
			break;
		default:
			return ;
	}
}

static char	scancode_to_ascii(u8 scancode) {
	const u8	*low   = (current_layout == AZERTY) ? azerty_to_ascii_low : qwerty_to_ascii_low;
	const u8	*shift_map = (current_layout == AZERTY) ? azerty_to_ascii_shift : qwerty_to_ascii_shift;
	const u8	*caps_map  = (current_layout == AZERTY) ? azerty_to_ascii_caps : qwerty_to_ascii_caps;

	if ((!caps && !shift) || (caps && shift))
		return low[scancode];
	if (shift)
		return shift_map[scancode];
	return caps_map[scancode];
}

void	keyboard_handler(void) {
	u8	scancode = inb(0x60);

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
			else {
				u8	c = scancode_to_ascii(scancode);
				print_char(c, (BG_COLOR << 4) + FG_COLOR);
			}
		}
    	}
	
	PIC_sendEOI(1);
	return ;
}

