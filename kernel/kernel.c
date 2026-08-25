#include "../headers/types.h"
#include "../headers/screen.h"
#include "../headers/low_level.h"
#include "../headers/idt.h"
#include "../headers/print_stack.h"
#include "../headers/signal.h"

static	void on_sigint(int signum) {
	printk("[SIGNAL RECEIVED] SIGINT (%d): Interrupted by user (CTRL+C)!", signum);
}

static	void on_sigterm(int signum) {
	printk("[SIGNAL RECEIVED] SIGTERM (%d): Terminating process (CTRL+D)!", signum);
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
