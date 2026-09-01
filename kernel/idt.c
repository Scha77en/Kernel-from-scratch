#include <screen.h>
#include <idt.h>

static idtr_t idtr;

__attribute__((aligned(0x10)))
static idt_entry_t idt[IDT_ENTRIES];


void	timer_handler(void) {
	PIC_sendEOI(0);
	return ;
}

static _bool vectors[IDT_MAX_DESCRIPTORS];

extern void*	isr_stub_table[];
extern void	isr_stub_128(void);


void	interrupt_handler(u32 irq_num) {

	if (irq_num == 32) {
		timer_handler();
		return ;
	}
	else if (irq_num == 33) {

		keyboard_handler();
		return ;
	}
	else if(irq_num < 32) {
		kernel_panic(irq_num);
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
		if (vector == 3 || vector == 4 || vector == 5 || vector == 128) {
			idt_set_descriptor(vector, isr_stub_table[vector], 0xEF);
			vectors[vector] = true;
			continue ;
		}
		if (vector == 6 || vector == 7 || vector == 16 || vector == 19) {
			idt_set_descriptor(vector, isr_stub_table[vector], 0x8F);
			vectors[vector] = true;
			continue ;
		}

		idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
		vectors[vector] = true;
	}

	idt_set_descriptor(128, (void *)isr_stub_128, 0xEF);
	vectors[128] = true;

	__asm__ volatile ("lidt %0" : : "m"(idtr));
	__asm__ volatile ("sti");
}

