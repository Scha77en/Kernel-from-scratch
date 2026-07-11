#include "../headers/types.h"
#include "../headers/screen.h"
#include "../headers/low_level.h"
#include "../headers/idt.h"

int main(void) {
	clear_screen();
	clear_buffers();
	PIC_remap(0x20, 0x28);
	idt_init();

	print_string("42", (BLACK << 4) + YELLOW);
	return 0;
}
