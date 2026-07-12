#include "../headers/screen.h"
#include "../headers/idt.h"

static idtr_t idtr;

__attribute__((aligned(0x10)))
static idt_entry_t idt[IDT_ENTRIES];


void	timer_handler(void) {
	PIC_sendEOI(0);
	return ;
}

static _bool vectors[IDT_MAX_DESCRIPTORS];

extern void* isr_stub_table[];


void	interrupt_handler(u32 irq_num) {

	if (irq_num == 32) {
		timer_handler();
		return ;
	}
	else if (irq_num == 33) {

		keyboard_handler();
		return ;
	}
	else
		return ;
}

static u16	get_current_code_segment(void) {
	u16	cs;
	__asm__ volatile("mov %%cs, %0" : "=r"(cs));
	return cs;
}

void	idt_set_descriptor(u8 vector, void* isr, u8 flags) {
	idt_entry_t* descriptor = &idt[vector];

	descriptor->isr_low = (u32)isr & 0xFFFF;
	descriptor->kernel_cs = get_current_code_segment();
	descriptor->reserved = 0;
	descriptor->attributes = flags;
	descriptor->isr_high = (u32)isr >> 16;
}

void idt_init(void) {
    idtr.base = (u32)&idt[0];
    idtr.limit = (u16)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for (u8 vector = 0; vector < 48; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    __asm__ volatile ("lidt %0" : : "m"(idtr));
    __asm__ volatile ("sti");
}

