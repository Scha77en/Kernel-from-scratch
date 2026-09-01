#include <types.h>
#include <screen.h>
#include <low_level.h>
#include <idt.h>
#include <print_stack.h>
#include <signal.h>
#include <panic.h>

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

void	divide_by_zero(void) {
	__asm__ volatile (
		"xor %%edx, %%edx\n\t"
		"mov $5, %%eax\n\t"
		"xor %%ecx, %%ecx\n\t"
		"div %%ecx"
		:
		:
		: "eax", "edx", "ecx"
	);
}

void	on_sigpanic(int signum) {
	divide_by_zero();
}

int main(void) {
	signal_init();
	signal_register(SIGINT, on_sigint);
	signal_register(SIGTERM, on_sigterm);
	signal_register(SIGPANIC, on_sigpanic);

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
