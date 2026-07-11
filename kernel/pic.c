#include "pic.h"
#include "screen.h"

void	PIC_sendEOI(u8 irq) {
	if (irq >= 8)
		outb(PIC2_COMMAND, PIC_EOI);
	outb(PIC1_COMMAND, PIC_EOI);
}

void	PIC_remap(int offset1, int offset2) {

	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);
	io_wait();
	outb(PIC2_DATA, offset2);
	io_wait();
	outb(PIC1_DATA, 1 << CASCADE_IRQ);
	io_wait();
	outb(PIC2_DATA, 2);
	io_wait();
	
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	outb(PIC1_DATA, 0);
	outb(PIC2_DATA, 0);
}

