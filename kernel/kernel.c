#include "../headers/types.h"
#include "../headers/screen.h"
#include "../headers/low_level.h"
#include "../headers/idt.h"
#include "../headers/print_stack.h"

int main(void) {
	clear_screen();
	clear_buffers();
	PIC_remap(0x20, 0x28);
	idt_init();
	//print_string("wash a 3shiri hani mrta7 kolshi mzn\n", (BLACK << 4) + CYAN);

	//print_buffer_sc();
	return 0;
}
