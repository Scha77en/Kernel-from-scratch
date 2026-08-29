#include "../headers/types.h"
#include "../headers/screen.h"
#include "../headers/low_level.h"
#include "../headers/idt.h"
#include "../headers/print_stack.h"
#include "../headers/signal.h"

u8	sc_color[3] = {YELLOW, LIGHT_RED, LIGHT_PURPLE};

static	void on_sigint(int signum) {
	if (in_getline)
		printk("^C\n");
	else
		printk("SIGINT (%d): Interrupted by user (CTRL+C)!\n", signum);
	clean_after_sig();
}

static	void on_sigterm(int signum) {
	if (in_getline)
		printk("^D\n");
	else
		printk("SIGTERM (%d): Terminating process (CTRL+D)!\n", signum);
	clean_after_sig();
}

int main(void) {
	signal_init();
	signal_register(SIGINT, on_sigint);
	signal_register(SIGTERM, on_sigterm);

	clear_screen(0);
	clear_buffers();

	PIC_remap(0x20, 0x28);
	idt_init();
	base_ebp = 0;
	asm volatile ("movl %%ebp, %0" : "=r"(base_ebp));

	while(1) {
		dispatch_scheduled_signals();
	}

	return 0;
}
